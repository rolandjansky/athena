#include "DerivationFrameworkTop/ExKtbbAugmentation.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"


namespace DerivationFramework {


ExKtbbAugmentation::ExKtbbAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p)
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("LargeJetRadius",m_radius=0.8);
    declareProperty("LargeJetCollectionName",m_largeJetCollectionName="AntiKt8EMPFlowJets");
    declareProperty("SmallJetCollectionName",m_smallJetCollectionName="AntiKt4EMPFlowJets");
    declareProperty("isMC",m_isMC=false);
}



ExKtbbAugmentation::~ExKtbbAugmentation(){}



StatusCode ExKtbbAugmentation::initialize(){

  ATH_MSG_INFO("Initializing ExKtbbAugmentation tool... " );

  return StatusCode::SUCCESS;

}



StatusCode ExKtbbAugmentation::finalize(){

  return StatusCode::SUCCESS;

}



StatusCode ExKtbbAugmentation::addBranches() const{

  const xAOD::EventInfo* eventInfo;

  if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
    ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
    return StatusCode::FAILURE;
  }


  const xAOD::JetContainer* jets = evtStore()->retrieve< const xAOD::JetContainer >( m_largeJetCollectionName );
  const xAOD::JetContainer* smalljets = evtStore()->retrieve< const xAOD::JetContainer >( m_smallJetCollectionName );

  std::string algName = "ExKtbb";
  static SG::AuxElement::Decorator<double> jet_maxmv2(algName+"_MaxMV2c10");
  static SG::AuxElement::Decorator<double> jet_maxMV2r(algName+"_MaxMV2r");
  static SG::AuxElement::Decorator<double> jet_maxDL1_pb(algName+"_MaxDL1_pb");
  static SG::AuxElement::Decorator<double> jet_maxDL1_pc(algName+"_MaxDL1_pc");
  static SG::AuxElement::Decorator<double> jet_maxDL1_pu(algName+"_MaxDL1_pu");
  static SG::AuxElement::Decorator<double> jet_maxDL1r_pb(algName+"_MaxDL1r_pb");
  static SG::AuxElement::Decorator<double> jet_maxDL1r_pc(algName+"_MaxDL1r_pc");
  static SG::AuxElement::Decorator<double> jet_maxDL1r_pu(algName+"_MaxDL1r_pu");

  static SG::AuxElement::Decorator<double> jet_minmv2(algName+"_MinMV2c10");
  static SG::AuxElement::Decorator<double> jet_minMV2r(algName+"_MinMV2r");
  static SG::AuxElement::Decorator<double> jet_minDL1_pb(algName+"_MinDL1_pb");
  static SG::AuxElement::Decorator<double> jet_minDL1_pc(algName+"_MinDL1_pc");
  static SG::AuxElement::Decorator<double> jet_minDL1_pu(algName+"_MinDL1_pu");
  static SG::AuxElement::Decorator<double> jet_minDL1r_pb(algName+"_MinDL1r_pb");
  static SG::AuxElement::Decorator<double> jet_minDL1r_pc(algName+"_MinDL1r_pc");
  static SG::AuxElement::Decorator<double> jet_minDL1r_pu(algName+"_MinDL1r_pu");

  static SG::AuxElement::Decorator<double> jet_subDR(algName+"_SubjetDR");
  static SG::AuxElement::Decorator<double> jet_ptasym(algName+"_SubjetPtAsym");

  static SG::AuxElement::Decorator<int> jet_leadingPt_nTruthBHadrons(algName+"_leadingPtNBHadrons");
  static SG::AuxElement::Decorator<int> jet_leadingPt_nTruthCHadrons(algName+"_leadingPtNCHadrons");
  static SG::AuxElement::Decorator<int> jet_subleadingPt_nTruthBHadrons(algName+"_subleadingPtNBHadrons");
  static SG::AuxElement::Decorator<int> jet_subleadingPt_nTruthCHadrons(algName+"_subleadingPtNCHadrons");

  std::string algName3 = "ExKt3bb";
  static SG::AuxElement::Decorator<double> jet_exkt3_maxmv2(algName3+"_MaxMV2c10");
  static SG::AuxElement::Decorator<double> jet_exkt3_minmv2(algName3+"_MinMV2c10");

  static SG::AuxElement::Decorator<double> jet_exkt3_j1_MV2r(algName3+"_j1_MV2r");
  static SG::AuxElement::Decorator<double> jet_exkt3_j1_DL1_pb(algName3+"_j1_DL1_pb");
  static SG::AuxElement::Decorator<double> jet_exkt3_j1_DL1_pc(algName3+"_j1_DL1_pc");
  static SG::AuxElement::Decorator<double> jet_exkt3_j1_DL1_pu(algName3+"_j1_DL1_pu");
  static SG::AuxElement::Decorator<double> jet_exkt3_j1_DL1r_pb(algName3+"_j1_DL1r_pb");
  static SG::AuxElement::Decorator<double> jet_exkt3_j1_DL1r_pc(algName3+"_j1_DL1r_pc");
  static SG::AuxElement::Decorator<double> jet_exkt3_j1_DL1r_pu(algName3+"_j1_DL1r_pu");

  static SG::AuxElement::Decorator<double> jet_exkt3_j2_MV2r(algName3+"_j2_MV2r");
  static SG::AuxElement::Decorator<double> jet_exkt3_j2_DL1_pb(algName3+"_j2_DL1_pb");
  static SG::AuxElement::Decorator<double> jet_exkt3_j2_DL1_pc(algName3+"_j2_DL1_pc");
  static SG::AuxElement::Decorator<double> jet_exkt3_j2_DL1_pu(algName3+"_j2_DL1_pu");
  static SG::AuxElement::Decorator<double> jet_exkt3_j2_DL1r_pb(algName3+"_j2_DL1r_pb");
  static SG::AuxElement::Decorator<double> jet_exkt3_j2_DL1r_pc(algName3+"_j2_DL1r_pc");
  static SG::AuxElement::Decorator<double> jet_exkt3_j2_DL1r_pu(algName3+"_j2_DL1r_pu");

  static SG::AuxElement::Decorator<double> jet_exkt3_j3_MV2r(algName3+"_j3_MV2r");
  static SG::AuxElement::Decorator<double> jet_exkt3_j3_DL1_pb(algName3+"_j3_DL1_pb");
  static SG::AuxElement::Decorator<double> jet_exkt3_j3_DL1_pc(algName3+"_j3_DL1_pc");
  static SG::AuxElement::Decorator<double> jet_exkt3_j3_DL1_pu(algName3+"_j3_DL1_pu");
  static SG::AuxElement::Decorator<double> jet_exkt3_j3_DL1r_pb(algName3+"_j3_DL1r_pb");
  static SG::AuxElement::Decorator<double> jet_exkt3_j3_DL1r_pc(algName3+"_j3_DL1r_pc");
  static SG::AuxElement::Decorator<double> jet_exkt3_j3_DL1r_pu(algName3+"_j3_DL1r_pu");


  static SG::AuxElement::Decorator<double> jet_exkt3_subDR(algName3+"_SubjetDR");
  static SG::AuxElement::Decorator<double> jet_exkt3_ptasym(algName3+"_SubjetPtAsym");

  static SG::AuxElement::Decorator<int> jet_exkt3_leadingMV2c10_nTruthBHadrons(algName3+"_leadingMV2c10NBHadrons");
  static SG::AuxElement::Decorator<int> jet_exkt3_leadingMV2c10_nTruthCHadrons(algName3+"_leadingMV2c10NCHadrons");
  static SG::AuxElement::Decorator<int> jet_exkt3_subleadingMV2c10_nTruthBHadrons(algName3+"_subleadingMV2c10NBHadrons");
  static SG::AuxElement::Decorator<int> jet_exkt3_subleadingMV2c10_nTruthCHadrons(algName3+"_subleadingMV2c10NCHadrons");


  static SG::AuxElement::Decorator<int>    jet_largeJetLabel("LargeJetLabel");
  static SG::AuxElement::Decorator<int>    jet_largeJetTruthNB("LargeJetTruthNB");
  static SG::AuxElement::Decorator<int>    jet_largeJetNAnti4("LargeJetNAnti4");
  static SG::AuxElement::Decorator<int>    jet_nTracks("LargeJetNTracks");

  for (unsigned j = 0; j <  smalljets->size() ; j++){

        jet_maxmv2(*(smalljets->at(j))) = -999;
        jet_maxMV2r(*(smalljets->at(j))) = -999;
        jet_maxDL1_pb(*(smalljets->at(j))) = -999;
        jet_maxDL1_pc(*(smalljets->at(j))) = -999;
        jet_maxDL1_pu(*(smalljets->at(j))) = -999;
        jet_maxDL1r_pb(*(smalljets->at(j))) = -999;
        jet_maxDL1r_pc(*(smalljets->at(j))) = -999;
        jet_maxDL1r_pu(*(smalljets->at(j))) = -999;

        jet_minmv2(*(smalljets->at(j))) = -999;
        jet_minMV2r(*(smalljets->at(j))) = -999;
        jet_minDL1_pb(*(smalljets->at(j))) = -999;
        jet_minDL1_pc(*(smalljets->at(j))) = -999;
        jet_minDL1_pu(*(smalljets->at(j))) = -999;
        jet_minDL1r_pb(*(smalljets->at(j))) = -999;
        jet_minDL1r_pc(*(smalljets->at(j))) = -999;
        jet_minDL1r_pu(*(smalljets->at(j))) = -999;

        jet_subDR(*(smalljets->at(j))) = -999;
        jet_ptasym(*(smalljets->at(j))) = -999;

        jet_exkt3_maxmv2(*(smalljets->at(j))) = -999;
        jet_exkt3_minmv2(*(smalljets->at(j))) = -999;

        jet_exkt3_j1_MV2r(*(smalljets->at(j))) = -999;
        jet_exkt3_j1_DL1_pb(*(smalljets->at(j))) = -999;
        jet_exkt3_j1_DL1_pc(*(smalljets->at(j))) = -999;
        jet_exkt3_j1_DL1_pu(*(smalljets->at(j))) = -999;
        jet_exkt3_j1_DL1r_pb(*(smalljets->at(j))) = -999;
        jet_exkt3_j1_DL1r_pc(*(smalljets->at(j))) = -999;
        jet_exkt3_j1_DL1r_pu(*(smalljets->at(j))) = -999;

        jet_exkt3_j2_MV2r(*(smalljets->at(j))) = -999;
        jet_exkt3_j2_DL1_pb(*(smalljets->at(j))) = -999;
        jet_exkt3_j2_DL1_pc(*(smalljets->at(j))) = -999;
        jet_exkt3_j2_DL1_pu(*(smalljets->at(j))) = -999;
        jet_exkt3_j2_DL1r_pb(*(smalljets->at(j))) = -999;
        jet_exkt3_j2_DL1r_pc(*(smalljets->at(j))) = -999;
        jet_exkt3_j2_DL1r_pu(*(smalljets->at(j))) = -999;

        jet_exkt3_j3_MV2r(*(smalljets->at(j))) = -999;
        jet_exkt3_j3_DL1_pb(*(smalljets->at(j))) = -999;
        jet_exkt3_j3_DL1_pc(*(smalljets->at(j))) = -999;
        jet_exkt3_j3_DL1_pu(*(smalljets->at(j))) = -999;
        jet_exkt3_j3_DL1r_pb(*(smalljets->at(j))) = -999;
        jet_exkt3_j3_DL1r_pc(*(smalljets->at(j))) = -999;
        jet_exkt3_j3_DL1r_pu(*(smalljets->at(j))) = -999;

        jet_exkt3_subDR(*(smalljets->at(j))) = -999;
        jet_exkt3_ptasym(*(smalljets->at(j))) = -999;
        jet_largeJetLabel(*(smalljets->at(j))) = -1;
        jet_largeJetTruthNB(*(smalljets->at(j))) = -1;

        jet_leadingPt_nTruthBHadrons(*(smalljets->at(j))) = -1;
        jet_leadingPt_nTruthCHadrons(*(smalljets->at(j))) = -1;
        jet_subleadingPt_nTruthBHadrons(*(smalljets->at(j))) = -1;
        jet_subleadingPt_nTruthCHadrons(*(smalljets->at(j))) = -1;
        jet_exkt3_leadingMV2c10_nTruthBHadrons(*(smalljets->at(j))) = -1;
        jet_exkt3_leadingMV2c10_nTruthCHadrons(*(smalljets->at(j))) = -1;
        jet_exkt3_subleadingMV2c10_nTruthBHadrons(*(smalljets->at(j))) = -1;
        jet_exkt3_subleadingMV2c10_nTruthCHadrons(*(smalljets->at(j))) = -1;


  for (unsigned i = 0; i < jets->size() ; i++){

    double maxmv2 = -999;
    double maxMV2r = -999;
    double maxDL1_pb = -999;
    double maxDL1_pc = -999;
    double maxDL1_pu = -999;
    double maxDL1r_pb = -999;
    double maxDL1r_pc = -999;
    double maxDL1r_pu = -999;

    double minmv2 = -999;
    double minMV2r = -999;
    double minDL1_pb = -999;
    double minDL1_pc = -999;
    double minDL1_pu = -999;
    double minDL1r_pb = -999;
    double minDL1r_pc = -999;
    double minDL1r_pu = -999;

    double subDR = -999;
    double ptasym = -999;

    int leadingPt_nTruthBHadrons = -1;
    int leadingPt_nTruthCHadrons = -1;
    int subleadingPt_nTruthBHadrons = -1;
    int subleadingPt_nTruthCHadrons = -1;

    double exkt3_maxmv2 = -999;
    double exkt3_minmv2 = -999;

    double exkt3_j1_MV2r = -999;
    double exkt3_j1_DL1_pb = -999;
    double exkt3_j1_DL1_pc = -999;
    double exkt3_j1_DL1_pu = -999;
    double exkt3_j1_DL1r_pb = -999;
    double exkt3_j1_DL1r_pc = -999;
    double exkt3_j1_DL1r_pu = -999;

    double exkt3_j2_MV2r = -999;
    double exkt3_j2_DL1_pb = -999;
    double exkt3_j2_DL1_pc = -999;
    double exkt3_j2_DL1_pu = -999;
    double exkt3_j2_DL1r_pb = -999;
    double exkt3_j2_DL1r_pc = -999;
    double exkt3_j2_DL1r_pu = -999;

    double exkt3_j3_MV2r = -999;
    double exkt3_j3_DL1_pb = -999;
    double exkt3_j3_DL1_pc = -999;
    double exkt3_j3_DL1_pu = -999;
    double exkt3_j3_DL1r_pb = -999;
    double exkt3_j3_DL1r_pc = -999;
    double exkt3_j3_DL1r_pu = -999;

    double exkt3_subDR = -999;
    double exkt3_ptasym = -999;

    int exkt3_leadingMV2c10_nTruthBHadrons = -1;
    int exkt3_leadingMV2c10_nTruthCHadrons = -1;
    int exkt3_subleadingMV2c10_nTruthBHadrons = -1;
    int exkt3_subleadingMV2c10_nTruthCHadrons = -1;


    if(jets->at(i)->p4().DeltaR(smalljets->at(j)->p4()) < m_radius){


    std::vector<const xAOD::Jet*> ExKtSubjets;
    auto v_el = jets->at(i)->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >("ExKt2SubJets");
    for(auto el : v_el){
      if(!el.toTransient()){
        ATH_MSG_WARNING("Unable to reset element link in . You would crash soon ...");
      }

      if(!el.isValid()){
        ATH_MSG_ERROR("Invalid link to subjet through link !");
        return false;
      }
      else{
        auto subjet = dynamic_cast<const xAOD::Jet*>(*el);
        if(subjet == 0){
          ATH_MSG_ERROR("Empty ptr to subjet! You will crash soon...");
          return false;
        }
        else{
          ExKtSubjets.push_back(subjet);
        }
      }
    }

    if(ExKtSubjets.size() == 2){
      std::sort(ExKtSubjets.begin(), ExKtSubjets.end(),
        [](const xAOD::Jet* j1, const xAOD::Jet* j2){
          return (j1->pt() > j2->pt());
        }
      );
      if(m_isMC){
        leadingPt_nTruthBHadrons = ExKtSubjets.at(0)->auxdata<int>("GhostBHadronsFinalCount");
        leadingPt_nTruthCHadrons = ExKtSubjets.at(0)->auxdata<int>("GhostCHadronsFinalCount");
        subleadingPt_nTruthBHadrons = ExKtSubjets.at(1)->auxdata<int>("GhostBHadronsFinalCount");
        subleadingPt_nTruthCHadrons = ExKtSubjets.at(1)->auxdata<int>("GhostCHadronsFinalCount");
      }

      const xAOD::BTagging* bjet_LeadExKtSubJet = ExKtSubjets.at(0)->btagging();
      const xAOD::BTagging* bjet_SubLeadExKtSubJet = ExKtSubjets.at(1)->btagging();

      if( (!bjet_LeadExKtSubJet) || (!bjet_SubLeadExKtSubJet) ){
              std::cout << "Exclusive kt subjet is not well b-tagged!" << std::endl;
      }

      maxmv2    = std::max(bjet_LeadExKtSubJet->auxdata<double>("MV2c10_discriminant"),bjet_SubLeadExKtSubJet->auxdata<double>("MV2c10_discriminant"));
      maxMV2r    = std::max(bjet_LeadExKtSubJet->auxdata<double>("MV2r_discriminant"),bjet_SubLeadExKtSubJet->auxdata<double>("MV2r_discriminant"));
      maxDL1_pb    = std::max(bjet_LeadExKtSubJet->auxdata<double>("DL1_pb"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1_pb"));
      maxDL1_pc    = std::max(bjet_LeadExKtSubJet->auxdata<double>("DL1_pc"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1_pc"));
      maxDL1_pu    = std::max(bjet_LeadExKtSubJet->auxdata<double>("DL1_pu"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1_pu"));
      maxDL1r_pb    = std::max(bjet_LeadExKtSubJet->auxdata<double>("DL1r_pb"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1r_pb"));
      maxDL1r_pc    = std::max(bjet_LeadExKtSubJet->auxdata<double>("DL1r_pc"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1r_pc"));
      maxDL1r_pu    = std::max(bjet_LeadExKtSubJet->auxdata<double>("DL1r_pu"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1r_pu"));

      minmv2    = std::min(bjet_LeadExKtSubJet->auxdata<double>("MV2c10_discriminant"),bjet_SubLeadExKtSubJet->auxdata<double>("MV2c10_discriminant"));
      minMV2r    = std::min(bjet_LeadExKtSubJet->auxdata<double>("MV2r_discriminant"),bjet_SubLeadExKtSubJet->auxdata<double>("MV2r_discriminant"));
      minDL1_pb    = std::min(bjet_LeadExKtSubJet->auxdata<double>("DL1_pb"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1_pb"));
      minDL1_pc    = std::min(bjet_LeadExKtSubJet->auxdata<double>("DL1_pc"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1_pc"));
      minDL1_pu    = std::min(bjet_LeadExKtSubJet->auxdata<double>("DL1_pu"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1_pu"));
      minDL1r_pb    = std::min(bjet_LeadExKtSubJet->auxdata<double>("DL1r_pb"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1r_pb"));
      minDL1r_pc    = std::min(bjet_LeadExKtSubJet->auxdata<double>("DL1r_pc"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1r_pc"));
      minDL1r_pu    = std::min(bjet_LeadExKtSubJet->auxdata<double>("DL1r_pu"),bjet_SubLeadExKtSubJet->auxdata<double>("DL1r_pu"));

      subDR     = ExKtSubjets.at(0)->p4().DeltaR(ExKtSubjets.at(1)->p4());
      if(ExKtSubjets.at(0)->pt()+ExKtSubjets.at(1)->pt() > 0) ptasym =  fabs(ExKtSubjets.at(0)->pt()-ExKtSubjets.at(1)->pt())/(ExKtSubjets.at(0)->pt()+ExKtSubjets.at(1)->pt());
      else ptasym = -999;

    }

    std::vector<const xAOD::Jet*> ExKt3Subjets;
    auto v_el3 = jets->at(i)->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >("ExKt3SubJets");
    for(auto el : v_el3){
      if(!el.toTransient()){
        ATH_MSG_WARNING("Unable to reset element link in. You would crash soon ...");
      }

      if(!el.isValid()){
        ATH_MSG_ERROR("Invalid link to subjet through link !");
        return false;
      }
      else{
        auto subjet = dynamic_cast<const xAOD::Jet*>(*el);
        if(subjet == 0){
          ATH_MSG_ERROR("Empty ptr to subjet! You will crash soon...");
          return false;
        }
        else{
          ExKt3Subjets.push_back(subjet);
        }
      }
    }
    if(ExKt3Subjets.size() == 3){
      std::sort(ExKt3Subjets.begin(), ExKt3Subjets.end(),
        [](const xAOD::Jet* j1, const xAOD::Jet* j2){
          return (j1->btagging()->auxdata<double>("MV2c10_discriminant") > j2->btagging()->auxdata<double>("MV2c10_discriminant"));
        }

      );
      if(m_isMC){
        exkt3_leadingMV2c10_nTruthBHadrons = ExKt3Subjets.at(0)->auxdata<int>("GhostBHadronsFinalCount");
        exkt3_leadingMV2c10_nTruthCHadrons = ExKt3Subjets.at(0)->auxdata<int>("GhostCHadronsFinalCount");
        exkt3_subleadingMV2c10_nTruthBHadrons = ExKt3Subjets.at(1)->auxdata<int>("GhostBHadronsFinalCount");
        exkt3_subleadingMV2c10_nTruthCHadrons = ExKt3Subjets.at(1)->auxdata<int>("GhostCHadronsFinalCount");
      }
      //subjets order by MV2c10
      const xAOD::BTagging* bjet_LeadExKt3SubJet = ExKt3Subjets.at(0)->btagging();
      const xAOD::BTagging* bjet_SubLeadExKt3SubJet = ExKt3Subjets.at(1)->btagging();
      const xAOD::BTagging* bjet_SubsubLeadExKt3SubJet = ExKt3Subjets.at(2)->btagging();


      exkt3_maxmv2    = std::max(bjet_LeadExKt3SubJet->auxdata<double>("MV2c10_discriminant"),bjet_SubLeadExKt3SubJet->auxdata<double>("MV2c10_discriminant"));
      exkt3_minmv2    = std::min(bjet_LeadExKt3SubJet->auxdata<double>("MV2c10_discriminant"),bjet_SubLeadExKt3SubJet->auxdata<double>("MV2c10_discriminant"));

      exkt3_j1_MV2r = bjet_LeadExKt3SubJet->auxdata<double>("MV2r_discriminant");
      exkt3_j1_DL1_pb = bjet_LeadExKt3SubJet->auxdata<double>("DL1_pb");
      exkt3_j1_DL1_pc = bjet_LeadExKt3SubJet->auxdata<double>("DL1_pc");
      exkt3_j1_DL1_pu = bjet_LeadExKt3SubJet->auxdata<double>("DL1_pu");
      exkt3_j1_DL1r_pb = bjet_LeadExKt3SubJet->auxdata<double>("DL1r_pb");
      exkt3_j1_DL1r_pc = bjet_LeadExKt3SubJet->auxdata<double>("DL1r_pc");
      exkt3_j1_DL1r_pu = bjet_LeadExKt3SubJet->auxdata<double>("DL1r_pu");

      exkt3_j2_MV2r = bjet_SubLeadExKt3SubJet->auxdata<double>("MV2r_discriminant");
      exkt3_j2_DL1_pb = bjet_SubLeadExKt3SubJet->auxdata<double>("DL1_pb");
      exkt3_j2_DL1_pc = bjet_SubLeadExKt3SubJet->auxdata<double>("DL1_pc");
      exkt3_j2_DL1_pu = bjet_SubLeadExKt3SubJet->auxdata<double>("DL1_pu");
      exkt3_j2_DL1r_pb = bjet_SubLeadExKt3SubJet->auxdata<double>("DL1r_pb");
      exkt3_j2_DL1r_pc = bjet_SubLeadExKt3SubJet->auxdata<double>("DL1r_pc");
      exkt3_j2_DL1r_pu = bjet_SubLeadExKt3SubJet->auxdata<double>("DL1r_pu");

      exkt3_j3_MV2r = bjet_SubsubLeadExKt3SubJet->auxdata<double>("MV2r_discriminant");
      exkt3_j3_DL1_pb = bjet_SubsubLeadExKt3SubJet->auxdata<double>("DL1_pb");
      exkt3_j3_DL1_pc = bjet_SubsubLeadExKt3SubJet->auxdata<double>("DL1_pc");
      exkt3_j3_DL1_pu = bjet_SubsubLeadExKt3SubJet->auxdata<double>("DL1_pu");
      exkt3_j3_DL1r_pb = bjet_SubsubLeadExKt3SubJet->auxdata<double>("DL1r_pb");
      exkt3_j3_DL1r_pc = bjet_SubsubLeadExKt3SubJet->auxdata<double>("DL1r_pc");
      exkt3_j3_DL1r_pu = bjet_SubsubLeadExKt3SubJet->auxdata<double>("DL1r_pu");

      exkt3_subDR     = ExKt3Subjets.at(0)->p4().DeltaR(ExKt3Subjets.at(1)->p4());
      if(ExKt3Subjets.at(0)->pt()+ExKt3Subjets.at(1)->pt() > 0) exkt3_ptasym =  fabs(ExKt3Subjets.at(0)->pt()-ExKt3Subjets.at(1)->pt())/(ExKt3Subjets.at(0)->pt()+ExKt3Subjets.at(1)->pt());
      else exkt3_ptasym = -999;

    }


    jet_maxmv2(*(smalljets->at(j))) = maxmv2;
    jet_maxMV2r(*(smalljets->at(j))) = maxMV2r;
    jet_maxDL1_pb(*(smalljets->at(j))) = maxDL1_pb;
    jet_maxDL1_pc(*(smalljets->at(j))) = maxDL1_pc;
    jet_maxDL1_pu(*(smalljets->at(j))) = maxDL1_pu;
    jet_maxDL1r_pb(*(smalljets->at(j))) = maxDL1r_pb;
    jet_maxDL1r_pc(*(smalljets->at(j))) = maxDL1r_pc;
    jet_maxDL1r_pu(*(smalljets->at(j))) = maxDL1r_pu;

    jet_minmv2(*(smalljets->at(j))) = minmv2;
    jet_minMV2r(*(smalljets->at(j))) = minMV2r;
    jet_minDL1_pb(*(smalljets->at(j))) = minDL1_pb;
    jet_minDL1_pc(*(smalljets->at(j))) = minDL1_pc;
    jet_minDL1_pu(*(smalljets->at(j))) = minDL1_pu;
    jet_minDL1r_pb(*(smalljets->at(j))) = minDL1r_pb;
    jet_minDL1r_pc(*(smalljets->at(j))) = minDL1r_pc;
    jet_minDL1r_pu(*(smalljets->at(j))) = minDL1r_pu;

    jet_subDR(*(smalljets->at(j))) = subDR;
    jet_ptasym(*(smalljets->at(j))) = ptasym;
    jet_leadingPt_nTruthBHadrons(*(smalljets->at(j))) = leadingPt_nTruthBHadrons;
    jet_leadingPt_nTruthCHadrons(*(smalljets->at(j))) = leadingPt_nTruthCHadrons;
    jet_subleadingPt_nTruthBHadrons(*(smalljets->at(j))) = subleadingPt_nTruthBHadrons;
    jet_subleadingPt_nTruthCHadrons(*(smalljets->at(j))) = subleadingPt_nTruthCHadrons;


    jet_exkt3_maxmv2(*(smalljets->at(j))) = exkt3_maxmv2;
    jet_exkt3_minmv2(*(smalljets->at(j))) = exkt3_minmv2;

    jet_exkt3_j1_MV2r(*(smalljets->at(j))) = exkt3_j1_MV2r;
    jet_exkt3_j1_DL1_pb(*(smalljets->at(j))) = exkt3_j1_DL1_pb;
    jet_exkt3_j1_DL1_pc(*(smalljets->at(j))) = exkt3_j1_DL1_pc;
    jet_exkt3_j1_DL1_pu(*(smalljets->at(j))) = exkt3_j1_DL1_pu;
    jet_exkt3_j1_DL1r_pb(*(smalljets->at(j))) = exkt3_j1_DL1r_pb;
    jet_exkt3_j1_DL1r_pc(*(smalljets->at(j))) = exkt3_j1_DL1r_pc;
    jet_exkt3_j1_DL1r_pu(*(smalljets->at(j))) = exkt3_j1_DL1r_pu;

    jet_exkt3_j2_MV2r(*(smalljets->at(j))) = exkt3_j2_MV2r;
    jet_exkt3_j2_DL1_pb(*(smalljets->at(j))) = exkt3_j2_DL1_pb;
    jet_exkt3_j2_DL1_pc(*(smalljets->at(j))) = exkt3_j2_DL1_pc;
    jet_exkt3_j2_DL1_pu(*(smalljets->at(j))) = exkt3_j2_DL1_pu;
    jet_exkt3_j2_DL1r_pb(*(smalljets->at(j))) = exkt3_j2_DL1r_pb;
    jet_exkt3_j2_DL1r_pc(*(smalljets->at(j))) = exkt3_j2_DL1r_pc;
    jet_exkt3_j2_DL1r_pu(*(smalljets->at(j))) = exkt3_j2_DL1r_pu;

    jet_exkt3_j3_MV2r(*(smalljets->at(j))) = exkt3_j3_MV2r;
    jet_exkt3_j3_DL1_pb(*(smalljets->at(j))) = exkt3_j3_DL1_pb;
    jet_exkt3_j3_DL1_pc(*(smalljets->at(j))) = exkt3_j3_DL1_pc;
    jet_exkt3_j3_DL1_pu(*(smalljets->at(j))) = exkt3_j3_DL1_pu;
    jet_exkt3_j3_DL1r_pb(*(smalljets->at(j))) = exkt3_j3_DL1r_pb;
    jet_exkt3_j3_DL1r_pc(*(smalljets->at(j))) = exkt3_j3_DL1r_pc;
    jet_exkt3_j3_DL1r_pu(*(smalljets->at(j))) = exkt3_j3_DL1r_pu;

    jet_exkt3_subDR(*(smalljets->at(j))) = exkt3_subDR;
    jet_exkt3_ptasym(*(smalljets->at(j))) = exkt3_ptasym;
    jet_exkt3_leadingMV2c10_nTruthBHadrons(*(smalljets->at(j))) = exkt3_leadingMV2c10_nTruthBHadrons;
    jet_exkt3_leadingMV2c10_nTruthCHadrons(*(smalljets->at(j))) = exkt3_leadingMV2c10_nTruthCHadrons;
    jet_exkt3_subleadingMV2c10_nTruthBHadrons(*(smalljets->at(j))) = exkt3_subleadingMV2c10_nTruthBHadrons;
    jet_exkt3_subleadingMV2c10_nTruthCHadrons(*(smalljets->at(j))) = exkt3_subleadingMV2c10_nTruthCHadrons;

    jet_largeJetLabel(*(smalljets->at(j))) = i;




  } // if
}  // for large

} // for small
 return StatusCode::SUCCESS;

}



} /// namespace
