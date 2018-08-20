/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ordering/impl/on_demand_os_client_grpc.hpp"

#include "backend/protobuf/proposal.hpp"
#include "network/impl/grpc_channel_builder.hpp"

using namespace iroha::ordering::transport;

OnDemandOsClientGrpc::OnDemandOsClientGrpc(
    std::unique_ptr<proto::OnDemandOrdering::StubInterface> stub,
    std::shared_ptr<network::AsyncGrpcClient<google::protobuf::Empty>>
        async_call,
    std::function<TimepointType()> time_provider,
    std::chrono::milliseconds proposal_request_timeout)
    : log_(logger::log("OnDemandOsClientGrpc")),
      stub_(std::move(stub)),
      async_call_(std::move(async_call)),
      time_provider_(std::move(time_provider)),
      proposal_request_timeout_(proposal_request_timeout) {}

void OnDemandOsClientGrpc::onTransactions(CollectionType transactions) {
  proto::TransactionsCollection message;
  for (auto &transaction : transactions) {
    *message.add_transactions() = std::move(
        static_cast<shared_model::proto::Transaction *>(transaction.get())
            ->getTransport());
  }

  log_->debug("Propagating: '{}'", message.DebugString());

  async_call_->Call([&](auto context, auto cq) {
    return stub_->AsyncSendTransactions(context, message, cq);
  });
}

boost::optional<OdOsNotification::ProposalType>
OnDemandOsClientGrpc::onRequestProposal(transport::RoundType round) {
  grpc::ClientContext context;
  context.set_deadline(time_provider_() + proposal_request_timeout_);
  proto::ProposalRequest request;
  request.set_block_round(round.first);
  request.set_reject_round(round.second);
  proto::ProposalResponse response;
  auto status = stub_->RequestProposal(&context, request, &response);
  if (not status.ok()) {
    log_->warn("RPC failed: {}", status.error_message());
    return boost::none;
  }
  if (not response.has_proposal()) {
    return boost::none;
  }
  return ProposalType{std::make_unique<shared_model::proto::Proposal>(
      std::move(response.proposal()))};
}

OnDemandOsClientGrpcFactory::OnDemandOsClientGrpcFactory(
    std::shared_ptr<network::AsyncGrpcClient<google::protobuf::Empty>>
        async_call,
    std::function<OnDemandOsClientGrpc::TimepointType()> time_provider,
    OnDemandOsClientGrpc::TimeoutType proposal_request_timeout)
    : async_call_(std::move(async_call)),
      time_provider_(time_provider),
      proposal_request_timeout_(proposal_request_timeout) {}

std::unique_ptr<OdOsNotification> OnDemandOsClientGrpcFactory::create(
    const shared_model::interface::Peer &to) {
  return std::make_unique<OnDemandOsClientGrpc>(
      network::createClient<proto::OnDemandOrdering>(to.address()),
      async_call_,
      time_provider_,
      proposal_request_timeout_);
}