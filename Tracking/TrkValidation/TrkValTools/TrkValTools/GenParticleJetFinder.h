/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// GenParticleJetFinder.h
//   Header file for PrimaryTruthClassification tool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVALTOOLS_GENPARTICLEJETFINDER_H
#define TRKVALTOOLS_GENPARTICLEJETFINDER_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/IGenParticleJetFinder.h"

namespace Trk {

/** @class GenParticleJetFinder
    @brief Durham jet clustering algorithm for MC-truth tracks.

    ycut value is a job option

    @author Loek Hooft van Huysduynen,
            Wolfgang Liebig
*/
  class GenParticleJetFinder : virtual public IGenParticleJetFinder,
    public AthAlgTool {
  public:

      GenParticleJetFinder(const std::string& type, const std::string& name,
                                     const IInterface* parent);
      ~GenParticleJetFinder() {}
      virtual StatusCode initialize();
      virtual StatusCode finalize();

      /** main method returning vector of truth-jets */
      virtual std::vector< Trk::GenParticleJet >*  jetMCFinder
        (std::vector <HepMC::ConstGenParticlePtr> & ) const;

  private:
      float           m_yijCut; // the Durham ycut parameter

  };

} // end namespace
#endif
