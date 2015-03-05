/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/DBM_Det.h"
#include "PixelGeoModel/DBM_Telescope.h"
#include "PixelGeoModel/DBM_Services.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include <iostream>

DBM_Det::DBM_Det() {

  double Trans_Y = 0.;
  
  // Radius to beamline
  // Hardcoded, so if change then change in DBM_module too
  double trans_rad = 46.678*CLHEP::mm + (gmt_mgr->DBMTelescopeY()) / 2.; // 10-CLHEP::degree version

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
  double rmin = 45.*CLHEP::mm;//41.0*CLHEP::mm;
  double rmax = 150.*CLHEP::mm; //244.*CLHEP::mm;
  double halflength = gmt_mgr->DBMTelescopeZ()/2.;//200.*CLHEP::mm
  GeoTube * Shape = new GeoTube(rmin,rmax,halflength);
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoLogVol* Log = new GeoLogVol("OutsideDBM",Shape,air);
  GeoFullPhysVol* Phys = new GeoFullPhysVol(Log);

  CLHEP::HepRotation pp0rm;

  // add PP0 board
  DBM_PP0 pp0Board;
  GeoVPhysVol* pp0BoardPhys = pp0Board.Build();
  CLHEP::Hep3Vector pp0Pos(0., 0., -halflength + gmt_mgr->DBMPP0Thick()/2. + safety);
  GeoTransform* pp0xform = new GeoTransform(HepGeom::Transform3D(pp0rm,pp0Pos));
  GeoNameTag* pp0tag = new GeoNameTag("DBM_PP0"); 
  Phys->add(pp0tag);
  Phys->add(pp0xform);
  Phys->add(pp0BoardPhys);	      

  //we are now adding four DBM telescopes
  DBM_Telescope dbm;
  for(int i=0; i<4; i++)
    {
      gmt_mgr->SetEta(0);
      gmt_mgr->SetPhi(i);
      // Fixing swaping of module 0 and 2 on side C (-4)
      // sinceDBM side C is 180deg rotation around global Y
      if ((gmt_mgr->GetSide() < 0) && (i == 0)) gmt_mgr->SetPhi(2);
      else if ((gmt_mgr->GetSide() < 0) && (i == 2)) gmt_mgr->SetPhi(0);

      //setting transformation
      CLHEP::HepRotation rm;
      CLHEP::Hep3Vector pos(m_module[i].at(0), m_module[i].at(1), m_module[i].at(2));
      rm.rotateX(m_module[i].at(3)*CLHEP::deg);
      rm.rotateY(m_module[i].at(4)*CLHEP::deg);
      rm.rotateZ(m_module[i].at(5)*CLHEP::deg);
      GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rm,pos));

      GeoNameTag* tag = new GeoNameTag("DBM Module"); 
      GeoVPhysVol* dbmModPhys = dbm.Build();
      Phys->add(tag);
      Phys->add(new GeoIdentifierTag(i));
      Phys->add(xform);
      Phys->add(dbmModPhys);	      
    }


  // Set numerology for the full DBM system

  if(DDmgr->numerology().numEndcapsDBM()==0){

    int numDisk=3;
    int numPhiModules=4;    
    DDmgr->numerology().setNumDisksDBM(numDisk);
    DDmgr->numerology().setNumBarrelDBM(0);
    DDmgr->numerology().addEndcapDBM(4);
    DDmgr->numerology().addEndcapDBM(-4);
    for(int disk=0; disk<numDisk; disk++){
      DDmgr->numerology().setNumPhiModulesForDiskRingDBM(disk, 0, numPhiModules);
    }
  }

  return Phys;
}
