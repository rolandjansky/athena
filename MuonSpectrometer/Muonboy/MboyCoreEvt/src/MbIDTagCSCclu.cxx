/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIDTagCSCclu.h"

MbIDTagCSCclu::MbIDTagCSCclu():MbIDTag()
{

 m_SGChamberLayer  = 0     ;
 m_SGWireLayer     = 0     ;
 m_SGMeasuresPhi   = 0     ;
 m_SGStrip         = 0     ;

 m_JSL             = 0     ;
 m_JSTRI           = 0     ;
 m_SZFLAG          = 0.    ;
 
}
MbIDTagCSCclu::~MbIDTagCSCclu(){}
MbIDTagCSCclu::MbIDTagCSCclu(const MbIDTagCSCclu& ToBeCopied):MbIDTag(ToBeCopied) 
{ 
  CopyIds(&ToBeCopied) ;
}
MbIDTagCSCclu& MbIDTagCSCclu::operator=( const MbIDTagCSCclu& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
    MbIDTag::operator=(ToBeCopied);
    CopyIds(&ToBeCopied) ;
  }
  return *this; 
}

// Set All

void MbIDTagCSCclu::SetSGChamberLayer ( const int&    SGChamberLayer ) { m_SGChamberLayer = SGChamberLayer ; }
void MbIDTagCSCclu::SetSGWireLayer    ( const int&    SGWireLayer    ) { m_SGWireLayer    = SGWireLayer    ; }
void MbIDTagCSCclu::SetSGMeasuresPhi  ( const int&    SGMeasuresPhi  ) { m_SGMeasuresPhi  = SGMeasuresPhi  ; }
void MbIDTagCSCclu::SetSGStrip        ( const int&    SGStrip        ) { m_SGStrip        = SGStrip        ; }
void MbIDTagCSCclu::SetJSL 	      ( const int&    JSL            ) { m_JSL            = JSL            ; }
void MbIDTagCSCclu::SetJSTRI	      ( const int&    JSTRI          ) { m_JSTRI          = JSTRI          ; }
void MbIDTagCSCclu::SetSZFLAG	      ( const double& SZFLAG         ) { m_SZFLAG         = SZFLAG         ; }

// Get All

int    MbIDTagCSCclu::GetSGChamberLayer () const { return m_SGChamberLayer ; }
int    MbIDTagCSCclu::GetSGWireLayer    () const { return m_SGWireLayer    ; }
int    MbIDTagCSCclu::GetSGMeasuresPhi  () const { return m_SGMeasuresPhi  ; }
int    MbIDTagCSCclu::GetSGStrip        () const { return m_SGStrip        ; }
int    MbIDTagCSCclu::GetJSL            () const { return m_JSL            ; }
int    MbIDTagCSCclu::GetJSTRI          () const { return m_JSTRI          ; }
double MbIDTagCSCclu::GetSZFLAG         () const { return m_SZFLAG         ; }

void MbIDTagCSCclu::PrintAmdIds(std::ostream* out) const{

    size_t save_prec = out->precision();

    *out
                    << "CLU "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_JFF
                    << std::setw(4)<< m_JZZ
                    << std::setw(4)<< m_JOB
                    << std::setw(4)<< m_JSL
                    << std::setw(4)<< m_JSTRI
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_SZFLAG ;

    out->precision(save_prec);

}
void MbIDTagCSCclu::PrintAthIds(std::ostream* out) const{

    *out
                    << "CLU "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta  
                    << std::setw(4)<< m_SGStationPhi  
                    << std::setw(4)<< m_SGChamberLayer
                    << std::setw(4)<< m_SGWireLayer   
                    << std::setw(4)<< m_SGMeasuresPhi 
                    << std::setw(4)<< m_SGStrip;

}

void MbIDTagCSCclu::CopyIds(const MbIDTagCSCclu* pMbIDTag){
CopyBaseIds(pMbIDTag);
SetSGChamberLayer ( pMbIDTag->GetSGChamberLayer () );
SetSGWireLayer    ( pMbIDTag->GetSGWireLayer    () );
SetSGMeasuresPhi  ( pMbIDTag->GetSGMeasuresPhi  () );
SetSGStrip        ( pMbIDTag->GetSGStrip        () );
SetJSL            ( pMbIDTag->GetJSL            () );
SetJSTRI          ( pMbIDTag->GetJSTRI          () );
SetSZFLAG         ( pMbIDTag->GetSZFLAG         () );
}
