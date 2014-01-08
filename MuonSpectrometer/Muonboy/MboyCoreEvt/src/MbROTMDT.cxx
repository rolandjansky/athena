/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbROTMDT.h"
#include <cmath>

MbROTMDT::MbROTMDT():MbROT(),MbIDTagMDT()
{ 

 Tech_MDT   () ;

 m_RZsim = 0. ;
 m_EZfit = 1. ;
 m_Rdigi = 0. ;
 m_Rtrak = 0. ;

}
MbROTMDT::~MbROTMDT(){}

// Set Digit
void MbROTMDT::SetMuonRecDigit(const MuonRecDigitMDT& MuonRecDigitObj){

m_MuonRecDigitObj = MuonRecDigitObj;
CopyIds(&MuonRecDigitObj);

}

// Get Digit
const MuonRecDigitMDT* MbROTMDT::GetMuonRecDigit() const{
 const MuonRecDigitMDT* ToBeReturned = &m_MuonRecDigitObj;
 return ToBeReturned;
}

// Set Parameters
void MbROTMDT::SetRZsim(double RZsim){m_RZsim = RZsim ;}
void MbROTMDT::SetEZfit(double EZfit){m_EZfit = EZfit ;}

// Get Parameters
double MbROTMDT::GetRZsim() const{return m_RZsim ;}
double MbROTMDT::GetEZfit() const{return m_EZfit ;}

// Set Parameters
void MbROTMDT::SetRdigi(double Rdigi){m_Rdigi = Rdigi ;}
void MbROTMDT::SetRtrak(double Rtrak){m_Rtrak = Rtrak ;}

// Get Parameters
double MbROTMDT::GetRdigi() const{return m_Rdigi ;}
double MbROTMDT::GetRtrak() const{return m_Rtrak ;}

//Print
void MbROTMDT::Print(std::ostream* out) const{

 double Rdigi = GetRdigi()/10.;
 double Rtrak = GetRtrak()/10.;
 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitMDT* pMuonRecDigitMDT = GetMuonRecDigit();
 std::string TheStationName  = pMuonRecDigitMDT->GetStationName() ;
 int         JFF             = pMuonRecDigitMDT->GetJFF()         ;
 int         JZZ             = pMuonRecDigitMDT->GetJZZ()         ;
 int         JOB             = pMuonRecDigitMDT->GetJOB()         ;
 int         JSL             = pMuonRecDigitMDT->GetJSL()         ;
 int         JTUBE           = pMuonRecDigitMDT->GetJTUBE()       ;
 int         TDCcount        = pMuonRecDigitMDT->GetTDCcount()    ;
 double      DDD             = pMuonRecDigitMDT->GetDriftTime()   ;

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
      << std::setw(5)<< JTUBE
      << "          "
      << std::setw(12)<<std::setprecision(4)<< Rdigi
      << std::setw(12)<<std::setprecision(4)<< Rtrak
      << std::setw(12)<<std::setprecision(2)<< TDCcount
      << std::setw(12)<<std::setprecision(2)<< DDD
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
void MbROTMDT::PrintCout() const{

 double Rdigi = GetRdigi()/10.;
 double Rtrak = GetRtrak()/10.;
 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitMDT* pMuonRecDigitMDT = GetMuonRecDigit();
 std::string TheStationName  = pMuonRecDigitMDT->GetStationName() ;
 int         JFF             = pMuonRecDigitMDT->GetJFF()         ;
 int         JZZ             = pMuonRecDigitMDT->GetJZZ()         ;
 int         JOB             = pMuonRecDigitMDT->GetJOB()         ;
 int         JSL             = pMuonRecDigitMDT->GetJSL()         ;
 int         JTUBE           = pMuonRecDigitMDT->GetJTUBE()       ;
 int         TDCcount        = pMuonRecDigitMDT->GetTDCcount()    ;
 double      DDD             = pMuonRecDigitMDT->GetDriftTime()   ;

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
      << std::setw(5)<< JTUBE
      << "          "
      << std::setw(12)<<std::setprecision(4)<< Rdigi
      << std::setw(12)<<std::setprecision(4)<< Rtrak
      << std::setw(12)<<std::setprecision(2)<< TDCcount
      << std::setw(12)<<std::setprecision(2)<< DDD
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
void MbROTMDT::PrintAmdIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAmdIds(out); }
void MbROTMDT::PrintAthIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAthIds(out); }
