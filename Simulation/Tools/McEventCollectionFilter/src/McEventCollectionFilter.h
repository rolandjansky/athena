/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCEVENTCOLLECTIONFILTER_H
#define MCEVENTCOLLECTIONFILTER_H

// Base class include
#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <GeneratorObjects/McEventCollection.h>
#include <InDetSimEvent/TRTUncompressedHitCollection.h>


class McEventCollectionFilter : public AthReentrantAlgorithm
{
public:
  McEventCollectionFilter(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  StatusCode findElectronsLinkedToTRTHits(const EventContext &ctx, std::vector<int> *barcodes) const;

  SG::ReadHandleKey<McEventCollection> m_inputTruthCollectionKey {this, "InputTruthCollection", "TruthEventOLD", "Input truth collection name"};
  SG::WriteHandleKey<McEventCollection> m_outputTruthCollectionKey {this, "OutputTruthCollection", "TruthEvent", "Output truth collection name"};

  Gaudi::Property<bool> m_keepElectronsLinkedToTRTHits {this, "KeepElectronsLinkedToTRTHits", false, "Keep electrons linked to TRT hits"};
  SG::ReadHandleKey<TRTUncompressedHitCollection> m_inputTRTHitsKey {this, "InputTRTHits", "TRTUncompressedHitsOLD", "Input TRT hits name"};

  Gaudi::Property<int> m_pileUpParticlePDGID {this, "PileUpParticlePDGID", 999, "Pile-up particle PDG ID (defaults to geantino)"};
};

#endif
