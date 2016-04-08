/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"

#include "TrigOnlineSpacePointTool/SpacePointConversionUtils.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigSpacePointConversionTool.h"

TrigSpacePointConversionTool::TrigSpacePointConversionTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ) : AthAlgTool(t,n,p),
								      m_layerNumberTool("TrigL2LayerNumberTool"), 
								      m_beamCondSvc(0) {
  declareInterface< ITrigSpacePointConversionTool >( this );

  declareProperty( "RegionSelectorService",  m_regionSelectorName = "RegSelSvc" );
  declareProperty( "PixelSP_ContainerName",  m_pixelSpContName = "PixelTrigSpacePoints" );
  declareProperty( "SCT_SP_ContainerName",   m_sctSpContName = "SCT_TrigSpacePoints" );
  declareProperty( "DoPhiFiltering",         m_filter_phi = true );
  declareProperty( "UseBeamTilt",            m_useBeamTilt = true );
  declareProperty( "UseNewLayerScheme",      m_useNewScheme = false );
  declareProperty( "layerNumberTool",        m_layerNumberTool);
}

StatusCode TrigSpacePointConversionTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();

  ATH_MSG_INFO("In initialize...");

  sc = serviceLocator()->service( m_regionSelectorName, m_regionSelector);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to retrieve RegionSelector Service  " << m_regionSelectorName);
    return sc;
  }

  sc=m_layerNumberTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve "<<m_layerNumberTool);
    return sc;
  }

  sc = detStore()->retrieve(m_atlasId, "AtlasID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get ATLAS ID helper"); 
    return sc;
  } 

  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper"); 
    return sc;
  } 

  sc = detStore()->retrieve(m_sctId, "SCT_ID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get SCT ID helper"); 
    return sc;
  } 
 
  sc = service("BeamCondSvc", m_beamCondSvc);
  if (sc.isFailure() || m_beamCondSvc == 0) {
    ATH_MSG_FATAL("Could not retrieve Beam Conditions Service. ");
    return sc;
  }

  ATH_MSG_INFO("TrigSpacePointConversionTool initialized ");

  return sc;
}

StatusCode TrigSpacePointConversionTool::finalize() {

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


StatusCode TrigSpacePointConversionTool::getSpacePoints(const IRoiDescriptor& internalRoI, 
							std::vector<TrigSiSpacePointBase>& output, int& nPix, int& nSct) {

  output.clear();
  nPix = 0;
  nSct = 0;

  StatusCode sc = retrieveSpacePointsContainers();
  if(sc.isFailure()) return sc;

  std::vector<IdentifierHash> listOfPixIds;
  std::vector<IdentifierHash> listOfSctIds;
        
  m_regionSelector->DetHashIDList(PIXEL, internalRoI, listOfPixIds); 
  m_regionSelector->DetHashIDList(SCT, internalRoI, listOfSctIds); 

  int offsets[3];

  offsets[0] = m_layerNumberTool->offsetEndcapPixels();
  offsets[1] = m_layerNumberTool->offsetBarrelSCT();
  offsets[2] = m_layerNumberTool->offsetEndcapSCT();
    
  FTF::LayerCalculator lc(m_atlasId, m_pixelId, m_sctId, offsets);
    
  //filter spacepoints to reject those beyound internalRoI boundaries
    
  FTF::RoI_Filter filter(output, lc, &internalRoI, m_filter_phi);
  FTF::SpacePointSelector<FTF::RoI_Filter> selector(filter);
  
  if(m_useNewScheme) {
    nPix=selector.select(m_pixelSpacePointsContainer,listOfPixIds, m_layerNumberTool->pixelLayers());
    nSct=selector.select(m_sctSpacePointsContainer,listOfSctIds, m_layerNumberTool->sctLayers());
  }
  else {
    nPix=selector.select(m_pixelSpacePointsContainer,listOfPixIds);
    nSct=selector.select(m_sctSpacePointsContainer,listOfSctIds);
  }
  if(!m_useBeamTilt) shiftSpacePoints(output);
  else transformSpacePoints(output);

  return StatusCode::SUCCESS;
}


StatusCode TrigSpacePointConversionTool::retrieveSpacePointsContainers() {

  StatusCode sc=evtStore()->retrieve(m_pixelSpacePointsContainer,m_pixelSpContName);
  if(sc.isFailure()) {
    ATH_MSG_WARNING("Pixel SP container " <<m_pixelSpContName <<" not found"); 
    return sc;
  }
  sc=evtStore()->retrieve(m_sctSpacePointsContainer,m_sctSpContName);
  if(sc.isFailure()) {
    ATH_MSG_WARNING("SCT SP container " <<m_sctSpContName <<" not found"); 
    return sc;
  }
  return sc;
}

void TrigSpacePointConversionTool::shiftSpacePoints(std::vector<TrigSiSpacePointBase>& output) {
  
  Amg::Vector3D vertex = m_beamCondSvc->beamPos();
  double shift_x = vertex.x() - m_beamCondSvc->beamTilt(0)*vertex.z();
  double shift_y = vertex.y() - m_beamCondSvc->beamTilt(1)*vertex.z();

  std::for_each(output.begin(), output.end(), FTF::SpacePointShifter(shift_x, shift_y));

}


void TrigSpacePointConversionTool::transformSpacePoints(std::vector<TrigSiSpacePointBase>& output) {

  Amg::Vector3D origin = m_beamCondSvc->beamPos();
  double tx = tan(m_beamCondSvc->beamTilt(0));
  double ty = tan(m_beamCondSvc->beamTilt(1));

  double phi   = atan2(ty,tx);
  double theta = acos(1.0/sqrt(1.0+tx*tx+ty*ty));
  double sint = sin(theta);
  double cost = cos(theta);
  double sinp = sin(phi);
  double cosp = cos(phi);
  
  std::array<float, 4> xtrf, ytrf, ztrf;

  xtrf[0] = float(origin.x());
  xtrf[1] = float(cost*cosp*cosp+sinp*sinp);
  xtrf[2] = float(cost*sinp*cosp-sinp*cosp);
  xtrf[3] =-float(sint*cosp);
  
  ytrf[0] = float(origin.y());
  ytrf[1] = float(cost*cosp*sinp-sinp*cosp);
  ytrf[2] = float(cost*sinp*sinp+cosp*cosp);
  ytrf[3] =-float(sint*sinp);
  
  ztrf[0] = float(origin.z());
  ztrf[1] = float(sint*cosp);
  ztrf[2] = float(sint*sinp);
  ztrf[3] = float(cost);

  std::for_each(output.begin(), output.end(), FTF::SpacePointTransform(xtrf, ytrf, ztrf));

}
