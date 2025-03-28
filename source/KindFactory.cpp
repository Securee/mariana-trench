/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <mariana-trench/Assert.h>
#include <mariana-trench/KindFactory.h>
#include <mariana-trench/LocalArgumentKind.h>
#include <mariana-trench/LocalReturnKind.h>
#include <mariana-trench/NamedKind.h>
#include <mariana-trench/PartialKind.h>
#include <mariana-trench/TriggeredPartialKind.h>

namespace marianatrench {

KindFactory::KindFactory()
    : local_return_(std::make_unique<LocalReturnKind>()) {
  local_receiver_ = local_argument_.create(0);
}

const NamedKind* KindFactory::get(const std::string& name) const {
  return named_.create(name);
}

const PartialKind* KindFactory::get_partial(
    const std::string& name,
    const std::string& label) const {
  return partial_.create(std::make_tuple(name, label), name, label);
}

const TriggeredPartialKind* KindFactory::get_triggered(
    const PartialKind* partial,
    int rule_code) const {
  return triggered_partial_.create(
      std::make_tuple(partial, rule_code), partial, rule_code);
}

const LocalReturnKind* KindFactory::local_return() const {
  return local_return_.get();
}

const LocalArgumentKind* KindFactory::local_receiver() const {
  return local_receiver_;
}

const LocalArgumentKind* KindFactory::local_argument(
    ParameterPosition parameter) const {
  return local_argument_.create(parameter);
}

const TransformKind* KindFactory::transform_kind(
    const Kind* base_kind,
    const TransformList* MT_NULLABLE local_transforms,
    const TransformList* MT_NULLABLE global_transforms) const {
  mt_assert(base_kind != nullptr);
  mt_assert(base_kind->as<TransformKind>() == nullptr);
  mt_assert(local_transforms != nullptr || global_transforms != nullptr);

  return transforms_.create(
      std::make_tuple(base_kind, local_transforms, global_transforms),
      base_kind,
      local_transforms,
      global_transforms);
}

std::vector<const Kind*> KindFactory::kinds() const {
  std::vector<const Kind*> result;
  result.push_back(local_return_.get());
  result.push_back(local_receiver_);
  for (const auto& [_key, kind] : named_) {
    result.push_back(kind);
  }
  for (const auto& [_key, kind] : partial_) {
    result.push_back(kind);
  }
  for (const auto& [_key, kind] : triggered_partial_) {
    result.push_back(kind);
  }

  return result;
}

const KindFactory& KindFactory::singleton() {
  // Thread-safe global variable, initialized on first call.
  static KindFactory instance;
  return instance;
}

} // namespace marianatrench
