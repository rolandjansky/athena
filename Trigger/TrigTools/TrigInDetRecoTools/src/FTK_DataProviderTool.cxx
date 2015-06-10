/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <bitset>
#include "TrigInDetRecoTools/FTK_DataProviderTool.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/Perigee.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "IRegionSelector/IRoiDescriptor.h"


FTK_DataProviderTool::FTK_DataProviderTool(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_ftkTracksName("FTK_Trk_Tracks"),
  m_ftkTrackContainer(0)
{

  declareInterface<IFTK_DataProviderTool>( this );
  
}



FTK_DataProviderTool::~FTK_DataProviderTool() 
{

}



StatusCode FTK_DataProviderTool::initialize() 
{

  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  athenaLog << MSG::INFO << "FTK_DataProviderTool initialized" << endreq;

  return sc;

}



StatusCode FTK_DataProviderTool::finalize() 
{

  StatusCode sc = AthAlgTool::finalize(); 

  return sc;

}


StatusCode FTK_DataProviderTool::retrieveFTK_Data(TrackCollection& trackCollection)
{
 
  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::INFO) {
    athenaLog << MSG::INFO << "FTK_DataProviderTool called in full-scan mode to retrieve FTK TrackCollection from StoreGate and return TrackCollection" << endreq;
  }

  sc = getTrackCollection();

  if (sc.isFailure()) {
    athenaLog << MSG::ERROR << "Failed to retrieve FTK TrackCollection" << endreq;
    return sc;
  }

  trackCollection.clear();

  TrackCollection::const_iterator trkItEnd = m_ftkTrackContainer->end();
  for (TrackCollection::const_iterator trkIt = m_ftkTrackContainer->begin(); trkIt != trkItEnd; ++trkIt) {
    
    const Trk::Track* inTrkTrack = (*trkIt);
    if (!inTrkTrack) continue;

    Trk::Track* outTrkTrack = createOutputTrack(inTrkTrack);
    if (!outTrkTrack) continue;
    
    trackCollection.push_back(outTrkTrack);
    
  }
  
  if (outputLevel <= MSG::INFO) {
    athenaLog << MSG::INFO << "Number of retrieved tracks : " << trackCollection.size() << " (out of " << m_ftkTrackContainer->size() << ")" << endreq;
  }

  return sc;
  
}


StatusCode FTK_DataProviderTool::retrieveFTK_Data(const IRoiDescriptor& roi, TrackCollection& trackCollection) 
{
 
  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::INFO) {
    athenaLog << MSG::INFO << "FTK_DataProviderTool called in RoI-based mode to retrieve FTK TrackCollection from StoreGate and return TrackCollection" << endreq;
  }

  sc = getTrackCollection();

  if(sc.isFailure()) {
    athenaLog << MSG::ERROR << "Failed to retrieve FTK Tracks" << endreq;
    return sc;
  }

  double roiPhiMin = roi.phiMinus();
  double roiPhiMax = roi.phiPlus();
  double roiEtaMin = roi.etaMinus();
  double roiEtaMax = roi.etaPlus();

  trackCollection.clear();

  TrackCollection::const_iterator trkItEnd = m_ftkTrackContainer->end();
  for (TrackCollection::const_iterator trkIt = m_ftkTrackContainer->begin(); trkIt != trkItEnd; ++trkIt) {
    
    const Trk::Track* inTrkTrack = (*trkIt);
    if (!inTrkTrack) continue;

    const Trk::Perigee* perigee = inTrkTrack->perigeeParameters();
    if (!perigee) continue;

    double pEta = perigee->eta();
    double pPhi = perigee->parameters()[Trk::phi0];

    if (pEta<roiEtaMin || pEta>roiEtaMax || pPhi<roiPhiMin || pPhi>roiPhiMax) continue;

    Trk::Track* outTrkTrack = createOutputTrack(inTrkTrack);
    if (!outTrkTrack) continue;
    
    trackCollection.push_back(outTrkTrack);
    
  }

  athenaLog << MSG::INFO << "Number of retrieved tracks : " << trackCollection.size() << " (out of " << m_ftkTrackContainer->size() << ")" << endreq;

  return sc;

}


StatusCode FTK_DataProviderTool::retrieveFTK_Data(std::vector<const Trk::Track*>& vtrack)
{
 
  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::INFO) {
    athenaLog << MSG::INFO << "FTK_DataProviderTool called in full-scan mode to retrieve FTK TrackCollection from StoreGate and return std::vector<const Trk:Track*>" << endreq;
  }

  sc = getTrackCollection();

  if (sc.isFailure()) {
    athenaLog << MSG::ERROR << "Failed to retrieve FTK Tracks" << endreq;
    return sc;
  }

  vtrack.clear();
  vtrack.reserve(m_ftkTrackContainer->size());

  TrackCollection::const_iterator trkItEnd = m_ftkTrackContainer->end();
  for (TrackCollection::const_iterator trkIt = m_ftkTrackContainer->begin(); trkIt != trkItEnd; ++trkIt) {
    
    const Trk::Track* inTrkTrack = (*trkIt);
    if (!inTrkTrack) continue;
    
    Trk::Track* outTrkTrack = createOutputTrack(inTrkTrack);
    if (!outTrkTrack) continue;
    
    vtrack.push_back(outTrkTrack);
    
  }
  
  if (vtrack.size() != m_ftkTrackContainer->size()) {
    athenaLog << MSG::WARNING << "Only " << vtrack.size() << " tracks out of " << m_ftkTrackContainer->size() << " successfully retrieved!" << endreq; 
  }

  return sc;
  
}


StatusCode FTK_DataProviderTool::retrieveFTK_Data(const IRoiDescriptor& roi, std::vector<const Trk::Track*>& vtrack) 
{
 
  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::INFO) {
    athenaLog << MSG::INFO << "FTK_DataProviderTool called in RoI-based mode to retrieve FTK TrackCollection from StoreGate and return std::vector<const Trk::Track*>" << endreq;
  }

  sc = getTrackCollection();

  if(sc.isFailure()) {
    athenaLog << MSG::ERROR << "Failed to retrieve FTK Tracks" << endreq;
    return sc;
  }

  double roiPhiMin = roi.phiMinus();
  double roiPhiMax = roi.phiPlus();
  double roiEtaMin = roi.etaMinus();
  double roiEtaMax = roi.etaPlus();

  vtrack.clear();

  TrackCollection::const_iterator trkItEnd = m_ftkTrackContainer->end();
  for (TrackCollection::const_iterator trkIt = m_ftkTrackContainer->begin(); trkIt != trkItEnd; ++trkIt) {
    
    const Trk::Track* inTrkTrack = (*trkIt);
    if (!inTrkTrack) continue;

    const Trk::Perigee* perigee = inTrkTrack->perigeeParameters();
    if (!perigee) continue;

    double pEta = perigee->eta();
    double pPhi = perigee->parameters()[Trk::phi0];

    if (pEta<roiEtaMin || pEta>roiEtaMax || pPhi<roiPhiMin || pPhi>roiPhiMax) continue;

    Trk::Track* outTrkTrack = createOutputTrack(inTrkTrack);
    if (!outTrkTrack) continue;
    
    vtrack.push_back(outTrkTrack);
    
  }

  return sc;

}


StatusCode FTK_DataProviderTool::retrieveFTK_Data(std::vector<TrigFTK_TrackSeed>& vtrackseed)
{

  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::INFO) {
    athenaLog << MSG::INFO << "FTK_DataProviderTool called in full-scan mode to retrieve FTK TrackCollection from StoreGate and return std::vector<TrigFTK_TrackSeed>" << endreq;
  }
  
  sc = getTrackCollection();

  if(sc.isFailure()) {
    athenaLog << MSG::ERROR << "Failed to retrieve FTK Tracks" << endreq;
    return sc;
  }

  vtrackseed.clear();
  vtrackseed.reserve(m_ftkTrackContainer->size());

  TrackCollection::const_iterator trkItEnd = m_ftkTrackContainer->end();
  for (TrackCollection::const_iterator trkIt = m_ftkTrackContainer->begin(); trkIt != trkItEnd; ++trkIt) {
    
    const Trk::Track* inTrkTrack = (*trkIt);
    if (!inTrkTrack) continue;

    TrigFTK_TrackSeed trackseed;    

    const Trk::Perigee* perigee = inTrkTrack->perigeeParameters();
    if (!perigee) continue;

    //const Trk::MeasuredPerigee* mp = dynamic_cast<const Trk::MeasuredPerigee*>(perigee);
    //if (!mp) continue;
    
    //trackseed.InitMeasuredPerigee(mp);
    trackseed.InitMeasuredPerigee(perigee);

    const DataVector<const Trk::TrackStateOnSurface>* DVtsos = inTrkTrack->trackStateOnSurfaces();    

    if (DVtsos!=0) {
      
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosItEnd = DVtsos->end();
      for (DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIt = DVtsos->begin(); tsosIt != tsosItEnd; ++tsosIt) {
	
	const Trk::TrackStateOnSurface* tsos = (*tsosIt);
	if (tsos==0) continue;
	
	if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {

	  //const Trk::GlobalPosition* pGP = tsos->surface().localToGlobal(tsos->trackParameters()->localPosition());	
	  const Amg::Vector3D* pGP = tsos->surface().localToGlobal(tsos->trackParameters()->localPosition());	
	  if (!pGP) continue;

	  trackseed.AddGlobalPosition(*pGP);

	  delete pGP;

	}
	
      }

    }
    
    vtrackseed.push_back(trackseed);      

  }

  return sc;
  
}


StatusCode FTK_DataProviderTool::retrieveFTK_Data(const IRoiDescriptor& roi, std::vector<TrigFTK_TrackSeed>& vtrackseed) 
{

  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::INFO) {
    athenaLog << MSG::INFO << "FTK_DataProviderTool called in RoI-based mode to retrieve FTK TrackCollection from StoreGate and return std::vector<TrigFTK_TrackSeed>" << endreq;
  }

  sc = getTrackCollection();

  if(sc.isFailure()) {
    athenaLog << MSG::ERROR << "Failed to retrieve FTK Tracks" << endreq;
    return sc;
  }

  double roiPhiMin = roi.phiMinus();
  double roiPhiMax = roi.phiPlus();
  double roiEtaMin = roi.etaMinus();
  double roiEtaMax = roi.etaPlus();

  vtrackseed.clear();
  
  TrackCollection::const_iterator trkItEnd = m_ftkTrackContainer->end();
  for (TrackCollection::const_iterator trkIt = m_ftkTrackContainer->begin(); trkIt != trkItEnd; ++trkIt) {
    
    const Trk::Track* inTrkTrack = (*trkIt);
    if (!inTrkTrack) continue;
        
    const Trk::Perigee* perigee = inTrkTrack->perigeeParameters();
    if (!perigee) continue;

    double pEta = perigee->eta();
    double pPhi = perigee->parameters()[Trk::phi0];
    
    if (pEta<roiEtaMin || pEta>roiEtaMax || pPhi<roiPhiMin || pPhi>roiPhiMax) continue;
    
    TrigFTK_TrackSeed trackseed;

    //const Trk::MeasuredPerigee* mp = dynamic_cast<const Trk::MeasuredPerigee*>(perigee);
    //if (!mp) continue; 

    //trackseed.InitMeasuredPerigee(mp);
    trackseed.InitMeasuredPerigee(perigee);

    const DataVector<const Trk::TrackStateOnSurface>* DVtsos = inTrkTrack->trackStateOnSurfaces();    

    if (DVtsos!=0) {

      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosItEnd = DVtsos->end();
      for (DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIt = DVtsos->begin(); tsosIt != tsosItEnd; ++tsosIt) {
	
	const Trk::TrackStateOnSurface* tsos = (*tsosIt);
	if (tsos==0) continue;
	
	if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {	  

	  //const Trk::GlobalPosition* pGP = tsos->surface().localToGlobal(tsos->trackParameters()->localPosition());
	  const Amg::Vector3D* pGP = tsos->surface().localToGlobal(tsos->trackParameters()->localPosition());
	  if (!pGP) continue;

	  trackseed.AddGlobalPosition(*pGP);

	  delete pGP;

	}
	
      }

    }

    vtrackseed.push_back(trackseed);      
    
  }

  return sc;
  
}


StatusCode FTK_DataProviderTool::getTrackCollection() 
{
  // Set member pointer 'm_ftkTrackContainer' to point to 
  // the FTK TrackCollection from StoreGate 

  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());  

  m_ftkTrackContainer = nullptr;

  if (!evtStore()->transientContains<TrackCollection>(m_ftkTracksName)) {  
    athenaLog << MSG::WARNING << " FTK tracks  " << m_ftkTracksName << " not found in StoreGate !" << endreq;
    return sc;
  }  
  else { 
    sc = evtStore()->retrieve(m_ftkTrackContainer, m_ftkTracksName); 
    if (sc.isFailure()) { 
      athenaLog << MSG::ERROR << "Failed to get FTK Tracks Container" << endreq; 
      return sc; 
    }
  } 

  if (!m_ftkTrackContainer) sc = StatusCode::FAILURE;

  return sc;

}


Trk::Track* FTK_DataProviderTool::createOutputTrack(const Trk::Track* inTrack) 
{

  // Return a copy of 'inTrack'

  Trk::Track* outTrack = nullptr;
  
  if (inTrack) outTrack = new Trk::Track(*inTrack);

  return outTrack;

}
