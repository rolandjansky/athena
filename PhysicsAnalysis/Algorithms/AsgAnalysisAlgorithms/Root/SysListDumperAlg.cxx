/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


//
// includes
//

#include <AsgAnalysisAlgorithms/SysListDumperAlg.h>
#include <SystematicsHandles/Helpers.h>
#include <TH1.h>

//
// method implementations
//

namespace CP
{
  SysListDumperAlg :: 
  SysListDumperAlg (const std::string& name, 
                    ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("histogramName", m_histogramName, "the name of the output histogram");
  }



  StatusCode SysListDumperAlg ::
  initialize ()
  {
    if (m_histogramName.empty())
    {
      ANA_MSG_ERROR ("histogram name should not be empty");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (m_systematicsList.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode SysListDumperAlg ::
  execute ()
  {
    if (!m_firstEvent)
    {
      return StatusCode::SUCCESS;
    }

    m_firstEvent = false;

    const std::unordered_set<CP::SystematicSet> systematics = m_systematicsList.systematicsVector();

    ANA_CHECK (book (TH1F (m_histogramName.c_str(), "systematics", systematics.size(), 0, systematics.size())));
    TH1 *histogram = hist (m_histogramName);

    int i = 1;
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode
    {
      std::string name = sys.name();
      if (name.empty())
        name = nominalSystematicsName();

      histogram->GetXaxis()->SetBinLabel(i, name.c_str());
      i++;

      return StatusCode::SUCCESS;
    });
  }
}
