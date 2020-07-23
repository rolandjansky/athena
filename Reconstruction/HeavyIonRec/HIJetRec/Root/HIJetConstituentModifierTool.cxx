/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetConstituentModifierTool.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "JetRec/JetModifierBase.h"

#include "StoreGate/ReadHandle.h"

HIJetConstituentModifierTool::HIJetConstituentModifierTool(const std::string& myname): JetModifierBase(myname)
{
}

StatusCode HIJetConstituentModifierTool::initialize(){

  ATH_MSG_INFO("Initializing HIJetConstituentModifierTool w/ Cluster Key " << m_clusterKey.key() );
  ATH_CHECK( m_clusterKey.initialize() );
  return StatusCode::SUCCESS;

}

int HIJetConstituentModifierTool::modifyJet(xAOD::Jet& jet) const {

    float E_min=m_subtractorTool->minEnergyForMoments();
    const xAOD::JetConstituentVector constituents = jet.getConstituents();
    std::vector<size_t> cluster_indices;
    cluster_indices.reserve(constituents.size());

    for (auto citer = constituents.begin(); citer != constituents.end(); ++citer)
    {
      cluster_indices.push_back(citer->rawConstituent()->index());
    }

    /// The accessor for the cluster element links
   static SG::AuxElement::Accessor< std::vector< ElementLink< xAOD::IParticleContainer > > >
     constituentAcc( "constituentLinks" );
   static SG::AuxElement::Accessor< std::vector< float> >
     constituentWeightAcc( "constituentWeights" );

   if( constituentAcc.isAvailable(jet) ) constituentAcc( jet ).resize(0);
   if( constituentWeightAcc.isAvailable(jet) ) constituentWeightAcc( jet ).resize(0);

   //save unsubtracted kinematics as moment if they don't exist already...
   xAOD::IParticle::FourMom_t unsubtractedP4;
   unsubtractedP4 = jet.p4();
   jet.setJetP4(HIJetRec::unsubtractedJetState(),jet.jetP4());

   xAOD::IParticle::FourMom_t subtractedP4;
   xAOD::IParticle::FourMom_t subtractedOriginCorrP4;
   xAOD::JetFourMom_t subtractedJet4vec;
   xAOD::JetFourMom_t subtractedOriginCorrJet4vec;
   //Cluster container access (from the e-gamma + cluster subtracted deep copy)
   SG::ReadHandle<xAOD::CaloClusterContainer> readHandleSubtractedClusters ( m_clusterKey );

   const xAOD::CaloClusterContainer* ccl=readHandleSubtractedClusters.get();

   for(auto index : cluster_indices)
   {
     const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(ccl->at(index));

     jet.addConstituent(cl);
     xAOD::IParticle::FourMom_t subtractedClusterP4;
     subtractedClusterP4=HIJetRec::getClusterP4( cl, HIJetRec::subtractedClusterState());
     subtractedP4+=subtractedClusterP4;
     ATH_MSG_DEBUG("Subracted Cluster #: " << cl->index() << " :: E: " << subtractedClusterP4.E() << " :: Eta: " << subtractedClusterP4.Eta() << " :: Phi: " << subtractedClusterP4.Phi());

     if( m_originCorrection ){
       xAOD::IParticle::FourMom_t subtractedOriginCorrClusterP4;
       subtractedOriginCorrClusterP4=HIJetRec::getClusterP4( cl, HIJetRec::subtractedOriginCorrectedClusterState());
       subtractedOriginCorrP4+=subtractedOriginCorrClusterP4;
       ATH_MSG_DEBUG("Subrtracted OC Cluster #: " << cl->index() << " :: E: " << subtractedOriginCorrClusterP4.E() << " :: Eta: " << subtractedOriginCorrClusterP4.Eta() << " :: Phi: " << subtractedOriginCorrClusterP4.Phi());
     }

   }

   //Check for subtracted Kinematics
   if(subtractedP4.E()/std::cosh(subtractedP4.Eta()) < E_min)
   {
     subtractedP4=unsubtractedP4;
     subtractedP4*=1e-7;//ghost scale
   }
   //Check for subtracted + Origin Correction Kinematics
   if(subtractedOriginCorrP4.E()/std::cosh(subtractedOriginCorrP4.Eta()) < E_min && m_originCorrection )
   {
     subtractedOriginCorrP4=unsubtractedP4;
     subtractedOriginCorrP4*=1e-7;//ghost scale
   }

   subtractedJet4vec.SetCoordinates(subtractedP4.Pt(),subtractedP4.Eta(),
                                    subtractedP4.Phi(),subtractedP4.M());
   jet.setJetP4(HIJetRec::subtractedJetState(), subtractedJet4vec);

   if( m_originCorrection ) {
     subtractedOriginCorrJet4vec.SetCoordinates(subtractedOriginCorrP4.Pt(),subtractedOriginCorrP4.Eta(),
                                                subtractedOriginCorrP4.Phi(),subtractedOriginCorrP4.M());
     jet.setJetP4(HIJetRec::subtractedOriginCorrectedJetState(), subtractedOriginCorrJet4vec);
     jet.setJetP4(subtractedOriginCorrJet4vec);
     jet.setConstituentsSignalState(HIJetRec::subtractedOriginCorrectedConstitState());

   }
   else {
     jet.setJetP4(subtractedJet4vec);
     jet.setConstituentsSignalState(HIJetRec::subtractedConstitState());
   }

   return 0;
}
