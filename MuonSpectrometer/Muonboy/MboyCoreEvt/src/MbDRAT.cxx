/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbDRAT.h"

MbDRAT::MbDRAT(){

  m_Xpoint  = 0 ;
  m_Ypoint  = 0 ;
  m_Zpoint  = 0 ;

  m_Xdir    = 0 ;
  m_Ydir    = 0 ;
  m_Zdir    = 0 ;

  m_Mome    = 0 ;
  m_Charge  = 0 ;
 
  m_kounter = 0 ;

  m_ISC0    = 0 ;
  m_CCDRAT  = 0 ;
  m_FFDRAT  = 0 ;
  m_GGDRAT  = 0 ;
  m_EZDRAT  = 0 ;
  m_JFDRAT  = 0 ;
 
  m_IWDRAT1 = 0 ;
  m_ZZDRAT1 = 0 ;
  m_TTDRAT1 = 0 ;
  m_SSDRAT1 = 0 ;
 
  m_IWDRAT2 = 0 ;
  m_ZZDRAT2 = 0 ;
  m_TTDRAT2 = 0 ;
  m_SSDRAT2 = 0 ;
 
  m_STRICT  = 0 ;

  m_Ident   =-1 ;

  m_VectorOfMbIDTagMDT.push_back( 0 );
  m_VectorOfMbIDTagCSCclu.push_back( 0 );
  m_VectorOfMbIDTagRPC.push_back( 0 );
  m_VectorOfMbIDTagTGC.push_back( 0 );

  m_VectorOfStatusMDT.push_back( 0 );
  m_VectorOfStatusCSCclu.push_back( 0 );
  
  m_VectorOfMbIDTagMDT.clear() ;
  m_VectorOfMbIDTagCSCclu.clear() ;
  m_VectorOfMbIDTagRPC.clear() ;
  m_VectorOfMbIDTagTGC.clear() ;

  m_VectorOfStatusMDT.clear() ;
  m_VectorOfStatusCSCclu.clear() ;

}
MbDRAT::~MbDRAT(){
  std::vector< MbIDTagMDT* >::iterator itMDT       = m_VectorOfMbIDTagMDT.begin() ; 
  std::vector< MbIDTagMDT* >::iterator itMDT_end   = m_VectorOfMbIDTagMDT.end()   ;
  for(; itMDT!=itMDT_end;++itMDT ){ delete (*itMDT) ; }
  
  std::vector< MbIDTagCSCclu* >::iterator itCSCclu       = m_VectorOfMbIDTagCSCclu.begin() ; 
  std::vector< MbIDTagCSCclu* >::iterator itCSCclu_end   = m_VectorOfMbIDTagCSCclu.end()   ;
  for(; itCSCclu!=itCSCclu_end;++itCSCclu ){ delete (*itCSCclu) ; }

  std::vector< MbIDTagRPC* >::iterator itRPC       = m_VectorOfMbIDTagRPC.begin() ; 
  std::vector< MbIDTagRPC* >::iterator itRPC_end   = m_VectorOfMbIDTagRPC.end()   ;
  for(; itRPC!=itRPC_end;++itRPC ){ delete (*itRPC) ; }

  std::vector< MbIDTagTGC* >::iterator itTGC       = m_VectorOfMbIDTagTGC.begin() ; 
  std::vector< MbIDTagTGC* >::iterator itTGC_end   = m_VectorOfMbIDTagTGC.end()   ;
  for(; itTGC!=itTGC_end;++itTGC ){ delete (*itTGC) ; }
  
}
MbDRAT::MbDRAT(const MbDRAT& ToBeCopied)
{
  m_Xpoint  = ToBeCopied.m_Xpoint  ;
  m_Ypoint  = ToBeCopied.m_Ypoint  ;
  m_Zpoint  = ToBeCopied.m_Zpoint  ;

  m_Xdir    = ToBeCopied.m_Xdir    ;
  m_Ydir    = ToBeCopied.m_Ydir    ;
  m_Zdir    = ToBeCopied.m_Zdir    ;

  m_Mome    = ToBeCopied.m_Mome    ;
  m_Charge  = ToBeCopied.m_Charge  ;

  m_kounter = ToBeCopied.m_kounter ;

  m_ISC0    = ToBeCopied.m_ISC0    ;
  m_CCDRAT  = ToBeCopied.m_CCDRAT  ;
  m_FFDRAT  = ToBeCopied.m_FFDRAT  ;
  m_GGDRAT  = ToBeCopied.m_GGDRAT  ;
  m_EZDRAT  = ToBeCopied.m_EZDRAT  ;
  m_JFDRAT  = ToBeCopied.m_JFDRAT  ;
 
  m_IWDRAT1 = ToBeCopied.m_IWDRAT1 ;
  m_ZZDRAT1 = ToBeCopied.m_ZZDRAT1 ;
  m_TTDRAT1 = ToBeCopied.m_TTDRAT1 ;
  m_SSDRAT1 = ToBeCopied.m_SSDRAT1 ;
 
  m_IWDRAT2 = ToBeCopied.m_IWDRAT2 ;
  m_ZZDRAT2 = ToBeCopied.m_ZZDRAT2 ;
  m_TTDRAT2 = ToBeCopied.m_TTDRAT2 ;
  m_SSDRAT2 = ToBeCopied.m_SSDRAT2 ;
 
  m_STRICT  = ToBeCopied.m_STRICT  ;
  
  m_Ident   = ToBeCopied.m_Ident   ;
  
  m_VectorOfMbIDTagMDT.push_back( 0 );
  m_VectorOfMbIDTagCSCclu.push_back( 0 );
  m_VectorOfMbIDTagRPC.push_back( 0 );
  m_VectorOfMbIDTagTGC.push_back( 0 );

  m_VectorOfStatusMDT.push_back( 0 );
  m_VectorOfStatusCSCclu.push_back( 0 );
  
  m_VectorOfMbIDTagMDT.clear() ;
  m_VectorOfMbIDTagCSCclu.clear() ;
  m_VectorOfMbIDTagRPC.clear() ;
  m_VectorOfMbIDTagTGC.clear() ;

  m_VectorOfStatusMDT.clear() ;
  m_VectorOfStatusCSCclu.clear() ;
  m_VectorOfStatusCSCclu.clear() ;

  std::vector< MbIDTagMDT* >::const_iterator itMDT       = ToBeCopied.m_VectorOfMbIDTagMDT.begin() ; 
  std::vector< MbIDTagMDT* >::const_iterator itMDT_end   = ToBeCopied.m_VectorOfMbIDTagMDT.end()   ;
  std::vector< int         >::const_iterator itStatusMDT = ToBeCopied.m_VectorOfStatusMDT.begin() ; 
  for(; itMDT!=itMDT_end;++itMDT, ++itStatusMDT){ 
      MbIDTagMDT* pMbIDTagLocal = new MbIDTagMDT( *(*itMDT) ) ;   
      m_VectorOfMbIDTagMDT.push_back( pMbIDTagLocal );
      m_VectorOfStatusMDT.push_back( *(itStatusMDT) );
  }

  std::vector< MbIDTagCSCclu* >::const_iterator itCSCclu       = ToBeCopied.m_VectorOfMbIDTagCSCclu.begin() ; 
  std::vector< MbIDTagCSCclu* >::const_iterator itCSCclu_end   = ToBeCopied.m_VectorOfMbIDTagCSCclu.end()   ;
  std::vector< int            >::const_iterator itStatusCSCclu = ToBeCopied.m_VectorOfStatusCSCclu.begin() ; 
  for(; itCSCclu!=itCSCclu_end;++itCSCclu, ++itStatusCSCclu){ 
      MbIDTagCSCclu* pMbIDTagLocal = new MbIDTagCSCclu( *(*itCSCclu) ) ;  
      m_VectorOfMbIDTagCSCclu.push_back( pMbIDTagLocal );
      m_VectorOfStatusCSCclu.push_back( *(itStatusCSCclu) );
  }

  std::vector< MbIDTagRPC* >::const_iterator itRPC       = ToBeCopied.m_VectorOfMbIDTagRPC.begin() ; 
  std::vector< MbIDTagRPC* >::const_iterator itRPC_end   = ToBeCopied.m_VectorOfMbIDTagRPC.end()   ;
  for(; itRPC!=itRPC_end;++itRPC){ 
      MbIDTagRPC* pMbIDTagLocal = new MbIDTagRPC( *(*itRPC) ) ;   
      m_VectorOfMbIDTagRPC.push_back( pMbIDTagLocal );
  }

  std::vector< MbIDTagTGC* >::const_iterator itTGC       = ToBeCopied.m_VectorOfMbIDTagTGC.begin() ; 
  std::vector< MbIDTagTGC* >::const_iterator itTGC_end   = ToBeCopied.m_VectorOfMbIDTagTGC.end()   ;
  for(; itTGC!=itTGC_end;++itTGC){ 
      MbIDTagTGC* pMbIDTagLocal = new MbIDTagTGC( *(*itTGC) ) ;   
      m_VectorOfMbIDTagTGC.push_back( pMbIDTagLocal );
  }

}
MbDRAT& MbDRAT::operator=( const MbDRAT& ToBeCopied ) 
{ 
  if( &ToBeCopied != this ){
  
    m_Xpoint  = ToBeCopied.m_Xpoint  ;
    m_Ypoint  = ToBeCopied.m_Ypoint  ;
    m_Zpoint  = ToBeCopied.m_Zpoint  ;

    m_Xdir    = ToBeCopied.m_Xdir    ;
    m_Ydir    = ToBeCopied.m_Ydir    ;
    m_Zdir    = ToBeCopied.m_Zdir    ;
    
    m_Mome    = ToBeCopied.m_Mome    ;
    m_Charge  = ToBeCopied.m_Charge  ;

    m_kounter = ToBeCopied.m_kounter ;

    m_ISC0    = ToBeCopied.m_ISC0    ;
    m_CCDRAT  = ToBeCopied.m_CCDRAT  ;
    m_FFDRAT  = ToBeCopied.m_FFDRAT  ;
    m_GGDRAT  = ToBeCopied.m_GGDRAT  ;
    m_EZDRAT  = ToBeCopied.m_EZDRAT  ;
    m_JFDRAT  = ToBeCopied.m_JFDRAT  ;
 
    m_IWDRAT1 = ToBeCopied.m_IWDRAT1 ;
    m_ZZDRAT1 = ToBeCopied.m_ZZDRAT1 ;
    m_TTDRAT1 = ToBeCopied.m_TTDRAT1 ;
    m_SSDRAT1 = ToBeCopied.m_SSDRAT1 ;
 
    m_IWDRAT2 = ToBeCopied.m_IWDRAT2 ;
    m_ZZDRAT2 = ToBeCopied.m_ZZDRAT2 ;
    m_TTDRAT2 = ToBeCopied.m_TTDRAT2 ;
    m_SSDRAT2 = ToBeCopied.m_SSDRAT2 ;
 
    m_STRICT  = ToBeCopied.m_STRICT  ;
    
    m_Ident   = ToBeCopied.m_Ident   ;
    
    m_VectorOfMbIDTagMDT.push_back( 0 );
    m_VectorOfMbIDTagCSCclu.push_back( 0 );
    m_VectorOfMbIDTagRPC.push_back( 0 );
    m_VectorOfMbIDTagTGC.push_back( 0 );
  
    m_VectorOfStatusMDT.push_back( 0 );
    m_VectorOfStatusCSCclu.push_back( 0 );
  
    m_VectorOfMbIDTagMDT.clear() ;
    m_VectorOfMbIDTagCSCclu.clear() ;
    m_VectorOfMbIDTagRPC.clear() ;
    m_VectorOfMbIDTagTGC.clear() ;

    m_VectorOfStatusMDT.clear() ;
    m_VectorOfStatusCSCclu.clear() ;

    std::vector< MbIDTagMDT* >::const_iterator itMDT       = ToBeCopied.m_VectorOfMbIDTagMDT.begin() ; 
    std::vector< MbIDTagMDT* >::const_iterator itMDT_end   = ToBeCopied.m_VectorOfMbIDTagMDT.end()   ;
    std::vector< int         >::const_iterator itStatusMDT = ToBeCopied.m_VectorOfStatusMDT.begin() ; 
    for(; itMDT!=itMDT_end;++itMDT, ++itStatusMDT){ 
      MbIDTagMDT* pMbIDTagLocal = new MbIDTagMDT( *(*itMDT) ) ;   
      m_VectorOfMbIDTagMDT.push_back( pMbIDTagLocal );
      m_VectorOfStatusMDT.push_back( *(itStatusMDT) );
    }

    std::vector< MbIDTagCSCclu* >::const_iterator itCSCclu       = ToBeCopied.m_VectorOfMbIDTagCSCclu.begin() ; 
    std::vector< MbIDTagCSCclu* >::const_iterator itCSCclu_end   = ToBeCopied.m_VectorOfMbIDTagCSCclu.end()   ;
    std::vector< int            >::const_iterator itStatusCSCclu = ToBeCopied.m_VectorOfStatusCSCclu.begin() ; 
    for(; itCSCclu!=itCSCclu_end;++itCSCclu, ++itStatusCSCclu){ 
      MbIDTagCSCclu* pMbIDTagLocal = new MbIDTagCSCclu( *(*itCSCclu) ) ;  
      m_VectorOfMbIDTagCSCclu.push_back( pMbIDTagLocal );
      m_VectorOfStatusCSCclu.push_back( *(itStatusCSCclu) );
    }
    
    std::vector< MbIDTagRPC* >::const_iterator itRPC       = ToBeCopied.m_VectorOfMbIDTagRPC.begin() ; 
    std::vector< MbIDTagRPC* >::const_iterator itRPC_end   = ToBeCopied.m_VectorOfMbIDTagRPC.end()   ;
    for(; itRPC!=itRPC_end;++itRPC){ 
        MbIDTagRPC* pMbIDTagLocal = new MbIDTagRPC( *(*itRPC) ) ;   
        m_VectorOfMbIDTagRPC.push_back( pMbIDTagLocal );
    }

    std::vector< MbIDTagTGC* >::const_iterator itTGC       = ToBeCopied.m_VectorOfMbIDTagTGC.begin() ; 
    std::vector< MbIDTagTGC* >::const_iterator itTGC_end   = ToBeCopied.m_VectorOfMbIDTagTGC.end()   ;
    for(; itTGC!=itTGC_end;++itTGC){ 
        MbIDTagTGC* pMbIDTagLocal = new MbIDTagTGC( *(*itTGC) ) ;   
        m_VectorOfMbIDTagTGC.push_back( pMbIDTagLocal );
    }
  }
  return *this; 
}
