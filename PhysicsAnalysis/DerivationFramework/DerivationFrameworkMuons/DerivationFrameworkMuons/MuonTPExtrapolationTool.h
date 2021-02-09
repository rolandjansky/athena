/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MuonTPExtrapolationTool_H
#define MuonTPExtrapolationTool_H

#include "DerivationFrameworkMuons/IMuonTPExtrapolationTool.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include <xAODTracking/TrackParticle.h>

class MuonTPExtrapolationTool: public asg::AsgTool, virtual public IMuonTPExtrapolationTool {
        ASG_TOOL_CLASS(MuonTPExtrapolationTool, IMuonTPExtrapolationTool)

        public:
        MuonTPExtrapolationTool(const std::string& myname);
        virtual ~MuonTPExtrapolationTool()=default;

        virtual StatusCode initialize() override;

        /// compute dR on trigger pivot plane
        /// see interface class for full description
        double dROnTriggerPivotPlane(const xAOD::Muon& tag, const xAOD::IParticle* probe) const override;

        // this method is intended for use in the DAOD production. It takes only one particle as argument, and handles the decoration.
         StatusCode decoratePivotPlaneCoords(const xAOD::IParticle* particle)const override;

    private:
        /// run the extrapolation - only available in full athena
        std::unique_ptr<const Trk::TrackParameters> extrapolateToTriggerPivotPlane(const xAOD::TrackParticle& track) const;

        // Utility method to handle extrapolation and decoration for one TrackParticle.
        // In AthAnalysis, it attempts to read the decoration that should be present in the DAODs we use (MUON1/2).
        // In full athena, it looks for the decoration, and, if it is missing, runs track extrapolation, decorating the result
        // to the particle to avoid repeating the process unnecessarily.
        // Returns success (true) or failure (false) of the procedure, fills eta and phi coordinates via reference
        // If the extrapolation fails or the decoration is missing in AthAnalysis, it will *not* change eta and phi
        // So you can set them to defaults before calling this guy, and they will be preserved in case of failure.
        bool extrapolateAndDecorateTrackParticle(const xAOD::TrackParticle* particle, float & eta, float & phi) const ;
   
        ToolHandle<Trk::IExtrapolator> m_extrapolator;
        /// Checks whether the extrapolation has been applied on the probe or nt
        bool is_extrapolated(const xAOD::IParticle* probe) const;


        // utility method: Obtains the track particle which we want to extrapolate into the MS.
        // Works for all kinds of probes.
        const xAOD::TrackParticle* getPreferredTrackParticle(const xAOD::IParticle* probe) const;

        // these define the surfaces that we extrapolate to.
        // We approximate the pivot plane in the form of a cylinder surface and two disks
        float m_endcapPivotPlaneZ;
        float m_endcapPivotPlaneMinimumRadius;
        float m_endcapPivotPlaneMaximumRadius;
        float m_barrelPivotPlaneRadius;
        float m_barrelPivotPlaneHalfLength;

        bool m_is_on_DAOD;

};

#endif
