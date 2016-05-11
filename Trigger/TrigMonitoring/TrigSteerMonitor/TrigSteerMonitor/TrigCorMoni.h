/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigSignatureMonitor
 * @Class  : TrigSignatureMoni
 * @Created: Nov, 2006
 *
 * @brief  Make correlation plots between L1 accept and HLT accept and HLT streams
 *
 **********************************************************************************/
#ifndef TRIGCORMONI_H
#define TRIGCORMONI_H

#include <string>
#include <map>

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

class TH1I;
class TH2I;
class StoreGateSvc;

namespace LVL1CTP
{
  class Lvl1Result;
}
namespace HLT 
{
  class TrigSteer;
}
namespace TrigConf
{
  class TriggerItem;
  class HLTChain;
}

class TrigCorMoni: public TrigMonitorToolBase {  
public:

   TrigCorMoni(const std::string & type,
               const std::string & name,
               const IInterface* parent);
  
   virtual ~TrigCorMoni();

   virtual StatusCode initialize();
  
   virtual StatusCode bookHists();
   virtual StatusCode fillHists();
   virtual StatusCode finalHists();
  
private:

   bool getLvl1Result(LVL1CTP::Lvl1Result &resultL1);
  
   std::vector<const TrigConf::TriggerItem *> FindL1Items(const TrigConf::HLTChain &chain);

private:
  
   struct Data {
      Data() : bin(-1) {}

      int bin;
      std::set<unsigned> ctpids;
   };

private:

   // Tool properties
   std::string             m_histPath;         // Histogram path with HistSvc
   std::string             m_keyL1Result;      // Key to retrieve the L1Result from SG
   std::string             m_keyRBResult;      // Key to retrieve the RoIBResult from SG

   // Tool variables
   std::string             m_level;
   const HLT::TrigSteer   *m_parentAlg;
  
   ServiceHandle<TrigConf::ITrigConfigSvc > m_configSvc;
  
   std::map<std::string, unsigned>  m_stream2bin; // map from stream name to bin number
   std::map<unsigned, Data>         m_hash2chain; // map from chain hash id to chain struct

   // histograms
   TH1I* m_rejectL1;
   TH2I* m_streamL1;
   TH2I* m_acceptL1;
};

#endif


