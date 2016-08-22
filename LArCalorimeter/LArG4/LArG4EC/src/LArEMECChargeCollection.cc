/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArEMECChargeCollection.cc
// Prepared by J.Toth 02-July-2003;
//*****************************************************************************
// Code for charge collection in emec                                         *
// Source: endegeo.age of Dice by A.Chekhtman and S.Klimenko in 1995.         *
//                                                                            *
//  Nov-2006 (JT) -include fieldmap for the very first and last fold as well  *
//                -include dynamic memory allocation for the fieldmaps        *
//                -field map is used for defining the signal in the Barrett   *
//                    volume                                                  *
//  31-July-2005  (J.T.):                                                     *
//     suppress signals closer to an electrodes than CHC_Esr                  *
//                                                                            *
//  01-July-2005 J.T. - fieldmap extended for outer wheel;                    *
//                   - 5 new layers 1200< r < 1500 mm are introduced          *
//  25-May-2005 (J.T.):                                                       *
//     - HV values for each power supplies are treated:                       *
//       read it from file                                                    *
//  	 /afs/cern.ch/atlas/offline/data/lar/emec/efield/HVEMECMap.dat;       *
//       (the values are the nominal ones from TB)                            *
//     - current computation changed:                                         *
//         - HV value from the above file is taken into account               *
//         - IonRecombination effect is taken into accont                     *
//         - drift velocity parametrized as a function of T and Efield        *
//  	     from:note ATLAS-LARG-99-008;                                     *
//           T is set to te average tempr. measured for ECC0,1,5 at TB        *
//  	   - current ~ edep*1/HV* Efield*v_drift(T,Efield);                   *
//  	     normalized at eta=2.25 where the gapsize is 1.3mm(=AverageGap)   *
//                                                                            *
//  20-May-2005 (J.T.):                                                       *
//             - in case of err=-999,                                         *
//               edep in EnergyCalculator is set to 0.                *
//             - change gap computation to be consistent with the fieldmap    *
//             - extrapolation of the field first,                            *
//               then put it on power of 1.3(old case is the reverse)         *
//             - change duble,int,bool..etc for G4double,G4int,G4bool etc.    *
//                                                                            *
//  20-July-2004 (J.T.): New fieldmap prepared. This takes into account the   *
//        correct thickness and folding radius of the electrodes.             *
//        Modification of the code to read either the Old or New map          *
//         was done.                                                          *
//        The desired version (New or Old) of the fieldmap should be set      *
//        to s_FieldMapVersion variable of the LArEMECEnergyCorrrection.cc      *
//        The file name which contains the map should also be set             *
//        to FieldMapFileName variable of the LArEMECEnergyCorrrection.cc     *
//                                                                            *
//  02-July-2003 (J.T.): code improved and rewritten                          *
//                                                                            *
//   -The present version uses a fieldmap which was generated using           *
//    electrode tickness of 300 mikron and curvature radius of the            *
//    electrode's folds of 3mm in both wheels.                                *
//    On other hand the WheelSolid geometry includes 275 mikron thick         *
//    electrodes with fold radius of 3mm for outer and 3.25mm for inner       *
//    wheel. The space point where the energy correction                      *
//    should be computed is defined relative to the new electrodes.           *
//    Still the old tables are used for the actual computation such a way     *
//    that the new LAr gap in the r-phi plane is mapped onto the old one.     *
//   -First and last fold are not treated, kept as they were outside          *
//    of folding region                                                       *
//   -There maybe a problem with folding angle parametrization(see comment    *
//    in the function FoldingAngle                                            *
//   -Another problem may arise, because PhiGap number computed in the        *
//    m_calculator sometimes does not match the one computed during           *
//    charge collection.                                                      *
//    (taking into account a possible 1 bin shift between them by default)    *
//     Here the result of latter is kept.                                     *
//   -The present code works independently of the parameters and functions    *
//    defined in the calculator. This is in order to keep the possibility of  *
//    cross checks for a while.                                               *
//                                                                            *
//  21-May-2003 (J.T.) : Code modified to work in                             *
//                       EnergyCalculator class                        *
//                                                                            *
//  15-Dec-2001 (J.T.) :The original code has been translated to work in      *
//                      C++ environment                                       *
//*****************************************************************************

#include <cassert>
#include <cmath>

#include "GaudiKernel/MsgStream.h"

#include "G4ThreeVector.hh"
#include "globals.hh"

#include "LArG4Code/LArG4BirksLaw.h"
#include "LArG4EC/EnergyCalculator.h"

#include "CLHEP/Units/PhysicalConstants.h"

// #define DEBUGGetCur

using namespace LArG4::EC;

// ***************************************************************************
void EnergyCalculator::CreateArrays(Wheel_Efield_Map &wheel, G4int n){
// ***************************************************************************

//  std::cout<<"***CreateArrays wheeltype; n="<<n
//	   <<std::endl;

  wheel.NumberOfRadialLayer = n;
  wheel.RadiusOfLayers         =new G4double [n];
  wheel.FoldinAngleOfLayers    =new G4double [n];
  wheel.HalfLArGapSizeOfLayers =new G4double [n];

  if(wheel.Fold0.FieldMapPrepared){

    wheel.Fold0.MinZofLayer      =new G4double [n];
    wheel.Fold0.MaxZofLayer      =new G4double [n];
    wheel.Fold0.MinYofLayer      =new G4double [n];
    wheel.Fold0.MaxYofLayer      =new G4double [n];
    wheel.Fold0.NofColofLayer    =new G4int    [n];
    wheel.Fold0.NofRowofLayer    =new G4int    [n];
    wheel.Fold0.NofPointsinLayer =new G4int    [n];
    wheel.Fold0.pLayer           =new G4int    [n];
  }

  if(wheel.Fold1.FieldMapPrepared){

    wheel.Fold1.MinZofLayer      =new G4double [n];
    wheel.Fold1.MaxZofLayer      =new G4double [n];
    wheel.Fold1.MinYofLayer      =new G4double [n];
    wheel.Fold1.MaxYofLayer      =new G4double [n];
    wheel.Fold1.NofColofLayer    =new G4int    [n];
    wheel.Fold1.NofRowofLayer    =new G4int    [n];
    wheel.Fold1.NofPointsinLayer =new G4int    [n];
    wheel.Fold1.pLayer           =new G4int    [n];
  }
}


// ***************************************************************************
void EnergyCalculator::IniGeomforFieldMaps(void){
// ***************************************************************************
//	 std::cout
	(*m_msg) << MSG::INFO
	       << "***IniGeomforFieldMaps() called for FieldMapVersion = "
	       << s_FieldMapVersion
	       << endmsg;
//		  <<std::endl;

       if(s_FieldMapVersion == "v00" || s_FieldMapVersion == "v01" ||
          s_FieldMapVersion == "v02"){

	 s_ChCollInner.Fold1.FieldMapPrepared=true;
	 s_ChCollOuter.Fold1.FieldMapPrepared=true;

	 s_ChCollInner.Fold0.FieldMapPrepared=false;
	 s_ChCollOuter.Fold0.FieldMapPrepared=false;

       if(s_FieldMapVersion == "v00") {

	 G4int nlayer=6; // number of radial layers
	 std::cout<<" ***IniGeomforFieldMaps calls CreateArrays"
		  <<std::endl;
      	 CreateArrays(s_ChCollInner,nlayer);
      	 CreateArrays(s_ChCollOuter,nlayer);

         s_ChCollInner.RadiusOfLayers[0]=300.;//[mm]
         s_ChCollInner.RadiusOfLayers[1]=400.;
         s_ChCollInner.RadiusOfLayers[2]=500.;
         s_ChCollInner.RadiusOfLayers[3]=600.;
         s_ChCollInner.RadiusOfLayers[4]=700.;
         s_ChCollInner.RadiusOfLayers[5]=800.;

         s_ChCollOuter.RadiusOfLayers[0]= 600.; //[mm]
         s_ChCollOuter.RadiusOfLayers[1]= 900.;
         s_ChCollOuter.RadiusOfLayers[2]=1200.;
         s_ChCollOuter.RadiusOfLayers[3]=1500.;
         s_ChCollOuter.RadiusOfLayers[4]=1800.;
         s_ChCollOuter.RadiusOfLayers[5]=2100.;

         s_ChCollInner.FoldinAngleOfLayers[0]= 112.7;//[degree]
         s_ChCollInner.FoldinAngleOfLayers[1]=  92.2;
         s_ChCollInner.FoldinAngleOfLayers[2]=  78.7;
         s_ChCollInner.FoldinAngleOfLayers[3]=  68.8;
         s_ChCollInner.FoldinAngleOfLayers[4]=  61.3;
         s_ChCollInner.FoldinAngleOfLayers[5]=  55.2;

         s_ChCollInner.HalfLArGapSizeOfLayers[0]= 1.44; //[mm]
         s_ChCollInner.HalfLArGapSizeOfLayers[1]= 1.94;
         s_ChCollInner.HalfLArGapSizeOfLayers[2]= 2.29;
         s_ChCollInner.HalfLArGapSizeOfLayers[3]= 2.56;
         s_ChCollInner.HalfLArGapSizeOfLayers[4]= 2.78;
         s_ChCollInner.HalfLArGapSizeOfLayers[5]= 2.96;

         s_ChCollOuter.FoldinAngleOfLayers[0]=129.;//[degree]
         s_ChCollOuter.FoldinAngleOfLayers[1]=101.4;
         s_ChCollOuter.FoldinAngleOfLayers[2]= 84.8;
         s_ChCollOuter.FoldinAngleOfLayers[3]= 73.2;
         s_ChCollOuter.FoldinAngleOfLayers[4]= 64.5;
         s_ChCollOuter.FoldinAngleOfLayers[5]= 57.7;

         s_ChCollOuter.HalfLArGapSizeOfLayers[0]=0.87; //[mm]
         s_ChCollOuter.HalfLArGapSizeOfLayers[1]=1.5;
         s_ChCollOuter.HalfLArGapSizeOfLayers[2]=1.96;
         s_ChCollOuter.HalfLArGapSizeOfLayers[3]=2.31;
         s_ChCollOuter.HalfLArGapSizeOfLayers[4]=2.58;
         s_ChCollOuter.HalfLArGapSizeOfLayers[5]=2.79;
       }

       if(s_FieldMapVersion == "v01") {

	 G4int nlayer=6;
      	 CreateArrays(s_ChCollInner,nlayer);
      	 CreateArrays(s_ChCollOuter,nlayer);

         s_ChCollInner.RadiusOfLayers[0]=300.;//[mm]
         s_ChCollInner.RadiusOfLayers[1]=400.;
         s_ChCollInner.RadiusOfLayers[2]=500.;
         s_ChCollInner.RadiusOfLayers[3]=600.;
         s_ChCollInner.RadiusOfLayers[4]=700.;
         s_ChCollInner.RadiusOfLayers[5]=800.;

         s_ChCollOuter.RadiusOfLayers[0]= 600.; //[mm]
         s_ChCollOuter.RadiusOfLayers[1]= 900.;
         s_ChCollOuter.RadiusOfLayers[2]=1200.;
         s_ChCollOuter.RadiusOfLayers[3]=1500.;
         s_ChCollOuter.RadiusOfLayers[4]=1800.;
         s_ChCollOuter.RadiusOfLayers[5]=2100.;

         s_ChCollInner.FoldinAngleOfLayers[0]= 112.742024;  //[degree]
         s_ChCollInner.FoldinAngleOfLayers[1]=  92.2379512;
         s_ChCollInner.FoldinAngleOfLayers[2]=  78.6993545;
         s_ChCollInner.FoldinAngleOfLayers[3]=  68.8423119;
         s_ChCollInner.FoldinAngleOfLayers[4]=  61.2714745;
         s_ChCollInner.FoldinAngleOfLayers[5]=  55.2462062;

         s_ChCollInner.HalfLArGapSizeOfLayers[0]= 1.47806735;  //[mm]
         s_ChCollInner.HalfLArGapSizeOfLayers[1]= 1.95080191;
         s_ChCollInner.HalfLArGapSizeOfLayers[2]= 2.30318731;
         s_ChCollInner.HalfLArGapSizeOfLayers[3]= 2.57486515;
         s_ChCollInner.HalfLArGapSizeOfLayers[4]= 2.79014569;
         s_ChCollInner.HalfLArGapSizeOfLayers[5]= 2.96463479;

         s_ChCollOuter.FoldinAngleOfLayers[0]= 129.00197;  //[degree]
         s_ChCollOuter.FoldinAngleOfLayers[1]= 101.408128;
         s_ChCollOuter.FoldinAngleOfLayers[2]=  84.7966382;
         s_ChCollOuter.FoldinAngleOfLayers[3]=  73.1643989;
         s_ChCollOuter.FoldinAngleOfLayers[4]=  64.4458572;
         s_ChCollOuter.FoldinAngleOfLayers[5]=  57.6301588;

         s_ChCollOuter.HalfLArGapSizeOfLayers[0]= 0.877808149;  //[mm]
         s_ChCollOuter.HalfLArGapSizeOfLayers[1]= 1.5116158;
         s_ChCollOuter.HalfLArGapSizeOfLayers[2]= 1.97238634;
         s_ChCollOuter.HalfLArGapSizeOfLayers[3]= 2.31937997;
         s_ChCollOuter.HalfLArGapSizeOfLayers[4]= 2.58864056;
         s_ChCollOuter.HalfLArGapSizeOfLayers[5]= 2.80290925;
       }

       if(s_FieldMapVersion == "v02") {

	 G4int nilayer=6;  // number of layers for inner/outer wheel
	 G4int nolayer=11;
      	 CreateArrays(s_ChCollInner,nilayer);
      	 CreateArrays(s_ChCollOuter,nolayer);

         s_ChCollInner.RadiusOfLayers[0]=300.;//[mm]
         s_ChCollInner.RadiusOfLayers[1]=400.;
         s_ChCollInner.RadiusOfLayers[2]=500.;
         s_ChCollInner.RadiusOfLayers[3]=600.;
         s_ChCollInner.RadiusOfLayers[4]=700.;
         s_ChCollInner.RadiusOfLayers[5]=800.;

         s_ChCollOuter.RadiusOfLayers[0] = 600.; //[mm]
         s_ChCollOuter.RadiusOfLayers[1] = 900.;
         s_ChCollOuter.RadiusOfLayers[2] =1200.;
         s_ChCollOuter.RadiusOfLayers[3] =1250.;
         s_ChCollOuter.RadiusOfLayers[4] =1300.;
         s_ChCollOuter.RadiusOfLayers[5] =1350.;
         s_ChCollOuter.RadiusOfLayers[6] =1400.;
         s_ChCollOuter.RadiusOfLayers[7] =1450.;
         s_ChCollOuter.RadiusOfLayers[8] =1500.;
         s_ChCollOuter.RadiusOfLayers[9] =1800.;
         s_ChCollOuter.RadiusOfLayers[10]=2100.;

         s_ChCollInner.FoldinAngleOfLayers[0]= 112.742024;  //[degree]
         s_ChCollInner.FoldinAngleOfLayers[1]=  92.2379512;
         s_ChCollInner.FoldinAngleOfLayers[2]=  78.6993545;
         s_ChCollInner.FoldinAngleOfLayers[3]=  68.8423119;
         s_ChCollInner.FoldinAngleOfLayers[4]=  61.2714745;
         s_ChCollInner.FoldinAngleOfLayers[5]=  55.2462062;

         s_ChCollInner.HalfLArGapSizeOfLayers[0]= 1.47806735;  //[mm]
         s_ChCollInner.HalfLArGapSizeOfLayers[1]= 1.95080191;
         s_ChCollInner.HalfLArGapSizeOfLayers[2]= 2.30318731;
         s_ChCollInner.HalfLArGapSizeOfLayers[3]= 2.57486515;
         s_ChCollInner.HalfLArGapSizeOfLayers[4]= 2.79014569;
         s_ChCollInner.HalfLArGapSizeOfLayers[5]= 2.96463479;

         s_ChCollOuter.FoldinAngleOfLayers[0]= 129.00197;  //[degree]
         s_ChCollOuter.FoldinAngleOfLayers[1]= 101.408128;
         s_ChCollOuter.FoldinAngleOfLayers[2]=  84.7966382;
         s_ChCollOuter.FoldinAngleOfLayers[3]=  82.5893317;
         s_ChCollOuter.FoldinAngleOfLayers[4]=  80.5022136;
         s_ChCollOuter.FoldinAngleOfLayers[5]=  78.524804;
         s_ChCollOuter.FoldinAngleOfLayers[6]=  76.6479315;
         s_ChCollOuter.FoldinAngleOfLayers[7]=  74.8635164;
         s_ChCollOuter.FoldinAngleOfLayers[8]=  73.1643989;
         s_ChCollOuter.FoldinAngleOfLayers[9]=  64.4458572;
         s_ChCollOuter.FoldinAngleOfLayers[10]= 57.6301588;

         s_ChCollOuter.HalfLArGapSizeOfLayers[0]= 0.877808149;  //[mm]
         s_ChCollOuter.HalfLArGapSizeOfLayers[1]= 1.5116158;
         s_ChCollOuter.HalfLArGapSizeOfLayers[2]= 1.97238634;
         s_ChCollOuter.HalfLArGapSizeOfLayers[3]= 2.03692756;
         s_ChCollOuter.HalfLArGapSizeOfLayers[4]= 2.09855584;
         s_ChCollOuter.HalfLArGapSizeOfLayers[5]= 2.15745335;
         s_ChCollOuter.HalfLArGapSizeOfLayers[6]= 2.21378826;
         s_ChCollOuter.HalfLArGapSizeOfLayers[7]= 2.26771591;
         s_ChCollOuter.HalfLArGapSizeOfLayers[8]= 2.31937997;
         s_ChCollOuter.HalfLArGapSizeOfLayers[9]= 2.58864056;
         s_ChCollOuter.HalfLArGapSizeOfLayers[10]=2.80290925;
       }
       }
       else if(s_FieldMapVersion == "v03"){

	 s_ChCollInner.Fold1.FieldMapPrepared=true;
	 s_ChCollOuter.Fold1.FieldMapPrepared=true;

	 s_ChCollInner.Fold0.FieldMapPrepared=true;
	 s_ChCollOuter.Fold0.FieldMapPrepared=true;

	 G4int nilayer=6; //number of radial layers; should be the same for
	 G4int nolayer=16;// all fold type
      	 CreateArrays(s_ChCollInner,nilayer);
      	 CreateArrays(s_ChCollOuter,nolayer);

	 for(G4int i=0;i<nilayer;++i){
	   s_ChCollInner.RadiusOfLayers[i]=300.+i*100.;
	 }
         s_ChCollInner.FoldinAngleOfLayers[0]= 112.7420;  //[degree]
         s_ChCollInner.FoldinAngleOfLayers[1]=  92.238;
         s_ChCollInner.FoldinAngleOfLayers[2]=  78.6994;
         s_ChCollInner.FoldinAngleOfLayers[3]=  68.8423;
         s_ChCollInner.FoldinAngleOfLayers[4]=  61.2715;
         s_ChCollInner.FoldinAngleOfLayers[5]=  55.2462;

         s_ChCollInner.HalfLArGapSizeOfLayers[0]= 1.5326;  //[mm]
         s_ChCollInner.HalfLArGapSizeOfLayers[1]= 2.0053;
         s_ChCollInner.HalfLArGapSizeOfLayers[2]= 2.3577;
         s_ChCollInner.HalfLArGapSizeOfLayers[3]= 2.6294;
         s_ChCollInner.HalfLArGapSizeOfLayers[4]= 2.8446;
         s_ChCollInner.HalfLArGapSizeOfLayers[5]= 3.0191;

	 for(G4int i=0;i<nolayer;++i){
	   s_ChCollOuter.RadiusOfLayers[i]=600.+i*100.;
	 }

         s_ChCollOuter.FoldinAngleOfLayers[0]= 129.002;  //[degree]
         s_ChCollOuter.FoldinAngleOfLayers[1]= 117.7235;
         s_ChCollOuter.FoldinAngleOfLayers[2]= 108.8045;
         s_ChCollOuter.FoldinAngleOfLayers[3]= 101.4081;
         s_ChCollOuter.FoldinAngleOfLayers[4]=  95.1012;
         s_ChCollOuter.FoldinAngleOfLayers[5]=  89.6220;
         s_ChCollOuter.FoldinAngleOfLayers[6]=  84.7966;
         s_ChCollOuter.FoldinAngleOfLayers[7]=  80.5022;
         s_ChCollOuter.FoldinAngleOfLayers[8]=  76.6479;
         s_ChCollOuter.FoldinAngleOfLayers[9]=  73.1644;
         s_ChCollOuter.FoldinAngleOfLayers[10]= 69.9972;
         s_ChCollOuter.FoldinAngleOfLayers[11]= 67.1028;
         s_ChCollOuter.FoldinAngleOfLayers[12]= 64.4459;
         s_ChCollOuter.FoldinAngleOfLayers[13]= 61.9970;
         s_ChCollOuter.FoldinAngleOfLayers[14]= 59.7320;
         s_ChCollOuter.FoldinAngleOfLayers[15]= 57.6302;

         s_ChCollOuter.HalfLArGapSizeOfLayers[0]= 0.9367;  //[mm]
         s_ChCollOuter.HalfLArGapSizeOfLayers[1]= 1.1723;
         s_ChCollOuter.HalfLArGapSizeOfLayers[2]= 1.3824;
         s_ChCollOuter.HalfLArGapSizeOfLayers[3]= 1.5705;
         s_ChCollOuter.HalfLArGapSizeOfLayers[4]= 1.7398;
         s_ChCollOuter.HalfLArGapSizeOfLayers[5]= 1.8927;
         s_ChCollOuter.HalfLArGapSizeOfLayers[6]= 2.0313;
         s_ChCollOuter.HalfLArGapSizeOfLayers[7]= 2.1575;
         s_ChCollOuter.HalfLArGapSizeOfLayers[8]= 2.2727;
         s_ChCollOuter.HalfLArGapSizeOfLayers[9]= 2.3783;
         s_ChCollOuter.HalfLArGapSizeOfLayers[10]=2.4754;
         s_ChCollOuter.HalfLArGapSizeOfLayers[11]=2.5648;
         s_ChCollOuter.HalfLArGapSizeOfLayers[12]=2.6476;
         s_ChCollOuter.HalfLArGapSizeOfLayers[13]=2.7242;
         s_ChCollOuter.HalfLArGapSizeOfLayers[14]=2.7955;
         s_ChCollOuter.HalfLArGapSizeOfLayers[15]=2.8618;

       }
       else{
	 (*m_msg) << MSG::FATAL
		<< "IniGeomforFieldMaps -  FieldMapVersion = '"
		<< s_FieldMapVersion << "' not found!" << endmsg;
       }

       s_ChCollInner.GridShift=0.050;
       s_ChCollOuter.GridShift=0.0667;
}

// ***********************************************************
void EnergyCalculator::GetFieldMaps(const G4String fname){
// ***********************************************************

  G4int i,j,npoints,wheeltype,foldtype,foldtypemx=0,nlayer;
  char fieldmapversion[10];

  (*m_msg) << MSG::INFO
	 << "GetFieldMaps from file = " << fname << endmsg;

   FILE *lun = fopen(fname, "r");

   if(lun == 0){
      (*m_msg) << MSG::FATAL
	     << "GetFieldMaps - file '" << fname << "' not found!"
	     << endmsg;
      throw std::runtime_error("LArEMECChargeCollection: File not found!");
   }

   if(s_FieldMapVersion != "v00"){
           if (fscanf(lun, "%9s", fieldmapversion) < 1) {
             (*m_msg) << MSG::ERROR
                      << "GetFieldMaps Error reading field map" << endmsg;
           }
	   (*m_msg) << MSG::INFO
		  << "This file contains fieldmap of version '" << fieldmapversion
		  << "'"
                  << endmsg;
   }

// Check which fold should have a map

    if     (s_ChCollInner.Fold1.FieldMapPrepared &&
            s_ChCollOuter.Fold1.FieldMapPrepared &&
            s_ChCollInner.Fold0.FieldMapPrepared &&
            s_ChCollOuter.Fold0.FieldMapPrepared)     foldtypemx=2;
    else if(s_ChCollInner.Fold1.FieldMapPrepared &&
            s_ChCollOuter.Fold1.FieldMapPrepared &&
           !s_ChCollInner.Fold0.FieldMapPrepared &&
           !s_ChCollOuter.Fold0.FieldMapPrepared)     foldtypemx=1;
    else{

      (*m_msg) << MSG::FATAL
	     <<" ***GetFieldMaps file format is incompatible"
	     <<endmsg;
    }

// occupy store for the FieldMaps

    for(foldtype  = 0; foldtype  < foldtypemx; ++ foldtype){
     for(wheeltype = 0; wheeltype < 2;          ++ wheeltype){

       if(wheeltype == 0)  m_ChCollWheelType = &s_ChCollInner;
       if(wheeltype == 1)  m_ChCollWheelType = &s_ChCollOuter;
       if(foldtype  == 0 ) m_ChCollFoldType  = &(m_ChCollWheelType->Fold1);
       if(foldtype  == 1 ) m_ChCollFoldType  = &(m_ChCollWheelType->Fold0);

       nlayer = m_ChCollWheelType->NumberOfRadialLayer; // should be the same for Fold0 and Fold1

	 for(i=0; i<nlayer; ++i){
             if (fscanf(lun, "%lg%lg%lg%lg", &m_ChCollFoldType->MinZofLayer[i],
                                             &m_ChCollFoldType->MaxZofLayer[i],
                                             &m_ChCollFoldType->MinYofLayer[i],
                                             &m_ChCollFoldType->MaxYofLayer[i]) < 4)
             {
               (*m_msg) << MSG::ERROR
                        << "GetFieldMaps Error reading field map" << endmsg;
             }
	     if (fscanf(lun, "%i%i",         &m_ChCollFoldType->NofColofLayer[i],
                                             &m_ChCollFoldType->NofRowofLayer[i]) < 2)
             {
               (*m_msg) << MSG::ERROR
                        << "GetFieldMaps Error reading field map" << endmsg;
             }

	     m_ChCollFoldType->NofColofLayer[i] ++;
	     m_ChCollFoldType->NofRowofLayer[i] ++;
	     npoints = m_ChCollFoldType->NofColofLayer[i]*m_ChCollFoldType->NofRowofLayer[i];
	     if(npoints>0 && npoints < std::numeric_limits<int>::max()){
	     m_ChCollFoldType->NofPointsinLayer[i]=npoints;
	     if(i==0) m_ChCollFoldType->pLayer[i]=0;
	     else{    m_ChCollFoldType->pLayer[i]=m_ChCollFoldType->pLayer[i-1]+
			             3*m_ChCollFoldType->NofPointsinLayer[i-1];
	     }
	     
	     for(j=0;j<npoints;++j){
	       G4double zdummy,ydummy,wdummy;
	       if (fscanf(lun, "%lg%lg%lg", &zdummy,&ydummy,&wdummy) < 3) {
                 (*m_msg) << MSG::ERROR
                          << "GetFieldMaps Error reading field map" << endmsg;
               }
	     } // end for points
	     }
//	     std::cout
	     (*m_msg) << MSG::INFO
	            <<" foldtype="<<foldtype<< " wheeltype="<<wheeltype
		    <<" ilayer = " << i << " zmin, max = "
		    << m_ChCollFoldType->MinZofLayer[i] << " "
		    << m_ChCollFoldType->MaxZofLayer[i] << " "
		    << " ymin, max = "
		    << m_ChCollFoldType->MinYofLayer[i] << " "
		    << m_ChCollFoldType->MaxYofLayer[i] << " "
		    << " ncol = " << m_ChCollFoldType->NofColofLayer[i]
		    << " nrow = " << m_ChCollFoldType->NofRowofLayer[i]
		    << " npoints = " << npoints
		    <<endmsg;
//		    << std::endl;

	   } // end for layers
	 
	 G4int length=m_ChCollFoldType->pLayer[nlayer-1]+3*m_ChCollFoldType->NofPointsinLayer[nlayer-1];
	 m_ChCollFoldType->FieldMap = new G4double [length];

	 //	 std::cout<<" ***GetFieldMaps : length of FieldMap="<<length
	 //	  <<std::endl;

	 if(m_ChCollFoldType->FieldMap == 0 ) {

	   (*m_msg) << MSG::FATAL
		  <<" ***GetFieldMaps: FATAL  FieldMap array cannot be created"
		  <<endmsg;
	 }

      }  // end for wheels
    }   //end for foldtypes

    rewind(lun);
    if(s_FieldMapVersion != "v00") {
      if (fscanf(lun, "%9s", fieldmapversion) < 1) {
        (*m_msg) << MSG::ERROR
                 << "GetFieldMaps Error reading field map" << endmsg;
      }
    }

// now fill the coordinates and efield in the FieldMaps

    for(foldtype  = 0; foldtype  < foldtypemx; ++ foldtype){
     for(wheeltype = 0; wheeltype < 2;          ++ wheeltype){

       if(wheeltype == 0)  m_ChCollWheelType = &s_ChCollInner;
       if(wheeltype == 1)  m_ChCollWheelType = &s_ChCollOuter;
       if(foldtype  == 0 ) m_ChCollFoldType  = &(m_ChCollWheelType->Fold1);
       if(foldtype  == 1 ) m_ChCollFoldType  = &(m_ChCollWheelType->Fold0);

       nlayer = m_ChCollWheelType->NumberOfRadialLayer; // should be the same for Fold0 and Fold1

       for(i=0; i<nlayer; ++i){

	   G4double dummy1,dummy2,dummy3,dummy4;
	   G4int    dummy5,dummy6;
	   if (fscanf(lun, "%lg%lg%lg%lg", &dummy1, &dummy2, &dummy3, &dummy4) < 4) {
             (*m_msg) << MSG::ERROR
                      << "GetFieldMaps Error reading field map" << endmsg;
           }
	   if (fscanf(lun, "%i%i",         &dummy5, &dummy6) < 2) {
             (*m_msg) << MSG::ERROR
                      << "GetFieldMaps Error reading field map" << endmsg;
           }

           npoints = m_ChCollFoldType->NofPointsinLayer[i];
	   if(npoints>0){
	   for(j = 0; j < npoints; ++ j){
	     if (fscanf(lun, "%lg%lg%lg", &m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,0,j)],
                        &m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,1,j)],
                        &m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,2,j)]) < 3)
             {
               (*m_msg) << MSG::ERROR
                        << "GetFieldMaps Error reading field map" << endmsg;
             }

/*	     if(j<10) G4cout<<" z,y,weight="
			    <<m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,0,j)]<<" "
			    <<m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,1,j)]<<" "
			    <<m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,2,j)]
			    <<G4endl; */

	   } // end of loop(j) on points
	   }
       }    // end of loop(i) on layers
     }	   // end of loop on wheeltype
    }    // end of loop for foldtypes
    //     (*m_msg) << endmsg;
    fclose(lun);
    s_FieldMapsRead = true;
}

// **********************************************************
void EnergyCalculator::PrepareFieldMap(void){
// **********************************************************
  //Selects points which are in the range  -0<z<QuarterWaveLength;
  //Set weights to 1 for points in LAr if they were not defined
  //previously;
  //Map array is too long(not optimal) because:
  //  -the array length is fixed,
  //while the layers are described by different number of points
  //(this maybe solved by introducing some type of dynamic
  //memory handling);
  //  -z,y values are not needed at all.
  //The points in the map are strictly ordered, therefore their
  //coordinates can be computed from the point-address and
  // minz,maxz,miny,maxy parameters.

  G4int nredef,nlayer,ncol,ipnt,ipold,i,j,foldtype,foldtypemx;
  G4double minz,y,z,w,wx;
  G4int npoints; G4double maxz=0.;

  (*m_msg) << MSG::INFO
	 << "PrepareFieldMap for solidtype = "
	 << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())
	 << endmsg;

  nlayer = m_ChCollWheelType->NumberOfRadialLayer; // should be the same for Fold0 and Fold1
   foldtypemx=1;
   if( m_ChCollWheelType->Fold1.FieldMapPrepared &&
       m_ChCollWheelType->Fold0.FieldMapPrepared) foldtypemx=2;

   for(foldtype  = 0; foldtype  < foldtypemx; ++ foldtype){

     if(foldtype  == 0 ) m_ChCollFoldType  = &(m_ChCollWheelType->Fold1);  //normal fold
     if(foldtype  == 1 ) m_ChCollFoldType  = &(m_ChCollWheelType->Fold0); //very first fold

      for(i=0; i<nlayer; ++i){

	if(foldtype == 0) {  //normal fold
          minz=m_ChCollFoldType->MinZofLayer[i];
          maxz=lwc()->GetQuarterWaveLength();
          ncol=G4int( (maxz-minz)/s_GridSize+1.1 );
          m_ChCollFoldType->NofColofLayer[i]=ncol;
          m_ChCollFoldType->MaxZofLayer[i]=lwc()->GetQuarterWaveLength();
	}

        ipnt=-1;  // new counter for numbering the selected points
        nredef=0; //counter to know how many weights are redefined to be 1 (only for V00)
	npoints=m_ChCollFoldType->NofPointsinLayer[i];
	if(npoints>0){
        for(j=0;j<npoints;++j){

	  z=m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,0,j)];
	  y=m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,1,j)];
	  w=m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,2,j)];
	  wx=w;

	  ipold=ipnt;

	  if(foldtype == 1 ){  // very first fold
	    ++ipnt;
	    if(wx < 0.00001 )  wx=0.;
	  }
	  else{  // normal folds
	    if(z<maxz){  // only grid points within quarterwavelength is to be included
	      ++ipnt;
	      if(w<0.00001){  // check weight in the file
		  if(s_FieldMapVersion != "v00") wx=0.; // new map
                  else{                               // old map
		    SetHalfWave(lwc()->GetStraightStartSection()+m_WaveLength+z);
		    m_PointFoldMapArea=1;
		    SetYlimitsofPhigapinFieldMap(i);
     		    if((y>=m_Ylimits[0] && y<=m_Ylimits[1]) ||
		       (y>=m_Ylimits[2] && y<=m_Ylimits[3])){  //point in LAr
	              wx=1.;     // Dice computes the m_Ylimits at (z+GridShift) value,
	              ++nredef;  // I don't know why...By neglecting GridShift nredef
	                         // is much less.
		    }
		  }// endif for old map
	      } //endif for  w<0
	    }// endif for z<maxz
	  }//end for normal fold

	  if(ipold != ipnt){

	    m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,0,ipnt)]=z;
	    m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,1,ipnt)]=y;
	    m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,2,ipnt)]=wx;

/*	    if(ipnt<10)
	      G4cout<<" ilayer="<<i<<" ipnt="<<ipnt<<" weights="
		    <<m_ChCollFoldType->FieldMap[Index(m_ChCollFoldType,i,2,ipnt)]
		    <<G4endl;*/
	  }

        }  // end j loop for points
	}
	if(ipnt+1 != m_ChCollFoldType->NofColofLayer[i]*m_ChCollFoldType->NofRowofLayer[i] ){
	     (*m_msg) << MSG::FATAL
		    <<" ERROR: ipnt+1="<<ipnt+1<<" npoints="<<
	       m_ChCollFoldType->NofColofLayer[i]*m_ChCollFoldType->NofRowofLayer[i]
		    <<endmsg;
	}
     } //end i loop for layers
   } //end for foldtype
} //end of function

// ****************************************************************************
G4double EnergyCalculator::GetHV_Value_ChColl_Wheel(
		       const G4ThreeVector& p, G4int phigap, G4int phihalfgap) const{
// ****************************************************************************
    const G4int atlasside = (lwc()->GetAtlasZside() > 0) ? 0 : 1;

    const G4ThreeVector pforeta= G4ThreeVector(p.x(),p.y(),p.z()+lwc()->GetElecFocaltoWRP()+lwc()->GetdWRPtoFrontFace());
    const G4double eta=pforeta.pseudoRapidity();
    G4int etasection=-1;
    for(G4int i=1;i<=s_NofEtaSection;++i){
      if(eta<=s_HV_Etalim[i]) {etasection=i-1;break;}
    }

    if(!(etasection>=0 && etasection <=s_NofEtaSection-1)) throw std::runtime_error("Index out of range");
    //assert(etasection>=0 && etasection <=s_NofEtaSection-1);
                               /*(right side of e large phi)*/ /*left side of electrode(small phi)*/
    const G4int electrodeside = (phihalfgap%2 == 0 ) ?   1   :   0  ;  

    const G4int firstelectrode=s_HV_Start_phi[atlasside][etasection][electrodeside];

    if(!( firstelectrode>=0 && firstelectrode<= lwc()->GetNumberOfFans()-1)){
		(*m_msg) << MSG::FATAL <<" GetCurrent:Electrode number is out of range" << endmsg;
		G4Exception("EnergyCalculator", "ElectrodeOutOfRange", FatalException, "GetCurrent: Electrode number is out of range");
    }
    G4int electrodeindex=(phigap-1)-firstelectrode;
    if(electrodeindex<0) electrodeindex=electrodeindex+lwc()->GetNumberOfFans();
    const G4int nofelectrodesinphisection=lwc()->GetNumberOfFans()/m_NofPhiSections;//24(8) for outer(inner) wheel
    const G4int phisection=electrodeindex/nofelectrodesinphisection;

    if(!(phisection>=0 && phisection<=m_NofPhiSections-1)){
		(*m_msg) << MSG::FATAL <<" GetCurrent::Electrode number is out of range" << endmsg;
		G4Exception("EnergyCalculator", "ElectrodeOutOfRange", FatalException,"GetCurrent: Electrode number is out of range");
    }

    G4double HV_value= s_HV_Values[atlasside][etasection][electrodeside][phisection];

    return HV_value;
}


// **********************************************************************
void EnergyCalculator::TransFromBarrtoWheel(G4double vb[], G4double v[]){
// **********************************************************************
// this is valid for BacOuterBarrett
// vb: point coord. in the local system of the BackOuterBarrett:PhiDiv
// v : point coord. in the Wheel's system (extended in - and + z direction)
// ellenorizni, hogy p.phi()  0-2pi kozott valtozik-e!!

  const G4ThreeVector pb=G4ThreeVector(vb[0],vb[1],vb[2]);
  v[2]=lwc()->GetWheelThickness()+lwc()->GetdWRPtoFrontFace()*0.5-vb[2];
  const G4double rb   = pb.perp();
  const G4double phib = pb.phi();                     // local phi coord. of a point in the PhiDiv
  G4double phi_inb = s_PhiStartOfPhiDiv + phib;  //s_PhiStartOfPhiDiv defined in FindIdentifier!!
                                             // it is the phi pos. of the PhiDiv in the Barrette system
                                           //phi_inb is the point's phi coord. in the BArrette volume
                                          // it can be bigger than 2pi!
  if(lwc()->type() == LArWheelCalculator::OuterAbsorberWheel ){
    if(phi_inb<0.)      phi_inb=  phi_inb + CLHEP::twopi;
    if(phi_inb>CLHEP::twopi) phi_inb=  phi_inb - CLHEP::twopi;
    phi_inb = CLHEP::twopi - phi_inb;     //now:phi_inb is the  phi coord in the Wheel's system
  }
  else if(lwc()->type() == LArWheelCalculator::OuterAbsorberModule) {
    phi_inb = CLHEP::pi - phi_inb;        //now:phi_inb is the  point's phi coord in the Wheel's system;
  }else{                             // the transfromation is different bec. the back support
                                     // positioned with an extra rotation into the Mother

    (*m_msg) << MSG::FATAL <<" ERROR: TransFromBarrtoWheel: type("<<LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())<<") is unknown"
	   <<endmsg;
  }

  v[0]=rb*cos(phi_inb);
  v[1]=rb*sin(phi_inb);
}

// **********************************************************************
G4double EnergyCalculator::GetCurrent(
	     G4double StartofStep[],G4double EndofStep[],G4double edep) { // need to make const
// **********************************************************************
  G4double v1b[3],v2b[3],vstepb[3]/*,vstepx[3]*/,hvpoint[3];
  G4double current/*,z1,z2*/,v1[3],v2[3],vstep[3],vmap[3];//,vmid[3],vmidmap[3];
  G4double steplength,Ymid,yratiox/*,rvmid*/,gap;
  G4double yratio,yonlowerlayer,yonupperlayer,weightdown,weightup,r,dr;//,weight;
  G4double ds/*,curdown,curup*/,cur,rvstep/*,halflargapsizefrommap*/,rforalpha;
  //G4double cura,curb,currenta,currentb;
  G4double /*efield,efieldnormalized,*/tol,shift;
  G4double yshift_on_map,yshift_on_wheel,cylgapcorr;

  G4int i,j/*,yup*/,irlayer,igap1,igap2,ihalfgap1,ihalfgap2,gapup,gaperr;

  yratio=0.,yonlowerlayer=0.; yonupperlayer=0.; //to avoid compiler warning

  tol=0.1*CLHEP::mm; // tolerance for geom. check
  gaperr=0;

  if(lwc()->GetisBarrette()){
    for(i=0;i<3;++i){
      v1b[i]=StartofStep[i];
      v2b[i]=EndofStep[i];
    }
    TransFromBarrtoWheel(v1b,v1);
    TransFromBarrtoWheel(v2b,v2);
  }
  else{
    for(i=0;i<3;++i){
      v1[i]=StartofStep[i];
      v2[i]=EndofStep[i];
    }
  }

  SetHalfWave(v2[2]); //check whether start and endpoints are in the same gap
  GetPhiGap(v2);
  igap2    = m_PhiGapNumber;
  ihalfgap2= m_PhiHalfGapNumber;

  SetHalfWave(v1[2]);
  GetPhiGap(v1);
  SetFoldArea(v1[2]);
  igap1    = m_PhiGapNumber;  // from 1 to NumberOfFans
  ihalfgap1= m_PhiHalfGapNumber;

  if( (igap1 != igap2) || (ihalfgap1 != ihalfgap2) )  gaperr = -1;

  m_chcollPhiGap=igap1-1;

// define electrode side on the field map;
// start point is used, because G4 does not garantee that the middlepoint is in the LAr gap

  TransformWheeltoFieldMap(v1,vmap);
  r=sqrt(v1[0]*v1[0]+v1[1]*v1[1]);
  r=sqrt(r*r-vmap[1]*vmap[1]);

  shift = lwc()->GetStraightStartSection();
  if(m_PointFoldMapArea != 0) shift += m_WaveLength;
  SetHalfWave(shift+vmap[2]);
  SetYlimitsofPhigapinWheel(r,r); // get the y limits of LAr gap

  Ymid=0.5*(m_Ylimits[1]+m_Ylimits[2]);
  if(vmap[1]>Ymid) gapup=+1;          //step is above electrode
  else gapup=-1;                      //step is below electrode
                                       //this should be true for the full step

  if( vmap[1] > m_Ylimits[3]+tol || vmap[1] < m_Ylimits[0]-tol ||
     (vmap[1] < m_Ylimits[2]-tol && vmap[1] > m_Ylimits[1]+tol)  ) gaperr=gaperr-10;

//loop for substeps

  steplength=sqrt((v1[0]-v2[0])*(v1[0]-v2[0])+(v1[1]-v2[1])*(v1[1]-v2[1])+
                  (v1[2]-v2[2])*(v1[2]-v2[2]));
  const G4int nofstep=int(steplength/s_GridSize)+1;  // step is divided to substeps
  current=0.;                          //current to be returned

  const G4double inv_nofstep = 1. / static_cast<double>(nofstep);
  for(i=0;i<nofstep;++i){

     G4double supcorr =1.;

// compute substep point
     ds=(i+0.5)*inv_nofstep;
     for(j=0;j<3;++j){
       vstep[j]=v1[j]*(1.-ds)+v2[j]*ds;    // get the middle point of substep
       hvpoint[j]=vstep[j];                // hvpoint is always in local coord. Wheel or PhiDiv)
       if(lwc()->GetisBarrette()){
		  vstepb[j]=v1b[j]*(1.-ds)+v2b[j]*ds;
		  hvpoint[j]=vstepb[j];
       }
     }

     G4double HV_value;
     if(lwc()->GetisBarrette())
       HV_value=GetHV_Value_Barrett(
                G4ThreeVector(hvpoint[0],hvpoint[1],hvpoint[2]));
     else
       HV_value=GetHV_Value_ChColl_Wheel(
                G4ThreeVector(hvpoint[0],hvpoint[1],hvpoint[2]),igap1,ihalfgap1);

     G4ThreeVector tmp = G4ThreeVector( hvpoint[0],hvpoint[1],hvpoint[2]);
     G4double dte = (this->*m_distance_to_the_nearest_electrode_type)(tmp);

     if(fabs(dte) < s_CHC_Esr) continue; //skip point if too close to the electrode

     G4double agap=(this->*m_GetGapSize_type)(tmp);    //correction to electrode suppression not to
     G4double x= agap/(agap-s_CHC_Esr);     // change av. signal in the gap
     if(x >=0.) supcorr=x;

     SetFoldArea(vstep[2]);                 // set fold type
     SetHalfWave(vstep[2]);                 // set halfwave parameters for substep
     TransformWheeltoFieldMap(vstep,vmap);  //get corresponding point in Map
                                            //system (it is equivalent to the
                                            //halfwave system of pos. slope)
     rvstep   =sqrt(vstep[0]*vstep[0]+vstep[1]*vstep[1]);
     rforalpha=sqrt(rvstep*rvstep-vmap[1]*vmap[1]); // radius in the electrode system
     gap=HalfLArGapSize(rforalpha,rforalpha);       // gapsize in the straight section
     yshift_on_map  = rforalpha*M_PI/lwc()->GetNumberOfFans()-(m_FanAbsThickness+m_FanEleThickness)/2.;
     yshift_on_wheel=    rvstep*M_PI/lwc()->GetNumberOfFans()-(m_FanAbsThickness+m_FanEleThickness)/2.;
     cylgapcorr=yshift_on_wheel/yshift_on_map; // scale difference between plane and cylindrical surface
     /*
     std::cout<< " GetCurrent0**Nabs="<<lwc()->GetNumberOfFans()<<" absthick="<<m_FanAbsThickness
	      <<" elethick="<<m_FanEleThickness
	      <<" cylgapcorr-1="<<cylgapcorr-1
	      <<" ZinHalfWave="<<m_ZinHalfWave<<" HalfWaveNumber="<<m_HalfWaveNumber
	      <<std::endl;
     */
//search for radial layers the substep is in between;

     irlayer=0;
     G4int numberofradiallayers=m_ChCollWheelType->NumberOfRadialLayer;
     for(j=0;j<numberofradiallayers;++j){
       if(rforalpha<m_ChCollWheelType->RadiusOfLayers[j]) {
	 irlayer=j-1;
	 break;}
     }
     assert(irlayer>=0 && irlayer<numberofradiallayers-1);

// check whether interpolation is needed

     if(!m_ChCollFoldType->FieldMapPrepared ||                // fieldmap is not available for this fold
	(vmap[2]<m_ChCollFoldType->MinZofLayer[irlayer]   &&  // or out of z range of both maps
	 vmap[2]<m_ChCollFoldType->MinZofLayer[irlayer+1]) ){

       cur=1.;  // interpolation not needed
     }
     else{        // interpolation needed

// compute radial interpolation parameter

       dr=(rforalpha-m_ChCollWheelType->RadiusOfLayers[irlayer])
       /(m_ChCollWheelType->RadiusOfLayers[irlayer+1]-
         m_ChCollWheelType->RadiusOfLayers[irlayer]);

// get  m_Ylimits  limiting the LAr gap along the y axis;

       shift = lwc()->GetStraightStartSection();
       if(m_PointFoldMapArea != 0) shift += m_WaveLength;
       SetHalfWave(shift+vmap[2]);
       SetYlimitsofPhigapinWheel(rforalpha,rforalpha);

//check geom. err condition if point is outside of LAr gap
       if( gaperr > -100  &&
	   ( vmap[1] > m_Ylimits[3]+tol || vmap[1] < m_Ylimits[0]-tol ||
            (vmap[1] < m_Ylimits[2]-tol && vmap[1] > m_Ylimits[1]+tol) )) gaperr=gaperr-100;

// get relative y coordinate

       if      (gapup==-1) yratio=(vmap[1]- m_Ylimits[0])/(m_Ylimits[1]- m_Ylimits[0]);
       else if (gapup==+1) yratio=(vmap[1]- m_Ylimits[2])/(m_Ylimits[3]- m_Ylimits[2]);

       yratiox=yratio;
       if(yratiox<=0.)      yratio=0.00001;  // pull the point into the gap if it wouldn't be there;
       else if(yratiox>=1.) yratio=0.99999;  // this may happen bec.G4 does not grantee that the full
                                             // step-line is within the same volume
// get corresponding y coordinates on the radial layers
// where the weight is to be taken from;

       SetYlimitsofPhigapinFieldMap(irlayer);    //on the lower layer

       if    (gapup==-1)
	 yonlowerlayer=m_Ylimits[0]*(1.-yratio)+m_Ylimits[1]*yratio;
       else if(gapup==1)
	 yonlowerlayer=m_Ylimits[2]*(1.-yratio)+m_Ylimits[3]*yratio;

       SetYlimitsofPhigapinFieldMap(irlayer+1);  //on the upper layer

       if(gapup==-1)
	 yonupperlayer=m_Ylimits[0]*(1.-yratio)+m_Ylimits[1]*yratio;
       else if(gapup==1)
	 yonupperlayer=m_Ylimits[2]*(1.-yratio)+m_Ylimits[3]*yratio;

// get weights from the maps of lower and upper layer

       weightdown=GetWeightfromFieldMap(irlayer,  vmap[2],yonlowerlayer);
       weightup  =GetWeightfromFieldMap(irlayer+1,vmap[2],yonupperlayer);

       // get the interpolated normalized Efield alias cur
       
       cur=weightdown*(1.-dr)+weightup*dr;
       
     }  // end of interpolation
     
// old current calculation: edep*E**1.3*(gap0/gap)**1.3
//    normalized so, that signal=edep in the straight section of gap0=1.3mm
//                  (it is at eta=2.25, r=787mm, foldingangle~110 degree)
//    currentb=currentb+(edep/nofstep)*pow(curb*AverageGap/gap,1.3);
//(25-05-2005) new current calculation: edep*1/U*IonReco*E*v_drift
//   normalized so that signal=edep in the straight section of the same gap
//                             as above;
//       where: HV=1250V,E=9.615 [kv/cm] ,T=88.16 K,
//       vdrift=4.62 [mm/mikrosec],  Ionreco=96.4%
// ==> AverageCurrent=3.425/[mikrosec];

     G4double efield=0.01*cur*HV_value/gap/cylgapcorr; // [kV/cm], bec.HV[V],gap[mm]
     
     double dc = (edep*inv_nofstep)/s_AverageCurrent*
       cur/gap/cylgapcorr*IonReco(efield)*DriftVelo(s_LArTemperature_av,efield)*
       supcorr;
     
     current += dc;
  } // end of loop for substeps
  
  if(current < 0.) {
    gaperr=gaperr-1000;
    current=0.;
  }
  if(gaperr != 0 ) {
    s_CHCEbad=s_CHCEbad+edep;
    if(s_CHCIprint < s_CHCMaxPrint ){
      s_CHCIprint=s_CHCIprint+1;
      (*m_msg) << MSG::WARNING
	       <<"GetCurrent has strange step,gaperr="<<gaperr<<" correction still computed"
	       <<" bad edep ratio="<<s_CHCEbad/s_CHCEtotal
	       << endmsg;
    }
  }

  //std::cout <<"GetCurrent0:: edep="<<edep<<" current="<<current<<" gaperr="<<gaperr<<std::endl;
  
  return current;
}

// **********************************************************
G4double EnergyCalculator::GetWeightfromFieldMap(
                             G4int ilayer, G4double z, G4double y){
// **********************************************************
  //return interpolated weight from the fieldmap;
  // inp: ilayer= layernumber;
  //      z,y     = z and y coord. of the point where the weight
  //                is to be computed;
  //                they are defined in the coord system of the Map;
  //  interpolated weight =1 by default;

  G4int i,icol,irow,ip11,ip12,ip21,ip22;
  G4double weight,eps;
  G4double w11,w12,w21,w22,z11/*,z12,z21,z22*/,y11/*,y12,y21,y22*/,wdown,wup,
         w,dz,dy,dzx,dyx;

  w11=1.; w12=1.; w21=1.; w22=1.; z11=0.; y11=0.; // to avoid compiler warning

    weight=1.;
    eps=0.0005;
    i=ilayer;

    //check whether the point is in the area of validity of the table
    if(z<m_ChCollFoldType->MinZofLayer[i]-eps || z>m_ChCollFoldType->MaxZofLayer[i]+eps ||
       y<m_ChCollFoldType->MinYofLayer[i]-eps || y>m_ChCollFoldType->MaxYofLayer[i]+eps ){
      return weight;
    }

    //Search for the 4 points in the grid surrounding P(z,y).
    //Index of points in the FieldMap:
    //   (21)----(22)
    //     | P    |
    //   (11)----(12)
    //If some of them is outside of LAr their weight=0 by default.
    //Weights set to zero by hand for those surrounding points which
    //have no correspondant grid point in the table.
    //Such situation will happen for example at the z border,
    //because the z coordinate of the last
    //column is less then QuarteWaveLength by 50 and 67 mikron for
    //inner and outer wheel respectively;
    //Only gridpoints of nonzero weight are taken into account
    //for interpolation.
    //The case of all 4 weights are zero should not happen. If still,
    // interpolated weight =1 is returned.


    icol=int((z-m_ChCollFoldType->MinZofLayer[i])/s_GridSize)+1; //column number of left corners

    if(icol>m_ChCollFoldType->NofColofLayer[i]) icol=m_ChCollFoldType->NofColofLayer[i];

    irow=int((y-m_ChCollFoldType->MinYofLayer[i])/s_GridSize)+1; //row number of lower corners
    if(irow>m_ChCollFoldType->NofRowofLayer[i]) irow=m_ChCollFoldType->NofRowofLayer[i];

    assert(icol>0 && irow>0);

    ip11=(icol-1)*m_ChCollFoldType->NofRowofLayer[i]+irow-1; //index of points in the FieldMap
    ip21=ip11+1;                                             //   (21)----(22)
    ip12= icol   *m_ChCollFoldType->NofRowofLayer[i]+irow-1;  //   (11)----(12)
    ip22=ip12+1;

    G4int i2ip11=Index(m_ChCollFoldType,i,2,ip11);
    G4int i2ip12=Index(m_ChCollFoldType,i,2,ip12);
    G4int i2ip21=Index(m_ChCollFoldType,i,2,ip21);
    G4int i2ip22=Index(m_ChCollFoldType,i,2,ip22);
    G4int i0ip11=Index(m_ChCollFoldType,i,0,ip11);
    //G4int i0ip12=Index(m_ChCollFoldType,i,0,ip12);
    //G4int i0ip21=Index(m_ChCollFoldType,i,0,ip21);
    //G4int i0ip22=Index(m_ChCollFoldType,i,0,ip22);
    G4int i1ip11=Index(m_ChCollFoldType,i,1,ip11);
    //G4int i1ip12=Index(m_ChCollFoldType,i,1,ip12);
    //G4int i1ip21=Index(m_ChCollFoldType,i,1,ip21);
    //G4int i1ip22=Index(m_ChCollFoldType,i,1,ip22);


    if(icol<m_ChCollFoldType->NofColofLayer[i] && irow<m_ChCollFoldType->NofRowofLayer[i]){
      w11=m_ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=m_ChCollFoldType->FieldMap[i2ip12]; //[i][2][ip12];
      w21=m_ChCollFoldType->FieldMap[i2ip21]; //[i][2][ip21];
      w22=m_ChCollFoldType->FieldMap[i2ip22]; //[i][2][ip22];
      z11=m_ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=m_ChCollFoldType->FieldMap[i0ip12]; //[i][0][ip12];
      //z21=m_ChCollFoldType->FieldMap[i0ip21]; //[i][0][ip21];
      //z22=m_ChCollFoldType->FieldMap[i0ip22]; //[i][0][ip22];
      y11=m_ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=m_ChCollFoldType->FieldMap[i1ip12]; //[i][1][ip12];
      //y21=m_ChCollFoldType->FieldMap[i1ip21]; //[i][1][ip21];
      //y22=m_ChCollFoldType->FieldMap[i1ip22]; //[i][1][ip22];
    }
    else if(icol==m_ChCollFoldType->NofColofLayer[i] && irow<m_ChCollFoldType->NofRowofLayer[i]){
      w11=m_ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=0.;
      w21=m_ChCollFoldType->FieldMap[i2ip21]; //[i][2][ip21];
      w22=0.;
      z11=m_ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=z11+s_GridSize;
      //z21=m_ChCollFoldType->FieldMap[i0ip21]; //[i][0][ip21];
      //z22=z11+s_GridSize;
      y11=m_ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=y11;
      //y21=m_ChCollFoldType->FieldMap[i1ip21]; //[i][1][ip21];
      //y22=y11+s_GridSize;
    }
    else if(icol<m_ChCollFoldType->NofColofLayer[i] && irow==m_ChCollFoldType->NofRowofLayer[i]){
      w11=m_ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=m_ChCollFoldType->FieldMap[i2ip12]; //[i][2][ip12];
      w21=0.;
      w22=0.;
      z11=m_ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=m_ChCollFoldType->FieldMap[i0ip12]; //[i][0][ip12];
      //z21=z11;
      //z22=z11+s_GridSize;
      y11=m_ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=m_ChCollFoldType->FieldMap[i1ip12]; //[i][1][ip12];
      //y21=y11+s_GridSize;
      //y22=y11+s_GridSize;
    }
    else if(icol==m_ChCollFoldType->NofColofLayer[i] && irow==m_ChCollFoldType->NofRowofLayer[i]){
      w11=m_ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=0.;
      w21=0.;
      w22=0.;
      z11=m_ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=z11+s_GridSize;
      //z21=z11;
      //z22=z11+s_GridSize;
      y11=m_ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=y11;
      //y21=y11+s_GridSize;
      //y22=y11+s_GridSize;
    }

    dz=(z-z11)/s_GridSize;
    dy=(y-y11)/s_GridSize;

    if(dz<0.) dz=0.;
    if(dz>1.) dz=1.;
    if(dy<0.) dy=0.;
    if(dy>1.) dy=1.;

    dzx=dz;
    if(w11<0.000001) dzx=1.;
    if(w12<0.000001) dzx=0.;
    wdown=w11*(1.-dzx)+w12*dzx;  //interpol. along lower line
    dzx=dz;
    if(w21<0.000001) dzx=1.;
    if(w22<0.000001) dzx=0.;
    wup=w21*(1.-dzx)+w22*dzx;  //interpol. along upper line

    dyx=dy;
    if(wdown<0.000001) dyx=1.;
    if(wup  <0.000001) dyx=0.;
    w=wdown*(1.-dyx)+wup*dyx;   //interpol along column
    if(w>0.000001) weight=w;

    return weight;
}

// ***********************************************************
void EnergyCalculator::SetFoldArea(G4double zinwheel){
// ***********************************************************
  if(zinwheel > lwc()->GetStraightStartSection()+lwc()->GetQuarterWaveLength()*0.5 &&
     zinwheel < lwc()->GetWheelThickness()-lwc()->GetStraightStartSection()-lwc()->GetQuarterWaveLength()*0.5){
    m_PointFoldMapArea=1;
    m_ChCollFoldType=&(m_ChCollWheelType->Fold1);
  }
  else{
    m_PointFoldMapArea=0;
    m_ChCollFoldType=&(m_ChCollWheelType->Fold0);
  }
}
// ***********************************************************
void EnergyCalculator::SetHalfWave(G4double zinwheel){
// ***********************************************************

//  G4cout<<"***SetHalfWave zin="<<zinwheel<<G4endl;

  G4double z;
  z=zinwheel - lwc()->GetStraightStartSection();
  m_HalfWaveNumber  =int((z+lwc()->GetQuarterWaveLength())/lwc()->GetHalfWaveLength());
  m_ZinHalfWave=z-m_HalfWaveNumber*lwc()->GetHalfWaveLength();
  m_SignofZinHalfWave=+1;
  if(m_ZinHalfWave<0.) m_SignofZinHalfWave=-1;
  m_SignofSlopeofHalfWave=-1;
  if(m_HalfWaveNumber %2 == 0) m_SignofSlopeofHalfWave=+1;

}

// ***********************************************************
void EnergyCalculator::TransformWheeltoFieldMap(
               G4double PointinWheel[], G4double PointinFieldMap[]){
// ***********************************************************
  // fieldmap is defined for
  //  --  'normal' folds  :
  //      in the coord syst. of a halfwave of pos.slope
  //      in the range of 0<z<QuarterWaveLength;
  //  --  0th and last fold in the range -13mm < z < QuarterWaveLength/2.
  // SetHalfWave() and GetPhiGap() should be called previously;

  PointinFieldMap[0]=PointinWheel[0]*  m_CosPhiGap +PointinWheel[1]*m_SinPhiGap;
  PointinFieldMap[1]=PointinWheel[0]*(-m_SinPhiGap)+PointinWheel[1]*m_CosPhiGap;

  if(m_PointFoldMapArea == 0){
    if(m_HalfWaveNumber == 0 )             /*0th fold*/
      PointinFieldMap[2]= m_ZinHalfWave;
    else{                                /*last fold*/
    PointinFieldMap[1]=-PointinFieldMap[1];
    PointinFieldMap[2]= -m_ZinHalfWave;
    }
  }
  else{    // normal folds
    PointinFieldMap[1]=m_SignofZinHalfWave*m_SignofSlopeofHalfWave
                       *PointinFieldMap[1];
    PointinFieldMap[2]=fabs(m_ZinHalfWave);
  }
}

// *********************************************************************
void EnergyCalculator::SetYlimitsofPhigapinFieldMap(G4int ilayer){
// *********************************************************************
  // compute limits of LAr gap along y axis in the system of Halfwaves
  //  using the geometry defined at the time when fieldmap was created;
  //   
  // SetHalfWave, GetPhiGap, TransformWheeltoFieldMap should be called previously

  G4double /*y[4],*/alpha/*,s*/,halfgap;//,eleshift,absshift;

  alpha=m_ChCollWheelType->FoldinAngleOfLayers[ilayer] * (CLHEP::deg / CLHEP::rad);
  //s=sin(alpha/2.);
  halfgap=m_ChCollWheelType->RadiusOfLayers[ilayer]*CLHEP::pi/lwc()->GetNumberOfFans(); //half of y distance

  m_Ylimits[3]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-m_FanAbsThickness)+halfgap;
  m_Ylimits[2]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+m_FanEleThickness);
  m_Ylimits[1]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-m_FanEleThickness);
  m_Ylimits[0]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+m_FanAbsThickness)-halfgap;
}

// ******************************************************************
void EnergyCalculator::SetYlimitsofPhigapinWheel(
                   G4double rforpoint, G4double rforalpha){
// ******************************************************************
  // compute limits of LAr gap in the system of Halfwaves using the
  // present geometry of the wheels;
  // valid in 0<z<QuaterWavelength for normal fold;
  // and   in -13mm<z<QuaterWavelength/2. for the 0th fold
  // SetHalfWave GetPhiGap and
  // TransformWheeltoFieldMap should be called previously

  G4double /*y[4],*/alpha/*,s*/,halfgap;//,eleshift,absshift;

  alpha=FoldingAngle(rforalpha);
  //s=sin(alpha*0.5);
  halfgap=rforpoint*CLHEP::pi/lwc()->GetNumberOfFans(); //half of y distance between 2 abs.

  m_Ylimits[3]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-m_FanAbsThickness)+halfgap;
  m_Ylimits[2]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+m_FanEleThickness);
  m_Ylimits[1]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-m_FanEleThickness);
  m_Ylimits[0]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+m_FanAbsThickness)-halfgap;
}


// ***********************************************************
void EnergyCalculator::GetPhiGap(G4double SpacePoint[]){  // need to make const
// ***********************************************************
// inp: x,y of SpacePoint defined in the Wheel's system
// outp: gap number(=1,2,..NumberOfFans);
//       first gap is at phi=0 +- FanStepOnPhi/2
//     : HalfGapNumber, number of half LAr gaps,
//          indexed from 1 to 2*NumberOfFans
//          1st and 2nd halfgaps are in the 1st gap
// SetHalfWave(z) should be called previously

  G4double x,y,r,r2,phiofSpacePoint,y0,phiCross,dphi,phi;
  G4int phigap,phihalfgap;

  x=SpacePoint[0];
  y=SpacePoint[1];
  r2=x*x+y*y;
  r=sqrt(r2);
  phiofSpacePoint=atan2(y,x);   //-pi<phi<+pi;
  if(phiofSpacePoint<0.) phiofSpacePoint=CLHEP::twopi+phiofSpacePoint;
  assert(phiofSpacePoint>0. && phiofSpacePoint<CLHEP::twopi);

  //compute the crossingpoint of the circle of radius r on the x-y plane
  // with the surface of the neutral fibre of the fan 1.
  // fan 1 positioned at phi=0. One iteration gives precise result.
  // It was checked by solving exact equations.

  y0=YofNeutralFibre(FoldingAngle(r),lwc()->GetFanFoldRadius());
  r =sqrt(r2-y0*y0); // this is an approximation, based on 2 conditons:
                        // surface tangent in the transvrsal plane is almost
                        // pointing to the beamline,
                        // (rold-rnew)/rold << 1.
  y0=YofNeutralFibre(FoldingAngle(r),lwc()->GetFanFoldRadius());
  phiCross=atan(y0/r);  //-pi<phi<+pi; // asin is used in Dice - it is a bug
  phiCross=phiCross-lwc()->GetFanStepOnPhi()/2.;
  if(phiCross<0.) phiCross=CLHEP::twopi+phiCross;
                 //position of the crossing point
                 //on the neutral fibre of the left absorber of the first gap
  dphi=phiofSpacePoint-phiCross;
  if(dphi<0.) dphi=CLHEP::twopi+dphi; //distance in phi from the SpacePoint to the
                                //point on the left absorber's neu.fibre
                                //at the same radius
  assert (dphi>=0. && dphi<=CLHEP::twopi);

  phigap=int(dphi/lwc()->GetFanStepOnPhi())+1;
  assert(phigap>=1 && phigap<=lwc()->GetNumberOfFans());

  phihalfgap=int(2.*dphi/lwc()->GetFanStepOnPhi())+1;
  assert(phihalfgap>=1 && phihalfgap<=2*lwc()->GetNumberOfFans());

  m_PhiGapNumber=phigap;
  m_PhiHalfGapNumber=phihalfgap;
  phi=(phigap-1)*lwc()->GetFanStepOnPhi();
  m_CosPhiGap=cos(phi);
  m_SinPhiGap=sin(phi);
}
// ***********************************************************************************
G4double EnergyCalculator::YofSurface(G4double alpha,G4double rho,G4double thickness) const {
// ***********************************************************************************
// compute y coord of of the fan surface 
//
// coord system of halfwave is used
//  y axis: parallel to y axis of ATLAS
//  z axis: parallel to z axis of Atlas
//          z is limited by: -wavelength/4 < z < +wavelength/4
//  origin of the system is fixed  at the node of the halfwave
//  inp:
//    halfwave parameters: should be set previously by SetHalfWave();
//    m_ZinHalfWave :z coord of a point where YofSurf is to be computed
//    alpha :folding angle[rad]
//    rho   : curvature radius of the fold
//    thickness: >0 --> compute y of the  upper surface
//               =0 --> compute y of the neutral fibre
//               <0 --> compute y of the lower surface


  const G4double alp=alpha/2.;
  const G4double s=sin(alp);    //sin(alpha/2.)
  const G4double c=cos(alp);    //cos(alpha/2.)
  const G4double t=s/c;         //tan(alpha/2);
  const G4double tb=(1.-s)/c;   //tan(beta); beta=0.5*(90-alpha/2);
  const G4double th=thickness/2.;
  
  if(m_HalfWaveNumber==0){                  //0.th 'halfwave'

    const G4double zminoffirstfold   = -rho*tb;
    const G4double zmaxoffirstfold   = -rho*tb+(rho-th)*tb*(1.+s);
    const G4double zminofsecondfold  =  lwc()->GetQuarterWaveLength()-(rho+th)*c;
    
    if(m_ZinHalfWave<=zminoffirstfold) return th;
    if(m_ZinHalfWave<=zmaxoffirstfold) return rho-sqrt( pow((rho-th),2)-pow((m_ZinHalfWave+rho*tb),2) );
    if(m_ZinHalfWave<=zminofsecondfold)return m_ZinHalfWave/t+th/s;
    return (lwc()->GetQuarterWaveLength()*c-rho)/s + sqrt( pow((rho+th),2)-pow((m_ZinHalfWave-lwc()->GetQuarterWaveLength()),2));
    
  } // end of first fold
  
  else if(m_HalfWaveNumber==lwc()->GetNumberOfHalfWaves()){ //last 'halfwave'
    
    const G4double zmaxoflastfold=rho*tb;
    const G4double zminoflastfold=rho*tb-(rho+th)*tb*(1.+s);
    const G4double zmaxofpreviousfold=-lwc()->GetQuarterWaveLength()+(rho-th)*c;
    
    if(m_ZinHalfWave>=zmaxoflastfold)     return th;
    if(m_ZinHalfWave>=zminoflastfold)     return -rho+sqrt( pow((rho+th),2)-pow((m_ZinHalfWave-rho*tb),2) );
    if(m_ZinHalfWave>=zmaxofpreviousfold) return m_ZinHalfWave/t+th/s;
    return -(lwc()->GetQuarterWaveLength()*c-rho)/s-sqrt( pow((rho-th),2)-pow((m_ZinHalfWave+lwc()->GetQuarterWaveLength()),2) );
    
  } // end of last fold
  
  else if(m_HalfWaveNumber>0 && m_HalfWaveNumber<lwc()->GetNumberOfHalfWaves()){
    
    const G4double sn=m_SignofZinHalfWave*m_SignofSlopeofHalfWave; 
    const G4double z2= -lwc()->GetQuarterWaveLength()+(rho-m_SignofSlopeofHalfWave*th)*c;
    const G4double z3=  lwc()->GetQuarterWaveLength()-(rho+m_SignofSlopeofHalfWave*th)*c;

    if(m_ZinHalfWave<=z2 || z3<=m_ZinHalfWave) 
      return sn*(lwc()->GetQuarterWaveLength()*c-rho)/s+
	     sn*sqrt( pow((rho+sn*th),2)-pow((m_ZinHalfWave-m_SignofZinHalfWave*lwc()->GetQuarterWaveLength()),2) );
    
    if(z2<=m_ZinHalfWave && m_ZinHalfWave<=z3) return m_SignofSlopeofHalfWave*m_ZinHalfWave/t+th/s;
     
    //   std::cout<<"*** ERROR1 in YofSURF!!!!"<<std::endl;
    return 0.;    
  }
  //std::cout<<"*** ERROR2 in YofSURF!!!!"<<std::endl;
  return 0.;  
}

// **********************************************************
G4double EnergyCalculator::FoldingAngle(G4double radius) const {
// **********************************************************
  // inp:  radius in [mm]
  // outp:  angle in [radian]
  //
  // WARNING !! This formula does not reproduce the folding angle at
  //    radius=800mm for the inner wheel. Therefore LAr halfgapsize
  //    (folding angle) predicted is bigger by 100mikron (1.6degree).
  //    This may give an unpredictable effect making the interpolation
  //    at charge collection, because one of the tables is defined at
  //    this radius. Fitting parameters should be improved!!
  //    The imprecision for outer wheel at smallest/highest radius
  //    is present as well, however it is smaller by a factor of 2.
  //    In case of 'medium' radia the fitted parameters are ok.

	return CLHEP::pi - 2. * lwc()->parameterized_slant_angle(radius);
}
// ***************************************************************
G4double  EnergyCalculator::HalfLArGapSizeOld(G4double radius) const {
// ***************************************************************
  const G4double alpha=FoldingAngle(radius);
  const G4double s=sin(alpha *0.5);
  const G4double halfgap=CLHEP::pi*radius/lwc()->GetNumberOfFans()*s-(m_FanAbsThickness+m_FanEleThicknessOld)*0.5;
  return halfgap;
}
// ***************************************************************
G4double  EnergyCalculator::HalfLArGapSize(G4double rforpoint,G4double rforalpha) const {
// ***************************************************************
  const G4double alpha=FoldingAngle(rforalpha);
  const G4double s=sin(alpha/2.);
  const G4double halfgap=CLHEP::pi*rforpoint/lwc()->GetNumberOfFans()*s-(m_FanAbsThickness+m_FanEleThickness)/2.;
  return halfgap;
}








