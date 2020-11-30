/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/BosonTag.h"

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

#define APP_NAME "BosonTag"

using namespace JetSubStructureUtils;

// make all static accessors static to this file, like extern but hip
SG::AuxElement::ConstAccessor<int>      BosonTag::s_AlgorithmType ("AlgorithmType");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_SizeParameter ("SizeParameter");
SG::AuxElement::ConstAccessor<int>      BosonTag::s_InputType ("InputType");
SG::AuxElement::ConstAccessor<int>      BosonTag::s_TransformType ("TransformType");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_RClus ("RClus");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_PtFrac ("PtFrac");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_RCut ("RCut");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_ZCut ("ZCut");
SG::AuxElement::ConstAccessor<char>     BosonTag::s_BDRS ("BDRS");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_YMin ("YMin");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_MuMax ("MuMax");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_YFilt ("YFilt");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_D2 ("D2");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_ECF1 ("ECF1");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_ECF2 ("ECF2");
SG::AuxElement::ConstAccessor<float>    BosonTag::s_ECF3 ("ECF3");


BosonTag::CONFIG::CONFIG() :
  m_mass_params(2, -99.9),
  m_mass_window(0.01),
  m_d2_params(5, -99.9),
  m_d2_cut_direction(""),
  m_isConfig(false)
{}

bool BosonTag::CONFIG::setConfigs(const std::vector<float> mass_params, float mass_window, const std::vector<float> d2_params, const std::string d2_cut_direction){
  if(mass_params.size() != m_mass_params.size()){
    printf("<%s>: mass_params.size() is incorrect.\r\n\tExpected: %zu\r\n\tGiven:    %zu\r\n", APP_NAME, m_mass_params.size(), mass_params.size());
    return false;
  }
  if(d2_params.size() != m_d2_params.size()){
    printf("<%s>: d2_params.size() is incorrect.\r\n\tExpected: %zu\r\n\tGiven:    %zu\r\n", APP_NAME, m_d2_params.size(), d2_params.size());
    return false;
  }
  if(d2_cut_direction != "LEFT" && d2_cut_direction != "RIGHT"){
    printf("<%s>: d2_cut_direction is not valid.\r\n\tExpected: LEFT, RIGHT\r\n\tGiven:    %s\r\n", APP_NAME, d2_cut_direction.c_str());
    return false;
  }
  m_mass_params = mass_params;
  m_mass_window = mass_window;
  m_d2_params = d2_params;
  m_d2_cut_direction = d2_cut_direction;
  return m_isConfig = true;
}

BosonTag::BosonTag( std::string working_point,
                    std::string tagger_alg,
                    std::string recommendations_file,
                    bool debug,
                    bool verbose) :
  m_working_point(working_point),
  m_tagger_alg(tagger_alg),
  m_recommendations_file(recommendations_file),
  m_debug(debug),
  m_verbose(verbose),
  m_bad_configuration(false)
{

  /* check configurations passed in, use m_bad_configuration as flag:
        - flag it to true if something is badly configured
        - otherwise, it should be false if everything seems ok
  */

  if(m_verbose)
    printf("<%s>: Attempting to configure with\r\n\t"
            "Working Point     %s\r\n\t"
            "Tagger Algorithm  %s\r\n\t"
            "Recommendations   %s\r\n\t"
            "Debug Output?     %s\r\n\t"
            "Verbose Output?   %s\r\n"
            "=========================================\r\n",
            APP_NAME, m_working_point.c_str(), m_tagger_alg.c_str(),
            m_recommendations_file.c_str(),
            m_debug?"Yes":"No", m_verbose?"Yes":"No");

  std::set<std::string> validWorkingPoints;
  validWorkingPoints.insert("tight");
  validWorkingPoints.insert("medium");
  validWorkingPoints.insert("loose");
  validWorkingPoints.insert("veryloose");

  if( validWorkingPoints.find(m_working_point) == validWorkingPoints.end()){
    printf("<%s>: Unknown working point requested.\r\n\tExpected: veryloose, loose, medium, tight\r\n\tGiven:    %s\r\n", APP_NAME, m_working_point.c_str());
    m_bad_configuration |= true;
  } else {
    if(m_verbose) printf("<%s>: Valid working point requested.\r\n", APP_NAME);
  }

  std::set<std::string> validTaggers;
  validTaggers.insert("smooth");
  validTaggers.insert("run1");

  if( validTaggers.find(m_tagger_alg) == validTaggers.end()){
    printf("<%s>: Unknown taggger requested.\r\n\tExpected: smooth, run1\r\n\tGiven:    %s\r\n", APP_NAME, m_tagger_alg.c_str());
    m_bad_configuration |= true;
  } else {
    if(m_verbose) printf("<%s>: Valid tagger algorithm requested.\r\n", APP_NAME);
  }

  // only care about recommendations file if we use smooth algorithm
  // no need to parse a file if we use the Run-1 tagger for example
  if( m_tagger_alg == "smooth" ){
    if( m_recommendations_file.empty() ){
      printf("<%s>: You choose the 'smooth' tagger but did not provide a recommendations file.\r\n", APP_NAME);
      m_bad_configuration |= true;
    } else {
      if(m_verbose) printf("<%s>: Looking for the recommendations file provided.\r\n", APP_NAME);

#ifdef ROOTCORE
      m_recommendations_file = gSystem->ExpandPathName(m_recommendations_file.c_str());
#else
      m_recommendations_file = PathResolverFindXMLFile(m_recommendations_file);
#endif
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
        }

        std::string line;
        while( std::getline(f_in, line) ){
          /* token contains the current splitted text */
          std::string token;

          if(m_verbose) printf("<%s>: Reading in line\r\n\t'%s'\r\n", APP_NAME, line.c_str());

          /* notes, one should add a check on all values to make sure they are valid! */

          // split by space
          std::istringstream ss(line);
          /* lineDetails is an array of the splits */
          std::vector<std::string> lineDetails{std::istream_iterator<std::string>{ss}, std::istream_iterator<std::string>{}};

          std::string l_working_point = lineDetails[0];
          std::string l_algorithm     = lineDetails[1];

          std::vector<float> l_mass_params(2, 0.0);
          for(int i=0; i < 2; i++)
            l_mass_params[i] = std::stof(lineDetails[i+2]);

          float l_mass_window = std::stof(lineDetails[4]);

          std::vector<float> l_d2_params(5, 0.0);
          for(int i=0; i<5; i++)
            l_d2_params[i] = std::stof(lineDetails[i+5]);

          std::string l_d2_cut_direction = lineDetails[10];

          // build up a new config for the line
          BosonTag::CONFIG new_config = {};
          bool res = new_config.setConfigs(l_mass_params, l_mass_window, l_d2_params, l_d2_cut_direction);
          if(!res){
            printf("<%s>: Could not create a configuration object for the line in the recommendations file.\r\n\tLine: %s\r\n", APP_NAME, line.c_str());
            m_bad_configuration |= true;
            break;
          }

          // add it to all configurations we have
          m_configurations[l_working_point][l_algorithm] = new_config;

          if(m_verbose) printf("<%s>: Parsed line as\r\n\t0: %s\r\n\t1: %s\r\n\t2: %0.10f\r\n\t3: %0.10f\r\n\t4: %0.10f\r\n\t5: %0.10f\r\n\t6: %0.10f\r\n\t7: %0.10f\r\n\t8: %0.10f\r\n\t9: %0.10f\r\n\t10: %s\r\n", APP_NAME,
                              l_working_point.c_str(), l_algorithm.c_str(), l_mass_params[0], l_mass_params[1], l_mass_window,
                              l_d2_params[0], l_d2_params[1], l_d2_params[2], l_d2_params[3], l_d2_params[4], l_d2_cut_direction.c_str());
        }

        if( !m_configurations.count(m_working_point) ){
          printf("<%s>: Could not find working point `%s` in recommendations file!\r\n", APP_NAME, m_working_point.c_str());
          m_bad_configuration |= true;
        } else {
          if(m_verbose) printf("<%s>: Found a configuration for the given working point in the recommendations file.\r\n", APP_NAME);
        }

      }
    }
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
    std::cout << "|            BosonTagger is misconfigured!            |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|=====================================================|" << std::endl;
  } else {
    if(m_verbose) printf("<%s>: BosonTagger is configured successfuly! Congratulations on such an achievement.\r\n", APP_NAME);
  }

}

std::pair<bool, std::string> BosonTag::get_algorithm_name(const xAOD::Jet& jet,
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


std::pair<bool, BosonTag::CONFIG> BosonTag::get_configuration(std::string algorithm_name) const {
  if( !m_configurations.at(m_working_point).count(algorithm_name) ){
    if(m_debug) printf("<%s>: Could not find jet algorithm name `%s` in recommendations file!\r\n", APP_NAME, algorithm_name.c_str());
    return std::pair<bool, BosonTag::CONFIG>(false, {});
  } else {
   if(m_verbose) printf("<%s>: Introspected the jet to be of type `%s`.\r\n", APP_NAME, algorithm_name.c_str());
  }

  return std::pair<bool, BosonTag::CONFIG>(true, m_configurations.at(m_working_point).at(algorithm_name));
}

int BosonTag::result(const xAOD::Jet& jet) const
{
  // bad configuration
  if(m_bad_configuration){
    if(m_debug) printf("<%s>: BosonTag has a bad configuration!", APP_NAME);
    return -9;
  }

  // if we call via this method, we need these 4 things defined
  if( !s_AlgorithmType.isAvailable(jet) ){
    if(m_debug) printf("<%s>: s_AlgorithmType is not defined for the jet.\r\n", APP_NAME);
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
    return false;
  } else {
    if(m_verbose) printf("<%s>: Jet introspection successful.\r\n", APP_NAME);
  }

  return result(jet, res.second);
}

int BosonTag::result(const xAOD::Jet& jet, std::string algorithm_name) const {
  // bad configuration
  if(m_bad_configuration){
    if(m_debug) printf("<%s>: BosonTag has a bad configuration!\r\n", APP_NAME);
    return -9;
  }

  // check basic kinematic selection
  if(jet.pt()/1.e3 < 200.0 || std::fabs(jet.eta()) > 2.0){
    if(m_verbose) printf("<%s>: Jet does not pass basic kinematic selection. pT > 200 GeV, |eta| < 2.0\r\n\tJet Pt: %0.6f GeV\r\n\tJet |eta|: %0.6f\r\n", APP_NAME, jet.pt()/1.e3, std::fabs(jet.eta()));
    return -5;
  }
  if(jet.pt()/1.e3 > 2000.0){
    if(m_verbose) printf("<%s>: Warning, jet has pT > 2 TeV!\r\nJet Pt: %0.6f GeV", APP_NAME, jet.pt()/1.e3);
    return -5;
  }

  // overall result
  int passMass(0);
  int passSub(0);

  // buffer holder to make it easier to print messages for verbosity
  std::string buffer;

  if(m_tagger_alg == "smooth"){
    // could not find a configuration for the particular jet
    std::pair<bool, BosonTag::CONFIG> c = get_configuration(algorithm_name);
    if(!c.first){
      if(m_debug) printf("<%s>: (smooth) The given jet does not have a configuration parameter.\r\n", APP_NAME);
      return -9;
    }

    // start with the mass window
    float mean_mass_val = c.second.m_mass_params[0] + c.second.m_mass_params[1] * jet.pt()*1.e-3;
    float window_bottom = mean_mass_val - c.second.m_mass_window;
    float window_top = mean_mass_val + c.second.m_mass_window;
    if(m_verbose) printf("<%s>: (smooth) Jet pt: %0.6f GeV", APP_NAME, jet.pt()*1.e-3);
    buffer = "<%s>: (smooth) Jet %s the mass window cut.\r\n\tMass: %0.6f GeV\r\n\tMean Mass Value: %0.6f GeV\r\n\tMass Window: [ %0.6f, %0.6f ] GeV (note: +/- %0.2f GeV)\r\n";
    if(! ((window_bottom < jet.m()*1.e-3)&&(jet.m()*1.e-3 < window_top)) ){
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", jet.m()*1.e-3, mean_mass_val, window_bottom, window_top, c.second.m_mass_window);
      passMass = 0;
    } else {
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", jet.m()*1.e-3, mean_mass_val, window_bottom, window_top, c.second.m_mass_window);
      passMass = 1;
    }

    // then calculate d2 and check that
    float cut_on_d2_val = c.second.m_d2_params[0] + c.second.m_d2_params[1] * jet.pt()*1.e-3 + c.second.m_d2_params[2] * pow(jet.pt()*1.e-3, 2) + c.second.m_d2_params[3] * pow(jet.pt()*1.e-3, 3) + c.second.m_d2_params[4] * pow(jet.pt()*1.e-3, 4);
    float d2(0.0);
    if(s_D2.isAvailable(jet)){
      d2 = s_D2(jet);
    } else {
      if((!s_ECF1.isAvailable(jet) || !s_ECF2.isAvailable(jet) || !s_ECF3.isAvailable(jet))){
        if(m_debug) printf("<%s>: (smooth) D2 wasn't calculated. ECF# variables are not available.\r\n", APP_NAME);
        return -9;
      }
      d2 = s_ECF3(jet) * pow(s_ECF1(jet), 3.0) / pow(s_ECF2(jet), 3.0);
    }

    // figure out if we tag or not
    //      RIGHT: pass if a < cut
    //      LEFT: pass if cut < a
    buffer = "<%s>: (smooth) Jet %s the D2 cut from %s\r\n\tD2: %0.6f\r\n\tCut: %0.6f\r\n";
    if(!(
          ((d2 < cut_on_d2_val)&&(c.second.m_d2_cut_direction == "RIGHT")) ||
          ((cut_on_d2_val < d2)&&(c.second.m_d2_cut_direction == "LEFT"))
        )
      ){
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", c.second.m_d2_cut_direction.c_str(), d2, cut_on_d2_val);
      passSub = 0;
    } else {
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", c.second.m_d2_cut_direction.c_str(), d2, cut_on_d2_val);
      passSub = 1;
    }

  } else if(m_tagger_alg == "run1"){
    // only use CAMKT12BDRSMU100SMALLR30YCUT4
    if(algorithm_name != "CA12BDRSM100R30Y4"){
      if(m_debug) printf("<%s>: (Run-1) You can only use Run-1 Tagger on CA12 BDRS M100 R30 Y4 jets.\r\n", APP_NAME);
      return -9;
    }

    // at this point, we know the jet is correct, so apply a mass window cut
    buffer = "<%s>: (Run-1) Jet %s the mass window cut.\r\n\tMass: %0.6f GeV\r\n\tMass Window: [ 69.00, 107.00 ] GeV\r\n";
    if(! (69. < jet.m()*1.e-3 && jet.m()*1.e-3 < 107.) ){
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", jet.m()*1.e-3);
      passMass = 0;
    } else {
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", jet.m()*1.e-3);
      passMass = 1;
    }

    //  and a \sqrt{y_S} cut (on subjets moment balance)
    float ys(0.0);
    if(!s_YFilt.isAvailable(jet)){
      if(m_debug) printf("<%s>: (Run-1) Could not find YFilt on jet (subjets moment balance, y_S)\r\n", APP_NAME);
      return -9;
    }
    ys = s_YFilt(jet);
    buffer = "<%s>: (Run-1) Jet %s the sqrt{y_S} cut.\r\n\tsqrt{y_S}: %0.2f\r\n\tCut: 0.45\r\n";
    if(! (sqrt(ys) > 0.45) ){
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "failed", sqrt(ys));
      passSub = 0;
    } else {
      passSub = 1;
      if(m_verbose) printf(buffer.c_str(), APP_NAME, "passed", sqrt(ys));
    }

  } else {
    printf("<%s>: Err... how the hell did you get here?\r\n", APP_NAME);
    return -9;
  }

    if(m_verbose) printf("<%s>: Jet has passed %s!\r\n",
        APP_NAME, (passMass == 1)?((passSub == 1)?"a mass cut":"both cuts"):((passSub==1)?"a substructure cut":"neither cuts"));

    return (passMass << 1)|(passSub << 0);

}
