/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyAline.h"

using std::setiosflags;

MboyAline::MboyAline()
{
  std::string astring = "XYZ" ;
  SetStationType(astring);
  m_jff = 0 ;
  m_jzz = 0 ;
  m_job = 0 ;

  m_s    = 0. ; 
  m_z    = 0. ;
  m_t    = 0. ;
  m_rotS = 0. ;
  m_rotZ = 0. ;
  m_rotT = 0. ;
}
MboyAline::~MboyAline(){}

void MboyAline::Print(std::ostream* out) const
{

  *out      << setiosflags(std::ios::fixed);
  *out       
            <<  std::setw(3)                       << GetStationType() 
            <<  std::setw(4)                       << Getjff        () 
            <<  std::setw(4)                       << Getjzz        () 
            <<  std::setw(4)                       << Getjob        ()
            <<  std::setw(10)<<std::setprecision(3)<< Gets    ()
            <<  std::setw(10)<<std::setprecision(3)<< Getz    ()
            <<  std::setw(10)<<std::setprecision(3)<< Gett    ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotS ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotZ ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotT ()
            << std::endl; 
            
}
void MboyAline::Print()                  const
{

  std::cout << setiosflags(std::ios::fixed);
  std::cout  
            <<  std::setw(3)                       << GetStationType() 
            <<  std::setw(4)                       << Getjff        () 
            <<  std::setw(4)                       << Getjzz        () 
            <<  std::setw(4)                       << Getjob        ()
            <<  std::setw(10)<<std::setprecision(3)<< Gets    ()
            <<  std::setw(10)<<std::setprecision(3)<< Getz    ()
            <<  std::setw(10)<<std::setprecision(3)<< Gett    ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotS ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotZ ()
            <<  std::setw(10)<<std::setprecision(6)<< GetrotT ()
            << std::endl; 
            
}
void MboyAline::SetStationType(std::string StationType){
                                                        m_StationType = StationType;
                                                        m_StationNameChar[0]= m_StationType[0];
                                                        m_StationNameChar[1]= m_StationType[1];
                                                        m_StationNameChar[2]= m_StationType[2];
						       }
void MboyAline::Setjff	      (int         jff	      ){m_jff	      = jff	   ;}
void MboyAline::Setjzz	      (int         jzz	      ){m_jzz	      = jzz	   ;}
void MboyAline::Setjob	      (int         job	      ){m_job	      = job	   ;}

std::string MboyAline::GetStationType() const {return m_StationType ;}
int	    MboyAline::Getjff	     () const {return m_jff	    ;}
int	    MboyAline::Getjzz	     () const {return m_jzz	    ;}
int	    MboyAline::Getjob	     () const {return m_job	    ;}
void        MboyAline::GetStationNameChar (char* NameI) const {
                                                               NameI[0]=m_StationNameChar[0];
                                                               NameI[1]=m_StationNameChar[1];
                                                               NameI[2]=m_StationNameChar[2];
				                              }

void MboyAline::Sets    (double s    ){m_s    = s    ;}
void MboyAline::Setz    (double z    ){m_z    = z    ;}
void MboyAline::Sett    (double t    ){m_t    = t    ;}
void MboyAline::SetrotS (double rotS ){m_rotS = rotS ;}
void MboyAline::SetrotZ (double rotZ ){m_rotZ = rotZ ;}
void MboyAline::SetrotT (double rotT ){m_rotT = rotT ;}

double MboyAline::Gets    () const {return m_s    ;}
double MboyAline::Getz    () const {return m_z    ;}
double MboyAline::Gett    () const {return m_t    ;}
double MboyAline::GetrotS () const {return m_rotS ;}
double MboyAline::GetrotZ () const {return m_rotZ ;}
double MboyAline::GetrotT () const {return m_rotT ;}

int MboyAline::Getjtyp() const {
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
 return -1;
}
