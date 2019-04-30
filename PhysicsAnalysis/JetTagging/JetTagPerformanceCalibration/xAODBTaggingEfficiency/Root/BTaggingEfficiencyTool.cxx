/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTagging/BTagging.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"

#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"

#include "TSpline.h"

#include <algorithm>

using CP::CorrectionCode;
using CP::SystematicSet;
using CP::SystematicVariation;
using CP::SystematicCode;
using CP::SystematicRegistry;

using Analysis::Uncertainty;
using Analysis::CalibrationDataVariables;
using Analysis::CalibResult;
using Analysis::CalibrationStatus;
using Analysis::Total;
using Analysis::SFEigen;
using Analysis::SFNamed;
using Analysis::None;
using Analysis::Extrapolation;
using Analysis::TauExtrapolation;

using xAOD::IParticle;

// The following essentially  duplicates code that already exists in package ParticleJetTools (in JetFlavourInfo.cxx).
// This duplication isn't desirable, but the alternative (to use ParticleJetTools directly) pulls in a lot of dependencies.

namespace {

  int GAFinalHadronFlavourLabel (const xAOD::Jet& jet) {

    const std::string labelB = "GhostBHadronsFinal";
    const std::string labelC = "GhostCHadronsFinal";
    const std::string labelTau = "GhostTausFinal";

    std::vector<const IParticle*> ghostB;
    if (jet.getAssociatedObjects<IParticle>(labelB, ghostB) && ghostB.size() > 0) return 5;
    std::vector<const IParticle*> ghostC;
    if (jet.getAssociatedObjects<IParticle>(labelC, ghostC) && ghostC.size() > 0) return 4;
    std::vector<const IParticle*> ghostTau;
    if (jet.getAssociatedObjects<IParticle>(labelTau, ghostTau) && ghostTau.size() > 0) return 15;
    return 0;
  }

  int ConeFinalPartonFlavourLabel (const xAOD::Jet& jet) {
    // default label means "invalid"
    int label = -1;

    // First try the new naming scheme
    if (jet.getAttribute("ConeTruthLabelID",label)) return label;
    // If that fails, revert to the old scheme. In this case, further testing is not very useful
    jet.getAttribute("TruthLabelID", label);
    return label;
  }

  int ExclusiveConeHadronFlavourLabel (const xAOD::Jet& jet, bool doExtended = false) {
    // default label means "invalid"
    int label = -1;

    // We don't check the return value, as we would not be able to handle it gracefully anyway
    if (doExtended) {
      jet.getAttribute("HadronConeExclExtendedTruthLabelID",label);
    } else {
      jet.getAttribute("HadronConeExclTruthLabelID",label);
    }
    return label;
  }

  int jetFlavourLabel (const xAOD::Jet& jet, bool doConeLabelling, bool doOldLabelling, bool doExtended) {
    if (doConeLabelling)
      return (doOldLabelling) ? ConeFinalPartonFlavourLabel(jet) : ExclusiveConeHadronFlavourLabel(jet, doExtended);
    else
      return GAFinalHadronFlavourLabel(jet);
  }


  // local utility function: trim leading and trailing whitespace in the property strings
  std::string trim(const std::string& str,
		   const std::string& whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
  }

  // local utility function: split string into a vector of substrings separated by a specified separator
  std::vector<std::string> split(const std::string& str, char token = ';') {
    std::vector<std::string> result;
    if (str.size() > 0) {
      std::string::size_type end;
      std::string tmp(str);
      do {
	end = tmp.find(token);
	std::string entry = trim(tmp.substr(0,end));
	if (entry.size() > 0) result.push_back(entry); 
	if (end != std::string::npos) tmp = tmp.substr(end+1);
      } while (end != std::string::npos);
    }
    return result;
  }
}

BTaggingEfficiencyTool::BTaggingEfficiencyTool( const std::string & name) : asg::AsgTool( name ), m_selectionTool("") {
  declareProperty("TaggerName",                          m_taggerName="",               "tagging algorithm name as specified in CDI file");
  declareProperty("OperatingPoint",                      m_OP="",                       "operating point as specified in CDI file");
  declareProperty("JetAuthor",                           m_jetAuthor="",                "jet collection & JVF/JVT specification in CDI file");
  declareProperty("ScaleFactorFileName",                 m_SFFile = "",                 "name of the official scale factor calibration CDI file (uses PathResolver)");
  declareProperty("UseDevelopmentFile",                  m_useDevFile = false,          "specify whether or not to use the (PathResolver) area for temporary scale factor calibration CDI files");
  declareProperty("EfficiencyFileName",                  m_EffFile = "",                "name of optional user-provided MC efficiency CDI file");
  declareProperty("ScaleFactorBCalibration",             m_SFNames["B"] = "default",    "name of b-jet scale factor calibration object");
  declareProperty("ScaleFactorCCalibration",             m_SFNames["C"] = "default",    "name of c-jet scale factor calibration object");
  declareProperty("ScaleFactorTCalibration",             m_SFNames["T"] = "default",    "name of tau-jet scale factor calibration object");
  declareProperty("ScaleFactorLightCalibration",         m_SFNames["Light"] = "default","name of light-flavour jet scale factor calibration object");
  declareProperty("EigenvectorReductionB",               m_EVReduction["B"] = "Loose",  "b-jet scale factor Eigenvector reduction strategy; choose between 'Loose', 'Medium', 'Tight'");
  declareProperty("EigenvectorReductionC",               m_EVReduction["C"] = "Loose",  "c-jet scale factor Eigenvector reduction strategy; choose between 'Loose', 'Medium', 'Tight'");
  declareProperty("EigenvectorReductionLight",           m_EVReduction["Light"] = "Loose", "light-flavour jet scale factor Eigenvector reduction strategy; choose between 'Loose', 'Medium', 'Tight'");
  declareProperty("EfficiencyBCalibrations",             m_EffNames["B"] = "default",   "(semicolon-separated) name(s) of b-jet efficiency object(s)");
  declareProperty("EfficiencyCCalibrations",             m_EffNames["C"] = "default",   "(semicolon-separated) name(s) of c-jet efficiency object(s)");
  declareProperty("EfficiencyTCalibrations",             m_EffNames["T"] = "default",   "(semicolon-separated) name(s) of tau-jet efficiency object(s)");
  declareProperty("EfficiencyLightCalibrations",         m_EffNames["Light"] = "default", "(semicolon-separated) name(s) of light-flavour-jet efficiency object(s)");
  declareProperty("UncertaintyBSuffix",                  m_uncertaintySuffixes["B"] = "","optional suffix for b-jet uncertainty naming");
  declareProperty("UncertaintyCSuffix",                  m_uncertaintySuffixes["C"] = "","optional suffix for c-jet uncertainty naming");
  declareProperty("UncertaintyTSuffix",                  m_uncertaintySuffixes["T"] = "","optional suffix for tau-jet uncertainty naming");
  declareProperty("UncertaintyLightSuffix",              m_uncertaintySuffixes["Light"] = "","optional suffix for light-flavour-jet uncertainty naming");
  declareProperty("ExcludeFromEigenVectorTreatment",     m_excludeFromEV = "",          "(semicolon-separated) names of uncertainties to be excluded from all eigenvector decompositions (if used)");
  declareProperty("ExcludeFromEigenVectorBTreatment",    m_excludeFlvFromEV["B"] = "",  "(semicolon-separated) names of uncertainties to be excluded from b-jet eigenvector decomposition (if used)");
  declareProperty("ExcludeFromEigenVectorCTreatment",    m_excludeFlvFromEV["C"] = "",  "(semicolon-separated) names of uncertainties to be excluded from c-jet eigenvector decomposition (if used)");
  declareProperty("ExcludeFromEigenVectorLightTreatment",m_excludeFlvFromEV["Light"] = "", "(semicolon-separated) names of uncertainties to be excluded from light-flavour-jet eigenvector decomposition (if used)");
  declareProperty("ExcludeRecommendedFromEigenVectorTreatment", m_useRecommendedEVExclusions = false, "specify whether or not to add recommended lists to the user specified eigenvector decomposition exclusion lists");
  // declareProperty("ExcludeJESFromEVTreatment",        m_excludeJESFromEV = true,     "specify whether or not to exclude JES uncertainties from eigenvector decomposition (if used)");
  declareProperty("SystematicsStrategy",                 m_systStrategy = "SFEigen",    "name of systematics model; presently choose between 'SFEigen' and 'Envelope'");
  declareProperty("ConeFlavourLabel",                    m_coneFlavourLabel = true,     "specify whether or not to use the cone-based flavour labelling instead of the default ghost association based labelling");
  declareProperty("ExtendedFlavourLabel",                m_extFlavourLabel = false,     "specify whether or not to use an 'extended' flavour labelling (allowing for multiple HF hadrons or perhaps partons)");
  declareProperty("OldConeFlavourLabel",                 m_oldConeFlavourLabel = false, "when using cone-based flavour labelling, specify whether or not to use the (deprecated) Run-1 legacy labelling");
  declareProperty("IgnoreOutOfValidityRange",            m_ignoreOutOfValidityRange = false, "ignore out-of-extrapolation-range errors as returned by the underlying tool");
  // initialise some variables needed for caching
  // TODO : add configuration of the mapIndices - rather than just using the default of 0
  //m_mapIndices["Light"] = m_mapIndices["T"] = m_mapIndices["C"] = m_mapIndices["B"] = 0;
  m_initialised = false;
  // m_getTagWeight = 0;
  m_applySyst = false;
  m_isContinuous = false;

  // declare the selection tool to be private (not absolutely sure this is needed?)
  m_selectionTool.declarePropertyFor(this, "BTaggingSelectionTool", "selection tool to be used internally");
}

BTaggingEfficiencyTool::~BTaggingEfficiencyTool() {
  delete m_CDI;
}

// /// Silly copy constructor for the benefit of dictionary generation
// BTaggingEfficiencyTool::BTaggingEfficiencyTool(const BTaggingEfficiencyTool& other) :
//   IBTaggingEfficiencyTool(other), CP::ISystematicsTool(other), asg::AsgTool(other.name()+"_copy"),
//   m_path(other.m_path), m_SFFile(other.m_SFFile), m_EffFile(other.m_EffFile),
//   m_SFBName(other.m_SFBName), m_SFCName(other.m_SFCName), m_SFTName(other.m_SFTName), m_SFLightName(other.m_SFLightName),
//   m_EffBName(other.m_EffBName), m_EffCName(other.m_EffCName), m_EffTName(other.m_EffTName), m_EffLightName(other.m_EffLightName),
//   m_excludeFromEV(other.m_excludeFromEV), m_useDevFile(other.m_useDevFile), m_coneFlavourLabel(other.m_coneFlavourLabel),
//   m_initialised(other.m_initialised), m_applySyst(other.m_applySyst),
//   m_applyThisSyst(other.m_applyThisSyst), m_systematicsInfo(other.m_systematicsInfo), m_systematics(other.m_systematics),
//   m_OP(other.m_OP), m_jetAuthor(other.m_jetAuthor), m_isContinuous(other.m_isContinuous), m_getTagWeight(other.m_getTagWeight),
//   m_mapIndices(other.m_mapIndices), m_SFIndices(other.m_SFIndices), m_EffIndices(other.m_EffIndices) {

//   // The one dynamically allocated data member is the CDI
//   m_CDI = new Analysis::CalibrationDataInterfaceROOT(*(other.m_CDI));
//   ATH_MSG_INFO( "Instantiated copy CalibrationDataInterfaceROOT object at " << m_CDI);
// }

StatusCode BTaggingEfficiencyTool::initialize() {
  ATH_MSG_INFO( " Hello BTaggingEfficiencyTool user... initializing");
  ATH_MSG_INFO( " TaggerName = " << m_taggerName);
  ATH_MSG_INFO( " OP = " << m_OP);
  ATH_MSG_INFO( " b-jet     SF/eff calibration = " << m_SFNames["B"] <<     " / " << m_EffNames["B"]);
  ATH_MSG_INFO( " c-jet     SF/eff calibration = " << m_SFNames["C"] <<     " / " << m_EffNames["C"]);
  ATH_MSG_INFO( " tau-jet   SF/eff calibration = " << m_SFNames["T"] <<     " / " << m_EffNames["T"]);
  ATH_MSG_INFO( " light-jet SF/eff calibration = " << m_SFNames["Light"] << " / " << m_EffNames["Light"]);

  if (m_OP == "Continuous") {
    // continuous tagging is special in two respects:
    // 1  the tag weight needs to be retrieved
    // 2  the generator dependent scale factor rescaling is done differently, and therefore
    //    CalibrationDataInterfaceROOT::getWeightScaleFactor() instead of
    //    CalibrationDataInterfaceROOT::getScaleFactor() must be used
    m_isContinuous = true;
    // if (m_taggerName == "MV1")
    //   m_getTagWeight = &xAOD::BTagging::MV1_discriminant;
    // else {
    //   ATH_MSG_FATAL( "No tag weight retrieval function defined for tagger = " << m_taggerName);
    // }
  }
  ATH_MSG_INFO( " JetAuthor = " << m_jetAuthor);

  std::string flavours[] = { "B", "C", "Light", "T" };
  // "pack" the efficiency map names for each flavour. Note that multiple, semicolon separated, entries may exist; so this needs to be decoded first
  std::map<std::string, std::vector<std::string> > EffNames;
  for (auto const& flavour : flavours) {
    EffNames[flavour] = split(m_EffNames[flavour]);
  }

  // Strategies for eigenvector reductions (only relevant if eigenvector variations are used, of course).
  // For now, we will assume that the strategy for tau "jets" is identical to that for c jets.
  std::map<std::string, Analysis::EVReductionStrategy> EVRedStrategies, mappings;
  mappings["Loose"] = Analysis::Loose;
  mappings["Medium"] = Analysis::Medium;
  mappings["Tight"] = Analysis::Tight;

  std::string EVflavours[] = { "B", "C", "Light" };
  for (auto const& flavour : EVflavours) {
    EVRedStrategies[flavour] = mappings.find(trim(m_EVReduction[flavour])) == mappings.end() ? mappings["Loose"] : mappings[trim(m_EVReduction[flavour])];
  }
  EVRedStrategies["T"] = EVRedStrategies["C"];

  // specify which systematic variations are to be excluded from the eigenvector decomposition
  std::map<std::string, std::vector<std::string> > excludeFromEVCov;

  // First, look for uncertainties to be excluded for all flavours
  std::vector<std::string> to_exclude = split(m_excludeFromEV);
  // use this as a starting point for all flavours (here B, C, Light)
  for (auto const& flavour : EVflavours) {
    excludeFromEVCov[flavour] = to_exclude;
  }
  // Subsequently process per-flavour lists
  for (auto const& flavour : EVflavours) {
    to_exclude = split(m_excludeFlvFromEV[flavour]);
    // Append to the existing list
    excludeFromEVCov[flavour].insert(excludeFromEVCov[flavour].end(), to_exclude.begin(), to_exclude.end());
  }
  // For the SFEigen strategy, tau "jets" are treated differently from other flavours. 
  // First, copy the charm-jet calibration settings
  excludeFromEVCov["T"] = excludeFromEVCov["C"];
  // Then ensure that the charm -> tau extrapolation uncertainty is added.
  // Technically the additional condition should never be necessary, as existing entries should not apply to tau "jets"; so this is mostly to protect users against a duplicate specification
  if (m_systStrategy != "Envelope" && std::find(excludeFromEVCov["T"].begin(), excludeFromEVCov["T"].end(), "extrapolation from charm") == excludeFromEVCov["T"].end())
    excludeFromEVCov["T"].push_back("extrapolation from charm");

  // Use the PathResolver to find the full pathname (behind the scenes this can also be used to download the file),
  // if the file cannot be found directly.
  // For consistency with the PathResolver code, use the Boost library to check on this first possibility.
  m_SFFile = trim(m_SFFile);
  std::string location = PathResolverFindCalibFile(m_SFFile);
  if (location == "") {
    std::string prepend = "";
    if (m_useDevFile) {
      ATH_MSG_WARNING(" Attempting to retrieve b-tagging scale factor calibration file from development area");
      prepend = "dev/";
    }
    prepend += "xAODBTaggingEfficiency/";
    m_SFFile = PathResolverFindCalibFile(prepend + m_SFFile);
    if (m_SFFile == "")
      ATH_MSG_WARNING(" Unable to retrieve b-tagging scale factor calibration file!");
    else
      ATH_MSG_DEBUG(" Retrieving b-tagging scale factor calibration file as " << m_SFFile);
  } else {
    m_SFFile = location;
  }
  // The situation for the efficiency file is a bit simpler since it need not reside under "xAODBTaggingEfficiency"
  m_EffFile = trim(m_EffFile);
  if (m_EffFile != "") m_EffFile = PathResolverFindCalibFile(m_EffFile);

  // Note that the instantiation below does not leave a choice: the Eigenvector variations and generator-specific scale factors are always used
  std::vector<std::string> jetAliases;
  m_CDI = new Analysis::CalibrationDataInterfaceROOT(m_taggerName,                              // tagger name: always needed
						     m_SFFile.c_str(),                          // full pathname of the SF calibration file: always needed
						     (m_EffFile == "") ? 0 : m_EffFile.c_str(), // full pathname of optional efficiency file
						     jetAliases,                                // since we configure the jet "collection name" by hand, we don't need this
						     m_SFNames,                                 // names of the scale factor calibrations to be used
						     EffNames,                                  // names of the efficiency calibrations to be used (can be multiple per flavour)
						     excludeFromEVCov,                          // names of systematic uncertainties to be excluded from the EV decomposition
						     EVRedStrategies,                           // strategies for eigenvector reductions
						     m_systStrategy != "Envelope",              // assume that eigenvector variations will be used unless the "Envelope" model is used
						     true,                                      // use MC/MC scale factors
						     false,                                     // do not use topology rescaling (only relevant for pseudo-continuous tagging)
						     m_useRecommendedEVExclusions);             // if true, add pre-set lists of uncertainties to be excluded from EV decomposition

  setMapIndex("Light",0);
  setMapIndex("C",0);
  setMapIndex("B",0);
  setMapIndex("T",0);

  ATH_MSG_INFO( "Using systematics model " << m_systStrategy);
  if (m_systStrategy != "Envelope" && m_useRecommendedEVExclusions) ATH_MSG_INFO( "excluding pre-set uncertainties from eigenvector decomposition");

  // We have a double loop over flavours here.. not nice but this is to ensure that the suffixes are always well determined before using them.
  // Note that (in the SFEigen model) suffixes are attached to eigenvector variations only (the idea being that named uncertainties, if used, are likely to be correlated).
  std::vector<std::string> suffixes;
  for (int i = 0; i < 4; ++i) {
    std::string flav = flavours[i]; if (flav == "T") flav = "C";
    // add an underscore to any specified suffix (if specified and if not already starting with a suffix)
    std::string test = trim(m_uncertaintySuffixes[flav]);
    if (test.length() > 0 && test[0] != '_') test.insert(0,"_");
    suffixes.push_back(test);
  }

  // If the tool has not already been initialised and m_OP and m_jetAuthor have been set - ie via the properties "OperatingPoint" and "JetAuthor"
  // then autmatically set things up to use these by default
  // All this must happen before registerSystematics otherwise that won't work
  for (int i = 0; i < 4; ++i) {
    unsigned int flavourID = getFlavourID(flavours[i]);
    // std::map<unsigned int, unsigned int>::const_iterator
    auto mapIter = m_SFIndices.find(flavourID);
    if( mapIter==m_SFIndices.end()) { // if the flavour doesn't have an entry need to fail the initialization
      ATH_MSG_ERROR( "No entry for flavour " << flavourID << " in SFIndices map, invalid initialization");
      return StatusCode::FAILURE;
    }
    int id = mapIter->second;

    // Implement the different strategies for dealing with uncertainties here.
    if (m_systStrategy == "SFEigen") {
      //
      // Generally recommended model: use eigenvector variations. Notes:
      // -   The list of systematics to be excluded from the eigenvector variation approach is dynamic.
      // -   The tau SF are identical to the c-jet ones, with merely one additional uncertainty assigned due to the extrapolation.
      //
      unsigned int flavourIDRef = (flavourID == 15) ? 4 : flavourID;
      int idRef = m_SFIndices.find(flavourIDRef)->second;
      // First, handle any named variations
      std::vector<std::string> systematics = m_CDI->listScaleFactorUncertainties(idRef,true);
      // Replace any spaces with underscores (this is to make ROOT browsing happy).
      // Also, remove the "extrapolation" uncertainty from the list (it will be added later under Extrapolation rather than SFNamed).
      bool hasExtrapolation = false;
      for (unsigned int i = 0; i < systematics.size(); ++i) {
	if (systematics[i] == "extrapolation") {
          hasExtrapolation = true;
          systematics.erase(systematics.begin() + i--); // don't forget to decrement i
	} else {
	  std::replace_if(systematics[i].begin(), systematics[i].end(), [] (char c) { return c == ' '; }, '_');
	  // We don't add suffixes here but only for EV variations (see JIRA: AFT-343)
	  // systematics[i].append(suffixes[i]);
	}
      }
      if (!addSystematics(systematics, flavourID, SFNamed)) {
	ATH_MSG_ERROR("SFEigen model: error adding named systematics for flavour " << getLabel(flavourIDRef) << ", invalid initialization");
	return StatusCode::FAILURE;
      }
      // Add here the extrapolation uncertainty (if it exists -- which ought to be the case).
      // "Cosmetic" fix: the outside world wants to see "FT_EFF_" prefixes. On the other hand, like for the above named uncertainties, we don't add suffixes here
      if (hasExtrapolation) {
        std::vector<std::string> extrapSyst; extrapSyst.push_back(std::string("FT_EFF_extrapolation"));
        if (! addSystematics(extrapSyst, flavourID, Extrapolation)) {
          ATH_MSG_ERROR("SFEigen model: error adding extrapolation uncertainty for flavour " << getLabel(flavourIDRef) << ", invalid initialization");
          return StatusCode::FAILURE;
        }
      }
      // And then the eigenvector variations
      std::vector<std::string> eigenSysts = makeEigenSyst(getLabel(flavourIDRef),m_CDI->getNumVariations(idRef, SFEigen), suffixes[i]);
      if (!addSystematics(eigenSysts, flavourID, SFEigen)) {
	ATH_MSG_ERROR("SFEigen model: error adding eigenvector systematics for flavour " << getLabel(flavourIDRef) << ", invalid initialization");
	return StatusCode::FAILURE;
      }
      // The above should cover all uncertainties except the charm -> tau extrapolation; so we take care of that here.
      if (flavourID == 15) {
	// First extract the complete list of uncertainties for taus
	std::vector<std::string> all_systematics = m_CDI->listScaleFactorUncertainties(id);
	// And from this list extract only this particular uncertainty (if it exists)
	const std::string s_tau_extrap = "extrapolation from charm";
	if (std::find(all_systematics.begin(), all_systematics.end(), s_tau_extrap) != all_systematics.end()) {
	  // Again, we don't add the suffix here (per JIRA: AFT-343)
	  std::string entry = "FT_EFF_extrapolation_from_charm"; // entry.append(suffixes[i]);
	  std::vector<std::string> extrapSyst; extrapSyst.push_back(entry);
	  if (! addSystematics(extrapSyst, flavourID, TauExtrapolation)) {
	    ATH_MSG_ERROR("SFEigen model: error adding charm->tau systematics for flavour " << getLabel(flavourID) << ", invalid initialization");
	    return StatusCode::FAILURE;
	  }
	}
      }
    } else if (m_systStrategy == "Envelope") {
      //
      // Simplified model: use uncertainty envelopes supplemented by a (common) extrapolation uncertainty
      // (since the extrapolation uncertainties aren't included in the Total uncertainty). Notes:
      // -   The tau SF are identical to the c-jet ones, with merely one additional uncertainty assigned due to the extrapolation.
      // -   The "total" uncertainty is always expected to be available; the code will bomb if this is not the case.
      //     Also, the "total" uncertainties for different flavours are assumed to be uncorrelated.
      //
      unsigned int flavourIDRef = (flavourID == 15) ? 4 : flavourID;
      int idRef = m_SFIndices.find(flavourIDRef)->second;
      // First, handle the Total variations; these need different prefixes to reflect them being uncorrelated
      std::vector<std::string> all_ref_systematics = m_CDI->listScaleFactorUncertainties(idRef,false);
      // std::cout << "uncertainties for flavour " << getLabel(flavourIDRef) << ":";
      // for (auto syst : all_ref_systematics) std::cout << " " << syst;
      // std::cout << std::endl;
      const std::string s_total = "systematics";
      if (std::find(all_ref_systematics.begin(), all_ref_systematics.end(), s_total) == all_ref_systematics.end()) {
	ATH_MSG_ERROR("Envelope model: required uncertainty " << s_total << " not found for flavour " << getLabel(flavourIDRef)
		      << ", invalid initialization");
	return StatusCode::FAILURE;
      }
      std::vector<std::string> totalSyst; totalSyst.push_back("FT_EFF_" + getLabel(flavourIDRef) + "_" + s_total + suffixes[i]);
      if (! addSystematics(totalSyst, flavourID, Total)) {
	  ATH_MSG_ERROR("Envelope model: error adding systematics uncertainty for flavour " << getLabel(flavourIDRef)
		      << ", invalid initialization");
	return StatusCode::FAILURE;
      }
      // Second, handle the extrapolation variations; these are shared between flavours (unless different suffixes are specified)
      const std::string s_extrap = "extrapolation";
      if (std::find(all_ref_systematics.begin(), all_ref_systematics.end(), s_extrap) != all_ref_systematics.end()) {
	std::vector<std::string> extrapSyst; extrapSyst.push_back("FT_EFF_" + s_extrap + suffixes[i]);
	if (! addSystematics(extrapSyst, flavourID, Extrapolation)) {
	  ATH_MSG_ERROR("Envelope model: error adding extrapolation uncertainty for flavour " << getLabel(flavourIDRef)
		      << ", invalid initialization");
	  return StatusCode::FAILURE;
	}
      }
      // Finally, handle the charm -> tau extrapolation (as in the above)
      if (flavourID == 15) {
	// First extract the complete list of uncertainties for taus
	std::vector<std::string> all_systematics = m_CDI->listScaleFactorUncertainties(id);
	// And from this list extract only this particular uncertainty (if it exists)
	const std::string s_tau_extrap = "extrapolation from charm";
	if (std::find(all_systematics.begin(), all_systematics.end(), s_tau_extrap) != all_systematics.end()) {
	  std::vector<std::string> extrapSyst; extrapSyst.push_back("FT_EFF_extrapolation_from_charm" + suffixes[i]);
	  if (! addSystematics(extrapSyst, flavourID, TauExtrapolation)) {
	    ATH_MSG_ERROR("Envelope model: error adding charm->tau systematics for flavour " << getLabel(flavourID) << ", invalid initialization");
	    return StatusCode::FAILURE;
	  }
	}
      }
    }
  }
  // now fill the SystematicSet
  for( std::map<SystematicVariation,SystInfo>::const_iterator mapIter = m_systematicsInfo.begin();
       mapIter != m_systematicsInfo.end();++mapIter) {
    const SystematicVariation & variation = mapIter->first;
    m_systematics.insert(variation);
  }
  // systematics framework
  SystematicRegistry & registry = SystematicRegistry::getInstance();
  if( registry.registerSystematics(*this) != SystematicCode::Ok) 
    return StatusCode::FAILURE;

  // Finally, also initialise the selection tool, if needed (for now this is the case only for DL1 tag weight computations,
  // so we do this only when DL1 is specified)
  if (m_taggerName.find("DL1") != std::string::npos) {
    m_selectionTool.setTypeAndName("BTaggingSelectionTool/" + name() + "_selection");
    ATH_CHECK( m_selectionTool.setProperty("FlvTagCutDefinitionsFileName", m_SFFile) );
    ATH_CHECK( m_selectionTool.setProperty("TaggerName",                   m_taggerName) );
    ATH_CHECK( m_selectionTool.setProperty("OperatingPoint",               m_OP) );
    ATH_CHECK( m_selectionTool.setProperty("JetAuthor",                    m_jetAuthor) );
    ATH_CHECK( m_selectionTool.retrieve() );
  }

  // if the user decides to ignore these errors, at least make her/him aware of this
  if (m_ignoreOutOfValidityRange) {
    ATH_MSG_INFO("!!!!! You have chosen to disable out-of-validity return codes -- contact the Flavour Tagging group if such jets comprise a substantial part of the phase space in your analysis !!!!!");
  }
  
  m_initialised = true;
  return StatusCode::SUCCESS;
}

CorrectionCode
BTaggingEfficiencyTool::getScaleFactor( const xAOD::Jet & jet, float & sf) const
{
  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingEfficiencyTool has not been initialised");
    return CorrectionCode::Error;
  }
  
  // get the btag label
  int flavour = jetFlavourLabel(jet, m_coneFlavourLabel, m_oldConeFlavourLabel, m_extFlavourLabel);

  Analysis::CalibrationDataVariables vars;
  if (! fillVariables(jet, vars)) {
    ATH_MSG_ERROR("unable to fill variables required for scale factor evaluation");
    return CorrectionCode::Error;
  }

  return getScaleFactor(flavour, vars, sf);
}

CorrectionCode
BTaggingEfficiencyTool::getScaleFactor( int flavour, const Analysis::CalibrationDataVariables& v,
					float & sf) const
{
  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingEfficiencyTool has not been initialised");
    return CorrectionCode::Error;
  }
  
  CalibResult result;
  
  unsigned int sfindex = 0;
  unsigned int efindex = 0;
  
  if( !getIndices(flavour,sfindex,efindex)) {
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getScaleFactor call to getIndices failed " << flavour << " " << sfindex << " " << efindex);
    return CorrectionCode::Error;
  }
  CalibrationStatus status;
  Uncertainty unc = None;
  unsigned int unc_ind=0;
  
  if( m_applySyst) {

    unc = m_applyThisSyst.uncType;
    
    if(!m_applyThisSyst.getIndex(flavour,unc_ind)) {
      ATH_MSG_VERBOSE("getScaleFactor: requested variation cannot be applied to flavour " << getLabel(flavour)
		      << ", returning nominal result");
      unc = None;
    }
  }

  status = m_isContinuous ?
    m_CDI->getWeightScaleFactor(v,sfindex,efindex,
				unc,unc_ind,result) :
    m_CDI->getScaleFactor(v,sfindex,efindex,
			  unc,unc_ind,result);

  // Interpret what has been retrieved;
  // this depends both on the uncertainty type and on the up/down setting.
  sf = result.first; // central value or up variation
  if (m_applySyst && unc != None) {
    if (! (unc == SFEigen || unc == SFNamed))
      sf += m_applyThisSyst.isUp ? result.second : -result.second ;
    else if (!m_applyThisSyst.isUp) {
      sf = result.second; // down variation
    }
  }

  // std::cout << "CDI getScaleFactor: " << sf << ", status: " << status << std::endl;
  switch (status) {
  case Analysis::kError:
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getScaleFactor call to underlying code returned a kError!");
    return CorrectionCode::Error;
  case Analysis::kExtrapolatedRange:
    return m_ignoreOutOfValidityRange ? CorrectionCode::Ok : CorrectionCode::OutOfValidityRange;
  case Analysis::kSuccess:
  default:
    return CorrectionCode::Ok;
  }
}
  
CorrectionCode
BTaggingEfficiencyTool::getEfficiency( const xAOD::Jet & jet, float & eff) const
{
  if (! m_initialised) return CorrectionCode::Error;

  // get the btag label
  int flavour = jetFlavourLabel(jet, m_coneFlavourLabel, m_oldConeFlavourLabel, m_extFlavourLabel);

  Analysis::CalibrationDataVariables vars;
  if (! fillVariables(jet, vars)) {
    ATH_MSG_ERROR("unable to fill variables required for efficiency evaluation");
    return CorrectionCode::Error;
  }

  return getEfficiency(flavour, vars, eff);
}

CorrectionCode
BTaggingEfficiencyTool::getEfficiency( int flavour, const Analysis::CalibrationDataVariables& v, 
				       float & eff) const
{
  if (! m_initialised) return CorrectionCode::Error;

  CalibResult result;

  unsigned int sfindex = 0;
  unsigned int efindex = 0;

  if( !getIndices(flavour,sfindex,efindex)) {
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getEfficiency call to getIndices failed " << flavour << " " << sfindex << " " << efindex);
    return CorrectionCode::Error;
  }
  Uncertainty unc = None;
  unsigned int unc_ind = 0;

  if( m_applySyst) {
    
    unc = m_applyThisSyst.uncType;
    // if( m_applyThisSyst.isNamed) {
    //   unc = SFNamed;
    // } else {
    //   unc = SFEigen;
    // }
    
    if(!m_applyThisSyst.getIndex(flavour,unc_ind)) {
      ATH_MSG_VERBOSE("getEfficiency: requested variation cannot be applied to flavour " << getLabel(flavour)
		      << ", returning nominal result");
      unc = None;
    }
  }

  CalibrationStatus status = m_CDI->getEfficiency(v,sfindex,efindex,
						  unc,unc_ind,result);
  // Interpret what has been retrieved;
  // this depends both on the uncertainty type and on the up/down setting.
  eff = result.first; // central value or up variation
  if (m_applySyst && unc != None) {
    if (! (unc == SFEigen || unc == SFNamed))
      eff += m_applyThisSyst.isUp ? result.second : -result.second ;
    else if (!m_applyThisSyst.isUp) {
      eff = result.second; // down variation
    }
  }
  
  switch (status) {
  case Analysis::kError:
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getEfficiency call to underlying code returned a kError!");
    return CorrectionCode::Error;
  case Analysis::kExtrapolatedRange:
    return m_ignoreOutOfValidityRange ? CorrectionCode::Ok : CorrectionCode::OutOfValidityRange;
  case Analysis::kSuccess:
  default:
    return CorrectionCode::Ok;
  }
}
  
CorrectionCode
BTaggingEfficiencyTool::getInefficiency( const xAOD::Jet & jet, float & eff) const
{
  if (! m_initialised) return CorrectionCode::Error;

  // get the btag label
  int flavour = jetFlavourLabel(jet, m_coneFlavourLabel, m_oldConeFlavourLabel, m_extFlavourLabel);

  Analysis::CalibrationDataVariables vars;
  if (! fillVariables(jet, vars)) {
    ATH_MSG_ERROR("unable to fill variables required for scale factor evaluation");
    return CorrectionCode::Error;
  }

  return getInefficiency (flavour, vars, eff);
}

CorrectionCode
BTaggingEfficiencyTool::getInefficiency( int flavour, const Analysis::CalibrationDataVariables& v, 
					 float & eff) const
{
  if (! m_initialised) return CorrectionCode::Error;

  CalibResult result;

  unsigned int sfindex = 0;
  unsigned int efindex = 0;

  if( !getIndices(flavour,sfindex,efindex)) {
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getInefficiency call to getIndices failed " << flavour << " " << sfindex << " " << efindex);
    return CorrectionCode::Error;
  }
  Uncertainty unc = None;
  unsigned int unc_ind = 0;
  if( m_applySyst) {
    
    unc = m_applyThisSyst.uncType;
    // if( m_applyThisSyst.isNamed) {
    //   unc = SFNamed;
    // } else {
    //   unc = SFEigen;
    // }
    
    if(!m_applyThisSyst.getIndex(flavour,unc_ind)) {
      ATH_MSG_VERBOSE("getInefficiency: requested variation cannot be applied to flavour " << getLabel(flavour)
		      << ", returning nominal result");
      unc = None;
    }
  }

  CalibrationStatus status = m_CDI->getInefficiency(v, sfindex, efindex,
						    unc, unc_ind, result);
  // Interpret what has been retrieved;
  // this depends both on the uncertainty type and on the up/down setting.
  // For the Total uncertainty, note also the sign change compared to e.g. getEfficiency().
  eff = result.first; // central value or up variation
  if (m_applySyst && unc != None) {
    if (! (unc == SFEigen || unc == SFNamed))
      eff += m_applyThisSyst.isUp ? -result.second : result.second ;
    else if (!m_applyThisSyst.isUp) {
      eff = result.second; // down variation
    }
  }

  switch (status) {
  case Analysis::kError:
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getInefficiency call to underlying code returned a kError!");
    return CorrectionCode::Error;
  case Analysis::kExtrapolatedRange:
    return m_ignoreOutOfValidityRange ? CorrectionCode::Ok : CorrectionCode::OutOfValidityRange;
  case Analysis::kSuccess:
  default:
    return CorrectionCode::Ok;
  }
}

CorrectionCode
BTaggingEfficiencyTool::getInefficiencyScaleFactor( const xAOD::Jet & jet, float & sf) const
{
  if (! m_initialised) return CorrectionCode::Error;

  // get the btag label
  int flavour = jetFlavourLabel(jet, m_coneFlavourLabel, m_oldConeFlavourLabel, m_extFlavourLabel);

  Analysis::CalibrationDataVariables vars;
  if (! fillVariables(jet, vars)) {
    ATH_MSG_ERROR("unable to fill variables required for scale factor evaluation");
    return CorrectionCode::Error;
  }
  
  return getInefficiencyScaleFactor( flavour, vars, sf);
}

CorrectionCode
BTaggingEfficiencyTool::getInefficiencyScaleFactor( int flavour, const Analysis::CalibrationDataVariables& v, 
						    float & sf) const
{
  if (! m_initialised) return CorrectionCode::Error;

  CalibResult result;

  unsigned int sfindex = 0;
  unsigned int efindex = 0;
  
  if( !getIndices(flavour,sfindex,efindex)) {
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getInefficiencyScaleFactor call to getIndices failed " << flavour << " " << sfindex << " " << efindex);
    return CorrectionCode::Error;
  }
  Uncertainty unc=None;
  unsigned int unc_ind=0;
  if( m_applySyst) {
    
    unc = m_applyThisSyst.uncType;
    // if( m_applyThisSyst.isNamed) {
    //   unc = SFNamed;
    // } else {
    //   unc = SFEigen;
    // }
    
    if(!m_applyThisSyst.getIndex(flavour,unc_ind)) {
      ATH_MSG_VERBOSE("getInefficiencyScaleFactor: requested variation cannot be applied to flavour " << getLabel(flavour)
		      << ", returning nominal result");
      unc = None;
    }
  }

  CalibrationStatus status = m_CDI->getInefficiencyScaleFactor(v,sfindex,efindex,
							       unc, unc_ind, result);
  // Interpret what has been retrieved;
  // this depends both on the uncertainty type and on the up/down setting.
  // For the Total uncertainty, note also the sign change compared to e.g. getScaleFactor().
  sf = result.first; // central value or up variation
  if (m_applySyst && unc != None) {
    if (! (unc == SFEigen || unc == SFNamed))
      sf += m_applyThisSyst.isUp ? -result.second : result.second ;
    else if (!m_applyThisSyst.isUp) {
      sf = result.second; // down variation
    }
  }

  switch (status) {
  case Analysis::kError:
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getInefficiencyScaleFactor call to underlying code returned a kError!");
    return CorrectionCode::Error;
  case Analysis::kExtrapolatedRange:
    return m_ignoreOutOfValidityRange ? CorrectionCode::Ok : CorrectionCode::OutOfValidityRange;
  case Analysis::kSuccess:
  default:
    return CorrectionCode::Ok;
  }
}
  
CorrectionCode
BTaggingEfficiencyTool::getMCEfficiency( const xAOD::Jet & jet, float & eff) const
{
  if (! m_initialised) return CorrectionCode::Error;

  // get the btag label
  int flavour = jetFlavourLabel(jet, m_coneFlavourLabel, m_oldConeFlavourLabel, m_extFlavourLabel);

  Analysis::CalibrationDataVariables vars;
  if (! fillVariables(jet, vars)) {
    ATH_MSG_ERROR("unable to fill variables required for scale factor evaluation");
    return CorrectionCode::Error;
  }
  
  return getMCEfficiency( flavour, vars, eff);
}

CorrectionCode
BTaggingEfficiencyTool::getMCEfficiency( int flavour, const Analysis::CalibrationDataVariables& v, 
					 float & eff) const
{
  if (! m_initialised) return CorrectionCode::Error;

  CalibResult result;

  unsigned int sfindex = 0;
  unsigned int efindex = 0;
  
  if( !getIndices(flavour,sfindex,efindex)) {
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency call to getIndices failed " << flavour << " " << sfindex << " " << efindex);
    return CorrectionCode::Error;
  }
  Uncertainty unc = None;
  // no uncertainty index here as there aren't any uncertainties associated with the MC efficiencies
  CalibrationStatus status = m_CDI->getMCEfficiency(v,efindex,
						    unc,result);
  
  eff = result.first;
  if( m_applySyst && !m_applyThisSyst.isUp) {
    eff = result.second; // down variation
  }
  
  switch (status) {
  case Analysis::kError:
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency call to underlying code returned a kError!");
    return CorrectionCode::Error;
  case Analysis::kExtrapolatedRange:
    return m_ignoreOutOfValidityRange ? CorrectionCode::Ok : CorrectionCode::OutOfValidityRange;
  case Analysis::kSuccess:
  default:
    return CorrectionCode::Ok;
  }
}

// Systematics framework - modelled on PhysicsAnalysis/AnalysisCommon/CPAnalysisExamples/Root/JetCalibrationToolExample3.cxx
// returns true if the argument systematic is supported by this tool
bool BTaggingEfficiencyTool::isAffectedBySystematic( const SystematicVariation & systematic ) const
{
  SystematicSet sys = affectingSystematics();
  return sys.find( systematic) != sys.end();
}

// this returns a list of systematics supported by this tool
SystematicSet BTaggingEfficiencyTool::affectingSystematics() const {
  return m_systematics;
}

// subset of systematics that are recommended
SystematicSet BTaggingEfficiencyTool::recommendedSystematics() const {
  return affectingSystematics();
}

const std::map<SystematicVariation, std::vector<std::string> >
BTaggingEfficiencyTool::listSystematics() const {
  std::map<SystematicVariation, std::vector<std::string> > results;

  if (! m_initialised) {
    ATH_MSG_ERROR("listSystematics() cannot be called before initialisation is finished");
    return results;
  }

  const unsigned int all_flavours[4] = { 5, 4, 15, 0 };

  for (auto info : m_systematicsInfo) {
    // The map key is easy...
    const SystematicVariation& variation = info.first;
    // Then see for which flavours this particular key is relevant
    std::vector<std::string> flavours;
    for (unsigned int flv = 0; flv < 4; ++flv) {
      unsigned int idx;
      unsigned int flavour = all_flavours[flv];
      if (info.second.getIndex(flavour, idx))
	flavours.push_back(getLabel(int(flavour)));
    }
    results[variation] = flavours;
  }
  return results;
}
///
/// This method retrieves all systematic uncertainties known to the relevant calibration objects.
/// Since the expected use of this method is in the context of the SFEigen model, we will assume (and not check) that this model is being used.
/// Note: the uncertainties returned are in the format of the underlying CDI, and do not have the rewriting applied to them that one would use in analysis.
///
std::map<std::string, std::vector<std::string> >
BTaggingEfficiencyTool::listScaleFactorSystematics(bool named) const {
  std::map<std::string, std::vector<std::string> > uncertainties;

  const unsigned int all_flavours[4] = { 5, 4, 15, 0 };
  for (unsigned int flv = 0; flv < 4; ++flv) {
    unsigned int flavourID = all_flavours[flv];
    // Assumed model: use eigenvector variations. In this model, the tau SF are identical to the c-jet ones,
    // with merely one additional uncertainty assigned due to the extrapolation.
    unsigned int flavourIDRef = (flavourID == 15) ? 4 : flavourID;
    auto mapIter = m_SFIndices.find(flavourIDRef);
    if( mapIter==m_SFIndices.end()) { // if the flavour doesn't have an entry need to fail the initialization
      ATH_MSG_ERROR( "No entry for flavour " << flavourIDRef << " in SFIndices map, invalid initialization");
      continue;
    }
    int idRef = mapIter->second;
    // Retrieve the actual list
    std::vector<std::string> systematics = m_CDI->listScaleFactorUncertainties(idRef, named);
    // For the special case of tau SF, add the extrapolation from charm.
    // Since this comes on top of the charm uncertainties, it would always be a "named" uncertainty,
    // so there is no need to check for the "named" argument.
    if (flavourID == 15) systematics.push_back("extrapolation from charm");
    uncertainties[getLabel(int(flavourID))] = systematics;
  }
  return uncertainties;
}

// WARNING the behaviour of future calls to getEfficiency and friends are modified by this
// method - it indicates which systematic shifts are to be applied for all future calls
SystematicCode BTaggingEfficiencyTool::applySystematicVariation( const SystematicSet & systConfig) {
  // First filter out any systematics that do not apply to us
  SystematicSet filteredSysts;
  if (SystematicSet::filterForAffectingSystematics(systConfig, affectingSystematics(), filteredSysts) != SystematicCode::Ok) {
    ATH_MSG_ERROR("received unsupported systematics: " << systConfig.name());
    return SystematicCode::Unsupported;
  }
  // check the size of the remaining (filtered) SystematicSet
  if (filteredSysts.size() == 0) {
    // If it is 0 then turn off systematics
    ATH_MSG_VERBOSE("empty systematics set; nothing to be done");
    m_applySyst = false;
    return SystematicCode::Ok;
  } else if (filteredSysts.size() > 1) {
    // Restriction: we allow only a single systematic variation affecting b-tagging
    ATH_MSG_WARNING("more than a single b-tagging systematic variation requested but not (yet) supported");
    return SystematicCode::Unsupported;
  } else {
    // Interpret the (single) remaining variation
    SystematicVariation var = *(filteredSysts.begin());
    auto mapIter = m_systematicsInfo.find(var);
    if (mapIter == m_systematicsInfo.end()) {
      ATH_MSG_WARNING("variation '" << var.name() << "' not found! Cannot apply");
      return SystematicCode::Unsupported;
    }
    m_applySyst = true;
    m_applyThisSyst = mapIter->second;
    ATH_MSG_VERBOSE("variation '" << var.name() << "' applied successfully");
  }
  return SystematicCode::Ok;
}
//

bool
BTaggingEfficiencyTool::fillVariables( const xAOD::Jet & jet, CalibrationDataVariables& x) const
{
  x.jetPt = jet.pt();
  x.jetEta = jet.eta();
  x.jetTagWeight = 0.;
  x.jetAuthor = m_jetAuthor;
  //bool weightOK = true;
  if (m_isContinuous) {
    const xAOD::BTagging* tagInfo = jet.btagging();
    if (!tagInfo) return false;
    // x.jetTagWeight = (tagInfo->*m_getTagWeight)();
    // For now, we defer the tag weight computation to the selection tool only in the case of DL1* (this is likely to be revisited)
    if (m_taggerName.find("DL1") != std::string::npos) {
      return (m_selectionTool->getTaggerWeight(jet, x.jetTagWeight) == CP::CorrectionCode::Ok);
    } else {
      return tagInfo->MVx_discriminant(m_taggerName, x.jetTagWeight);
    }
  }
  return true;
}

bool
BTaggingEfficiencyTool::fillVariables( const double jetPt, const double jetEta, const double jetTagWeight, CalibrationDataVariables& x) const
{
  x.jetPt = jetPt;
  x.jetEta = jetEta;
  x.jetTagWeight = jetTagWeight;
  x.jetAuthor = m_jetAuthor;

  return true;
}

// FIXME - if this method is kept - then need additional version that lets one pick the MapIndex by name - but this 
//         would also mean a change in the CDI tool to retrieve the list of names
// FIXME - this method might screw up the systematics framework by changing the list of valid systematics??? needs checking
bool
BTaggingEfficiencyTool::setMapIndex(const std::string& label, unsigned int index)
{
  // do nothing unless it's needed!
  if (m_initialised && index == m_mapIndices[label]) return true;
  // convert to integer index
  unsigned int flavour = getFlavourID(label);
  // retrieve the new calibration index
  unsigned int effIndex;
  if (m_CDI->retrieveCalibrationIndex(label, m_OP, m_jetAuthor, false, effIndex, index)) {
    // replace cached information
    m_mapIndices[label] = index;
    m_EffIndices[flavour] = effIndex;
    unsigned int sfIndex;
    if( m_CDI->retrieveCalibrationIndex(label, m_OP, m_jetAuthor, true, sfIndex, index)) {
      m_SFIndices[flavour] = sfIndex;
      return true;
    } else {
      ATH_MSG_ERROR("setMapIndex failed to find a SF calibration object" << label << " " << index);
    }
  } else {
    // flag non-existent calibration object & do nothing
    ATH_MSG_ERROR("setMapIndex failed to find an Eff calibration object" << label << " " << index);
  }
  return false;
}

// private method to generate the list of eigenvector variations
// internally these are not named, they are just numbered
// but the systematics framework needs names
std::vector<std::string> BTaggingEfficiencyTool::makeEigenSyst(const std::string & flav, int number, const std::string& suffix) {
  std::vector<std::string> systStrings;
  for(int i=0;i<number;++i) {
    std::ostringstream ost;
    ost << flav << "_" << i << suffix;
    std::string basename="FT_EFF_Eigen_"+ost.str();
    systStrings.push_back(basename);
  }
  return systStrings;
}

bool
BTaggingEfficiencyTool::getIndices(unsigned int flavour, unsigned int & sf, unsigned int & ef) const {
  // std::map<unsigned int,unsigned int>::const_iterator
  auto mapIter = m_SFIndices.find(flavour);
  if(mapIter != m_SFIndices.end()) {
    sf = mapIter->second;
  } else {
    return false;
  }
    
  mapIter = m_EffIndices.find(flavour);
  if(mapIter != m_EffIndices.end()) {
    ef = mapIter->second;
  } else {
    return false;
  }
  return true;
}

bool
BTaggingEfficiencyTool::SystInfo::getIndex( unsigned int flavourID, unsigned int & index) const {
  // std::map<unsigned int, unsigned int>::const_iterator
  auto mapIter = indexMap.find(flavourID);
  if (mapIter==indexMap.end()) {
    return false;
  } else {
    index = mapIter->second;
    return true;
  }
}

// helper method to take a list of systematic names and a flavour and add them to the map of SystematicVariation to SystInfo
// this map is used to do the lookup of which systematic to apply.
// ie it is used to map the systematics framework on the systematics approach of the CDI

bool BTaggingEfficiencyTool::addSystematics(const std::vector<std::string> & systematicNames, unsigned int flavourID,
					    Uncertainty uncType) {
  for (int i=0, n=systematicNames.size(); i<n; ++i) {
    const std::string systName = systematicNames[i];
    SystematicVariation up(systName,1);
    SystematicVariation down(systName,-1);
    std::map<SystematicVariation,SystInfo>::iterator iter = m_systematicsInfo.find(up);
    if (iter == m_systematicsInfo.end()) {
      // First case: new variation
      SystInfo info;
      info.isUp = true;
      info.uncType = uncType;
      info.indexMap[flavourID] = i;
      m_systematicsInfo[up] = info;
      ATH_MSG_VERBOSE("addSystematics: adding " << systName << " for flavour " << getLabel(flavourID));
      info.isUp = false;
      m_systematicsInfo[down]=info;
    } else {
      // Second case: already known variation. This can happen if a variation applies to more than one
      // jet flavour. Check that indeed it's not registered yet for the requested flavour.
      SystInfo info = iter->second; // make a copy
      std::map<unsigned int, unsigned int>::const_iterator indIter = info.indexMap.find(flavourID);
      if (indIter != info.indexMap.end()) {
	ATH_MSG_ERROR("addSystematics : flavourID " << flavourID << " is already in the map for uncertainty '" << systName << "', ignoring");
	continue;
      } else {
	info.indexMap[flavourID] = i;
	m_systematicsInfo[up] = info;
	ATH_MSG_VERBOSE("addSystematics: adding " << systName << " for flavour " << getLabel(flavourID));
	info.isUp = false;
	m_systematicsInfo[down] = info;
      }
    }
  }
  return true;
}
