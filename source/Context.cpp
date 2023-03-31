/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <mariana-trench/ArtificialMethods.h>
#include <mariana-trench/CallGraph.h>
#include <mariana-trench/ClassHierarchies.h>
#include <mariana-trench/ClassProperties.h>
#include <mariana-trench/Context.h>
#include <mariana-trench/ControlFlowGraphs.h>
#include <mariana-trench/Dependencies.h>
#include <mariana-trench/Features.h>
#include <mariana-trench/FieldCache.h>
#include <mariana-trench/Fields.h>
#include <mariana-trench/Kinds.h>
#include <mariana-trench/Methods.h>
#include <mariana-trench/Options.h>
#include <mariana-trench/Overrides.h>
#include <mariana-trench/Positions.h>
#include <mariana-trench/Rules.h>
#include <mariana-trench/Scheduler.h>
#include <mariana-trench/Statistics.h>
#include <mariana-trench/Transforms.h>
#include <mariana-trench/Types.h>
#include <mariana-trench/UsedKinds.h>

namespace marianatrench {

Context::Context()
    : kinds(std::make_unique<Kinds>()),
      features(std::make_unique<Features>()),
      statistics(std::make_unique<Statistics>()),
      transforms(std::make_unique<Transforms>()) {}

Context::Context(Context&&) noexcept = default;

Context::~Context() = default;

} // namespace marianatrench
