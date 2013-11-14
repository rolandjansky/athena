/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonBackTracker_H
#define MuonBackTracker_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
        
//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"

namespace Trk {
  class  MeasuredPerigee;
}

#include "MboyAthToolInterfaces/IMuonBackTracker.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class IMboyAthToolHelper;
class MboyVarByName;
class MB_Track;
class MB_TrackAth ;

namespace Muon {
  class IMboyProperTrackRepresentationTransformTool ;
  class IMboyTrackRepresentationTransformTool ;
  class IMboyDetectorLimitsTool ;
  class IMboyTrackUndressingTool ;
  class IMboyProperTrackRepresentationTransformTool ;
}

namespace Trk {
  class IMuonboyToTrackTool;
}
 /**
   @class MuonBackTracker

  @author samusog@cern.ch
  
  */
  
namespace Muon {

class MuonBackTracker:public AthAlgTool, virtual public IMuonBackTracker {
public:
    MuonBackTracker(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonBackTracker 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   virtual Trk::Track*           MuonBackTrack(const Trk::Track*           pTrack          ) ;

private:
///////////////////////////////////

   MB_Track* MboyMuonBackTrack(MB_Track* pMB_Track ) ;

   std::string m_ToolName ;

   /** Controled proprieties  */
   void SetOwnMboyVarByName();
   void declareControledProperty( std::string Name1, double Value, std::string Comment = " No Comment ") ; //!< Controled proprieties 
   void PrintControledProperty()                                                                         ; //!< Controled proprieties 
   int    m_NberOfCtrlPpty                                                                               ; //!< Controled proprieties 
   std::vector<std::string>  m_NameOfCtrlPpty                                                            ; //!< Controled proprieties 
   double m_ValueOfCtrlPpty[100]                                                                         ; //!< Controled proprieties 
   std::vector<std::string>  m_CommentOfCtrlPpty                                                         ; //!< Controled proprieties 

   /** Define configuration variables to call the Svc */
   MboyVarByName* p_MboyVarByName   ; //!< configuration variables
      
   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper
   ToolHandle< Muon::IMboyProperTrackRepresentationTransformTool > p_IMboyProperTrackRepresentationTransformTool ; //!< Pointer On IMboyProperTrackRepresentationTransformTool
   ToolHandle< Muon::IMboyTrackUndressingTool > p_IMboyTrackUndressingTool ; //!< Pointer On IMboyTrackUndressingTool
   ToolHandle< Muon::IMboyDetectorLimitsTool > p_IMboyDetectorLimitsTool ; //!< Pointer On IMboyDetectorLimitsTool
   ToolHandle< Muon::IMboyTrackRepresentationTransformTool > p_IMboyTrackRepresentationTransformTool ; //!< Pointer On IMboyTrackRepresentationTransformTool
   ToolHandle< Trk::IMuonboyToTrackTool >   p_IMuonboyToTrackTool        ; //!< Pointer On IMuonboyToTrackTool

};

}

#endif

