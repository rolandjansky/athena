/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  30/03/00
//
// The MagFAthenaTest02 class
// is an Algorithm class to test MagFieldAccess
//
/////////////////////////////////////////////////////////
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
 
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
 
/////////////////////////////////////////////////////////
#include "BFieldAthTest/MagFAthenaTest02.h"
 
#include "BFieldCore/MagFieldAccess.h"
 
MagFAthenaTest02::MagFAthenaTest02(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator){
//
}
 
// Initialize
StatusCode MagFAthenaTest02::initialize(){
//
   MsgStream log(messageService(), name());
   log << MSG::INFO << "in initialize()" << endreq;
   StatusCode sc ;
//
  return StatusCode::SUCCESS;
//
}
 
// Execute
StatusCode MagFAthenaTest02::execute(){
//
   MsgStream log(messageService(), name());
   log << MSG::INFO << "in execute()" << endreq;
//
//*Do Dumpy02
   Dumpy02();
//
//
  return StatusCode::SUCCESS;
//
}				
 
// Finalize
StatusCode MagFAthenaTest02::finalize(){
//
   MsgStream log(messageService(), name());
   log << MSG::INFO << "in finalize()" << endreq;
//
  return StatusCode::SUCCESS;
//
}

// Do Dumpy02
void MagFAthenaTest02::Dumpy02(){
//
//*Access AbstractMagneticField
   AbstractMagneticField* pMagField = MagFieldAccess::GetAbstractMagneticField();
//
//*Set the file to dump
   std::ofstream  OutTest;
   OutTest.open("Out.BFieldAthTest02");
   OutTest << setiosflags(std::ios::fixed);
//
//*Dump
   OutTest << " Dumpy02 " << std::endl;
//
   int    nz     = 2300 ;
   double zmax   = 2300.;
   double zmin   =    0.;
//   
   int    nr     = 1500 ;
   double rmax   = 1500.;
   double rmin   =    0.;
//   
   int    nphi   = 23  ;
   double phimax = 22.5;
   double phimin =  0.5; 
//   
   int nstep     = 100;
//   
   double zstep   = (zmax  -zmin  )/(nz  -1);
   double rstep   = (rmax  -rmin  )/(nr  -1);
   double phistep = (phimax-phimin)/(nphi-1);
//   
   for (int iz=1; iz<=nz; iz=iz+nstep){
    double The_z = zmin + zstep * (iz-1);
    for (int ir=1; ir<=nr; ir=ir+nstep){
     double The_r = rmin + rstep * (ir-1);
     for (int iphi=1; iphi<=nphi; iphi=iphi+nstep){
      double The_phi = (phimin + phistep * (iphi-1))*M_PI/180.;
//
      double The_x   = cos(The_phi)*The_r;
      double The_y   = sin(The_phi)*The_r;
//
      float XYZ[3]   ;
      float BGXYZ[12] ;
      XYZ[0] = The_x ;
      XYZ[1] = The_y ;
      XYZ[2] = The_z ;
      pMagField->fieldGradient(XYZ,BGXYZ);
      OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
              << std::endl;
      OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[3]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[4]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[5]
              << std::endl;
      OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[6]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[7]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[8]
              << std::endl;
      OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[9]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[10]
              << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[11]
              << std::endl;
     }
    }
   }
//
//*Close the file to dump
   OutTest.close();
//
}
