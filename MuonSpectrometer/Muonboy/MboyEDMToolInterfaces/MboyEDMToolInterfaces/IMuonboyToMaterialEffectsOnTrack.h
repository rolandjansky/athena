/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToMaterialEffectsOnTrack_H
#define IMuonboyToMaterialEffectsOnTrack_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class MaterialEffectsOnTrack;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MbPOT;

static const InterfaceID IID_IMuonboyToMaterialEffectsOnTrack("IMuonboyToMaterialEffectsOnTrack", 1, 0);

  /**
   @class IMuonboyToMaterialEffectsOnTrack

   This class provides TrackParameters and  MaterialEffectsOnTrack for a MbPOT 

  @author samusog@cern.ch
  
  */

class IMuonboyToMaterialEffectsOnTrack:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Get TrackParameters and  MaterialEffectsOnTrack for a MbPOT **/
   virtual std::pair<const Trk::TrackParameters*, const Trk::MaterialEffectsOnTrack*> GetTrackParamAndMatEffect(
                             const MbPOT* pMbPOT
                            ) = 0 ;

};

inline const InterfaceID& IMuonboyToMaterialEffectsOnTrack::interfaceID() { return IID_IMuonboyToMaterialEffectsOnTrack;}

#endif

