/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbDRAT_H
#define MbDRAT_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"
#include "MboyCoreEvt/MbIDTagRPC.h"
#include "MboyCoreEvt/MbIDTagTGC.h"

  /**
   @class MbDRAT

  @author samusog@cern.ch
  
  */

class MbDRAT {
public:
    MbDRAT();
    virtual ~MbDRAT();
    MbDRAT(const MbDRAT& ToBeCopied);
    MbDRAT& operator=( const MbDRAT& ToBeCopied );


  double m_Xpoint ;
  double m_Ypoint ;
  double m_Zpoint ;

  double m_Xdir ;
  double m_Ydir ;
  double m_Zdir ;

  double m_Mome ;
  double m_Charge ;

//  MbDRAT does own the MbIDTag objects
  std::vector< MbIDTagMDT*    > m_VectorOfMbIDTagMDT    ;
  std::vector< int > m_VectorOfStatusMDT    ;

  std::vector< MbIDTagCSCclu* > m_VectorOfMbIDTagCSCclu ;
  std::vector< int > m_VectorOfStatusCSCclu ;

  std::vector< MbIDTagRPC*    > m_VectorOfMbIDTagRPC    ;

  std::vector< MbIDTagTGC*    > m_VectorOfMbIDTagTGC    ;

  int    m_kounter    ;

  int    m_ISC0    ;
  double m_CCDRAT  ;
  double m_FFDRAT  ;
  double m_GGDRAT  ;
  double m_EZDRAT  ;
  int    m_JFDRAT  ;
  
  int    m_IWDRAT1 ;
  double m_ZZDRAT1 ;
  double m_TTDRAT1 ;
  double m_SSDRAT1 ;
  
  int    m_IWDRAT2 ;
  double m_ZZDRAT2 ;
  double m_TTDRAT2 ;
  double m_SSDRAT2 ;
  
  int    m_STRICT  ;
 
  int    m_Ident   ;
 
};

#endif
