/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITIZATION_TOOL_H
#define LUCID_DIGITIZATION_TOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "LUCID_DigitizationToolBox.h"

#include "LUCID_SimEvent/LUCID_SimHitCollection.h"
#include "LUCID_RawEvent/LUCID_DigitContainer.h"

#include "xAODEventInfo/EventInfo.h"   // SubEventIterator
#include "xAODEventInfo/EventAuxInfo.h"// SubEventIterator

#include "AthenaKernel/IAthRNGSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <vector>
#include <utility> /* pair */

namespace CLHEP {
  class HepRandomEngine;
}

class LUCID_PileUpTool: public PileUpToolBase, public LUCID_DigitizationToolBox {

public:

  LUCID_PileUpTool(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /// called before the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode prepareEvent(const EventContext& ctx, const unsigned int nInputEvents) override final;

  /// called for each active bunch-crossing to process current SubEvents bunchXing is in ns
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const override final;

  /// called at the end of the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

  /// perform the digitization - used by LUCID_DigiTop::execute
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

private:

  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "mergeSvc", "PileUpMergeSvc", ""};
  ServiceHandle<IAthRNGSvc> m_randomSvc{this, "RndmSvc", "AthRNGSvc", ""};
  Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "LUCIDRndEng", ""};

  std::string m_dataObjectName;
  std::string m_SimHitCollectionName;
  std::string m_digitsContainerName;
  std::string m_key_digitCnt;

  LUCID_DigitizationToolBox *m_digitToolBox{};

  ITHistSvc  *m_digitHistSvc{};

  LUCID_SimHitCollection* m_mergedhitList{};
};

#endif
