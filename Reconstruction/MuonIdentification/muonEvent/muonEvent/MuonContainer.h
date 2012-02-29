/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENT_MUONCONTAINER_H
#define MUONEVENT_MUONCONTAINER_H

//////////////////////////////////////////////////////////////////////////////////////
///
/// Name:     MuonContainer.h
/// Package : offline/Reconstruction/MuonIdentification/muonEvent
///
/// Authors:  K. A. Assamagan
/// Created:  December 2004
///
/// Purpose:  This is a data object, containing a collection of Muon Objects
/////////////////////////////////////////////////////////////////////////////////////

// INCLUDE HEADER FILES:

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "muonEvent/Muon.h"

#include "NavFourMom/IParticleContainer.h"
// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<Analysis::Muon> derives from DV<IParticle>
DATAVECTOR_BASE( Analysis::Muon, IParticle );

namespace Analysis {

  /** @class Analysis::MuonContainer
      @brief definition of StoreGate container holding a vector of Analysis::Muon
      @author K. A. Assamagan
  */

class MuonContainer : public DataVector<Analysis::Muon> 
{
 public:
  MuonContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
	  : DataVector<Analysis::Muon>(own) {}
  virtual ~MuonContainer() {};
  void print();

};

}

CLASS_DEF(Analysis::MuonContainer, 1073853171, 1)
SG_BASE(Analysis::MuonContainer, DataVector<Analysis::Muon> ); // < needed for auto-symlink feature

#endif





