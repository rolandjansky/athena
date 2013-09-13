/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileTrackMuFeatureContainer_p1
 *
 * @brief persistent partner for TrigMuonEFContainer
 *
 * @author Heuijin Lim (hjlim@hep.anl.gov)
 * @Created: Jul. 7. 2008
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TileTrackMuFeatureContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TileTrackMuFeatureContainer_p1(){}
  friend class TileTrackMuFeatureContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_TileTrackMuFeature;

};// end of class definitions
 
 
#endif
