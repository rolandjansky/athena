/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcSkim_h
#define D3PD_L1TgcSkim_h

#include <string>
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "Particle/TrackParticle.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trig { class TrigDecisionTool; }

class ISvcLocator;
class ITHistSvc;
class TH1I;



class L1TgcSkim : public AthFilterAlgorithm {
  public:
    L1TgcSkim(const std::string& name,
              ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    bool preSelection(const std::string& key);
    bool tagAndProbeMs(const std::string& key);
    bool tagAndProbeId(const std::string& key);
    bool isCandidate(const Rec::TrackParticle* tag,
                     const Rec::TrackParticle* probe,
                     const int prescale=0) const;
    bool isGoodIdTrack(const Rec::TrackParticle* tp) const;

    ServiceHandle<ITHistSvc> m_thistSvc;
    ToolHandle<Trig::TrigDecisionTool> m_tdt;

    std::string m_muonContainerName1;
    std::string m_muonContainerName2;
    std::string m_trackParticleContainerName;
    std::string m_jetFilterPattern;
    std::string m_passThroughFilterPattern;
    double m_preselMuonPtCut;
    double m_jpsiTagPtCut;
    double m_jpsiProbePtCut;
    double m_zTagPtCut;
    double m_zProbePtCut;
    double m_jpsiMassLowerLimit;
    double m_jpsiMassUpperLimit;
    double m_zMassLowerLimit;
    double m_zMassUpperLimit;
    bool m_hasOppositeCharge;
    bool m_singleMuon;
    bool m_dumpAll;
    bool m_jetFilter;
    int m_prescaleTandp;
    int m_prescaleNotTandp;

    TH1I* m_hNEvents;
    int nEventsProcessed;
    int nEventsSkipped;
    int nEventsPreselection;
    int nEventsTandpMs;
    int nEventsTandpId;
    mutable int nJpsiCandidates;
    mutable int nZCandidates;
};
#endif
/* eof */
