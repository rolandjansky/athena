/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#ifdef ROOTCORE

#include <QuickAna/ValidationHists.h>

#include <EventLoop/IWorker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <RootCoreUtils/Assert.h>
#include <TH1.h>

//
// method implementations
//

namespace ana
{
  void ValidationHists ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  ValidationHists ::
  ValidationHists (EL::IWorker *val_worker,
		   const InternalConfiguration& configuration)
    : m_worker (val_worker),
      m_selectionAccessor (configuration.selectionName (SelectionStep::ANALYSIS))
  {
    RCU_NEW_INVARIANT (this);
  }



  void ValidationHists ::
  fillHists (const std::string& prefix, const IEventObjects& objects,
	     float weight)
  {
    RCU_CHANGE_INVARIANT (this);
    fillHist (prefix + "weightln", 20, -5, 5, log (weight), 1);
    if (objects.jets()) fillHists (prefix + "jet", *objects.jets(), weight);
    if (objects.fat_jets()) fillHists (prefix + "fat_jet", *objects.fat_jets(), weight);
    if (objects.photons()) fillHists (prefix + "photon", *objects.photons(), weight);
    if (objects.electrons()) fillHists (prefix + "electron", *objects.electrons(), weight);
    if (objects.muons()) fillHists (prefix + "muon", *objects.muons(), weight);
    if (objects.taus()) fillHists (prefix + "tau", *objects.taus(), weight);
    if (objects.met()) fillHists (prefix + "met", *(*objects.met())["Final"], weight);
    if (objects.met2()) fillHists (prefix + "met2", *(*objects.met2())["Final"], weight);
  }



  void ValidationHists ::
  fillHists (const std::string& prefix,
	     const xAOD::IParticleContainer& particles,
	     float weight)
  {
    std::size_t count = 0;
    for (auto& particle : particles)
    {
      if (m_selectionAccessor (*particle))
      {
	fillHist (prefix + "_e", 100, 0, 1e6, particle->e(), weight);
	fillHist (prefix + "_pt", 100, 0, 1e6, particle->pt(), weight);
	fillHist (prefix + "_eta", 100, -5, 5, particle->eta(), weight);
	fillHist (prefix + "_phi", 100, -M_PI, M_PI, particle->phi(), weight);
	fillHist (prefix + "_weightln", 20, -5, 5, log (particle->auxdata<float> ("ana_weight")), 1);
	++ count;
      }
    }
    fillHist (prefix + "_num", 21, -0.5, 20.5, count, weight);
  }



  void ValidationHists ::
  fillHists (const std::string& prefix,
	     const xAOD::MissingET& met, float weight)
  {
    fillHist (prefix + "_pt", 100, 0, 1e6, met.met(), weight);
    fillHist (prefix + "_phi", 100, -M_PI, M_PI, met.phi(), weight);
    fillHist (prefix + "_px", 100, -1e6, 1e6, met.mpx(), weight);
    fillHist (prefix + "_py", 100, -1e6, 1e6, met.mpy(), weight);
  }



  void ValidationHists ::
  fillHist (const std::string& name, unsigned nbins, float low,
	    float high, float value, float weight)
  {
    auto iter = m_hists.find (name);
    if (iter != m_hists.end())
    {
      iter->second->Fill (value, weight);
      return;
    }

    TH1 *hist = new TH1D (name.c_str(), name.c_str(), nbins, low, high);
    hist->Fill (value, weight);
    m_hists[name] = hist;
    m_worker->addOutput (hist);
  }
}

#endif
