/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IsolationSelection_IsolationCloseByCorrectionTool_H
#define IsolationSelection_IsolationCloseByCorrectionTool_H

#include <AsgTools/AsgTool.h>
#include <AsgTools/AnaToolHandle.h>

#include <IsolationSelection/IIsolationCloseByCorrectionTool.h>
#include <IsolationSelection/IsoVariableHelper.h>
#include <IsolationSelection/IsolationWP.h>
#include <IsolationSelection/Defs.h>

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
    
    //For tracks it does not matter whether we're using a set of a vector
    typedef std::set<const xAOD::TrackParticle*> TrackCollection;
    typedef std::vector<const xAOD::CaloCluster*> ClusterCollection;

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

            virtual float getOriginalIsolation(const xAOD::IParticle& P, IsoType type) const;
            virtual float getOriginalIsolation(const xAOD::IParticle*  particle, IsoType type) const;

        private:

            void isoTypesFromWP(const std::vector<IsolationWP*> &WP, IsoVector & types);
            //Helper function to check whether an element is in the vector
            template<typename T> bool isElementInList(const std::vector<T> &List, const T& Element) const;
            template<typename T> bool isElementInList(const std::set<T> &List, const T& Element) const;

            CorrectionCode performCloseByCorrection(xAOD::IParticleContainer* Particles, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const;
            CorrectionCode performCloseByCaloCorrection(xAOD::IParticleContainer* Cont1, xAOD::IParticleContainer* Cont2) const;

            const IsoVector* getIsolationTypes(const xAOD::IParticle*  particle) const;
            //Functions to  perfrom  the isolation correction  directly
            CorrectionCode subtractCloseByContribution(xAOD::IParticle* P, const IsoVector& types, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const;
            CorrectionCode getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle*  particlear, IsoType type, const TrackCollection& tracks) const;
            CorrectionCode getCloseByCorrectionTopoIso(float& correction, const xAOD::IParticle*  particlear, IsoType type, const ClusterCollection& clusters) const;
            CorrectionCode getCloseByCaloCorrection(float& correction, const xAOD::IParticle*  particlear, const xAOD::IParticleContainer* CloseByPars, IsoType type, int Model) const;

            //Retrieve the primary vertex
            const xAOD::Vertex* retrieveIDBestPrimaryVertex() const;

            void getExtrapEtaPhi(const xAOD::IParticle*  particlear, float& eta, float& phi) const;

            //Returns the Size of the Isolation cone
            double coneSize(const xAOD::IParticle*  particle, IsoType Cone) const;
            //Retrieves the uncalibrated pt from the particle
            double unCalibPt(const xAOD::IParticle*  particle) const;
            //Which particles shall actually be corrected
            bool passSelectionQuality(const xAOD::IParticle*  particle) const;
            bool considerForCorrection(const xAOD::IParticle*  particle) const;

            //Some helper functions for Overlap and DeltaR
            bool isSame(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1) const;
            bool overlap(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1, double dR) const;
            double deltaR2(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1, bool AvgCalo = false) const;

            float caloCorrectionFraction(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1, float ConeSize, int Model) const;
            float caloCorrectionFromDecorator(const xAOD::IParticle* ToCorrect, const xAOD::IParticle* CloseBy, float ConeSize, int Model) const;

            bool isFixedTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool isVarTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool isTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool isTopoEtIso(xAOD::Iso::IsolationType Iso) const;

            bool isEgamma(const xAOD::IParticle*  particle) const;

            const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle*  particle, bool force_id = false) const;
            const xAOD::IParticle* trackIsoRefPart(const xAOD::IParticle*  particle) const;

            TrackCollection getAssociatedTracks(const xAOD::IParticle*  particle) const;
            void getTrackCandidates(const xAOD::IParticleContainer* Container, const xAOD::Vertex* Vtx, TrackCollection &Tracks) const;

            const xAOD::IParticle* topoEtIsoRefPart(const xAOD::IParticle*  particle) const;
            const xAOD::CaloCluster* getCluster(const xAOD::IParticle*  particle) const;

            ClusterCollection getAssociatedClusters(const xAOD::IParticle*  particle) const;
            void getClusterCandidates(const xAOD::IParticleContainer* Container, ClusterCollection& Clusters) const;

            float clusterEtMinusTile(const xAOD::CaloCluster* C) const;

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

            std::string m_ToCorrect_name;

            SelectionAccessor m_acc_quality;
            SelectionAccessor m_acc_passOR;

            SelectionAccessor m_acc_ToCorrect;

            SelectionDecorator m_dec_isoselection;

            //Functionallity to backup the original cone variables if needed
            std::string m_backup_prefix;

            asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool;
            mutable IsoHelperMap m_isohelpers;

    };

}
#endif
