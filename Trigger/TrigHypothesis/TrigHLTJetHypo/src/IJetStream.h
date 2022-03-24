/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IJETSTREAM_H
#define TRIGHLTJETHYPO_IJETSTREAM_H

/*
 * PABC Interface to trigger jet hypo JetStream classes
 * In this context, jets are represented by integer indices.
 * a jet stream steps through the available indices in a manner
 * that is implemnented in the concrete classes. The selected
 * indices are returned in a vector.
 *
 * Checks on the validity of the stream can be made after construction
 *
 * Concrete implementations  a pointer to a right neighbor
 * of typeIJetStream object, and so can be an element  of a linked list.
 * 
 * Cycling is used by the left neighbor to decide whether the left
 * neigbor should be bumped.
 * 
 * When the state of all elements of the list is cycled, the caller is
 * informed, and will normally stop the iteration,
 *
 */

#include <vector>
#include <string>

class IJetStream {
 public:
  virtual ~IJetStream(){}
  virtual std::vector<std::size_t> get() = 0;
  virtual bool valid() const = 0;
  virtual bool bump() = 0;
  virtual std::string dump() const= 0;
};

#endif
