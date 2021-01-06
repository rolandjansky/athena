/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/LastBinThresholdAction.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TProfile.h>
#include <TClass.h>
#include <ers/ers.h>

#include <iostream>
#include <cstdlib>
#include <regex>

#include <dqm_core/AlgorithmManager.h>

namespace {


  dqm_algorithms::LastBinThresholdAction<dqm_algorithms::GreaterThan, dqm_algorithms::NoAction>
  LastBinGreaterThanThreshold("LastBinGreaterThanThreshold");

  dqm_algorithms::LastBinThresholdAction<dqm_algorithms::LessThan, dqm_algorithms::NoAction>
  LastBinLessThanThreshold("LastBinLessThanThreshold");

  dqm_algorithms::LastBinThresholdAction<dqm_algorithms::DifferentThan, dqm_algorithms::NoAction>
  LastBinDifferentThanThreshold("LastBinDifferentThanThreshold");

  dqm_algorithms::LastBinThresholdAction<dqm_algorithms::GreaterThan, dqm_algorithms::TileDQAction>
  TileDataCorruptionFractionGreaterThanThreshold("TileDataCorruptionGreaterThanThreshold");
}

void dqm_algorithms::TileDQAction::operator() (const std::string& histogramName, std::string action,
                                               double averageBinContent, double lastBinContent) const {

  const char* actionPath = std::getenv("TILE_DQ_ACTION_PATH");
  if (actionPath != nullptr) action = std::string(actionPath) + "/" + action;

  std::smatch match;
  std::regex expression (".*([LE]B[AC]\\d\\d).*");

  std::string module("UNKNOWN");
  if (std::regex_search(histogramName, match, expression) && match.size() > 1) {
    module = match.str(1);
  }

  action += " ";
  action += histogramName;
  action += " ";
  action += module;
  action += " ";
  action += std::to_string(lastBinContent);
  action += " ";
  action += std::to_string(averageBinContent);
  action += " &";
  std::system(action.c_str());
}


template<class Exceed, class Action>
dqm_algorithms::LastBinThresholdAction<Exceed, Action>::LastBinThresholdAction(const std::string& name)
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

template<class Exceed, class Action>
dqm_algorithms::LastBinThresholdAction<Exceed, Action>*
dqm_algorithms::LastBinThresholdAction<Exceed, Action>::clone() {
  return new LastBinThresholdAction(m_name);
}


template<class Exceed, class Action> dqm_core::Result*
dqm_algorithms::LastBinThresholdAction<Exceed, Action>::execute(const std::string&  name,
                                                                const TObject& object,
                                                                const dqm_core::AlgorithmConfig& config )
{
  const TProfile* histogram;

  if( object.IsA()->InheritsFrom( "TProfile" ) ) {
    histogram = dynamic_cast<const TProfile*>(&object);
    if (histogram->GetDimension() > 1){
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TProfile" );
  }

  const double minStat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  const double fixedError = dqm_algorithms::tools::GetFirstFromMap( "FixedError", config.getParameters(), -1);
  const bool ignoreEmpty = static_cast<bool>( dqm_algorithms::tools::GetFirstFromMap( "IgnoreEmpty", config.getParameters(), 1) );
  const bool publish = static_cast<bool>( dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0) );
  const int maxPublish = static_cast<int>( dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20) );
  const int nBinsToWatch = static_cast<int>( dqm_algorithms::tools::GetFirstFromMap( "NBinsToWatch", config.getParameters(), -1) );
  const int nBinsForAction = static_cast<int>( dqm_algorithms::tools::GetFirstFromMap( "NBinsForAction", config.getParameters(), 99999) );

  std::string action("");
  std::map<std::string, std::string>::const_iterator itAction = config.getGenericParameters().find("Action");
  if (itAction != config.getGenericParameters().end()) {
    action = itAction->second;
  }

  if (histogram->GetEntries() < minStat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }

  double binThreshold;
  double greenThreshold;
  double redThreshold;
  try {
    binThreshold = dqm_algorithms::tools::GetFirstFromMap( "BinThreshold", config.getParameters() );
    redThreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getRedThresholds() );
    greenThreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getGreenThresholds() );
  } catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  dqm_core::Result* result = new dqm_core::Result();

  int nBinsOverThreshold = 0;

  int firstBin = 1;
  int lastBin = histogram->GetNbinsX();

  if (nBinsToWatch > 0) {
    while ((histogram->GetBinEntries(lastBin) == 0) && (lastBin > 1)) --lastBin;
    firstBin = (lastBin > nBinsToWatch) ? (lastBin - nBinsToWatch + 1) : 1;

    result->tags_["LastBinNumber"] = lastBin; // report where we began the checks (mostly for debugging)
    result->tags_["LastBinCenter"] = histogram->GetBinCenter(lastBin); // report where that is on the x-axis
  }

  double lastBinOverThresholdContent(0.0);
  double binsOverThresholdContent(0.0);

  for (int bin = firstBin; bin <= lastBin; ++bin) {
    if (ignoreEmpty && (histogram->GetBinEntries(bin) == 0)) {
      continue;
    }
    double content = histogram->GetBinContent(bin);
    if (m_exceeds(content, binThreshold, fixedError)) {
      ++nBinsOverThreshold;
      lastBinOverThresholdContent = content;
      binsOverThresholdContent += content;
      if (publish && nBinsOverThreshold < maxPublish){
        dqm_algorithms::tools::PublishBin(histogram, bin, 1, content, result);
      }
    }
  }

  ERS_DEBUG(1,"Number of bins exceeded threshold of " << binThreshold << " is " << nBinsOverThreshold );
  ERS_DEBUG(1,"Green threshold: "<< greenThreshold << " bin(s);   Red threshold : " << redThreshold << " bin(s) ");

  result->tags_["NBins"] = nBinsOverThreshold;
  if (greenThreshold > redThreshold) {
    if (nBinsOverThreshold >= greenThreshold) {
      result->status_ = dqm_core::Result::Green;
    } else if (nBinsOverThreshold > redThreshold) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
  } else {
    if (nBinsOverThreshold <= greenThreshold) {
      result->status_ = dqm_core::Result::Green;
    } else if (nBinsOverThreshold < redThreshold) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
  }

  if (!action.empty() && nBinsOverThreshold >= nBinsForAction) {
    double averageBinContent = binsOverThresholdContent / nBinsOverThreshold;
    std::string histogramName(histogram->GetName());
    m_doAction(histogramName, action, lastBinOverThresholdContent, averageBinContent);
  }

  return result;

}

template<class Exceed, class Action>
void dqm_algorithms::LastBinThresholdAction<Exceed, Action>::printDescription(std::ostream& out) const {

  out << m_name + ": Checks for number of bins exceded threshold value" << std::endl;
  out << "Mandatory Parameter: BinThreshold: Look for bins exceeded BinTreshold; Count number of bins satifying requirement" << std::endl;
  out << "Mandatory Green/Red Threshold: NBins: Number of bins satifying BinThreshold constraint to give Green/Red result" << std::endl;

  out << "Optional Parameter: FixedError: override the histogram errors with this value" << std::endl;
  out << "Optional Parameter: IgnoreEmpty: Ignore bins which have zero entries in histogram" << std::endl;
  out << "Optional Parameter: PublishBins: Save bins which are different from average in Result (set to 1)" << std::endl;
  out << "Optional Parameter: MaxPublish: Max number of bins to save (default 20)" << std::endl;
  out << "Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm" << std::endl;
  out << "Optional parameter: NBinsToWatch - number of final bins that will be checked. (NBinsToWatch >= 1, default = -1)" << std::endl;
}


// Make sure these are completely instantiated here, as the definitions
// are only in this file.
template class dqm_algorithms::LastBinThresholdAction<dqm_algorithms::GreaterThan, dqm_algorithms::TileDQAction>;
template class dqm_algorithms::LastBinThresholdAction<dqm_algorithms::DifferentThan, dqm_algorithms::TileDQAction>;
