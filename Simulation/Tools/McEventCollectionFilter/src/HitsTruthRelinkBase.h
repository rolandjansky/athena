/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HITSTRUTHRELINKBASE_H
#define HITSTRUTHRELINKBASE_H

// Base class include
#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <GeneratorObjects/McEventCollection.h>


class HitsTruthRelinkBase : public AthReentrantAlgorithm
{
public:
  HitsTruthRelinkBase(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;

protected:
  StatusCode getReferenceBarcode(const EventContext &ctx, int *barcode) const;

  SG::ReadHandleKey<McEventCollection> m_inputTruthCollectionKey {this, "InputTruthCollection", "TruthEvent", "Input truth collection name"};
};

#endif
