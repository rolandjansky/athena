/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_IISOLATIONHELPERTOOL_H
#define ISOLATIONTOOL_IISOLATIONHELPERTOOL_H

#include "AsgTools/AsgTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"
#include "xAODPrimitives/IsolationType.h"
#include <xAODPrimitives/IsolationCorrection.h>
#include <xAODPrimitives/IsolationCorrectionHelper.h>
#include <xAODBase/IParticle.h>
#include <xAODEventShape/EventShape.h>
#include "xAODCore/ShallowCopy.h"
#include "xAODMuon/MuonContainer.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#ifdef XAOD_ANALYSIS
#include "IsolationSelection/IsolationSelectionTool.h"
#endif // XAOD_ANALYSIS

struct strObj;

namespace CP
{
    class IIsolationHelper : public virtual asg::IAsgTool {
   
        ASG_TOOL_INTERFACE( CP::IIsolationHelper )
        
        public: 
            virtual bool isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type) const = 0;
            virtual bool isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const = 0;
            virtual bool correctionBitset(xAOD::Iso::IsolationCaloCorrectionBitset& mask, const xAOD::IParticle& par, xAOD::Iso::IsolationType type) const = 0;
            virtual bool isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<xAOD::Iso::IsolationCaloCorrection>& corrs) const = 0;
            virtual bool correction(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const = 0;
            virtual bool getPileupCorrection(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const = 0;
            virtual bool getPileupCorrection(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, float coreArea) const = 0;
            virtual bool updateIsolation(xAOD::MuonContainer*& copy,xAOD::ShallowAuxContainer*& copyaux, std::vector<xAOD::Iso::IsolationType>& types, xAOD::Iso::IsolationCaloCorrectionBitset corrMask, std::string muonkey, bool recordSG) const = 0;
            virtual bool removeOverlap(std::vector<float>& results, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, float trackRadiusMin = 0.0, float caloRadiusMin = 0.0, int topoetconeModel = -1, const xAOD::Vertex* vertex = 0, const xAOD::CaloClusterContainer* topoClusters = 0) const = 0;  
//#ifdef ROOTCORE
#ifdef XAOD_ANALYSIS
            virtual const Root::TAccept& acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, const std::vector<xAOD::Iso::IsolationType>& isoTypes, float trackRadiusMin = 0.0, float caloRadiusMin = 0.0, int topoetconeModel = -1, const xAOD::Vertex* vertex = 0, const xAOD::CaloClusterContainer* topoClusters = 0) const = 0; 
            virtual const Root::TAccept& acceptCorrected(const xAOD::IParticle& x) const = 0; 
#ifdef XAOD_STANDALONE
            virtual bool setSelectorProperty(const char* field, const char* property) = 0;
#endif // XAOD_STANDALONE
#endif // XAOD_ANALYSIS
//#endif
    };
  
}
#endif
