/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICaloCellSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ICALOCELLSELECTOR_H
#define ICALOCELLSELECTOR_H

#include "ParticleCaloExtension/ParticleCellAssociation.h"

class CaloCell;


namespace Trk {
  
  class CaloExtension;

  /** @brief abstract class that judge if a calo cell is to be included in a collection */
  class ICaloCellSelector {
  public:
    /** virtual destructor */
    virtual ~ICaloCellSelector() {}

    /** Method to determine if a cell is to be selected or not
        @param CaloCell 
        @return bool
    */
    virtual bool select( const CaloCell& cell ) const = 0; // select or reject the cell
    
    /** Initialize selector wtih CaloExtension, returns false if the selection cannot be performed for the given input */
    virtual bool preSelectAction( const Trk::CaloExtension& caloExtension );

    /** Initialize selector wtih ParticleCellAssociation, returns false if the selection cannot be performed for the given input  */
    virtual bool preSelectActionCells( const Rec::ParticleCellAssociation& cellAssociation );

  };

  inline bool ICaloCellSelector::preSelectAction( const Trk::CaloExtension& ) {
    return true;
  }

  inline bool ICaloCellSelector::preSelectActionCells( const Rec::ParticleCellAssociation& cellAssociation ) {
    return preSelectAction(cellAssociation.caloExtension());
  }

} // end of namespace

#endif 
