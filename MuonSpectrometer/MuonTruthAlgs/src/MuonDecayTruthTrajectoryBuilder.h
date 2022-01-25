/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef MUONDECAYTRUTHTRAJECTORYBUILDER_H
#define MUONDECAYTRUTHTRAJECTORYBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AtlasHepMC/GenVertex.h"
#include "TrkToolInterfaces/ITruthTrajectoryBuilder.h"

namespace Muon {

    class MuonDecayTruthTrajectoryBuilder : virtual public Trk::ITruthTrajectoryBuilder, public AthAlgTool {
    public:
        MuonDecayTruthTrajectoryBuilder(const std::string& type, const std::string& name, const IInterface* parent);

       

        void buildTruthTrajectory(TruthTrajectory* result, HepMC::ConstGenParticlePtr input) const override;

        /** Returns an umambiguous mother of the truth particle on a TruthTrajectory, or 0.
         * Implemented via truthTrajectoryCuts().
         */
        HepMC::ConstGenParticlePtr getMother(HepMC::ConstGenParticlePtr particle) const override;

        /** Returns an umambiguous daughter of the truth particle on a TruthTrajectory, or 0.
         * This assumes no one mother can correspond to no more than one daughter.
         * Implemented via truthTrajectoryCuts().
         */
        HepMC::ConstGenParticlePtr getDaughter(HepMC::ConstGenParticlePtr particle) const override;

    private:
        /** Return type for the next method */
        typedef std::pair<HepMC::ConstGenParticlePtr, HepMC::ConstGenParticlePtr> MotherDaughter;

        /**
         * Decides if the vertex connects two particles on the same
         * TruthTrajectory.  Returns the (mother,daughter) pair,
         * where either of the two can be NULL.
         *
         * This interface implies that there is one-to-one correspondence
         * between mother and daughter particles; i.e. TruthTrajectories
         * are not allowed to branch.
         */
        MotherDaughter truthTrajectoryCuts(HepMC::ConstGenVertexPtr vtx) const;

        // mutable bool m_isDecayIntoTwoMuons; // Really don't think this does anything? EJWM.
    };

}  // namespace Muon

#endif
