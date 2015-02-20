/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  18/05/00
//
// MagFAthenaTest03 is a C function for testing MagFieldAccess
//
#include "BFieldCore/MagFieldAccess.h"
 
extern "C"
void magfathenatest03_(){
//
//*Access AbstractMagneticField
   AbstractMagneticField* pMagField = MagFieldAccess::GetAbstractMagneticField();
//
//*Set the file to dump
   std::ofstream  OutTest;
   OutTest.open("Out.BFieldAthTest03");
   OutTest << setiosflags(std::ios::fixed);
//
//*Dump
   OutTest << " Dumpy03 " << std::endl;
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
