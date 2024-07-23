/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <mariana-trench/AbstractTreeDomain.h>
#include <mariana-trench/AccessPathTreeDomain.h>
#include <mariana-trench/IncludeMacros.h>
#include <mariana-trench/Taint.h>
#include <mariana-trench/TaintTree.h>

namespace marianatrench {

class TaintAccessPathTree final
    : public sparta::AbstractDomain<TaintAccessPathTree> {
 private:
  // We wrap `AccessPathTreeDomain<Taint, TaintTreeConfiguration>`
  // in order to properly update the collapse depth when collapsing.

  using Tree = AccessPathTreeDomain<Taint, TaintTreeConfiguration>;

  explicit TaintAccessPathTree(Tree tree) : tree_(std::move(tree)) {}

 public:
  /* Create the bottom (empty) taint access path tree. */
  TaintAccessPathTree() : tree_(Tree::bottom()) {}

  explicit TaintAccessPathTree(
      std::initializer_list<std::pair<AccessPath, Taint>> edges)
      : tree_(std::move(edges)) {}

  INCLUDE_DEFAULT_COPY_CONSTRUCTORS_AND_ASSIGNMENTS(TaintAccessPathTree)

  INCLUDE_ABSTRACT_DOMAIN_METHODS(TaintAccessPathTree, Tree, tree_)

  TaintTree read(Root root) const;

  template <typename Propagate> // Taint(Taint, Path::Element)
  TaintTree read(const AccessPath& access_path, Propagate&& propagate) const {
    return TaintTree(
        tree_.read(access_path, std::forward<Propagate>(propagate)));
  }

  TaintTree read(const AccessPath& access_path) const;

  TaintTree raw_read(const AccessPath& access_path) const;

  /* Write taint at the given access path. */
  void write(const AccessPath& access_path, Taint taint, UpdateKind kind);

  /* Write a taint tree at the given access path. */
  void write(const AccessPath& access_path, TaintTree tree, UpdateKind kind);

  /**
   * Iterate on all non-empty taint in the tree.
   *
   * When visiting the tree, taint do not include their ancestors.
   */
  template <typename Visitor> // void(const AccessPath&, const Taint&)
  void visit(Visitor&& visitor) const {
    return tree_.visit(std::forward<Visitor>(visitor));
  }

  /* Return the list of pairs (access path, taint) in the tree. */
  std::vector<std::pair<AccessPath, const Taint&>> elements() const;

  /* Return the list of pairs (root, taint tree) in the tree. */
  std::vector<std::pair<Root, TaintTree>> roots() const;

  /* Apply the given function on all taint. */
  template <typename Function> // Taint(Taint)
  void transform(Function&& f) {
    tree_.transform(std::forward<Function>(f));
  }

  /* Collapse children that have more than `max_leaves` leaves. */
  void limit_leaves(
      std::size_t max_leaves,
      const FeatureMayAlwaysSet& broadening_features);

  /**
   * When a path is invalid, collapse its taint into its parent's.
   * See AbstractTreeDomain::collapse_invalid_paths.
   */
  template <typename Accumulator>
  void collapse_invalid_paths(
      const std::function<
          std::pair<bool, Accumulator>(const Accumulator&, Path::Element)>&
          is_valid,
      const std::function<Accumulator(const Root&)>& initial_accumulator,
      const FeatureMayAlwaysSet& broadening_features) {
    tree_.collapse_invalid_paths(
        is_valid, initial_accumulator, [&broadening_features](Taint taint) {
          taint.add_locally_inferred_features(broadening_features);
          taint.update_maximum_collapse_depth(CollapseDepth::zero());
          return taint;
        });
  }

  /**
   * Transforms the tree to shape it according to a mold.
   *
   * `make_mold` is a function applied on taint to create a mold tree.
   *
   * This is used to prune the taint tree of duplicate taint, for
   * better performance at the cost of precision. `make_mold` creates a new
   * taint without any non-essential information (i.e, removing features). Since
   * the tree domain automatically removes taint on children if it is present
   * at the root (closure), this will collapse unnecessary branches.
   * `AbstractTreeDomain::shape_with` will then collapse branches in the
   * original taint tree if it was collapsed in the mold.
   */
  template <typename MakeMold>
  void shape_with(
      MakeMold&& make_mold, // Taint(Taint)
      const FeatureMayAlwaysSet& broadening_features) {
    tree_.shape_with(
        std::forward<MakeMold>(make_mold), [&broadening_features](Taint taint) {
          taint.add_locally_inferred_features(broadening_features);
          taint.update_maximum_collapse_depth(CollapseDepth::zero());
          return taint;
        });
  }

  friend std::ostream& operator<<(
      std::ostream& out,
      const TaintAccessPathTree& tree) {
    return out << "TaintAccessPathTree" << tree.tree_;
  }

 private:
  Tree tree_;
};

} // namespace marianatrench
