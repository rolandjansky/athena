/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MetAnalysisAlgorithms/MetSignificanceAlg.h>

#include <xAODMissingET/MissingETAuxContainer.h>
#include "xAODEventInfo/EventInfo.h"

//
// method implementations
//
namespace CP
{
  MetSignificanceAlg ::
  MetSignificanceAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_significanceTool ("METMaker", this)
  {
    declareProperty ("significanceTool", m_significanceTool, "the significance tool we apply");
    declareProperty ("significanceDecoration", m_significanceDecoration, "the decoration to use for the significance");
    declareProperty ("totalMETName", m_totalMETName, "the key for the final met term");
    declareProperty ("jetTermName", m_jetTermName, "the key for the jets term");
    declareProperty ("softTermName", m_softTermName, "the key for the soft term");
  }



  StatusCode MetSignificanceAlg ::
  initialize ()
  {
    if (m_significanceDecoration.empty())
    {
      ANA_MSG_ERROR ("no significance decoration name set");
      return StatusCode::FAILURE;
    }
    m_significanceAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_significanceDecoration);

    ANA_CHECK (m_significanceTool.retrieve());
    m_systematicsList.addHandle (m_metHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode MetSignificanceAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        // I'm not sure why this can't be const, but the interface
        // requires a non-const object
        xAOD::MissingETContainer *met {};
        ANA_CHECK (m_metHandle.getCopy (met, sys));
	
	const xAOD::EventInfo* evtInfo = 0;
	ANA_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

        ANA_CHECK (m_significanceTool->varianceMET (met, evtInfo->averageInteractionsPerCrossing(), m_jetTermName, m_softTermName, m_totalMETName));
        const float significance = m_significanceTool->GetSignificance();
        (*m_significanceAccessor) (*(*met)[m_totalMETName]) = significance;

        return StatusCode::SUCCESS;
      });
  }
}
