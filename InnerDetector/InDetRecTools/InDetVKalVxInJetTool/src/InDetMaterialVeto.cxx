/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Header include

#include "InDetVKalVxInJetTool/InDetMaterialVeto.h"
#include "GeoModelKernel/GeoTube.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"



namespace InDet {

  
  InDetMaterialVeto::InDetMaterialVeto(const BeamPipeDetectorManager* beamPipeMgr,
				       const InDetDD::PixelDetectorManager* pixelManager) :
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"InDetMaterialVeto")
  {

    ATH_MSG_INFO("Building material rejection map from BeamPipe and PixelDetectorManager");
    
    static constexpr int nbins_R = 76;
    double bins_R[nbins_R+1];
    for(unsigned int i=0; i<=15; i++) bins_R[i] = 2*i - 1.;          // 2 mm bin width below R=29 mm, centered on R=24 mm (beampipe)
    bins_R[16] = 30.; // 29-30 makes junction with pixel bins, veto IPT R=29.3 mm
    static constexpr double Rbinwidth_pixel = 6.;
    for(unsigned int i=1; i<=60; i++) bins_R[i+16] = 30 + Rbinwidth_pixel*i; // 6 mm bin width beyond R=30 mm
    
    static constexpr int nbins_Z = 1000;
    static constexpr double zmax = 3000.;
    static constexpr double zbinwidth = 2*zmax/nbins_Z;
    
    std::string mapName = "ITkMaterialMap";
    m_ITkPixMaterialMap = std::make_unique<TH2D>(mapName.c_str(),mapName.c_str(),nbins_Z,-zmax,zmax,nbins_R,bins_R); // x-axis = global z coordinates, -3240 mm to +3240 mm, 6 mm bin width / y-axis = global R coordinates, variable bin width
    
    
    // Retrieve the beam pipe radius from the SectionC03 volume
    
    double beamPipeRadius = 0.;
    PVConstLink beamPipeTopVolume =  beamPipeMgr->getTreeTop(0);
    const GeoLogVol* beamPipeLogVolume = beamPipeTopVolume->getLogVol();
    const GeoTube* beamPipeTube = 0;
    
    if (beamPipeLogVolume){
      beamPipeTube = dynamic_cast<const GeoTube*>(beamPipeLogVolume->getShape());
      if (beamPipeTube){
	
	for(unsigned int i=0;i<beamPipeTopVolume->getNChildVols();i++){
	  
	  if(beamPipeTopVolume->getNameOfChildVol(i)=="SectionC03"){
	    PVConstLink childTopVolume =  beamPipeTopVolume->getChildVol(i);
	    const GeoLogVol* childLogVolume = childTopVolume->getLogVol();
	    const GeoTube* childTube = 0;
	    
	    if (childLogVolume){
	      childTube = dynamic_cast<const GeoTube*>(childLogVolume->getShape());
	      if (childTube){
		beamPipeRadius = 0.5 * (childTube->getRMax()+childTube->getRMin());
	      }
	    }
	    
	    break; //Exit loop after SectionC03 is found
	  }
	  
	} // Loop over child volumes
	
      }
    } // if(beamPipeLogVolume)
    
    ATH_MSG_INFO("BeamPipeRadius used for material rejection="<<beamPipeRadius);
    
    // Fill map with beam pipe radius for all z
    for(double z = -zmax + 0.5*zbinwidth; z<zmax; z+=zbinwidth) m_ITkPixMaterialMap->Fill(z,beamPipeRadius);
    
    
    // Retrieve IPT radius
    // Labelled as anonymous volume so taken as pixel volume with smallest radius
    
    double IPTRadius = -1.;
    
    PVConstLink pixelTopVolume =  pixelManager->getTreeTop(0);
    
    for(unsigned int i=0;i<pixelTopVolume->getNChildVols();i++){
      
      PVConstLink childTopVolume =  pixelTopVolume->getChildVol(i);
      const GeoLogVol* childLogVolume = childTopVolume->getLogVol();
      const GeoTube* childTube = 0;
      
      if (childLogVolume){
	childTube = dynamic_cast<const GeoTube*>(childLogVolume->getShape());
	if (childTube){
	  double radius = 0.5*(childTube->getRMin()+childTube->getRMax());
	  if(IPTRadius<0. || radius<IPTRadius) IPTRadius = radius;
	}
      }
      
    }
    
    ATH_MSG_INFO("IPTRadius used for material rejection="<<IPTRadius);
    
    // Fill map with IPT radius for all z
    for(double z = -zmax + 0.5*zbinwidth; z<zmax; z+=zbinwidth) m_ITkPixMaterialMap->Fill(z,IPTRadius);
    

    InDetDD::SiDetectorElementCollection::const_iterator iter;
    for (iter = pixelManager->getDetectorElementBegin(); iter != pixelManager->getDetectorElementEnd(); ++iter) {
      // get the ID
      Identifier Pixel_ModuleID = (*iter)->identify();
      // check the validity
      if (Pixel_ModuleID.is_valid()) {
	const InDetDD::SiDetectorElement *module = pixelManager->getDetectorElement(Pixel_ModuleID);
	
	//Take into account full module extent
	InDetDD::DetectorType type = module->design().type();
	
	if(type==InDetDD::PixelInclined){
	  double zMin_mod = module->zMin();
	  double zMax_mod = module->zMax();
	  double rMin_mod = module->rMin();
	  double rMax_mod = module->rMax();
	  if(zMin_mod>0){
	    double alpha = (rMin_mod-rMax_mod)/(zMax_mod-zMin_mod);
	    for(double z=zMin_mod; z<=zMax_mod; z+=zbinwidth){
	      double R = rMax_mod + alpha*(z-zMin_mod);
	      m_ITkPixMaterialMap->Fill(z,R);
	    }
	  }
	  else{
	    double alpha = (rMax_mod-rMin_mod)/(zMax_mod-zMin_mod);
	    for(double z=zMin_mod; z<=zMax_mod; z+=zbinwidth){
	      double R = rMin_mod + alpha*(z-zMin_mod);
	      m_ITkPixMaterialMap->Fill(z,R);
	    }
	  }
	}
	
	else if(type==InDetDD::PixelBarrel){
	  double zMin_mod = module->zMin();
	  double zMax_mod = module->zMax();
	  double R = std::hypot(module->center().x(),module->center().y());
	  for(double z=zMin_mod; z<=zMax_mod; z+=zbinwidth) m_ITkPixMaterialMap->Fill(z,R);
	}
	
	else if(type==InDetDD::PixelEndcap){
	  double rMin_mod = module->rMin();
	  double rMax_mod = module->rMax();
	  double z = module->center().z();
	  for(double R=rMin_mod; R<=rMax_mod; R+=Rbinwidth_pixel) m_ITkPixMaterialMap->Fill(z,R);
	}
	
      }
    } // end loop over pixel modules

  }

}

