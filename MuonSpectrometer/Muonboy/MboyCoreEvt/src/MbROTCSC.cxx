/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbROTCSC.h"
#include <cmath>
MbROTCSC::MbROTCSC():MbROT(),MbIDTagCSC()
{

 Tech_CSC   () ;

 m_RZsim = 0. ;  
 m_EZfit = 1. ;

}
MbROTCSC::~MbROTCSC(){}

// Set Digit
void MbROTCSC::SetMuonRecDigit(const MuonRecDigitCSC& MuonRecDigitObj){

m_MuonRecDigitObj = MuonRecDigitObj;
CopyIds(&MuonRecDigitObj);

}

// Get Digit
const MuonRecDigitCSC* MbROTCSC::GetMuonRecDigit() const{
 const MuonRecDigitCSC* ToBeReturned = &m_MuonRecDigitObj;
 return ToBeReturned;
}

// Set Parameters
void MbROTCSC::SetRZsim(double RZsim){m_RZsim = RZsim ;}
void MbROTCSC::SetEZfit(double EZfit){m_EZfit = EZfit ;}

// Get Parameters
double MbROTCSC::GetRZsim() const{return m_RZsim ;}
double MbROTCSC::GetEZfit() const{return m_EZfit ;}

//Print
void MbROTCSC::Print(std::ostream* out) const{

 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitCSC* pMuonRecDigitCSC = GetMuonRecDigit();
 std::string TheStationName = pMuonRecDigitCSC->GetStationName() ;
 int         JFF            = pMuonRecDigitCSC->GetJFF()         ;
 int         JZZ            = pMuonRecDigitCSC->GetJZZ()         ;
 int         JOB            = pMuonRecDigitCSC->GetJOB()         ;
 int         JSL            = pMuonRecDigitCSC->GetJSL()         ;
 int         JSTRI          = pMuonRecDigitCSC->GetJSTRI()       ;
 double      SZFLAG         = pMuonRecDigitCSC->GetSZFLAG()      ;

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
void MbROTCSC::PrintCout() const{

 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitCSC* pMuonRecDigitCSC = GetMuonRecDigit();
 std::string TheStationName = pMuonRecDigitCSC->GetStationName() ;
 int         JFF            = pMuonRecDigitCSC->GetJFF()         ;
 int         JZZ            = pMuonRecDigitCSC->GetJZZ()         ;
 int         JOB            = pMuonRecDigitCSC->GetJOB()         ;
 int         JSL            = pMuonRecDigitCSC->GetJSL()         ;
 int         JSTRI          = pMuonRecDigitCSC->GetJSTRI()       ;
 double      SZFLAG         = pMuonRecDigitCSC->GetSZFLAG()      ;

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
void MbROTCSC::PrintAmdIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAmdIds(out); }
void MbROTCSC::PrintAthIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAthIds(out); }
