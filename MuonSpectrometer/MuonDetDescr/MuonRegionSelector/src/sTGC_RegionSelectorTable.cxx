/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRegionSelector/sTGC_RegionSelectorTable.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"

#include <iostream>
#include <fstream>
#include <string>

#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 
#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "RegSelLUT/RegSelSiLUT.h"

#include <iostream>
#include <vector>
#include <fstream>

sTGC_RegionSelectorTable::sTGC_RegionSelectorTable(const std::string& type, 
					       const std::string& name,
					       const IInterface*  parent)
  :  AthAlgTool(type,name,parent),  
     m_regionLUT(0) 
{
  declareInterface<IRegionIDLUT_Creator>(this);
}


sTGC_RegionSelectorTable::~sTGC_RegionSelectorTable() { 
  if ( m_regionLUT ) {
    delete m_regionLUT;
    m_regionLUT =0;
  }
}


StatusCode sTGC_RegionSelectorTable::initialize() { 
  msg(MSG::INFO) << "initialize() sTGC_RegionSelectorTable" << endmsg;

  //  locate new cabling service    
  //  StatusCode sc = service("MuonsTGC_CablingSvc",m_mmCabling);
  //  if (sc.isFailure() ){
  //    msg() << MSG::ERROR << "Could not retrieve " << m_mmCabling << endmsg;
  //    return sc;
  //  }
  //  msg() << MSG::DEBUG << "Retrieved service " << m_mmCabling << endmsg;
  
  return createTable();
}


StatusCode sTGC_RegionSelectorTable::finalize() { 
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


RegSelSiLUT* sTGC_RegionSelectorTable::getLUT() {
  return m_regionLUT;
} 



  
StatusCode sTGC_RegionSelectorTable::createTable() { 

  /// this code is essentially identical to the Micromegas code - could
  /// probably reuse this same class for that also with some template 
  /// arguments for the different helper classes
	
  RegSelSiLUT* stgclut = new RegSelSiLUT();

  const MuonGM::MuonDetectorManager*	p_MuonMgr = NULL;

  ATH_CHECK(detStore()->retrieve( p_MuonMgr ));

  const sTgcIdHelper*  p_IdHelper = p_MuonMgr->stgcIdHelper();

  //  std::vector<Identifier>::const_iterator idfirst = p_IdHelper->module_begin();
  //  std::vector<Identifier>::const_iterator idlast  = p_IdHelper->module_end();

  std::vector<Identifier>::const_iterator idfirst = p_IdHelper->detectorElement_begin();
  std::vector<Identifier>::const_iterator idlast  = p_IdHelper->detectorElement_end();

  const IdContext ModuleContext = p_IdHelper->detectorElement_context();

  for ( std::vector<Identifier>::const_iterator i=idfirst ; i!=idlast ; i++ ) {
   
      Identifier     Id = *i;
      IdentifierHash hashId;

      p_IdHelper->get_hash( Id, hashId, &ModuleContext );

      const MuonGM::sTgcReadoutElement* mm = p_MuonMgr->getsTgcReadoutElement(Id);
      if (!mm) continue;

      //      std::cout << "stgc station name: " << mm->getStationName() << "\teta: " << mm->getStationEta() << "\tphi: " << mm->getStationPhi() << std::endl; 
  
      int multilayer = p_IdHelper->multilayer(Id);

      char side     = mm->getStationEta() < 0 ? 'C' : 'A';

      char sector_l = mm->getStationName().substr(2,1)=="L" ? 'L' : 'S';

      sTGCDetectorHelper aHelper;
      sTGCDetectorDescription* md = aHelper.Get_sTGCDetector( sector_l, std::abs(mm->getStationEta()), mm->getStationPhi(), multilayer, side );
  
      Amg::Vector3D mmPos = mm->center();      
  
      double swidth = md->sWidth();
      double lwidth = md->lWidth();

      double ycutout = md->yCutout();

      double length = md->Length();
      double depth  = md->Tck();

      double moduleR = std::sqrt( mmPos.mag()*mmPos.mag() -  mmPos.z()*mmPos.z());

      double zmin = mmPos.z()-0.5*depth;
      double zmax = mmPos.z()+0.5*depth;

      double rmin = moduleR-0.5*length;
      double rmax = std::sqrt( (moduleR+0.5*length)*(moduleR+0.5*length) + lwidth*lwidth/4 );

      double dphi1 = std::atan( (0.5*lwidth)/(moduleR+0.5*length) );
      double dphi2 = std::atan( (0.5*swidth)/(moduleR-0.5*length) );

      double dphi = ( dphi1 > dphi2 ? dphi1 : dphi2 );

      if ( ycutout > 0 ) { 
	double rcutout = moduleR+0.5*length - ycutout;
	double dphicutout = std::atan( (0.5*lwidth)/rcutout );
        if ( dphi < dphicutout ) dphi = dphicutout;
      }

      double phimin = mmPos.phi()-dphi;
      double phimax = mmPos.phi()+dphi;

      if ( phimin >  M_PI ) phimin -= 2*M_PI;
      if ( phimin < -M_PI ) phimin += 2*M_PI;

      int layerid = multilayer;
      int detid   = ( side == 'C' ? -1 : 1 );

      int robId  = 0;

      //      std::cout << "sTGC::module r: " << moduleR << "\tlength: " << length << "\tswidth: " << swidth << "\tlwidth: " << lwidth << "\tycut: " << ycutout << std::endl; 
      
      RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, int(layerid), int(detid), uint32_t(robId), hashId );

      stgclut->addModule( m );

  }

  stgclut->initialise(); 

  //  stgclut->write("stgc.map");

  m_regionLUT = stgclut;

  return StatusCode::SUCCESS;
}
