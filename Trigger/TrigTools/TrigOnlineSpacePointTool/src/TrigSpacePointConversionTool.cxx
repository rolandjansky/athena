/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"

#include "SpacePointConversionUtils.h"
#include "IRegionSelector/IRegSelSvc.h"

#include "IRegionSelector/IRegSelTool.h"

#include "TrigSpacePointConversionTool.h"



TrigSpacePointConversionTool::TrigSpacePointConversionTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ) : 
  AthAlgTool(t,n,p),
  m_layerNumberTool("TrigL2LayerNumberTool"),
  m_regsel_pix( "RegSelTool/RegSelTool_Pixel", this),
  m_regsel_sct( "RegSelTool/RegSel_SCT", this),
  m_regsel_ttem( "RegSelTool/RegSel_TTEM", this),
  m_regsel_tthec( "RegSelTool/RegSel_TTHEC", this),
  m_regsel_fcalem( "RegSelTool/RegSel_FCALEM", this),
  m_regsel_fcalhad( "RegSelTool/RegSel_FCALHAD", this)
{
  declareInterface< ITrigSpacePointConversionTool >( this );

  declareProperty( "RegionSelectorService",  m_regionSelectorName = "RegSelSvc" );
  declareProperty( "RegSel_Pixel",           m_regsel_pix);
  declareProperty( "RegSel_SCT",             m_regsel_sct);

  declareProperty( "RegSel_TTEM",             m_regsel_ttem);
  declareProperty( "RegSel_TTHEC",            m_regsel_tthec);
  declareProperty( "RegSel_FCALEM",           m_regsel_fcalem);
  declareProperty( "RegSel_FCALHAD",          m_regsel_fcalhad);

  declareProperty( "DoPhiFiltering",         m_filter_phi = true );
  declareProperty( "UseBeamTilt",            m_useBeamTilt = true );
  declareProperty( "UseNewLayerScheme",      m_useNewScheme = false );
  declareProperty( "layerNumberTool",        m_layerNumberTool);
  declareProperty( "PixelSP_ContainerName",  m_pixelSpacePointsContainerKey = std::string("PixelTrigSpacePoints"));
  declareProperty( "SCT_SP_ContainerName",   m_sctSpacePointsContainerKey = "SCT_TrigSpacePoints" );
}

StatusCode TrigSpacePointConversionTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();

  ATH_MSG_INFO("In initialize...");

  sc = serviceLocator()->service( m_regionSelectorName, m_regionSelector);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to retrieve RegionSelector Service  " << m_regionSelectorName);
    return sc;
  }

  ATH_CHECK(m_regsel_pix.retrieve());
  ATH_CHECK(m_regsel_sct.retrieve());

  ATH_CHECK(m_regsel_ttem.retrieve());
  ATH_CHECK(m_regsel_tthec.retrieve());
  ATH_CHECK(m_regsel_fcalem.retrieve());
  ATH_CHECK(m_regsel_fcalhad.retrieve());

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
 
  ATH_CHECK(m_beamSpotKey.initialize());

  ATH_CHECK(m_pixelSpacePointsContainerKey.initialize());
  ATH_CHECK(m_sctSpacePointsContainerKey.initialize());

  ATH_CHECK(m_pixelSpacePointsContainerKey.initialize());
  ATH_CHECK(m_sctSpacePointsContainerKey.initialize());

  ATH_MSG_INFO("TrigSpacePointConversionTool initialized ");

  return sc;
}

StatusCode TrigSpacePointConversionTool::finalize() {

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


#include "simpletimer.h"

StatusCode TrigSpacePointConversionTool::getSpacePoints(const IRoiDescriptor& internalRoI, 
							std::vector<TrigSiSpacePointBase>& output, int& nPix, int& nSct) const {

  output.clear();
  

  SG::ReadHandle<SpacePointContainer> pixelSpacePointsContainer(m_pixelSpacePointsContainerKey);
  ATH_CHECK(pixelSpacePointsContainer.isValid());
  SG::ReadHandle<SpacePointContainer> sctSpacePointsContainer(m_sctSpacePointsContainerKey);
  ATH_CHECK(sctSpacePointsContainer.isValid());

  std::vector<IdentifierHash> listOfPixIds;
  std::vector<IdentifierHash> listOfSctIds;
  
  m_regsel_pix->HashIDList( internalRoI, listOfPixIds );
  m_regsel_sct->HashIDList( internalRoI, listOfSctIds );



  {   
    double otimes[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    double ntimes[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
    DETID  detector[12] = { SCT, TTEM, TTHEC, FCALEM, FCALHAD, PIXEL, MDT, RPC, TGC, CSC, MM, STGC };
  
    std::string label[12] = { "SCT", "TTEM", "TTHEC", "FCALEM", "FCALHAD", "PIX", "MDT", "RPC", "TGC", "CSC", "MM", "sTGC" };
    
    std::cout << "SUTT:getSpacePoints() RegSelTools" << std::endl; 
    
    std::cout << "SUTT:RoiDescriptor " << internalRoI << std::endl;
    
    const IRegSelTool* rs[6] = { 
      &(*m_regsel_sct), 
      &(*m_regsel_ttem), 
      &(*m_regsel_tthec), 
      &(*m_regsel_fcalem), 
      &(*m_regsel_fcalhad), 
      &(*m_regsel_pix) 
    }; 
    
    static int iroi = 0;
    
    for ( int i=0 ; i<6 ; i++ ) { 
      
      std::cout << "SUTT::\ttool " << detector[i] << " " << label[i] << std::endl; 
      
      std::vector<IdentifierHash> hashidn;
      hashidn.clear();
      struct timeval ntimer = simpletimer_start();
      rs[i]->HashIDList( internalRoI, hashidn ); 
      ntimes[i] = simpletimer_stop( ntimer ); 
      
      
      std::vector<IdentifierHash> hashid;
      hashid.clear();
      struct timeval otimer = simpletimer_start();
      m_regionSelector->DetHashIDList( detector[i], internalRoI, hashid ); 
      otimes[i] = simpletimer_stop( otimer ); 
      
      
      std::cout << "SUTT::detector " << label[i] << "\t" 
		<< hashid.size() << "\t" 
		<< hashidn.size() 
		<< "\t\t ::\t\t time: " 
		<< otimes[i] << " ms \t" 
		<< ntimes[i] << " ms" << std::endl;
      
      std::cout << "SUTT::PLOT   " << iroi << "\t" << i << "\t" 
		<< hashid.size() << "\t" 
		<< hashidn.size() << "\t" 
		<< "\t\t" 
		<< otimes[i] << "\t" 
		<< ntimes[i] << std::endl;
      
      if ( hashidn.size() != hashid.size() ) { 
	std::cout << "difference in output : " <<  label[i] << std::endl;
	
	std::sort( hashid.begin(),  hashid.end() );
	std::sort( hashidn.begin(), hashidn.end() );
	
	for ( size_t j=0 ; j<hashid.size()  ; j++ ) std::cout << "\thashid  " << j << " " << hashid[j]  << std::endl;
	for ( size_t j=0 ; j<hashidn.size() ; j++ ) std::cout << "\thashidn " << j << " " << hashidn[j] << std::endl;
      }
      
    }

  }
   
  int offsets[3];

  offsets[0] = m_layerNumberTool->offsetEndcapPixels();
  offsets[1] = m_layerNumberTool->offsetBarrelSCT();
  offsets[2] = m_layerNumberTool->offsetEndcapSCT();
    
  FTF::LayerCalculator lc(m_atlasId, m_pixelId, m_sctId, offsets);
    
  //filter spacepoints to reject those beyound internalRoI boundaries
    
  FTF::RoI_Filter filter(output, lc, &internalRoI, m_filter_phi);
  FTF::SpacePointSelector<FTF::RoI_Filter> selector(filter);
  
  nPix = 0;
  nSct = 0;

  if(m_useNewScheme) {
    nPix=selector.select(*pixelSpacePointsContainer,listOfPixIds, m_layerNumberTool->pixelLayers());
    nSct=selector.select(*sctSpacePointsContainer,listOfSctIds, m_layerNumberTool->sctLayers());
  }
  else {
    nPix=selector.select(*pixelSpacePointsContainer,listOfPixIds);
    nSct=selector.select(*sctSpacePointsContainer,listOfSctIds);
  }
  if(!m_useBeamTilt) shiftSpacePoints(output);
  else transformSpacePoints(output);

  return StatusCode::SUCCESS;
}


void TrigSpacePointConversionTool::shiftSpacePoints(std::vector<TrigSiSpacePointBase>& output) const {

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };  
  const Amg::Vector3D &vertex = beamSpotHandle->beamPos();
  double shift_x = vertex.x() - beamSpotHandle->beamTilt(0)*vertex.z();
  double shift_y = vertex.y() - beamSpotHandle->beamTilt(1)*vertex.z();

  std::for_each(output.begin(), output.end(), FTF::SpacePointShifter(shift_x, shift_y));

}


void TrigSpacePointConversionTool::transformSpacePoints(std::vector<TrigSiSpacePointBase>& output) const {

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
  const Amg::Vector3D &origin = beamSpotHandle->beamPos();
  double tx = tan(beamSpotHandle->beamTilt(0));
  double ty = tan(beamSpotHandle->beamTilt(1));

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
