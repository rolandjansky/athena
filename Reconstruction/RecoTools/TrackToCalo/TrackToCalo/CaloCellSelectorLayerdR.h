/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorLayerdR.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOCELLSELECTORLAYERDR_H
#define CALOCELLSELECTORLAYERDR_H

#include "RecoToolInterfaces/ICaloCellSelector.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace Trk 
{

  //Recode of original method by TrackInCaloTools to select calo cells around track
  //get mid point for each calo layer from the entry & exit point
  //for each layer's mid point select calo cell within dR in the SAME layer
  //Fast (?) but
  //1. may miss calo cell at another layer which nevertheless is close to the crossing
  //2. current implementation will fail in case where there's multiple entry to a calo layer

  class CaloCellSelectorLayerdR : public ICaloCellSelector {
  public:
    CaloCellSelectorLayerdR(double coneSize);
    ~CaloCellSelectorLayerdR ();

    void setConeSize( double coneSize ) { m_coneSize2 = coneSize*coneSize; }

    bool preSelectAction( const Trk::CaloExtension& caloExtension );
    bool preSelectAction( const xAOD::CaloCluster& caloCluster );
    bool select( const CaloCell& cell ) const; // select or reject the cell

  private:
    CaloExtensionHelpers::EtaPhiHashLookupVector m_midPoints;
    double m_coneSize2;
    //TrackParametersIdHelper  parsIdHelper;
  };
} // end of namespace

#endif 
