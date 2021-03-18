// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H
#define ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"

// EDM include(s):
#include "xAODPrimitives/IsolationType.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
// #include "IsolationSelection/strMuon.h"

// std include(s):
#include <map>
#include <memory>

///
/// @author John Morris <john.morris@cern.ch>
///
/// $Revision: 764207 $
/// $Date: 2016-07-25 20:16:03 +0200 (Mon, 25 Jul 2016) $
///
/// @brief Select isolated Photons, Electrons and Muons
///
struct strObj;

namespace CP {
    class IsolationWP;
    class IIsolationSelectionTool: virtual public asg::IAsgTool {

            /// Declare the interface that the class provides
            ASG_TOOL_INTERFACE (CP::IIsolationSelectionTool)

        public:
            // Cut on object
            virtual asg::AcceptData accept(const xAOD::Photon& x) const = 0;
            virtual asg::AcceptData accept(const xAOD::Electron& x) const = 0;
            virtual asg::AcceptData accept(const xAOD::Muon& x) const = 0;
            virtual asg::AcceptData accept(const strObj& x) const = 0;
            virtual asg::AcceptData accept(const xAOD::IParticle& x) const = 0;
            virtual StatusCode setIParticleCutsFrom(xAOD::Type::ObjectType ObjType) = 0;

            virtual const asg::AcceptInfo& getPhotonAcceptInfo() const = 0;
            virtual const asg::AcceptInfo& getElectronAcceptInfo() const = 0;
            virtual const asg::AcceptInfo& getMuonAcceptInfo() const = 0;
            virtual const asg::AcceptInfo& getObjAcceptInfo() const = 0;

            virtual const std::vector<IsolationWP*>& getMuonWPs() const=0;
            virtual const std::vector<IsolationWP*>& getElectronWPs() const=0;
            virtual const std::vector<IsolationWP*>& getPhotonWPs() const=0;
            virtual const std::vector<IsolationWP*>& getObjWPs() const=0;

    };
} // end namespace
#endif
