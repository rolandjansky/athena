/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_Muctpi
#define TrigConf_Muctpi

#include <string>
#include <vector>
#include <map>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {

   class Run3MuonTriggerThreshold;

   class Muctpi : public L1DataBaseclass {
   public:

      enum Detector { RPC, TGC }; 

      Muctpi();
      ~Muctpi();
    
      // getters
      int lowptThreshold()    const { return m_LowptThreshold; }
      int highptThreshold()   const { return m_HighptThreshold; }
      int maxCand()           const { return m_MaxCand; }
      const std::vector<const Run3MuonTriggerThreshold*> & thresholds() const { return m_thresholds; } 
      const std::vector<std::pair<std::string, std::vector<unsigned int>>> &
         roiExclusionList(const std::string & name, Detector det) const; 


      // setters
      void setLowptThreshold( int l )     { m_LowptThreshold = l; }
      void setHighptThreshold( int h )    { m_HighptThreshold = h; }
      void setMaxCand( int m )            { m_MaxCand = m; }
      void addThreshold( const Run3MuonTriggerThreshold * thr ) { m_thresholds.push_back(thr); }
      void createRoiExclusionList(const std::string & name, Detector det);
      void addSectorRoiList(const std::string & listname, Detector det, const std::string & sectorname, const std::string & roiids);

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
    
   private:
      int  m_LowptThreshold;
      int  m_HighptThreshold;
      int  m_MaxCand;
      std::vector<const Run3MuonTriggerThreshold*> m_thresholds;
      std::map<std::string, std::vector<std::pair<std::string, std::vector<unsigned int>>>> m_rpcExcl;
      std::map<std::string, std::vector<std::pair<std::string, std::vector<unsigned int>>>> m_tgcExcl;

   };
}

#endif 
  
