/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbMuScToTSOSTool_H
#define MbMuScToTSOSTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMbMuScToTSOSTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class MbPOT;
class IMuonboyToMaterialEffectsOnTrack ;


namespace Trk {

  /**
   @class MbMuScToTSOSTool

   This tools converts Muonboy tracks into Trk::Track
   
  @author samusog@cern.ch
  
  */

class MbMuScToTSOSTool: public AthAlgTool, virtual public IMbMuScToTSOSTool{
public:
    MbMuScToTSOSTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MbMuScToTSOSTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   virtual const Trk::TrackStateOnSurface* GiveTSOSforMbMuSc(const MbPOT* pMbPOT);

private:
///////////////////////////////////
//Functions
   
     
//Data
   ToolHandle< IMuonboyToMaterialEffectsOnTrack > p_IMuonboyToMaterialEffectsOnTrack ; //!< Pointer On IMuonboyToMaterialEffectsOnTrack

};
}

#endif

