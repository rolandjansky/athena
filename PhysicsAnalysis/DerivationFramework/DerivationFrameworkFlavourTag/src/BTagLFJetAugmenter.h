/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_LF_JET_AUGMENTER_HH
#define BTAG_LF_JET_AUGMENTER_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"

class BTagJetAugmenter;

class BTagLFJetAugmenter: public AthAlgorithm {
 public:
  BTagLFJetAugmenter(const std::string& name,
		     ISvcLocator* pSvcLocator );
  virtual StatusCode  execute() override;
 private:
  std::string m_jet_collection;

  // Dec for no. parent particles
  SG::AuxElement::Decorator<float> m_dec_nConversion;
  SG::AuxElement::Decorator<float> m_dec_nKs;
  SG::AuxElement::Decorator<float> m_dec_nLambda;
  SG::AuxElement::Decorator<float> m_dec_nshadron;

  //Dec for no. tracks
  SG::AuxElement::Decorator<float> m_dec_Conversion;
  SG::AuxElement::Decorator<float> m_dec_Ks;
  SG::AuxElement::Decorator<float> m_dec_Lambda;
  SG::AuxElement::Decorator<float> m_dec_shadron;
  SG::AuxElement::Decorator<float> m_dec_HadMatInt;
  SG::AuxElement::Decorator<float> m_dec_FakeHS;
  SG::AuxElement::Decorator<float> m_dec_HS;
  SG::AuxElement::Decorator<float> m_dec_NTruthLink;

  std::string m_outStream;

};

#endif
