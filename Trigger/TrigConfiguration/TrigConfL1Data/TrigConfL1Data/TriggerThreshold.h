// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TriggerThreshold
#define TrigConf_TriggerThreshold

#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/L1DataBaseclass.h"

#include <string>
#include <vector>
#include <iostream>

namespace TrigConf {

   class TriggerThreshold : public L1DataBaseclass {
      
   public:
      TriggerThreshold();
      TriggerThreshold( const TriggerThreshold& thr);
      virtual ~TriggerThreshold();

      static TriggerThresholdValue* createThresholdValue(const std::string& type);

      // accessors
      L1DataDef::TriggerType ttype()                    const { return m_type; }
      const std::string&     type()                     const { return m_SType; }
      bool                   active()                   const { return m_Active; }
      int                    mapping()                  const { return m_Mapping; }
      const std::string&     cableName()                const { return m_CableName; }
      const std::string&     cableCtpin()               const { return m_CableCtpin; }
      const std::string&     cableConnector()           const { return m_CableConnector; }
      int                    cableStart()               const { return m_CableStart; }
      int                    cableEnd()                 const { return m_CableEnd; }
      unsigned int           clock()                    const { return m_Clock; }
      int                    thresholdNumber()          const { return m_ThresholdNumber; }
      int                    numberofValues()           const { return m_TriggerThresholdValueVector.size(); }
      const std::string&     zbSeedingThresholdName()   const { return m_ZBSeedingThresholdName; }
      int                    zbSeedingThresholdMulti()  const { return m_ZBSeedingThresholdMulti; }
      int                    bcDelay()                  const { return m_BCDelay; }
      unsigned int           bitnum()                   const { return m_Bitnum; }
      const std::string&     input()                    const { return m_Input; }

      // setters
      void setType                    (L1DataDef::TriggerType type);
      void setType                    (const std::string& type);
      void setActive                  (bool x)                        { m_Active = x; }
      void setMapping                 (int m)                         { m_Mapping = m; }
      void setCableName               (const std::string& cablename ) { m_CableName = cablename; }
      void setCableCtpin              (const std::string& cablectpin) { m_CableCtpin = cablectpin; }
      void setCableConnector          (const std::string& connector)  { m_CableConnector = connector; }
      void setCableStart              (int cable)                     { m_CableStart = cable; }
      void setCableEnd                (int cable)                     { m_CableEnd = cable; }
      void setClock                   (unsigned int clock)            { m_Clock = clock; }
      void setThresholdNumber         (int number)                    { m_ThresholdNumber = number; }
      void setZBSeedingThresholdName  (const std::string& seed)       { m_ZBSeedingThresholdName = seed; }
      void setZBSeedingThresholdMulti (int seedmulti)                 { m_ZBSeedingThresholdMulti = seedmulti; }
      void setBCDelay                 (int bcdelay)                   { m_BCDelay = bcdelay; }
      void setBitnum                  (unsigned int bitnum)           { m_Bitnum = bitnum; }
      void setInput                   (const std::string& input);

      // is internal threshold
      bool isInternal() const;

      // @brief return value for given eta and phi
      TriggerThresholdValue* triggerThresholdValue(int eta,int phi) const;
      const std::vector<TriggerThresholdValue*>& thresholdValueVector() const;
      void clearThresholdValues(); 

      void addThresholdValue(TriggerThresholdValue* value);
      TriggerThresholdValue* addThresholdValue(const std::string& type);

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

   protected:
      L1DataDef::TriggerType                m_type {L1DataDef::UNDEF};
      std::string                           m_SType {"dummy"};
      bool                                  m_Active {true};
      int                                   m_Mapping {-1};
      std::vector<TriggerThresholdValue*>   m_TriggerThresholdValueVector{};
      std::string                           m_CableName{""};
      std::string                           m_CableCtpin{""};
      std::string                           m_CableConnector{""};
      int                                   m_CableStart{0};
      int                                   m_CableEnd{0};
      unsigned int                          m_Clock{0};
      int                                   m_ThresholdNumber{-1}; // specifies the number within a certain set of thresholds
                                                                   // e.g. the 5th cluster threshold
      std::string                           m_ZBSeedingThresholdName{""};
      int                                   m_ZBSeedingThresholdMulti{0};
      int                                   m_BCDelay{3564};
      unsigned int                          m_Bitnum{1};
      std::string                           m_Input{"ctpin"};
   };

} 

#endif 
  
