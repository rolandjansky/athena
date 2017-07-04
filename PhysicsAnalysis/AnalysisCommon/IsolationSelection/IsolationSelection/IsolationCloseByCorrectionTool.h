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
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include <vector>
#include <set>
#include <memory>
#include <map>
#include <string>

namespace InDet {
    class IInDetTrackSelectionTool;
}
namespace CP {
    class IsolationWP;
    class IsoVariableHelper;

    typedef SG::AuxElement::ConstAccessor<char> CharAccessor;
    typedef SG::AuxElement::ConstAccessor<float> FloatAccessor;

    typedef SG::AuxElement::Decorator<char> CharDecorator;
    typedef SG::AuxElement::Decorator<float> FloatDecorator;

    //For tracks it does not matter whether we're using a set of a vector
    typedef std::set<const xAOD::TrackParticle*> TrackCollection;

    typedef std::vector<const xAOD::CaloCluster*> ClusterCollection;

    typedef std::unique_ptr<CharAccessor> SelectionAccessor;
    typedef std::unique_ptr<CharDecorator> SelectionDecorator;

    typedef std::unique_ptr<IsoVariableHelper> IsoHelperPtr;
    typedef std::map<IsoType, IsoHelperPtr> IsoHelperMap;
    typedef std::pair<IsoType, IsoHelperPtr> IsoHelperPair;

    class IsolationCloseByCorrectionTool: public asg::AsgTool, public virtual IIsolationCloseByCorrectionTool {

        public:
            IsolationCloseByCorrectionTool(const std::string& name);
            //Proper constructor for athena
            ASG_TOOL_CLASS(IsolationCloseByCorrectionTool, IIsolationCloseByCorrectionTool)

            virtual StatusCode initialize();

            virtual CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const;
            virtual CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const xAOD::IParticleContainer& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const;

            virtual Root::TAccept acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const;
            virtual Root::TAccept acceptCorrected(const xAOD::IParticle& x, const xAOD::IParticleContainer& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const;

            virtual CorrectionCode getCloseByIsoCorrection(xAOD::ElectronContainer* Electrons = nullptr, xAOD::MuonContainer* Muons = nullptr, xAOD::PhotonContainer* Photons = nullptr, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const;
            virtual CorrectionCode subtractCloseByContribution(xAOD::IParticle& x, const xAOD::IParticleContainer& closebyPar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const;

            virtual float GetOriginalIsolation(const xAOD::IParticle& P, IsoType type) const;
            virtual float GetOriginalIsolation(const xAOD::IParticle* P, IsoType type) const;

        private:
            Root::TAccept passIsolationWP(const xAOD::IParticle *x) const;
            Root::TAccept passIsolationWP(const xAOD::IParticle& x) const;

            void IsoTypesFromWP(const std::vector<IsolationWP*> &WP, IsoVector & types);
            //Helper function to check whether an element is in the vector
            template<typename T> bool IsElementInList(const std::vector<T> &List, const T& Element) const;
            template<typename T> bool IsElementInList(const std::set<T> &List, const T& Element) const;

            CorrectionCode performCloseByCorrection(xAOD::IParticleContainer* Particles, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const;
            CorrectionCode performCloseByCaloCorrection(xAOD::IParticleContainer* Cont1, xAOD::IParticleContainer* Cont2) const;

            const IsoVector* getIsolationTypes(const xAOD::IParticle* P) const;
            //Functions to  perfrom  the isolation correction  directly
            CorrectionCode subtractCloseByContribution(xAOD::IParticle* P, const IsoVector& types, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const;
            CorrectionCode getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle* par, IsoType type, const TrackCollection& tracks) const;
            CorrectionCode getCloseByCorrectionTopoIso(float& correction, const xAOD::IParticle* par, IsoType type, const ClusterCollection& clusters) const;
            CorrectionCode getCloseByCaloCorrection(float& correction, const xAOD::IParticle* par, const xAOD::IParticleContainer* CloseByPars, IsoType type, int Model) const;

            //Retrieve the primary vertex
            const xAOD::Vertex* retrieveIDBestPrimaryVertex() const;

            void getExtrapEtaPhi(const xAOD::IParticle* par, float& eta, float& phi) const;

            //Returns the Size of the Isolation cone
            double ConeSize(const xAOD::IParticle* P, IsoType Cone) const;
            //Retrieves the uncalibrated pt from the particle
            double UnCalibPt(const xAOD::IParticle* P) const;

            //Some helper functions for Overlap and DeltaR
            bool IsSame(const xAOD::IParticle* P, const xAOD::IParticle* P1) const;
            bool Overlap(const xAOD::IParticle* P, const xAOD::IParticle* P1, double dR) const;
            double DeltaR2(const xAOD::IParticle* P, const xAOD::IParticle* P1, bool AvgCalo = false) const;

            float CaloCorrectionFraction(const xAOD::IParticle* P, const xAOD::IParticle* P1, float ConeSize, int Model) const;
            float CaloCorrectionFromDecorator(const xAOD::IParticle* ToCorrect, const xAOD::IParticle* CloseBy, float ConeSize, int Model) const;

            bool IsFixedTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool IsVarTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool IsTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool IsTopoEtIso(xAOD::Iso::IsolationType Iso) const;

            bool IsEgamma(const xAOD::IParticle* P) const;

            const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle* par) const;
            const xAOD::IParticle* TrackIsoRefPart(const xAOD::IParticle* P) const;

            TrackCollection GetAssociatedTracks(const xAOD::IParticle* P) const;
            void GetTrackCandidates(const xAOD::IParticleContainer* Container, const xAOD::Vertex* Vtx, TrackCollection &Tracks) const;

            const xAOD::IParticle* TopoEtIsoRefPart(const xAOD::IParticle* P) const;
            const xAOD::CaloCluster* Cluster(const xAOD::IParticle* P) const;

            ClusterCollection GetAssociatedClusters(const xAOD::IParticle* P) const;
            void GetClusterCandidates(const xAOD::IParticleContainer* Container, ClusterCollection& Clusters) const;

            float ClusterEtMinusTile(const xAOD::CaloCluster* C) const;
            bool ConsiderForCorrection(const xAOD::IParticle* P) const;

            std::string particleName(const xAOD::IParticle* C) const;

            ToolHandle<CP::IIsolationSelectionTool> m_selectorTool;

            float m_coreCone; //The core of the topoEt variables. Clusters within the core shall not be
                              //added to the isolation of the object itself. They are defined to be associated with it.

            float m_ptvarconeRadius; //Reference value to calculate the size of the mini-iso variables
                                     // dR = min (fixed , m_ptvarcone / particle->pt())

            float m_maxTopoPolution; // Upper limit on the energy fraction of the close-by cluster
                                     // to the isolation variable such that it is still subtracted from the cone.
                                     // Only considered if the particle to correct is not an Egamma object. Since the reference
                                     // position is extrapolated from the ID-track into the calorimeter.
                                     // Aim of the game -> Find out whether it might contributed
            float m_ConeSizeVariation; // Extend - shrink the cone size to account for extrapolation effects

            bool m_isInitialised;
            bool m_isCoreSubtracted;
            std::string m_indetTrackParticleLocation;
            std::string m_VertexContainerName;
            std::string m_CaloClusterContainerName;

            IsoVector m_muon_isoTypes;
            IsoVector m_electron_isoTypes;
            IsoVector m_photon_isoTypes;

            //Name of the isolation selection and input quality decorators
            std::string m_quality_name;
            std::string m_passOR_name;
            std::string m_isoSelection_name;

            SelectionAccessor m_acc_quality;
            SelectionAccessor m_acc_passOR;
            SelectionDecorator m_dec_isoselection;

            //Functionallity to backup the original cone variables if needed
            std::string m_backup_prefix;

            asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool;
            mutable IsoHelperMap m_isohelpers;

    };

    class IsoVariableHelper {
        public:
            CorrectionCode GetOrignalIsolation(const xAOD::IParticle* P, float& Value) const;
            CorrectionCode GetIsolation(const xAOD::IParticle* P, float& Value) const;
            CorrectionCode BackupIsolation(const xAOD::IParticle* P) const;
            CorrectionCode SetIsolation(xAOD::IParticle* P, float Value) const;
          
            IsoType isotype() const;
            std::string name() const;
            
            IsoVariableHelper(IsoType type, const std::string& BackupPreFix);

        private:

            IsoType m_IsoType;
            bool m_BackupIso;
            CharDecorator m_dec_IsoIsBackup;
            CharAccessor m_acc_IsoIsBackup;

            FloatAccessor m_acc_iso_variable;
            FloatDecorator m_dec_iso_variable;

            FloatAccessor m_acc_iso_backup;
            FloatDecorator m_dec_iso_backup;

    };
}
#endif
