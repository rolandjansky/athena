/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaRSqrIncl1.h
//  TopoCore
//  Created by Joerg Stelzer/V Sorin on 9/16/14.

#ifndef __TopoCore__DeltaRSqrIncl1__
#define __TopoCore__DeltaRSqrIncl1__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class DeltaRSqrIncl1 : public DecisionAlg {
   public:
      DeltaRSqrIncl1(const std::string & name);
      virtual ~DeltaRSqrIncl1();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_DeltaRMin[3] = {0, 0, 0};
      parType_t      p_DeltaRMax[3] = {0, 0, 0};
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
      parType_t      p_OneBarrel = { 0 };
     
      TH1 * m_histAccept[3] = {};
      TH1 * m_histReject[3] = {};
   };
   
}

#endif
