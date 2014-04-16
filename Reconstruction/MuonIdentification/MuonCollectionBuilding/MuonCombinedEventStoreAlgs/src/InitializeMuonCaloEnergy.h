/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_INITIALIZEMUONCALOENERGY_H
#define REC_INITIALIZEMUONCALOENERGY_H
/*****************************************************************************
Name    : InitialzeMuonCaloEnergy.h
Package : Reconstruction/MuonIdentification/MuonCollectionBuilding/MuonCombinedEventStoreAlgs
Author  : Ketevi A. Assamagan
Created : August 2007
Note    : 

DESCRIPTION:

	- Initalization of Muon CaloEnergy Making: open a storegate container.
	- called before any combined muon algorithm runs

*****************************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

namespace Rec {
  class InitializeMuonCaloEnergy : public AthAlgorithm {
  public:

    InitializeMuonCaloEnergy(const std::string& name, ISvcLocator* pSvcLocator);
    ~InitializeMuonCaloEnergy();

    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();

  private:

    std::string m_muonCaloEnergyContainerName;

  };
}

#endif // REC_INITIALIZEMUONCALOENERGY_H
