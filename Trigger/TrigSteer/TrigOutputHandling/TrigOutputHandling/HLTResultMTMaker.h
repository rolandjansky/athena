/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGOUTPUTHANDLING_HLTRESULTMTMAKER_H
#define TRIGOUTPUTHANDLING_HLTRESULTMTMAKER_H

#include "TrigOutputHandling/HLTResultMT.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandle.h"

/** @class HLTResultMTMaker
 *  @brief Tool to create the HLTResultMT at the end of each event
 **/
class HLTResultMTMaker : public AthAlgTool {
public:
  /// Standard constructor
  HLTResultMTMaker(const std::string& type, const std::string& name, const IInterface* parent);
  /// Standard destructor
  virtual ~HLTResultMTMaker();

  // ------------------------- IStateful methods -----------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ------------------------- Specific methods of this tool -----------------
  /// Builds the HLTResultMT and records it in the event store
  StatusCode makeResult(const EventContext& eventContext) const;
  /// Return name of the HLTResultMT
  const std::string& resultName() const {return m_hltResultWHKey.key();}

private:
  /// StoreGate key for the HLTResultMT
  SG::WriteHandleKey<HLTResultMT> m_hltResultWHKey;
};

#endif // TRIGOUTPUTHANDLING_HLTRESULTMTMAKER_H
