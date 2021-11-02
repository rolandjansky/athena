/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


//
// includes
//

#include <regex>
#include <AsgAnalysisAlgorithms/SysListDumperAlg.h>
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
    declareProperty ("systematicsService", m_systematicsService, "systematics service");
    declareProperty ("systematicsRegex", m_regex, "systematics regex");
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

    ANA_CHECK (m_systematicsService.retrieve());

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

    const std::vector<CP::SystematicSet> systematics = makeSystematicsVector (m_regex);

    ANA_CHECK (book (TH1F (m_histogramName.c_str(), "systematics", systematics.size(), 0, systematics.size())));
    TH1 *histogram = hist (m_histogramName);

    int i = 1;
    for (const auto& sys : systematics)
    {
      std::string name;
      ANA_CHECK (m_systematicsService->makeSystematicsName (name, "%SYS%", sys));

      histogram->GetXaxis()->SetBinLabel(i, name.c_str());
      i++;
    }

    return StatusCode::SUCCESS;
  }



  std::vector<CP::SystematicSet> SysListDumperAlg ::
  makeSystematicsVector (const std::string &regex) const
  {
    std::vector<CP::SystematicSet> inputVector = m_systematicsService->makeSystematicsVector ();
    if (regex.empty())
    {
      return inputVector;
    }

    std::vector<CP::SystematicSet> systematicsVector;
    std::regex expr (regex);
    for (const CP::SystematicSet& sys : inputVector)
    {
      if (regex_match (sys.name(), expr))
      {
        systematicsVector.push_back (sys);
      }
    }
    return systematicsVector;
  }

}
