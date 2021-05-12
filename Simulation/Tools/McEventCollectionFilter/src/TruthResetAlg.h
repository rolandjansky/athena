/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRESETALG_H
#define TRUTHRESETALG_H

// Base class include
#include "AthenaBaseComps/AthAlgorithm.h"

// Athena includes
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "GeneratorObjects/McEventCollection.h"

class TruthResetAlg : public AthAlgorithm {

public:
   TruthResetAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~TruthResetAlg() = default;
   virtual StatusCode initialize() override final;
   virtual StatusCode execute() override final;

  private:

   SG::ReadHandleKey<McEventCollection> m_inputMcEventCollection;
   SG::WriteHandleKey<McEventCollection> m_outputMcEventCollection;

};
#endif
