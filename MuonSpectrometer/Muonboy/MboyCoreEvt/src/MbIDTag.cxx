/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIDTag.h"

MbIDTag::MbIDTag(){ 

 std::string astring = "XYZ" ;
 SetSGStationName(astring);

 m_AreAmdbIds = 0 ;

 m_SGStationNber   = 0  ; 

 m_SGStationEta    = 0  ; 
 m_SGStationPhi    = 0  ;

 m_JTYP            = 0  ;

 m_JFF             = 0  ;
 m_JZZ             = 0  ;
 m_JOB             = 0  ;
 
 m_Identifier      = -1 ;

}

MbIDTag::~MbIDTag(){}
MbIDTag::MbIDTag(const MbIDTag& ToBeCopied) { CopyBaseIds(&ToBeCopied) ;}
MbIDTag& MbIDTag::operator=( const MbIDTag& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
    CopyBaseIds(&ToBeCopied) ;
  }
  return *this; 
}

void MbIDTag::SetAreAmdbIds    ( const int& AreAmdbIds            ) { m_AreAmdbIds = AreAmdbIds ; }
void MbIDTag::SetStationName   ( const std::string& StationName   ) { SetSGStationName(StationName); }
void MbIDTag::SetSGStationNber ( const int& SGStationNber         ) { m_SGStationNber  = SGStationNber ; }
void MbIDTag::SetSGStationEta  ( const int& SGStationEta          ) { m_SGStationEta   = SGStationEta  ; }
void MbIDTag::SetSGStationPhi  ( const int& SGStationPhi          ) { m_SGStationPhi   = SGStationPhi  ; }
void MbIDTag::SetJTYP          ( const int& JTYP                  ) { m_JTYP = JTYP ;}
void MbIDTag::SetJFF           ( const int& JFF                   ) { m_JFF  = JFF  ;}
void MbIDTag::SetJZZ           ( const int& JZZ                   ) { m_JZZ  = JZZ  ;}
void MbIDTag::SetJOB           ( const int& JOB                   ) { m_JOB  = JOB  ;}

int         MbIDTag::GetAreAmdbIds    () const { return m_AreAmdbIds    ; }
std::string MbIDTag::GetStationName   () const { return m_StationName   ; }
int	    MbIDTag::GetSGStationNber () const { return m_SGStationNber ; }
int	    MbIDTag::GetSGStationEta  () const { return m_SGStationEta  ; }
int	    MbIDTag::GetSGStationPhi  () const { return m_SGStationPhi  ; }
int	    MbIDTag::GetJTYP          () const { return m_JTYP	        ; } 
int	    MbIDTag::GetJFF           () const { return m_JFF	        ; }
int	    MbIDTag::GetJZZ           () const { return m_JZZ	        ; }
int	    MbIDTag::GetJOB           () const { return m_JOB	        ; }




void MbIDTag::GetStationNameChar(char* NameI) const {
                                                     NameI[0]=m_StationNameChar[0];
                                                     NameI[1]=m_StationNameChar[1];
                                                     NameI[2]=m_StationNameChar[2];
					            }

std::string MbIDTag::GetSGStationName () const { return m_StationName   ; }

void MbIDTag::SetSGStationName ( const std::string& SGStationName ) {
                                                                     m_StationName  = SGStationName ;
                                                                     m_StationNameChar[0]= m_StationName[0] ;
                                                                     m_StationNameChar[1]= m_StationName[1] ;
                                                                     m_StationNameChar[2]= m_StationName[2] ;
								    }
void MbIDTag::SetIdentifier (int identifier) { m_Identifier = identifier; }

int  MbIDTag::GetIdentifier () const { return m_Identifier; }

void MbIDTag::CopyBaseIds(const MbIDTag* pMbIDTag){
SetAreAmdbIds    ( pMbIDTag->GetAreAmdbIds    () );
SetStationName   ( pMbIDTag->GetStationName   () );
SetSGStationNber ( pMbIDTag->GetSGStationNber () );
SetSGStationEta  ( pMbIDTag->GetSGStationEta  () );
SetSGStationPhi  ( pMbIDTag->GetSGStationPhi  () );
SetJTYP          ( pMbIDTag->GetJTYP          () );
SetJFF           ( pMbIDTag->GetJFF           () );
SetJZZ           ( pMbIDTag->GetJZZ           () );
SetJOB           ( pMbIDTag->GetJOB           () );
SetIdentifier    ( pMbIDTag->GetIdentifier    () );
}
