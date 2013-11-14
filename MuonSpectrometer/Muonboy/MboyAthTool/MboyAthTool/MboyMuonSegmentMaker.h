/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMuonSegmentMaker_H
#define MboyMuonSegmentMaker_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "MboyAthToolInterfaces/IMboyMuonSegmentMaker.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class MboyVarByName;
class IMboyAthToolHelper;

  /**
   @class MboyMuonSegmentMaker

  @author samusog@cern.ch
  
  */

namespace Muon {

class MboyMuonSegmentMaker:public AthAlgTool, virtual public IMboyMuonSegmentMaker{
public:
    MboyMuonSegmentMaker(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyMuonSegmentMaker 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();


   /** Get Segments from PrepRawData */
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MboyROAStoreAth* pMboyROAExternalStoreAth ) ;
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData  ) ;
   
   /** Get Segments vector from RIO_OnTrack */
   virtual std::vector<const Muon::MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& rios) ;
   
   virtual std::vector<const Muon::MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& rios1,
 	                                                const std::vector<const Trk::RIO_OnTrack*>& rios2 ) ;

   virtual std::vector<const Muon::MuonSegment*>* find( const std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts,
 	                                                const std::vector<const Muon::MuonClusterOnTrack*>&  clusters) ;

           std::vector<const Muon::MuonSegment*>* find( const std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts,
 	                                                const std::vector<const Muon::MuonClusterOnTrack*>&  clusters,
                                                        MboyROAStoreAth* pMboyROAExternalStoreAth) ;


   virtual std::vector<const Muon::MuonSegment*>* find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
                                                        const std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts,
                                                        const std::vector<const Muon::MuonClusterOnTrack*>&  clusters,
                                                        bool updatePhi=false, double momentum = 1e9 ) ;

   virtual std::vector<const Muon::MuonSegment*>* find( const Trk::TrackRoad& road,
                                                        const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts,
                                                        const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >&  clusters,
                                                        bool updatePhi=false, double momentum = 1e9 ) ;            

private:
///////////////////////////////////

   std::string m_ToolName ;

   int m_UseNewSegmentFinding               ; //!< Use the default or new segment finding

   /** Controled proprieties  */
   void declareControledProperty( std::string Name1, double Value, std::string Comment = " No Comment ") ; //!< Controled proprieties 
   void PrintControledProperty()                                                                         ; //!< Controled proprieties 
   int    m_NberOfCtrlPpty                                                                               ; //!< Controled proprieties 
   std::vector<std::string>  m_NameOfCtrlPpty                                                            ; //!< Controled proprieties 
   double m_ValueOfCtrlPpty[100]                                                                         ; //!< Controled proprieties 
   std::vector<std::string>  m_CommentOfCtrlPpty                                                         ; //!< Controled proprieties 

   /** Define configuration variables to call the Svc */
   MboyVarByName* p_MboyVarByName   ; //!< configuration variables
   
   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper

   int m_Clean ;  //!< if 1 clean segments

};

}

#endif

