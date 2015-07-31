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
//        to FieldMapVersion variable of the LArEMECEnergyCorrrection.cc      *
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
	       << FieldMapVersion
	       << endreq;
//		  <<std::endl;

       if(FieldMapVersion == "v00" || FieldMapVersion == "v01" ||
          FieldMapVersion == "v02"){

	 ChCollInner.Fold1.FieldMapPrepared=true;
	 ChCollOuter.Fold1.FieldMapPrepared=true;

	 ChCollInner.Fold0.FieldMapPrepared=false;
	 ChCollOuter.Fold0.FieldMapPrepared=false;

       if(FieldMapVersion == "v00") {

	 G4int nlayer=6; // number of radial layers
	 std::cout<<" ***IniGeomforFieldMaps calls CreateArrays"
		  <<std::endl;
      	 CreateArrays(ChCollInner,nlayer);
      	 CreateArrays(ChCollOuter,nlayer);

         ChCollInner.RadiusOfLayers[0]=300.;//[mm]
         ChCollInner.RadiusOfLayers[1]=400.;
         ChCollInner.RadiusOfLayers[2]=500.;
         ChCollInner.RadiusOfLayers[3]=600.;
         ChCollInner.RadiusOfLayers[4]=700.;
         ChCollInner.RadiusOfLayers[5]=800.;

         ChCollOuter.RadiusOfLayers[0]= 600.; //[mm]
         ChCollOuter.RadiusOfLayers[1]= 900.;
         ChCollOuter.RadiusOfLayers[2]=1200.;
         ChCollOuter.RadiusOfLayers[3]=1500.;
         ChCollOuter.RadiusOfLayers[4]=1800.;
         ChCollOuter.RadiusOfLayers[5]=2100.;

         ChCollInner.FoldinAngleOfLayers[0]= 112.7;//[degree]
         ChCollInner.FoldinAngleOfLayers[1]=  92.2;
         ChCollInner.FoldinAngleOfLayers[2]=  78.7;
         ChCollInner.FoldinAngleOfLayers[3]=  68.8;
         ChCollInner.FoldinAngleOfLayers[4]=  61.3;
         ChCollInner.FoldinAngleOfLayers[5]=  55.2;

         ChCollInner.HalfLArGapSizeOfLayers[0]= 1.44; //[mm]
         ChCollInner.HalfLArGapSizeOfLayers[1]= 1.94;
         ChCollInner.HalfLArGapSizeOfLayers[2]= 2.29;
         ChCollInner.HalfLArGapSizeOfLayers[3]= 2.56;
         ChCollInner.HalfLArGapSizeOfLayers[4]= 2.78;
         ChCollInner.HalfLArGapSizeOfLayers[5]= 2.96;

         ChCollOuter.FoldinAngleOfLayers[0]=129.;//[degree]
         ChCollOuter.FoldinAngleOfLayers[1]=101.4;
         ChCollOuter.FoldinAngleOfLayers[2]= 84.8;
         ChCollOuter.FoldinAngleOfLayers[3]= 73.2;
         ChCollOuter.FoldinAngleOfLayers[4]= 64.5;
         ChCollOuter.FoldinAngleOfLayers[5]= 57.7;

         ChCollOuter.HalfLArGapSizeOfLayers[0]=0.87; //[mm]
         ChCollOuter.HalfLArGapSizeOfLayers[1]=1.5;
         ChCollOuter.HalfLArGapSizeOfLayers[2]=1.96;
         ChCollOuter.HalfLArGapSizeOfLayers[3]=2.31;
         ChCollOuter.HalfLArGapSizeOfLayers[4]=2.58;
         ChCollOuter.HalfLArGapSizeOfLayers[5]=2.79;
       }

       if(FieldMapVersion == "v01") {

	 G4int nlayer=6;
      	 CreateArrays(ChCollInner,nlayer);
      	 CreateArrays(ChCollOuter,nlayer);

         ChCollInner.RadiusOfLayers[0]=300.;//[mm]
         ChCollInner.RadiusOfLayers[1]=400.;
         ChCollInner.RadiusOfLayers[2]=500.;
         ChCollInner.RadiusOfLayers[3]=600.;
         ChCollInner.RadiusOfLayers[4]=700.;
         ChCollInner.RadiusOfLayers[5]=800.;

         ChCollOuter.RadiusOfLayers[0]= 600.; //[mm]
         ChCollOuter.RadiusOfLayers[1]= 900.;
         ChCollOuter.RadiusOfLayers[2]=1200.;
         ChCollOuter.RadiusOfLayers[3]=1500.;
         ChCollOuter.RadiusOfLayers[4]=1800.;
         ChCollOuter.RadiusOfLayers[5]=2100.;

         ChCollInner.FoldinAngleOfLayers[0]= 112.742024;  //[degree]
         ChCollInner.FoldinAngleOfLayers[1]=  92.2379512;
         ChCollInner.FoldinAngleOfLayers[2]=  78.6993545;
         ChCollInner.FoldinAngleOfLayers[3]=  68.8423119;
         ChCollInner.FoldinAngleOfLayers[4]=  61.2714745;
         ChCollInner.FoldinAngleOfLayers[5]=  55.2462062;

         ChCollInner.HalfLArGapSizeOfLayers[0]= 1.47806735;  //[mm]
         ChCollInner.HalfLArGapSizeOfLayers[1]= 1.95080191;
         ChCollInner.HalfLArGapSizeOfLayers[2]= 2.30318731;
         ChCollInner.HalfLArGapSizeOfLayers[3]= 2.57486515;
         ChCollInner.HalfLArGapSizeOfLayers[4]= 2.79014569;
         ChCollInner.HalfLArGapSizeOfLayers[5]= 2.96463479;

         ChCollOuter.FoldinAngleOfLayers[0]= 129.00197;  //[degree]
         ChCollOuter.FoldinAngleOfLayers[1]= 101.408128;
         ChCollOuter.FoldinAngleOfLayers[2]=  84.7966382;
         ChCollOuter.FoldinAngleOfLayers[3]=  73.1643989;
         ChCollOuter.FoldinAngleOfLayers[4]=  64.4458572;
         ChCollOuter.FoldinAngleOfLayers[5]=  57.6301588;

         ChCollOuter.HalfLArGapSizeOfLayers[0]= 0.877808149;  //[mm]
         ChCollOuter.HalfLArGapSizeOfLayers[1]= 1.5116158;
         ChCollOuter.HalfLArGapSizeOfLayers[2]= 1.97238634;
         ChCollOuter.HalfLArGapSizeOfLayers[3]= 2.31937997;
         ChCollOuter.HalfLArGapSizeOfLayers[4]= 2.58864056;
         ChCollOuter.HalfLArGapSizeOfLayers[5]= 2.80290925;
       }

       if(FieldMapVersion == "v02") {

	 G4int nilayer=6;  // number of layers for inner/outer wheel
	 G4int nolayer=11;
      	 CreateArrays(ChCollInner,nilayer);
      	 CreateArrays(ChCollOuter,nolayer);

         ChCollInner.RadiusOfLayers[0]=300.;//[mm]
         ChCollInner.RadiusOfLayers[1]=400.;
         ChCollInner.RadiusOfLayers[2]=500.;
         ChCollInner.RadiusOfLayers[3]=600.;
         ChCollInner.RadiusOfLayers[4]=700.;
         ChCollInner.RadiusOfLayers[5]=800.;

         ChCollOuter.RadiusOfLayers[0] = 600.; //[mm]
         ChCollOuter.RadiusOfLayers[1] = 900.;
         ChCollOuter.RadiusOfLayers[2] =1200.;
         ChCollOuter.RadiusOfLayers[3] =1250.;
         ChCollOuter.RadiusOfLayers[4] =1300.;
         ChCollOuter.RadiusOfLayers[5] =1350.;
         ChCollOuter.RadiusOfLayers[6] =1400.;
         ChCollOuter.RadiusOfLayers[7] =1450.;
         ChCollOuter.RadiusOfLayers[8] =1500.;
         ChCollOuter.RadiusOfLayers[9] =1800.;
         ChCollOuter.RadiusOfLayers[10]=2100.;

         ChCollInner.FoldinAngleOfLayers[0]= 112.742024;  //[degree]
         ChCollInner.FoldinAngleOfLayers[1]=  92.2379512;
         ChCollInner.FoldinAngleOfLayers[2]=  78.6993545;
         ChCollInner.FoldinAngleOfLayers[3]=  68.8423119;
         ChCollInner.FoldinAngleOfLayers[4]=  61.2714745;
         ChCollInner.FoldinAngleOfLayers[5]=  55.2462062;

         ChCollInner.HalfLArGapSizeOfLayers[0]= 1.47806735;  //[mm]
         ChCollInner.HalfLArGapSizeOfLayers[1]= 1.95080191;
         ChCollInner.HalfLArGapSizeOfLayers[2]= 2.30318731;
         ChCollInner.HalfLArGapSizeOfLayers[3]= 2.57486515;
         ChCollInner.HalfLArGapSizeOfLayers[4]= 2.79014569;
         ChCollInner.HalfLArGapSizeOfLayers[5]= 2.96463479;

         ChCollOuter.FoldinAngleOfLayers[0]= 129.00197;  //[degree]
         ChCollOuter.FoldinAngleOfLayers[1]= 101.408128;
         ChCollOuter.FoldinAngleOfLayers[2]=  84.7966382;
         ChCollOuter.FoldinAngleOfLayers[3]=  82.5893317;
         ChCollOuter.FoldinAngleOfLayers[4]=  80.5022136;
         ChCollOuter.FoldinAngleOfLayers[5]=  78.524804;
         ChCollOuter.FoldinAngleOfLayers[6]=  76.6479315;
         ChCollOuter.FoldinAngleOfLayers[7]=  74.8635164;
         ChCollOuter.FoldinAngleOfLayers[8]=  73.1643989;
         ChCollOuter.FoldinAngleOfLayers[9]=  64.4458572;
         ChCollOuter.FoldinAngleOfLayers[10]= 57.6301588;

         ChCollOuter.HalfLArGapSizeOfLayers[0]= 0.877808149;  //[mm]
         ChCollOuter.HalfLArGapSizeOfLayers[1]= 1.5116158;
         ChCollOuter.HalfLArGapSizeOfLayers[2]= 1.97238634;
         ChCollOuter.HalfLArGapSizeOfLayers[3]= 2.03692756;
         ChCollOuter.HalfLArGapSizeOfLayers[4]= 2.09855584;
         ChCollOuter.HalfLArGapSizeOfLayers[5]= 2.15745335;
         ChCollOuter.HalfLArGapSizeOfLayers[6]= 2.21378826;
         ChCollOuter.HalfLArGapSizeOfLayers[7]= 2.26771591;
         ChCollOuter.HalfLArGapSizeOfLayers[8]= 2.31937997;
         ChCollOuter.HalfLArGapSizeOfLayers[9]= 2.58864056;
         ChCollOuter.HalfLArGapSizeOfLayers[10]=2.80290925;
       }
       }
       else if(FieldMapVersion == "v03"){

	 ChCollInner.Fold1.FieldMapPrepared=true;
	 ChCollOuter.Fold1.FieldMapPrepared=true;

	 ChCollInner.Fold0.FieldMapPrepared=true;
	 ChCollOuter.Fold0.FieldMapPrepared=true;

	 G4int nilayer=6; //number of radial layers; should be the same for
	 G4int nolayer=16;// all fold type
      	 CreateArrays(ChCollInner,nilayer);
      	 CreateArrays(ChCollOuter,nolayer);

	 for(G4int i=0;i<nilayer;++i){
	   ChCollInner.RadiusOfLayers[i]=300.+i*100.;
	 }
         ChCollInner.FoldinAngleOfLayers[0]= 112.7420;  //[degree]
         ChCollInner.FoldinAngleOfLayers[1]=  92.238;
         ChCollInner.FoldinAngleOfLayers[2]=  78.6994;
         ChCollInner.FoldinAngleOfLayers[3]=  68.8423;
         ChCollInner.FoldinAngleOfLayers[4]=  61.2715;
         ChCollInner.FoldinAngleOfLayers[5]=  55.2462;

         ChCollInner.HalfLArGapSizeOfLayers[0]= 1.5326;  //[mm]
         ChCollInner.HalfLArGapSizeOfLayers[1]= 2.0053;
         ChCollInner.HalfLArGapSizeOfLayers[2]= 2.3577;
         ChCollInner.HalfLArGapSizeOfLayers[3]= 2.6294;
         ChCollInner.HalfLArGapSizeOfLayers[4]= 2.8446;
         ChCollInner.HalfLArGapSizeOfLayers[5]= 3.0191;

	 for(G4int i=0;i<nolayer;++i){
	   ChCollOuter.RadiusOfLayers[i]=600.+i*100.;
	 }

         ChCollOuter.FoldinAngleOfLayers[0]= 129.002;  //[degree]
         ChCollOuter.FoldinAngleOfLayers[1]= 117.7235;
         ChCollOuter.FoldinAngleOfLayers[2]= 108.8045;
         ChCollOuter.FoldinAngleOfLayers[3]= 101.4081;
         ChCollOuter.FoldinAngleOfLayers[4]=  95.1012;
         ChCollOuter.FoldinAngleOfLayers[5]=  89.6220;
         ChCollOuter.FoldinAngleOfLayers[6]=  84.7966;
         ChCollOuter.FoldinAngleOfLayers[7]=  80.5022;
         ChCollOuter.FoldinAngleOfLayers[8]=  76.6479;
         ChCollOuter.FoldinAngleOfLayers[9]=  73.1644;
         ChCollOuter.FoldinAngleOfLayers[10]= 69.9972;
         ChCollOuter.FoldinAngleOfLayers[11]= 67.1028;
         ChCollOuter.FoldinAngleOfLayers[12]= 64.4459;
         ChCollOuter.FoldinAngleOfLayers[13]= 61.9970;
         ChCollOuter.FoldinAngleOfLayers[14]= 59.7320;
         ChCollOuter.FoldinAngleOfLayers[15]= 57.6302;

         ChCollOuter.HalfLArGapSizeOfLayers[0]= 0.9367;  //[mm]
         ChCollOuter.HalfLArGapSizeOfLayers[1]= 1.1723;
         ChCollOuter.HalfLArGapSizeOfLayers[2]= 1.3824;
         ChCollOuter.HalfLArGapSizeOfLayers[3]= 1.5705;
         ChCollOuter.HalfLArGapSizeOfLayers[4]= 1.7398;
         ChCollOuter.HalfLArGapSizeOfLayers[5]= 1.8927;
         ChCollOuter.HalfLArGapSizeOfLayers[6]= 2.0313;
         ChCollOuter.HalfLArGapSizeOfLayers[7]= 2.1575;
         ChCollOuter.HalfLArGapSizeOfLayers[8]= 2.2727;
         ChCollOuter.HalfLArGapSizeOfLayers[9]= 2.3783;
         ChCollOuter.HalfLArGapSizeOfLayers[10]=2.4754;
         ChCollOuter.HalfLArGapSizeOfLayers[11]=2.5648;
         ChCollOuter.HalfLArGapSizeOfLayers[12]=2.6476;
         ChCollOuter.HalfLArGapSizeOfLayers[13]=2.7242;
         ChCollOuter.HalfLArGapSizeOfLayers[14]=2.7955;
         ChCollOuter.HalfLArGapSizeOfLayers[15]=2.8618;

       }
       else{
	 (*m_msg) << MSG::FATAL
		<< "IniGeomforFieldMaps -  FieldMapVersion = '"
		<< FieldMapVersion << "' not found!" << endreq;
       }

       ChCollInner.GridShift=0.050;
       ChCollOuter.GridShift=0.0667;
}

// ***********************************************************
void EnergyCalculator::GetFieldMaps(const G4String fname){
// ***********************************************************

  G4int i,j,npoints,wheeltype,foldtype,foldtypemx=0,nlayer;
  char fieldmapversion[10];

  (*m_msg) << MSG::INFO
	 << "GetFieldMaps from file = " << fname << endreq;

   FILE *lun = fopen(fname, "r");

   if(lun == 0){
      (*m_msg) << MSG::FATAL
	     << "GetFieldMaps - file '" << fname << "' not found!"
	     << endreq;
      throw std::runtime_error("LArEMECChargeCollection: File not found!");
   }

   if(FieldMapVersion != "v00"){
           if (fscanf(lun, "%9s", fieldmapversion) < 1) {
             (*m_msg) << MSG::ERROR
                      << "GetFieldMaps Error reading field map" << endreq;
           }
	   (*m_msg) << MSG::INFO
		  << "This file contains fieldmap of version '" << fieldmapversion
		  << "'"
                  << endreq;
   }

// Check which fold should have a map

    if     (ChCollInner.Fold1.FieldMapPrepared &&
            ChCollOuter.Fold1.FieldMapPrepared &&
            ChCollInner.Fold0.FieldMapPrepared &&
            ChCollOuter.Fold0.FieldMapPrepared)     foldtypemx=2;
    else if(ChCollInner.Fold1.FieldMapPrepared &&
            ChCollOuter.Fold1.FieldMapPrepared &&
           !ChCollInner.Fold0.FieldMapPrepared &&
           !ChCollOuter.Fold0.FieldMapPrepared)     foldtypemx=1;
    else{

      (*m_msg) << MSG::FATAL
	     <<" ***GetFieldMaps file format is incompatible"
	     <<endreq;
    }

// occupy store for the FieldMaps

    for(foldtype  = 0; foldtype  < foldtypemx; ++ foldtype){
     for(wheeltype = 0; wheeltype < 2;          ++ wheeltype){

       if(wheeltype == 0)  ChCollWheelType = &ChCollInner;
       if(wheeltype == 1)  ChCollWheelType = &ChCollOuter;
       if(foldtype  == 0 ) ChCollFoldType  = &(ChCollWheelType->Fold1);
       if(foldtype  == 1 ) ChCollFoldType  = &(ChCollWheelType->Fold0);

       nlayer = ChCollWheelType->NumberOfRadialLayer; // should be the same for Fold0 and Fold1

	 for(i=0; i<nlayer; ++i){
             if (fscanf(lun, "%lg%lg%lg%lg", &ChCollFoldType->MinZofLayer[i],
                                             &ChCollFoldType->MaxZofLayer[i],
                                             &ChCollFoldType->MinYofLayer[i],
                                             &ChCollFoldType->MaxYofLayer[i]) < 4)
             {
               (*m_msg) << MSG::ERROR
                        << "GetFieldMaps Error reading field map" << endreq;
             }
	     if (fscanf(lun, "%i%i",         &ChCollFoldType->NofColofLayer[i],
                                             &ChCollFoldType->NofRowofLayer[i]) < 2)
             {
               (*m_msg) << MSG::ERROR
                        << "GetFieldMaps Error reading field map" << endreq;
             }

	     ChCollFoldType->NofColofLayer[i] ++;
	     ChCollFoldType->NofRowofLayer[i] ++;
	     npoints = ChCollFoldType->NofColofLayer[i]*ChCollFoldType->NofRowofLayer[i];
	     if(npoints>0 && npoints < std::numeric_limits<int>::max()){
	     ChCollFoldType->NofPointsinLayer[i]=npoints;
	     if(i==0) ChCollFoldType->pLayer[i]=0;
	     else{    ChCollFoldType->pLayer[i]=ChCollFoldType->pLayer[i-1]+
			             3*ChCollFoldType->NofPointsinLayer[i-1];
	     }
	     
	     for(j=0;j<npoints;++j){
	       G4double zdummy,ydummy,wdummy;
	       if (fscanf(lun, "%lg%lg%lg", &zdummy,&ydummy,&wdummy) < 3) {
                 (*m_msg) << MSG::ERROR
                          << "GetFieldMaps Error reading field map" << endreq;
               }
	     } // end for points
	     }
//	     std::cout
	     (*m_msg) << MSG::INFO
	            <<" foldtype="<<foldtype<< " wheeltype="<<wheeltype
		    <<" ilayer = " << i << " zmin, max = "
		    << ChCollFoldType->MinZofLayer[i] << " "
		    << ChCollFoldType->MaxZofLayer[i] << " "
		    << " ymin, max = "
		    << ChCollFoldType->MinYofLayer[i] << " "
		    << ChCollFoldType->MaxYofLayer[i] << " "
		    << " ncol = " << ChCollFoldType->NofColofLayer[i]
		    << " nrow = " << ChCollFoldType->NofRowofLayer[i]
		    << " npoints = " << npoints
		    <<endreq;
//		    << std::endl;

	   } // end for layers
	 
	 G4int length=ChCollFoldType->pLayer[nlayer-1]+3*ChCollFoldType->NofPointsinLayer[nlayer-1];
	 ChCollFoldType->FieldMap = new G4double [length];

	 //	 std::cout<<" ***GetFieldMaps : length of FieldMap="<<length
	 //	  <<std::endl;

	 if(ChCollFoldType->FieldMap == 0 ) {

	   (*m_msg) << MSG::FATAL
		  <<" ***GetFieldMaps: FATAL  FieldMap array cannot be created"
		  <<endreq;
	 }

      }  // end for wheels
    }   //end for foldtypes

    rewind(lun);
    if(FieldMapVersion != "v00") {
      if (fscanf(lun, "%9s", fieldmapversion) < 1) {
        (*m_msg) << MSG::ERROR
                 << "GetFieldMaps Error reading field map" << endreq;
      }
    }

// now fill the coordinates and efield in the FieldMaps

    for(foldtype  = 0; foldtype  < foldtypemx; ++ foldtype){
     for(wheeltype = 0; wheeltype < 2;          ++ wheeltype){

       if(wheeltype == 0)  ChCollWheelType = &ChCollInner;
       if(wheeltype == 1)  ChCollWheelType = &ChCollOuter;
       if(foldtype  == 0 ) ChCollFoldType  = &(ChCollWheelType->Fold1);
       if(foldtype  == 1 ) ChCollFoldType  = &(ChCollWheelType->Fold0);

       nlayer = ChCollWheelType->NumberOfRadialLayer; // should be the same for Fold0 and Fold1

       for(i=0; i<nlayer; ++i){

	   G4double dummy1,dummy2,dummy3,dummy4;
	   G4int    dummy5,dummy6;
	   if (fscanf(lun, "%lg%lg%lg%lg", &dummy1, &dummy2, &dummy3, &dummy4) < 4) {
             (*m_msg) << MSG::ERROR
                      << "GetFieldMaps Error reading field map" << endreq;
           }
	   if (fscanf(lun, "%i%i",         &dummy5, &dummy6) < 2) {
             (*m_msg) << MSG::ERROR
                      << "GetFieldMaps Error reading field map" << endreq;
           }

           npoints = ChCollFoldType->NofPointsinLayer[i];
	   if(npoints>0){
	   for(j = 0; j < npoints; ++ j){
	     if (fscanf(lun, "%lg%lg%lg", &ChCollFoldType->FieldMap[Index(ChCollFoldType,i,0,j)],
                        &ChCollFoldType->FieldMap[Index(ChCollFoldType,i,1,j)],
                        &ChCollFoldType->FieldMap[Index(ChCollFoldType,i,2,j)]) < 3)
             {
               (*m_msg) << MSG::ERROR
                        << "GetFieldMaps Error reading field map" << endreq;
             }

/*	     if(j<10) G4cout<<" z,y,weight="
			    <<ChCollFoldType->FieldMap[Index(ChCollFoldType,i,0,j)]<<" "
			    <<ChCollFoldType->FieldMap[Index(ChCollFoldType,i,1,j)]<<" "
			    <<ChCollFoldType->FieldMap[Index(ChCollFoldType,i,2,j)]
			    <<G4endl; */

	   } // end of loop(j) on points
	   }
       }    // end of loop(i) on layers
     }	   // end of loop on wheeltype
    }    // end of loop for foldtypes
    //     (*m_msg) << endreq;
    fclose(lun);
    FieldMapsRead = true;
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
	 << endreq;

  nlayer = ChCollWheelType->NumberOfRadialLayer; // should be the same for Fold0 and Fold1
   foldtypemx=1;
   if( ChCollWheelType->Fold1.FieldMapPrepared &&
       ChCollWheelType->Fold0.FieldMapPrepared) foldtypemx=2;

   for(foldtype  = 0; foldtype  < foldtypemx; ++ foldtype){

     if(foldtype  == 0 ) ChCollFoldType  = &(ChCollWheelType->Fold1);  //normal fold
     if(foldtype  == 1 ) ChCollFoldType  = &(ChCollWheelType->Fold0); //very first fold

      for(i=0; i<nlayer; ++i){

	if(foldtype == 0) {  //normal fold
          minz=ChCollFoldType->MinZofLayer[i];
          maxz=lwc()->GetQuarterWaveLength();
          ncol=G4int( (maxz-minz)/GridSize+1.1 );
          ChCollFoldType->NofColofLayer[i]=ncol;
          ChCollFoldType->MaxZofLayer[i]=lwc()->GetQuarterWaveLength();
	}

        ipnt=-1;  // new counter for numbering the selected points
        nredef=0; //counter to know how many weights are redefined to be 1 (only for V00)
	npoints=ChCollFoldType->NofPointsinLayer[i];
	if(npoints>0){
        for(j=0;j<npoints;++j){

	  z=ChCollFoldType->FieldMap[Index(ChCollFoldType,i,0,j)];
	  y=ChCollFoldType->FieldMap[Index(ChCollFoldType,i,1,j)];
	  w=ChCollFoldType->FieldMap[Index(ChCollFoldType,i,2,j)];
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
		  if(FieldMapVersion != "v00") wx=0.; // new map
                  else{                               // old map
		    SetHalfWave(lwc()->GetStraightStartSection()+WaveLength+z);
		    PointFoldMapArea=1;
		    SetYlimitsofPhigapinFieldMap(i);
     		    if((y>=Ylimits[0] && y<=Ylimits[1]) ||
		       (y>=Ylimits[2] && y<=Ylimits[3])){  //point in LAr
	              wx=1.;     // Dice computes the Ylimits at (z+GridShift) value,
	              ++nredef;  // I don't know why...By neglecting GridShift nredef
	                         // is much less.
		    }
		  }// endif for old map
	      } //endif for  w<0
	    }// endif for z<maxz
	  }//end for normal fold

	  if(ipold != ipnt){

	    ChCollFoldType->FieldMap[Index(ChCollFoldType,i,0,ipnt)]=z;
	    ChCollFoldType->FieldMap[Index(ChCollFoldType,i,1,ipnt)]=y;
	    ChCollFoldType->FieldMap[Index(ChCollFoldType,i,2,ipnt)]=wx;

/*	    if(ipnt<10)
	      G4cout<<" ilayer="<<i<<" ipnt="<<ipnt<<" weights="
		    <<ChCollFoldType->FieldMap[Index(ChCollFoldType,i,2,ipnt)]
		    <<G4endl;*/
	  }

        }  // end j loop for points
	}
	if(ipnt+1 != ChCollFoldType->NofColofLayer[i]*ChCollFoldType->NofRowofLayer[i] ){
	     (*m_msg) << MSG::FATAL
		    <<" ERROR: ipnt+1="<<ipnt+1<<" npoints="<<
	       ChCollFoldType->NofColofLayer[i]*ChCollFoldType->NofRowofLayer[i]
		    <<endreq;
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
    for(G4int i=1;i<=NofEtaSection;++i){
      if(eta<=HV_Etalim[i]) {etasection=i-1;break;}
    }

    if(!(etasection>=0 && etasection <=NofEtaSection-1)) throw std::runtime_error("Index out of range");
    //assert(etasection>=0 && etasection <=NofEtaSection-1);
                               /*(right side of e large phi)*/ /*left side of electrode(small phi)*/
    const G4int electrodeside = (phihalfgap%2 == 0 ) ?   1   :   0  ;  

    const G4int firstelectrode=HV_Start_phi[atlasside][etasection][electrodeside];

    if(!( firstelectrode>=0 && firstelectrode<= lwc()->GetNumberOfFans()-1)){
		(*m_msg) << MSG::FATAL <<" GetCurrent:Electrode number is out of range" << endreq;
		G4Exception("EnergyCalculator", "ElectrodeOutOfRange", FatalException, "GetCurrent: Electrode number is out of range");
    }
    G4int electrodeindex=(phigap-1)-firstelectrode;
    if(electrodeindex<0) electrodeindex=electrodeindex+lwc()->GetNumberOfFans();
    const G4int nofelectrodesinphisection=lwc()->GetNumberOfFans()/NofPhiSections;//24(8) for outer(inner) wheel
    const G4int phisection=electrodeindex/nofelectrodesinphisection;

    if(!(phisection>=0 && phisection<=NofPhiSections-1)){
		(*m_msg) << MSG::FATAL <<" GetCurrent::Electrode number is out of range" << endreq;
		G4Exception("EnergyCalculator", "ElectrodeOutOfRange", FatalException,"GetCurrent: Electrode number is out of range");
    }

    G4double HV_value= HV_Values[atlasside][etasection][electrodeside][phisection];

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
  G4double phi_inb = PhiStartOfPhiDiv + phib;  //PhiStartOfPhiDiv defined in FindIdentifier!!
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
	   <<endreq;
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

  G4int i,j/*,yup*/,irlayer,igap1,igap2,ihalfgap1,ihalfgap2,nofstep,gapup,gaperr;

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
  igap2    = PhiGapNumber;
  ihalfgap2= PhiHalfGapNumber;

  SetHalfWave(v1[2]);
  GetPhiGap(v1);
  SetFoldArea(v1[2]);
  igap1    = PhiGapNumber;  // from 1 to NumberOfFans
  ihalfgap1= PhiHalfGapNumber;

  if( (igap1 != igap2) || (ihalfgap1 != ihalfgap2) )  gaperr = -1;

  chcollPhiGap=igap1-1;

// define electrode side on the field map;
// start point is used, because G4 does not garantee that the middlepoint is in the LAr gap

  TransformWheeltoFieldMap(v1,vmap);
  r=sqrt(v1[0]*v1[0]+v1[1]*v1[1]);
  r=sqrt(r*r-vmap[1]*vmap[1]);

  shift = lwc()->GetStraightStartSection();
  if(PointFoldMapArea != 0) shift += WaveLength;
  SetHalfWave(shift+vmap[2]);
  SetYlimitsofPhigapinWheel(r,r); // get the y limits of LAr gap

  Ymid=0.5*(Ylimits[1]+Ylimits[2]);
  if(vmap[1]>Ymid) gapup=+1;          //step is above electrode
  else gapup=-1;                      //step is below electrode
                                       //this should be true for the full step

  if( vmap[1] > Ylimits[3]+tol || vmap[1] < Ylimits[0]-tol ||
     (vmap[1] < Ylimits[2]-tol && vmap[1] > Ylimits[1]+tol)  ) gaperr=gaperr-10;

//loop for substeps

  steplength=sqrt((v1[0]-v2[0])*(v1[0]-v2[0])+(v1[1]-v2[1])*(v1[1]-v2[1])+
                  (v1[2]-v2[2])*(v1[2]-v2[2]));
  nofstep=int(steplength/GridSize)+1;  // step is divided to substeps
  current=0.;                          //current to be returned

  for(i=0;i<nofstep;++i){

     G4double supcorr =1.;

// compute substep point
     ds=(i+0.5)/nofstep;
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
     G4double dte = (this->*distance_to_the_nearest_electrode_type)(tmp);

     if(fabs(dte) < CHC_Esr) continue; //skip point if too close to the electrode

     G4double agap=(this->*GetGapSize_type)(tmp);    //correction to electrode suppression not to
     G4double x= agap/(agap-CHC_Esr);     // change av. signal in the gap
     if(x >=0.) supcorr=x;

     SetFoldArea(vstep[2]);                 // set fold type
     SetHalfWave(vstep[2]);                 // set halfwave parameters for substep
     TransformWheeltoFieldMap(vstep,vmap);  //get corresponding point in Map
                                            //system (it is equivalent to the
                                            //halfwave system of pos. slope)
     rvstep   =sqrt(vstep[0]*vstep[0]+vstep[1]*vstep[1]);
     rforalpha=sqrt(rvstep*rvstep-vmap[1]*vmap[1]); // radius in the electrode system
     gap=HalfLArGapSize(rforalpha,rforalpha);       // gapsize in the straight section
     yshift_on_map  = rforalpha*M_PI/lwc()->GetNumberOfFans()-(FanAbsThickness+FanEleThickness)/2.;
     yshift_on_wheel=    rvstep*M_PI/lwc()->GetNumberOfFans()-(FanAbsThickness+FanEleThickness)/2.;
     cylgapcorr=yshift_on_wheel/yshift_on_map; // scale difference between plane and cylindrical surface
     /*
     std::cout<< " GetCurrent0**Nabs="<<lwc()->GetNumberOfFans()<<" absthick="<<FanAbsThickness
	      <<" elethick="<<FanEleThickness
	      <<" cylgapcorr-1="<<cylgapcorr-1
	      <<" ZinHalfWave="<<ZinHalfWave<<" HalfWaveNumber="<<HalfWaveNumber
	      <<std::endl;
     */
//search for radial layers the substep is in between;

     irlayer=0;
     G4int numberofradiallayers=ChCollWheelType->NumberOfRadialLayer;
     for(j=0;j<numberofradiallayers;++j){
       if(rforalpha<ChCollWheelType->RadiusOfLayers[j]) {
	 irlayer=j-1;
	 break;}
     }
     assert(irlayer>=0 && irlayer<numberofradiallayers-1);

// check whether interpolation is needed

     if(!ChCollFoldType->FieldMapPrepared ||                // fieldmap is not available for this fold
	(vmap[2]<ChCollFoldType->MinZofLayer[irlayer]   &&  // or out of z range of both maps
	 vmap[2]<ChCollFoldType->MinZofLayer[irlayer+1]) ){

       cur=1.;  // interpolation not needed
     }
     else{        // interpolation needed

// compute radial interpolation parameter

       dr=(rforalpha-ChCollWheelType->RadiusOfLayers[irlayer])
       /(ChCollWheelType->RadiusOfLayers[irlayer+1]-
         ChCollWheelType->RadiusOfLayers[irlayer]);

// get  Ylimits  limiting the LAr gap along the y axis;

       shift = lwc()->GetStraightStartSection();
       if(PointFoldMapArea != 0) shift += WaveLength;
       SetHalfWave(shift+vmap[2]);
       SetYlimitsofPhigapinWheel(rforalpha,rforalpha);

//check geom. err condition if point is outside of LAr gap
       if( gaperr > -100  &&
	   ( vmap[1] > Ylimits[3]+tol || vmap[1] < Ylimits[0]-tol ||
            (vmap[1] < Ylimits[2]-tol && vmap[1] > Ylimits[1]+tol) )) gaperr=gaperr-100;

// get relative y coordinate

       if      (gapup==-1) yratio=(vmap[1]- Ylimits[0])/(Ylimits[1]- Ylimits[0]);
       else if (gapup==+1) yratio=(vmap[1]- Ylimits[2])/(Ylimits[3]- Ylimits[2]);

       yratiox=yratio;
       if(yratiox<=0.)      yratio=0.00001;  // pull the point into the gap if it wouldn't be there;
       else if(yratiox>=1.) yratio=0.99999;  // this may happen bec.G4 does not grantee that the full
                                             // step-line is within the same volume
// get corresponding y coordinates on the radial layers
// where the weight is to be taken from;

       SetYlimitsofPhigapinFieldMap(irlayer);    //on the lower layer

       if    (gapup==-1)
	 yonlowerlayer=Ylimits[0]*(1.-yratio)+Ylimits[1]*yratio;
       else if(gapup==1)
	 yonlowerlayer=Ylimits[2]*(1.-yratio)+Ylimits[3]*yratio;

       SetYlimitsofPhigapinFieldMap(irlayer+1);  //on the upper layer

       if(gapup==-1)
	 yonupperlayer=Ylimits[0]*(1.-yratio)+Ylimits[1]*yratio;
       else if(gapup==1)
	 yonupperlayer=Ylimits[2]*(1.-yratio)+Ylimits[3]*yratio;

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
     
     double dc = (edep/nofstep)/AverageCurrent*
       cur/gap/cylgapcorr*IonReco(efield)*DriftVelo(LArTemperature_av,efield)*
       supcorr;
     
     current += dc;
  } // end of loop for substeps
  
  if(current < 0.) {
    gaperr=gaperr-1000;
    current=0.;
  }
  if(gaperr != 0 ) {
    CHCEbad=CHCEbad+edep;
    if(CHCIprint < CHCMaxPrint ){
      CHCIprint=CHCIprint+1;
      (*m_msg) << MSG::WARNING
	       <<"GetCurrent has strange step,gaperr="<<gaperr<<" correction still computed"
	       <<" bad edep ratio="<<CHCEbad/CHCEtotal
	       << endreq;
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
    if(z<ChCollFoldType->MinZofLayer[i]-eps || z>ChCollFoldType->MaxZofLayer[i]+eps ||
       y<ChCollFoldType->MinYofLayer[i]-eps || y>ChCollFoldType->MaxYofLayer[i]+eps ){
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


    icol=int((z-ChCollFoldType->MinZofLayer[i])/GridSize)+1; //column number of left corners

    if(icol>ChCollFoldType->NofColofLayer[i]) icol=ChCollFoldType->NofColofLayer[i];

    irow=int((y-ChCollFoldType->MinYofLayer[i])/GridSize)+1; //row number of lower corners
    if(irow>ChCollFoldType->NofRowofLayer[i]) irow=ChCollFoldType->NofRowofLayer[i];

    assert(icol>0 && irow>0);

    ip11=(icol-1)*ChCollFoldType->NofRowofLayer[i]+irow-1; //index of points in the FieldMap
    ip21=ip11+1;                                             //   (21)----(22)
    ip12= icol   *ChCollFoldType->NofRowofLayer[i]+irow-1;  //   (11)----(12)
    ip22=ip12+1;

    G4int i2ip11=Index(ChCollFoldType,i,2,ip11);
    G4int i2ip12=Index(ChCollFoldType,i,2,ip12);
    G4int i2ip21=Index(ChCollFoldType,i,2,ip21);
    G4int i2ip22=Index(ChCollFoldType,i,2,ip22);
    G4int i0ip11=Index(ChCollFoldType,i,0,ip11);
    //G4int i0ip12=Index(ChCollFoldType,i,0,ip12);
    //G4int i0ip21=Index(ChCollFoldType,i,0,ip21);
    //G4int i0ip22=Index(ChCollFoldType,i,0,ip22);
    G4int i1ip11=Index(ChCollFoldType,i,1,ip11);
    //G4int i1ip12=Index(ChCollFoldType,i,1,ip12);
    //G4int i1ip21=Index(ChCollFoldType,i,1,ip21);
    //G4int i1ip22=Index(ChCollFoldType,i,1,ip22);


    if(icol<ChCollFoldType->NofColofLayer[i] && irow<ChCollFoldType->NofRowofLayer[i]){
      w11=ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=ChCollFoldType->FieldMap[i2ip12]; //[i][2][ip12];
      w21=ChCollFoldType->FieldMap[i2ip21]; //[i][2][ip21];
      w22=ChCollFoldType->FieldMap[i2ip22]; //[i][2][ip22];
      z11=ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=ChCollFoldType->FieldMap[i0ip12]; //[i][0][ip12];
      //z21=ChCollFoldType->FieldMap[i0ip21]; //[i][0][ip21];
      //z22=ChCollFoldType->FieldMap[i0ip22]; //[i][0][ip22];
      y11=ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=ChCollFoldType->FieldMap[i1ip12]; //[i][1][ip12];
      //y21=ChCollFoldType->FieldMap[i1ip21]; //[i][1][ip21];
      //y22=ChCollFoldType->FieldMap[i1ip22]; //[i][1][ip22];
    }
    else if(icol==ChCollFoldType->NofColofLayer[i] && irow<ChCollFoldType->NofRowofLayer[i]){
      w11=ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=0.;
      w21=ChCollFoldType->FieldMap[i2ip21]; //[i][2][ip21];
      w22=0.;
      z11=ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=z11+GridSize;
      //z21=ChCollFoldType->FieldMap[i0ip21]; //[i][0][ip21];
      //z22=z11+GridSize;
      y11=ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=y11;
      //y21=ChCollFoldType->FieldMap[i1ip21]; //[i][1][ip21];
      //y22=y11+GridSize;
    }
    else if(icol<ChCollFoldType->NofColofLayer[i] && irow==ChCollFoldType->NofRowofLayer[i]){
      w11=ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=ChCollFoldType->FieldMap[i2ip12]; //[i][2][ip12];
      w21=0.;
      w22=0.;
      z11=ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=ChCollFoldType->FieldMap[i0ip12]; //[i][0][ip12];
      //z21=z11;
      //z22=z11+GridSize;
      y11=ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=ChCollFoldType->FieldMap[i1ip12]; //[i][1][ip12];
      //y21=y11+GridSize;
      //y22=y11+GridSize;
    }
    else if(icol==ChCollFoldType->NofColofLayer[i] && irow==ChCollFoldType->NofRowofLayer[i]){
      w11=ChCollFoldType->FieldMap[i2ip11]; //[i][2][ip11];
      w12=0.;
      w21=0.;
      w22=0.;
      z11=ChCollFoldType->FieldMap[i0ip11]; //[i][0][ip11];
      //z12=z11+GridSize;
      //z21=z11;
      //z22=z11+GridSize;
      y11=ChCollFoldType->FieldMap[i1ip11]; //[i][1][ip11];
      //y12=y11;
      //y21=y11+GridSize;
      //y22=y11+GridSize;
    }

    dz=(z-z11)/GridSize;
    dy=(y-y11)/GridSize;

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
    PointFoldMapArea=1;
    ChCollFoldType=&(ChCollWheelType->Fold1);
  }
  else{
    PointFoldMapArea=0;
    ChCollFoldType=&(ChCollWheelType->Fold0);
  }
}
// ***********************************************************
void EnergyCalculator::SetHalfWave(G4double zinwheel){
// ***********************************************************

//  G4cout<<"***SetHalfWave zin="<<zinwheel<<G4endl;

  G4double z;
  z=zinwheel - lwc()->GetStraightStartSection();
  HalfWaveNumber  =int((z+lwc()->GetQuarterWaveLength())/lwc()->GetHalfWaveLength());
  ZinHalfWave=z-HalfWaveNumber*lwc()->GetHalfWaveLength();
  SignofZinHalfWave=+1;
  if(ZinHalfWave<0.) SignofZinHalfWave=-1;
  SignofSlopeofHalfWave=-1;
  if(HalfWaveNumber %2 == 0) SignofSlopeofHalfWave=+1;

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

  PointinFieldMap[0]=PointinWheel[0]*  CosPhiGap +PointinWheel[1]*SinPhiGap;
  PointinFieldMap[1]=PointinWheel[0]*(-SinPhiGap)+PointinWheel[1]*CosPhiGap;

  if(PointFoldMapArea == 0){
    if(HalfWaveNumber == 0 )             /*0th fold*/
      PointinFieldMap[2]= ZinHalfWave;
    else{                                /*last fold*/
    PointinFieldMap[1]=-PointinFieldMap[1];
    PointinFieldMap[2]= -ZinHalfWave;
    }
  }
  else{    // normal folds
    PointinFieldMap[1]=SignofZinHalfWave*SignofSlopeofHalfWave
                       *PointinFieldMap[1];
    PointinFieldMap[2]=fabs(ZinHalfWave);
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

  alpha=ChCollWheelType->FoldinAngleOfLayers[ilayer] / CLHEP::rad * CLHEP::deg;
  //s=sin(alpha/2.);
  halfgap=ChCollWheelType->RadiusOfLayers[ilayer]*CLHEP::pi/lwc()->GetNumberOfFans(); //half of y distance

  Ylimits[3]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-FanAbsThickness)+halfgap;
  Ylimits[2]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+FanEleThickness);
  Ylimits[1]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-FanEleThickness);
  Ylimits[0]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+FanAbsThickness)-halfgap;
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

  Ylimits[3]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-FanAbsThickness)+halfgap;
  Ylimits[2]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+FanEleThickness);
  Ylimits[1]=YofSurface(alpha,lwc()->GetFanFoldRadius(),-FanEleThickness);
  Ylimits[0]=YofSurface(alpha,lwc()->GetFanFoldRadius(),+FanAbsThickness)-halfgap;
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

  PhiGapNumber=phigap;
  PhiHalfGapNumber=phihalfgap;
  phi=(phigap-1)*lwc()->GetFanStepOnPhi();
  CosPhiGap=cos(phi);
  SinPhiGap=sin(phi);
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
//    ZinHalfWave :z coord of a point where YofSurf is to be computed
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
  
  if(HalfWaveNumber==0){                  //0.th 'halfwave'

    const G4double zminoffirstfold   = -rho*tb;
    const G4double zmaxoffirstfold   = -rho*tb+(rho-th)*tb*(1.+s);
    const G4double zminofsecondfold  =  lwc()->GetQuarterWaveLength()-(rho+th)*c;
    
    if(ZinHalfWave<=zminoffirstfold) return th;
    if(ZinHalfWave<=zmaxoffirstfold) return rho-sqrt( pow((rho-th),2)-pow((ZinHalfWave+rho*tb),2) );
    if(ZinHalfWave<=zminofsecondfold)return ZinHalfWave/t+th/s;
    return (lwc()->GetQuarterWaveLength()*c-rho)/s + sqrt( pow((rho+th),2)-pow((ZinHalfWave-lwc()->GetQuarterWaveLength()),2));
    
  } // end of first fold
  
  else if(HalfWaveNumber==lwc()->GetNumberOfHalfWaves()){ //last 'halfwave'
    
    const G4double zmaxoflastfold=rho*tb;
    const G4double zminoflastfold=rho*tb-(rho+th)*tb*(1.+s);
    const G4double zmaxofpreviousfold=-lwc()->GetQuarterWaveLength()+(rho-th)*c;
    
    if(ZinHalfWave>=zmaxoflastfold)     return th;
    if(ZinHalfWave>=zminoflastfold)     return -rho+sqrt( pow((rho+th),2)-pow((ZinHalfWave-rho*tb),2) );
    if(ZinHalfWave>=zmaxofpreviousfold) return ZinHalfWave/t+th/s;
    return -(lwc()->GetQuarterWaveLength()*c-rho)/s-sqrt( pow((rho-th),2)-pow((ZinHalfWave+lwc()->GetQuarterWaveLength()),2) );
    
  } // end of last fold
  
  else if(HalfWaveNumber>0 && HalfWaveNumber<lwc()->GetNumberOfHalfWaves()){
    
    const G4double sn=SignofZinHalfWave*SignofSlopeofHalfWave; 
    const G4double z2= -lwc()->GetQuarterWaveLength()+(rho-SignofSlopeofHalfWave*th)*c;
    const G4double z3=  lwc()->GetQuarterWaveLength()-(rho+SignofSlopeofHalfWave*th)*c;

    if(ZinHalfWave<=z2 || z3<=ZinHalfWave) 
      return sn*(lwc()->GetQuarterWaveLength()*c-rho)/s+
	     sn*sqrt( pow((rho+sn*th),2)-pow((ZinHalfWave-SignofZinHalfWave*lwc()->GetQuarterWaveLength()),2) );
    
    if(z2<=ZinHalfWave && ZinHalfWave<=z3) return SignofSlopeofHalfWave*ZinHalfWave/t+th/s;
     
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
  const G4double halfgap=CLHEP::pi*radius/lwc()->GetNumberOfFans()*s-(FanAbsThickness+FanEleThicknessOld)*0.5;
  return halfgap;
}
// ***************************************************************
G4double  EnergyCalculator::HalfLArGapSize(G4double rforpoint,G4double rforalpha) const {
// ***************************************************************
  const G4double alpha=FoldingAngle(rforalpha);
  const G4double s=sin(alpha/2.);
  const G4double halfgap=CLHEP::pi*rforpoint/lwc()->GetNumberOfFans()*s-(FanAbsThickness+FanEleThickness)/2.;
  return halfgap;
}








