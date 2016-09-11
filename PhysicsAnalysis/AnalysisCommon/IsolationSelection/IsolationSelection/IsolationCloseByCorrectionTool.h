/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IsolationSelection_IsolationCloseByCorrectionTool_H
#define IsolationSelection_IsolationCloseByCorrectionTool_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"

#include "xAODTracking/VertexContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
// #include "ParticlesInConeTools/ITrackParticlesInConeTool.h" // TO BE ADDED AT SOME POINT!!!

namespace CP {

    class IsolationCloseByCorrectionTool : public asg::AsgTool,
        public virtual IIsolationCloseByCorrectionTool {
        
    ASG_TOOL_CLASS(IsolationCloseByCorrectionTool, IIsolationCloseByCorrectionTool)
    
    public:
        IsolationCloseByCorrectionTool (const std::string& name);
        virtual ~IsolationCloseByCorrectionTool(){};
  
        virtual StatusCode initialize();
        virtual StatusCode finalize();

        CP::CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;

        virtual Root::TAccept& acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1);
        
        ToolHandle<CP::IIsolationSelectionTool> m_selectorTool;
          
    private:
        const xAOD::IParticle* getReferenceParticle(const xAOD::IParticle& par) const;
        CP::CorrectionCode getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar) const;
        CP::CorrectionCode getCloseByCorrectionTopoetcone(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;
        const xAOD::Vertex* retrieveIDBestPrimaryVertex() const;
        CP::CorrectionCode getExtrapEtaPhi(const xAOD::IParticle& par, float& eta, float& phi) const;
        xAOD::Iso::IsolationType getIsolationTypeFromString(const TString& isoTypeString, int particleType);
  
        inline float phiInRange(float phi) const { 
            while (phi >= M_PI) phi -= 2*M_PI;
            while (phi < -M_PI) phi += 2*M_PI;
            return phi;
        }
  
        inline float getMax(float f1, float f2) const { 
            if(f1 >= f2) {
                return f1;
            } else {
                return f2;
            }
        }
  
        inline float getMin(float f1, float f2) const { 
            if(f1 <= f2) {
                return f1;
            } else {
                return f2;
            }
        }
  
        const xAOD::TrackParticleContainer* retrieveTrackParticleContainer() const;
  
        CP::CorrectionCode getparticlesInCone(float eta, float phi, float dr, std::vector< const xAOD::TrackParticle*>& output) const; // TO BE REMOVED AT SOME POINT!!!

        float m_coreCone;
        float m_ptvarconeRadius;
        mutable Root::TAccept m_accept;
        bool m_isInitialised;
        bool m_isCoreSubtracted;
        std::string m_indetTrackParticleLocation;
        std::vector<xAOD::Iso::IsolationType> m_muon_isoTypes;
        std::vector<xAOD::Iso::IsolationType> m_electron_isoTypes;
        std::vector<xAOD::Iso::IsolationType> m_photon_isoTypes;
        bool m_muon_isoTypesSet;
        bool m_electron_isoTypesSet;
        bool m_photon_isoTypesSet;
        
	asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool;
//         ToolHandle<xAOD::ITrackParticlesInConeTool> m_tracksInConeTool; // TO BE ADDED AT SOME POINT!!!

    };
}
#endif
