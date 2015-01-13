/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorMinPerp.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOCELLSELECTORLAYERDR_H
#define CALOCELLSELECTORLAYERDR_H

#include "RecoToolInterfaces/ICaloCellSelector.h"
#include "GeoPrimitives/GeoPrimitives.h"


namespace Trk 
{

  //Using min perperdicular distance from the nearest interpolated track segment as selection criteria
  //the nearest interpolation point need not be at the same layer as the cell
  //so the "missing calo cell at another layer" problem in the "classic" method
  //is avoided.
  //Slower as need to find nearest interpoaltion point for each calo cell

  class CaloCellSelectorMinPerp : public ICaloCellSelector {
  public:
    CaloCellSelectorMinPerp(double coneSize);
    ~CaloCellSelectorMinPerp () = default;

    bool preSelectAction( const Trk::CaloExtension& caloExtension );

    bool select( const CaloCell& cell )const; // select or reject the cell

  private:
    const Trk::CaloExtension* m_caloExtension;
    double m_coneSize;
    Amg::Vector3D m_meanPos;
    double m_perp2cut;
  };
} // end of namespace

#endif 
