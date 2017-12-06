// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file TileEvent/TileRawDataNonConstContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2017
 * @brief Helper class for building a TileRawDataContainer.
 */


#ifndef TILEEVENT_TILERAWDATANONCONSTCONTAINER_H
#define TILEEVENT_TILERAWDATANONCONSTCONTAINER_H


#include "TileIdentifier/TileFragHash.h"
#include "TileConditions/TileCablingService.h"
#include <vector>


/**
 * @brief This is a minimal version of a @c TileRawDataContainer in which
 *        the saved collections remain non-const.
 *
 * This can be used as a temporary while building the container.
 * Once everything's built, the collections can then be moved to the
 * final @c TileRawDataContainer, after which they'll be const.
 */
template <typename COLLECTION>
class TileRawDataNonConstContainer
{
public:
  typedef typename COLLECTION::_TElement element_t;

  TileRawDataNonConstContainer (SG::OwnershipPolicy ownPolicy);
  void push_back (element_t* rc);

  typedef typename std::vector<std::unique_ptr<COLLECTION> >::iterator iterator;
  iterator begin() { return m_colls.begin(); }
  iterator end() { return m_colls.end(); }

  
private:
  TileFragHash m_hashFunc;
  std::vector<std::unique_ptr<COLLECTION> > m_colls;
};


template <typename COLLECTION>
TileRawDataNonConstContainer<COLLECTION>::TileRawDataNonConstContainer (SG::OwnershipPolicy ownPolicy)
{
  m_hashFunc.initialize(TileCablingService::getInstance()->getTileHWID(),
                        TileFragHash::Digitizer);
  size_t ncoll = m_hashFunc.max();
  m_colls.reserve (ncoll);
  for (size_t i = 0; i < ncoll; i++) {
    TileFragHash::ID frag = m_hashFunc.identifier(i);
    m_colls.push_back (std::make_unique<COLLECTION> (frag, ownPolicy));
  }
}


template <typename COLLECTION>
void TileRawDataNonConstContainer<COLLECTION>::push_back (element_t* rc)
{
  int frag = rc->frag_ID();
  IdentifierHash fragHash = static_cast<IdentifierHash>(m_hashFunc(frag));
  assert (fragHash < m_colls.size());
  m_colls[fragHash]->push_back (rc);
}


#endif // not TILEEVENT_TILERAWDATANONCONSTCONTAINER_H
