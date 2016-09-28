/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/AmdcBline.h"

using std::setiosflags;

AmdcBline::AmdcBline(){

 std::string astring = "XYZ" ;
 SetStationType(astring);
 m_jff = 0 ;
 m_jzz = 0 ;
 m_job = 0 ;

 m_bz = 0 ; 
 m_bp = 0 ;
 m_bn = 0 ;
 m_sp = 0 ;
 m_sn = 0 ;
 m_tw = 0 ;
 m_pg = 0 ;
 m_tr = 0 ;
 m_eg = 0 ;
 m_ep = 0 ;
 m_en = 0 ;

}
AmdcBline::~AmdcBline(){}

void AmdcBline::Print(std::ostream* out) const
{

  *out       
            << " " << GetStationType() 
            << " " << Getjff        () 
            << " " << Getjzz        () 
            << " " << Getjob        ()
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
void AmdcBline::Print()                  const
{

  std::cout  
            << " " << GetStationType() 
            << " " << Getjff        () 
            << " " << Getjzz        () 
            << " " << Getjob        ()
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
void AmdcBline::SuperPrint(std::ostream* out) const
{

  *out      << setiosflags(std::ios::fixed);
  *out       
            <<  std::setw(3)                        << GetStationType() 
            <<  std::setw(4)                        << Getjff        () 
            <<  std::setw(4)                        << Getjzz        () 
            <<  std::setw(4)                        << Getjob        ()
            <<  std::setw(16)<<std::setprecision(12)<< Getbz ()
            <<  std::setw(16)<<std::setprecision(12)<< Getbp ()
            <<  std::setw(16)<<std::setprecision(12)<< Getbn ()
            <<  std::setw(16)<<std::setprecision(12)<< Getsp ()
            <<  std::setw(16)<<std::setprecision(12)<< Getsn ()
            <<  std::setw(16)<<std::setprecision(12)<< Gettw ()
            <<  std::setw(16)<<std::setprecision(12)<< Getpg ()
            <<  std::setw(16)<<std::setprecision(12)<< Gettr ()
            <<  std::setw(16)<<std::setprecision(12)<< Geteg ()
            <<  std::setw(16)<<std::setprecision(12)<< Getep ()
            <<  std::setw(16)<<std::setprecision(12)<< Geten ()
            << std::endl; 
            
}
void AmdcBline::SuperPrint()                  const
{

  std::cout << setiosflags(std::ios::fixed);
  std::cout  
            <<  std::setw(3)                        << GetStationType() 
            <<  std::setw(4)                        << Getjff        () 
            <<  std::setw(4)                        << Getjzz        () 
            <<  std::setw(4)                        << Getjob        ()
            <<  std::setw(16)<<std::setprecision(12)<< Getbz ()
            <<  std::setw(16)<<std::setprecision(12)<< Getbp ()
            <<  std::setw(16)<<std::setprecision(12)<< Getbn ()
            <<  std::setw(16)<<std::setprecision(12)<< Getsp ()
            <<  std::setw(16)<<std::setprecision(12)<< Getsn ()
            <<  std::setw(16)<<std::setprecision(12)<< Gettw ()
            <<  std::setw(16)<<std::setprecision(12)<< Getpg ()
            <<  std::setw(16)<<std::setprecision(12)<< Gettr ()
            <<  std::setw(16)<<std::setprecision(12)<< Geteg ()
            <<  std::setw(16)<<std::setprecision(12)<< Getep ()
            <<  std::setw(16)<<std::setprecision(12)<< Geten ()
            << std::endl; 
            
}
void AmdcBline::SetStationType(std::string StationType){
                                                        m_StationType = StationType;
                                                        m_StationNameChar[0]= m_StationType[0];
                                                        m_StationNameChar[1]= m_StationType[1];
                                                        m_StationNameChar[2]= m_StationType[2];
						       }
void AmdcBline::Setjff	      (int         jff	      ){m_jff	      = jff	   ;}
void AmdcBline::Setjzz	      (int         jzz	      ){m_jzz	      = jzz	   ;}
void AmdcBline::Setjob	      (int         job	      ){m_job	      = job	   ;}

std::string AmdcBline::GetStationType() const {return m_StationType ;}
int	    AmdcBline::Getjff	     () const {return m_jff	    ;}
int	    AmdcBline::Getjzz	     () const {return m_jzz	    ;}
int	    AmdcBline::Getjob	     () const {return m_job	    ;}
void        AmdcBline::GetStationNameChar (char* NameI) const {
                                                               NameI[0]=m_StationNameChar[0];
                                                               NameI[1]=m_StationNameChar[1];
                                                               NameI[2]=m_StationNameChar[2];
				                              }

void AmdcBline::Setbz(double bz ){m_bz = bz;}
void AmdcBline::Setbp(double bp ){m_bp = bp;}
void AmdcBline::Setbn(double bn ){m_bn = bn;}
void AmdcBline::Setsp(double sp ){m_sp = sp;}
void AmdcBline::Setsn(double sn ){m_sn = sn;}
void AmdcBline::Settw(double tw ){m_tw = tw;}
void AmdcBline::Setpg(double pg ){m_pg = pg;}
void AmdcBline::Settr(double tr ){m_tr = tr;}
void AmdcBline::Seteg(double eg ){m_eg = eg;}
void AmdcBline::Setep(double ep ){m_ep = ep;}
void AmdcBline::Seten(double en ){m_en = en;}

double AmdcBline::Getbz() const {return m_bz;}
double AmdcBline::Getbp() const {return m_bp;}
double AmdcBline::Getbn() const {return m_bn;}
double AmdcBline::Getsp() const {return m_sp;}
double AmdcBline::Getsn() const {return m_sn;}
double AmdcBline::Gettw() const {return m_tw;}
double AmdcBline::Getpg() const {return m_pg;}
double AmdcBline::Gettr() const {return m_tr;}
double AmdcBline::Geteg() const {return m_eg;}
double AmdcBline::Getep() const {return m_ep;}
double AmdcBline::Geten() const {return m_en;}

int AmdcBline::Getjtyp() const {
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
