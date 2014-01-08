/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbROTTGC.h"
#include <cmath>
MbROTTGC::MbROTTGC():MbROT(),MbIDTagTGC()
{ 

 Tech_TGC   () ;

 m_RSsim = 0. ; 
 m_ESfit = 1. ;
 m_Rdigi = 0. ;

}
MbROTTGC::~MbROTTGC(){}

// Set Digit
void MbROTTGC::SetMuonRecDigit(const MuonRecDigitTGC& MuonRecDigitObj){

m_MuonRecDigitObj = MuonRecDigitObj;
CopyIds(&MuonRecDigitObj);

}

// Get Digit
const MuonRecDigitTGC* MbROTTGC::GetMuonRecDigit() const{
 const MuonRecDigitTGC* ToBeReturned = &m_MuonRecDigitObj;
 return ToBeReturned;
}

// Set Parameters
void MbROTTGC::SetRSsim(double RSsim){m_RSsim = RSsim ;}
void MbROTTGC::SetESfit(double ESfit){m_ESfit = ESfit ;}

// Get Parameters
double MbROTTGC::GetRSsim() const{return m_RSsim ;}
double MbROTTGC::GetESfit() const{return m_ESfit ;}

// Set Parameters
void MbROTTGC::SetRdigi(double Rdigi){m_Rdigi = Rdigi ;}

// Get Parameters
double MbROTTGC::GetRdigi() const{return m_Rdigi ;}


//Print
void MbROTTGC::Print(std::ostream* out) const{

//double Rdigi = GetRdigi()/10.;
 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitTGC* pMuonRecDigitTGC = GetMuonRecDigit();
 std::string TheStationName = pMuonRecDigitTGC->GetStationName() ;
 int         JFF            = pMuonRecDigitTGC->GetJFF()         ;
 int         JZZ            = pMuonRecDigitTGC->GetJZZ()         ;
 int         JOB            = pMuonRecDigitTGC->GetJOB()         ;
 int         JSPLI          = pMuonRecDigitTGC->GetJSPLI()       ;
 int         JSL            = pMuonRecDigitTGC->GetJSL()         ;
 int         JSZ            = pMuonRecDigitTGC->GetJSZ()         ;
 int         JSTRI          = pMuonRecDigitTGC->GetJSTRI()       ;

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
      << std::setw(5)<< JSPLI  
      << std::setw(5)<< JSL  
      << std::setw(5)<< JSZ  
      << std::setw(5)<< JSTRI
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
void MbROTTGC::PrintCout() const{

//double Rdigi = GetRdigi()/10.;
 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double ResoL = GetReso() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 const MuonRecDigitTGC* pMuonRecDigitTGC = GetMuonRecDigit();
 std::string TheStationName = pMuonRecDigitTGC->GetStationName() ;
 int         JFF            = pMuonRecDigitTGC->GetJFF()         ;
 int         JZZ            = pMuonRecDigitTGC->GetJZZ()         ;
 int         JOB            = pMuonRecDigitTGC->GetJOB()         ;
 int         JSPLI          = pMuonRecDigitTGC->GetJSPLI()       ;
 int         JSL            = pMuonRecDigitTGC->GetJSL()         ;
 int         JSZ            = pMuonRecDigitTGC->GetJSZ()         ;
 int         JSTRI          = pMuonRecDigitTGC->GetJSTRI()       ;

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
      << std::setw(5)<< JSPLI  
      << std::setw(5)<< JSL  
      << std::setw(5)<< JSZ  
      << std::setw(5)<< JSTRI
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
void MbROTTGC::PrintAmdIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAmdIds(out); }
void MbROTTGC::PrintAthIds(std::ostream* out) const{ GetMuonRecDigit()->PrintAthIds(out); }
