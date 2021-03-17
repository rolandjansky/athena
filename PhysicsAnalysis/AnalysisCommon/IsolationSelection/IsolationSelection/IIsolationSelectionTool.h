// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: IIsolationSelectionTool.h 764207 2016-07-25 18:16:03Z alesage $
// #ifndef ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H
// #define ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H
#ifndef ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H
#define ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

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
            virtual const Root::TAccept& accept(const xAOD::Photon& x) const = 0;
            virtual const Root::TAccept& accept(const xAOD::Electron& x) const = 0;
            virtual const Root::TAccept& accept(const xAOD::Muon& x) const = 0;
            virtual const Root::TAccept& accept(const strObj& x) const = 0;
            virtual const Root::TAccept& accept(const xAOD::IParticle& x) const = 0;
            virtual StatusCode setIParticleCutsFrom(xAOD::Type::ObjectType ObjType) = 0;

            virtual const Root::TAccept& getPhotonTAccept() const = 0;
            virtual const Root::TAccept& getElectronTAccept() const = 0;
            virtual const Root::TAccept& getMuonTAccept() const = 0;
            virtual const Root::TAccept& getObjTAccept() const = 0;

            virtual const std::vector<IsolationWP*>& getMuonWPs() const=0;
            virtual const std::vector<IsolationWP*>& getElectronWPs() const=0;
            virtual const std::vector<IsolationWP*>& getPhotonWPs() const=0;
            virtual const std::vector<IsolationWP*>& getObjWPs() const=0;

    };
} // end namespace
#endif
