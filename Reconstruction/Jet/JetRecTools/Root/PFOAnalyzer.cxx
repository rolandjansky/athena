#include <vector>

#include "JetRecTools/PFOAnalyzer.h"
#include "xAODBase/IParticleHelpers.h"

#include "MCTruthClassifier/MCTruthClassifier.h"

using namespace std;

PFOAnalyzer::PFOAnalyzer(const std::string& name) : JetConstituentModifierBase(name)
{
#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

declareProperty("JetCollection",   m_jetCollectionName="",    "Name of collection of jets for matching.");

}

StatusCode PFOAnalyzer::process(xAOD::IParticleContainer* cont) const {
  xAOD::PFOContainer* pfoCont = dynamic_cast<xAOD::PFOContainer*> (cont);
  if(pfoCont) return process(pfoCont);
  else{
    ATH_MSG_ERROR("Unable to dynamic cast IParticleContainer to PFOContainer");
    return StatusCode::FAILURE;
  }
}

StatusCode PFOAnalyzer::process(xAOD::PFOContainer* cont) const {

  const xAOD::JetContainer* jetCollection;
  if (m_jetCollectionName!="")
    ATH_CHECK( evtStore()->retrieve(jetCollection, m_jetCollectionName) );
  SG::AuxElement::Accessor<bool> jetMatchedAcc("matchedToJet");

  SG::AuxElement::Accessor<int> typeAcc("truthType");
  SG::AuxElement::Accessor<int> originAcc("truthOrigin");

  SG::AuxElement::Accessor<bool> PVMatchedAcc("matchedToPV");

  SG::AuxElement::Accessor<float> nearestNeighborDRAcc("nearestNeighborDR");
  SG::AuxElement::Accessor<float> nearestChargedNeighborDRAcc("nearestChargedNeighborDR");
  SG::AuxElement::Accessor<float> nearestChargedPVNeighborDRAcc("nearestChargedPVNeighborDR");
  SG::AuxElement::Accessor<float> nearestChargedPUNeighborDRAcc("nearestChargedPUNeighborDR");
  SG::AuxElement::Accessor<float> nearestNeutralNeighborDRAcc("nearestNeutralNeighborDR");

  SG::AuxElement::Accessor<int> nNearNeighborsAcc("nNearNeighbors");
  SG::AuxElement::Accessor<int> nNearChargedNeighborsAcc("nNearChargedNeighbors");
  SG::AuxElement::Accessor<int> nNearChargedPVNeighborsAcc("nNearChargedPVNeighbors");
  SG::AuxElement::Accessor<int> nNearChargedPUNeighborsAcc("nNearChargedPUNeighbors");
  SG::AuxElement::Accessor<int> nNearNeutralNeighborsAcc("nNearNeutralNeighbors");

  //const xAOD::TruthParticleContainer* truthParticles = 0;
  //ATH_CHECK( evtStore()->retrieve(truthParticles, "TruthParticles") );
  ToolHandle<IMCTruthClassifier> truthClassifier("MCTruthClassifier");


  for ( xAOD::PFO* thisPFO : *cont ) {

    if (!PVMatchedAcc.isAvailable(*thisPFO)){
      ATH_MSG_ERROR("Not known if PFO is matched to primary vertex.  Run CorrectPFOTool before PFOAnalyzer");
        return StatusCode::FAILURE;
    }


    if(thisPFO->charge()!=0){
      const xAOD::TrackParticle* track=thisPFO->track(0);
      if (track){
        std::pair<unsigned int, unsigned int> res;
        res=truthClassifier->particleTruthClassifier(track);
        typeAcc(*thisPFO)=res.first;
        originAcc(*thisPFO)=res.second;
      }
    }
    else{
      typeAcc(*thisPFO)=-1;
      originAcc(*thisPFO)=-1;

    }
    
    bool jetMatched=false;
    if (m_jetCollectionName!=""){

      for (const xAOD::Jet* jet: *jetCollection){
        if(jet->p4().DeltaR(thisPFO->p4()) < 0.4){
          jetMatched=true;
          break;
        }
      }
    }
    jetMatchedAcc(*thisPFO)=jetMatched;

    /*
    //----------------------------------------------------------
    //Truth matching

    float dRMin=99999;
    xAOD::TruthParticle* match = 0;
    for ( xAOD::TruthParticle* p : *truthParticles){
      if ( p.isCharged())
      float dR=thisPFO->p4().DeltaR(p->p4());
      if ( dR<dRMin ){
        dRMin=dR;
        match=p;
      }
    }
    */
    //----------------------------------------------------------

    float nearestNeighborDR=999;
    float nearestChargedNeighborDR=999;
    float nearestChargedPVNeighborDR=999;
    float nearestChargedPUNeighborDR=999;
    float nearestNeutralNeighborDR=999;

    int nNearNeighbors=0;
    int nNearChargedNeighbors=0;
    int nNearChargedPVNeighbors=0;
    int nNearChargedPUNeighbors=0;
    int nNearNeutralNeighbors=0;

    for ( xAOD::PFO* otherPFO : *cont ) {

      if (otherPFO->pt()<=0) continue;
      if (otherPFO->pt()==thisPFO->pt()) continue;

      //----------------------------------------------------------

      float dR=thisPFO->p4().DeltaR(otherPFO->p4());
      //if (0.001<dR && dR<0.3){
      if (dR<0.3){
        nNearNeighbors++;
        if (otherPFO->charge()==0) nNearNeutralNeighbors++;
        else{
          if (PVMatchedAcc(*otherPFO))nNearChargedPVNeighbors++;
          else nNearChargedPUNeighbors++;
        }
      }

      if(dR<nearestNeighborDR) nearestNeighborDR=dR;
      if (otherPFO->charge()==0){
        if (dR<nearestNeutralNeighborDR) nearestNeutralNeighborDR=dR;
      }
      else{
        if (PVMatchedAcc(*otherPFO)){
          if (dR<nearestChargedPVNeighborDR) nearestChargedPVNeighborDR=dR;
        }
        else{
          if (dR<nearestChargedPUNeighborDR) nearestChargedPUNeighborDR=dR;
        }
      }
    
    nearestNeighborDRAcc(*thisPFO)=nearestNeighborDR;
    nearestChargedNeighborDRAcc(*thisPFO)=nearestChargedNeighborDR;
    nearestChargedPVNeighborDRAcc(*thisPFO)=nearestChargedPVNeighborDR;
    nearestChargedPUNeighborDRAcc(*thisPFO)=nearestChargedPUNeighborDR;
    nearestNeutralNeighborDRAcc(*thisPFO)=nearestNeutralNeighborDR;

    nNearNeighborsAcc(*thisPFO)=nNearNeighbors;
    nNearChargedNeighborsAcc(*thisPFO)=nNearChargedNeighbors;
    nNearChargedPVNeighborsAcc(*thisPFO)=nNearChargedPVNeighbors;
    nNearChargedPUNeighborsAcc(*thisPFO)=nNearChargedPUNeighbors;
    nNearNeutralNeighborsAcc(*thisPFO)=nNearNeutralNeighbors;
  	
    }
  }
  return StatusCode::SUCCESS;
}