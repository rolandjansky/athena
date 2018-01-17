/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_HLTConfigSvc
#define TrigConfigSvc_HLTConfigSvc

#include <string>
#include <vector>

#include "GaudiKernel/HistoProperty.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigConfigSvc/ConfigSvcBase.h"
#include "TrigConfHLTData/HLTFrame.h"

class TH1F;
class TH2I;
class TrigTimer;

namespace TrigConf {

   class CTPConfig;
   class HLTChainList;
   class HLTSequenceList;
   class HLTPrescaleSet;
   class TrigDBConnectionConfig;

   /**
    * @brief Service providing the HLT trigger configuration chains and sequences
    *
    * This service should be accessed, to read the configuration lists
    */
   class HLTConfigSvc : public extends1<ConfigSvcBase, IHLTConfigSvc>
   {
   public:

      // implementing IIHLTConfigSvc
      const HLTChainList*    chainList() const __attribute__ ((deprecated));
      const HLTChainList&    chains() const;
      
      const HLTSequenceList* sequenceList() const __attribute__ ((deprecated));
      const HLTSequenceList& sequences() const;
      
      /*@brief constructor*/
      HLTConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~HLTConfigSvc();

      StatusCode initialize();
      StatusCode finalize();
      StatusCode start();

      StatusCode queryInterface( const InterfaceID& riid, void** ppvIF );

      StatusCode updatePrescaleSets(uint requestcount);

      // Access functions described by IHLTConfigSvc:
      const HLTFrame*        hltFrame() const { return &m_HLTFrame; }
      uint32_t               masterKey() const;
      uint32_t               hltPrescaleKey() const;

      void setL2LowerChainCounter(const CTPConfig*);

   private:
      StatusCode assignPrescalesToChains(uint lumiblock );

      StatusCode bookHistograms();
      void applyPrescaleSet(const HLTPrescaleSet& pss);
    
      enum ConfigSource { XML, ORACLE, MYSQL, SQLITE, DBLOOKUP };

      StoreGateSvc* m_eventStore;

      // The configuration:
      HLTFrame    m_HLTFrame; // what for is this varaible - seems unused ???

      uint        m_currentLumiblock;
      uint        m_currentPSS;

      // Properties:
      bool            m_setMergedHLT; 
      bool            m_doMon;
      std::string     m_partition;
      Histo1DProperty m_histProp_timePrescaleUpdate;

      // Histograms:
      TH1F* m_hist_timePrescaleUpdate;
      TH2I* m_hist_prescaleLB;

      StringProperty  m_PartitionName; // non-empty job-property overwrite value from DataFlowConfig

   };
}

#endif
