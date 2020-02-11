// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_Run3MuonTriggerThreshold
#define TrigConf_Run3MuonTriggerThreshold

#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/L1DataBaseclass.h"

#include <string>
#include <vector>
#include <iostream>

namespace TrigConf {

   class Run3MuonTriggerThreshold : public L1DataBaseclass {
   public:
      class Region {
      public:
         enum SpecificationType { FULL, ETAPHI, REGION, UNSPEC };
         
         void setEtamin(int);
         void setEtamax(int);
         void setPhimin(int);
         void setPhimax(int);
         void setDetector(const std::string &);

         int etamin() const { return m_etamin;}
         int etamax() const { return m_etamax;}
         int phimin() const { return m_phimin;}
         int phimax() const { return m_phimax;}
         std::string detector() const { return m_detector;}
         SpecificationType spec() const { return m_spec; }
      private:
         int m_etamin { 0 };
         int m_etamax { 0 };
         int m_phimin { 0 };
         int m_phimax { 0 };
         std::string m_detector { "" };
         unsigned short m_region { 0 };
         SpecificationType m_spec { UNSPEC };         
      };

      class Cable {
      public:
         void setName(const std::string & name ) { m_name = name; }
         void setType(const std::string & type ) { m_type = type; }
         void setInput(const std::string & input ) { m_input = input; }
         void setConnector(const std::string & connector ) { m_connector = connector; }
         void setRangeBegin( unsigned int rb ) { m_rangeBegin = rb; }
         void setRangeEnd( unsigned int re ) { m_rangeEnd = re; }

         std::string name() const { return m_name;  }
         std::string type() const { return m_type; }
         std::string input() const { return m_input; }
         std::string connector() const { return m_connector; }
         unsigned int rangeBegin() const { return m_rangeBegin; }
         unsigned int rangeEnd() const { return m_rangeEnd; }

         std::string asString() const;
      private:
         std::string  m_name { "" };
         std::string  m_type { "" };
         std::string  m_input { "" };
         std::string  m_connector { "" };
         unsigned int m_rangeBegin { 0 };
         unsigned int m_rangeEnd { 0 };
      };

   public:
      Run3MuonTriggerThreshold();
      Run3MuonTriggerThreshold( const Run3MuonTriggerThreshold& thr);
      virtual ~Run3MuonTriggerThreshold();

      // accessors
      const std::string&     type()    const { return m_type; }
      unsigned int           mapping() const { return m_mapping; }
      unsigned int           bitnum()  const { return m_bitnum; }
      const std::string&     input()   const { return m_input; }
      bool                   isMultiplicityThr() const { return m_isMultiplicityThr; }
      unsigned int           rpcThr() const { return m_rpcThr; }
      unsigned int           tgcThr() const { return m_tgcThr; }
      const std::string &    rpcExcl() const { return m_rpcExcl; }
      const std::string &    tgcExcl() const { return m_tgcExcl; }
      const std::string &    tgcFlags() const { return m_tgcFlags; }
      const Region &         region()  const { return m_region; }
      const Cable &          cable()   const { return m_cable; }
      Region &               region() { return m_region; }
      Cable &                cable()  { return m_cable; }

      // setters
      void setType              (const std::string& type)       { m_type = type; }
      void setMapping           (int mapping)                   { m_mapping = mapping; }
      void setBitnum            (unsigned int bitnum)           { m_bitnum = bitnum; }
      void setInput             (const std::string & input)     { m_input = input; }
      void setIsMultiplicityThr (bool isMultiplicityThr)        { m_isMultiplicityThr = isMultiplicityThr; }
      void setRPCThr            (unsigned int rpcThr)           { m_rpcThr = rpcThr; }
      void setRPCExcl           (const std::string & rpcExcl)   { m_rpcExcl = rpcExcl; }
      void setTGCThr            (unsigned int tgcThr)           { m_tgcThr = tgcThr; }
      void setTGCFlags          (const std::string & tgcFlags)  { m_tgcFlags = tgcFlags; }
      void setTGCExcl           (const std::string & tgcExcl)   { m_tgcExcl = tgcExcl; }

      virtual void print(const std::string& indent="", unsigned int detail=3) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

   protected:

      std::string  m_type { "" };
      std::string  m_input { "" };
      unsigned int m_bitnum { 0 };
      unsigned int m_mapping { 0 };
      bool         m_isMultiplicityThr { true };
      unsigned int m_rpcThr { 0 };
      unsigned int m_tgcThr { 0 };
      std::string  m_tgcFlags { "" };
      std::string  m_rpcExcl { "" };
      std::string  m_tgcExcl { "" };
      Region       m_region;
      Cable        m_cable;
   };

} 

#endif 
  
