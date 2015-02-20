/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef HLTRATES2COOL_HLTCOUNTER
#define HLTRATES2COOL_HLTCOUNTER

#include <string>
#include <iostream>

namespace hltca {
   class HLTCounter;
}

class TAxis;
class TH2;

namespace hltca {
   std::ostream & operator<<(std::ostream &, const HLTCounter &);

   class HLTCounter
   {
   public:
   
      enum TriggerLevel { UNKNOWN=0, BOTH=1, L2=2, EF=3 };

      HLTCounter( unsigned int chainCounter,
                  unsigned int input,
                  unsigned int afterPrescale,
                  unsigned int raw,
                  unsigned int total,
                  TriggerLevel lvl):
         m_chainCounter(chainCounter),
         m_input(input),
         m_afterPS(afterPrescale),
         m_raw(raw),
         m_total(total),
         m_lvl(lvl)
      {};

      HLTCounter( unsigned int chainCounter,
                  TriggerLevel lvl):
         m_chainCounter(chainCounter),
         m_input(0),
         m_afterPS(0),
         m_raw(0),
         m_total(0),
         m_lvl(lvl)
      {};

    
      unsigned int input()          const { return m_input; }
      unsigned int chain_counter()  const { return m_chainCounter; }
      unsigned int after_prescale() const { return m_afterPS; }
      unsigned int raw()            const { return m_raw; }
      unsigned int total()          const { return m_total; }
      TriggerLevel trlevel()        const { return m_lvl; }
      std::string level() const {
         switch (m_lvl) {
         case L2: return "L2"; break;
         case EF: return "EF"; break;
         case BOTH: return "BOTH"; break;
         case UNKNOWN: return "UNKNOWN"; break;
         }
         return "";
      }

      void setContent(const TH2* h, int binx);

      static bool hasBinsSet();
      static bool setBins(TAxis * axis);
  
   private:
      unsigned int m_chainCounter;
      unsigned int m_input;
      unsigned int m_afterPS;
      unsigned int m_raw;
      unsigned int m_total;
      TriggerLevel m_lvl;

      static int g_inputbin;
      static int g_afterPSbin;
      static int g_rawbin;
      static int g_totalbin;

      friend std::ostream & operator<<(std::ostream &, const HLTCounter &);

   };
}

#endif
