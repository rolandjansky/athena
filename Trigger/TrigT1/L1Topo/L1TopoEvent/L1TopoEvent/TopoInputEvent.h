//  TopoInputEvent.h
//  L1TopoEvent
//  Created by Joerg Stelzer on 11/21/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __L1TopoEvent__TopoInputEvent__
#define __L1TopoEvent__TopoInputEvent__

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
      
      StatusCode addCluster(const ClusterTOB & cluster);
      StatusCode addTau(const ClusterTOB & tau);
      StatusCode addMuon(const MuonTOB & muon);
      StatusCode addLateMuon(const LateMuonTOB & muon);
      StatusCode addMuonNextBC(const MuonNextBCTOB & muon);
      StatusCode addJet(const JetTOB & jet);
      StatusCode setMET(const MetTOB & met);

      // access all inputs by type
      const InputTOBArray * inputTOBs(TCS::inputTOBType_t) const;

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

      bool              m_dumpEnabled { false };
      std::string       m_inputDumpFile { "" };

   };
   
}


#endif
