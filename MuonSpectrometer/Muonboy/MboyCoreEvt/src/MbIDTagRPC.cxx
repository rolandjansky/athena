/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIDTagRPC.h"

MbIDTagRPC::MbIDTagRPC():MbIDTag()
{
   
 m_SGDoubletR      = 0     ;
 m_SGDoubletZ      = 0     ;
 m_SGDoubletPhi    = 0     ;
 m_SGGasGap        = 0     ;
 m_SGMeasuresPhi   = 0     ; 
 m_SGStrip         = 0     ;

 m_JSPLI           = 0     ;
 m_JSL             = 0     ;
 m_JSZ             = 0     ;
 m_JSTRI           = 0     ;

}
MbIDTagRPC::~MbIDTagRPC(){}
MbIDTagRPC::MbIDTagRPC(const MbIDTagRPC& ToBeCopied):MbIDTag(ToBeCopied)
{ 
  CopyIds(&ToBeCopied) ;
}
MbIDTagRPC& MbIDTagRPC::operator=( const MbIDTagRPC& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
    MbIDTag::operator=(ToBeCopied);
    CopyIds(&ToBeCopied) ;
  }
  return *this; 
}

// Set All

void MbIDTagRPC::SetSGDoubletR    ( const int& SGDoubletR    ) { m_SGDoubletR    = SGDoubletR    ; }
void MbIDTagRPC::SetSGDoubletZ    ( const int& SGDoubletZ    ) { m_SGDoubletZ    = SGDoubletZ    ; }
void MbIDTagRPC::SetSGDoubletPhi  ( const int& SGDoubletPhi  ) { m_SGDoubletPhi  = SGDoubletPhi  ; }
void MbIDTagRPC::SetSGGasGap      ( const int& SGGasGap      ) { m_SGGasGap      = SGGasGap      ; }
void MbIDTagRPC::SetSGMeasuresPhi ( const int& SGMeasuresPhi ) { m_SGMeasuresPhi = SGMeasuresPhi ; }
void MbIDTagRPC::SetSGStrip       ( const int& SGStrip       ) { m_SGStrip       = SGStrip       ; }
void MbIDTagRPC::SetJSPLI         ( const int& JSPLI         ) { m_JSPLI         = JSPLI         ; }
void MbIDTagRPC::SetJSL           ( const int& JSL           ) { m_JSL           = JSL           ; }
void MbIDTagRPC::SetJSZ           ( const int& JSZ           ) { m_JSZ           = JSZ           ; }
void MbIDTagRPC::SetJSTRI         ( const int& JSTRI         ) { m_JSTRI         = JSTRI         ; }

// Get All

int MbIDTagRPC::GetSGDoubletR    () const { return m_SGDoubletR    ; }
int MbIDTagRPC::GetSGDoubletZ    () const { return m_SGDoubletZ    ; }
int MbIDTagRPC::GetSGDoubletPhi  () const { return m_SGDoubletPhi  ; }
int MbIDTagRPC::GetSGGasGap      () const { return m_SGGasGap      ; }
int MbIDTagRPC::GetSGMeasuresPhi () const { return m_SGMeasuresPhi ; }
int MbIDTagRPC::GetSGStrip       () const { return m_SGStrip       ; }
int MbIDTagRPC::GetJSPLI         () const { return m_JSPLI         ; }
int MbIDTagRPC::GetJSL           () const { return m_JSL           ; }
int MbIDTagRPC::GetJSZ           () const { return m_JSZ           ; }
int MbIDTagRPC::GetJSTRI         () const { return m_JSTRI         ; }

void MbIDTagRPC::PrintAmdIds(std::ostream* out) const{

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
void MbIDTagRPC::PrintAthIds(std::ostream* out) const{

    *out
                    << "RPC "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta 
                    << std::setw(4)<< m_SGStationPhi 
                    << std::setw(4)<< m_SGDoubletR   
                    << std::setw(4)<< m_SGDoubletZ   
                    << std::setw(4)<< m_SGDoubletPhi 
                    << std::setw(4)<< m_SGGasGap     
                    << std::setw(4)<< m_SGMeasuresPhi
                    << std::setw(4)<< m_SGStrip;

}

void MbIDTagRPC::CopyIds(const MbIDTagRPC* pMbIDTag){
CopyBaseIds(pMbIDTag);
SetSGDoubletR    ( pMbIDTag->GetSGDoubletR    () );
SetSGDoubletZ    ( pMbIDTag->GetSGDoubletZ    () );
SetSGDoubletPhi  ( pMbIDTag->GetSGDoubletPhi  () );
SetSGGasGap      ( pMbIDTag->GetSGGasGap      () );
SetSGMeasuresPhi ( pMbIDTag->GetSGMeasuresPhi () );
SetSGStrip       ( pMbIDTag->GetSGStrip       () );
SetJSPLI         ( pMbIDTag->GetJSPLI         () );
SetJSL           ( pMbIDTag->GetJSL           () );
SetJSZ           ( pMbIDTag->GetJSZ           () );
SetJSTRI         ( pMbIDTag->GetJSTRI         () );
}
