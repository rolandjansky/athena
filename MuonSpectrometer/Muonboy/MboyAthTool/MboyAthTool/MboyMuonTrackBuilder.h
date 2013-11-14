/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMuonTrackBuilder_H
#define MboyMuonTrackBuilder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/IIncidentListener.h"

class IIncidentSvc;
        
//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Can not be forwarded due to typedef
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Muon {
  class MuPatCandidateBase;
  class MuPatSegment;
  class MuPatTrack;  
  class MuonSegment;  
  class IFindMonitor;  
  class IRefineMonitor;  
}


#include "MuonRecToolInterfaces/IMuonTrackBuilder.h"
#include "MuonRecToolInterfaces/IMuonSegmentInfoExtender.h"
#include "MuonRecToolInterfaces/IMuonTrackRefiner.h"

namespace Trk {
  class IMuonboyToTrackParametersTool ;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class IMboyGetPrepDataFromEventTool;
class IMboyAthToolHelper;

class MboyVarByName;
class MuonRecDigiStoreAth ;

  /**
   @class MboyMuonTrackBuilder

  @author samusog@cern.ch
  
  */
  
static const InterfaceID IID_MboyMuonTrackBuilder("Muon::MboyMuonTrackBuilder",1,0);

namespace Muon {

class MboyMuonTrackBuilder:public AthAlgTool, virtual public IMuonTrackBuilder,  virtual public IMuonSegmentInfoExtender,  virtual public IMuonTrackRefiner, virtual public IIncidentListener {
public:
    MboyMuonTrackBuilder(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyMuonTrackBuilder 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();
   static const InterfaceID& interfaceID() { return IID_MboyMuonTrackBuilder; }

   virtual std::vector<Muon::MuPatTrack*>* find( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree ) ;

   virtual void extendInfo( 
                              Muon::MuPatSegment * pMuPatSegment 
                          ) ;
                          
   virtual Muon::MuPatTrack* refine( Muon::MuPatTrack& MuPatTrackToBeRefined );
   
   /**  incident service handle */
   void handle(const Incident& incident); 

   Muon::MuPatTrack* DoMuPaTrackFromMuPatSegment(
                                                 Muon::MuPatSegment* pMuPatSegment 
                                                 ) ;

private:
///////////////////////////////////

   std::vector<Muon::MuPatTrack*>* findProtected( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree ) ;
   std::vector<Muon::MuPatTrack*>* findUnprotected( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree ) ;

   Muon::MuPatTrack* refineProtected( Muon::MuPatTrack& MuPatTrackToBeRefined );
   
   void DumpMuPatTrack(const Muon::MuPatTrack* pMuPatTrack);
   
   void DumpMuPatTrackS(const std::vector<Muon::MuPatTrack*>* pVectorOfMuPatTrack);
   
   void DumpMuPatSegment(const Muon::MuPatSegment* pMuPatSegment);

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
      
   ToolHandle< Muon::IFindMonitor > p_IFindMonitor ; //!< Pointer On IFindMonitor
   ToolHandle< Muon::IRefineMonitor > p_IRefineMonitor ; //!< Pointer On IRefineMonitor
   ToolHandle< Trk::IMuonboyToTrackParametersTool > p_IMuonboyToTrackParametersTool ; //!< Pointer On IMuonboyToTrackParametersTool
   ToolHandle< IMboyGetPrepDataFromEventTool > p_IMboyGetPrepDataFromEventTool ; //!< Pointer On IMboyGetPrepDataFromEventTool
   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper
   ServiceHandle< IIncidentSvc > p_IIncidentSvc; //!< Pointer On IIncidentSvc

   /** Load the Event */
   MuonRecDigiStoreAth* p_MuonRecDigiStoreAth ;
   int m_EventShouldBeLoaded ;
   void LoadEvent();

   int m_DoDBg ;
   int m_MonitorFind ;
   int m_MonitorRefine ;

};

}

#endif

