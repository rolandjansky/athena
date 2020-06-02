/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetConstituentModifierTool.h"
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

  ATH_MSG_INFO("Initializing HIJetConstituentModifierTool");
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

   //save unsubtracted kinematics as moment if they don’t exist already...
   xAOD::IParticle::FourMom_t unsubtrP4;
   unsubtrP4 = jet.p4();
   jet.setJetP4(HIJetRec::unsubtractedJetState(),jet.jetP4());

   xAOD::IParticle::FourMom_t subtrP4;
   xAOD::JetFourMom_t jet4vec;
   //need to add usual safety checks on cluster container access
   SG::ReadHandle<xAOD::CaloClusterContainer> readHandleSubtractedClusters ( m_clusterKey );

   const xAOD::CaloClusterContainer* ccl=readHandleSubtractedClusters.get();

   for(auto index : cluster_indices)
   {
     auto cl=ccl->at(index);
     jet.addConstituent(cl);
     subtrP4+=cl->p4(HIJetRec::subtractedClusterState());
   }

   if(subtrP4.E()/std::cosh(subtrP4.Eta()) < E_min)
   {
     subtrP4=unsubtrP4;
     subtrP4*=1e-7;//ghost scale
   }
   jet4vec.SetCoordinates(subtrP4.Pt(),subtrP4.Eta(),subtrP4.Phi(),subtrP4.M());
   jet.setJetP4(jet4vec);

   return 0;
}
