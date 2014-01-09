/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "RpcCalibTools/RpcExtrapolationTool.h"
#include "RpcCalibTools/RpcResidualsTool.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonTGRecTools/MuonTGMeasurementTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "GaudiKernel/StatusCode.h"

static const InterfaceID IID_IRpcResidualsTool("RpcResidualsTool", 1, 0);

const InterfaceID& RpcResidualsTool::interfaceID( ) { 
  return IID_IRpcResidualsTool; 
}

RpcResidualsTool::RpcResidualsTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AlgTool(type,name,parent),
     m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
{
  declareProperty("ClusterCollection",m_clusCollection="rpcClusters");
  declareInterface< RpcResidualsTool  >( this );
}


// destructor   
RpcResidualsTool::~RpcResidualsTool() {
  
}


StatusCode RpcResidualsTool::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()" << endreq;

   StatusCode sc;
  
  // Event store service
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc.isFailure()) 
    {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return  sc;
    }

  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    log << MSG::ERROR << " Cannot get ActiveStoreSvc " << endreq;
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure())   {
    log << MSG::ERROR << "Can't locate the DetectorStore" << endreq; 
    return sc;
  }

    // Retrieve the MuonDetectorManager
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure())
    {
      log << MSG::ERROR 
	  << "Can't retrieve MuonDetectorManager from detector store" << endreq;
      return sc;
    }

  // get extrapolation tool

  sc = toolSvc()->retrieveTool("RpcExtrapolationTool",m_rpcExtrapolTool);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Can't get handle on the rpc extrapolation tool" << endreq;
    return StatusCode::FAILURE;
  } 


  sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Can't retrieve RpcIdHelper" << endreq;
      return sc;
    }

  sc = m_idHelperTool.retrieve();
  if (sc.isSuccess()){
    log<<MSG::INFO << "Retrieved " << m_idHelperTool << endreq;
  }else{
    log<<MSG::FATAL<<"Could not get " << m_idHelperTool <<endreq; 
    return sc;
  }
  
  return StatusCode::SUCCESS;  

}

StatusCode RpcResidualsTool::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}

void RpcResidualsTool::getRpcResiduals(TrackCollection::const_iterator theTrack, std::vector<RpcResiduals> &theResult){
  MsgStream log(msgSvc(), name());

  StatusCode sc;

  // assume the result vector has been cleaned before passing it to us

  // start by calling the extrapolator tool

  // extrapolations of the track ill be stored here

  std::vector<RpcExtrapolationResults> extrapolations;

  m_rpcExtrapolTool->getRpcIntersections(theTrack, extrapolations);
  
//   if (extrapolations.size()%2){

//     log << MSG::WARNING << "Odd number of extrapolations" << endreq;
    
//     for (int i=0; i<extrapolations.size(); ++i){
      
//       Identifier idTwinView=GetTwinViewPanel(extrapolations[i].id);

//       bool found = false;
      
//       for (int j=0; j<extrapolations.size()&&!found; ++j){
      
// 	if (m_rpcIdHelper->panelID(extrapolations[j].id)==idTwinView) found = true;
	
//       }

//       if (!found) {

// 	log << MSG::WARNING << "ID " << extrapolations[i].id<<" name " <<m_rpcIdHelper->stationName(extrapolations[i].id)<< " sec "<<m_rpcIdHelper->stationPhi(extrapolations[i].id)*2+m_rpcIdHelper->stationName(extrapolations[i].id)%2-1<< " eta "<<m_rpcIdHelper->stationEta(extrapolations[i].id)<<" dZ " <<m_rpcIdHelper->doubletZ(extrapolations[i].id)<<" dPhi " <<m_rpcIdHelper->doubletPhi(extrapolations[i].id)<< " r "<<m_rpcIdHelper->doubletR(extrapolations[i].id)<< " gg "<<m_rpcIdHelper->gasGap(extrapolations[i].id)<< " mp "<<m_rpcIdHelper->measuresPhi(extrapolations[i].id)<<" strip "<<m_rpcIdHelper->strip(extrapolations[i].id)<< " found, but "<<endreq;

// 	log << MSG::WARNING << "ID " << idTwinView<<" name " <<m_rpcIdHelper->stationName(idTwinView)<< " sec "<<m_rpcIdHelper->stationPhi(idTwinView)*2+m_rpcIdHelper->stationName(idTwinView)%2-1<< " eta "<<m_rpcIdHelper->stationEta(idTwinView)<<" dZ " <<m_rpcIdHelper->doubletZ(idTwinView)<<" dPhi " <<m_rpcIdHelper->doubletPhi(idTwinView)<< " r "<<m_rpcIdHelper->doubletR(idTwinView)<< " gg "<<m_rpcIdHelper->gasGap(idTwinView)<< " mp "<<m_rpcIdHelper->measuresPhi(idTwinView)<<" strip "<<m_rpcIdHelper->strip(idTwinView)<<" missing"<<endreq;
	
//       }
      
      
//     }   
    
//   }
  
  // get cluster container
  const Muon::RpcPrepDataContainer* rpc_container;
  sc = (*m_activeStore)->retrieve(rpc_container,m_clusCollection);
  if (sc.isFailure()) {
    log << MSG::WARNING << " Cannot retrieve RpcPrepDataContainer " << endreq;
    return;
  }
  
  
  std::set<Identifier> panels;

  // start first loop over clusters


  if(rpc_container->size()){

    Muon::RpcPrepDataContainer::const_iterator it = rpc_container->begin();
    

    for(;it!=rpc_container->end(); ++it){
      
      const Muon::RpcPrepDataCollection* clusterCollection = *it;
      
      //      if(clusterCollection->identify()==theElement&&clusterCollection->size()>0){
      if(clusterCollection->size()>0){
	
    // there is a collection for the element. now have to loop over it
   
	for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = clusterCollection->begin(); 
	     rpcPrd != clusterCollection->end(); ++rpcPrd) {

	  panels.insert(m_rpcIdHelper->panelID((*rpcPrd)->identify()));
	  
  
	}
      }
    }
  }
  
  // now loop over extrapolations:
  
  std::vector<RpcExtrapolationResults>::const_iterator theExtra=extrapolations.begin();

  for(;theExtra!=extrapolations.end();++theExtra){
    
    // for each extrapolation, want to keep only lowest residual
    
    float lowest=-999;
    int lowest_csize=-999;
    float pitch=-999;
    float lowest_time=-999;

    // create here residuals to be returned to caller. give proper panel id, and default (i.e. inefficient) residual
    RpcResiduals result;    

    result.id=(*theExtra).id;

    result.hasTwin=0;
    
    if(panels.find(GetTwinPanel(result.id))!=panels.end()) result.hasTwin=1;  
    
    // get the position from extrapolation

    Amg::Vector3D thePosition=(*theExtra).params->position();

    Identifier theElement;
    Identifier idParent;

    theElement=m_rpcIdHelper->elementID((*theExtra).id);
    idParent = m_rpcIdHelper->parentID((*theExtra).id);

    IdContext rpcModuleContext = m_rpcIdHelper->module_context();

    IdentifierHash elementIDhash;

    m_rpcIdHelper->get_hash(idParent, elementIDhash,&rpcModuleContext);
    
    Muon::RpcPrepDataContainer::const_iterator it = rpc_container->indexFind(elementIDhash);
    
    if(it!=rpc_container->end()){

      const Muon::RpcPrepDataCollection* clusterCollection = *it;
     
      if(clusterCollection->size()>0){

	// there is a collection for the element. now have to loop over it
	const Muon::RpcPrepDataCollection* clusterCollection = *it;
	
	for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = clusterCollection->begin(); 
	     rpcPrd != clusterCollection->end(); ++rpcPrd) {
	  
//	  int layer=0;
	  
	  // BM stations
	  /*if(m_rpcIdHelper->stationName((*rpcPrd)->identify())<4||m_rpcIdHelper->stationName((*rpcPrd)->identify())==8){
	    
	    layer=m_rpcIdHelper->doubletR((*rpcPrd)->identify())*2+m_rpcIdHelper->gasGap((*rpcPrd)->identify())-2;
	    
	  } else { // BO stations
	    
	    layer=4+m_rpcIdHelper->gasGap((*rpcPrd)->identify());
	    
	  }*/
	  
	  // look for cluster on the same panel
	  
	  Identifier panel_id = m_rpcIdHelper->panelID((*rpcPrd)->identify());
	  
	  // check if the cluster is on the same panel as the extrapolation
	  
	  if(panel_id==m_rpcIdHelper->panelID(result.id)){
	    
	    const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement((*rpcPrd)->identify());
	    
	    // eta residual
	    float residual=0.;
	    if(! m_rpcIdHelper->measuresPhi(result.id)) residual= thePosition.z()-(*rpcPrd)->globalPosition().z();
	    else {
	      int sign=(thePosition.phi() < (*rpcPrd)->globalPosition().phi()) ? -1 : 1;
	      residual=sign * sqrt(
				   pow(thePosition.x()-(*rpcPrd)->globalPosition().x(),2) +
				   pow(thePosition.y()-(*rpcPrd)->globalPosition().y(),2)
				   );
	    }
	    
	    if(fabs(residual)<fabs(lowest)){
	      lowest=residual;
	      lowest_time=(*rpcPrd)->time();
	      lowest_csize=(*rpcPrd)->rdoList().size();
	      pitch=descriptor->StripPitch(m_rpcIdHelper->measuresPhi(result.id));
	    }
	  }
	  
	} // end loop clusters
      
	
      } // end if collection id and size
      
      
    } // end for collections
    
    
    // now store residual value. will be default, or the lowest if clusters were found

    result.residual=lowest;
    result.csize=lowest_csize;
    result.time=lowest_time;
    result.pitch=pitch;
    result.params=(*theExtra).params;
   
    // give back to caller what we have found

    theResult.push_back(result);
    
  } // end loop extrapolations
  
}

Identifier RpcResidualsTool::GetTwinPanel(Identifier id){
  
  //unpack the panelID
      
  int sec = m_rpcIdHelper->stationPhi(id);
  int sta = m_rpcIdHelper->stationName(id);
  int dr = m_rpcIdHelper->doubletR(id);
  int dp = m_rpcIdHelper->doubletPhi(id);
  int mp = m_rpcIdHelper->measuresPhi(id);
  int gg = m_rpcIdHelper->gasGap(id);
  int dz = m_rpcIdHelper->doubletZ(id);
  int eta = m_rpcIdHelper->stationEta(id);
  
  return m_rpcIdHelper->panelID(sta, eta, sec, dr, dz, dp, gg%2 + 1, mp);
  
}

Identifier RpcResidualsTool::GetTwinViewPanel(Identifier id){
  
  //unpack the panelID
      
  int sec = m_rpcIdHelper->stationPhi(id);
  int sta = m_rpcIdHelper->stationName(id);
  int dr = m_rpcIdHelper->doubletR(id);
  int dp = m_rpcIdHelper->doubletPhi(id);
  int mp = m_rpcIdHelper->measuresPhi(id);
  int gg = m_rpcIdHelper->gasGap(id);
  int dz = m_rpcIdHelper->doubletZ(id);
  int eta = m_rpcIdHelper->stationEta(id);
  
  return m_rpcIdHelper->panelID(sta, eta, sec, dr, dz, dp, gg, !mp);
  
}
