/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyGenTrack.h"

MboyGenTrack::MboyGenTrack(){
 m_PDGCode = 0 ;  
 m_PosX    = 0 ;
 m_PosY    = 0 ;
 m_PosZ    = 0 ;
 m_PX      = 0 ;
 m_PY      = 0 ;  
 m_PZ      = 0 ;
}
MboyGenTrack::~MboyGenTrack(){}

// Set Parameters
void MboyGenTrack::SetPDGCode(int    PDGCode){m_PDGCode = PDGCode;} 
void MboyGenTrack::SetPosX   (double PosX   ){m_PosX	= PosX   ;}
void MboyGenTrack::SetPosY   (double PosY   ){m_PosY	= PosY   ;}
void MboyGenTrack::SetPosZ   (double PosZ   ){m_PosZ	= PosZ   ;}
void MboyGenTrack::SetPX     (double PX	    ){m_PX	= PX	 ;}
void MboyGenTrack::SetPY     (double PY	    ){m_PY	= PY	 ;} 
void MboyGenTrack::SetPZ     (double PZ	    ){m_PZ	= PZ	 ;}

// Get Parameters
int    MboyGenTrack::GetPDGCode() const{return m_PDGCode ;}
double MboyGenTrack::GetPosX   () const{return m_PosX    ;}
double MboyGenTrack::GetPosY   () const{return m_PosY    ;}
double MboyGenTrack::GetPosZ   () const{return m_PosZ    ;}
double MboyGenTrack::GetPX     () const{return m_PX      ;}
double MboyGenTrack::GetPY     () const{return m_PY      ;} 
double MboyGenTrack::GetPZ     () const{return m_PZ      ;}

// Print
void MboyGenTrack::Print
  (std::ostream* out) const{
//
   *out
           << "TRK" << " "
           << "Sim" << " "
           << GetPDGCode() << " "
           << GetPosX   () << " "
           << GetPosY   () << " "
           << GetPosZ   () << " "
           << GetPX     ()  << " "
           << GetPY     () << " "
           << GetPZ     () << " "
           << std::endl;
//
}
 
