#pragma once

#include "envoy/buffer/buffer.h"
#include "envoy/http/header_map.h"
#include "envoy/service/ext_proc/v3/external_processor.pb.h"
#include "envoy/stats/stats.h"

#include "source/common/common/logger.h"
#include "source/extensions/filters/common/mutation_rules/mutation_rules.h"

#include "absl/status/status.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace ExternalProcessing {

class MutationUtils : public Logger::Loggable<Logger::Id::ext_proc> {
public:
  // Convert a header map into a protobuf
  static void headersToProto(const Http::HeaderMap& headers_in,
                             const std::vector<Matchers::StringMatcherPtr>& header_matchers,
                             envoy::config::core::v3::HeaderMap& proto_out);

  // Modify header map based on a set of mutations from a protobuf. An error will be
  // returned if any mutations are not allowed and if the filter has been
  // configured to reject failed mutations. The "rejected_mutations" counter
  // will be incremented with the number of invalid mutations, regardless of
  // whether an error is returned.
  static absl::Status
  applyHeaderMutations(const envoy::service::ext_proc::v3::HeaderMutation& mutation,
                       Http::HeaderMap& headers, bool replacing_message,
                       const Filters::Common::MutationRules::Checker& rule_checker,
                       Stats::Counter& rejected_mutations);

  // Modify a buffer based on a set of mutations from a protobuf
  static void applyBodyMutations(const envoy::service::ext_proc::v3::BodyMutation& mutation,
                                 Buffer::Instance& buffer);

  // Determine if a particular HTTP status code is valid.
  static bool isValidHttpStatus(int code);

private:
  // Check whether header:key is in header_matchers.
  static bool headerInAllowList(absl::string_view key,
                                const std::vector<Matchers::StringMatcherPtr>& header_matchers);
  // Check whether the header mutations in the response is over the HCM size config.
  static absl::Status
  responseHeaderSizeCheck(const Http::HeaderMap& headers,
                          const envoy::service::ext_proc::v3::HeaderMutation& mutation,
                          Stats::Counter& rejected_mutations);
  // Check whether the header size after mutation is over the HCM size config.
  static absl::Status headerMutationResultCheck(const Http::HeaderMap& headers,
                                                Stats::Counter& rejected_mutations);
};

} // namespace ExternalProcessing
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
