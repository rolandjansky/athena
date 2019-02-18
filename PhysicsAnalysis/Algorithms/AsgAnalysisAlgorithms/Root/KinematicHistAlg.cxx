/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AsgAnalysisAlgorithms/KinematicHistAlg.h>

#include <RootCoreUtils/StringUtil.h>
#include <TH1.h>

//
// method implementations
//

namespace CP
{
  KinematicHistAlg ::
  KinematicHistAlg (const std::string& name, 
                          ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("histPattern", m_histPattern, "the pattern for histogram names");
  }



  StatusCode KinematicHistAlg ::
  initialize ()
  {
    m_systematicsList.addHandle (m_inputHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode KinematicHistAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        const xAOD::IParticleContainer *input = nullptr;
        ANA_CHECK (m_inputHandle.retrieve (input, sys));

        auto histIter = m_hist.find (sys);
        if (histIter == m_hist.end())
        {
          std::string name;
          HistGroup group;

          name = RCU::substitute (m_histPattern, "%VAR%", "multiplicity");
          name = makeSystematicsName (name, sys);
          ANA_CHECK (book (TH1F (name.c_str(), "multiplicity", 20, 0, 20)));
          group.multiplicity = hist (name);

          m_hist.insert (std::make_pair (sys, group));
          histIter = m_hist.find (sys);
          assert (histIter != m_hist.end());
        }

        std::size_t count = 0;
        for (std::size_t iter = 0; iter != input->size(); ++ iter)
        {
          const xAOD::IParticle *particle = (*input)[iter];
          if (m_preselection.getBool (*particle))
          {
            while (histIter->second.perObject.size() <= count)
            {
              std::string name;
              HistSubgroup group;

              name = RCU::substitute (m_histPattern, "%VAR%", "pt" + std::to_string(histIter->second.perObject.size()));
              name = makeSystematicsName (name, sys);
              ANA_CHECK (book (TH1F (name.c_str(), "pt", 20, 0, 200e3)));
              group.pt = hist (name);

              name = RCU::substitute (m_histPattern, "%VAR%", "eta" + std::to_string(histIter->second.perObject.size()));
              name = makeSystematicsName (name, sys);
              ANA_CHECK (book (TH1F (name.c_str(), "eta", 20, -5, 5)));
              group.eta = hist (name);

              name = RCU::substitute (m_histPattern, "%VAR%", "phi" + std::to_string(histIter->second.perObject.size()));
              name = makeSystematicsName (name, sys);
              ANA_CHECK (book (TH1F (name.c_str(), "phi", 20, -M_PI, M_PI)));
              group.phi = hist (name);

              histIter->second.perObject.push_back (group);
            }

            HistSubgroup& group = histIter->second.perObject[count];

            group.pt->Fill (particle->pt());
            group.eta->Fill (particle->eta());
            group.phi->Fill (particle->phi());
            count += 1;
          }
        }

        histIter->second.multiplicity->Fill (count);

        return StatusCode::SUCCESS;
      });
  }
}
