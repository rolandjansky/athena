/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  NotMatch.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__NotMatch__
#define __TopoCore__NotMatch__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class NotMatch : public DecisionAlg {
   public:
      NotMatch(const std::string & name);
      virtual ~NotMatch();

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
      parType_t      p_MinET1[2]  = { 0,0 };
      parType_t      p_EtaMin1[2]  = { 0,0 };
      parType_t      p_EtaMax1[2]  = { 49,49 };
      parType_t      p_MinET2[2]  = { 0,0 };
      parType_t      p_EtaMin2[2]  = { 0,0 };
      parType_t      p_EtaMax2[2]  = { 49,49 };
      parType_t      p_DRCut[2] = { 0,0 };

   };
   
}

#endif
