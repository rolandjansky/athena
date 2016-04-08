/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// #include "MuonTrkPhysMonitoring/MuonTrkPhysMonUtils.h"
// #include <iostream>
// #include <cmath>


// //---------------------------------------------------------------------------------------
// float MuonTrkPhysMonUtils::EtaRegionFine(double eta)
// {
//   if (eta < -2.50) {return -1.;} // underflow
//   if (eta < -2.00) {return 0.5;} // CSC C
//   if (eta < -1.70) {return 1.5;} // ENDCAP C
//   if (eta < -1.05) {return 2.5;} // Trans C
//   if (eta <  0.00) {return 3.5;} // Barrel C
//   if (eta >  2.50) {return 8.5;} // OverFlow
//   if (eta >  2.00) {return 7.5;} // CSC A
//   if (eta >  1.70) {return 6.5;} // ENDCAP A
//   if (eta >  1.05) {return 5.5;} // Trans A
//   if (eta >  0.00) {return 4.5;} // Barrel A
//   return -1.;
// }
// //---------------------------------------------------------------------------------------
// float MuonTrkPhysMonUtils::EtaRegionCoarse(double eta)
// {
    
//   if(eta < -2.50) {return -1.;} // underflow
//   if(eta < -1.05) {return 0.5;} // EC
//   if(eta <  0.00) {return 1.5;} // BC
//   if(eta <  1.05) {return 2.5;} // BA
//   if(eta <  2.50) {return 3.5;} // BC
//   return 4.5; // Overflow
    
// }
// //---------------------------------------------------------------------------------------
// bool MuonTrkPhysMonUtils::LabelLargeEtaRegions(TAxis * axis)
// {
//   axis->SetBinLabel(1, "CSC C");
//   axis->SetBinLabel(2, "EC C");
//   axis->SetBinLabel(3, "TRANS C");
//   axis->SetBinLabel(4, "BARR C");
//   axis->SetBinLabel(5, "BARR A");
//   axis->SetBinLabel(6, "TRANS A");
//   axis->SetBinLabel(7, "EC A");
//   axis->SetBinLabel(8," CSC A");
//   axis->SetTitle("Detector #eta Regions");
    
//   return true;
// }
// //---------------------------------------------------------------------------------------
// bool MuonTrkPhysMonUtils::LabelCombinedEtaRegions(TAxis * axis)
// {
//   axis->SetBinLabel( 1, "EC-EC");
//   axis->SetBinLabel( 2, "EC-BC");
//   axis->SetBinLabel( 3, "EC-BA");
//   axis->SetBinLabel( 4, "EC-EA");
//   axis->SetBinLabel( 5, "BC-EC");
//   axis->SetBinLabel( 6, "BC-BC");
//   axis->SetBinLabel( 7, "BC-BA");
//   axis->SetBinLabel( 8, "BC-EA");
//   axis->SetBinLabel( 9, "BA-EC");
//   axis->SetBinLabel(10, "BA-BC");
//   axis->SetBinLabel(11, "BA-BA");
//   axis->SetBinLabel(12, "BA-EA");
//   axis->SetBinLabel(13, "EA-EC");
//   axis->SetBinLabel(14, "EA-BC");
//   axis->SetBinLabel(15, "EA-BA");
//   axis->SetBinLabel(16, "EA-EA");
    
//   axis->SetTitle("#eta Region Permutations [+#mu, -#mu]");
    
//   return true;
// }
// //---------------------------------------------------------------------------------------
// bool MuonTrkPhysMonUtils::LabelSectorAxis(TAxis * axis){
    
//   axis->SetTitle("#phi Sectors 1-16");
//   if(axis->GetNbins() == 16) axis->CenterLabels(kTRUE);
    
//   if(axis->GetNbins()%16){
//     std::cout << "Sector Axis Label Error, not a multiple of 16 bins!\n";
//     return false;
//   }
    
//   int nSectorDevisions = axis->GetNbins()/16;
//   int skip_counter     = 1;
    
//   axis->SetNdivisions(16,nSectorDevisions,0,kFALSE);
    
    
//   for (int i = 1; i <= axis->GetNbins(); i++) {
        
//     if (skip_counter == nSectorDevisions) {
//       axis->SetBinLabel(i,Form("%d",i/nSectorDevisions));
//       skip_counter = 1;
//     }else {
//       axis->SetBinLabel(i," ");
//       skip_counter++;
//     }
        
//   }
    
//   return true;
// }
// //---------------------------------------------------------------------------------------
// bool MuonTrkPhysMonUtils::LabelQuality(TAxis * axis)
// {
//   axis->SetBinLabel(1, "Tight");
//   axis->SetBinLabel(2, "Medium");
//   axis->SetBinLabel(3, "Loose");
//   axis->SetBinLabel(4, "Very Loose");
//   axis->SetTitle("Muon Quality");

//   return true;
// }
// //---------------------------------------------------------------------------------------
// bool MuonTrkPhysMonUtils::LabelAuthor(TAxis * axis)
// {

//   axis->SetBinLabel(1, "MuidCo");
//   axis->SetBinLabel(2, "STACO");
//   axis->SetBinLabel(3, "MuTag");
//   axis->SetBinLabel(4, "MuTagIMO");
//   axis->SetBinLabel(5, "MuidSA");
//   axis->SetBinLabel(6, "MuGirl");
//   axis->SetBinLabel(7, "MuGirlLowBeta");
//   axis->SetBinLabel(8, "CaloTag");
//   axis->SetBinLabel(9, "CaloLikelihood");
//   axis->SetBinLabel(10, "ExtrapolateMuonToIP");
//   axis->SetTitle("Muon Author");
//   axis->SetTitleOffset(1.4);

//   return true;
// }
// //---------------------------------------------------------------------------------------
// float MuonTrkPhysMonUtils::PhiSector(float ATLAS_Phi_Coordinate)
// {
    
//   if (ATLAS_Phi_Coordinate < 0) { ATLAS_Phi_Coordinate += 2*M_PI;} //convert to [0-2Pi) range from [-pi to pi)
    
//   float Shifted_Phi_Coordinate = ATLAS_Phi_Coordinate + M_PI/16.0 /* 1/2 of 1/16th shift (phi = 0 => "0.5 stations")*/;
    
//   float Station_Location = (16.0 /*stations*/ / (2*M_PI) /*Rads*/)*(Shifted_Phi_Coordinate /*Rads*/);
    
//   if (Station_Location > 16) {Station_Location -= 16;} // wrap 16+ to 0+
    
//   return Station_Location + 1.0; // shift from 1-16+ to 0-16+ 
    
// }

// //---------------------------------------------------------------------------------------
// float MuonTrkPhysMonUtils::RadsToDegrees(float Radians)
// {
//   float Degrees = Radians * (180.) / M_PI;
//   return Degrees;
// }

// //---------------------------------------------------------------------------------------
// float MuonTrkPhysMonUtils::DeltaR(float eta1, float eta2, float phi1, float phi2)
// {
    
//   float deta = eta1 - eta2;
//   float dphi = phi1 - phi2;
//   if(fabs(dphi) > 3.1416) dphi = 6.2832 - fabs(dphi);
//   float dr = sqrt(pow(deta,2) + pow(dphi,2));
//   return dr;
    
// }

// int MuonTrkPhysMonUtils::getDetectorRegion( double eta, double /*phi*/ ) {
//   if( eta < -2.5 ) return -1;
//   if( eta < -1.5 ) return 0;
//   if( eta < -1. )  return 1;
//   if( eta <  0. )  return 2;
//   if( eta <  1. )  return 3;
//   if( eta <  1.5 ) return 4;
//   if( eta <  2.5 ) return 5;
//   return -1;
// }
