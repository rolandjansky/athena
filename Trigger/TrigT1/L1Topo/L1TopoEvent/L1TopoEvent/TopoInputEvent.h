// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_TopoInputEvent
#define L1TopoEvent_TopoInputEvent

#include <iostream>
#include <vector>

#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/LateMuonTOBArray.h"
#include "L1TopoEvent/MuonNextBCTOBArray.h"
#include "L1TopoEvent/MuonTOBArray.h"
#include "L1TopoEvent/MetTOBArray.h"
#include "L1TopoCommon/StatusCode.h"
#include "L1TopoCommon/Types.h"

#include "TrigConfBase/TrigConfMessaging.h"

namespace TCS {
   class TopoInputEvent;
}

namespace TCS {

  std::ostream & operator<<(std::ostream &, const TCS::TopoInputEvent &);

   class TopoInputEvent : public TrigConf::TrigConfMessaging {
   public:
      
      TopoInputEvent(const std::string & clusterName = "InputClusters",
                     const std::string & tauName = "InputTaus",
                     const std::string & jetName = "InputJets",
                     const std::string & muonName = "InputMuons",
                     const std::string & lateMuonName = "InputLateMuons",
                     const std::string & muonNextBCName = "InputMuonsNextBC",
                     const std::string & metName = "InputMet");
      ~TopoInputEvent();
      
      const ClusterTOBArray & clusters() const { return m_clusters; }
      const ClusterTOBArray & taus() const { return m_taus; }
      const MuonTOBArray & muons() const { return m_muons; }
      const LateMuonTOBArray & lateMuons() const { return m_lateMuons; }
      const MuonNextBCTOBArray & muonsNextBC() const { return m_muonsNextBC; }
      const JetTOBArray & jets() const { return m_jets; }
      const MetTOB & met() const { return m_met[0]; }
      uint32_t run_number()        const { return m_runNo; }
      uint32_t event_number()      const { return m_evtNo; }
      uint32_t lumi_block()        const { return m_lumiB; }
      uint32_t bunch_crossing_id() const { return m_BCID;  }
      /** @defgroup groupOverflowGetters
       *  @brief getter function for overflow bits
       *  @{
       */
      bool overflowFromEmtauInput () const { return m_overflowFromEmtauInput ; }
      bool overflowFromJetInput   () const { return m_overflowFromJetInput   ; }
      bool overflowFromEnergyInput() const { return m_overflowFromEnergyInput; }
      bool overflowFromMuonInput  () const { return m_overflowFromMuonInput  ; }
      /** @} */ // end of groupOverflowGetters
      StatusCode addCluster(const ClusterTOB & cluster);
      StatusCode addTau(const ClusterTOB & tau);
      StatusCode addMuon(const MuonTOB & muon);
      StatusCode addLateMuon(const LateMuonTOB & muon);
      StatusCode addMuonNextBC(const MuonNextBCTOB & muon);
      StatusCode addJet(const JetTOB & jet);
      StatusCode setMET(const MetTOB & met);
      StatusCode setEventInfo(const uint32_t runNo, const uint32_t evtNo, const uint32_t lumiB, const uint32_t BCID);
      /** @defgroup groupOverflowSetters
       *  @brief setter function for overflow bits
       *  The bits are being propagated from the input providers.
       *  @{
       */
      void setOverflowFromEmtauInput (const bool &v);
      void setOverflowFromJetInput   (const bool &v);
      void setOverflowFromEnergyInput(const bool &v);
      void setOverflowFromMuonInput  (const bool &v);
      /** @} */ // end of groupOverflowSetters
      // access all inputs by type
      const InputTOBArray * inputTOBs(TCS::inputTOBType_t) const;
      /// @brief whether there are input overflows from Mioct for muon and from CMX for calo
      bool hasInputOverflow(TCS::inputTOBType_t) const;

      // to be calles after each event
      StatusCode clear();

      // to enable dumping of the input after each event
      void enableInputDump( const std::string& filename, bool enable = true );

      // to trigger the writing of the event
      void dumpStart();
      void dump();
      void dumpFinish();

      void print() const;

   private:
      friend std::ostream & operator<<(std::ostream &, const TCS::TopoInputEvent &);

      // data is owned by the TopoInputEvent
      ClusterTOBArray   m_clusters;
      ClusterTOBArray   m_taus;
      JetTOBArray       m_jets;
      MuonTOBArray      m_muons;
      LateMuonTOBArray  m_lateMuons;
      MuonNextBCTOBArray  m_muonsNextBC;
      MetTOBArray       m_met; // will have size 1

      uint32_t m_runNo {0};
      uint32_t m_evtNo {0};
      uint32_t m_lumiB {0};
      uint32_t m_BCID {0};

      bool m_dumpEnabled { false };
      bool m_overflowFromEmtauInput { false };
      bool m_overflowFromJetInput { false };
      bool m_overflowFromEnergyInput { false };
      bool m_overflowFromMuonInput { false };
      std::string m_inputDumpFile { "" };

   };
   
}


#endif
