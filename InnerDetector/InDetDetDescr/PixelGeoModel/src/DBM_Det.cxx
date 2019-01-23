/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DBM_Det.h"
#include "DBM_Telescope.h"
#include "DBM_Services.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include <iostream>

DBM_Det::DBM_Det() {

  double Trans_Y = 0.;
  
  // Radius to beamline
  // Hardcoded, so if change then change in DBM_module too
  double trans_rad = 46.678*Gaudi::Units::mm + (m_gmt_mgr->DBMTelescopeY()) / 2.; // 10-Gaudi::Units::degree version

  //                 TRANS_X                        TRANS_Y                        TRANS_Z                          ROT_X                       ROT_Y                      ROT_Z        
  m_module[0].push_back(trans_rad);   m_module[0].push_back(0);         m_module[0].push_back(Trans_Y);    m_module[0].push_back(0);     m_module[0].push_back(0);    m_module[0].push_back(270);  
  m_module[1].push_back(0);        m_module[1].push_back(trans_rad);    m_module[1].push_back(Trans_Y);    m_module[1].push_back(0);     m_module[1].push_back(0);    m_module[1].push_back(0);   
  m_module[2].push_back(-trans_rad); m_module[2].push_back(0);        m_module[2].push_back(Trans_Y);   m_module[2].push_back(0);    m_module[2].push_back(0);   m_module[2].push_back(90); 
  m_module[3].push_back(0);      m_module[3].push_back(-trans_rad); m_module[3].push_back(Trans_Y);  m_module[3].push_back(0);   m_module[3].push_back(0);  m_module[3].push_back(180); 

}

GeoVPhysVol* DBM_Det::Build()
{
  double safety = 0.001;

  //create a cylinder 8mm smaller than the BPSS outer radius to place the 4 DBM telescopes
  double rmin = 45.*Gaudi::Units::mm;//41.0*Gaudi::Units::mm;
  double rmax = 150.*Gaudi::Units::mm; //244.*Gaudi::Units::mm;
  double halflength = m_gmt_mgr->DBMTelescopeZ()/2.;//200.*Gaudi::Units::mm
  GeoTube * Shape = new GeoTube(rmin,rmax,halflength);
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoLogVol* Log = new GeoLogVol("OutsideDBM",Shape,air);
  GeoFullPhysVol* Phys = new GeoFullPhysVol(Log);

  // add PP0 board
  DBM_PP0 pp0Board;
  GeoVPhysVol* pp0BoardPhys = pp0Board.Build();
  GeoTrf::Translate3D pp0Pos(0., 0., -halflength + m_gmt_mgr->DBMPP0Thick()/2. + safety);
  GeoTransform* pp0xform = new GeoTransform(pp0Pos);
  GeoNameTag* pp0tag = new GeoNameTag("DBM_PP0"); 
  Phys->add(pp0tag);
  Phys->add(pp0xform);
  Phys->add(pp0BoardPhys);	      

  //we are now adding four DBM telescopes
  DBM_Telescope dbm;
  for(int i=0; i<4; i++)
    {
      m_gmt_mgr->SetEta(0);
      m_gmt_mgr->SetPhi(i);
      // Fixing swaping of module 0 and 2 on side C (-4)
      // sinceDBM side C is 180deg rotation around global Y
      if ((m_gmt_mgr->GetSide() < 0) && (i == 0)) m_gmt_mgr->SetPhi(2);
      else if ((m_gmt_mgr->GetSide() < 0) && (i == 2)) m_gmt_mgr->SetPhi(0);

      //setting transformation
      GeoTrf::Transform3D rm  = GeoTrf::RotateZ3D(m_module[i].at(5)*Gaudi::Units::deg)
	* GeoTrf::RotateY3D(m_module[i].at(4)*Gaudi::Units::deg)
	* GeoTrf::RotateX3D(m_module[i].at(3)*Gaudi::Units::deg);
      GeoTrf::Translation3D pos(m_module[i].at(0), m_module[i].at(1), m_module[i].at(2));
      GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm));

      GeoNameTag* tag = new GeoNameTag("DBM Module"); 
      GeoVPhysVol* dbmModPhys = dbm.Build();
      Phys->add(tag);
      Phys->add(new GeoIdentifierTag(i));
      Phys->add(xform);
      Phys->add(dbmModPhys);	      
    }


  // Set numerology for the full DBM system

  if(m_DDmgr->numerology().numEndcapsDBM()==0){

    int numDisk=3;
    int numPhiModules=4;    
    m_DDmgr->numerology().setNumDisksDBM(numDisk);
    m_DDmgr->numerology().setNumBarrelDBM(0);
    m_DDmgr->numerology().addEndcapDBM(4);
    m_DDmgr->numerology().addEndcapDBM(-4);
    for(int disk=0; disk<numDisk; disk++){
      m_DDmgr->numerology().setNumPhiModulesForDiskRingDBM(disk, 0, numPhiModules);
    }
  }

  return Phys;
}
