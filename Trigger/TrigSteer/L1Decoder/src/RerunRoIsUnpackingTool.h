/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_RERUNROISUNPACKINGTOOL_H
#define L1DECODER_RERUNROISUNPACKINGTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "RoIsUnpackingToolBase.h"



/**
 * @class $(klass)s
 * @brief
 **/

class RerunRoIsUnpackingTool : public RoIsUnpackingToolBase
{

 public:

  RerunRoIsUnpackingTool( const std::string& type,
			  const std::string& name,
			  const IInterface* parent );

  virtual ~RerunRoIsUnpackingTool();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode updateConfiguration() override;

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack( const EventContext& ctx,
		     const ROIB::RoIBResult& roib,
		     const HLT::IDSet& activeChains ) const override;


 private:
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_sourceKey{
    this, "SourceDecisions", "UnspecifiedRoIDecisionsForRerunning",
    "The collection which has to be checked for RoIs to re-enable" };

};


#endif //> !L1DECODER_RERUNROISUNPACKINGTOOL_H
