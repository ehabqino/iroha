/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_POSTGRES_QUERY_EXECUTOR_HPP
#define IROHA_POSTGRES_QUERY_EXECUTOR_HPP

#include "ametsuchi/query_executor.hpp"

#include "ametsuchi/impl/soci_utils.hpp"
#include "ametsuchi/storage.hpp"
#include "builders/protobuf/builder_templates/query_response_template.hpp"
#include "interfaces/commands/add_asset_quantity.hpp"
#include "interfaces/commands/add_peer.hpp"
#include "interfaces/commands/add_signatory.hpp"
#include "interfaces/commands/append_role.hpp"
#include "interfaces/commands/create_account.hpp"
#include "interfaces/commands/create_asset.hpp"
#include "interfaces/commands/create_domain.hpp"
#include "interfaces/commands/create_role.hpp"
#include "interfaces/commands/detach_role.hpp"
#include "interfaces/commands/grant_permission.hpp"
#include "interfaces/commands/remove_signatory.hpp"
#include "interfaces/commands/revoke_permission.hpp"
#include "interfaces/commands/set_account_detail.hpp"
#include "interfaces/commands/set_quorum.hpp"
#include "interfaces/commands/subtract_asset_quantity.hpp"
#include "interfaces/commands/transfer_asset.hpp"
#include "interfaces/common_objects/common_objects_factory.hpp"
#include "interfaces/queries/blocks_query.hpp"
#include "interfaces/queries/query.hpp"
#include "interfaces/query_responses/query_response.hpp"

namespace iroha {
  namespace ametsuchi {

    using QueryResponseBuilder =
        shared_model::proto::TemplateQueryResponseBuilder<0>;

    using QueryResponseBuilderDone =
        shared_model::proto::TemplateQueryResponseBuilder<1>;
    class PostgresQueryExecutorVisitor
        : public boost::static_visitor<QueryResponseBuilderDone> {
     public:
      PostgresQueryExecutorVisitor(
          soci::session &sql,
          std::shared_ptr<shared_model::interface::CommonObjectsFactory>
              factory);

      void setCreatorId(
          const shared_model::interface::types::AccountIdType &creator_id);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetAccount &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetSignatories &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetAccountTransactions &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetTransactions &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetAccountAssetTransactions &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetAccountAssets &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetAccountDetail &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetRoles &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetRolePermissions &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetAssetInfo &q);

      QueryResponseBuilderDone operator()(
          const shared_model::interface::GetPendingTransactions &q);

     private:
      soci::session &sql_;
      shared_model::interface::types::AccountIdType creator_id_;
      std::shared_ptr<shared_model::interface::CommonObjectsFactory> factory_;
    };

    class PostgresQueryExecutor : public QueryExecutor {
     public:
      explicit PostgresQueryExecutor(
          std::shared_ptr<ametsuchi::Storage> storage,
          soci::session &sql,
          std::shared_ptr<shared_model::interface::CommonObjectsFactory>
              factory);

      QueryExecutorResult validateAndExecute(
          const shared_model::interface::Query &query) override;

      bool validate(const shared_model::interface::BlocksQuery &query) override;

     private:
      std::shared_ptr<ametsuchi::Storage> storage_;
      soci::session &sql_;
      std::shared_ptr<shared_model::interface::CommonObjectsFactory> factory_;
      PostgresQueryExecutorVisitor visitor_;
    };

  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_POSTGRES_QUERY_EXECUTOR_HPP
