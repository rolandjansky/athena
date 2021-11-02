/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloCellDetPos_H
#define CaloCellDetPos_H

/*  
    Helper method
    Getting detector level (raw) eta-phi position from Atlas level (aligned) eta-phi positions (wrt to 0,0,0)
    for a given calorimeter layer

    December 2010  G.Unal

*/

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
class CaloCellDetPos
{

 public:

  /**
   * @brief  get Detector level eta-phi position from Atlas level (aligned) position
   * @param  const CaloDetDescrManager mgr CaloDetDescManager Instance
   * @param  CaloCell_ID::CaloSample sam  layer to use
   * @param  double etaAtlas : input eta Atlas level position
   * @param  double phiAtlas : input phi Atlas level position
   * @param  double etaDet   : estimated eta detector in nominal calo frame
   * @param  double phiDet   : estimated phi detector in nominal calo frame
   */
  static bool getDetPosition(const CaloDetDescrManager& mgr,
                      CaloCell_ID::CaloSample sam, double etaAtlas,
                      double phiAtlas, double& etaDet, double& phiDet) ;

  /**
   * @brief  get Atlas (aligned) level eta-phi position from detector level eta-phi positions
   * @param  const CaloDetDescrManager mgr CaloDetDescManager Instance
   * @param  CaloCell_ID::CaloSample sam  layer to use
   * @param  double etaDet   : input eta detector level position
   * @param  double phiDet   : input phi detector level position
   * @param  double etaAtlas : estimated eta detector in aligned Atlas frame
   * @param  double phiAtlas : estimated phi detector in aligned Atlas frame
   */

  static bool getAtlasPosition(const CaloDetDescrManager& mgr,
                        CaloCell_ID::CaloSample sam, double etaDet,
                        double phiDet, double& etaAtlas,
                        double& phiAtlas) ;


};
#endif
