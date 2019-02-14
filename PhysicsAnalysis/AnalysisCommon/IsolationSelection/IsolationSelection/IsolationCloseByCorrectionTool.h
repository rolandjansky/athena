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
    class ITrackVertexAssociationTool;
    
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
            //Helper function to check whether an element is in the data container or not
            template<typename T> bool isElementInList(const std::vector<T> &List, const T& Element) const;
            template<typename T> bool isElementInList(const std::set<T> &List, const T& Element) const;

            //Function to pipe each container given by the interfaces through. It loops over all 
            //particles and removes the isolation overlap between the objects
            CorrectionCode performCloseByCorrection(xAOD::IParticleContainer* Particles, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters, const xAOD::Vertex* vtx) const;

            //Helper function to obtain the isolation cones to use for a given particle
            const IsoVector* getIsolationTypes(const xAOD::IParticle*  particle) const;
            
            
            //Functions to  perfrom  the isolation correction  directly
            CorrectionCode subtractCloseByContribution(xAOD::IParticle* P, const IsoVector& types, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters, const xAOD::Vertex* vtx) const;
            CorrectionCode getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle*  particlear, IsoType type, const TrackCollection& tracks, const xAOD::Vertex* vtx) const;
            CorrectionCode getCloseByCorrectionTopoIso(float& correction, const xAOD::IParticle*  particlear, IsoType type, const ClusterCollection& clusters) const;
            CorrectionCode getCloseByCaloCorrection(float& correction, const xAOD::IParticle*  particlear, const xAOD::IParticleContainer* CloseByPars, IsoType type, int Model) const;

            //Retrieve the primary vertex
            const xAOD::Vertex* retrieveIDBestPrimaryVertex() const;

            //Returns the Size of the Isolation cone
            double coneSize(const xAOD::IParticle*  particle, IsoType Cone) const;
            //Retrieves the uncalibrated pt from the particle
            double unCalibPt(const xAOD::IParticle*  particle) const;
            
            //Clusters and tracks of particles surviving the selection quality 
            //are considered for corrections
            bool passSelectionQuality(const xAOD::IParticle*  particle) const;
            
            // Optionally the user can parse another set of particles
            // whose variables should be corrected but their clusters
            // & tracks do not contribute
            bool considerForCorrection(const xAOD::IParticle*  particle) const;
     
        public:
            // Extrapolated phi eta needed for proper dR of the muons
            void getExtrapEtaPhi(const xAOD::IParticle*  particlear, float& eta, float& phi) const;
            void calcExtrapEtaPhi(const xAOD::IParticle*  particlear, float& eta, float& phi) const;
            
            //Some helper functions for Overlap and DeltaR
            bool isSame(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1) const;
            bool overlap(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1, double dR) const;
            double deltaR2(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1, bool AvgCalo = false) const;

            float caloCorrectionFraction(const xAOD::IParticle*  particle, const xAOD::IParticle*  particle1, float ConeSize, int Model) const;
            float caloCorrectionFromDecorator(const xAOD::IParticle* ToCorrect, const xAOD::IParticle* CloseBy, float ConeSize, int Model) const;
            
            // Fixed cone size isolation variables
            bool isFixedTrackIso(xAOD::Iso::IsolationType type) const;
            // Any trackisolation variable with variable con size
            bool isVarTrackIso(xAOD::Iso::IsolationType type) const;
                        
            bool isFixedTrackIsoTTVA(xAOD::Iso::IsolationType type) const;
            // PtVarcones of the pile-up robust isolation variables
            bool isVarTrackIsoTTVA(xAOD::Iso::IsolationType Iso) const;
            //  Any track isolation variable
            bool isTrackIso(xAOD::Iso::IsolationType type) const;
            // Pileup robust track isolation variables
            bool isTrackIsoTTVA(xAOD::Iso::IsolationType type) const;
            // The pile-up robust isolation cones only accept
            // tracks with a minimum pt requirement
            float trackPtCut(xAOD::Iso::IsolationType type) const;
            
            bool isTopoEtIso(xAOD::Iso::IsolationType type) const;
            
            bool isPFlowIso(xAOD::Iso::IsolationType type) const;
            
            bool isEgamma(const xAOD::IParticle* particle) const;

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
            std::string particleName(xAOD::Type::ObjectType T) const;
            void printIsolationCones(const IsoVector& types, xAOD::Type::ObjectType T) const;
            
       private:
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
            // bool m_isCoreSubtracted;
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

            BoolAccessor m_chk_assocEtaPhi;
            FloatAccessor m_acc_assocEta;
            FloatAccessor m_acc_assocPhi;

            BoolDecorator m_dec_assocEtaPhi;
            FloatDecorator m_dec_assocEta;
            FloatDecorator m_dec_assocPhi;

            //Functionallity to backup the original cone variables if needed
            std::string m_backup_prefix;

            asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool;
            asg::AnaToolHandle<CP::ITrackVertexAssociationTool> m_ttvaTool;
            mutable IsoHelperMap m_isohelpers;
    };

}
#endif
