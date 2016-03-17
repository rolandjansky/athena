//  JetSimpleCone.h

#ifndef __TopoCore__JetSimpleCone__
#define __TopoCore__JetSimpleCone__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class JetSimpleCone : public DecisionAlg {
   public:
      JetSimpleCone(const std::string & name);
      virtual ~JetSimpleCone();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:
      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_R = { 10 };
      parType_t      p_MinET = { 0 };
      parType_t      p_EtaMin = { 0 };
      parType_t      p_EtaMax = { 49 };
      parType_t      p_SimpleCone[6] = { 0 , 0,0,0,0,0 };

      TH1 * m_histAcceptSimpleCone[6] = {};
      TH1 * m_histRejectSimpleCone[6] = {};

   };
   
}

#endif
