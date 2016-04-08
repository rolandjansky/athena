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
    bool showFraction; 
    /** Flag for processing the event example */
    bool runEvent;
    /** Flag for processing the event example and make check histograms of TGC2 and TGC3 */
    bool runEventHistos;
    /** Flag for processing the TgcPrepData example */
    bool runTgcPrepData;
    /** Flag for processing the TgcCoinData example */
    bool runTgcCoinData;
    /** Flag for processing the Muon example */
    bool runMuon;
    /** Flag for processing the Track example */
    bool runTrack;

    /** ITHistSvc */
    ITHistSvc* m_thistSvc;
    /** TH2I for number of TGC2 and 3 hits */
    TH2I *m_h_nTGC23Hit[TgcCoinHierarchy::NTYPES]; 
    /** TH1I for number of TGC1 hits */
    TH1I *m_h_nTGC1Hit[TgcCoinHierarchy::NTYPES]; 

    /** Counters */
    unsigned long int nEvent;
    unsigned long int nSL;
    TH1F *m_hnSL;
    unsigned long int nSLWithTrackletWire;
    TH1F *m_hnSLWithTrackletWire;
    unsigned long int nSLWithTrackletStrip;
    TH1F *m_hnSLWithTrackletStrip;
    unsigned long int nSLWithPT2456;
    TH1F *m_hnSLWithPT2456;
    unsigned long int nSLWithPT2456WithHiPtWire;
    TH1F *m_hnSLWithPT2456WithHiPtWire;
    unsigned long int nSLWithPT2456WithHiPtStrip;
    TH1F *m_hnSLWithPT2456WithHiPtStrip;
    unsigned long int nHiPtWire;
    TH1F *m_hnHiPtWire;
    unsigned long int nHiPtWireWithTrackletWire;
    TH1F *m_hnHiPtWireWithTrackletWire;
    unsigned long int nHiPtWireWithTGC1WireHit;
    TH1F *m_hnHiPtWireWithTGC1WireHit;
    unsigned long int nHiPtStrip;
    TH1F *m_hnHiPtStrip;
    unsigned long int nHiPtStripWithTrackletStrip;
    TH1F *m_hnHiPtStripWithTrackletStrip;
    unsigned long int nHiPtStripWithTGC1StripHit;
    TH1F *m_hnHiPtStripWithTGC1StripHit;
    unsigned long int nTrackletWire;
    TH1F *m_hnTrackletWire;
    unsigned long int nTrackletWireWithTGC23WireHit;
    TH1F *m_hnTrackletWireWithTGC23WireHit;
    unsigned long int nTrackletStrip;
    TH1F *m_hnTrackletStrip;
    unsigned long int nTrackletStripWithTGC23StripHit;
    TH1F *m_hnTrackletStripWithTGC23StripHit;

    TH2I *m_hMuonPt;
  };
}

#endif // TRIGGER_TGCCOINHIERARCHYTESTALG_H
