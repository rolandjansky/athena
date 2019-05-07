/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "AthenaMonitoring/HistogramDef.h"

using namespace Monitored;

typedef boost::tokenizer<boost::char_separator<char>> tokenizer_t;

const HistogramDef HistogramDef::parse(const std::string &histogramDefinition) {
  HistogramDef result;

  try {
    std::vector<std::string> properties = splitWithSeparator(histogramDefinition, ",");
    std::vector<std::string>::iterator propertiesIterator = properties.begin();

    if (properties.size() < 5) {
      return result;
    }

    result.path = nextProperty(propertiesIterator);
    result.type = nextProperty(propertiesIterator);
    result.weight = nextProperty(propertiesIterator);
    result.name.push_back(nextProperty(propertiesIterator));

    if (result.type.find("TH2") == 0 || result.type == "TProfile" || result.type == "TEfficiency") {
      result.name.push_back(nextProperty(propertiesIterator));
    } else if (result.type == "TProfile2D") {
      result.name.push_back(nextProperty(propertiesIterator));
      result.name.push_back(nextProperty(propertiesIterator));
    }

    resolveAlias(result);
  
    result.title = nextProperty(propertiesIterator);

    resolveAxies(result, properties, propertiesIterator);

    // check if there are bin labels (e.g. str1:str2:str3:str4)
    if (propertiesIterator != properties.end() && propertiesIterator->find(":") != std::string::npos) {
      std::vector<std::string> splitResult = splitWithSeparator(nextProperty(propertiesIterator), ":");

      result.labels.insert(result.labels.end(), splitResult.begin(), splitResult.end());
    }

    // opt
    if (propertiesIterator != properties.end()) {
      result.opt = (*propertiesIterator);
    }

    result.ok = true;
    return result;
  } catch (TokenException &ex) {
    throw HistogramDefParseException(result.name[0] + " NOT booked: " + ex.what());
  }
}

std::vector<std::string> HistogramDef::splitWithSeparator(const std::string &input, const char *separator) {
  boost::char_separator<char> sep(separator);
  tokenizer_t tokens(input, sep);
  std::vector<std::string> result;

  for (tokenizer_t::iterator itr = tokens.begin(); itr != tokens.end(); ++itr) {
    std::string word = *itr;
    boost::trim(word);
    result.push_back(word);
  }

  return result;
}

std::string HistogramDef::nextProperty(std::vector<std::string>::iterator &propertiesIterator) {
  return *(propertiesIterator++);
}

template<typename T>
T HistogramDef::parseToken(const std::string &token, const std::string &errorDescription) {
    try {
      return boost::lexical_cast<T>(token);
    } catch (boost::bad_lexical_cast &) {
      throw TokenException(errorDescription);
    }
}

void HistogramDef::resolveAlias(HistogramDef &histogramDefinition) {
    std::string::size_type semicolon_pos = histogramDefinition.name.back().find(';');

    if (semicolon_pos != std::string::npos) {
      std::string actual_name = histogramDefinition.name.back().substr(0, semicolon_pos);
      histogramDefinition.alias = histogramDefinition.name.back().substr(semicolon_pos + 1);
      histogramDefinition.name.back() = actual_name;
    } else {
      histogramDefinition.alias = histogramDefinition.name[0];

      if (histogramDefinition.name.size() > 1) {
        histogramDefinition.alias += "_vs_" + histogramDefinition.name[1];
      } 

      if (histogramDefinition.name.size() > 2) {
        histogramDefinition.alias += "_vs_" + histogramDefinition.name[2];
      }
    }
}

void HistogramDef::resolveAxies(HistogramDef &histogramDefinition, std::vector<std::string> &properties, std::vector<std::string>::iterator &propertiesIterator) {
  if (distance(propertiesIterator, properties.end()) < 3) {
    throw TokenException("NOT enough parameters for defining 1-D histogram");
  }

  histogramDefinition.xbins = parseToken<int>(nextProperty(propertiesIterator), "int expected for xbins");
  histogramDefinition.xmin = parseToken<double>(nextProperty(propertiesIterator), "double expected for xmin");
  histogramDefinition.xmax = parseToken<double>(nextProperty(propertiesIterator), "double expected for xmax");

  if (histogramDefinition.type.find("TH2") == 0) {
    if (distance(propertiesIterator, properties.end()) < 3) {
      throw TokenException("y-axis definition expected for TH2");
    }

    histogramDefinition.ybins = parseToken<int>(nextProperty(propertiesIterator), "int expected for ybins");
    histogramDefinition.ymin = parseToken<double>(nextProperty(propertiesIterator), "double expected for ymin");
    histogramDefinition.ymax = parseToken<double>(nextProperty(propertiesIterator), "double expected for ymax");
  } else if (histogramDefinition.type == "TProfile2D") {
    if (distance(propertiesIterator, properties.end()) < 3) {
      throw TokenException("y-axis definition expected for TProfile2D");
    }

    histogramDefinition.ybins = parseToken<int>(nextProperty(propertiesIterator), "int expected for ybins");
    histogramDefinition.ymin = parseToken<double>(nextProperty(propertiesIterator), "double expected for ymin");
    histogramDefinition.ymax = parseToken<double>(nextProperty(propertiesIterator), "double expected for ymax");

    if (distance(propertiesIterator, properties.end()) >= 2) {
      histogramDefinition.zmin = parseToken<double>(nextProperty(propertiesIterator), "double expected for zmin of TProfile2D");
      histogramDefinition.zmax = parseToken<double>(nextProperty(propertiesIterator), "double expected for zmax of TProfile2D");
      histogramDefinition.zcut = true;
    }
  }
}
