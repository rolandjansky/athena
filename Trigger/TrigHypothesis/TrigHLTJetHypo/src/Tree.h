/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TREE_H
#define TRIGHLTJETHYPO_TREE_H

/*
 * The input vector of unsigned ints is interpreted as a tree structure of
 * conditions objects. The elements oif the input vector refer to the position
 * of the Conditions objects is some sequence.
 * 
 * With this structure nested conditions are supported.
 */
#include <vector>
#include <ostream>

class Tree{
 public:
  Tree(const std::vector<std::size_t>&);

  std::size_t parent(std::size_t) const;
  std::vector<std::size_t> siblings(std::size_t) const;
  std::size_t size() const;
  const std::vector<std::size_t>& leaves() const;
  const std::vector<std::size_t>& firstGeneration() const;

  std::vector<std::size_t>::const_iterator cbegin() const {
    return m_parents.cbegin();
  }


  std::vector<std::size_t>::const_iterator cend() const {
    return m_parents.cend();
  }



  std::size_t depth(std::size_t) const;

 private:
  std::vector<std::size_t> m_parents;
  std::vector<std::vector<std::size_t>> m_children;
  std::vector<std::size_t> m_leaves;
  std::vector<std::size_t> m_firstGeneration;
};

std::ostream& operator<< (std::ostream&, const Tree&);
#endif
