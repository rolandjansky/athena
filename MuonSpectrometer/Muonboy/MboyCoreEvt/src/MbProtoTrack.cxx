/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbProtoTrack.h"

MbProtoTrack::MbProtoTrack()
{
  m_MboyStatus = -1  ;

  m_Xpoint  = 0 ;
  m_Ypoint  = 0 ;
  m_Zpoint  = 0 ;

  m_Xdir    = 0 ;
  m_Ydir    = 0 ;
  m_Zdir    = 0 ;

  m_Mome    = 0 ;
  m_Charge  = 0 ;
 
  m_ISC0 = 0  ;

  m_DZT1 = 0. ;
  m_DS1  = 0. ;
  m_DZT2 = 0. ;
  m_DS2  = 0. ;
  
  m_SMU  = 0. ;

  m_CFI  = 0. ;
  m_SFI  = 0. ;
  m_CFA  = 0. ;
  m_SFA  = 0. ;

  m_Z1   = 0. ;
  m_T1   = 0. ;
  m_S1   = 0. ;

  m_Z2   = 0. ;
  m_T2   = 0. ;
  m_S2   = 0. ;

  m_Own_m_VectorOfMbDRAT   = 0 ;

  m_VectorOfMbDRAT.push_back( 0 );
  m_VectorOfMbDRAT.clear();
  
  m_VectorOfMbDRATTRAKDO.push_back( 0 );
  m_VectorOfMbDRATTRAKDO.clear();
  
}
MbProtoTrack::~MbProtoTrack(){
  std::vector< MbDRAT* >::const_iterator it       = m_VectorOfMbDRATTRAKDO.begin() ; 
  std::vector< MbDRAT* >::const_iterator it_end   = m_VectorOfMbDRATTRAKDO.end()   ;
  for(; it!=it_end;++it ){ delete (*it) ; }
  m_VectorOfMbDRATTRAKDO.clear();
  if ( m_Own_m_VectorOfMbDRAT == 1 ){
    std::vector< MbDRAT* >::const_iterator it       = m_VectorOfMbDRAT.begin() ; 
    std::vector< MbDRAT* >::const_iterator it_end   = m_VectorOfMbDRAT.end()   ;
    for(; it!=it_end;++it ){ delete (*it) ; }
  }else{
    m_VectorOfMbDRAT.clear();
  }
}
MbProtoTrack::MbProtoTrack(const MbProtoTrack& ToBeCopied)
{
  m_MboyStatus = ToBeCopied.m_MboyStatus ;

  m_Xpoint  = ToBeCopied.m_Xpoint  ;
  m_Ypoint  = ToBeCopied.m_Ypoint  ;
  m_Zpoint  = ToBeCopied.m_Zpoint  ;

  m_Xdir    = ToBeCopied.m_Xdir    ;
  m_Ydir    = ToBeCopied.m_Ydir    ;
  m_Zdir    = ToBeCopied.m_Zdir    ;

  m_Mome    = ToBeCopied.m_Mome    ;
  m_Charge  = ToBeCopied.m_Charge  ;

  m_ISC0 = ToBeCopied.m_ISC0 ;

  m_DZT1 = ToBeCopied.m_DZT1 ;
  m_DS1  = ToBeCopied.m_DS1  ;
  m_DZT2 = ToBeCopied.m_DZT2 ;
  m_DS2  = ToBeCopied.m_DS2  ;
  
  m_SMU  = ToBeCopied.m_SMU  ;

  m_CFI  = ToBeCopied.m_CFI  ;
  m_SFI  = ToBeCopied.m_SFI  ;
  m_CFA  = ToBeCopied.m_CFA  ;
  m_SFA  = ToBeCopied.m_SFA  ;

  m_Z1   = ToBeCopied.m_Z1   ;
  m_T1   = ToBeCopied.m_T1   ;
  m_S1   = ToBeCopied.m_S1   ;

  m_Z2   = ToBeCopied.m_Z2   ;
  m_T2   = ToBeCopied.m_T2   ;
  m_S2   = ToBeCopied.m_S2   ;

  m_Own_m_VectorOfMbDRAT   = 0   ;

  m_VectorOfMbDRAT.push_back( 0 );
  m_VectorOfMbDRAT.clear(); 

  std::vector< MbDRAT* >::const_iterator it       = ToBeCopied.m_VectorOfMbDRAT.begin() ; 
  std::vector< MbDRAT* >::const_iterator it_end   = ToBeCopied.m_VectorOfMbDRAT.end()   ;
  for(; it!=it_end;++it ){ m_VectorOfMbDRAT.push_back( (*it) ); }

  m_VectorOfMbDRATTRAKDO.push_back( 0 );
  m_VectorOfMbDRATTRAKDO.clear(); 

  it       = ToBeCopied.m_VectorOfMbDRATTRAKDO.begin() ; 
  it_end   = ToBeCopied.m_VectorOfMbDRATTRAKDO.end()   ;
  for(; it!=it_end;++it ){ 
    MbDRAT* pMbDRAT = new MbDRAT( *(*it) );
    m_VectorOfMbDRATTRAKDO.push_back( pMbDRAT ); 
  }

}
MbProtoTrack& MbProtoTrack::operator=( const MbProtoTrack& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
    m_MboyStatus = ToBeCopied.m_MboyStatus ;

    m_Xpoint  = ToBeCopied.m_Xpoint  ;
    m_Ypoint  = ToBeCopied.m_Ypoint  ;
    m_Zpoint  = ToBeCopied.m_Zpoint  ;

    m_Xdir    = ToBeCopied.m_Xdir    ;
    m_Ydir    = ToBeCopied.m_Ydir    ;
    m_Zdir    = ToBeCopied.m_Zdir    ;

    m_Mome    = ToBeCopied.m_Mome    ;
    m_Charge  = ToBeCopied.m_Charge  ;

    m_ISC0 = ToBeCopied.m_ISC0 ;

    m_DZT1 = ToBeCopied.m_DZT1 ;
    m_DS1  = ToBeCopied.m_DS1  ;
    m_DZT2 = ToBeCopied.m_DZT2 ;
    m_DS2  = ToBeCopied.m_DS2  ;
    
    m_SMU  = ToBeCopied.m_SMU  ;

    m_CFI  = ToBeCopied.m_CFI  ;
    m_SFI  = ToBeCopied.m_SFI  ;
    m_CFA  = ToBeCopied.m_CFA  ;
    m_SFA  = ToBeCopied.m_SFA  ;

    m_Z1   = ToBeCopied.m_Z1   ;
    m_T1   = ToBeCopied.m_T1   ;
    m_S1   = ToBeCopied.m_S1   ;

    m_Z2   = ToBeCopied.m_Z2   ;
    m_T2   = ToBeCopied.m_T2   ;
    m_S2   = ToBeCopied.m_S2   ;

    m_Own_m_VectorOfMbDRAT   = 0   ;

    m_VectorOfMbDRAT.push_back( 0 );
    m_VectorOfMbDRAT.clear(); 

    std::vector< MbDRAT* >::const_iterator it       = ToBeCopied.m_VectorOfMbDRAT.begin() ; 
    std::vector< MbDRAT* >::const_iterator it_end   = ToBeCopied.m_VectorOfMbDRAT.end()   ;
    for(; it!=it_end;++it ){ m_VectorOfMbDRAT.push_back( (*it) ); }

    m_VectorOfMbDRATTRAKDO.push_back( 0 );
    m_VectorOfMbDRATTRAKDO.clear(); 

    it       = ToBeCopied.m_VectorOfMbDRATTRAKDO.begin() ; 
    it_end   = ToBeCopied.m_VectorOfMbDRATTRAKDO.end()   ;
    for(; it!=it_end;++it ){ 
      MbDRAT* pMbDRAT = new MbDRAT( *(*it) );
      m_VectorOfMbDRATTRAKDO.push_back( pMbDRAT ); 
    }

  }
  return *this; 
}
