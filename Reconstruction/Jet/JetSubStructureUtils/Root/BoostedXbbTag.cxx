/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/BoostedXbbTag.h"

#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif

#include "xAODBTagging/BTaggingUtilities.h"

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
const SG::AuxElement::ConstAccessor<int>      BoostedXbbTag::s_AlgorithmType ("AlgorithmType");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_SizeParameter ("SizeParameter");
const SG::AuxElement::ConstAccessor<int>      BoostedXbbTag::s_InputType ("InputType");
const SG::AuxElement::ConstAccessor<int>      BoostedXbbTag::s_TransformType ("TransformType");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_RClus ("RClus");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_PtFrac ("PtFrac");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_RCut ("RCut");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_ZCut ("ZCut");
const SG::AuxElement::ConstAccessor<char>     BoostedXbbTag::s_BDRS ("BDRS");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_YMin ("YMin");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_MuMax ("MuMax");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_D2 ("D2");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_ECF1 ("ECF1");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_ECF2 ("ECF2");
const SG::AuxElement::ConstAccessor<float>    BoostedXbbTag::s_ECF3 ("ECF3");
const SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> BoostedXbbTag::s_parent("Parent");

BoostedXbbTag::BoostedXbbTag( std::string working_point,
                              std::string recommendations_file,
                              std::string boson_type,
                              std::string algorithm_name,
                              int num_bTags,
                              std::string decor_prefix,
                              bool debug,
                              bool verbose) :
  m_working_point(working_point),
  m_recommendations_file(recommendations_file),
  m_boson_type(boson_type),
  m_algorithm_name(algorithm_name),
  m_num_bTags(num_bTags),
  m_decor_prefix(decor_prefix),
  m_debug(debug),
  m_verbose(verbose),
  m_bTagCut(FLT_MIN),
  m_bTagCutCharm(FLT_MIN),
  m_massMin(FLT_MIN),
  m_massMax(FLT_MAX),
  m_D2_params(5, FLT_MIN),
  m_D2_cut_direction("None"),
  m_muonSelectionTool(new CP::MuonSelectionTool("JSSU_MuonSelection")),
  m_bad_configuration(false),
  m_isB(SG::AuxElement::Decorator<int>(m_decor_prefix+"m_isB")),
  m_matchedMuonsLink(SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > >(m_decor_prefix+"MatchedMuonsLink")),
  m_correctedJetDecor(SG::AuxElement::Decorator<TLorentzVector>(m_decor_prefix+"CorrectedJetP4")),
  m_massWindow(SG::AuxElement::Decorator<std::pair<float, float>>(m_decor_prefix+"MassWindow")),
  m_D2Pivot(SG::AuxElement::Decorator<std::pair<float, std::string>>(m_decor_prefix+"m_D2Pivot"))
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

  std::set<std::string> validWorkingPoints = {"tight", "medium", "loose", "veryloose", "single", "single_loose", "single_medium"};
  if( validWorkingPoints.find(m_working_point) == validWorkingPoints.end()){
    printf("<%s>: Unknown working point requested.\r\n\tExpected: single, single_loose, single_medium, veryloose, loose, medium, tight\r\n\tGiven:    %s\r\n", APP_NAME, m_working_point.c_str());
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
        if(lineDetails.size() != 14) continue;

        if(m_verbose) printf("<%s>: Reading in line\r\n\t'%s'\r\n", APP_NAME, line.c_str());
        if(lineDetails[0] != m_boson_type) continue;
        if(lineDetails[1] != m_working_point) continue;
        if(lineDetails[2] != m_algorithm_name) continue;
        if(std::stoi(lineDetails[3]) != m_num_bTags) continue;

        m_bTagCut             = std::stof(lineDetails[4]);
        m_bTagCutCharm        = std::stof(lineDetails[5]);
        m_massMin             = std::stof(lineDetails[6]);
        m_massMax             = std::stof(lineDetails[7]);
        for(int i=0; i < 5; i++)
          m_D2_params[i]      = std::stof(lineDetails[i+8]);
        m_D2_cut_direction    = lineDetails[13];

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
  static const std::map<int, std::string> mapAlgorithmType = {
    {xAOD::JetAlgorithmType::kt_algorithm, "KT"},
    {xAOD::JetAlgorithmType::cambridge_algorithm, "CA"},
    {xAOD::JetAlgorithmType::antikt_algorithm, "AK"}
  };

  static const std::map<int, std::string> mapInputType {
    {xAOD::JetInput::LCTopo, "LC"},
    {xAOD::JetInput::EMTopo, "EM"},
    {xAOD::JetInput::Truth, "TRU"},
    {xAOD::JetInput::Track, "TRA"},
    {xAOD::JetInput::PFlow, "PFL"}
  };

  static const std::map<int, std::string> mapTransformType {
    {xAOD::JetTransform::Trim, "TRIM"},
    {xAOD::JetTransform::Prune, "PRUN"},
    {xAOD::JetTransform::MassDrop, "BDRS"}
  };

  // beginning of the algorithm_name
  std::string algorithm_name = mapAlgorithmType.at(jet_algorithm)
                              + std::to_string(static_cast<int>(size_parameter*10))
                              + mapInputType.at(jet_input)
                              + mapTransformType.at(jet_transform);


  // ending of algorithm_name
  switch(jet_transform){
    case xAOD::JetTransform::Trim:
      if( !s_PtFrac.isAvailable(jet) ){
        if(m_debug) printf("<%s>: PtFrac is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !s_RClus.isAvailable(jet) ){
        if(m_debug) printf("<%s>: RClus is not defined for the input jet.\r\n" , APP_NAME);
        error_flag |= true;
      }

      if(m_verbose) printf("<%s>: PtFrac: %0.2f\tRClus: %0.2f\r\n", APP_NAME, s_PtFrac(jet), s_RClus(jet));
      algorithm_name += "F" + std::to_string(static_cast<int>(s_PtFrac(jet)*100))
                       + "R" + std::to_string(static_cast<int>(s_RClus(jet)*100));
    break;
    case xAOD::JetTransform::Prune:
      if( !s_RCut.isAvailable(jet) ){
        if(m_debug) printf("<%s>: RCut is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !s_ZCut.isAvailable(jet) ){
        if(m_debug) printf("<%s>: ZCut is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }

      if(m_verbose) printf("<%s>: RCut: %0.2f\tZCut: %0.2f\r\n", APP_NAME, s_RCut(jet), s_ZCut(jet));
      algorithm_name += "R" + std::to_string(static_cast<int>(s_RCut(jet)*100))
                       + "Z" + std::to_string(static_cast<int>(s_ZCut(jet)*100));
    break;
    case xAOD::JetTransform::MassDrop:
      if( !s_MuMax.isAvailable(jet) ){
        if(m_debug) printf("<%s>: MuMax is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !s_RClus.isAvailable(jet) ){
        if(m_debug) printf("<%s>: RClus is not defined for the input jet.\r\n", APP_NAME);
        error_flag |= true;
      }
      if( !s_YMin.isAvailable(jet) ){
        if(m_debug) printf("<%s>: YMin is not defined for the input jet.\r\n" , APP_NAME);
        error_flag |= true;
      }
      if( !s_BDRS.isAvailable(jet) ){
        if(m_debug) printf("<%s>: BDRS is not defined for the input jet.\r\n" , APP_NAME);
        error_flag |= true;
      }

      if(m_verbose) printf("<%s>: MuMax: %0.2f\tRClus: %0.2f\tYMin: %0.2f\r\n", APP_NAME, s_MuMax(jet), s_RClus(jet), s_YMin(jet));
      algorithm_name += "M" + std::to_string(static_cast<int>(s_MuMax(jet)*100))
                       + "R" + std::to_string(static_cast<int>(s_RClus(jet)*100))
                       + "Y" + std::to_string(static_cast<int>(s_YMin(jet)*100));
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
  if( !s_AlgorithmType.isAvailable(jet) ){
    if(m_debug) printf("<%s>: AlgorithmType is not defined for the jet.\r\n", APP_NAME);
    return -9;
  }
  if( !s_SizeParameter.isAvailable(jet) ){
    if(m_debug) printf("<%s>: SizeParameter is not defined for the jet.\r\n", APP_NAME);
    return -9;
  }
  if( !s_InputType.isAvailable(jet) )    {
    if(m_debug) printf("<%s>: InputType is not defined for the jet.\r\n"    , APP_NAME);
    return -9;
  }
  if( !s_TransformType.isAvailable(jet) ){
    if(m_debug) printf("<%s>: TransformType is not defined for the jet.\r\n", APP_NAME);
    return -9;
  }

  if(m_verbose) printf("<%s>: Jet has the 4 main properties set.\r\n\t"
      "AlgorithmType:  %d\r\n\t"
      "Size Parameter: %0.2f\r\n\t"
      "Input Type:     %d\r\n\t"
      "Transform Type: %d\r\n",
      APP_NAME, s_AlgorithmType(jet), s_SizeParameter(jet), s_InputType(jet), s_TransformType(jet));

  // get the algorithm name and check result
  std::pair<bool, std::string> res = get_algorithm_name(jet,
                                                        static_cast<xAOD::JetAlgorithmType::ID>(s_AlgorithmType(jet)),
                                                        s_SizeParameter(jet),
                                                        static_cast<xAOD::JetInput::Type>(s_InputType(jet)),
                                                        static_cast<xAOD::JetTransform::Type>(s_TransformType(jet)));

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

  // check basic kinematic selection
  if(jet.pt()/1.e3 < 250.0 || std::fabs(jet.eta()) > 2.0){
    if(m_verbose) printf("<%s>: Jet does not pass basic kinematic selection. pT > 250 GeV, |eta| < 2.0\r\n\tJet Pt: %0.6f GeV\r\n\tJet |eta|: %0.6f\r\n", APP_NAME, jet.pt()/1.e3, std::fabs(jet.eta()));
    return -5;
  }
  // upper pT cut from 2015 not needed anymore because uncertainties are now available
  //if(jet.pt()/1.e3 > 2000.0){
  //  printf("<%s>: Warning, jet has pT > 2 TeV!\r\nJet Pt: %0.6f GeV", APP_NAME, jet.pt()/1.e3);
  //  return -5;
  //}

  // make sure we are using the right kind of jet
  if(algorithm_name != m_algorithm_name){
    if(m_debug) printf("<%s>: You configured for %s but you passed in a jet of type %s.\r\n", APP_NAME, m_algorithm_name.c_str(), algorithm_name.c_str());
    return -9;
  }

  /* Steps:
      1. Get all AntiKt2TrackJets asssociated with the ungroomed jet
      2. B-tag the two leading track-jets
        - if double b-tagging, require that both track-jets are b-tagged
        - if single b-tagging, and there is more than one track-jet, take the highest MV2 of the leading two jets
      3. Match the muon (if any) to these b-tagged track-jets
        - if more than 1 muon matches a track jet (within the radius of the track jet), only use the muon closest in DR
      4. Correct the fat-jet mass by putting the matched muon back (if there is a matched muon)
      5. Set a cut on the corrected fat jet mass
      6. Cut on the D2 of the fat-jet (D2 from calorimeter constituents only)
  */

  // global pass variables, set to false by default
  bool pass_mass = false,
       pass_d2   = false,
       pass_btag = false;

  // Step 1
  std::vector<const xAOD::Jet*> associated_trackJets;
  // get the track jets from the s_parent
  bool problemWithParent = false;
  ElementLink<xAOD::JetContainer> parentEL;
  if(!s_parent.isAvailable(jet)) problemWithParent = true;
  else parentEL = s_parent(jet);
  if(problemWithParent || !parentEL.isValid()){
    if(m_debug) printf("<%s>: ", APP_NAME);
    if(problemWithParent && m_debug) printf("Parent decoration does not exist. ");
    if(!parentEL.isValid() && m_debug) printf("Parent link is not valid. ");
    if(m_debug) printf("\r\n");
    return -2; // do not fallback -- comment out to enable fallback
    //// fallback here
    //if(m_debug) printf("Get track jets from groomed jet.\r\n");
    //if(!jet.getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", associated_trackJets)){
    //  if(m_verbose) printf("<%s>: No associated track jets found on jet.\r\n", APP_NAME);
    //  return -2;
    //}
    //else if(m_verbose) printf("<%s>: Got track jets from groomed jet.\r\n", APP_NAME);
  } else {
    const xAOD::Jet* parentJet = *parentEL;
    if(!parentJet->getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", associated_trackJets)){
      if(m_verbose) printf("<%s>: No associated track jets found on s_parent jet.\r\n", APP_NAME);
      return -2; // do not fallback -- comment out to enable fallback
      //// fallback here
      //if(m_debug) printf("Get track jets from groomed jet.\r\n");
      //if(!jet.getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", associated_trackJets)){
      //  if(m_verbose) printf("<%s>: No associated track jets found on jet.\r\n", APP_NAME);
      //  return -2;
      //}
      //else if(m_verbose) printf("<%s>: Got track jets from groomed jet.\r\n", APP_NAME);
    }
    else if(m_verbose) printf("<%s>: Got track jets from parent jet.\r\n", APP_NAME);
  }

  // decorate all trackjets by default
  for(const auto& trackJet: associated_trackJets)
    m_isB(*trackJet) = 0;

  // filter out the track jets we do not want (pT > 10 GeV and |eta| < 2.5 and at least 2 constituents)
  associated_trackJets.erase(
    std::remove_if(associated_trackJets.begin(), associated_trackJets.end(),  [](const xAOD::Jet* jet) -> bool { return (jet->pt()/1.e3 < 10.0 || fabs(jet->eta()) > 2.5 || jet->numConstituents() < 2); }),
    associated_trackJets.end());
  if(associated_trackJets.size() < 2){
    if(m_working_point.find("single") == std::string::npos){
      if(m_verbose) printf("<%s>: We need at least two associated track jets for working point %s.\r\n", APP_NAME, m_working_point.c_str());
      return -2;
    }
    else if(associated_trackJets.size() < 1){
      if(m_verbose) printf("<%s>: We need at least one associated track jet for working point %s.\r\n", APP_NAME, m_working_point.c_str());
      return -2;
    }
  }

  // Step 2
  std::sort(associated_trackJets.begin(), associated_trackJets.end(), [](const xAOD::IParticle* lhs, const xAOD::IParticle* rhs) -> bool { return (lhs->pt() > rhs->pt()); });
  int num_bTags(0);
  int num_trackJets_toLookAt = std::min((int)associated_trackJets.size(), 2);
  for(int i = 0; i < num_trackJets_toLookAt; i++){
    auto& trackJet = associated_trackJets.at(i);

    // MV2c10
    double mv2c10(FLT_MIN);
    const xAOD::BTagging* btag = xAOD::BTaggingUtilities::getBTagging( *trackJet );
    if(!btag->MVx_discriminant("MV2c10", mv2c10)){
      if(m_verbose) printf("<%s>: Could not retrieve the MV2c10 discriminant.\r\n", APP_NAME);
      return -9;
    }
    int isBTagged = static_cast<int>(mv2c10 > m_bTagCut);

//    // MV2c20
//    double mv2c20(FLT_MIN);
//    if(!trackJet->btagging()->MVx_discriminant("MV2c20", mv2c20)){
//      if(m_verbose) printf("<%s>: Could not retrieve the MV2c20 discriminant.\r\n", APP_NAME);
//      return -9;
//    }
//    int isBTagged = static_cast<int>(mv2c20 > m_bTagCut);

//    // 2D b-tagging
//    double mv2c00(FLT_MIN);
//    double mv2c100(FLT_MIN);
//    if(!trackJet->btagging()->MVx_discriminant("MV2c00", mv2c00)){
//      if(m_verbose) printf("<%s>: Could not retrieve the MV2c00 discriminant.\r\n", APP_NAME);
//      return -9;
//    }
//    if(!trackJet->btagging()->MVx_discriminant("MV2c100", mv2c100)){
//      if(m_verbose) printf("<%s>: Could not retrieve the MV2c100 discriminant.\r\n", APP_NAME);
//      return -9;
//    }
//    int isBTagged = static_cast<int>(mv2c00 > m_bTagCut && mv2c100 > m_bTagCutCharm);

    m_isB(*trackJet) = isBTagged;
    num_bTags += isBTagged;
  }
  if( num_bTags < m_num_bTags ){
    if(m_verbose) printf("<%s>: We require the %d leading track jet%s b-tagged. %d %s b-tagged.\r\n", APP_NAME, m_num_bTags, (m_num_bTags == 1)?"":"s", num_bTags, (num_bTags == 1)?"was":"were");
    //return -2;
  } else {
    pass_btag = true;
  }

  // Step 3
  std::vector<const xAOD::Muon*> matched_muons;
  // first select the muons: Combined, Medium, pT > 10 GeV, |eta| < 2.5
  std::vector<const xAOD::Muon*> preselected_muons(muons->size(), nullptr);
  // [Next line: selection for muon matching. IF PROBLEMS with getQuality, disable muon matching temporarily by using the second line below instead]
  auto it = std::copy_if(muons->begin(), muons->end(), preselected_muons.begin(), [this](const xAOD::Muon* muon) -> bool { return (muon->pt()/1.e3 > 10.0 && m_muonSelectionTool->getQuality(*muon) <= xAOD::Muon::Medium && fabs(muon->eta()) < 2.5); });
  //auto it = std::copy_if(muons->begin(), muons->end(), preselected_muons.begin(), [this](const xAOD::Muon* muon) -> bool { return false; });
  preselected_muons.resize(std::distance(preselected_muons.begin(), it)); // shrink container to new size
  if(preselected_muons.size() == 0){
    if(m_verbose) printf("<%s>: There are no muons that passed the kinematic preselection.\r\n", APP_NAME);
    //return -3;
  } else {
    for(int i = 0; i < num_trackJets_toLookAt; i++){
      auto& trackJet = associated_trackJets.at(i);
      // only match muon to b-tagged track jets
      if(m_isB(*trackJet) == 0) continue;
      // it's b-tagged, try to match it
      float maxDR(0.2);
      trackJet->getAttribute("SizeParameter", maxDR);
      const xAOD::Muon *closest_muon(nullptr);
      for(const auto muon: preselected_muons){
        float DR( trackJet->p4().DeltaR(muon->p4()) );
        if(DR > maxDR) continue;
        maxDR = DR;
        closest_muon = muon;
      }
      if(closest_muon) {
        matched_muons.push_back(closest_muon);
      }
    }
  }

  // Step 4
  TLorentzVector corrected_jet = jet.p4(); 
  if(m_verbose) printf("<%s>: There are %d matched muons.\r\n", APP_NAME, (int)matched_muons.size());
  std::vector<ElementLink<xAOD::IParticleContainer> > matched_muons_links;
  for(auto muon : matched_muons) {
    float eLoss(0.0);
    muon->parameter(eLoss,xAOD::Muon::EnergyLoss);
    if(m_debug) printf("<%s>: getELossTLV xAOD::Muon eLoss= %0.2f\r\n", APP_NAME, eLoss);
    auto mTLV = muon->p4();
    double eLossX = eLoss*sin(mTLV.Theta())*cos(mTLV.Phi());
    double eLossY = eLoss*sin(mTLV.Theta())*sin(mTLV.Phi());
    double eLossZ = eLoss*cos(mTLV.Theta());
    auto mLoss = TLorentzVector(eLossX,eLossY,eLossZ,eLoss);
    corrected_jet = corrected_jet + mTLV - mLoss;

    ElementLink<xAOD::IParticleContainer> el_muon( *muons, muon->index() );
    matched_muons_links.push_back(el_muon);
  }
  // may not always be the corrected jet, but always contains what is used to cut against
  m_correctedJetDecor(jet) = corrected_jet;
  m_matchedMuonsLink(jet) = matched_muons_links;

  std::string buffer;

  // Step 5
  m_massWindow(jet) = std::pair<float, float>(m_massMin, m_massMax);
  buffer = "<%s>: Jet %s the mass window cut.\r\n\tMass: %0.6f GeV\r\n\tMass Window: [ %0.6f, %0.6f ]\r\n";
  if(corrected_jet.M()/1.e3 < m_massMin || corrected_jet.M()/1.e3 > m_massMax){
    if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", corrected_jet.M()/1.e3, m_massMin, m_massMax);
    //return 0;
  } else {
    if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", corrected_jet.M()/1.e3, m_massMin, m_massMax);
    pass_mass = true;
  }

  // Step 6
  if(m_D2_cut_direction == "LEFT" || m_D2_cut_direction == "RIGHT"){
    float d2(0.0);
    if(s_D2.isAvailable(jet)){
      d2 = s_D2(jet);
    } else {
      if((!s_ECF1.isAvailable(jet) || !s_ECF2.isAvailable(jet) || !s_ECF3.isAvailable(jet))){
        if(m_debug) printf("<%s>: D2 wasn't calculated. ECF# variables are not available.\r\n", APP_NAME);
        return -9;
      }
      d2 = s_ECF3(jet) * pow(s_ECF1(jet), 3.0) / pow(s_ECF2(jet), 3.0);
    }
    buffer = "<%s>: Jet %s the D2 cut from %s\r\n\tD2: %0.6f\r\n\tCut: %0.6f\r\n";
    // then calculate d2 and check that
    float D2Cut = m_D2_params[0] + m_D2_params[1] * jet.pt()/1.e3 + m_D2_params[2] * pow(jet.pt()/1.e3, 2) + m_D2_params[3] * pow(jet.pt()/1.e3, 3) + m_D2_params[4] * pow(jet.pt()/1.e3, 4);
    m_D2Pivot(jet) = std::pair<float, std::string>(D2Cut, m_D2_cut_direction);
    if((d2 > D2Cut && m_D2_cut_direction == "RIGHT") || (d2 < D2Cut && m_D2_cut_direction == "LEFT")){
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", (m_D2_cut_direction == "RIGHT")?"above":"below", d2, D2Cut);
      //return 0;
    } else {
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", (m_D2_cut_direction == "RIGHT")?"above":"below", d2, D2Cut);
      pass_d2 = true;
    }
  } else {
    if(m_verbose) printf("<%s>: No D2 cut has been requested here. The cut direction specified was %s which is not 'LEFT' or 'RIGHT'.\r\n", APP_NAME, m_D2_cut_direction.c_str());
  }

  if(m_verbose) printf("<%s>: Jet is tagged as %s.\r\n", APP_NAME, m_boson_type.c_str());
  //return 1;
  return static_cast<int>((pass_mass << 2)|(pass_d2 << 1)|(pass_btag << 0));

}

std::vector<const xAOD::Jet*> BoostedXbbTag::get_bTagged_trackJets(const xAOD::Jet& jet) const {
  std::vector<const xAOD::Jet*> associated_trackJets;
  // get the track jets from the s_parent
  (*s_parent(jet))->getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", associated_trackJets);
  //// IF PROBLEMS with association to ungroomed parent, comment out to get track jets from trimmed jet
  //jet.getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", associated_trackJets);
  std::vector<const xAOD::Jet*> bTagged_trackJets;
  for(const auto& trackJet: associated_trackJets){
    if(m_isB(*trackJet) == 0) continue;
    bTagged_trackJets.push_back(trackJet);
  }
  return bTagged_trackJets;
}

std::vector<const xAOD::Muon*> BoostedXbbTag::get_matched_muons(const xAOD::Jet& jet) const {
  std::vector<const xAOD::Muon*> muons;

  auto muonsLink = m_matchedMuonsLink(jet);
  for(auto muonLink : muonsLink) {
    if(!muonLink.isValid()) {
      muons.push_back(nullptr);
    }
    else {
      muons.push_back(static_cast<const xAOD::Muon*>(*muonLink));
    }
  }
	
  return muons;
}


TLorentzVector BoostedXbbTag::get_correctedJet_TLV(const xAOD::Jet& jet) const {
  return m_correctedJetDecor(jet);
}

std::pair<float, float> BoostedXbbTag::get_mass_window(const xAOD::Jet& jet) const {
  return m_massWindow(jet);
}

std::pair<float, std::string> BoostedXbbTag::get_D2_pivot(const xAOD::Jet& jet) const {
  return m_D2Pivot(jet);
}
