/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_INDETCANDIDATE_H
#define MUONCOMBINEDEVENT_INDETCANDIDATE_H

#include <vector>

#include "AthLinks/ElementLink.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk {
    class CaloExtension;

}

namespace MuonCombined {

    class InDetCandidate {
    public:
        /** constructor taking a xAOD::TrackParicle&
            Users should ensure that the lifetime of the idTrack object is longer than the InDetCandidate
            as it internally caches a pointer to it.
         */
        InDetCandidate(const xAOD::TrackParticle& idTrack);

        /** constructor taking an ElementLink to a xAOD::TrackParicle&
            Users should ensure that the element link is valid and the lifetime of the idTrack object is longer than the InDetCandidate
            as it internally caches a pointer to it.
         */
        InDetCandidate(const ElementLink<xAOD::TrackParticleContainer>& idTrackLink);

        /** destructor */
        ~InDetCandidate();

        /** access TrackParticle */
        const xAOD::TrackParticle& indetTrackParticle() const;

        /** access TrackParticleLink */
        const ElementLink<xAOD::TrackParticleContainer>& indetTrackParticleLink() const;

        // access MuonSystemExtension
        const Muon::MuonSystemExtension* getExtension() const;
        // access the CaloExtension
        std::shared_ptr<Trk::CaloExtension> getCaloExtension() const;

        // set MuonSystemExtension, taking ownership
        void setExtension(std::unique_ptr<Muon::MuonSystemExtension> extension);
        /// set CaloExtension
        void setExtension(std::unique_ptr<Trk::CaloExtension> extension);
        void setExtension(std::shared_ptr<Trk::CaloExtension> extension);

        /** Returns true if this candidate was formed from a special far forward InDet track.*/
        bool isSiliconAssociated() const;

        /** Pass true if this candiate was created from a special far forward InDet track.*/
        void setSiliconAssociated(bool);

        /** print candidate to string */
        std::string toString() const;

    private:
        /** ElementLink to the InDet TrackParticle */
        ElementLink<xAOD::TrackParticleContainer> m_idTrackParticleLink;

        /** cached pointer to the InDet TrackParticle */
        const xAOD::TrackParticle* m_idTrackParticle{nullptr};

        /** Was this created using a special far forward indet track*/
        bool m_siAssociated{false};

        std::unique_ptr<Muon::MuonSystemExtension> m_extension;
        std::shared_ptr<Trk::CaloExtension> m_calo_extension;
    };

}  // namespace MuonCombined

#endif
