/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorRoughdR.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOCELLSELECTORLAYERDR_H
#define CALOCELLSELECTORLAYERDR_H

#include "RecoToolInterfaces/ICaloCellSelector.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

#include "TrkCaloExtension/CaloExtensionHelpers.h"

namespace Trk 
{
  //Rough method to select calo cells around track
  //mean eta phi was found for all the calorimeter layer crossing points
  //cells within dR+tolerance around the mean are selected
  //tolerance is the max deviation of individual cross points from the mean
  //Fast and easy, but not very valid when the track is highly curved

  class CaloCellSelectorRoughdR : public ICaloCellSelector {
  public:
    CaloCellSelectorRoughdR(double coneSize);
    ~CaloCellSelectorRoughdR ();

    bool preSelectAction( const Trk::CaloExtension& caloExtension );

    bool select( const CaloCell& cell )const; // select or reject the cell

  private:
    double m_coneSize;
    double m_midEta;
    double m_midPhi;
    double m_maxDiff;

    //TrackParametersIdHelper  parsIdHelper;
    CaloExtensionHelpers::EntryExitPerLayerVector m_crossPoints;
  };

} // end of namespace

#endif 
