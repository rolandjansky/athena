/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbMuSc.h"
#include <cmath>

MbMuSc::MbMuSc(const MbMuSc& aMbMuSc ):MbPOT(aMbMuSc)
{
 m_RRMASC    = aMbMuSc.GetRRMASC   () ; 
 
 m_LLMASC    = aMbMuSc.GetLLMASC   () ;

 m_X0MASC    = aMbMuSc.GetX0MASC   () ;

 m_S1MASC    = aMbMuSc.GetS1MASC   () ;
 m_S2MASC    = aMbMuSc.GetS2MASC   () ;
 m_THMASC    = aMbMuSc.GetTHMASC   () ;

 m_ALMASC    = aMbMuSc.GetALMASC   () ;
 m_BLMASC    = aMbMuSc.GetBLMASC   () ;

 m_XposMASC  = aMbMuSc.GetXposMASC () ;
 m_YposMASC  = aMbMuSc.GetYposMASC () ;
 m_ZposMASC  = aMbMuSc.GetZposMASC () ;
 
 m_XdirMASC  = aMbMuSc.GetXdirMASC () ;
 m_YdirMASC  = aMbMuSc.GetYdirMASC () ;
 m_ZdirMASC  = aMbMuSc.GetZdirMASC () ;
 
 m_Xdir2MASC = aMbMuSc.GetXdir2MASC() ;
 m_Ydir2MASC = aMbMuSc.GetYdir2MASC() ;
 m_Zdir2MASC = aMbMuSc.GetZdir2MASC() ;
 
 m_PmomMASC  = aMbMuSc.GetPmomMASC () ;

 m_Pmom2MASC = aMbMuSc.GetPmom2MASC() ;

 m_ChgeMASC  = aMbMuSc.GetChgeMASC () ;

}
MbMuSc::MbMuSc():MbPOT()
{
 Type_MUSC () ;
 m_RRMASC    = 0 ; 
 
 m_LLMASC    = 0 ;

 m_X0MASC    = 0 ;

 m_S1MASC    = 0 ;
 m_S2MASC    = 0 ;
 m_THMASC    = 0 ;

 m_ALMASC    = 0 ;
 m_BLMASC    = 0 ;

 m_XposMASC  = 0 ;
 m_YposMASC  = 0 ;
 m_ZposMASC  = 0 ;
 
 m_XdirMASC  = 0 ;
 m_YdirMASC  = 0 ;
 m_ZdirMASC  = 0 ;
 
 m_Xdir2MASC = 0 ;
 m_Ydir2MASC = 0 ;
 m_Zdir2MASC = 0 ;
 
 m_PmomMASC  = 0 ;

 m_Pmom2MASC = 0 ;

 m_ChgeMASC  = 0 ;

}

MbMuSc::~MbMuSc(){}

void MbMuSc::SetRRMASC(const double& RRMASC_in){ m_RRMASC = RRMASC_in ; }

void MbMuSc::SetLLMASC(const int&    LLMASC_in){ m_LLMASC = LLMASC_in ; }

void MbMuSc::SetX0MASC(const double& X0MASC_in){ m_X0MASC = X0MASC_in ; }

void MbMuSc::SetS1MASC(const double& S1MASC_in){ m_S1MASC = S1MASC_in ; }
void MbMuSc::SetS2MASC(const double& S2MASC_in){ m_S2MASC = S2MASC_in ; }
void MbMuSc::SetTHMASC(const double& THMASC_in){ m_THMASC = THMASC_in ; }

void MbMuSc::SetALMASC(const double& ALMASC_in){ m_ALMASC = ALMASC_in ; }
void MbMuSc::SetBLMASC(const double& BLMASC_in){ m_BLMASC = BLMASC_in ; }

void MbMuSc::SetPosDirMASC(
                       const double& XposMASC_in ,
                       const double& YposMASC_in ,
                       const double& ZposMASC_in ,
                       const double& XdirMASC_in ,
                       const double& YdirMASC_in ,
                       const double& ZdirMASC_in
){
  m_XposMASC = XposMASC_in ;  
  m_YposMASC = YposMASC_in ; 
  m_ZposMASC = ZposMASC_in ; 
  m_XdirMASC = XdirMASC_in ; 
  m_YdirMASC = YdirMASC_in ; 
  m_ZdirMASC = ZdirMASC_in ; 

  SetXdca ( GetXposMASC() );
  SetYdca ( GetYposMASC() );
  SetZdca ( GetZposMASC() );
  
  SetXdir ( GetXdirMASC() );
  SetYdir ( GetYdirMASC() );
  SetZdir ( GetZdirMASC() );
}

void MbMuSc::SetDir2MASC(
                       const double& XdirMASC_in ,
                       const double& YdirMASC_in ,
                       const double& ZdirMASC_in
){
  m_Xdir2MASC = XdirMASC_in ; 
  m_Ydir2MASC = YdirMASC_in ; 
  m_Zdir2MASC = ZdirMASC_in ; 
}

double MbMuSc::GetRRMASC() const { return m_RRMASC ; }

int    MbMuSc::GetLLMASC() const { return m_LLMASC ; }

double MbMuSc::GetX0MASC() const { return m_X0MASC ; }

double MbMuSc::GetS1MASC() const { return m_S1MASC ; }
double MbMuSc::GetS2MASC() const { return m_S2MASC ; }
double MbMuSc::GetTHMASC() const { return m_THMASC ; }

double MbMuSc::GetALMASC() const { return m_ALMASC ; }
double MbMuSc::GetBLMASC() const { return m_BLMASC ; }

double MbMuSc::GetXposMASC() const { return m_XposMASC ; }
double MbMuSc::GetYposMASC() const { return m_YposMASC ; }
double MbMuSc::GetZposMASC() const { return m_ZposMASC ; }

double MbMuSc::GetXdirMASC() const { return m_XdirMASC ; }
double MbMuSc::GetYdirMASC() const { return m_YdirMASC ; }
double MbMuSc::GetZdirMASC() const { return m_ZdirMASC ; }

double MbMuSc::GetXdir2MASC() const { return m_Xdir2MASC ; }
double MbMuSc::GetYdir2MASC() const { return m_Ydir2MASC ; }
double MbMuSc::GetZdir2MASC() const { return m_Zdir2MASC ; }

void MbMuSc::SetPmomMASC(const double& PmomMASC_in)
{ 
  m_PmomMASC = PmomMASC_in ; 
  SetEner ( GetPmomMASC() ); 
}

void MbMuSc::SetPmom2MASC(const double& PmomMASC_in)
{ 
  m_Pmom2MASC = PmomMASC_in ; 
}

void MbMuSc::SetChgeMASC(const double& ChgeMASC_in){ m_ChgeMASC = ChgeMASC_in ; }

double MbMuSc::GetPmomMASC() const { return m_PmomMASC ; }

double MbMuSc::GetPmom2MASC() const { return m_Pmom2MASC ; }

double MbMuSc::GetChgeMASC() const { return m_ChgeMASC ; }


void MbMuSc::Print(std::ostream* out) const{

 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;

 *out << std::setw(5)<< GetType()
      << std::setw(5)<< GetTech() ;
 *out << "  " << std::setw(3) << GetLLMASC()
      << std::setw(10)<<std::setprecision(2)<< GetX0MASC()  
      << std::setw(10)<<std::setprecision(5)<< GetALMASC()  
      << std::setw(10)<<std::setprecision(5)<< GetBLMASC()        
      << std::setw(10)<<std::setprecision(5)<< GetALMASC() + GetPmomMASC() * GetBLMASC()
      << std::setw(10)<<std::setprecision(5)<< GetS1MASC()  
      << std::setw(10)<<std::setprecision(5)<< GetS2MASC()  
      << std::setw(10)<<std::setprecision(5)<< GetTHMASC()  
      << std::setw(11)<<std::setprecision(1)<< GetPmomMASC()  
      << std::setw(11)<<std::setprecision(1)<< GetPmom2MASC()  
      << "  " 
      << std::setw(12)<<std::setprecision(4)<< XdcaL
      << std::setw(12)<<std::setprecision(4)<< YdcaL
      << std::setw(12)<<std::setprecision(4)<< ZdcaL
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
void MbMuSc::PrintCout() const{

 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;

 std::cout << std::setw(5)<< GetType()
      << std::setw(5)<< GetTech() ;
 std::cout << "  " << std::setw(3) << GetLLMASC()
      << std::setw(10)<<std::setprecision(2)<< GetX0MASC()  
      << std::setw(10)<<std::setprecision(5)<< GetALMASC()  
      << std::setw(10)<<std::setprecision(5)<< GetBLMASC()        
      << std::setw(10)<<std::setprecision(5)<< GetALMASC() + GetPmomMASC() * GetBLMASC()
      << std::setw(10)<<std::setprecision(5)<< GetS1MASC()  
      << std::setw(10)<<std::setprecision(5)<< GetS2MASC()  
      << std::setw(10)<<std::setprecision(5)<< GetTHMASC()  
      << std::setw(11)<<std::setprecision(1)<< GetPmomMASC()  
      << std::setw(11)<<std::setprecision(1)<< GetPmom2MASC()  
      << "          " 
      << std::setw(12)<<std::setprecision(4)<< XdcaL
      << std::setw(12)<<std::setprecision(4)<< YdcaL
      << std::setw(12)<<std::setprecision(4)<< ZdcaL
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
