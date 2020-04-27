/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcCalibTools/RpcExtrapolationTool.h"
#include "RpcCalibTools/RpcResidualsTool.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
#include "MuonTGRecTools/MuonTGMeasurementTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

static const InterfaceID IID_IRpcResidualsTool("RpcResidualsTool", 1, 0);

const InterfaceID& RpcResidualsTool::interfaceID( ) { 
  return IID_IRpcResidualsTool; 
}

RpcResidualsTool::RpcResidualsTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent)
{
  declareProperty("ClusterCollection",m_clusCollection="rpcClusters");
  declareInterface< RpcResidualsTool  >( this );
}

StatusCode RpcResidualsTool::initialize(){

  ATH_MSG_DEBUG( "in initialize()" );

  // retrieve the active store
  ATH_CHECK( serviceLocator()->service("ActiveStoreSvc", m_activeStore) );

  //retrieve detector manager from the conditions store
  ATH_CHECK(m_DetectorManagerKey.initialize());

  // get extrapolation tool

  ATH_CHECK( toolSvc()->retrieveTool("RpcExtrapolationTool",m_rpcExtrapolTool) );

  ATH_CHECK( m_idHelperSvc.retrieve() );
  
  return StatusCode::SUCCESS;  

}

void RpcResidualsTool::getRpcResiduals(TrackCollection::const_iterator theTrack, std::vector<RpcResiduals> &theResult){

  // assume the result vector has been cleaned before passing it to us

  // start by calling the extrapolator tool

  // extrapolations of the track ill be stored here

  std::vector<RpcExtrapolationResults> extrapolations;

  m_rpcExtrapolTool->getRpcIntersections(theTrack, extrapolations);
    
  // get cluster container
  const Muon::RpcPrepDataContainer* rpc_container;
  StatusCode sc = (*m_activeStore)->retrieve(rpc_container,m_clusCollection);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( " Cannot retrieve RpcPrepDataContainer " );
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

	  panels.insert(m_idHelperSvc->rpcIdHelper().panelID((*rpcPrd)->identify()));
	  
  
	}
      }
    }
  }
  
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return; 
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

    theElement=m_idHelperSvc->rpcIdHelper().elementID((*theExtra).id);
    idParent = m_idHelperSvc->rpcIdHelper().parentID((*theExtra).id);

    IdContext rpcModuleContext = m_idHelperSvc->rpcIdHelper().module_context();

    IdentifierHash elementIDhash;

    m_idHelperSvc->rpcIdHelper().get_hash(idParent, elementIDhash,&rpcModuleContext);
    
    Muon::RpcPrepDataContainer::const_iterator it = rpc_container->indexFind(elementIDhash);
    
    if(it!=rpc_container->end()){

      const Muon::RpcPrepDataCollection* clusterCollection = *it;
     
      if(clusterCollection->size()>0){

	// there is a collection for the element. now have to loop over it
	const Muon::RpcPrepDataCollection* clusterCollection = *it;
	
	for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = clusterCollection->begin(); 
	     rpcPrd != clusterCollection->end(); ++rpcPrd) {
	  
	  // look for cluster on the same panel
	  
	  Identifier panel_id = m_idHelperSvc->rpcIdHelper().panelID((*rpcPrd)->identify());
	  
	  // check if the cluster is on the same panel as the extrapolation
	  
	  if(panel_id==m_idHelperSvc->rpcIdHelper().panelID(result.id)){
	    
	    const MuonGM::RpcReadoutElement* descriptor = MuonDetMgr->getRpcReadoutElement((*rpcPrd)->identify());
	    
	    // eta residual
	    float residual=0.;
	    if(! m_idHelperSvc->rpcIdHelper().measuresPhi(result.id)) residual= thePosition.z()-(*rpcPrd)->globalPosition().z();
	    else {
	      int sign=(thePosition.phi() < (*rpcPrd)->globalPosition().phi()) ? -1 : 1;
	      residual=sign * std::sqrt(
				   std::pow(thePosition.x()-(*rpcPrd)->globalPosition().x(),2) +
				   std::pow(thePosition.y()-(*rpcPrd)->globalPosition().y(),2)
				   );
	    }
	    
	    if(std::abs(residual)<std::abs(lowest)){
	      lowest=residual;
	      lowest_time=(*rpcPrd)->time();
	      lowest_csize=(*rpcPrd)->rdoList().size();
	      pitch=descriptor->StripPitch(m_idHelperSvc->rpcIdHelper().measuresPhi(result.id));
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
      
  int sec = m_idHelperSvc->rpcIdHelper().stationPhi(id);
  int sta = m_idHelperSvc->rpcIdHelper().stationName(id);
  int dr = m_idHelperSvc->rpcIdHelper().doubletR(id);
  int dp = m_idHelperSvc->rpcIdHelper().doubletPhi(id);
  int mp = m_idHelperSvc->rpcIdHelper().measuresPhi(id);
  int gg = m_idHelperSvc->rpcIdHelper().gasGap(id);
  int dz = m_idHelperSvc->rpcIdHelper().doubletZ(id);
  int eta = m_idHelperSvc->rpcIdHelper().stationEta(id);
  
  return m_idHelperSvc->rpcIdHelper().panelID(sta, eta, sec, dr, dz, dp, gg%2 + 1, mp);
  
}

Identifier RpcResidualsTool::GetTwinViewPanel(Identifier id){
  
  //unpack the panelID
      
  int sec = m_idHelperSvc->rpcIdHelper().stationPhi(id);
  int sta = m_idHelperSvc->rpcIdHelper().stationName(id);
  int dr = m_idHelperSvc->rpcIdHelper().doubletR(id);
  int dp = m_idHelperSvc->rpcIdHelper().doubletPhi(id);
  int mp = m_idHelperSvc->rpcIdHelper().measuresPhi(id);
  int gg = m_idHelperSvc->rpcIdHelper().gasGap(id);
  int dz = m_idHelperSvc->rpcIdHelper().doubletZ(id);
  int eta = m_idHelperSvc->rpcIdHelper().stationEta(id);
  
  return m_idHelperSvc->rpcIdHelper().panelID(sta, eta, sec, dr, dz, dp, gg, !mp);
  
}
