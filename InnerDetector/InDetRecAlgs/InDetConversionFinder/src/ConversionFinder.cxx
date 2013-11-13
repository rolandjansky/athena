/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          ConversionFinder.cxx  -  Description
                             -------------------
    begin   : 10-11-2005
    authors : Tatjana Lenz
    email   : tatjana.lenz@cern.ch
    changes :
 ***************************************************************************/
#include "InDetConversionFinder/ConversionFinder.h"

#include "TrkTrack/TrackCollection.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"

namespace InDet
{
    ConversionFinder::ConversionFinder(const std::string& name, ISvcLocator* pSvcLocator) 
	: AthAlgorithm(name, pSvcLocator),
        m_tracksName("Tracks"),
        m_InDetConversionOutputName("InDetConversion"),
        m_VertexFinderTool("InDet::InDetConversionFinderTools")
    {
	/* Retrieve StoreGate container and tool names from job options */
	declareProperty("TracksName",m_tracksName);
	declareProperty("InDetConversionOutputName", m_InDetConversionOutputName);
	declareProperty("VertexFinderTool",m_VertexFinderTool);

    }
    
    ConversionFinder::~ConversionFinder(){}

   StatusCode ConversionFinder::initialize()
   {
        /* Get the VertexFinderTool */
        if ( m_VertexFinderTool.retrieve().isFailure() ) {
           msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexFinderTool << endreq;
           return StatusCode::FAILURE;
        } else {
	  msg(MSG::INFO) << "Retrieved tool " << m_VertexFinderTool << endreq;
        }

	resetStatistics();

	msg(MSG::INFO) << "Initialization successful" << endreq;

	return StatusCode::SUCCESS;
   }

   StatusCode ConversionFinder::finalize()
   {
      if ( msg().level() == MSG::INFO ){
          msg(MSG::INFO) <<"Summary:  "<<endreq;
          std::cout<< "------------------------------------------------------------"<<std::endl;
          std::cout<< "Processed: " << m_events_processed<< " events " <<std::endl;
          std::cout<< "Stored   : " << m_Gamma_stored<<" Conversions " <<std::endl;
          std::cout<< "------------------------------------------------------------"<<std::endl;
	  std::cout<< "| Double Conversions       Si-Si      Si-TRT      TRT-TRT  |"
	  <<std::endl;
	  std::cout<<"| "<<
	  std::setw(10)<<m_Double_Conversions<<
          std::setw(18)<<m_SiSi_Conversions<<
	  std::setw(11)<<m_SiTrt_Conversions<<
	  std::setw(13)<<m_TrtTrt_Conversions<<"     |"<<
	  std::endl;
          std::cout<< "------------------------------------------------------------" <<std::endl;
	  std::cout<< "| Single Conversions               Si                TRT   |"
	  <<std::endl;
	  std::cout<<"| "<<
	  std::setw(10)<<m_Single_Conversions<<
          std::setw(25)<<m_Si_Conversions<<
	  std::setw(19)<<m_Trt_Conversions<<"    |"<<
	  std::endl;
          std::cout<< "------------------------------------------------------------" <<std::endl;
      }
      return StatusCode::SUCCESS;
   }

   void ConversionFinder::resetStatistics() {
      m_events_processed   = 0;
      m_Gamma_stored       = 0;
      m_Double_Conversions = 0;
      m_Single_Conversions = 0;
      m_SiSi_Conversions   = 0;
      m_SiTrt_Conversions  = 0;
      m_TrtTrt_Conversions = 0;
      m_Si_Conversions     = 0;
      m_Trt_Conversions    = 0;
   }

    StatusCode ConversionFinder::execute()
    {
	
	m_events_processed++;
        VxContainer* InDetConversionContainer ( 0 );

	//---- First try if m_tracksName is a TrackCollection -----------------//
	if ( evtStore()->contains<TrackCollection> ( m_tracksName ) )
	  {
	    const DataVector<Trk::Track> * trk_coll(0);
	    
	    if( evtStore()->retrieve( trk_coll, m_tracksName ).isFailure() ){
	      msg(MSG::DEBUG) << "Could not find Trk::TrackCollection " <<  m_tracksName << " in StoreGate." << endreq;
	      return StatusCode::SUCCESS;
	    } 
	
	    // Find conversions
	    InDetConversionContainer = m_VertexFinderTool->findVertex ( trk_coll );
	  }
	//---- Second try if m_tracksName is a TrackParticleBaseCollection ----//
	else if ( evtStore()->contains<Trk::TrackParticleBaseCollection> ( m_tracksName ) )
	  {
	    const Trk::TrackParticleBaseCollection *trackParticleBaseCollection(0);
	    if ( evtStore()->retrieve ( trackParticleBaseCollection, m_tracksName ).isFailure() )
	      {
		msg(MSG::DEBUG) << "Could not find Trk::TrackParticleBaseCollection " << m_tracksName << " in StoreGate." << endreq;
		return StatusCode::SUCCESS;
	      }

	    InDetConversionContainer = m_VertexFinderTool->findVertex ( trackParticleBaseCollection );
	  }
	else
	  {
	    msg(MSG::DEBUG) << "Neither a TrackCollection nor a TrackParticleBaseCollection with key " << m_tracksName << " exists in StoreGate." << endreq;
	    return StatusCode::SUCCESS;
	  }

        //Analyze the results, update counters
        if (InDetConversionContainer) {
	  m_Gamma_stored += InDetConversionContainer->size();
	  analyzeResults(InDetConversionContainer);
	}

	// Write to StoreGate
	msg(MSG::DEBUG) << " Recording conversion collection with key: " << m_InDetConversionOutputName << endreq;
	if( evtStore()->record(InDetConversionContainer, m_InDetConversionOutputName, false).isFailure() ){
	  msg(MSG::ERROR) << "Could not register conversion." << endreq;
	  return StatusCode::FAILURE;
	}
     
	return StatusCode::SUCCESS;
    }

    void ConversionFinder::analyzeResults(VxContainer* convContainer) {

      VxContainer::const_iterator fz,fze = convContainer->end();
      for ( fz = convContainer->begin(); fz!=fze; ++fz){

	if ((*fz)->vxTrackAtVertex()) {
	  int numTracksPerVertex = (*fz)->vxTrackAtVertex()->size();
          if  (numTracksPerVertex == 2) m_Double_Conversions++;
	  else                          m_Single_Conversions++;

          bool isTrt1 = false; bool isSi1 = false; bool isTrt2 = false; bool isSi2 = false; 
	  std::vector<Trk::VxTrackAtVertex*> * trkAtVtx = (*fz)->vxTrackAtVertex();
	  for (unsigned int i = 0; i < trkAtVtx->size() ; ++i) {

	    const Trk::ITrackLink * trLink =(*(trkAtVtx))[i]->trackOrParticleLink();
	    const Trk::TrackParticleBase* tempTrk(0);
	    if(0!= trLink){
	      const Trk::LinkToTrackParticleBase * linkToTrackPB = dynamic_cast<const Trk::LinkToTrackParticleBase *>(trLink); 
	      if(0!= linkToTrackPB){
		if(linkToTrackPB->isValid()) tempTrk = linkToTrackPB->cachedElement();
		if(!tempTrk) continue;
		const Trk::TrackSummary* summary = tempTrk->trackSummary();
		if(!summary) continue;
	        int ncl  = summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfSCTHits);
	        int ntrt = summary->get(Trk::numberOfTRTHits);
		if(i==0) {
		  if(ncl>0) isSi1 = true; if(ncl==0 && ntrt>0) isTrt1 = true;
		}
		if(i==1) {
		  if(ncl>0) isSi2 = true; if(ncl==0 && ntrt>0) isTrt2 = true;
		}
	      }//end of dynamic_cast check
	    }//end of ITrackLink existance check

	  }

	  //Decide on the type of track combination in vertex
	  if(numTracksPerVertex==2){
	    if      (isSi1 && isSi2)  m_SiSi_Conversions++;
	    else if(isTrt1 && isTrt2) m_TrtTrt_Conversions++;
	    else                      m_SiTrt_Conversions++;
	  }
	  if(numTracksPerVertex==1){
	    if(isSi1)  m_Si_Conversions++;
	    if(isTrt1) m_Trt_Conversions++;
	  }
	
	}

      }

   }

}

