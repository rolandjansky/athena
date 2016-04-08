/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************
                 TrigConversionFinder.cxx  -  Description
                             -------------------
    begin   : 10-11-2005
    authors : Tatjana Lenz
    email   : tatjana.lenz@cern.ch
    trigger changes : Chrstopher Marino, Iwona Grabowska-Bold
***************************************************************************/

#include "InDetTrigConversionFinder/TrigConversionFinder.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace InDet
{//
  TrigConversionFinder::TrigConversionFinder(const std::string& n, ISvcLocator* pSvcLoc) 
    : HLT::FexAlgo(n, pSvcLoc),
      m_VertexFinderTool("InDet::InDetConversionFinderTool/InDetTrigConversionFinderTools")
  {
    declareProperty("VertexFinderTool",m_VertexFinderTool);
    
    declareMonitoredVariable("roi_nTrkPart", m_nTracks   );
    declareMonitoredVariable("roi_nConVx", m_nVertices    );
  }
  
  TrigConversionFinder::~TrigConversionFinder(){}
  

  //--------------------------------------------------------------------------------
  //initialize
  //--------------------------------------------------------------------------------
  HLT::ErrorCode TrigConversionFinder::hltInitialize()
  {
    msg() << MSG::INFO << "TrigConversionFinder::hltInitialize(). "<< endreq;
    
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_VertexFinderTool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_VertexFinderTool << endreq;
    }
    
    msg() << MSG::INFO << "Initialization successful" << endreq;
    
    resetStatistics();

    return HLT::OK;

  }//--end initialize---------------------------------------------------------------


  //--------------------------------------------------------------------------------
  //reset statistics
  //--------------------------------------------------------------------------------
  void TrigConversionFinder::resetStatistics() {
    m_events_processed   = 0;
    m_Gamma_stored       = 0;
    m_Double_Conversions = 0;
    m_Single_Conversions = 0;
    m_SiSi_Conversions   = 0;
    m_SiTrt_Conversions  = 0;
    m_TrtTrt_Conversions = 0;
    m_Si_Conversions     = 0;
    m_Trt_Conversions    = 0;
  }//--end reset statistics---------------------------------------------------------


  //--------------------------------------------------------------------------------
  //Execute
  //--------------------------------------------------------------------------------
  HLT::ErrorCode  TrigConversionFinder::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE){

    m_nTracks = 0;
    m_nVertices = 0;
	
    int outputLevel = msgLvl();

    m_events_processed++;
    xAOD::VertexContainer* InDetTrigConversionContainer (0);
	
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " In execHLTAlgorithm()" << endreq;

    
    //----------------------------------------------------------------------
    // Trigger specific part: navigate throw the trigger element to get the
    // input track collection.
    //----------------------------------------------------------------------
    
    const xAOD::TrackParticleContainer* TPC(0);

    if ( HLT::OK != getFeature(outputTE, TPC) ) {
      msg() << MSG::ERROR << " Input track particle collection could not be found " << endreq;
      
      return HLT::NAV_ERROR;
    }
    if(!TPC){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input track particle collection has 0 size. Algorithm not executed..." << endreq;
      
      return HLT::OK; 
    }
    else{
      m_nTracks = TPC->size();

      if (outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST: Retrieved input track particle container with "
	      << m_nTracks << " tracks. " << endreq;

#if 0
      Trk::TrackParticleBaseCollection* tpBaseColl = new Trk::TrackParticleBaseCollection(SG::VIEW_ELEMENTS);
      for (Rec::TrackParticleContainer::const_iterator itr  = TPC->begin(); itr != TPC->end(); itr++){
	tpBaseColl->push_back(*itr);
      }
#endif
     
      // Find conversions
      // virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackParticles) = 0;
      std::pair <xAOD::VertexContainer*, xAOD::VertexAuxContainer*> foo;
      foo = m_VertexFinderTool->findVertex(TPC);
      InDetTrigConversionContainer = foo.first;
      //delete tpBaseColl;
    }
      
    
    //Analyze the results, update counters
    if (InDetTrigConversionContainer) {
      m_Gamma_stored += InDetTrigConversionContainer->size();
      m_nVertices= InDetTrigConversionContainer->size();
      analyzeResults(InDetTrigConversionContainer);
    }
    
    //Attach to the trigger element
    if (HLT::OK != attachFeature(outputTE,InDetTrigConversionContainer,"ConversionVx") ) {
      
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      return HLT::NAV_ERROR;
    }
    
    if(outputLevel <= MSG::DEBUG){
      msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
      msg() << MSG::DEBUG << "REGTEST: Output conversion container size :" << InDetTrigConversionContainer->size() << endreq;
    } 
    
    return HLT::OK;
  }

  void TrigConversionFinder::analyzeResults(xAOD::VertexContainer* convContainer) {
    MsgStream log(msgSvc(), name());
    
    xAOD::VertexContainer::const_iterator fz,fze = convContainer->end();
    for ( fz = convContainer->begin(); fz!=fze; ++fz){
      
      if ((*fz)->vxTrackAtVertexAvailable()) {
	int numTracksPerVertex = (*fz)->vxTrackAtVertex().size();
	if  (numTracksPerVertex == 2) m_Double_Conversions++;
	else                          m_Single_Conversions++;
	
	bool isTrt1 = false; bool isSi1 = false; bool isTrt2 = false; bool isSi2 = false; 
	std::vector<Trk::VxTrackAtVertex> trkAtVtx = (*fz)->vxTrackAtVertex();
	for (unsigned int i = 0; i < trkAtVtx.size() ; ++i) {
	  
	  const Trk::ITrackLink * trLink = trkAtVtx[i].trackOrParticleLink();
	  const Trk::TrackParticleBase* tempTrk(0);
	  if(0!= trLink){
	    const Trk::LinkToTrackParticleBase * linkToTrackPB = dynamic_cast<const Trk::LinkToTrackParticleBase *>(trLink); 
	    if(0!= linkToTrackPB){
	      if(linkToTrackPB->isValid()) tempTrk = linkToTrackPB->cachedElement();
	      const Trk::TrackSummary* summary = tempTrk->trackSummary();
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

  //--------------------------------------------------------------------------------
  //finalize
  //--------------------------------------------------------------------------------
  HLT::ErrorCode TrigConversionFinder::hltFinalize() {
    
    msg() << MSG::INFO << "TrigConversionFinder::finalize()" << endreq;

    int outputLevel = msgLvl();

    if ( outputLevel <= MSG::INFO ){
      msg()<< MSG::INFO <<"Summary:  "<<endreq;
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
    
    return HLT::OK;
    
  }//--end finalize-----------------------------------------------------------------
  
  }//--end namespace InDet------------------------------------------------------------
