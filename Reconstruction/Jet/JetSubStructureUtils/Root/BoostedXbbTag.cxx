/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/BoostedXbbTag.h"

#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif

// root includes
#include <TSystem.h>
#include <TFile.h>
#include <TH2F.h>

// c++ includes
#include <iostream>
#include <sstream>
#include <fstream>

#define APP_NAME "BoostedXbbTag"

using namespace JetSubStructureUtils;

// make all static accessors static to this file, like extern but hip
SG::AuxElement::ConstAccessor<int>      BoostedXbbTag::AlgorithmType ("AlgorithmType");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::SizeParameter ("SizeParameter");
SG::AuxElement::ConstAccessor<int>      BoostedXbbTag::InputType ("InputType");
SG::AuxElement::ConstAccessor<int>      BoostedXbbTag::TransformType ("TransformType");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::RClus ("RClus");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::PtFrac ("PtFrac");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::RCut ("RCut");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::ZCut ("ZCut");
SG::AuxElement::ConstAccessor<char>     BoostedXbbTag::BDRS ("BDRS");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::YMin ("YMin");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::MuMax ("MuMax");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::D2 ("D2");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::ECF1 ("ECF1");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::ECF2 ("ECF2");
SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::ECF3 ("ECF3");

BoostedXbbTag::BoostedXbbTag( std::string working_point,
                              std::string recommendations_file,
                              std::string boson_type,
                              std::string algorithm_name,
                              int num_bTags,
                              bool debug,
                              bool verbose) :
  m_working_point(working_point),
  m_recommendations_file(recommendations_file),
  m_boson_type(boson_type),
  m_algorithm_name(algorithm_name),
  m_num_bTags(num_bTags),
  m_debug(debug),
  m_verbose(verbose),
  m_bTagCut(FLT_MIN),
  m_massMin(FLT_MIN),
  m_massMax(FLT_MAX),
  m_D2_params(5, FLT_MIN),
  m_D2_cut_direction("None"),
  m_muonSelectionTool(new CP::MuonSelectionTool("JSSU_MuonSelection")),
  m_bad_configuration(false)
{

  /* check configurations passed in, use m_bad_configuration as flag:
        - flag it to true if something is badly configured
        - otherwise, it should be false if everything seems ok
  */

  if(m_debug)
    printf("<%s>: Attempting to configure with\r\n\t"
            "Working Point          %s\r\n\t"
            "Recommendations File   %s\r\n\t"
            "Boson Type             %s\r\n\t"
            "Algorithm Name         %s\r\n\t"
            "Debug Output?          %s\r\n\t"
            "Verbose Output?        %s\r\n"
            "=========================================\r\n",
            APP_NAME, m_working_point.c_str(),
            m_recommendations_file.c_str(),
            m_boson_type.c_str(), m_algorithm_name.c_str(),
            m_debug?"Yes":"No", m_verbose?"Yes":"No");

  if(!m_muonSelectionTool->initialize().isSuccess()){
    printf("<%s>: Could not initialize the MuonSelectionTool.\r\n", APP_NAME);
    m_bad_configuration |= true;
  }

  std::set<std::string> validWorkingPoints = {"tight", "medium", "loose", "veryloose"};
  if( validWorkingPoints.find(m_working_point) == validWorkingPoints.end()){
    printf("<%s>: Unknown working point requested.\r\n\tExpected: veryloose, loose, medium, tight\r\n\tGiven:    %s\r\n", APP_NAME, m_working_point.c_str());
    m_bad_configuration |= true;
  } else {
    if(m_verbose) printf("<%s>: Valid working point requested.\r\n", APP_NAME);
  }

  std::set<std::string> validBosonTypes = {"Higgs"};
  if( validBosonTypes.find(m_boson_type) == validBosonTypes.end()){
    printf("<%s>: Unknown boson type requested.\r\n\tHiggs\r\n\tGiven:   %s\r\n", APP_NAME, m_boson_type.c_str());
    m_bad_configuration |= true;
  } else {
    if(m_verbose) printf("<%s>: Valid boson type requested.\r\n", APP_NAME);
  }

#ifdef ROOTCORE
  m_recommendations_file = gSystem->ExpandPathName(m_recommendations_file.c_str());
#else
  m_recommendations_file = PathResolverFindXMLFile(m_recommendations_file);
#endif

  bool found_configuration = false;

  /* https://root.cern.ch/root/roottalk/roottalk02/5332.html */
  FileStat_t fStats;
  int fSuccess = gSystem->GetPathInfo(m_recommendations_file.c_str(), fStats);
  if(fSuccess != 0){
    printf("<%s>: Recommendations file could not be found.\r\n\tGiven:   %s\r\n", APP_NAME, m_recommendations_file.c_str());
    m_bad_configuration |= true;
  } else {
    if(m_verbose) printf("<%s>: Recommendations file was found.\r\n", APP_NAME);

    // if we made it here, everything appears ok with our file, attempt to read it
    std::ifstream f_in;
    f_in.open(m_recommendations_file, std::ios::in);
    if( f_in.fail() ){
      printf("<%s>: Something is wrong with the recommendations file. Could not open for reading.\r\n", APP_NAME);
      m_bad_configuration |= true;
    } else {
      if(m_verbose) printf("<%s>: Recommendations file opened for reading.\r\n", APP_NAME);

      std::string line;
      while( std::getline(f_in, line) ){
        if(line.empty()) continue; // skip empty lines
        if(line[0] == '#') continue; // skip commented lines

        /* token contains the current splitted text */
        std::string token;

        // split by space
        std::istringstream ss(line);
        /* lineDetails is an array of the splits */
        std::vector<std::string> lineDetails{std::istream_iterator<std::string>{ss}, std::istream_iterator<std::string>{}};
        if(lineDetails.size() != 13) continue;

        if(m_verbose) printf("<%s>: Reading in line\r\n\t'%s'\r\n", APP_NAME, line.c_str());
        if(lineDetails[0] != m_boson_type) continue;
        if(lineDetails[1] != m_working_point) continue;
        if(lineDetails[2] != m_algorithm_name) continue;
        if(std::stoi(lineDetails[3]) != m_num_bTags) continue;

        m_bTagCut             = std::stof(lineDetails[4]);
        m_massMin             = std::stof(lineDetails[5]);
        m_massMax             = std::stof(lineDetails[6]);
        for(int i=0; i < 5; i++)
          m_D2_params[i]      = std::stof(lineDetails[i+7]);
        m_D2_cut_direction    = lineDetails[12];

        if(m_verbose) printf("<%s>: Found the configuration! We're done reading the file.\r\n", APP_NAME);
        found_configuration = true;
        break;
      }
    }
  }

  if(!found_configuration){
    printf("<%s>: Could not configure the tool. The configuration does not exist in the recommendations file.\r\n", APP_NAME);
    m_bad_configuration |= true;
  }

  if(m_bad_configuration){
    std::cout << "|=====================================================|" << std::endl;
    std::cout << "|        WARNING        WARNING        WARNING        |" << std::endl;
    std::cout << "|        WARNING        WARNING        WARNING        |" << std::endl;
    std::cout << "|        WARNING        WARNING        WARNING        |" << std::endl;
    std::cout << "|        WARNING        WARNING        WARNING        |" << std::endl;
    std::cout << "|-----------------------------------------------------|" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|         BoostedXbbTagger is misconfigured!          |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|=====================================================|" << std::endl;
  } else {
    if(m_verbose) printf("<%s>: BoostedXbbTagger is configured successfuly! Congratulations on such an achievement.\r\n", APP_NAME);
  }

}

std::pair<bool, std::string> BoostedXbbTag::get_algorithm_name(const xAOD::Jet& jet,
                                         const xAOD::JetAlgorithmType::ID jet_algorithm,
                                         const float size_parameter,
                                         const xAOD::JetInput::Type jet_input,
                                         const xAOD::JetTransform::Type jet_transform) const {
  bool error_flag(false);

  /* http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODJet/xAODJet/JetContainerInfo.h */
  static std::map<int, std::string> mapAlgorithmType = {
    {xAOD::JetAlgorithmType::kt_algorithm, "KT"},
    {xAOD::JetAlgorithmType::cambridge_algorithm, "CA"},
    {xAOD::JetAlgorithmType::antikt_algorithm, "AK"}
  };

  static std::map<int, std::string> mapInputType {
    {xAOD::JetInput::LCTopo, "LC"},
    {xAOD::JetInput::EMTopo, "EM"},
    {xAOD::JetInput::Truth, "TRU"},
    {xAOD::JetInput::Track, "TRA"},
    {xAOD::JetInput::PFlow, "PFL"}
  };

  static std::map<int, std::string> mapTransformType {
    {xAOD::JetTransform::Trim, "TRIM"},
    {xAOD::JetTransform::Prune, "PRUN"},
    {xAOD::JetTransform::MassDrop, "BDRS"}
  };

  // beginning of the algorithm_name
  std::string algorithm_name = mapAlgorithmType[jet_algorithm]
                              + std::to_string(static_cast<int>(size_parameter*10))
                              + mapInputType[jet_input]
                              + mapTransformType[jet_transform];


  // ending of algorithm_name
  switch(jet_transform){
    case xAOD::JetTransform::Trim:
      if( !PtFrac.isAvailable(jet) ){
        if(m_debug) printf("<%s>: PtFrac is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !RClus.isAvailable(jet) ){
        if(m_debug) printf("<%s>: RClus is not defined for the input jet.\r\n" , APP_NAME);
        error_flag |= true;
      }

      if(m_verbose) printf("<%s>: PtFrac: %0.2f\tRClus: %0.2f\r\n", APP_NAME, PtFrac(jet), RClus(jet));
      algorithm_name += "F" + std::to_string(static_cast<int>(PtFrac(jet)*100))
                       + "R" + std::to_string(static_cast<int>(RClus(jet)*100));
    break;
    case xAOD::JetTransform::Prune:
      if( !RCut.isAvailable(jet) ){
        if(m_debug) printf("<%s>: RCut is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !ZCut.isAvailable(jet) ){
        if(m_debug) printf("<%s>: ZCut is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }

      if(m_verbose) printf("<%s>: RCut: %0.2f\tZCut: %0.2f\r\n", APP_NAME, RCut(jet), ZCut(jet));
      algorithm_name += "R" + std::to_string(static_cast<int>(RCut(jet)*100))
                       + "Z" + std::to_string(static_cast<int>(ZCut(jet)*100));
    break;
    case xAOD::JetTransform::MassDrop:
      if( !MuMax.isAvailable(jet) ){
        if(m_debug) printf("<%s>: MuMax is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !RClus.isAvailable(jet) ){
        if(m_debug) printf("<%s>: RClus is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !YMin.isAvailable(jet) ){
        if(m_debug) printf("<%s>: YMin is not defined for the input jet.\r\n" , APP_NAME);
        error_flag |= true;
      }
      if( !BDRS.isAvailable(jet) ){
        if(m_debug) printf("<%s>: BDRS is not defined for the input jet.\r\n" , APP_NAME);
        error_flag |= true;
      }

      if(m_verbose) printf("<%s>: MuMax: %0.2f\tRClus: %0.2f\tYMin: %0.2f\r\n", APP_NAME, MuMax(jet), RClus(jet), YMin(jet));
      algorithm_name += "M" + std::to_string(static_cast<int>(MuMax(jet)*100))
                       + "R" + std::to_string(static_cast<int>(RClus(jet)*100))
                       + "Y" + std::to_string(static_cast<int>(YMin(jet)*100));
    break;
    default:
      if(m_debug) printf("<%s>: Current value of xAOD::JetTransform::Type is not supported!\r\n", APP_NAME);
      error_flag |= true;
    break;
  }

  return std::pair<bool, std::string>(!error_flag, algorithm_name);
}


int BoostedXbbTag::result(const xAOD::Jet& jet, const xAOD::MuonContainer* muons) const
{
  // bad configuration
  if(m_bad_configuration){
    if(m_debug) printf("<%s>: BoostedXbbTag has a bad configuration!\r\n", APP_NAME);
    return -9;
  }

  // if we call via this method, we need these 4 things defined
  if( !AlgorithmType.isAvailable(jet) ){
    if(m_debug) printf("<%s>: AlgorithmType is not defined for the jet.\r\n", APP_NAME);
    return -9;
  }
  if( !SizeParameter.isAvailable(jet) ){
    if(m_debug) printf("<%s>: SizeParameter is not defined for the jet.\r\n", APP_NAME);
    return -9;
  }
  if( !InputType.isAvailable(jet) )    {
    if(m_debug) printf("<%s>: InputType is not defined for the jet.\r\n"    , APP_NAME);
    return -9;
  }
  if( !TransformType.isAvailable(jet) ){
    if(m_debug) printf("<%s>: TransformType is not defined for the jet.\r\n", APP_NAME);
    return -9;
  }

  if(m_verbose) printf("<%s>: Jet has the 4 main properties set.\r\n\t"
      "AlgorithmType:  %d\r\n\t"
      "Size Parameter: %0.2f\r\n\t"
      "Input Type:     %d\r\n\t"
      "Transform Type: %d\r\n",
      APP_NAME, AlgorithmType(jet), SizeParameter(jet), InputType(jet), TransformType(jet));

  // get the algorithm name and check result
  std::pair<bool, std::string> res = get_algorithm_name(jet,
                                                        static_cast<xAOD::JetAlgorithmType::ID>(AlgorithmType(jet)),
                                                        SizeParameter(jet),
                                                        static_cast<xAOD::JetInput::Type>(InputType(jet)),
                                                        static_cast<xAOD::JetTransform::Type>(TransformType(jet)));

  // is it a valid result?
  if(!res.first){
    if(m_debug) printf("<%s>: Could not determine what jet you are using.\r\n", APP_NAME);
    return -9;
  } else {
    if(m_verbose) printf("<%s>: Jet introspection successful.\r\n", APP_NAME);
  }

  return result(jet, res.second, muons);
}

int BoostedXbbTag::result(const xAOD::Jet& jet, std::string algorithm_name, const xAOD::MuonContainer* muons) const {
  // bad configuration
  if(m_bad_configuration){
    if(m_debug) printf("<%s>: BoostedXbbTag has a bad configuration!\r\n", APP_NAME);
    return -9;
  }

  // make sure we are using the right kind of jet
  if(algorithm_name != m_algorithm_name){
    if(m_debug) printf("<%s>: You configured for %s but you passed in a jet of type %s.\r\n", APP_NAME, m_algorithm_name.c_str(), algorithm_name.c_str());
    return -9;
  }

  /* Steps:
      0. Fat Jet Selection
      1. Get all AntiKt2TrackJets asssociated with the ungroomed jet
      2. B-tag the two leading track-jets
      3. If both track-jets are b-tagged, match the muon (if any) to these b-tagged track-jets
        - if more than 1 muon matches a track jet (within the radius of the track jet), only use the muon closest in DR
      4. Correct the fat-jet mass by putting the matched muon back (if there is a matched muon)
      5. Set a cut on the corrected fat jet mass
      6. Cut on the D2 of the fat-jet (D2 from calorimeter constituents only)
  */

  // Step 0
  if(jet.pt()/1.e3 < 250.0 || abs(jet.eta()) > 2.0){
    if(m_verbose) printf("<%s>: Jet does not pass preselection: pT > 250 GeV and |eta| < 2.0.\r\n\t pT: %0.2f GeV\r\n\teta: %0.2f\r\n", APP_NAME, jet.pt()/1.e3, jet.eta());
    return -1;
  }

  // Step 1
  std::vector<const xAOD::Jet*> associated_trackJets;
  static SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> parent("Parent");
  // get the track jets from the parent
  bool problemWithParent = false;
  ElementLink<xAOD::JetContainer> parentEL;
  if(!parent.isAvailable(jet)) problemWithParent = true;
  else parentEL = parent(jet);
  if(problemWithParent || !parentEL.isValid()){
    if(m_debug) printf("<%s>: ", APP_NAME);
    if(problemWithParent && m_debug) printf("Parent decoration does not exist. ");
    if(!parentEL.isValid() && m_debug) printf("Parent link is not valid. ");
    if(m_debug) printf("\r\n");
    return -2; // do not fallback
    /*
    if(m_debug) printf("Get track jets from groomed jet.\r\n");
    if(!jet.getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", associated_trackJets)){
      if(m_verbose) printf("<%s>: No associated track jets found on jet.\r\n", APP_NAME);
      return -2;
    }
    if(m_verbose) printf("<%s>: Got track jets from groomed jet.\r\n", APP_NAME);
    */
  } else {
    const xAOD::Jet* parentJet = *parentEL;
    if(!parentJet->getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", associated_trackJets)){
      if(m_verbose) printf("<%s>: No associated track jets found on parent jet.\r\n", APP_NAME);
      return -2;
    }
    if(m_verbose) printf("<%s>: Got track jets from parent jet.\r\n", APP_NAME);
  }

  // filter out the track jets we do not want (pT > 10 GeV and |eta| < 2.5 and at least 2 constituents)
  std::remove_if(associated_trackJets.begin(), associated_trackJets.end(),  [](const xAOD::Jet* jet) -> bool { return (jet->pt()/1.e3 < 10.0 || abs(jet->eta()) > 2.5 || jet->numConstituents() < 2); });
  if(associated_trackJets.size() < 2){
    if(m_verbose) printf("<%s>: We need at least two associated track jets.\r\n", APP_NAME);
    return -2;
  }

  // Step 2
  std::sort(associated_trackJets.begin(), associated_trackJets.end(), [](const xAOD::IParticle* lhs, const xAOD::IParticle* rhs) -> bool { return (lhs->pt() > rhs->pt()); });
  static SG::AuxElement::Decorator<int> isB("isB");
  int num_bTags(0);
  for(int i=0; i<2; i++){
    double mv2c20(FLT_MIN);
    if(!associated_trackJets.at(i)->btagging()->MVx_discriminant("MV2c20", mv2c20)){
      if(m_verbose) printf("<%s>: Could not retrieve the MV2c20 discriminant.\r\n", APP_NAME);
      return -9;
    }
    int isBTagged = static_cast<int>(mv2c20 > m_bTagCut);
    isB(*(associated_trackJets.at(i))) = isBTagged;
    num_bTags += isBTagged;
  }
  if( num_bTags < m_num_bTags ){
    if(m_verbose) printf("<%s>: We require the %d leading track jet%s b-tagged. %d %s b-tagged.\r\n", APP_NAME, m_num_bTags, (m_num_bTags == 1)?"":"s", num_bTags, (num_bTags == 1)?"was":"were");
    return -2;
  }

  // Step 3
  const xAOD::Muon* matched_muon(nullptr);
  // first select the muons: Combined, Medium, pT > 10 GeV, |eta| < 2.5
  std::vector<const xAOD::Muon*> preselected_muons(muons->size(), nullptr);
  auto it = std::copy_if(muons->begin(), muons->end(), preselected_muons.begin(), [this](const xAOD::Muon* muon) -> bool { return (muon->pt()/1.e3 > 10.0 && m_muonSelectionTool->getQuality(*muon) <= xAOD::Muon::Medium && fabs(muon->eta()) < 2.5); });
  preselected_muons.resize(std::distance(preselected_muons.begin(), it)); // shrink container to new size
  if(preselected_muons.size() == 0){
    if(m_verbose) printf("<%s>: There are no muons that passed the kinematic preselection.\r\n", APP_NAME);
    //return -3;
  } else {
    for(int i=0; i<2; i++){
      float maxDR(0.2);
      associated_trackJets.at(i)->getAttribute("SizeParameter", maxDR);
      for(const auto muon: preselected_muons){
        float DR( associated_trackJets.at(i)->p4().DeltaR(muon->p4()) );
        if(DR > maxDR) continue;
        maxDR = DR;
        matched_muon = muon;
      }
    }
  }

  // Step 4
  TLorentzVector corrected_jet;
  if(!matched_muon){
    if(m_verbose) printf("<%s>: There is no matched muon.\r\n", APP_NAME);
    //return -3;
    corrected_jet = jet.p4();
  } else {
    // super optimized version, need to know name of Muons collection
    // ElementLink< xAOD::IParticleContainer > el_muon( "Muons", matched_muon->index() );
    static SG::AuxElement::Decorator<ElementLink<xAOD::IParticleContainer> > matchedMuonLink("MatchedMuonLink");
    ElementLink<xAOD::IParticleContainer> el_muon( *muons, matched_muon->index() );
    matchedMuonLink(jet) = el_muon;

    float eLoss(0.0);
    matched_muon->parameter(eLoss,xAOD::Muon::EnergyLoss);
    if(m_debug) printf("<%s>: getELossTLV xAOD::Muon eLoss= %0.2f\r\n", APP_NAME, eLoss);
    auto mTLV = matched_muon->p4();
    double eLossX = eLoss*sin(mTLV.Theta())*cos(mTLV.Phi());
    double eLossY = eLoss*sin(mTLV.Theta())*sin(mTLV.Phi());
    double eLossZ = eLoss*cos(mTLV.Theta());
    auto mLoss = TLorentzVector(eLossX,eLossY,eLossZ,eLoss);
    auto corrected_jet = jet.p4() + mTLV - mLoss;
  }

  std::string buffer;

  // Step 5
  buffer = "<%s>: Jet %s the mass window cut.\r\n\tMass: %0.6f GeV\r\n\tMass Window: [ %0.6f, %0.6f ]\r\n";
  if(corrected_jet.M()/1.e3 < m_massMin || corrected_jet.M()/1.e3 > m_massMax){
    if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", corrected_jet.M()/1.e3, m_massMin, m_massMax);
    return 0;
  } else {
    if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", corrected_jet.M()/1.e3, m_massMin, m_massMax);
  }

  // Step 6
  if(m_D2_cut_direction == "LEFT" || m_D2_cut_direction == "RIGHT"){
    float d2(0.0);
    if(D2.isAvailable(jet)){
      d2 = D2(jet);
    } else {
      if((!ECF1.isAvailable(jet) || !ECF2.isAvailable(jet) || !ECF3.isAvailable(jet))){
        if(m_debug) printf("<%s>: D2 wasn't calculated. ECF# variables are not available.\r\n", APP_NAME);
        return -9;
      }
      d2 = ECF3(jet) * pow(ECF1(jet), 3.0) / pow(ECF2(jet), 3.0);
    }
    buffer = "<%s>: Jet %s the D2 cut from %s\r\n\tD2: %0.6f\r\n\tCut: %0.6f\r\n";
    // then calculate d2 and check that
    float D2Cut = m_D2_params[0] + m_D2_params[1] * jet.pt()/1.e3 + m_D2_params[2] * pow(jet.pt()/1.e3, 2) + m_D2_params[3] * pow(jet.pt()/1.e3, 3) + m_D2_params[4] * pow(jet.pt()/1.e3, 4);
    if((d2 > D2Cut && m_D2_cut_direction == "RIGHT") || (d2 < D2Cut && m_D2_cut_direction == "LEFT")){
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", (m_D2_cut_direction == "RIGHT")?"above":"below", d2, D2Cut);
      return 0;
    } else {
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", (m_D2_cut_direction == "RIGHT")?"above":"below", d2, D2Cut);
    }
  } else {
    if(m_verbose) printf("<%s>: No D2 cut has been requested here. The cut direction specified was %s which is not 'LEFT' or 'RIGHT'.\r\n", APP_NAME, m_D2_cut_direction.c_str());
  }

  if(m_verbose) printf("<%s>: Jet is tagged as %s.\r\n", APP_NAME, m_boson_type.c_str());
  return 1;

}
