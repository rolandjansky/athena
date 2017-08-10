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

namespace CP {

    class IsolationCloseByCorrectionTool : public asg::AsgTool,
        public virtual IIsolationCloseByCorrectionTool {
        
    ASG_TOOL_CLASS(IsolationCloseByCorrectionTool, IIsolationCloseByCorrectionTool)
    
    public:
        IsolationCloseByCorrectionTool (const std::string& name);
        virtual ~IsolationCloseByCorrectionTool(){};
  
        virtual StatusCode initialize();
        virtual StatusCode finalize();

        virtual const CP::CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;

        virtual const Root::TAccept& acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;
          
    private:
        const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle& par) const;
        const CP::CorrectionCode getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar) const;
        const CP::CorrectionCode getCloseByCorrectionTopoetcone(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;
        const xAOD::Vertex* retrieveIDBestPrimaryVertex() const;
        const CP::CorrectionCode getExtrapEtaPhi(const xAOD::IParticle& par, float& eta, float& phi) const;
        xAOD::Iso::IsolationType getIsolationTypeFromString(const TString& isoTypeString, xAOD::Type::ObjectType particleType) const;
  
        static constexpr float m_2PI = 2*M_PI;
        inline float phiInRange(float phi) const { 
            while (phi >= M_PI) phi -= m_2PI;
            while (phi < -M_PI) phi += m_2PI;
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
  
        ToolHandle<CP::IIsolationSelectionTool> m_selectorTool;
        float m_coreCone;
        float m_ptvarconeRadius;
        mutable Root::TAccept m_accept;
        bool m_isInitialised;
        bool m_isCoreSubtracted;
        std::string m_indetTrackParticleLocation;
        std::vector<xAOD::Iso::IsolationType> m_muon_isoTypes;
        std::vector<xAOD::Iso::IsolationType> m_electron_isoTypes;
        std::vector<xAOD::Iso::IsolationType> m_photon_isoTypes;
        
	    asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool;

    };
}
#endif
