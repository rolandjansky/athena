/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHYTESTALG_H
#define TRIGGER_TGCCOINHIERARCHYTESTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"

class ITHistSvc;
class TH1F;
class TH1I;
class TH2I;

namespace Trigger {
  class ITgcCoinHierarchySvc;

  class TgcCoinHierarchyTestAlg : public AthAlgorithm {
  public:
    /** Constructor */
    TgcCoinHierarchyTestAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /** Initialize */
    StatusCode initialize();
    /** Process one event */
    StatusCode execute();
    /** Finalize */
    StatusCode finalize();

  private:
    /** The number of trigger sectors */
    enum NSECTORS { 
      NSIDES = 2, 
      NENDCAPTRIGGERSECTORS = 48, 
      NFORWARDTRIGGERSECTORS = 24, 
      NTRIGGERSECTORS = NSIDES*(NENDCAPTRIGGERSECTORS+NFORWARDTRIGGERSECTORS) 
    }; 
    
    /** ITgcCoinHierarchySvc */
    ITgcCoinHierarchySvc* m_tgcCoinSvc;

    /** Flag for showing the associated fractions */
    bool m_showFraction; 
    /** Flag for processing the event example */
    bool m_runEvent;
    /** Flag for processing the event example and make check histograms of TGC2 and TGC3 */
    bool m_runEventHistos;
    /** Flag for processing the TgcPrepData example */
    bool m_runTgcPrepData;
    /** Flag for processing the TgcCoinData example */
    bool m_runTgcCoinData;
    /** Flag for processing the Muon example */
    bool m_runMuon;
    /** Flag for processing the Track example */
    bool m_runTrack;

    /** ITHistSvc */
    ITHistSvc* m_thistSvc;
    /** TH2I for number of TGC2 and 3 hits */
    TH2I *m_h_nTGC23Hit[TgcCoinHierarchy::NTYPES]; 
    /** TH1I for number of TGC1 hits */
    TH1I *m_h_nTGC1Hit[TgcCoinHierarchy::NTYPES]; 

    /** Counters */
    unsigned long int m_nEvent;
    unsigned long int m_nSL;
    TH1F *m_hnSL;
    unsigned long int m_nSLWithTrackletWire;
    TH1F *m_hnSLWithTrackletWire;
    unsigned long int m_nSLWithTrackletStrip;
    TH1F *m_hnSLWithTrackletStrip;
    unsigned long int m_nSLWithPT2456;
    TH1F *m_hnSLWithPT2456;
    unsigned long int m_nSLWithPT2456WithHiPtWire;
    TH1F *m_hnSLWithPT2456WithHiPtWire;
    unsigned long int m_nSLWithPT2456WithHiPtStrip;
    TH1F *m_hnSLWithPT2456WithHiPtStrip;
    unsigned long int m_nHiPtWire;
    TH1F *m_hnHiPtWire;
    unsigned long int m_nHiPtWireWithTrackletWire;
    TH1F *m_hnHiPtWireWithTrackletWire;
    unsigned long int m_nHiPtWireWithTGC1WireHit;
    TH1F *m_hnHiPtWireWithTGC1WireHit;
    unsigned long int m_nHiPtStrip;
    TH1F *m_hnHiPtStrip;
    unsigned long int m_nHiPtStripWithTrackletStrip;
    TH1F *m_hnHiPtStripWithTrackletStrip;
    unsigned long int m_nHiPtStripWithTGC1StripHit;
    TH1F *m_hnHiPtStripWithTGC1StripHit;
    unsigned long int m_nTrackletWire;
    TH1F *m_hnTrackletWire;
    unsigned long int m_nTrackletWireWithTGC23WireHit;
    TH1F *m_hnTrackletWireWithTGC23WireHit;
    unsigned long int m_nTrackletStrip;
    TH1F *m_hnTrackletStrip;
    unsigned long int m_nTrackletStripWithTGC23StripHit;
    TH1F *m_hnTrackletStripWithTGC23StripHit;

    TH2I *m_hMuonPt;
  };
}

#endif // TRIGGER_TGCCOINHIERARCHYTESTALG_H
