/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIDTagMDT.h"

MbIDTagMDT::MbIDTagMDT():MbIDTag()
{
   
 m_SGMultilayer    = 0     ; 
 m_SGTubeLayer     = 0     ;
 m_SGTube          = 0     ;

 m_JSL             = 0     ;
 m_JTUBE           = 0     ;

}
MbIDTagMDT::~MbIDTagMDT(){}
MbIDTagMDT::MbIDTagMDT(const MbIDTagMDT& ToBeCopied):MbIDTag(ToBeCopied) 
{ 
  CopyIds(&ToBeCopied) ;
}
MbIDTagMDT& MbIDTagMDT::operator=( const MbIDTagMDT& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
    MbIDTag::operator=(ToBeCopied);
    CopyIds(&ToBeCopied) ;
  }
  return *this; 
}

// Set All

void MbIDTagMDT::SetSGMultilayer ( const int& SGMultilayer ){ m_SGMultilayer = SGMultilayer ; }
void MbIDTagMDT::SetSGTubeLayer  ( const int& SGTubeLayer  ){ m_SGTubeLayer  = SGTubeLayer  ; }
void MbIDTagMDT::SetSGTube       ( const int& SGTube       ){ m_SGTube       = SGTube       ; } 
void MbIDTagMDT::SetJSL          ( const int& JSL          ){ m_JSL          = JSL          ; }
void MbIDTagMDT::SetJTUBE        ( const int& JTUBE        ){ m_JTUBE        = JTUBE        ; }

// Get All

int MbIDTagMDT::GetSGMultilayer () const { return m_SGMultilayer ; }
int MbIDTagMDT::GetSGTubeLayer  () const { return m_SGTubeLayer  ; }
int MbIDTagMDT::GetSGTube       () const { return m_SGTube       ; }
int MbIDTagMDT::GetJSL          () const { return m_JSL          ; }
int MbIDTagMDT::GetJTUBE        () const { return m_JTUBE        ; }

void MbIDTagMDT::PrintAmdIds(std::ostream* out) const{

    *out
                    << "TUB "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_JFF
                    << std::setw(4)<< m_JZZ
                    << std::setw(4)<< m_JOB
                    << std::setw(4)<< m_JSL
                    << std::setw(4)<< m_JTUBE;

}
void MbIDTagMDT::PrintAthIds(std::ostream* out) const{

    *out
                    << "MDT "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta
                    << std::setw(4)<< m_SGStationPhi
                    << std::setw(4)<< m_SGMultilayer
                    << std::setw(4)<< m_SGTubeLayer
                    << std::setw(4)<< m_SGTube;

}

void MbIDTagMDT::CopyIds(const MbIDTagMDT* pMbIDTag){
CopyBaseIds(pMbIDTag);
SetSGMultilayer ( pMbIDTag->GetSGMultilayer () );
SetSGTubeLayer  ( pMbIDTag->GetSGTubeLayer  () );
SetSGTube       ( pMbIDTag->GetSGTube       () );
SetJSL          ( pMbIDTag->GetJSL          () );
SetJTUBE        ( pMbIDTag->GetJTUBE        () );
}
