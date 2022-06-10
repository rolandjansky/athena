/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGVSI_TRACKCOUNTHYPOTOOL_H
#define TRIGVSI_TRACKCOUNTHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "xAODTracking/Vertex.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include <vector>

class TrigVSIHypoTool : virtual public ::AthAlgTool
{
   public:

      TrigVSIHypoTool( const std::string& type,
         const std::string& name,
         const IInterface* parent );

      virtual ~TrigVSIHypoTool() = default;
      virtual StatusCode initialize() override;

      struct eventVtxInfo {
         TrigCompositeUtils::Decision*    decision;
         std::vector<const xAOD::Vertex*> vertices;
         const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      };

      StatusCode decide( eventVtxInfo& decisions ) const;

   private:

      HLT::Identifier m_decisionId;
      Gaudi::Property<float>  m_minNTrks {this, "minNTrks", 3,       "Accept vertices with minimum of this number of tracks"};
      Gaudi::Property<float>  m_maxNTrks {this, "maxNTrks", -1,      "Accept vertices with maximum of this number of tracks"};
      Gaudi::Property<float>  m_minR     {this, "minR" ,    20.,     "Accept vertices with decay position R greater than this limit"};
      Gaudi::Property<float>  m_maxR     {this, "maxR" ,    520.,    "Accept vertices with decay position R smaller than this limit"};
      Gaudi::Property<float>  m_minPt    {this, "minPt",    1000.,   "Accept vertices with momentum higher than this limit"};
      Gaudi::Property<float>  m_maxPt    {this, "maxPt",    1.4e7,   "Accept vertices with momentum smaller than this limit"};
      Gaudi::Property<float>  m_maxChi2  {this, "maxChi2",  10.,     "Accept vertices with chi2 less than this limit"};
      Gaudi::Property<float>  m_minMass  {this, "minMass", 1000.,  "Accept events with a vertex with the mass larger than this limit"};
      Gaudi::Property<float>  m_maxMass  {this, "maxMass", 1.4e7,  "Accept events with a vertex with the mass larger than this limit"};
      Gaudi::Property<bool>   m_logicAnd {this, "TriggerTypeAnd", true, "And/Or Logic for nTrkCut, RCut, pTCut, chi2Cut, massCut"};
};

#endif //> !TRIGVSI_TRACKCOUNTHYPOTOOL_H
