/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGVertexRefittingTool.h, (c) ATLAS Detector software
// author: ioannis.nomidis@cern.ch
///////////////////////////////////////////////////////////////////

/**
 * refit the primary vertex after removing the tracks of electrons from a Z->ee decay
 * to imitate the primary vertex reconstruction in H->yy events with unconverted photons
*/

#ifndef DERIVATIONFRAMEWORK_ZEEVERTEXREFITTINGTOOL_H
#define DERIVATIONFRAMEWORK_ZEEVERTEXREFITTINGTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "ExpressionEvaluation/ExpressionParser.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"



namespace DerivationFramework {

  class ZeeVertexRefittingTool : public AthAlgTool, public IAugmentationTool {
    public: 
      ZeeVertexRefittingTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize() override;
      StatusCode finalize() override;
      StatusCode addBranches() const override;

    private:
      std::string m_expression;
      std::unique_ptr<ExpressionParsing::ExpressionParser> m_parser;
      float m_massCut;

      SG::ReadHandleKey<xAOD::VertexContainer> m_primaryVertexKey{this, "PVContainerName", "PrimaryVertices", "" };
      SG::ReadHandleKey<xAOD::ElectronContainer> m_electronKey { this, "ElectronContainerName", "Electrons", "" };
      SG::WriteHandleKey<xAOD::VertexContainer> m_refitpvKey{this, "RefittedPVContainerName", "HggPrimaryVertices", "" };
      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey { this, "EventInfoKey", "EventInfo", "" };
      
      std::vector<unsigned int> m_MCSamples;

      ToolHandle < Analysis::PrimaryVertexRefitter > m_pvrefitter;

      StatusCode makeZeePairs( const xAOD::ElectronContainer *particles, std::vector<std::vector<unsigned int> > &ZeePairs) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_ZEEVERTEXREFITTINGTOOL_H
