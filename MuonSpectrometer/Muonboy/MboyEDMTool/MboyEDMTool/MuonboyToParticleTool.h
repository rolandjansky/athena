/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToParticleTool_H
#define MuonboyToParticleTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

namespace Trk {
  class  ITrackParticleCreatorTool;
}
namespace Rec {
  class  TrackParticle;
  class  TrackParticleContainer;
}

#include "MboyEDMToolInterfaces/IMuonboyToParticleTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MuonboyToParticleTool

   This tools converts Muonboy tracks into Rec::TrackParticle
   
  @author samusog@cern.ch
  
  */

class MuonboyToParticleTool :public AthAlgTool, virtual public IMuonboyToParticleTool{
public:
    MuonboyToParticleTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToParticleTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Convert collection */
   virtual StatusCode  convertCollection(
                                 const TrackCollection*       pTrackCollection,
                                 Rec::TrackParticleContainer* pTrackParticleContainer
                                );

   /** Convert single object */
   virtual Rec::TrackParticle* convert(
                               const Trk::Track* pTrack
			      );

private:
///////////////////////////////////
//Data

  ToolHandle< Trk::ITrackParticleCreatorTool > p_ITrackParticleCreatorTool ; //!< The ITrackParticleCreatorTool
  

};
#endif 




