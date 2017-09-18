// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: IsolationSelectionTool.h 704447 2015-10-29 12:00:39Z jdevivi $
#ifndef ISOLATIONSELECTION_ISOLATIONSELECTIONTOOL_H
#define ISOLATIONSELECTION_ISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IsolationWP.h"

// Forward declaration(s):
class TFile;
class TH1F;

class Interp3D;

namespace CP {

    /// Implementation of IIsolationSelectionTool
    ///
    ///
    /// @author John Morris <john.morris@cern.ch>
    /// @author Dongliang Zhang <dongliang.zhang@cern.ch>
    ///
    /// $Revision: 704447 $
    /// $Date: 2015-10-29 13:00:39 +0100 (Thu, 29 Oct 2015) $
    ///

    class IsolationSelectionTool: public virtual CP::IIsolationSelectionTool, public asg::AsgTool {

            /// Create a proper constructor for Athena
            ASG_TOOL_CLASS( IsolationSelectionTool , CP::IIsolationSelectionTool )

            public:
            /// Constructor for standalone usage, but please do use the interface and ToolHandle
            IsolationSelectionTool( const std::string& name );
            /// Destructor
            virtual ~IsolationSelectionTool();

            /// Function initialising the tool
            virtual StatusCode initialize();
            /// Function finalizing the tool
            virtual StatusCode finalize();

            enum IsoWPType {
                Efficiency, Cut
            };
            virtual const Root::TAccept& accept(const xAOD::Photon& x) const;
            virtual const Root::TAccept& accept(const xAOD::Electron& x) const;
            virtual const Root::TAccept& accept(const xAOD::Muon& x) const;
            virtual const Root::TAccept& accept(const strObj& x) const;
            virtual const Root::TAccept& accept(const xAOD::IParticle& x) const; // for tracks, and others?

            virtual const Root::TAccept& getPhotonTAccept() const;
            virtual const Root::TAccept& getElectronTAccept() const;
            virtual const Root::TAccept& getMuonTAccept() const;
            virtual const Root::TAccept& getObjTAccept() const;

            virtual const std::vector<IsolationWP*>& getMuonWPs() const;
            virtual const std::vector<IsolationWP*>& getElectronWPs() const;
            virtual const std::vector<IsolationWP*>& getPhotonWPs() const;
            virtual const std::vector<IsolationWP*>& getObjWPs() const;

            StatusCode addWP(std::string WP, xAOD::Type::ObjectType type);
            StatusCode addWP(IsolationWP* wp, xAOD::Type::ObjectType type);
            StatusCode addMuonWP(std::string wpname);
            StatusCode addPhotonWP(std::string wpname);
            StatusCode addElectronWP(std::string wpname);
            StatusCode addUserDefinedWP(std::string WPname, xAOD::Type::ObjectType ObjType, std::vector<std::pair<xAOD::Iso::IsolationType, std::string> >& cuts, std::string key = "", IsoWPType type = Efficiency);
            StatusCode setIParticleCutsFrom(xAOD::Type::ObjectType ObjType);
            void addCutToWP(IsolationWP* wp, std::string key, const xAOD::Iso::IsolationType t, const std::string expression);

            // Clearing, for very special use
            void clearPhotonWPs();
            void clearElectronWPs();
            void clearMuonWPs();
            void clearObjWPs();

        private:
            //same interface for xAOD::IParticle and StrObj -> use  template
            template<typename T> void evaluateWP(const T& x, const std::vector<IsolationWP*>& WP, Root::TAccept& accept) const;
            void clearWPs(std::vector<IsolationWP*>& WP);

            // Stupid ROOT
            TDirectory* getTemporaryDirectory(void) const;

            std::string m_muWPname;
            std::string m_elWPname;
            std::string m_phWPname;
            std::vector<std::string> m_muWPvec;
            std::vector<std::string> m_elWPvec;
            std::vector<std::string> m_phWPvec;
            std::string m_muWPKey;
            std::string m_elWPKey;
            std::string m_phWPKey;

            /// input file
            std::string m_calibFileName;
            std::shared_ptr<TFile> m_calibFile;

            /// internal use
            std::vector<IsolationWP*> m_muWPs;
            std::vector<IsolationWP*> m_elWPs;
            std::vector<IsolationWP*> m_phWPs;
            std::vector<IsolationWP*> m_objWPs;

            /// TAccept's
            mutable Root::TAccept m_photonAccept;
            mutable Root::TAccept m_electronAccept;
            mutable Root::TAccept m_muonAccept;
            mutable Root::TAccept m_objAccept;

            /// Iparticle interface
            std::vector<IsolationWP*>* m_iparWPs;
            mutable Root::TAccept* m_iparAccept;

            // for cut interpolation
            bool m_doInterpM, m_doInterpE;
            Interp3D *m_Interp;

            // Doc
            std::string m_TwikiLoc;

    };
} // namespace

#endif
