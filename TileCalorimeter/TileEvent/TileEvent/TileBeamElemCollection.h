/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileBeamElemCollection_H       
#define TileBeamElemCollection_H       
                                         
#include "TileEvent/TileBeamElem.h" 
#include "TileEvent/TileRawDataCollection.h"
                                         
class TileBeamElemCollection : public TileRawDataCollection<TileBeamElem>
{
  friend class TileBeamElemContainerCnv;
public:

  typedef TileRawDataCollection<TileBeamElem> MyBase;
  typedef MyBase::ID ID;
  typedef TileBeamElem TElement;

  TileBeamElemCollection (  ID id, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS  ) 
    : MyBase(id,ownPolicy) { }
  TileBeamElemCollection ( SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS )
    : MyBase(ownPolicy) { }

  /**
   * @brief Copy constructor.
   * @param other Collection to copy.
   * Does a deep copy of the contents.
   */
  TileBeamElemCollection (const TileBeamElemCollection& other);

  /**
   * @brief Move constructor.
   * @param other Collection to move.
   * Move the contents from other collection.
   */
  TileBeamElemCollection(TileBeamElemCollection&& other) = default;


  ~TileBeamElemCollection() { }
};

CLASS_DEF(TileBeamElemCollection,2936,0)
           
#endif

