/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRegionSelector/MM_RegionSelectorTable.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"
#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 
#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "RegSelLUT/RegSelSiLUT.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

MM_RegionSelectorTable::MM_RegionSelectorTable(const std::string& type, 
					       const std::string& name,
					       const IInterface*  parent)
  :  AthAlgTool(type,name,parent),  
     m_regionLUT(0) 
{
  declareInterface<IRegionIDLUT_Creator>(this);
}


MM_RegionSelectorTable::~MM_RegionSelectorTable() { 
  if ( m_regionLUT ) {
    delete m_regionLUT;
    m_regionLUT =0;
  }
}


StatusCode MM_RegionSelectorTable::initialize() { 
  msg(MSG::INFO) << "initialize() MM_RegionSelectorTable" << endmsg;

  //  locate new cabling service    
  //  StatusCode sc = service("MuonMM_CablingSvc",m_mmCabling);
  //  if (sc.isFailure() ){
  //    msg() << MSG::ERROR << "Could not retrieve " << m_mmCabling << endmsg;
  //    return sc;
  //  }
  //  msg() << MSG::DEBUG << "Retrieved service " << m_mmCabling << endmsg;
  
  return createTable();
}


StatusCode MM_RegionSelectorTable::finalize() { 
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


RegSelSiLUT* MM_RegionSelectorTable::getLUT() {
  return m_regionLUT;
} 



  
StatusCode MM_RegionSelectorTable::createTable() { 
	
  RegSelSiLUT* mmlut = new RegSelSiLUT();

  const MuonGM::MuonDetectorManager*	p_MuonMgr = NULL;
  
  ATH_CHECK( detStore()->retrieve( p_MuonMgr ));

  const MmIdHelper*  p_IdHelper = p_MuonMgr->mmIdHelper();

  std::vector<Identifier>::const_iterator idfirst = p_IdHelper->detectorElement_begin();
  std::vector<Identifier>::const_iterator idlast  = p_IdHelper->detectorElement_end();

  const IdContext ModuleContext = p_IdHelper->detectorElement_context();

  for ( std::vector<Identifier>::const_iterator i=idfirst ; i!=idlast ; i++ ) {

      Identifier     Id = *i;
      IdentifierHash hashId;

      p_IdHelper->get_hash( Id, hashId, &ModuleContext );

      const MuonGM::MMReadoutElement* mm = p_MuonMgr->getMMReadoutElement(Id);
      if (!mm) continue;

      int multilayer = p_IdHelper->multilayer(Id);

      char side     = mm->getStationEta() < 0 ? 'C' : 'A';

      char sector_l = mm->getStationName().substr(2,1)=="L" ? 'L' : 'S';

      MMDetectorHelper aHelper;
      MMDetectorDescription* md = aHelper.Get_MMDetector( sector_l, std::abs(mm->getStationEta()), mm->getStationPhi(), multilayer, side );

      /// now calculate the required limits

      Amg::Vector3D mmPos = mm->center();      
  
      
      double swidth = md->sWidth();
      double lwidth = md->lWidth();

      double length = md->Length();
      double depth  = md->Tck();

      double moduleR = std::sqrt( mmPos.mag()*mmPos.mag() -  mmPos.z()*mmPos.z());

      double zmin = mmPos.z()-0.5*depth;
      double zmax = mmPos.z()+0.5*depth;

      //      std::cout << "MM:module: rcen: " << moduleR << "\tlength: " << length << "\tswidth: " << swidth << "\tlwidth: " << lwidth << std::endl; 

      double rmin = moduleR-0.5*length;
      double rmax = std::sqrt( (moduleR+0.5*length)*(moduleR+0.5*length) + 0.25*lwidth*lwidth );

      double dphi1 = std::atan( (0.5*lwidth)/(moduleR+0.5*length) );
      double dphi2 = std::atan( (0.5*swidth)/(moduleR-0.5*length) );

      double dphi = ( dphi1 > dphi2 ? dphi1 : dphi2 );

      double phimin = mmPos.phi()-dphi;
      double phimax = mmPos.phi()+dphi;

      if ( phimin >  M_PI ) phimin -= 2*M_PI;
      if ( phimin < -M_PI ) phimin += 2*M_PI;

      if ( phimax >  M_PI ) phimax -= 2*M_PI;
      if ( phimax < -M_PI ) phimax += 2*M_PI;

      int layerid = multilayer;
      int detid   = ( side == 'C' ? -1 : 1 );

      /// no cabling map at the moment - don't store robID 
      int robId  = 0;

      RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, int(layerid), int(detid), uint32_t(robId), hashId );

      mmlut->addModule( m );

  }

  mmlut->initialise(); 

  //  mmlut->write("mm.map");

  m_regionLUT = mmlut;

  return StatusCode::SUCCESS;
}
