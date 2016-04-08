/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGMeasAssocAlg.cxx
//   Implementation file for class MuonTGMeasAssocAlg
///////////////////////////////////////////////////////////////////
// S.Todorova
///////////////////////////////////////////////////////////////////

#include "MuonTGMeasAssocAlg/MuonTGMeasAssocAlg.h"

// Gaudi includes
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include <vector>

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonSegment/MuonSegment.h"

//Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// Constructor with parameters:
Muon::MuonTGMeasAssocAlg::MuonTGMeasAssocAlg(const std::string &name, ISvcLocator *pSvcLocator ) :
  AthAlgorithm(name,pSvcLocator),
  m_mdtIn(true),
  m_rpcIn(true),
  m_cscIn(true),
  m_tgcIn(true),
  m_segmentsIn(false),
  m_writeTgHits(false),
  m_writeTgSegments(false),
  m_reAlign(false),
  m_misAlign(false),
  m_allowGeomAssoc(true),
  m_trackingGeometry(0),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_muonTgTool("Muon::MuonTGMeasurementTool/MuonTGMeasurementTool"),
  m_mdtIdHelper(0),
  m_rpcIdHelper(0),
  m_cscIdHelper(0),
  m_tgcIdHelper(0),
  m_muonMgr(0),
  m_inputSegmentCollectionMoore("MooreSegments"),
  m_inputSegmentCollectionMoMu("MuonSegments_MoMu"),
  m_inputSegmentCollectionMBoy("ConvertedMBoySegments"),
  m_inputMdtPrdCollection("MDT_DriftCircles"),  
  m_inputRpcPrdCollection("RPC_Measurements"),  
  m_inputTgcPrdCollection("TGC_Measurements"),  
  m_inputCscPrdCollection("CSC_Clusters"),  
  m_StoreGate(0),
  m_activeStore(0),
  m_mdtHits(0),
  m_rpcHits(0),
  m_cscHits(0),
  m_tgcHits(0),
  m_allHits(0),
  m_allSegments(0),
  m_extrapolator("Trk::Extrapolator/Extrapolator")
{
  declareProperty("TrackingGeometry",     m_trackingGeometryName);
  declareProperty("Extrapolator",         m_extrapolator);
  declareProperty("ProcessMdtHits",       m_mdtIn);
  declareProperty("ProcessRpcHits",       m_rpcIn);
  declareProperty("ProcessCscHits",       m_cscIn);
  declareProperty("ProcessTgcHits",       m_tgcIn);
  declareProperty("ProcessSegments",      m_segmentsIn);
  declareProperty("WriteAssociatedHits",  m_writeTgHits);
  declareProperty("WriteAssociatedSegments",  m_writeTgSegments);
  declareProperty("InputSegmentCollection_Moore",  m_inputSegmentCollectionMoore);
  declareProperty("InputSegmentCollection_MoMu" ,  m_inputSegmentCollectionMoMu);
  declareProperty("InputSegmentCollection_MBoy" ,  m_inputSegmentCollectionMBoy);
  declareProperty("InputMdtPrdCollection",  m_inputMdtPrdCollection);
  declareProperty("InputRpcPrdCollection",  m_inputRpcPrdCollection);
  declareProperty("InputTgcPrdCollection",  m_inputTgcPrdCollection);
  declareProperty("InputCscPrdCollection",  m_inputCscPrdCollection);
  declareProperty("AlignMTG",  m_reAlign);
  declareProperty("MisAlignMTG",  m_misAlign);
  declareProperty("AllowGeometricalAssociation",  m_allowGeomAssoc);
}

// Destructor
Muon::MuonTGMeasAssocAlg::~MuonTGMeasAssocAlg(){}

// Initialize method:
StatusCode Muon::MuonTGMeasAssocAlg::initialize()
{

  // Get the messaging service, print where you are
  ATH_MSG_INFO("MuonTGMeasAssocAlg::initialize()");

  StatusCode sc;

  // Get an Identifier helper object
  StoreGateSvc* detStore(0);
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Detector service not found !");
    return StatusCode::FAILURE;
  } 

  sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("StoreGate service not found !");
    return StatusCode::FAILURE;
  } 

  sc=service("ActiveStoreSvc",m_activeStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("ActiveStore service not found !");
    return StatusCode::FAILURE;
  } 

  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure())
  {
    ATH_MSG_ERROR("Cannot retrieve MdtIdHelper");
    return sc;
  }

  sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
  {
    ATH_MSG_ERROR("Cannot retrieve RpcIdHelper");
    return sc;
  }

  sc = detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure())
  {
    ATH_MSG_ERROR("Cannot retrieve cscIdHelper");
    return sc;
  }

  sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure())
  {
    ATH_MSG_ERROR("Cannot retrieve TgcIdHelper");
    return sc;
  }
  
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure())
  {
    ATH_MSG_ERROR("Cannot retrieve MuonDetectorManager...");
    return sc;
  }
  
  // get muonTG tool
  sc = m_muonTgTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find MuonTGMeasurementTool");
  }

  // get extrapolator
  sc = m_extrapolator.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find extrapolator");
  }

  // initialize misalignment
  if (m_misAlign) {
    int station,eta,phi;
    double x,y,z;
    double rotX,rotY,rotZ;
    std::ifstream misalign;
    m_misAlignDiff.clear();
    misalign.open("MTGmisAlignment.txt",std::ios::in);
    while (1) {
      misalign >> station >> eta >> phi >> x >> y >> z >> rotX >> rotY >> rotZ;
      if (!misalign.good()) break;
      Identifier id(0);
      if (station>=0) {
	id = m_mdtIdHelper->elementID(station,eta,phi);
        if (!m_mdtIdHelper->valid(id)) id = m_rpcIdHelper->elementID(station,eta,phi,1);
	if (!m_rpcIdHelper->valid(id)) id = m_tgcIdHelper->elementID(station,eta,phi);
	if (!m_tgcIdHelper->valid(id)) id = m_cscIdHelper->elementID(station,eta,phi);
      }
      Amg::Translation3D shift(x,y,z); 
      Amg::Transform3D transf = shift * Amg::RotationMatrix3D::Identity();
      transf *= Amg::AngleAxis3D(rotZ, Amg::Vector3D(0.,0.,1.));
      transf *= Amg::AngleAxis3D(rotY, Amg::Vector3D(0.,1.,0.));
      transf *= Amg::AngleAxis3D(rotX, Amg::Vector3D(1.,0.,0.));
      m_misAlignDiff.push_back(std::pair<Identifier,Amg::Transform3D>(id,transf));
    }
    misalign.close();
  }      
  return StatusCode::SUCCESS; 
}


// Execute method:
StatusCode Muon::MuonTGMeasAssocAlg::execute() 
{
  // Get the messaging service, print where you are
  ATH_MSG_INFO("MuonTGMeasAssocAlg::execute()");

  StatusCode sc;

  if (!m_trackingGeometry) {
 
    StoreGateSvc* detStore(0);
    sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Detector service not found !");
      return StatusCode::FAILURE;
    } 
    sc = detStore->retrieve(m_trackingGeometry, m_trackingGeometryName);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not find tool "<< m_trackingGeometryName<<". Exiting.");
      return sc;
    } else {
      ATH_MSG_INFO("tracking geometry Svc \""<<m_trackingGeometryName<<"\" booked ");
    } 
  }  
  // create station map if not done already ; misalign stations if required
  if (!m_stationMap.size()) {
    const Trk::TrackingVolume* vol = m_trackingGeometry->highestTrackingVolume();
    ATH_MSG_INFO("creating station map ");    
    createStationMap(vol);
    ATH_MSG_INFO("station map created with "<<m_stationMap.size()<<" members "); 
    // pass the map to the MuonTGMeasurementTool   
    // if (m_muonTgTool) m_muonTgTool->getMuonStationMap(&m_stationMap);
    //
    if (m_misAlign) misAlignStations();
  }

  // realign
  if (m_reAlign) reAlignStations();

  sc = retrieveMeasurements();

  if ( !sc.isFailure() && m_writeTgHits ) sc = storeMeasurements();

  if ( m_segmentsIn ) { 
    sc = createStationSegmentCollection();
    if ( !sc.isFailure() && m_writeTgSegments ) sc = storeSegments();
  }

  if ( m_muonTgTool ) {
    m_muonTgTool->updateAssocMeas(m_allHits,m_allSegments);
  }

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode Muon::MuonTGMeasAssocAlg::finalize() 
{
  // Get the messaging service, print where you are
  ATH_MSG_INFO("MuonTGMeasAssocAlg::finalize()");
  //delete m_tpMinFinder;
  return StatusCode::SUCCESS;
}

StatusCode Muon::MuonTGMeasAssocAlg::storeMeasurements() {
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonTGMeasAssocAlg::storeMeasurements()");
  //
  StatusCode sc;
 
  if ( m_allHits ) {
    std::string key = "MUON_TG_HITS";
    sc = m_StoreGate->record(m_allHits,key);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("MuonTGMeasAssocAlg::storeMeasurements():recording  of hit collection failed");
    } else {
      ATH_MSG_INFO("MuonTGMeasAssocAlg:::storeMeasurements():MUON_TG_HITS recorded");
    }
  }

  return StatusCode::SUCCESS;  
 }

StatusCode Muon::MuonTGMeasAssocAlg::storeSegments() {
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonTGMeasAssocAlg::storeSegments()");
  //
  StatusCode sc;
 
  if ( m_allSegments ) {
    std::string key = "MUON_TG_SEGMENTS";
    sc = m_StoreGate->record(m_allSegments,key);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("MuonTGMeasAssocAlg::storeSegments():recording of segment collection failed");
    } else {
      ATH_MSG_INFO("MuonTGMeasAssocAlg:::storeSegments():MUON_TG_SEGMENTS recorded");
    }
  }

  return StatusCode::SUCCESS;  
 }

 StatusCode Muon::MuonTGMeasAssocAlg::createStationHitCollection() const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonTGMeasAssocAlg::createStationHitCollection()" );
  
  StatusCode sc;

  //Define the pair and the vector of pairs
  m_allHits = new MuonTGHits;

  if (m_mdtIn && m_mdtHits) {
    for (unsigned int il=0; il<m_mdtHits->size();il++) {
      const Trk::DetachedTrackingVolume* station = (*m_mdtHits)[il]->first->enclosingDetachedTrackingVolume();
      if (station) { 
        bool found=false;
        unsigned int ist = 0;
        while ( !found && m_allHits->size()>0 && ist < m_allHits->size() ) {
	  if (station == (*m_allHits)[ist]->first ) {
            (*m_allHits)[ist]->second->push_back((*m_mdtHits)[il]);
	    found = true;          
          } 
          ist++;
        } 
        if (!found) {
	  std::vector<PairOfLayerPrd*>* layPrd= new std::vector<PairOfLayerPrd*>;
          layPrd->push_back((*m_mdtHits)[il]);
          m_allHits->push_back(new Muon::StationPrd(station,layPrd));
	}
      }
    }
  }

  if (m_rpcIn && m_rpcHits) {
    for (unsigned int il=0; il<m_rpcHits->size();il++) {
      const Trk::DetachedTrackingVolume* station = (*m_rpcHits)[il]->first->enclosingDetachedTrackingVolume();
      if (station) { 
        bool found=false;
        unsigned int ist = 0;
        while ( !found && ist < m_allHits->size() ) {
	  if (station == (*m_allHits)[ist]->first ) {
            (*m_allHits)[ist]->second->push_back((*m_rpcHits)[il]);
	    found = true;          
          } 
          ist++;
        } 
        if (!found) {
	  std::vector<PairOfLayerPrd*>* layPrd=new std::vector<PairOfLayerPrd*>;
          layPrd->push_back((*m_rpcHits)[il]);
          m_allHits->push_back(new Muon::StationPrd(station,layPrd));
	}
      }
    }
  }

  if (m_tgcIn && m_tgcHits) {
    for (unsigned int il=0; il<m_tgcHits->size();il++) {
      const Trk::DetachedTrackingVolume* station = (*m_tgcHits)[il]->first->enclosingDetachedTrackingVolume();
      if (station) { 
        bool found=false;
        unsigned int ist = 0;
        while ( !found && ist < m_allHits->size() ) {
	  if (station == (*m_allHits)[ist]->first ) {
            (*m_allHits)[ist]->second->push_back((*m_tgcHits)[il]);
	    found = true;          
          } 
          ist++;
        }
        if (!found) {
	  std::vector<PairOfLayerPrd*>* layPrd=new std::vector<PairOfLayerPrd*>;
          layPrd->push_back((*m_tgcHits)[il]);
          m_allHits->push_back(new Muon::StationPrd(station,layPrd));
	}
      }
    }
  }

  if (m_cscIn && m_cscHits) {
    for (unsigned int il=0; il<m_cscHits->size();il++) {
      const Trk::DetachedTrackingVolume* station = (*m_cscHits)[il]->first->enclosingDetachedTrackingVolume();
      if (station) { 
        bool found=false;
        unsigned int ist = 0;
        while ( !found && ist < m_allHits->size() ) {
	  if (station == (*m_allHits)[ist]->first ) {
            (*m_allHits)[ist]->second->push_back((*m_cscHits)[il]);
	    found = true;          
          } 
          ist++;
        } 
        if (!found) {
	  std::vector<PairOfLayerPrd*>* layPrd=new std::vector<PairOfLayerPrd*>;
          layPrd->push_back((*m_cscHits)[il]);
          m_allHits->push_back(new Muon::StationPrd(station,layPrd));
	}
      }
    }
  }

  return StatusCode::SUCCESS;
  
}

 StatusCode Muon::MuonTGMeasAssocAlg::createStationSegmentCollection() const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonTGMeasAssocAlg::createStationSegmentCollection()" );
  
  StatusCode sc;

  //Define the pair and the vector of pairs
  m_allSegments = 0;

  // retrieve input segments
  std::vector<const Trk::SegmentCollection*> segmColls;
  if (m_inputSegmentCollectionMoore != "") {
    const Trk::SegmentCollection* mooreColl = 0;
    sc = m_StoreGate->retrieve(mooreColl, m_inputSegmentCollectionMoore );
    if (!sc.isFailure()) {
      ATH_MSG_INFO(" retrieved segment collection " << m_inputSegmentCollectionMoore );
      segmColls.push_back(mooreColl);
    } else {
      ATH_MSG_INFO(" failed to retrieve segment collection " << m_inputSegmentCollectionMoore );
      return sc; 
    }
  }
  if (m_inputSegmentCollectionMoMu != "") {
    const Trk::SegmentCollection* momuColl = 0;
    sc = m_StoreGate->retrieve(momuColl, m_inputSegmentCollectionMoMu );
    if (!sc.isFailure()) {
      ATH_MSG_INFO(" retrieved segment collection " << m_inputSegmentCollectionMoMu );
      segmColls.push_back(momuColl);
    } else {
      ATH_MSG_INFO(" failed to retrieve segment collection " << m_inputSegmentCollectionMoMu );
      return sc; 
    }
  }
  if (m_inputSegmentCollectionMBoy != "") {
    const Trk::SegmentCollection* mboyColl = 0;
    sc = m_StoreGate->retrieve(mboyColl, m_inputSegmentCollectionMBoy );
    if (!sc.isFailure()) {
      ATH_MSG_INFO(" retrieved segment collection " << m_inputSegmentCollectionMBoy );
      segmColls.push_back(mboyColl);
    } else {
      ATH_MSG_INFO(" failed to retrieve segment collection " << m_inputSegmentCollectionMBoy );
      return sc; 
    }
  }
  
  if (!segmColls.size()) return StatusCode::FAILURE;

  m_allSegments = new MuonTGSegments;

  for (unsigned int ic=0; ic<segmColls.size(); ic++) {
    const Trk::SegmentCollection* segmColl = segmColls[ic];
    for (Trk::SegmentCollection::const_iterator iter = segmColl->begin(); iter!=segmColl->end(); ++iter) {
      const MuonSegment* segment = dynamic_cast<const MuonSegment*> (*iter);
      //std::cout << "segment:" << segment << "," << segmColl->size() <<"," << segment->globalPosition()<< std::endl;
      if (segment) {
	// retrieve station
	const Trk::DetachedTrackingVolume* detVol = 0;
	const std::vector<const Trk::DetachedTrackingVolume*>* 
	  detVols = m_trackingGeometry->lowestDetachedTrackingVolumes(segment->globalPosition());  
	if (detVols) {
	  if (detVols->size() > 1) ATH_MSG_INFO( "station overlaps ? " );
	  if (detVols->size() ) detVol = detVols->front();
	  if (!detVols->size() ) {
	    ATH_MSG_ERROR( "missing station?" << segment->globalPosition() );  
	    std::vector<const Trk::RIO_OnTrack*> rots = segment->containedROTs();
	    if ( rots.size() > 0 ) {
	      Identifier id = rots[0]->identify();
	      Amg::Vector3D pos(0., 0., 0.);
	      if (m_mdtIdHelper->is_mdt(id)) {
		const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(id);			
		pos = mdtROE->tubePos(id);
	      } else if ( m_mdtIdHelper->is_rpc(id)) {
		const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(id);			
		pos = rpcROE->stripPos(id);
	      } else if ( m_mdtIdHelper->is_csc(id)) {
		const MuonGM::CscReadoutElement* cscROE = m_muonMgr->getCscReadoutElement(id);			
		pos = cscROE->stripPos(id);
	      } else if ( m_mdtIdHelper->is_tgc(id)) {
		const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(id);			
		pos = tgcROE->channelPos(id);
	      }
	      const Trk::Layer* lay = m_trackingGeometry->associatedLayer(pos);
	      if (lay) detVol = lay->enclosingDetachedTrackingVolume();
	      if (detVol) ATH_MSG_DEBUG(" enclosing detached volume retrieved:" << detVol->name() );
	    }
	  }
	}            
	if (detVol) {
	  // extrapolate segment to station layer representation
	  // avoid propagation of error matrix for the moment
	  //const Trk::MeasuredAtaPlane segmPar(segment->globalPosition(),10000.*segment->globalDirection(),1.,
	  //				    segment->associatedSurface(), new Trk::ErrorMatrix(segment->localErrorMatrix()));
	  const Trk::AtaPlane segmPar(segment->globalPosition(),10000.*segment->globalDirection(),1., segment->associatedSurface());
	  const Trk::TrackParameters* layPar = m_extrapolator->extrapolateDirectly(segmPar,
										   detVol->layerRepresentation()->surfaceRepresentation(), 
										   Trk::anyDirection, true, Trk::muon);
	  // copy segment info
	  if (layPar) {
	    const Trk::PlaneSurface* plane = dynamic_cast<const Trk::PlaneSurface*> (&(detVol->layerRepresentation()->surfaceRepresentation()));
	    //std::cout <<"error matrix, fit quality:"<<&(segment->localErrorMatrix())<<","<<segment->fitQuality()<<std::endl;
	    //const Trk::MeasuredAtaPlane* mLayPar = dynamic_cast<const Trk::MeasuredAtaPlane*> (layPar);
	    //const MuonSegment* assocSegm = new MuonSegment( layPar->localPosition(),
	    //						   new Trk::LocalDirection(layPar->momentum().unit(),detVol->layerRepresentation()
	    //                                                ->surfaceRepresentation().transform().getRotation() ),
	    //					           &(mLayPar->localErrorMatrix()), new Trk::PlaneSurface(*plane),
	    //					           new std::vector<const Trk::RIO_OnTrack*>(segment->containedROTs()),
	    //						  new Trk::FitQuality( *(segment->fitQuality())) );
	    DataVector<const Trk::MeasurementBase>* meas= new DataVector<const Trk::MeasurementBase>;
	    if (segment->containedROTs().size()) {
	      const std::vector<const Trk::RIO_OnTrack*> rots=segment->containedROTs();
	      for (unsigned int im=0;im<rots.size();im++) meas->push_back(rots[im]);
	    } 

	    Trk::LocalDirection dir;
	    plane->globalToLocalDirection(layPar->momentum(),dir);
	    const MuonSegment* assocSegm = new MuonSegment( layPar->localPosition(),
							    dir,
							    segment->localCovariance(), new Trk::PlaneSurface(*plane),
							    meas,
							    new Trk::FitQuality( *(segment->fitQuality())),
							    segment->author()  );
	    // association
	    if (assocSegm) {  
	      bool found=false;
	      unsigned int ist = 0;
	      while ( !found && ist < m_allSegments->size() ) {
		if (detVol == (*m_allSegments)[ist]->first ) {
		  (*m_allSegments)[ist]->second->push_back(assocSegm);
		  found = true;          
		} 
		ist++;
	      } 
	      if (!found) {
		std::vector<const Trk::Segment*>* laySeg=new std::vector<const Trk::Segment*>;
		laySeg->push_back(assocSegm);
		StationSgs* assocPair = new StationSgs(detVol,laySeg);
		m_allSegments->push_back(assocPair);
	      }
	    }
	  }
	}
      }
    } 
  }
  return StatusCode::SUCCESS;
  
}

std::vector<std::pair<const Trk::Layer*,std::vector<const Trk::PrepRawData*>*>*>* Muon::MuonTGMeasAssocAlg::createMdtHitCollectionLayers() const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createHitCollectionLayers()" );
  
  StatusCode sc;

  //Define the pair and the vector of pairs
  std::vector<PairOfLayerPrd*>* vec_alllayer = new std::vector<PairOfLayerPrd*>;
  
  int layerid=0;
 
  //Try to retrieve the MDT hit collection
  const Muon::MdtPrepDataContainer* mdt_container;
  StatusCode sc_read = m_StoreGate->retrieve(mdt_container, m_inputMdtPrdCollection);
  if (sc_read.isFailure()) {
    ATH_MSG_ERROR( " Cannot retrieve MDT PrepData Container " );
    delete vec_alllayer;
    return NULL;
  }

  const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
  const DataHandle<Muon::MdtPrepDataCollection> lastColl;

  if (m_StoreGate->retrieve(mdtCollection,lastColl) ==StatusCode::SUCCESS) {
    for ( ; mdtCollection != lastColl ; ++mdtCollection ) {
       if (mdtCollection->size()>0) {
         for (Muon::MdtPrepDataCollection::const_iterator mdtPrd = mdtCollection->begin(); 
              mdtPrd != mdtCollection->end(); ++mdtPrd) {
           
           Identifier dig_id = (*mdtPrd)->identify();

           const Trk::Layer* layer = associatedLayer(0,dig_id);	   

	   if (!layer) {
	     ATH_MSG_ERROR( "MuonTGMeasAssocAlg::No layer associated with this MDT  hit! (digit = " << dig_id  << ")" );
	     ATH_MSG_ERROR( "station name,eta,phi" << m_mdtIdHelper->stationName(dig_id) << "," 
                                                         <<  m_mdtIdHelper->stationEta(dig_id) << "," 
                                                         << m_mdtIdHelper->stationPhi(dig_id) );
	     ATH_MSG_ERROR( "multilayer,layer,tube:" << m_mdtIdHelper->multilayer(dig_id) << "," <<
	                                                      m_mdtIdHelper->tubeLayer(dig_id) <<"," <<
                                                              m_mdtIdHelper->tube(dig_id) );
	     continue;
	   } 
 
           layerid = layer->layerType();           
		
	   //Check if the layer the hit is on already exists in the layer collection
	   bool stored = false;
	   std::vector<PairOfLayerPrd*>::const_iterator itr = vec_alllayer->begin();
	   for (; itr != vec_alllayer->end(); ++itr) {
	     //Compare the pointers
	     if ((*itr)->first->layerType() == layerid)
	       {
		 if (stored)
		   ATH_MSG_WARNING( "       Hit on two different layers! " );
		 stored = true;
		 //Add the hit to the layer, order hits by tube number
		 std::vector<const Trk::PrepRawData*>::iterator piter = (*itr)->second->begin();
                 while ( piter !=(*itr)->second->end() 
			 && m_mdtIdHelper->tube(dig_id)> m_mdtIdHelper->tube((*piter)->identify()) ) piter++;                
		 //(*itr)->second->push_back(new Muon::MdtPrepData(*(*mdtPrd)));
                 //(*itr)->second->insert(piter,new Muon::MdtPrepData(*(*mdtPrd)));
                 (*itr)->second->insert(piter,*mdtPrd);
	       }
	   }
	
	   //If it didn't exist, add a new pair to the layer collection
	   if (!stored) {
	     //Create a vector with the ID
	     std::vector<const Trk::PrepRawData*>* prd = new std::vector<const Trk::PrepRawData*>;
	     prd->push_back(*mdtPrd);
	  
	     //Create a new pair of Layer and PRD vector
	     PairOfLayerPrd* pair_layerhits = new PairOfLayerPrd(layer,prd);
	     vec_alllayer->push_back(pair_layerhits);
	   }	
	 }
       }
    }
  }

  //Return the object
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createMdtHitCollectionLayers() returns " << vec_alllayer->size() );
  return vec_alllayer;
}

std::vector<std::pair<const Trk::Layer*,std::vector<const Trk::PrepRawData*>*>*>* Muon::MuonTGMeasAssocAlg::createRpcHitCollectionLayers() const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createRpcHitCollectionLayers()" );
  
  StatusCode sc;

  //Define the pair and the vector of pairs
  std::vector<PairOfLayerPrd*>* vec_alllayer = 0;

  //Try to retrieve the RPC hit collection
  const Muon::RpcPrepDataContainer* rpc_container;
  StatusCode sc_read = m_StoreGate->retrieve(rpc_container, m_inputRpcPrdCollection);
  if (sc_read.isFailure()) {
    ATH_MSG_ERROR( " Cannot retrieve RPC PrepData Container " );
    return vec_alllayer;
  }
  
  const DataHandle<Muon::RpcPrepDataCollection> rpcCollection;
  const DataHandle<Muon::RpcPrepDataCollection> lastColl;

  vec_alllayer = new std::vector<PairOfLayerPrd*>;

  int layerid;

  if (m_StoreGate->retrieve(rpcCollection,lastColl) ==StatusCode::SUCCESS) {
    for ( ; rpcCollection != lastColl ; ++rpcCollection ) {
       if (rpcCollection->size()>0) {
         for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = rpcCollection->begin(); 
              rpcPrd != rpcCollection->end(); ++rpcPrd) {
           
           Identifier dig_id = (*rpcPrd)->identify();

           const Trk::Layer* layer = associatedLayer(1,dig_id);	   
	
	   if (!layer) {
	     ATH_MSG_ERROR( "       No layer associated with this RPC hit! (digit = " << dig_id  << ")" );
	     ATH_MSG_ERROR( "station name,eta,phi" << m_rpcIdHelper->stationName(dig_id) << "," 
                                                         << m_rpcIdHelper->stationEta(dig_id) << "," 
                                                         << m_rpcIdHelper->stationPhi(dig_id) );
             ATH_MSG_ERROR( "doubletR,doubletZ,doubletPhi,gasGap:"<<m_rpcIdHelper->doubletR(dig_id)<<","<<
			    m_rpcIdHelper->doubletZ(dig_id)<<","<<
			    m_rpcIdHelper->doubletPhi(dig_id) << ","<<
			    m_rpcIdHelper->gasGap(dig_id));
	     ATH_MSG_ERROR( "measuresPhi:"<<m_rpcIdHelper->measuresPhi(dig_id));
	     continue;
	   } 
 
           layerid = layer->layerType();           
	
	   //Check if the layer the hit is on already exists in the layer collection and if the hit is a new hit
	   bool stored = false;
	   bool digstored = false;
	   std::vector<PairOfLayerPrd*>::const_iterator itr = vec_alllayer->begin();
	   std::vector<const Trk::PrepRawData*>::const_iterator digitr;
	   for (; itr != vec_alllayer->end(); ++itr) {
	  
	     //Check for equivalent hits in the collection
	     for (digitr = (*itr)->second->begin(); digitr != (*itr)->second->end();++digitr) {
	       //Compare the found hit to the new hit
	       if ((*rpcPrd) == (*digitr)) {
		 stored = true;
		 digstored = true;
		 break;
	       }
	     }  
	     //Stop the process if the digit is not unique
	     if (digstored) {
	       break;
	     }
	  
	     //Compare the pointers of the layers
	     if ((*itr)->first->layerType() == layerid) {
	      if (stored)
		ATH_MSG_WARNING( "       Hit on two different layers! " );
	      stored = true;
	      //Add the hit to the layer
	      (*itr)->second->push_back(*rpcPrd);
	     }
	   }
	
	   //If it didn't exist, add a new pair to the layer collection
	   if (!stored) {
	     //Create a vector with the ID
	     std::vector<const Trk::PrepRawData*>* vec_id = new std::vector<const Trk::PrepRawData*>;
	     vec_id->push_back(*rpcPrd);
	  
	     //Create a new pair of Layer and ID vector
	     PairOfLayerPrd* pair_layerhits = new PairOfLayerPrd(layer,vec_id);
	     vec_alllayer->push_back(pair_layerhits);
	   }	
	 }
       }
    }
  }

  //Return the object
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createRpcHitCollectionLayers() returns " << vec_alllayer->size() );
  return vec_alllayer;
}

std::vector<std::pair<const Trk::Layer*,std::vector<const Trk::PrepRawData*>*>*>* Muon::MuonTGMeasAssocAlg::createCscHitCollectionLayers() const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createHitCollectionLayers()" );
  
  StatusCode sc;

  //Define the pair and the vector of pairs
  std::vector<PairOfLayerPrd*>* vec_alllayer = 0;

  int layerid;

  //Try to retrieve the CSC cluster collection
  const DataHandle<CscPrepDataContainer> csc_container ;
  StatusCode sc_read = m_StoreGate->retrieve(csc_container, m_inputCscPrdCollection);
  if (sc_read.isFailure()) {
    ATH_MSG_ERROR( " Cannot retrieve CSC Cluster Container " );
    return vec_alllayer;
  } else {
    vec_alllayer = new std::vector<PairOfLayerPrd*>;

    for ( CscPrepDataContainer::const_iterator icol=csc_container->begin();
          icol != csc_container->end(); ++icol ) {
      const CscPrepDataCollection* cscCollection = *icol;
      for (CscPrepDataCollection::const_iterator cscPrd = cscCollection->begin(); 
	   cscPrd != cscCollection->end(); ++cscPrd) {
           
	Identifier dig_id = (*cscPrd)->identify();

	const Trk::Layer* layer = associatedLayer(3,dig_id);	   
	
	if (!layer) {
	  ATH_MSG_ERROR( "       No layer associated with this CSC hit! (digit = " << dig_id  << ")" );
	  ATH_MSG_ERROR( "station name,eta,phi" << m_cscIdHelper->stationName(dig_id) << "," 
                                                      << m_cscIdHelper->stationEta(dig_id) << "," 
                                                      << m_cscIdHelper->stationPhi(dig_id) );
 	  ATH_MSG_ERROR( "chamberLayer,wireLayer,measuresPhi:" << m_cscIdHelper->chamberLayer(dig_id)<<","<<
                                                                        m_cscIdHelper->wireLayer(dig_id)<<","<<
                                                                        m_cscIdHelper->measuresPhi(dig_id)); 
	  continue;
	}
 
	layerid = layer->layerType();           
	
	//Check if the layer the hit is on already exists in the layer collection and if the hit is a new hit
	bool stored = false;
	bool digstored = false;
	std::vector<PairOfLayerPrd*>::const_iterator itr = vec_alllayer->begin();
	std::vector<const Trk::PrepRawData*>::const_iterator digitr;
	for (; itr != vec_alllayer->end(); ++itr) {
	  
	  //Check for equivalent hits in the collection
	  for (digitr = (*itr)->second->begin(); digitr != (*itr)->second->end();++digitr) {
	    //Compare the found hit to the new hit
	    if ((*cscPrd) == (*digitr)) {
	      stored = true;
	      digstored = true;
	      break;
	    }
	  }  
	  //Stop the process if the digit is not unique
	  if (digstored) {
	    break;
	  }
	  
	  //Compare the pointers of the layers
	  if ((*itr)->first->layerType() == layerid) {
	    if (stored)
	      ATH_MSG_WARNING( "       Hit on two different layers! " );
	    stored = true;
	    //Add the hit to the layer
	    (*itr)->second->push_back(*cscPrd);
	  }
	}
	
	if (!stored) {
	  //Create a vector with the ID
	  std::vector<const Trk::PrepRawData*>* vec_id = new std::vector<const Trk::PrepRawData*>;
	  vec_id->push_back(*cscPrd);
	  
	  //Create a new pair of Layer and ID vector
	  PairOfLayerPrd* pair_layerhits = new PairOfLayerPrd(layer,vec_id);
	  vec_alllayer->push_back(pair_layerhits);
	}	
      }
    }
  }
  
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createCscHitCollectionLayers() returns " << vec_alllayer->size() );
  return vec_alllayer;
}

std::vector<std::pair<const Trk::Layer*,std::vector<const Trk::PrepRawData*>*>*>* Muon::MuonTGMeasAssocAlg::createTgcHitCollectionLayers() const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createHitCollectionLayers():TGC" );
  
  StatusCode sc;

  //Define the pair and the vector of pairs
  std::vector<PairOfLayerPrd*>* vec_alllayer = 0;

  //Try to retrieve the TGC hit collection
  const Muon::TgcPrepDataContainer* tgc_container;
  StatusCode sc_read = m_StoreGate->retrieve(tgc_container, m_inputTgcPrdCollection);
  if (sc_read.isFailure()) {
    ATH_MSG_ERROR( " Cannot retrieve TGC PrepData Container " );
    return vec_alllayer;
  }
  
  const DataHandle<Muon::TgcPrepDataCollection> tgcCollection;
  const DataHandle<Muon::TgcPrepDataCollection> lastColl;

  vec_alllayer = new std::vector<PairOfLayerPrd*>;

  int layerid;

  if (m_StoreGate->retrieve(tgcCollection,lastColl) ==StatusCode::SUCCESS) {
    for ( ; tgcCollection != lastColl ; ++tgcCollection ) {
       if (tgcCollection->size()>0) {
         for (Muon::TgcPrepDataCollection::const_iterator tgcPrd = tgcCollection->begin(); 
              tgcPrd != tgcCollection->end(); ++tgcPrd) {
           
           Identifier dig_id = (*tgcPrd)->identify();

	   const Trk::Layer* layer = associatedLayer(2,dig_id);	   
	
	   if (!layer) {
	     ATH_MSG_ERROR( "       No layer associated with this TGC hit! (digit = " << dig_id  << ")" );
	     ATH_MSG_ERROR( "station name,eta,phi" << m_tgcIdHelper->stationName(dig_id) << "," 
		                                         << m_tgcIdHelper->stationEta(dig_id) << "," 
                                                         << m_tgcIdHelper->stationPhi(dig_id) );
	     ATH_MSG_ERROR( "gasGap,isStrip:" << m_tgcIdHelper->gasGap(dig_id)<<","<<
	                                               m_tgcIdHelper->isStrip(dig_id)); 
	     continue;
	   }
 
           layerid = layer->layerType();           
	
	   //Check if the layer the hit is on already exists in the layer collection and if the hit is a new hit
	   bool stored = false;
	   bool digstored = false;
	   std::vector<PairOfLayerPrd*>::const_iterator itr = vec_alllayer->begin();
	   std::vector<const Trk::PrepRawData*>::const_iterator digitr;
	   for (; itr != vec_alllayer->end(); ++itr) {
	  
	     //Check for equivalent hits in the collection
	     for (digitr = (*itr)->second->begin(); digitr != (*itr)->second->end();++digitr) {
	       //Compare the found hit to the new hit
	       if ((*tgcPrd) == (*digitr)) {
		 stored = true;
		 digstored = true;
		 break;
	       }
	     }  
	     //Stop the process if the digit is not unique
	     if (digstored) {
	       break;
	     }
	  
	     //Compare the pointers of the layers
	     if ((*itr)->first->layerType() == layerid) {
	      if (stored)
		ATH_MSG_WARNING( "       Hit on two different layers! " );
	      stored = true;
	      //Add the hit to the layer
	      (*itr)->second->push_back(*tgcPrd);
	     }
	   }
	
           if (!stored) {
	     //Create a vector with the ID
	     std::vector<const Trk::PrepRawData*>* vec_id = new std::vector<const Trk::PrepRawData*>;
	     vec_id->push_back(*tgcPrd);
	  
	     //Create a new pair of Layer and ID vector
	     PairOfLayerPrd* pair_layerhits = new PairOfLayerPrd(layer,vec_id);
	     vec_alllayer->push_back(pair_layerhits);
	   }	
	 }
       }
    }
  }
  
  ATH_MSG_DEBUG( "MuonTGMeasAssocAlg::createTgcHitCollectionLayers() returns " << vec_alllayer->size() );
  return vec_alllayer;
}


void Muon::MuonTGMeasAssocAlg::createStationMap(const Trk::TrackingVolume* vol) const
{
  if (vol->confinedVolumes()) {
    const std::vector<const Trk::TrackingVolume*> subVols = vol->confinedVolumes()->arrayObjects();
    std::vector<const Trk::TrackingVolume*>::const_iterator iter = subVols.begin();
    for (;iter!=subVols.end();iter++) createStationMap(*iter);
  }
  
  if (vol->confinedDetachedVolumes()) {
    const std::vector<const Trk::DetachedTrackingVolume*>* detVols = vol->confinedDetachedVolumes();
    std::vector<const Trk::DetachedTrackingVolume*>::const_iterator dter = detVols->begin();
    for (;dter!=detVols->end();dter++) {
      if ( (*dter)->layerRepresentation() && (*dter)->layerRepresentation()->layerType()>0 ) {
        Identifier id((*dter)->layerRepresentation()->layerType());
        const MuonGM::MuonStation* mStation = 0;
        if (m_mdtIdHelper->is_mdt(id)) mStation = m_muonMgr->getMdtReadoutElement(id)->parentMuonStation();      
        if (m_rpcIdHelper->is_rpc(id)) mStation = m_muonMgr->getRpcReadoutElement(id)->parentMuonStation();      
        if (m_tgcIdHelper->is_tgc(id)) {
          if ( !m_muonMgr->getTgcReadoutElement(id) ) {     // tgc readout element not found, get any active layer to recover
            const Trk::Layer* lay = associatedLayer((*dter)->trackingVolume(),Identifier(0));
            if (lay) mStation = m_muonMgr->getTgcReadoutElement(Identifier(lay->layerType()))->parentMuonStation();
          } else {
	    mStation = m_muonMgr->getTgcReadoutElement(id)->parentMuonStation();      
          }  
        }
        if (m_cscIdHelper->is_csc(id)) {
          if ( !m_muonMgr->getCscReadoutElement(id) ) {
            const Trk::Layer* lay = associatedLayer((*dter)->trackingVolume(),Identifier(0));
            if (lay) mStation = m_muonMgr->getCscReadoutElement(Identifier(lay->layerType()))->parentMuonStation();
         } else  mStation = m_muonMgr->getCscReadoutElement(id)->parentMuonStation();    
	} 
        // coming across station repeatedly 
        if (mStation && !m_stationMap[id].second )
	  m_stationMap[id] = std::pair<const MuonGM::MuonStation*,const Trk::DetachedTrackingVolume*>(mStation,*dter);
      }
    }
  }
}

void Muon::MuonTGMeasAssocAlg::misAlignStations() const
{
  std::map<Identifier,std::pair<const MuonGM::MuonStation*, const Trk::DetachedTrackingVolume*> >::iterator iter = m_stationMap.begin();

  for ( ;iter!=m_stationMap.end();iter++) {
    for (unsigned int mis = 0; mis<m_misAlignDiff.size(); mis++) {
      Identifier sId( iter->first);
      if ( sId == m_misAlignDiff[mis].first || !m_misAlignDiff[mis].first.get_identifier32().get_compact() ) {
	iter->second.second->move(m_misAlignDiff[mis].second);
	std::cout << "moving station :"<<iter->second.second->name() << " by " << m_misAlignDiff[mis].second.translation()<< std::endl;
	break;
      }
    }
  }  
}

void Muon::MuonTGMeasAssocAlg::reAlignStations() const
{
  std::map<Identifier,std::pair<const MuonGM::MuonStation*, const Trk::DetachedTrackingVolume*> >::iterator iter = m_stationMap.begin();

  unsigned int moved = 0;
  for ( ;iter!=m_stationMap.end();iter++) {  
    const Amg::Transform3D transfMS = Amg::CLHEPTransformToEigen(iter->second.first->getGeoTransform()->getTransform());
    const Amg::Transform3D transfTV = iter->second.second->trackingVolume()->transform();
    if ( !transfMS.isApprox(transfTV, 1e-3) ) {
      Amg::Transform3D shift = transfMS*transfTV.inverse();
      iter->second.second->move(shift);
      moved++;
    }
  }  
  ATH_MSG_DEBUG( moved <<  " stations realigned ");
}

const Trk::Layer* Muon::MuonTGMeasAssocAlg::associatedLayer(int techn, Identifier id) const
{
  const Trk::Layer* layer = 0;
  const MuonGM::MuonStation* mStation = 0;
  if ( techn==0 ) mStation = m_muonMgr->getMdtReadoutElement(id)->parentMuonStation();      
  if ( techn==1 ) mStation = m_muonMgr->getRpcReadoutElement(id)->parentMuonStation();      
  if ( techn==2 ) mStation = m_muonMgr->getTgcReadoutElement(id)->parentMuonStation();      
  if ( techn==3 ) mStation = m_muonMgr->getCscReadoutElement(id)->parentMuonStation();     

  if (!mStation ) ATH_MSG_ERROR( "no associated GM station found for hit id:"<< id << ","<< techn );
  if (!mStation ) return layer;

  //Identifier stId(m_muonTgTool->getStationId(id));
  Identifier stId = getStationId(id);

  const Trk::DetachedTrackingVolume* station = m_stationMap[stId].second;
  if (!station) {
    ATH_MSG_WARNING("no associated TG station found for hit id:"<< id << ","<< techn);
    if (techn==2 && m_allowGeomAssoc) {
      ATH_MSG_WARNING(m_tgcIdHelper->stationName(id)<<","<<m_tgcIdHelper->stationEta(id)<< ","<<m_tgcIdHelper->stationPhi(id));
    
      //Get the TgcReadoutElement and the tube position from it
      const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(id);                   
      Amg::Vector3D pos = tgcROE->channelPos(id);
      std::vector<const Trk::DetachedTrackingVolume*>* detVols = m_trackingGeometry->lowestDetachedTrackingVolumes(pos);
      for (unsigned int i = 0;i<detVols->size();i++) {
	Identifier sId((*detVols)[i]->layerRepresentation()->layerType());
	ATH_MSG_INFO("geom assoc with station:"<< (*detVols)[i]<<","<<(*detVols)[i]->name()<<","<<(*detVols)[i]->layerRepresentation()->layerType() <<":"<< m_tgcIdHelper->stationName(sId)<<","<<m_tgcIdHelper->stationEta(sId)
	    <<","<<m_tgcIdHelper->stationPhi(sId) );
	ATH_MSG_INFO("updating station map"); 
	m_stationMap[stId]=std::pair<const MuonGM::MuonStation*,const Trk::DetachedTrackingVolume*>(mStation,(*detVols)[i]);   
      }  
      station = m_stationMap[stId].second;
    }         
  }
  // associated layer
  if (station) {
    const Trk::Layer* lay = associatedLayer(station->trackingVolume(),id);
    if (lay) return lay;
  } 

  return layer; 
}

const Trk::Layer* Muon::MuonTGMeasAssocAlg::associatedLayer(const Trk::TrackingVolume* trVol, Identifier id) const
{
  // finds associated layer using identifier match; station already found
 
  const Trk::Layer* layer = 0;

  if (trVol->confinedLayers()) {    // MDT,TGC,CSC
    std::vector<const Trk::Layer*> confLays = trVol->confinedLayers()->arrayObjects();
    std::vector<const Trk::Layer*>::iterator iLay = confLays.begin();
    for ( ; iLay!=confLays.end(); iLay++ ) {
      Identifier idLay( (*iLay)->layerType() );
      if (!id.get_identifier32().get_compact() && idLay.get_identifier32().get_compact()) return (*iLay);      
      if (!idLay.get_identifier32().get_compact()) continue;
      if ( m_mdtIdHelper->is_mdt(id) &&
	   m_mdtIdHelper->multilayer(idLay)==m_mdtIdHelper->multilayer(id) &&
	   m_mdtIdHelper->tubeLayer(idLay)==m_mdtIdHelper->tubeLayer(id) ) return (*iLay);
      if ( m_tgcIdHelper->is_tgc(id) &&
	   m_tgcIdHelper->gasGap(idLay)==m_tgcIdHelper->gasGap(id) ) return (*iLay);
      if ( m_cscIdHelper->is_csc(id) &&
	   m_cscIdHelper->chamberLayer(idLay)==m_cscIdHelper->chamberLayer(id) &&
	   m_cscIdHelper->wireLayer(idLay)==m_cscIdHelper->wireLayer(id) ) return (*iLay);
    }
    return layer;
  }

  if (trVol->confinedArbitraryLayers()) {    // RPC
    const std::vector<const Trk::Layer*>* confLays = trVol->confinedArbitraryLayers();
    std::vector<const Trk::Layer*>::const_iterator iLay = confLays->begin();
    for ( ; iLay!=confLays->end(); iLay++ ) {
      Identifier idLay( (*iLay)->layerType() );
      if (!id.get_identifier32().get_compact() && idLay.get_identifier32().get_compact()) return (*iLay);
      if ( idLay.get_identifier32().get_compact()>0 && m_rpcIdHelper->is_rpc(id) &&
	   m_rpcIdHelper->doubletR(idLay) == m_rpcIdHelper->doubletR(id) &&
	   m_rpcIdHelper->doubletZ(idLay) == m_rpcIdHelper->doubletZ(id) &&
	   //&& m_rpcIdHelper->doubletPhi(idLay) == m_rpcIdHelper->doubletPhi(id) &&
	   m_rpcIdHelper->gasGap(idLay) == m_rpcIdHelper->gasGap(id) ) return (*iLay);
    }
    return layer;
  }

  if (trVol->confinedVolumes()) {
    std::vector<const Trk::TrackingVolume*> subVols=trVol->confinedVolumes()->arrayObjects();
    std::vector<const Trk::TrackingVolume*>::iterator iter=subVols.begin();
    for ( ; iter!=subVols.end(); iter++ ) {
      const Trk::Layer* lay = associatedLayer((*iter),id);
      if (lay) return lay;
    }
  } 
  return layer;
}

Identifier Muon::MuonTGMeasAssocAlg::getStationId( Identifier id ) const
{
  Identifier stId(0);
  if ( m_mdtIdHelper->is_mdt(id)) stId = m_mdtIdHelper->elementID(m_mdtIdHelper->stationName(id),
								  m_mdtIdHelper->stationEta(id),
								  m_mdtIdHelper->stationPhi(id));
  // BML station is an exception
  if (m_mdtIdHelper->is_mdt(id) && m_mdtIdHelper->stationName(id)==2)
                                  stId = m_rpcIdHelper->elementID(m_mdtIdHelper->stationName(id),
  								  m_mdtIdHelper->stationEta(id),
  								  m_mdtIdHelper->stationPhi(id),1);
             
  if ( m_rpcIdHelper->is_rpc(id)) stId = m_rpcIdHelper->elementID(m_rpcIdHelper->stationName(id),
  								  m_rpcIdHelper->stationEta(id),
  								  m_rpcIdHelper->stationPhi(id),1);

  // rpc not allways relevant    
  if ( m_rpcIdHelper->is_rpc(id) && m_stationMap.size() && !(m_stationMap)[stId].second ) {
                                  stId = m_mdtIdHelper->elementID(m_mdtIdHelper->stationName(id),
								  m_mdtIdHelper->stationEta(id),
								  m_mdtIdHelper->stationPhi(id) );
  }

  if ( m_tgcIdHelper->is_tgc(id)) stId = m_tgcIdHelper->elementID(m_tgcIdHelper->stationName(id),
								  m_tgcIdHelper->stationEta(id),
								  m_tgcIdHelper->stationPhi(id));
  if ( m_cscIdHelper->is_csc(id)) stId = m_cscIdHelper->elementID(m_cscIdHelper->stationName(id),
								  m_cscIdHelper->stationEta(id),
								  m_cscIdHelper->stationPhi(id));

  return stId;
}
