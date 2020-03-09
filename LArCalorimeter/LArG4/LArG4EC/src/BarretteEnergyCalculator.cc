/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <cassert>
#include <string>

#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "globals.hh"

#include "LArG4Code/LArG4BirksLaw.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"

#include "EnergyCalculator.h"
#include "AthenaKernel/Units.h"
#include "HVHelper.h"

using namespace LArG4::EC;
namespace Units = Athena::Units;

const G4double EnergyCalculator::s_ColdCorrection         =  1.0036256;
const G4double EnergyCalculator::s_LongBarThickness       = 20.*Units::mm;
const G4double EnergyCalculator::s_StripWidth             =  3.*Units::mm/s_ColdCorrection;
const G4double EnergyCalculator::s_KapGap                 =  1.*Units::mm/s_ColdCorrection;
const G4double EnergyCalculator::s_EdgeWidth              =  1.*Units::mm;
const G4double EnergyCalculator::s_DistOfEndofCuFromBack  = 22.77*Units::mm/s_ColdCorrection;
const G4double EnergyCalculator::s_DistOfStartofCuFromBack= 31.*Units::mm; // frontface of the barrette
const G4double EnergyCalculator::s_ZmaxOfSignal           = s_DistOfStartofCuFromBack
                                                                        - s_DistOfEndofCuFromBack + s_EdgeWidth;
G4double EnergyCalculator::s_RefzDist           = 0.;
G4bool EnergyCalculator::s_SetConstOuterBarrett = false;
G4bool EnergyCalculator::s_SetConstInnerBarrett = false;

const G4double EnergyCalculator::s_S3_Etalim[21]={
  1.50, 1.55, 1.60, 1.65, 1.70, 1.75, 1.80, 1.85, 1.90, 1.95,
  2.00, 2.05, 2.10, 2.15, 2.20, 2.25, 2.30, 2.35, 2.40, 2.45, 2.5
};
const G4double EnergyCalculator::s_Rmeas_outer[50]={
  11.59, 25.22, 57.28, 71.30, 85.90, 98.94, 103.09, 116.68, 130.42, 146.27,
  147.19, 11.59, 15.,   56.91, 44.37, 15.13,  14.93,  45.87,  35.03,  15.40,
  14.04, 39.67, 26.83, 15.64, 14.90, 30.26,  14.70,  29.09,  43.12,  34.51,
  25.08, 11.88, 14.39, 19.54, 17.80, 12.70,  15.31,  13.96,  11.79, -99.,
  23.57, 34.64, 55.32, 65.39, 76.34, 10.83,  94.84,  98.00, -99.,   -99.
};
const G4double EnergyCalculator::s_Zmeas_outer[2]={3.81, 7.81};

G4double EnergyCalculator::s_S3_Rlim[21];
G4double EnergyCalculator::s_rlim[50];
G4double EnergyCalculator::s_zlim[4];


G4double EnergyCalculator::getPhiStartOfPhiDiv(const G4Step* step) const {
  // --- moved from FindIdentifier_Barrett --- //
  // get Module and Phidiv number (result is put into VolumeNumber and PhiDivNumber)
  G4int ModuleNumber = -999;
  G4int PhiDivNumber = -999;

  G4bool validhit = GetVolumeIndex(step, ModuleNumber, PhiDivNumber);
  if (!validhit) {
    ATH_MSG_FATAL(" ERROR ::Process_Barrett::Module, Phidiv is not found"
                  <<" ModuleNumber= "<<ModuleNumber<<" PhiDivNumber= "<<PhiDivNumber);
  }

  if(lwc()->type() ==  LArG4::OuterAbsorberWheel){ // for wheel calculation
    //PhiStartOfPhiDiv =
    return lwc()->GetFanStepOnPhi()/2. + ModuleNumber * CLHEP::twopi/8.
      + PhiDivNumber * lwc()->GetFanStepOnPhi();
    //phi_inb = PhiStartOfPhiDiv + pinLocal.phi();  //in ::BackOuterBarrettes
    //if(phi_inb < 0.)      phi_inb = phi_inb + CLHEP::twopi;
    //if(phi_inb > CLHEP::twopi) phi_inb = phi_inb - CLHEP::twopi;
    //phi_inb =  CLHEP::twopi - phi_inb;        // phi in ::EmecMother system;
  }

  if(lwc()->type() ==  LArG4::OuterAbsorberModule){ // for TB modul calculation

    // M_PI_2 - M_PI/8. is from EMECSupportConstruction
    // PhiStartOfPhiDiv =
    return M_PI_2 - M_PI/8. + lwc()->GetFanStepOnPhi()/2 + PhiDivNumber * lwc()->GetFanStepOnPhi();

    //phi_inb = PhiStartOfPhiDiv + pinLocal.phi(); //in BackOuterBarrettes;
    //phi_inb =  M_PI - phi_inb;        // phi in ::EmecMother system;
  }
  // --- moved from FindIdentifier_Barrett --- //
  ATH_MSG_ERROR(" ERROR ::Process_Barrett::getPhiStartOfPhiDiv, LArWheelType is unknown ("<<lwc()->type()<<"), PhiStartOfPhiDiv is set zero!"
                <<" ModuleNumber= "<<ModuleNumber<<" PhiDivNumber= "<<PhiDivNumber);
  return 0.0;
}

// ****************************************************************************
G4bool EnergyCalculator::Process_Barrett(const G4Step* step, std::vector<LArHitData>& hdata) const{
  // ****************************************************************************
  const G4double PhiStartOfPhiDiv = getPhiStartOfPhiDiv(step);
  G4ThreeVector startPointinLocal, endPointinLocal;
  if(!FindIdentifier_Barrett(step, PhiStartOfPhiDiv, hdata, startPointinLocal, endPointinLocal)){
    //cell id is not found
    if(!lwc()->GetisBarretteCalib()) return false; // ==> normal calculator return false
    else{ // if Calibration Calculator for Barrett:
      // compute DM identifier;
      // In calibration calculator mode this process is used as "geometry calculator".
      // We have to be prepared to return
      // either the emec cell id or , if the hit does not belong to any cell,
      // the DeadMatter (DM) id.
      // DM identifier for Barrett is defined
      // by the EmecSupportCalibrationCalculator.( In general it is
      // activated  by the atlas_calo.py)
      const G4bool status=FindDMIdentifier_Barrett(step, hdata);
      if(!status)
        ATH_MSG_WARNING(" WARNING!!EnergyCalculator::Process_Barrett:"
                        << " DM identifier not found????");
      return status;
    } // end of calibr. calculator
  }  // end if cell id not found
  // compute signal in 'normal' calculator mode, if cellid is found
  G4double E = step->GetTotalEnergyDeposit() * step->GetTrack()->GetWeight();
  if (m_birksLaw){
    const G4ThreeVector midpoint = (startPointinLocal + endPointinLocal) * 0.5;
    const G4double wholeStepLengthCm = step->GetStepLength() / CLHEP::cm;
    const G4double gap = GetGapSize_Barrett(midpoint);    // const method
    G4double phi;
    G4int compartment, eta_bin;
    if(!GetBarrettePCE(midpoint, PhiStartOfPhiDiv, phi, compartment, eta_bin)){
      hdata[0].energy = 0.;
      return true;
    }
    const G4double efield =
      0.01 * m_HVHelper->GetVoltageBarrett(phi, compartment, eta_bin) / gap; // estimate Efield[KV/cm]
    E = (*m_birksLaw)(E, wholeStepLengthCm, efield);
  }

  hdata[0].energy = (this->*m_ecorr_method)(E, startPointinLocal, endPointinLocal, PhiStartOfPhiDiv);
  return true;
}

// ****************************************************************************
void EnergyCalculator::SetConst_InnerBarrett(void){
  // ****************************************************************************
  if(s_SetConstInnerBarrett) return;
  s_SetConstInnerBarrett=true;

  std::cout <<" ===>>> ERROR!!  SetConst_InnerBarrett is called!!!" <<std::endl;
  exit(99);
}

// ****************************************************************************
void EnergyCalculator::SetConst_OuterBarrett(void){
  // ****************************************************************************

  if(s_SetConstOuterBarrett) return;
  s_SetConstOuterBarrett=true;

  for(G4int i=0;i<=20;++i){
    const G4double teta = 2.*atan( exp(-s_S3_Etalim[i]));
    s_S3_Rlim[i] = s_RefzDist*tan(teta);
  }

  const G4double inv_ColdCorrection = 1. / s_ColdCorrection;
  s_rlim[0] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[0] /*11.59 */  * inv_ColdCorrection;
  s_rlim[1] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[1] /*25.22 */  * inv_ColdCorrection;
  s_rlim[2] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[2] /*57.28 */  * inv_ColdCorrection;
  s_rlim[3] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[3] /*71.30 */  * inv_ColdCorrection;
  s_rlim[4] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[4] /*85.90 */  * inv_ColdCorrection;
  s_rlim[5] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[5] /*98.94 */  * inv_ColdCorrection;
  s_rlim[6] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[6] /*103.09 */ * inv_ColdCorrection;
  s_rlim[7] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[7] /*116.68 */ * inv_ColdCorrection;
  s_rlim[8] = s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[8] /*130.42 */ * inv_ColdCorrection;
  s_rlim[9] = s_S3_Rlim[3] + s_KapGap/2. +  s_Rmeas_outer[9] /*146.27 */ * inv_ColdCorrection + s_EdgeWidth;
  s_rlim[10]= s_rlim[8]                +  s_Rmeas_outer[10]/*147.19 */ * inv_ColdCorrection;

  s_rlim[11]= s_S3_Rlim[3] + s_KapGap    +  s_Rmeas_outer[11]/*11.59 */ * inv_ColdCorrection; //eta=1.65
  s_rlim[12]= s_S3_Rlim[3] - s_KapGap    -  s_Rmeas_outer[12]/*15.   */ * inv_ColdCorrection;

  s_rlim[13]= s_S3_Rlim[4] + s_KapGap    +  s_Rmeas_outer[13]/*56.91 */ * inv_ColdCorrection; //eta=1.7
  s_rlim[14]= s_S3_Rlim[4] - s_KapGap    -  s_Rmeas_outer[14]/*44.37 */ * inv_ColdCorrection;

  s_rlim[15]= s_S3_Rlim[5] + s_KapGap    +  s_Rmeas_outer[15]/*15.13 */ * inv_ColdCorrection; //eta=1.75
  s_rlim[16]= s_S3_Rlim[5] - s_KapGap    -  s_Rmeas_outer[16]/*14.93 */ * inv_ColdCorrection;

  s_rlim[17]= s_S3_Rlim[6] + s_KapGap    +  s_Rmeas_outer[17]/*45.87 */ * inv_ColdCorrection; //eta=1.80
  s_rlim[18]= s_S3_Rlim[6] - s_KapGap    -  s_Rmeas_outer[18]/*35.03 */ * inv_ColdCorrection;

  s_rlim[19]= s_S3_Rlim[7] + s_KapGap    +  s_Rmeas_outer[19]/*15.40 */ * inv_ColdCorrection; //eta=1.85
  s_rlim[20]= s_S3_Rlim[7] - s_KapGap    -  s_Rmeas_outer[20]/*14.04 */ * inv_ColdCorrection;

  s_rlim[21]= s_S3_Rlim[8] + s_KapGap    +  s_Rmeas_outer[21]/*39.67 */ * inv_ColdCorrection; //eta=1.90
  s_rlim[22]= s_S3_Rlim[8] - s_KapGap    -  s_Rmeas_outer[22]/*26.83 */ * inv_ColdCorrection;

  s_rlim[23]= s_S3_Rlim[9] + s_KapGap    +  s_Rmeas_outer[23]/*15.64 */ * inv_ColdCorrection; //eta=1.95
  s_rlim[24]= s_S3_Rlim[9] - s_KapGap    -  s_Rmeas_outer[24]/*14.90 */ * inv_ColdCorrection;

  s_rlim[25]= s_S3_Rlim[10] + s_KapGap   +  s_Rmeas_outer[25]/*30.26 */ * inv_ColdCorrection; //eta=2.00
  s_rlim[26]= s_S3_Rlim[10] - s_KapGap   -  s_Rmeas_outer[26]/*14.70 */ * inv_ColdCorrection;

  s_rlim[27]= s_S3_Rlim[10] - s_KapGap   -  s_Rmeas_outer[27]/*29.09 */ * inv_ColdCorrection; //eta=2.05
  s_rlim[28]= s_S3_Rlim[10] - s_KapGap   -  s_Rmeas_outer[28]/*43.12 */ * inv_ColdCorrection; //SHAPE CHANGE!!ZZ

  s_rlim[29]= s_S3_Rlim[12] + s_KapGap   +  s_Rmeas_outer[29]/*34.51 */ * inv_ColdCorrection; //eta=2.10
  s_rlim[30]= s_S3_Rlim[12] - s_KapGap   -  s_Rmeas_outer[30]/*25.08 */ * inv_ColdCorrection;

  s_rlim[31]= s_S3_Rlim[13] + s_KapGap   +  s_Rmeas_outer[31]/*11.88 */ * inv_ColdCorrection; //eta=2.15
  s_rlim[32]= s_S3_Rlim[13] - s_KapGap   -  s_Rmeas_outer[32]/*14.39 */ * inv_ColdCorrection;

  s_rlim[33]= s_S3_Rlim[14] + s_KapGap   +  s_Rmeas_outer[33]/*19.54 */ * inv_ColdCorrection; //eta=2.20
  s_rlim[34]= s_S3_Rlim[14] - s_KapGap   -  s_Rmeas_outer[34]/*17.80 */ * inv_ColdCorrection; // !!ZZ

  s_rlim[35]= s_S3_Rlim[15] + s_KapGap   +  s_Rmeas_outer[35]/*12.70 */ * inv_ColdCorrection; //eta=2.25
  s_rlim[36]= s_S3_Rlim[15] - s_KapGap   -  s_Rmeas_outer[36]/*15.31 */ * inv_ColdCorrection;

  s_rlim[37]= s_S3_Rlim[16] + s_KapGap   +  s_Rmeas_outer[37]/*13.96 */ * inv_ColdCorrection; //eta=2.30
  s_rlim[38]= s_S3_Rlim[16] - s_KapGap   -  s_Rmeas_outer[38]/*11.79 */ * inv_ColdCorrection; // !!ZZ!!

  s_rlim[40]= s_S3_Rlim[16] - s_KapGap   -  s_Rmeas_outer[40]/*23.57 */ * inv_ColdCorrection;
  s_rlim[41]= s_S3_Rlim[16] - s_KapGap   -  s_Rmeas_outer[41]/*34.64 */ * inv_ColdCorrection;
  s_rlim[42]= s_S3_Rlim[16] - s_KapGap   -  s_Rmeas_outer[42]/*55.32 */ * inv_ColdCorrection;
  s_rlim[43]= s_S3_Rlim[16] - s_KapGap   -  s_Rmeas_outer[43]/*65.39 */ * inv_ColdCorrection;
  s_rlim[44]= s_S3_Rlim[16] - s_KapGap   -  s_Rmeas_outer[44]/*76.34 */ * inv_ColdCorrection;
  s_rlim[45]= s_rlim[44]               -  s_Rmeas_outer[45]/*10.83 */ * inv_ColdCorrection;
  s_rlim[46]= s_S3_Rlim[16] - s_KapGap/2.-  s_Rmeas_outer[46]/*94.84 */ * inv_ColdCorrection - s_EdgeWidth;
  s_rlim[47]= s_S3_Rlim[16] - s_KapGap/2.-  s_Rmeas_outer[47]/*98.00 */ * inv_ColdCorrection - s_EdgeWidth;

  s_zlim[0] = - s_EdgeWidth    +  s_Zmeas_outer[0]/*3.81*/ * inv_ColdCorrection; //rel. to the end_of_HV_Cu
  s_zlim[1] = - s_KapGap/2.    +  s_Zmeas_outer[1]/*7.81*/ * inv_ColdCorrection;
  s_zlim[2] =  s_StripWidth + 1./2. * s_KapGap;
  s_zlim[3] =2*s_StripWidth + 3./2. * s_KapGap;

  for (G4int i=0; i<=3; ++i){
    s_zlim[i]= (s_ZmaxOfSignal-s_EdgeWidth) - s_zlim[i]; //rel to start of the Barrette
    if(s_zlim[i] < 0.) s_zlim[i]=0.;
  }

  return;
}
// ****************************************************************************
G4bool EnergyCalculator::GetCompartment_Barrett(
                                                G4ThreeVector pforcell, G4double r_inb, G4double z_inb, G4double eta_inb,
                                                G4int &b_compartment, G4int &etabin) const {
  // ****************************************************************************

  G4double d,d1,d2,rlim1,rlim2,rlim3,zlim1,zlim2,eta1,eta2;
  G4int i;
  G4int i0 = 3;

  G4bool validhit=true;
  b_compartment=-99;
  etabin=-99;

  if(r_inb > s_rlim[10] || r_inb < s_rlim[47] )
    {validhit=false; goto label99;}
  if(z_inb > s_ZmaxOfSignal ){validhit=false; goto label99;}

  if(r_inb > s_rlim[0]) {  // Upper corner

    if(r_inb > s_rlim[9])      {
      if(z_inb > s_zlim[0])   {validhit=false; goto label99;}
    label1:
      if(z_inb > s_zlim[1])   {
        b_compartment = 8;
        etabin        = 4;
        goto label99;
      }
    label2:
      b_compartment = 9;
      etabin        = 0;
      goto label99;
    }
    if(r_inb > s_rlim[8])  goto label1;
    if(r_inb > s_rlim[7])  goto label2;
    if(r_inb > s_rlim[6])      {
      b_compartment = 8;
      etabin        = 5;
      goto label99;
    }
    if(r_inb > s_rlim[5])      {
    label3:
      b_compartment = 8;
      etabin        = 6;
      goto label99;
    }
    if(r_inb > s_rlim[4])      {
      if(z_inb >  s_zlim[1])   goto label3;

    label4:
      b_compartment = 9;
      etabin        = 1;
      goto label99;
    }

    if(r_inb > s_rlim[3])  goto label4;

    if(r_inb > s_rlim[2])      {
      b_compartment = 8;
      etabin        = 7;
      goto label99;
    }

    if(r_inb > s_rlim[1])      {
      b_compartment = 8;
      etabin        = 8;
      goto label99;
    }
    if(r_inb > s_rlim[0])      {
      b_compartment = 9;
      etabin        = 2;
      goto label99;
    }
  }

  if(r_inb < s_rlim[38]){    // lower corner

    if( r_inb > s_rlim[40] ) {
      b_compartment = 9;
      etabin        = 16;
      goto label99;
    }

    if( r_inb > s_rlim[41] ) {
    label5:
      b_compartment = 8;
      etabin        = 37;
      goto label99;
    }

    if( r_inb > s_rlim[42] ) {

      d=DistanceToEtaLine( pforcell,2.35);
      if(fabs(d) < s_StripWidth+s_KapGap) {
        if( d < 0.) {

        label6:
          b_compartment = 8;
          etabin        = 38;
          goto label99;
        }
      label7:
        if( z_inb < s_zlim[3] ) goto label5;
        goto label6;
      }

      if( d > 0. )  goto label7;

      b_compartment = 9;
      etabin        = 17;
      goto label99;
    }

    if( r_inb > s_rlim[43] ) {
      b_compartment = 8;
      etabin        = 39;
      goto label99;
    }

    if( r_inb > s_rlim[44] ) {
    label8:
      b_compartment = 8;
      etabin        = 40;
      goto label99;
    }

    if( r_inb > s_rlim[45] ) {
      if( z_inb < s_zlim[3] ) goto label8;

    label9:
      b_compartment = 9;
      etabin        = 18;
      goto label99;
    }

    if( r_inb > s_rlim[46] ) goto label9;

    if( z_inb < s_ZmaxOfSignal/(s_rlim[46]-s_rlim[47])*(r_inb-s_rlim[47])) goto label9;

    validhit=false;
    goto label99;
  }

  // medium r region:   s_rlim[0] > r > s_rlim[38];
  //   from middle of cellno 2 to middle of cellno. 16
  //

  for( i=3; i<=17; ++i){         // eta= 1.65 - 2.35
    if( eta_inb < s_S3_Etalim[i] ) {
      i0=i;
      break;
    }
  }

  i=i0;

  eta1    = s_S3_Etalim[i-1];
  eta2    = s_S3_Etalim[i];
  rlim1   = s_rlim[2*i+5 - 1];
  rlim2   = s_rlim[2*i+5];
  zlim1   = s_zlim[2];
  zlim2   = s_zlim[3];

  if( i == 15 || i == 17) {
    zlim1   = s_zlim[3];
    zlim2   = s_zlim[2];
  }

  switch(i){

  case 3:

    if( fabs( DistanceToEtaLine(pforcell, eta2) ) <  s_StripWidth+s_KapGap
        || z_inb > zlim1 ){

      b_compartment = 8;
      etabin        = 2*i+3;;
      break;
    }

    b_compartment = 9;
    etabin        = i-1;
    break;

  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 13:
  case 16:

    d1=fabs( DistanceToEtaLine( pforcell, eta1) );
    d2=fabs( DistanceToEtaLine( pforcell, eta2) );

    if( d1 < s_StripWidth+s_KapGap ){
    label11:
      b_compartment = 8;
      etabin        = 2*i+2;
      break;
    }
    if( d2 < s_StripWidth+s_KapGap ){
    label12:
      b_compartment = 8;
      etabin        = 2*i+3;
      break;
    }

    if( z_inb < zlim1 || (r_inb > rlim2 && r_inb < rlim1) ) {

      b_compartment = 9;
      etabin        = i-1;
      break;
    }
    if( r_inb > rlim1 ) goto label11;
    if( r_inb < rlim2 ) goto label12;
    validhit=false;
    break;
    //========================================================
  case 11:

    rlim3 = s_rlim[28];

    d1=fabs( DistanceToEtaLine( pforcell, eta1) );
    d2=fabs( DistanceToEtaLine( pforcell, eta2) );

    if( d1 < s_StripWidth+s_KapGap ){
    label13:
      b_compartment = 8;
      etabin        = 2*i+2;
      break;
    }

    if( r_inb > rlim1 && z_inb > zlim1 ) goto label13;
    if( r_inb > rlim2 ){
    label14:
      b_compartment = 9;
      etabin        = i-1;
      break;
    }

    if( d2 < s_StripWidth+s_KapGap ){
      if( z_inb > zlim1 ) {
      label15:
        b_compartment = 8;
        etabin        = 2*i+4;
        break;
      }

    label16:
      b_compartment = 8;
      etabin        = 2*i+3;
      break;
    }

    if( z_inb < zlim2) goto label14;
    if( r_inb > rlim3) goto label16;
    if( z_inb > zlim1) goto label15;
    goto label16;
    //======================================================
  case 12:

    d1=fabs( DistanceToEtaLine( pforcell, eta1) );
    d2=fabs( DistanceToEtaLine( pforcell, eta2) );

    if( d1 < s_StripWidth+s_KapGap ){
      b_compartment = 8;
      etabin        = 2*i+2;
      break;
    }
    if( d2 < s_StripWidth+s_KapGap ){
    label17:
      b_compartment = 8;
      etabin        = 2*i+3;
      break;
    }

    if( r_inb > rlim2 || z_inb < zlim1 ){
      b_compartment = 9;
      etabin        = i-1;
      break;
    }
    goto label17;

    //========================================================

  case 14:
  case 15:

    d1=fabs( DistanceToEtaLine( pforcell, eta1) );
    d2=fabs( DistanceToEtaLine( pforcell, eta2) );

    if( d1 < s_StripWidth+s_KapGap ){
    label18:
      b_compartment = 8;
      etabin        = 2*i+2;
      break;
    }
    if( d2 < s_StripWidth+s_KapGap ){
    label19:
      b_compartment = 8;
      etabin        = 2*i+3;
      break;
    }
    if( r_inb > rlim1 && z_inb > zlim1 ) goto label18;
    if( r_inb < rlim2 && z_inb > zlim2 ) goto label19;

    b_compartment = 9;
    etabin        = i-1;
    break;
    //======================================================

  case 17:

    d1=fabs( DistanceToEtaLine( pforcell, eta1) );

    if( d1 < s_StripWidth+s_KapGap || z_inb > zlim1  ){
      b_compartment = 8;
      etabin        = 2*i+2;
      break;
    }
    b_compartment = 9;
    etabin        = i-1;
    break;
  }

  //======================================================


  // end of search for compartment and etabin

 label99:
  return validhit;
}
// ****************************************************************************
G4bool EnergyCalculator::GetVolumeIndex(const G4Step *step, G4int & ModuleNumber, G4int & PhiDivNumber) const{
  // ****************************************************************************

  const G4StepPoint*  pre_step_point = step->GetPreStepPoint();
  const G4int         ndepth = pre_step_point->GetTouchable()->GetHistoryDepth();

  for(G4int i=0;i<=ndepth;++i){
    G4String ivolname=pre_step_point->GetTouchable()->GetVolume(i)->GetName();
    if( ivolname.find("BackOuterBarrette::Module") != std::string::npos ){
      ModuleNumber = pre_step_point->GetTouchable()->GetVolume(i)->GetCopyNo();
    }
    if( ivolname.find("BackOuterBarrette::Module::Phidiv") != std::string::npos ){
      PhiDivNumber = pre_step_point->GetTouchable()->GetVolume(i)->GetCopyNo();
    }
  }

  if(!lwc()->GetisModule()){
    if(ModuleNumber < 0 || PhiDivNumber < 0) {return false;}
  }
  else if(PhiDivNumber < 0 ) {return false;}

  return true;
}
// ****************************************************************************
G4bool EnergyCalculator::FindIdentifier_Barrett(
                                                const G4Step* step,
                                                G4double PhiStartOfPhiDiv,
                                                std::vector<LArHitData>& hdata,
                                                G4ThreeVector &startPointLocal,
                                                G4ThreeVector &endPointLocal
                                                ) const {
  // ****************************************************************************
  // works only for outer part of the full wheel or of the module in the Barrett
  //     at the back side of EMEC

  // check whether we are in the outer wheel
  if(lwc()->type() != LArG4::OuterAbsorberWheel && lwc()->type() != LArG4::OuterAbsorberModule) {
    ATH_MSG_FATAL(" ERROR ::FindIdentifier_Barrett, not yet prepared for solidtype="
                  << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type()));
  }

  G4bool validhit=true;

  // Get point coordinates in the Atlas coord. system

  const G4StepPoint*  pre_step_point = step->GetPreStepPoint();
  const G4StepPoint* post_step_point = step->GetPostStepPoint();

  const G4ThreeVector startPoint =  pre_step_point->GetPosition();
  const G4ThreeVector   endPoint = post_step_point->GetPosition();
  //	G4ThreeVector          p = 0.5 *(startPoint+endPoint);
  const G4ThreeVector p = startPoint; // bec. middle point maybe out of volume

  // transform point to the coord system of Barrett::Module::Phidiv (alias local)

  const G4AffineTransform transformation =
    pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();
  startPointLocal = transformation.TransformPoint(startPoint);
  endPointLocal = transformation.TransformPoint(endPoint);
  // G4ThreeVector  pinLocal = 0.5 * (startPointLocal + endPointLocal);
  const G4ThreeVector  pinLocal = startPointLocal;

  //------ code transfered to getPhiStartOfPhiDiv ------//

  const G4double z_inb= lwc()->GetdWRPtoFrontFace()/2.-pinLocal.z(); //dist. from front end of the Back Barrettes
  const G4double r_inb= pinLocal.perp();                 //dist from the z axis
  const G4ThreeVector pforcell=G4ThreeVector( pinLocal.x(), pinLocal.y(),
                                              lwc()->GetElecFocaltoWRP()+lwc()->GetdWRPtoFrontFace()+lwc()->GetWheelThickness()+z_inb );
  const G4double eta_inb=pforcell.pseudoRapidity();    //eta in the system where electrodes were designed

  G4int compartment,etabin;

  // get compartment and etaBin

  validhit=GetCompartment_Barrett(pforcell, r_inb, z_inb, eta_inb, compartment, etabin);

  G4int etaBin        = etabin;
  if (!validhit) {
    compartment = 9;  // to have some 'reasonable' number
    etaBin        = 0;
  }

  const G4int c = compartment-1;

  G4int sampling = s_geometry[c].sampling;
  G4int region   = s_geometry[c].region;
  const G4int atlasside = lwc()->GetAtlasZside() * s_geometry[c].zSide;

  if(lwc()->GetisModule() && atlasside < 0 ) {
    ATH_MSG_FATAL("EnergyCalculator: TB modul should be at pos z");
  }

  // get phiBin
  G4int phigap = GetPhiGap_Barrett(pinLocal, PhiStartOfPhiDiv); // in wheel numbering scheme
  //int phigapwheel=phigap; //for check

  if(lwc()->GetisModule()) {
    phigap = phigap - lwc()->GetStartGapNumber() + lwc()->GetLastFan()/2;  // in module numbering scheme

    if(phigap < lwc()->GetFirstFan() || phigap >= lwc()->GetLastFan()){
      if (phigap<lwc()->GetFirstFan()) phigap=lwc()->GetFirstFan();
      if (phigap>=lwc()->GetLastFan()) phigap=lwc()->GetLastFan()-1;
      validhit=false;
    }
  }

  G4int phiBin = phigap / s_geometry[c].gapsPerBin;

  if(atlasside < 0){

    // The following formula assumes that the z<0 endcap was derived
    // from the positive endcap by rotateY(180.*deg)
    // 29-March-2004 ML

    phiBin = (s_geometry[c].maxPhi - 1)/2 - phiBin;
    if(phiBin < 0) phiBin += s_geometry[c].maxPhi + 1;
  }

  // checks for phiBin and etaBin

  if(phiBin<0) {
    ATH_MSG_WARNING("FindIdentifier_Barrett: invalid hit, phiBin < 0");
    phiBin=0;
    validhit=false;
  }
  if(phiBin>s_geometry[c].maxPhi) {
    ATH_MSG_WARNING("FindIdentifier_Barrett: invalid hit, phiBin = " << phiBin
                    << " > geometry[" << c << "].maxPhi="<< s_geometry[c].maxPhi);
    phiBin=s_geometry[c].maxPhi;
    validhit=false;
  }
  if(etaBin < 0){
    ATH_MSG_WARNING("FindIdentifier_Barrett: invalid hit, etaBin < 0");
    etaBin=0;
    validhit=false;
  }
  if(etaBin > s_geometry[c].maxEta){
    ATH_MSG_WARNING("FindIdentifier_Barrett: invalid hit, etaBin = "
                    << etaBin << " > geometry[" << c << "].maxEta="
                    << s_geometry[c].maxEta);
    etaBin=s_geometry[c].maxEta;
    validhit=false;
  }

  hdata[0].id.clear();
  hdata[0].id  << 4 // LArCalorimeter
               << 1 // LArEM
               << atlasside
               << sampling
               << region
               << etaBin
               << phiBin;

  G4double timeOfFlight = 0.5 *
    ( pre_step_point->GetGlobalTime() +
      post_step_point->GetGlobalTime()    );
  hdata[0].time = timeOfFlight/Units::ns - p.mag()/CLHEP::c_light/Units::ns;

  return validhit;
}
// ****************************************************************************
G4bool EnergyCalculator::FindDMIdentifier_Barrett(const G4Step* step, std::vector<LArHitData>& hdata) const {
  // ****************************************************************************

  //  G4bool validid = false;
  //  hdata[0].id = LArG4Identifier();
  //  G4bool validid = m_supportCalculator->Process(step, LArG4::VCalibrationCalculator::kOnlyID);
  //  hdata[0].id = m_supportCalculator->identifier();
  //  return validid;
  hdata[0].id = LArG4Identifier();
  std::vector<G4double> tmpv;
  return m_supportCalculator->Process(step, hdata[0].id, tmpv, LArG4::kOnlyID );
}

G4double EnergyCalculator::_AdjustedPhiOfPoint_Barrett(const G4ThreeVector& p, G4double PhiStartOfPhiDiv) const {
  //  G4double phi=p.phi();           // in Module::Phidiv
  //  phi = PhiStartOfPhiDiv + phi;   // in Barrette
  //  if(phi < 0. )       phi=phi+CLHEP::twopi;
  //  if(phi >= CLHEP::twopi ) phi=phi-CLHEP::twopi;

  //  if(lwc()->GetisModule()) phi =    M_PI - phi;     // in EMECMother; TB modul
  //  else          phi = CLHEP::twopi - phi;     // in EMECMother; full wheel

  return (lwc()->GetisModule()) ?
    CLHEP::pi - _normalizeAngle2Pi(PhiStartOfPhiDiv + p.phi())    // in EMECMother; TB modul
    :
    CLHEP::twopi - _normalizeAngle2Pi(PhiStartOfPhiDiv + p.phi()) // in EMECMother; full wheel
    ;
}

//****************************************************************************
G4int EnergyCalculator::GetPhiGap_Barrett(const G4ThreeVector& p, G4double PhiStartOfPhiDiv) const {
  // ****************************************************************************
  const G4double phi = _AdjustedPhiOfPoint_Barrett(p, PhiStartOfPhiDiv);
  if(phi > CLHEP::twopi-lwc()->GetFanStepOnPhi()*0.5) {return 0;}
  return (G4int( (phi+lwc()->GetFanStepOnPhi()*0.5)/lwc()->GetFanStepOnPhi()) );
}
// ****************************************************************************
G4double EnergyCalculator::GetGapSize_Barrett(const G4ThreeVector& p) const {
  // ****************************************************************************
  const G4double r=p.perp();
  const G4double ta1=1./ sqrt(4.*r/FanAbsThickness()*r/FanAbsThickness() - 1.);
  const G4double ta2=1./ sqrt(4.*r/FanEleThickness()*r/FanEleThickness() - 1.);
  const G4double phieff= lwc()->GetFanStepOnPhi()*0.5-atan(ta1)-atan(ta2);
  return (r*phieff);
}
// ****************************************************************************
G4double EnergyCalculator::distance_to_the_nearest_electrode_Barrett(
                                                                     const G4ThreeVector &p,
                                                                     G4double PhiStartOfPhiDiv
                                                                     ) const {
  // ****************************************************************************
  const G4double phi = _AdjustedPhiOfPoint_Barrett(p, PhiStartOfPhiDiv);

  G4double r=p.perp();
  G4int igap;
  G4double elephi,dphi;
  if (phi > CLHEP::twopi-lwc()->GetFanStepOnPhi()*0.5) {
    dphi=phi-CLHEP::twopi;
  } else {
    igap=G4int( (phi+lwc()->GetFanStepOnPhi()*0.5)/lwc()->GetFanStepOnPhi() );
    elephi=igap*lwc()->GetFanStepOnPhi();
    dphi=phi-elephi;
  }
  G4double dist=r*sin(fabs(dphi))-FanEleThickness()*0.5;

  return fabs(dist);
}

G4bool EnergyCalculator::GetBarrettePCE(
    const G4ThreeVector& p, G4double PhiStartOfPhiDiv,
    G4double &phi, G4int &compartment, G4int &eta_bin
) const
{
 // get etasection
    const G4double z_inb = lwc()->GetdWRPtoFrontFace() * 0.5 - p.z(); //dist. from front end of the Back Barrettes
    const G4double r_inb = p.perp();                 //dist from the z axis
    const G4ThreeVector pforcell {
        p.x(), p.y(),
        lwc()->GetElecFocaltoWRP() + lwc()->GetdWRPtoFrontFace()
        + lwc()->GetWheelThickness() + z_inb
    };
    const G4double eta_inb = pforcell.pseudoRapidity();    //eta in the system where electrodes were designed
    G4bool validhit = GetCompartment_Barrett(
        pforcell, r_inb, z_inb, eta_inb, compartment, eta_bin
    );

    if(!validhit) return false;  // p is not in the sens. region

 // get electrode number and side
    phi = _AdjustedPhiOfPoint_Barrett(p, PhiStartOfPhiDiv);
    return true;
}
