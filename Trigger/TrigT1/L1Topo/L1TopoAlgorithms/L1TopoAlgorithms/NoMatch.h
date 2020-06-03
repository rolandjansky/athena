/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  NoMatch.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__NoMatch__
#define __TopoCore__NoMatch__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class NoMatch : public DecisionAlg {
   public:
      NoMatch(const std::string & name);
      virtual ~NoMatch();

      virtual StatusCode initialize();

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:
      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_MinET1 = { 0 };
      parType_t      p_EtaMin1 = { 0 };
      parType_t      p_EtaMax1 = { 49 };
      parType_t      p_MinET2 = { 0 };
      parType_t      p_EtaMin2 = { 0 };
      parType_t      p_EtaMax2 = { 49 };
      parType_t      p_DRCut = { 0 };

   };
   
}

#endif
