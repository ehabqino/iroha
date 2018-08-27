/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_QUERY_EXECUTOR_FACTORY_HPP
#define IROHA_QUERY_EXECUTOR_FACTORY_HPP

#include <boost/optional.hpp>

#include "ametsuchi/query_executor.hpp"

namespace iroha {
  namespace ametsuchi {
    class QueryExecutorFactory {
     public:
      /**
       * Creates a query executor from the current state.
       * @return Created query executor
       */
      virtual boost::optional<std::shared_ptr<QueryExecutor>>
      createQueryExecutor() const = 0;

      virtual ~QueryExecutorFactory() = default;
    };
  }  // namespace ametsuchi
}  // namespace iroha
#endif  // IROHA_QUERY_EXECUTOR_FACTORY_HPP
