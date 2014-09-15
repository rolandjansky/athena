/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyTrackUndressingTool_H
#define MboyTrackUndressingTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkParameters/TrackParameters.h"

namespace Muon {
class IMboyDetectorLimitsTool ;
class IMboyTrackRepresentationTransformTool ;
}

#include "MboyEDMToolInterfaces/IMboyTrackUndressingTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboyTrackUndressingTool

  @author samusog@cern.ch
  
  */

namespace Muon {

class MboyTrackUndressingTool: public AthAlgTool, virtual public IMboyTrackUndressingTool{
public:
    MboyTrackUndressingTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyTrackUndressingTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   virtual Trk::Track* UndressTrack (const Trk::Track* pTrack_atIP) ;
   virtual DataVector<const Trk::TrackStateOnSurface>* UndressTrackTSOSs(const Trk::Track* pTrack_atIP) ;

private:
///////////////////////////////////

  std::string m_ToolName ;

  ToolHandle< Muon::IMboyDetectorLimitsTool > p_IMboyDetectorLimitsTool ; //!< Pointer On IMboyDetectorLimitsTool
  ToolHandle< Muon::IMboyTrackRepresentationTransformTool > p_IMboyTrackRepresentationTransformTool ; //!< Pointer On IMboyTrackRepresentationTransformTool

  Trk::TrackStateOnSurface* AtACOrAtAD2TSOS(const Trk::TrackParameters* pTrackParameters) ;

};

}

#endif

