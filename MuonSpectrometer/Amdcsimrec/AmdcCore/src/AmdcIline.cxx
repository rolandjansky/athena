/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/AmdcIline.h"

using std::setiosflags;

AmdcIline::AmdcIline(){

 std::string astring = "XYZ" ;
 SetStationType(astring);
 m_jff         = 0     ;
 m_jzz         = 0     ;
 m_job         = 0     ;
 m_jlay        = 0     ;

 m_s           = 0     ; 
 m_z           = 0     ;
 m_t           = 0     ;
 m_rotS        = 0     ;
 m_rotZ        = 0     ;
 m_rotT        = 0     ;

}
AmdcIline::~AmdcIline(){}

void AmdcIline::Print(std::ostream* out) const
{

  *out      << setiosflags(std::ios::fixed);
  *out       
            <<  std::setw(3)                       << GetStationType() 
            <<  std::setw(4)                       << Getjff        () 
            <<  std::setw(4)                       << Getjzz        () 
            <<  std::setw(4)                       << Getjob        ()
            <<  std::setw(4)                       << Getjlay       ()
            <<  std::setw(10)<<std::setprecision(3)<< Gets    ()
            <<  std::setw(10)<<std::setprecision(3)<< Getz    ()
            <<  std::setw(10)<<std::setprecision(3)<< Gett    ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotS ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotZ ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotT ()
            << std::endl; 
            
}
void AmdcIline::Print()                  const
{

  std::cout << setiosflags(std::ios::fixed);
  std::cout  
            <<  std::setw(3)                       << GetStationType() 
            <<  std::setw(4)                       << Getjff        () 
            <<  std::setw(4)                       << Getjzz        () 
            <<  std::setw(4)                       << Getjob        ()
            <<  std::setw(4)                       << Getjlay       ()
            <<  std::setw(10)<<std::setprecision(3)<< Gets    ()
            <<  std::setw(10)<<std::setprecision(3)<< Getz    ()
            <<  std::setw(10)<<std::setprecision(3)<< Gett    ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotS ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotZ ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotT ()
            << std::endl; 
            
}
void AmdcIline::SuperPrint(std::ostream* out) const
{

  *out      << setiosflags(std::ios::fixed);
  *out       
            <<  std::setw(3)                        << GetStationType() 
            <<  std::setw(4)                        << Getjff        () 
            <<  std::setw(4)                        << Getjzz        () 
            <<  std::setw(4)                        << Getjob        ()
            <<  std::setw(4)                        << Getjlay       ()
            <<  std::setw(13)<<std::setprecision(6) << Gets    ()
            <<  std::setw(13)<<std::setprecision(6) << Getz    ()
            <<  std::setw(13)<<std::setprecision(6) << Gett    ()
            <<  std::setw(16)<<std::setprecision(12)<< GetrotS ()
            <<  std::setw(16)<<std::setprecision(12)<< GetrotZ ()
            <<  std::setw(16)<<std::setprecision(12)<< GetrotT ()
            << std::endl; 
            
}
void AmdcIline::SuperPrint()                  const
{

  std::cout << setiosflags(std::ios::fixed);
  std::cout  
            <<  std::setw(3)                        << GetStationType() 
            <<  std::setw(4)                        << Getjff        () 
            <<  std::setw(4)                        << Getjzz        () 
            <<  std::setw(4)                        << Getjob        ()
            <<  std::setw(4)                        << Getjlay       ()
            <<  std::setw(13)<<std::setprecision(6) << Gets    ()
            <<  std::setw(13)<<std::setprecision(6) << Getz    ()
            <<  std::setw(13)<<std::setprecision(6) << Gett    ()
            <<  std::setw(16)<<std::setprecision(12)<< GetrotS ()
            <<  std::setw(16)<<std::setprecision(12)<< GetrotZ ()
            <<  std::setw(16)<<std::setprecision(12)<< GetrotT ()
            << std::endl; 
            
}
void AmdcIline::SetStationType(std::string StationType){
                                                        m_StationType = StationType;
                                                        m_StationNameChar[0]= m_StationType[0];
                                                        m_StationNameChar[1]= m_StationType[1];
                                                        m_StationNameChar[2]= m_StationType[2];
						       }
void AmdcIline::Setjff  (int jff  ){m_jff  = jff  ;}
void AmdcIline::Setjzz  (int jzz  ){m_jzz  = jzz  ;}
void AmdcIline::Setjob  (int job  ){m_job  = job  ;}
void AmdcIline::Setjlay (int jlay ){m_jlay = jlay ;}

std::string AmdcIline::GetStationType() const {return m_StationType ;}
int	    AmdcIline::Getjff	     () const {return m_jff	    ;}
int	    AmdcIline::Getjzz	     () const {return m_jzz	    ;}
int	    AmdcIline::Getjob	     () const {return m_job	    ;}
int	    AmdcIline::Getjlay       () const {return m_jlay        ;}
void        AmdcIline::GetStationNameChar (char* NameI) const {
                                                               NameI[0]=m_StationNameChar[0];
                                                               NameI[1]=m_StationNameChar[1];
                                                               NameI[2]=m_StationNameChar[2];
				                              }

void AmdcIline::Sets    (double s    ){m_s    = s    ;}
void AmdcIline::Setz    (double z    ){m_z    = z    ;}
void AmdcIline::Sett    (double t    ){m_t    = t    ;}
void AmdcIline::SetrotS (double rotS ){m_rotS = rotS ;}
void AmdcIline::SetrotZ (double rotZ ){m_rotZ = rotZ ;}
void AmdcIline::SetrotT (double rotT ){m_rotT = rotT ;}

double AmdcIline::Gets    () const {return m_s    ;}
double AmdcIline::Getz    () const {return m_z    ;}
double AmdcIline::Gett    () const {return m_t    ;}
double AmdcIline::GetrotS () const {return m_rotS ;}
double AmdcIline::GetrotZ () const {return m_rotZ ;}
double AmdcIline::GetrotT () const {return m_rotT ;}

int AmdcIline::Getjtyp() const {
 if ( m_StationType == "BIL" ) return  1;
 if ( m_StationType == "BIS" ) return  2;
 if ( m_StationType == "BML" ) return  3;
 if ( m_StationType == "BMS" ) return  4;
 if ( m_StationType == "BOL" ) return  5;
 if ( m_StationType == "BOS" ) return  6;
 if ( m_StationType == "BEE" ) return  7; 
 if ( m_StationType == "BIR" ) return  8;
 if ( m_StationType == "BMF" ) return  9;
 if ( m_StationType == "BOF" ) return 10;
 if ( m_StationType == "BOG" ) return 11;
 if ( m_StationType == "BME" ) return 12; 
 if ( m_StationType == "EIC" ) return 13;
 if ( m_StationType == "EIL" ) return 14; 
 if ( m_StationType == "EEL" ) return 15;
 if ( m_StationType == "EES" ) return 16; 
 if ( m_StationType == "EMC" ) return 17;
 if ( m_StationType == "EML" ) return 18;
 if ( m_StationType == "EMS" ) return 19; 
 if ( m_StationType == "EOC" ) return 20;
 if ( m_StationType == "EOL" ) return 21;
 if ( m_StationType == "EOS" ) return 22;
 if ( m_StationType == "T1C" ) return 23;
 if ( m_StationType == "T1L" ) return 24;
 if ( m_StationType == "T1S" ) return 25;
 if ( m_StationType == "T2C" ) return 26;
 if ( m_StationType == "T2L" ) return 27;
 if ( m_StationType == "T2S" ) return 28;
 if ( m_StationType == "T3C" ) return 29;
 if ( m_StationType == "T3L" ) return 30;
 if ( m_StationType == "T3S" ) return 31;
 if ( m_StationType == "CI1" ) return 32;
 if ( m_StationType == "CI2" ) return 33;
 if ( m_StationType == "CI3" ) return 34;
 if ( m_StationType == "CI4" ) return 35;
 if ( m_StationType == "FIL" ) return 36;
 if ( m_StationType == "FIS" ) return 37;
 if ( m_StationType == "FML" ) return 38;
 if ( m_StationType == "FMS" ) return 39;
 if ( m_StationType == "FOL" ) return 40;
 if ( m_StationType == "FOS" ) return 41;
 if ( m_StationType == "T1F" ) return 42;
 if ( m_StationType == "T1E" ) return 43;
 if ( m_StationType == "T2F" ) return 44;
 if ( m_StationType == "T2E" ) return 45;
 if ( m_StationType == "T3F" ) return 46;
 if ( m_StationType == "T3E" ) return 47;
 if ( m_StationType == "T4F" ) return 48;
 if ( m_StationType == "T4E" ) return 49;
 if ( m_StationType == "EIS" ) return 50;
 if ( m_StationType == "CSS" ) return 51;
 if ( m_StationType == "CSL" ) return 52;
 if ( m_StationType == "BIM" ) return 53;
 if ( m_StationType == "BMG" ) return 54;
 return -1;
}
