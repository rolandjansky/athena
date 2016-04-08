/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "RpcCalibTools/RpcExtrapolationTool.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonTGRecTools/MuonTGMeasurementTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h" 

static const InterfaceID IID_IRpcExtrapolationTool("RpcExtrapolationTool", 1, 0);

const InterfaceID& RpcExtrapolationTool::interfaceID( ) { 
  return IID_IRpcExtrapolationTool; 
}

RpcExtrapolationTool::RpcExtrapolationTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
     m_propagator("Trk::StraightLinePropagator/MuonStraightLinePropagator"),
     m_navigator("Trk::Navigator/MuonNavigator"),
     m_extrapolator("Trk::Extrapolator/MuonSTEP_Extrapolator"),
     m_measTool("Muon::MuonTGMeasurementTool/MuonTGMeasurementTool"),
     m_holesTool("Muon::MuonHolesOnTrackTool/MuonHolesOnTrackTool")

{
  declareInterface< RpcExtrapolationTool  >( this );
  declareProperty("CosmicMode",m_cosmicMode=false);
}


// destructor   
RpcExtrapolationTool::~RpcExtrapolationTool() {
  
}


StatusCode RpcExtrapolationTool::initialize(){

  ATH_MSG_DEBUG( "in initialize()" );

    // Retrieve the MuonDetectorManager
  ATH_CHECK( detStore()->retrieve(m_muonMgr) );

  // Get the Navigation AlgTools
  ATH_CHECK( m_navigator.retrieve() );

  ATH_CHECK( detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER") );

  // get extrapolator
  ATH_CHECK( m_extrapolator.retrieve() );

  ATH_CHECK( m_idHelperTool.retrieve() );
  
  ATH_CHECK( m_holesTool.retrieve() );

  return StatusCode::SUCCESS;  

}

StatusCode RpcExtrapolationTool::finalize()
{
  ATH_MSG_INFO( "in finalize()" );

  return StatusCode::SUCCESS;
}

void RpcExtrapolationTool::getRpcIntersections(TrackCollection::const_iterator theTrack, std::vector<RpcExtrapolationResults> &theResult){

  // keep track here of intersected panels, so we don't double-count them

  std::map<Identifier,RpcExtrapolationResults> panels;

  // assume the result vector has been cleaned before passing it to us
  
  // get tsos

  const DataVector<const Trk::TrackStateOnSurface>*  tSoS =  m_holesTool->getHolesOnTrack(*(*theTrack) , Trk::muon);

 // now loop over holes
    
  if (tSoS) {

    DataVector<const Trk::TrackStateOnSurface>::const_iterator  iter = tSoS->begin();
    
    iter = tSoS->begin();

    for (;iter!=tSoS->end();iter++) {

      const Trk::TrackParameters* par=(*iter)->trackParameters();

      if(par){	
	
	const Trk::Surface & surf=par->associatedSurface();
	
	Identifier idEl = surf.associatedDetectorElementIdentifier();
	
	if (idEl.get_compact()){
	  
	  if(m_idHelperTool->isRpc(idEl)){
	    
	    if (m_rpcIdHelper->stationName(idEl)==3){

	      // from hole parameters get associated TG layer:
	      const Trk::Layer* lay =m_extrapolator->trackingGeometry()->associatedLayer(par->position());

	      // extrapolate hole track parameters to the associated TG layer:
	      const Trk::TrackParameters* layPar =m_extrapolator->extrapolate(*par,lay->surfaceRepresentation(),Trk::anyDirection,false,Trk::muon);

	      // get the nearest channel identifier using MuonTGMeasurementTool
	      double pitch=0.;
	      Identifier id = m_measTool->nearestDetEl(lay,layPar,m_rpcIdHelper->measuresPhi(idEl),pitch);

	      idEl=id;

	    }

	    Identifier panelID=m_rpcIdHelper->panelID(idEl);
	    
	    std::map<Identifier,RpcExtrapolationResults>::iterator it=panels.find(panelID);
	    
	    if(it==panels.end()){

	      // new panel
	      RpcExtrapolationResults thisResult;
	      thisResult.params=(*iter)->trackParameters();
	      thisResult.id=idEl;

	      panels.insert(std::map<Identifier,RpcExtrapolationResults>::value_type(panelID, thisResult) );
	      
	    } else {
	      
	      ATH_MSG_WARNING( "Found TWO HOLES on the same layer. don't know what to do... ignoring the second" );
	    }
	    
	  }
	  
	} else {

	  ATH_MSG_WARNING( "Hole has no associated detEl, trying to recover" );
	  
	  Amg::Vector3D pos=par->position();

	  const Trk::Layer* layer =  m_extrapolator->trackingGeometry()->associatedLayer(pos);
	    
	  if(layer){
	    
	    Identifier id(layer->layerType());
	    
	    if(m_idHelperTool->isRpc(id)){
	      
	      double pitch;
	      const Identifier  id_phi = m_measTool->nearestDetEl(layer,(*iter)->trackParameters() , true, pitch) ;
	      const Identifier  id_eta = m_measTool->nearestDetEl(layer,(*iter)->trackParameters() , false, pitch) ;
	      
	      Identifier panelEtaID=m_rpcIdHelper->panelID(id_eta);
	      Identifier panelPhiID=m_rpcIdHelper->panelID(id_phi);

	      // start with eta panel:
	      std::map<Identifier,RpcExtrapolationResults>::iterator it=panels.find(panelEtaID);
	      
	      if(it==panels.end()){
		
		// new panel
		RpcExtrapolationResults thisResult;
		
		thisResult.id=id_eta;
		thisResult.params=(*iter)->trackParameters();

		panels.insert(std::map<Identifier,RpcExtrapolationResults>::value_type(panelEtaID, thisResult) );
		
	      } else {
		ATH_MSG_WARNING( "Found TWO HOLES on the same layer. don't know what to do... ignoring the second" );

	      }

	      // repeat for phi
	      it=panels.find(panelPhiID);
	      
	      if(it==panels.end()){
		
		// new panel
		RpcExtrapolationResults thisResult;
		
		thisResult.id=id_phi;
		thisResult.params=(*iter)->trackParameters();
		
		panels.insert(std::map<Identifier,RpcExtrapolationResults>::value_type(panelPhiID, thisResult) );
		
	      } else {
		ATH_MSG_WARNING( "Found TWO HOLES on the same layer. don't know what to do... ignoring the second" );
		
	      }
	      
	    }
	    
	  }
	  
	  
	} 
	  
      } else ATH_MSG_WARNING( "TSOS has no parameter" );     
    }
    
  }  else ATH_MSG_WARNING( "Hole finding returns null pointer" );     
  
  // now loop over measurements
  
  tSoS= (*theTrack)->trackStateOnSurfaces();
  
  int count=0;
  
  if (!tSoS){
    
    count++;
    theResult.clear();
    return;

  } else {
    
    DataVector<const Trk::TrackStateOnSurface>::const_iterator  iter = tSoS->begin();
    
    for (;iter!=tSoS->end();iter++) {
      
      if  ((*iter)->type(Trk::TrackStateOnSurface::Measurement)||(*iter)->type(Trk::TrackStateOnSurface::Outlier)) {
	
	const Trk::RIO_OnTrack* hit = dynamic_cast<const Trk::RIO_OnTrack*> ((*iter)->measurementOnTrack());
	if  ( hit == NULL) {
	  ATH_MSG_WARNING( "The measurementOnTrack is not a RIO_OnTrack ... skipping" );
	  continue;
	}
	const Identifier idHit = hit->identify();
	
	const Trk::TrackParameters* par=(*iter)->trackParameters();
	
	if(par){
	  	  
	  if(m_idHelperTool->isRpc(idHit)){
	     
	    Identifier panelID=m_rpcIdHelper->panelID(idHit);
	    
	    std::map<Identifier,RpcExtrapolationResults>::iterator it=panels.find(panelID);
	    
	    if(it==panels.end()){
	      
	      // new panel
	      RpcExtrapolationResults thisResult;
	      thisResult.id=idHit;
	      
	      thisResult.params=(*iter)->trackParameters();
	      	      
	      panels.insert(std::map<Identifier,RpcExtrapolationResults>::value_type(panelID, thisResult) );
	      
	    } else {
	           
	      (*it).second.params=(*iter)->trackParameters();
	      (*it).second.id=idHit;
	      
	    }
	    

	    
	  } //close the control is an RPC hit
	} else 	ATH_MSG_WARNING( "TSOS has no parameter" );
	
      }    
      
    }
  }
  
  // now push the residuals into the final vector
  
  std::map<Identifier,RpcExtrapolationResults>::iterator it=panels.begin();
  
  for(;it!=panels.end();++it) theResult.push_back((*it).second);
  
  
  //std::cout<<"end of extrapolation search for this track "<<theResult.size()<<std::endl;
  
}
