/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIDTagCSC.h"

MbIDTagCSC::MbIDTagCSC():MbIDTag()
{
 
 m_SGChamberLayer  = 0     ;
 m_SGWireLayer     = 0     ;
 m_SGMeasuresPhi   = 0     ;
 m_SGStrip         = 0     ;

 m_JSL             = 0     ;
 m_JSTRI           = 0     ;
 m_SZFLAG          = 0.    ;

}
MbIDTagCSC::~MbIDTagCSC(){}
MbIDTagCSC::MbIDTagCSC(const MbIDTagCSC& ToBeCopied):MbIDTag(ToBeCopied) 
{ 
  CopyIds(&ToBeCopied) ;
}
MbIDTagCSC& MbIDTagCSC::operator=( const MbIDTagCSC& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
    MbIDTag::operator=(ToBeCopied);
    CopyIds(&ToBeCopied) ;
  }
  return *this; 
}

// Set All

void MbIDTagCSC::SetSGChamberLayer ( const int&    SGChamberLayer ) { m_SGChamberLayer = SGChamberLayer ; }
void MbIDTagCSC::SetSGWireLayer    ( const int&    SGWireLayer    ) { m_SGWireLayer    = SGWireLayer    ; }
void MbIDTagCSC::SetSGMeasuresPhi  ( const int&    SGMeasuresPhi  ) { m_SGMeasuresPhi  = SGMeasuresPhi  ; }
void MbIDTagCSC::SetSGStrip	   ( const int&    SGStrip        ) { m_SGStrip        = SGStrip        ; }    
void MbIDTagCSC::SetJSL            ( const int&    JSL            ) { m_JSL             = JSL           ; }
void MbIDTagCSC::SetJSTRI          ( const int&    JSTRI          ) { m_JSTRI           = JSTRI         ; }
void MbIDTagCSC::SetSZFLAG         ( const double& SZFLAG         ) { m_SZFLAG          = SZFLAG        ; }

// Get All
       
int    MbIDTagCSC::GetSGChamberLayer () const { return m_SGChamberLayer ; }
int    MbIDTagCSC::GetSGWireLayer    () const { return m_SGWireLayer    ; }
int    MbIDTagCSC::GetSGMeasuresPhi  () const { return m_SGMeasuresPhi  ; }
int    MbIDTagCSC::GetSGStrip        () const { return m_SGStrip        ; }
int    MbIDTagCSC::GetJSL            () const { return m_JSL            ; }
int    MbIDTagCSC::GetJSTRI          () const { return m_JSTRI          ; }
double MbIDTagCSC::GetSZFLAG         () const { return m_SZFLAG         ; }

void MbIDTagCSC::PrintAmdIds(std::ostream* out) const{

    size_t save_prec = out->precision();

    *out
                    << "CSC "
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
void MbIDTagCSC::PrintAthIds(std::ostream* out) const{

    *out
                    << "CSC "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta  
                    << std::setw(4)<< m_SGStationPhi  
                    << std::setw(4)<< m_SGChamberLayer
                    << std::setw(4)<< m_SGWireLayer   
                    << std::setw(4)<< m_SGMeasuresPhi 
                    << std::setw(4)<< m_SGStrip;

}

void MbIDTagCSC::CopyIds(const MbIDTagCSC* pMbIDTag){
CopyBaseIds(pMbIDTag);
SetSGChamberLayer ( pMbIDTag->GetSGChamberLayer () );
SetSGWireLayer    ( pMbIDTag->GetSGWireLayer    () );
SetSGMeasuresPhi  ( pMbIDTag->GetSGMeasuresPhi  () );
SetSGStrip        ( pMbIDTag->GetSGStrip        () );
SetJSL            ( pMbIDTag->GetJSL            () );
SetJSTRI          ( pMbIDTag->GetJSTRI          () );
SetSZFLAG         ( pMbIDTag->GetSZFLAG         () );
}
