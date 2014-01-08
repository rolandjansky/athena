/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbROTCSCclu.h"
#include <cmath>
MbROTCSCclu::MbROTCSCclu():MbROT(),MbIDTagCSCclu()
{

 Tech_CSCclu() ;

 m_Rsim  = 0. ; 
 m_Efit  = 1. ;
 m_Rdigi = 0. ;
 m_Rtrak = 0. ;

}
MbROTCSCclu::~MbROTCSCclu(){}

// Set Digit
void MbROTCSCclu::SetMuonRecDigit(const MuonRecDigitCSCClu& MuonRecDigitObj){

m_MuonRecDigitObj = MuonRecDigitObj;
CopyIds(&MuonRecDigitObj);

}

// Get Digit
const MuonRecDigitCSCClu* MbROTCSCclu::GetMuonRecDigit() const{
 const MuonRecDigitCSCClu* ToBeReturned = &m_MuonRecDigitObj;
 return ToBeReturned;
}

// Set Parameters
void MbROTCSCclu::SetRsim(double Rsim){m_Rsim = Rsim ;}
void MbROTCSCclu::SetEfit(double Efit){m_Efit = Efit ;}

// Get Parameters
double MbROTCSCclu::GetRsim() const{return m_Rsim ;}
double MbROTCSCclu::GetEfit() const{return m_Efit ;}

// Set Parameters
void MbROTCSCclu::SetRdigi(double Rdigi){m_Rdigi = Rdigi ;}
void MbROTCSCclu::SetRtrak(double Rtrak){m_Rtrak = Rtrak ;}

// Get Parameters
double MbROTCSCclu::GetRdigi() const{return m_Rdigi ;}
double MbROTCSCclu::GetRtrak() const{return m_Rtrak ;}

//Print
void MbROTCSCclu::Print(std::ostream* out) const{

//double Rdigi = GetRdigi()/10.;
//double Rtrak = GetRtrak()/10.;
 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitCSCClu* pMuonRecDigitCSCClu = GetMuonRecDigit();
 std::string TheStationName = pMuonRecDigitCSCClu->GetStationName() ;
 int         JFF            = pMuonRecDigitCSCClu->GetJFF()         ;
 int         JZZ            = pMuonRecDigitCSCClu->GetJZZ()         ;
 int         JOB            = pMuonRecDigitCSCClu->GetJOB()         ;
 int         JSL            = pMuonRecDigitCSCClu->GetJSL()         ;
 int         JSTRI          = pMuonRecDigitCSCClu->GetJSTRI()       ;
 double      SZFLAG         = pMuonRecDigitCSCClu->GetSZFLAG()      ;

 if (IsOutlier()){
   *out << " o"
        << std::setw(3)<< GetType()  
        << std::setw(5)<< GetTech() ;
 }else{
   *out << std::setw(5)<< GetType()
        << std::setw(5)<< GetTech() ;
 }
 *out << "  " << std::setw(3)<< TheStationName
      << std::setw(5)<< JFF  
      << std::setw(5)<< JZZ  
      << std::setw(5)<< JOB  
      << std::setw(5)<< JSL  
      << std::setw(5)<< JSTRI
      << std::setw(5)<<std::setprecision(1)<< SZFLAG  
      << "     "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcaL
      << std::setw(12)<<std::setprecision(4)<< YdcaL
      << std::setw(12)<<std::setprecision(4)<< ZdcaL
      << std::setw(12)<<std::setprecision(4)<< ResoL
      << std::endl;
 double StepSize = sqrt (  std::pow( (XdcaL - XdcbL), 2 )
                         + std::pow( (YdcaL - YdcbL), 2 )
                         + std::pow( (ZdcaL - ZdcbL), 2 )
                        );
 *out << "     " ;
 *out << "     "
      << "     " ;
 *out << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcbL
      << std::setw(12)<<std::setprecision(4)<< YdcbL
      << std::setw(12)<<std::setprecision(4)<< ZdcbL
      << std::setw(12)<<std::setprecision(4)<< StepSize
      << std::endl;
 *out << "     "
      << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< GetEner() /1000.
      << std::endl;

}
void MbROTCSCclu::PrintCout() const{

//double Rdigi = GetRdigi()/10.;
//double Rtrak = GetRtrak()/10.;
 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitCSCClu* pMuonRecDigitCSCClu = GetMuonRecDigit();
 std::string TheStationName = pMuonRecDigitCSCClu->GetStationName() ;
 int         JFF            = pMuonRecDigitCSCClu->GetJFF()         ;
 int         JZZ            = pMuonRecDigitCSCClu->GetJZZ()         ;
 int         JOB            = pMuonRecDigitCSCClu->GetJOB()         ;
 int         JSL            = pMuonRecDigitCSCClu->GetJSL()         ;
 int         JSTRI          = pMuonRecDigitCSCClu->GetJSTRI()       ;
 double      SZFLAG         = pMuonRecDigitCSCClu->GetSZFLAG()      ;

 if (IsOutlier()){
   std::cout << " o"
        << std::setw(3)<< GetType()  
        << std::setw(5)<< GetTech() ;
 }else{
   std::cout << std::setw(5)<< GetType()
        << std::setw(5)<< GetTech() ;
 }
 std::cout << "  " << std::setw(3)<< TheStationName
      << std::setw(5)<< JFF  
      << std::setw(5)<< JZZ  
      << std::setw(5)<< JOB  
      << std::setw(5)<< JSL  
      << std::setw(5)<< JSTRI
      << std::setw(5)<<std::setprecision(1)<< SZFLAG  
      << "     "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcaL
      << std::setw(12)<<std::setprecision(4)<< YdcaL
      << std::setw(12)<<std::setprecision(4)<< ZdcaL
      << std::setw(12)<<std::setprecision(4)<< ResoL
      << std::endl;
 double StepSize = sqrt (  std::pow( (XdcaL - XdcbL), 2 )
                         + std::pow( (YdcaL - YdcbL), 2 )
                         + std::pow( (ZdcaL - ZdcbL), 2 )
                        );
 std::cout << "     " ;
 std::cout << "     "
      << "     " ;
 std::cout << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcbL
      << std::setw(12)<<std::setprecision(4)<< YdcbL
      << std::setw(12)<<std::setprecision(4)<< ZdcbL
      << std::setw(12)<<std::setprecision(4)<< StepSize
      << std::endl;
 std::cout << "     "
      << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< GetEner() /1000.
      << std::endl;

}
void MbROTCSCclu::PrintAmdIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAmdIds(out); }
void MbROTCSCclu::PrintAthIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAthIds(out); }
