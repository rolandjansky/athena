/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyBline.h"

using std::setiosflags;

MboyBline::MboyBline()
{  
  std::string astring = "XYZ" ;
  SetStationType(astring);
  m_jff = 0 ;
  m_jzz = 0 ;
  m_job = 0 ;

  m_bz = 0. ; 
  m_bp = 0. ;
  m_bn = 0. ;
  m_sp = 0. ;
  m_sn = 0. ;
  m_tw = 0. ;
  m_pg = 0. ;
  m_tr = 0. ;
  m_eg = 0. ;
  m_ep = 0. ;
  m_en = 0. ;

}
MboyBline::~MboyBline(){}

void MboyBline::Print(std::ostream* out) const
{

  *out      << setiosflags(std::ios::fixed);
  *out       
            <<  std::setw(3)                       << GetStationType() 
            <<  std::setw(4)                       << Getjff        () 
            <<  std::setw(4)                       << Getjzz        () 
            <<  std::setw(4)                       << Getjob        ()
            << " " << Getbz ()
            << " " << Getbp ()
            << " " << Getbn ()
            << " " << Getsp ()
            << " " << Getsn ()
            << " " << Gettw ()
            << " " << Getpg ()
            << " " << Gettr ()
            << " " << Geteg ()
            << " " << Getep ()
            << " " << Geten ()
            << std::endl; 
            
}
void MboyBline::Print()                  const
{

  std::cout << setiosflags(std::ios::fixed);
  std::cout  
            <<  std::setw(3)                       << GetStationType() 
            <<  std::setw(4)                       << Getjff        () 
            <<  std::setw(4)                       << Getjzz        () 
            <<  std::setw(4)                       << Getjob        ()
            <<  std::setw(12)<<std::setprecision(6)<< Getbz ()
            <<  std::setw(12)<<std::setprecision(6)<< Getbp ()
            <<  std::setw(12)<<std::setprecision(6)<< Getbn ()
            <<  std::setw(12)<<std::setprecision(6)<< Getsp ()
            <<  std::setw(12)<<std::setprecision(6)<< Getsn ()
            <<  std::setw(12)<<std::setprecision(6)<< Gettw ()
            <<  std::setw(12)<<std::setprecision(6)<< Getpg ()
            <<  std::setw(12)<<std::setprecision(6)<< Gettr ()
            <<  std::setw(12)<<std::setprecision(6)<< Geteg ()
            <<  std::setw(12)<<std::setprecision(6)<< Getep ()
            <<  std::setw(12)<<std::setprecision(6)<< Geten ()
            << std::endl; 
            
}
void MboyBline::SetStationType(std::string StationType){
                                                        m_StationType = StationType;
                                                        m_StationNameChar[0]= m_StationType[0];
                                                        m_StationNameChar[1]= m_StationType[1];
                                                        m_StationNameChar[2]= m_StationType[2];
						       }
void MboyBline::Setjff	      (int         jff	      ){m_jff	      = jff	   ;}
void MboyBline::Setjzz	      (int         jzz	      ){m_jzz	      = jzz	   ;}
void MboyBline::Setjob	      (int         job	      ){m_job	      = job	   ;}

std::string MboyBline::GetStationType() const {return m_StationType ;}
int	    MboyBline::Getjff	     () const {return m_jff	    ;}
int	    MboyBline::Getjzz	     () const {return m_jzz	    ;}
int	    MboyBline::Getjob	     () const {return m_job	    ;}
void        MboyBline::GetStationNameChar (char* NameI) const {
                                                               NameI[0]=m_StationNameChar[0];
                                                               NameI[1]=m_StationNameChar[1];
                                                               NameI[2]=m_StationNameChar[2];
				                              }

void MboyBline::Setbz(double bz ){m_bz = bz;}
void MboyBline::Setbp(double bp ){m_bp = bp;}
void MboyBline::Setbn(double bn ){m_bn = bn;}
void MboyBline::Setsp(double sp ){m_sp = sp;}
void MboyBline::Setsn(double sn ){m_sn = sn;}
void MboyBline::Settw(double tw ){m_tw = tw;}
void MboyBline::Setpg(double pg ){m_pg = pg;}
void MboyBline::Settr(double tr ){m_tr = tr;}
void MboyBline::Seteg(double eg ){m_eg = eg;}
void MboyBline::Setep(double ep ){m_ep = ep;}
void MboyBline::Seten(double en ){m_en = en;}

double MboyBline::Getbz() const {return m_bz;}
double MboyBline::Getbp() const {return m_bp;}
double MboyBline::Getbn() const {return m_bn;}
double MboyBline::Getsp() const {return m_sp;}
double MboyBline::Getsn() const {return m_sn;}
double MboyBline::Gettw() const {return m_tw;}
double MboyBline::Getpg() const {return m_pg;}
double MboyBline::Gettr() const {return m_tr;}
double MboyBline::Geteg() const {return m_eg;}
double MboyBline::Getep() const {return m_ep;}
double MboyBline::Geten() const {return m_en;}

int MboyBline::Getjtyp() const {
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
