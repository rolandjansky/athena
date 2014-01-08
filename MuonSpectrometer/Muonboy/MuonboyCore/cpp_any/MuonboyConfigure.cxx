/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyConfigure.h"

#include "MboyCoreEvt/MboyVarByName.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

//*Wrapped Fortran routine
// extern "C" void readmbcontrol_( int& Lun );
extern "C" void recoini_();


MuonboyConfigure::MuonboyConfigure(){

 p_MboyVarByName = 0 ;
 m_verbose       = 0 ;

}

MuonboyConfigure::~MuonboyConfigure(){

 delete p_MboyVarByName;
 
}

void MuonboyConfigure::SetVariablesDefault(MboyVarByName*  pMboyVarByName)
{

  Reset();
  int TheSizeVar = pMboyVarByName->Size();

   if ( m_verbose == 1 ) {
   std::cout 
             << "MuonboyConfigure::SetVariablesDefault "
             << " TheSizeVar " << TheSizeVar 
             << std::endl;
   }

  for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){    
    std::string Name2        = pMboyVarByName->GetName2(IndexVar);
    double      ValueFromExt = pMboyVarByName->GetValue(IndexVar) ;

    if ( m_verbose == 1 ) {
    std::cout 
              << "MuonboyConfigure::SetVariablesDefault "
              << "  Name2 " << std::setw(10) << Name2 
              << "  ValueFromExt " << std::setw(15)<<std::setprecision(6) << ValueFromExt 
              ;
    }

    if ( ValueFromExt < -999990. ) {

      double ValueFromMboy = 0. ;
      int Ifound = GetVarSingle ( Name2, ValueFromMboy) ;
      if (Ifound == 1) pMboyVarByName->SetValue(IndexVar,ValueFromMboy);

      if ( m_verbose == 1 ) {
      std::cout 
              << "  ->SetValue      "
              << "  Ifound "        << std::setw(3)                        << Ifound 
              << "  ValueFromMboy " << std::setw(15)<<std::setprecision(6) << ValueFromMboy 
              << std::endl;
      }

    }else{
      double ValueFromMboy = 0. ;
      int Ifound = 0 ;
      if ( m_verbose == 1 ) {
      Ifound = GetVarSingle ( Name2, ValueFromMboy) ;
      }

      /*int Istatus =*/ SetVarSingle( Name2 , ValueFromExt ) ;

      if ( m_verbose == 1 ) {
      double thediff = ValueFromMboy - ValueFromExt ;
      std::cout 
               << "  ->SetVarSingle  "
               << "  Ifound "        << std::setw(3)                        << Ifound 
               << "  ValueFromMboy " << std::setw(15)<<std::setprecision(6) << ValueFromMboy 
               << "  diff "          << std::setw(15)<<std::setprecision(6) << thediff
               << std::endl;
      }

    }
  }
  if ( p_MboyVarByName == 0 ) {
    p_MboyVarByName = new MboyVarByName();
    for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){    
      std::string Name1        = "NoName";
      std::string Name2        = pMboyVarByName->GetName2(IndexVar);
      std::string Comment      = "NoComment";
      double      ValueFromExt = pMboyVarByName->GetValue(IndexVar) ;
      p_MboyVarByName->Define(Name1,Name2,Comment) ;
      p_MboyVarByName->SetValue(IndexVar,ValueFromExt);
    }
  }

  if ( m_verbose == 1 ) {
  std::cout << "  MuonboyConfigure::SetVariablesDefault  DumpMboyVar() " << std::endl;
  DumpMboyVar() ;
  }
 
}

void MuonboyConfigure::IniReco(MboyVarByName*  pMboyVarByName){ 

  SetVariablesDefault(pMboyVarByName);
  recoini_(); 

}

void MuonboyConfigure::Reset(){

  if ( m_verbose == 1 ) {
  std::cout << "  MuonboyConfigure::Reset A DumpMboyVar() " << std::endl;
  DumpMboyVar() ;
  }
  
  if ( m_verbose == 1 ) {
  std::cout << "  MuonboyConfigure::Reset B DumpMboyVar() " << std::endl;
  DumpMboyVar() ; 
  }
  
  if ( p_MboyVarByName != 0 ) {
    int TheSizeVar = p_MboyVarByName->Size();
    for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){    
      std::string Name2 = p_MboyVarByName->GetName2(IndexVar);
      double      Value = p_MboyVarByName->GetValue(IndexVar) ;
      /*int Istatus =*/ SetVarSingle( Name2 , Value ) ;
    }
  }

  if ( m_verbose == 1 ) {
  std::cout << "  MuonboyConfigure::Reset C DumpMboyVar() " << std::endl;
  DumpMboyVar() ; 
  }

}


int MuonboyConfigure::GetMuboIntVarBazSingle  (std::string VarName, int& value){

 int I1 = -1 ;
 int I2 = -1 ;
 int I3 = -1 ;
 return GetMuboIntVarBaz(VarName, I1, I2, I3 , value);

}

int MuonboyConfigure::GetMuboRealVarBazSingle (std::string VarName, double& value){

 int I1 = -1 ;
 int I2 = -1 ;
 int I3 = -1 ;
 return GetMuboRealVarBaz(VarName, I1, I2, I3 , value);

}

int MuonboyConfigure::SetMuboRealVarBazSingle  (std::string VarName, double value){

 int I1 = -1 ;
 int I2 = -1 ;
 int I3 = -1 ;
 return SetMuboRealVarBaz(VarName, I1, I2, I3 , value);

}

int MuonboyConfigure::SetMuboIntVarBazSingle   (std::string VarName, int value){

 int I1 = -1 ;
 int I2 = -1 ;
 int I3 = -1 ;
 return SetMuboIntVarBaz(VarName, I1, I2, I3 , value);

}

int MuonboyConfigure::GetVarSingle   (std::string VarName, double& value){

 int Istatus = 0 ;
 Istatus =  GetMuboRealVarBazSingle( VarName , value ) ;
 if (Istatus == 0) {
   int Ivalue ;
   Istatus =  GetMuboIntVarBazSingle( VarName , Ivalue ) ;
   value = Ivalue ;
 }
 if (Istatus == 0) std::cout << "MuonboyConfigure::GetVarSingle " << VarName << " Not Ok " << std::endl ;
 return Istatus;

}

int MuonboyConfigure::SetVarSingle   (std::string VarName, double value){

 int Istatus = 0 ;
 Istatus =  SetMuboRealVarBazSingle( VarName , value ) ;
 if (Istatus == 0) {
   int Ivalue = static_cast<int> (value) ;
   Istatus =  SetMuboIntVarBazSingle( VarName , Ivalue ) ;
 }
 if (Istatus == 0) std::cout << "MuonboyConfigure::SetVarSingle " << VarName << " Not Ok " << std::endl ;
 return Istatus;

}

void MuonboyConfigure::DumpMboyVar(){

 int IndexVarTot =  72 ;
 for  (int IndexVar= 0 ; IndexVar <IndexVarTot ; IndexVar++){   
   std::string NameVar = "" ;
   
   if ( IndexVar ==  0 ) NameVar = "XSeve" ;
   if ( IndexVar ==  1 ) NameVar = "DeTub" ;
   if ( IndexVar ==  2 ) NameVar = "ReTub" ;
   if ( IndexVar ==  3 ) NameVar = "TubEf" ;
   if ( IndexVar ==  4 ) NameVar = "StrEf" ;
   if ( IndexVar ==  5 ) NameVar = "AerSt" ;
   if ( IndexVar ==  6 ) NameVar = "AerML" ;
   if ( IndexVar ==  7 ) NameVar = "CerSt" ;
   if ( IndexVar ==  8 ) NameVar = "CerML" ;
   if ( IndexVar ==  9 ) NameVar = "FixMo" ;
   if ( IndexVar == 10 ) NameVar = "DsFastRT" ;
   if ( IndexVar == 11 ) NameVar = "MBPri" ;
   if ( IndexVar == 12 ) NameVar = "MBDum" ;
   if ( IndexVar == 13 ) NameVar = "MBDTh" ;
   if ( IndexVar == 14 ) NameVar = "MBAcc" ;
   if ( IndexVar == 15 ) NameVar = "MBSto" ;
   if ( IndexVar == 16 ) NameVar = "UseRoA" ;
   if ( IndexVar == 17 ) NameVar = "MDTnu" ;
   if ( IndexVar == 18 ) NameVar = "CSCpa" ;
   if ( IndexVar == 19 ) NameVar = "RCHAMImi" ;
   if ( IndexVar == 20 ) NameVar = "RCHAMAma" ;
   if ( IndexVar == 21 ) NameVar = "MBChkDig" ;
   if ( IndexVar == 22 ) NameVar = "MEloss" ;
   if ( IndexVar == 23 ) NameVar = "SimBug" ;
   if ( IndexVar == 24 ) NameVar = "IsSimu" ;
   if ( IndexVar == 25 ) NameVar = "AtCosm" ;
   if ( IndexVar == 26 ) NameVar = "MomNoField";
   if ( IndexVar == 27 ) NameVar = "MdtCut"    ;
   if ( IndexVar == 28 ) NameVar = "MdtAdc"    ;
   if ( IndexVar == 29 ) NameVar = "SegAdc"    ;
   if ( IndexVar == 30 ) NameVar = "RimRTSvc"  ;
   if ( IndexVar == 31 ) NameVar = "MaxConStr" ;
   if ( IndexVar == 32 ) NameVar = "UseAllSeg" ;
   if ( IndexVar == 33 ) NameVar = "UseRoaMdt" ;
   if ( IndexVar == 34 ) NameVar = "MinuiFOut" ;
   if ( IndexVar == 35 ) NameVar = "NoCutOut"  ;
   if ( IndexVar == 36 ) NameVar = "LunGeo"    ;
   if ( IndexVar == 37 ) NameVar = "MaxNberMS" ;
   if ( IndexVar == 38 ) NameVar = "FinalCut"  ;
   if ( IndexVar == 39 ) NameVar = "DoT0Fit"   ;
   if ( IndexVar == 40 ) NameVar = "UseT0Fit"  ;
   if ( IndexVar == 41 ) NameVar = "StepT0Fit" ;
   if ( IndexVar == 42 ) NameVar = "NsteT0Fit" ;
   if ( IndexVar == 43 ) NameVar = "ReTubNoT0" ;
   if ( IndexVar == 44 ) NameVar = "MBChkCsc"  ;
   if ( IndexVar == 45 ) NameVar = "TStripMin" ;
   if ( IndexVar == 46 ) NameVar = "TStripMax" ;
   if ( IndexVar == 47 ) NameVar = "TubStr2ML" ;
   if ( IndexVar == 48 ) NameVar = "TubStr1ML" ;
   if ( IndexVar == 49 ) NameVar = "TubLoo2ML" ;
   if ( IndexVar == 50 ) NameVar = "TubLoo1ML" ;
   if ( IndexVar == 51 ) NameVar = "ResBadBIS" ;
   if ( IndexVar == 52 ) NameVar = "ResBadBEE" ;
   if ( IndexVar == 53 ) NameVar = "ResBadEIL" ;
   if ( IndexVar == 54 ) NameVar = "ResBadEEL" ;
   if ( IndexVar == 55 ) NameVar = "ResBadAli" ;
   if ( IndexVar == 56 ) NameVar = "ResBadTomo";
   if ( IndexVar == 57 ) NameVar = "CscDeadLay";
   if ( IndexVar == 58 ) NameVar = "AligRPCeta";
   if ( IndexVar == 59 ) NameVar = "AligRPCphi";
   if ( IndexVar == 60 ) NameVar = "AligTGCeta";
   if ( IndexVar == 61 ) NameVar = "AligTGCphi";
   if ( IndexVar == 62 ) NameVar = "AligStBa"  ;
   if ( IndexVar == 63 ) NameVar = "AligStEc"  ;
   if ( IndexVar == 64 ) NameVar = "AligStCs"  ;
   if ( IndexVar == 65 ) NameVar = "AligBaEc"  ;
   if ( IndexVar == 66 ) NameVar = "RecovDead" ;
   if ( IndexVar == 67 ) NameVar = "DoAligFit" ;
   if ( IndexVar == 68 ) NameVar = "ErrZMsIdBa";
   if ( IndexVar == 69 ) NameVar = "ErrZMsIdEc";
   if ( IndexVar == 70 ) NameVar = "ResAngBa"  ;
   if ( IndexVar == 71 ) NameVar = "ResAngEc"  ;
    
   double ValueReadMBcontrol = 0. ;
   int Ifound = GetVarSingle ( NameVar, ValueReadMBcontrol) ;
   std::cout 
            << "  MuonboyConfigure::DumpMboyVar " 
            << "  NameVar "            << std::setw(10) << NameVar 
            << "  Ifound "             << std::setw(3)                        << Ifound 
            << "  ValueReadMBcontrol " << std::setw(15)<<std::setprecision(6) << ValueReadMBcontrol 
            << std::endl;
 }

}

