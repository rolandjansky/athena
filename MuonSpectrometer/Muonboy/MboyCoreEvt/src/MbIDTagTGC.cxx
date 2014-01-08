/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIDTagTGC.h"

MbIDTagTGC::MbIDTagTGC():MbIDTag()
{
 
 m_SGGasGap        = 0     ;
 m_SGIsStrip       = 0     ;
 m_SGChannel       = 0     ;

 m_JSPLI           = 0     ;
 m_JSL             = 0     ;
 m_JSZ             = 0     ;
 m_JSTRI           = 0     ;

}
MbIDTagTGC::~MbIDTagTGC(){}
MbIDTagTGC::MbIDTagTGC(const MbIDTagTGC& ToBeCopied):MbIDTag(ToBeCopied)
{ 
  CopyIds(&ToBeCopied) ;
}
MbIDTagTGC& MbIDTagTGC::operator=( const MbIDTagTGC& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
    MbIDTag::operator=(ToBeCopied);
    CopyIds(&ToBeCopied) ;
  }
  return *this; 
}

// Set All

void MbIDTagTGC::SetSGGasGap  ( const int& SGGasGap  ) { m_SGGasGap  = SGGasGap  ; }
void MbIDTagTGC::SetSGIsStrip ( const int& SGIsStrip ) { m_SGIsStrip = SGIsStrip ; }
void MbIDTagTGC::SetSGChannel ( const int& SGChannel ) { m_SGChannel = SGChannel ; }
void MbIDTagTGC::SetJSPLI     ( const int& JSPLI     ) { m_JSPLI     = JSPLI     ; }
void MbIDTagTGC::SetJSL       ( const int& JSL       ) { m_JSL       = JSL       ; }
void MbIDTagTGC::SetJSZ       ( const int& JSZ       ) { m_JSZ       = JSZ       ; }
void MbIDTagTGC::SetJSTRI     ( const int& JSTRI     ) { m_JSTRI     = JSTRI     ; }

// Get All
                            
int MbIDTagTGC::GetSGGasGap  () const { return  m_SGGasGap  ; }
int MbIDTagTGC::GetSGIsStrip () const { return  m_SGIsStrip ; }
int MbIDTagTGC::GetSGChannel () const { return  m_SGChannel ; }
int MbIDTagTGC::GetJSPLI     () const { return m_JSPLI      ; }
int MbIDTagTGC::GetJSL       () const { return m_JSL        ; }
int MbIDTagTGC::GetJSZ       () const { return m_JSZ        ; }
int MbIDTagTGC::GetJSTRI     () const { return m_JSTRI      ; }

void MbIDTagTGC::PrintAmdIds(std::ostream* out) const{

    *out
                    << "TRI "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_JFF
                    << std::setw(4)<< m_JZZ
                    << std::setw(4)<< m_JOB
                    << std::setw(4)<< m_JSPLI
                    << std::setw(4)<< m_JSL
                    << std::setw(4)<< m_JSZ
                    << std::setw(4)<< m_JSTRI;

}
void MbIDTagTGC::PrintAthIds(std::ostream* out) const{

    *out
                    << "TGC "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta
                    << std::setw(4)<< m_SGStationPhi
                    << std::setw(4)<< m_SGGasGap  
                    << std::setw(4)<< m_SGIsStrip 
                    << std::setw(4)<< m_SGChannel;

}

void MbIDTagTGC::CopyIds(const MbIDTagTGC* pMbIDTag){
CopyBaseIds(pMbIDTag);
SetSGGasGap  ( pMbIDTag->GetSGGasGap  () );
SetSGIsStrip ( pMbIDTag->GetSGIsStrip () );
SetSGChannel ( pMbIDTag->GetSGChannel () );
SetJSPLI     ( pMbIDTag->GetJSPLI     () );
SetJSL       ( pMbIDTag->GetJSL       () );
SetJSZ       ( pMbIDTag->GetJSZ       () );
SetJSTRI     ( pMbIDTag->GetJSTRI     () );
}
