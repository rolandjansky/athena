/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorNearestdR.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOCELLSELECTORLAYERDR_H
#define CALOCELLSELECTORLAYERDR_H

#include "RecoToolInterfaces/ICaloCellSelector.h"


namespace Trk 
{
  class CaloExtension;

  //Using dR from the nearest interpolation point as selection criteria
  //the nearest interpolation point need not be at the same layer as the cell
  //so the "missing calo cell at another layer" problem in the "classic" method
  //is avoided.
  //Slower as need to find nearest interpoaltion point for each calo cell

  class CaloCellSelectorNearestdR : public ICaloCellSelector {
  public:
    CaloCellSelectorNearestdR(double coneSize);
    ~CaloCellSelectorNearestdR ();

    bool preSelectAction( const Trk::CaloExtension& caloExtension );

    bool select( const CaloCell& cell )const; // select or reject the cell

  private:
    const Trk::CaloExtension* m_caloExtension;
    double m_coneSize2;
  };


} // end of namespace

#endif 
