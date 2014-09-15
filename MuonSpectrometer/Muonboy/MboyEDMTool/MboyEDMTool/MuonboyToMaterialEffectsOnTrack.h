/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToMaterialEffectsOnTrack_H
#define MuonboyToMaterialEffectsOnTrack_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class MaterialEffectsOnTrack;
  class IEnergyLossUpdator;
  class IMultipleScatteringUpdator;
}

#include "MboyEDMToolInterfaces/IMuonboyToMaterialEffectsOnTrack.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class MbPOT;

class IMuonboyToLayer;
class MbMuSc;

  /**
   @class MuonboyToMaterialEffectsOnTrack

   This class provides TrackParameters and  MaterialEffectsOnTrack for a MbPOT 

  @author samusog@cern.ch
  
  */

class MuonboyToMaterialEffectsOnTrack: public AthAlgTool, virtual public IMuonboyToMaterialEffectsOnTrack{
public:
    MuonboyToMaterialEffectsOnTrack(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToMaterialEffectsOnTrack 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Get TrackParameters and  MaterialEffectsOnTrack for a MbPOT **/
   virtual std::pair<const Trk::TrackParameters*, const Trk::MaterialEffectsOnTrack*> GetTrackParamAndMatEffect(
                             const MbPOT* pMbPOT
                            );

   ToolHandle< Trk::IEnergyLossUpdator > p_IEnergyLossUpdator;
   
   ToolHandle< Trk::IMultipleScatteringUpdator > p_IMultipleScatteringUpdator ; //!< Pointer On IMultipleScatteringUpdator

   ToolHandle< IMuonboyToLayer > p_IMuonboyToLayer ; //!< Pointer On IMuonboyToLayer

};

#endif

