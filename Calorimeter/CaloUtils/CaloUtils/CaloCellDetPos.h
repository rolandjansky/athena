/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class CaloCellDetPos
{

 public:

  /**
   * @brief constructor of CaloCellDetPos
   */
  CaloCellDetPos();

  /**
   * @brief destructor of CaloCellDetPos
   */
  ~CaloCellDetPos();

  /**
   * @brief  get Detector level eta-phi position from Atlas level (aligned) position
   * @param  CaloCell_ID::CaloSample sam  layer to use
   * @param  double etaAtlas : input eta Atlas level position
   * @param  double phiAtlas : input phi Atlas level position
   * @param  double etaDet   : estimated eta detector in nominal calo frame
   * @param  double phiDet   : estimated phi detector in nominal calo frame
   */
  bool getDetPosition(CaloCell_ID::CaloSample sam , double etaAtlas, double phiAtlas, double & etaDet, double & phiDet);


  /**
   * @brief  get Atlas (aligned) level eta-phi position from detector level eta-phi positions
   * @param  CaloCell_ID::CaloSample sam  layer to use
   * @param  double etaDet   : input eta detector level position
   * @param  double phiDet   : input phi detector level position
   * @param  double etaAtlas : estimated eta detector in aligned Atlas frame
   * @param  double phiAtlas : estimated phi detector in aligned Atlas frame
   */

  bool getAtlasPosition(CaloCell_ID::CaloSample sam , double etaDet, double phiDet, double & etaAtlas, double & phiAtlas);

  private:

   static CaloPhiRange s_range;

};
#endif
