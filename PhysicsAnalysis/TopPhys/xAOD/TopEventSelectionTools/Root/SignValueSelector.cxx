/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/SignValueSelector.h"
#include "TopConfiguration/Tokenize.h"

#include "TopEventSelectionTools/MsgCategory.h"
using namespace TopEventSelectionTools;

namespace top {
  SignValueSelector::SignValueSelector(const std::string& name, std::string params, bool multiplicityMode,
                                       bool cutValueMode) :
    m_sign(signNOIDEA),
    m_cutvalue(0),
    m_cutvalueString(""),
    m_multiplicity(-1) {
    m_cutvalueStringDelimReplace = "";
    //cutValueMode always set to false if multiplicityMode is false
    if (!multiplicityMode) cutValueMode = false;
    //get the sign and remove it from the params text
    m_sign = decodesign(params);
    if (!cutValueMode) std::istringstream(params) >> m_cutvalue >> m_multiplicity;
    else std::istringstream(params) >> m_cutvalueString >> m_multiplicity;

    //form the name string for the tool
    std::stringstream ss;
    if (!multiplicityMode) ss << name << " " << signstring() << " " << m_cutvalue;
    else {
      if (m_multiplicity < 0) {
        throw std::runtime_error("Cut value and multiplicity must be set for " + name);
      }

      if (!cutValueMode) ss << name << " " << m_cutvalue << " " << signstring() << " " << m_multiplicity;
      else ss << name << " " << m_cutvalueString << " " << signstring() << " " << m_multiplicity;
    }

    m_name = ss.str();
  }

  SignValueSelector::SignValueSelector(const std::string& name, std::string params, bool multiplicityMode,
                                       bool cutValueMode, std::string delim, std::string replace,
                                       std::string default_prefix) :
    SignValueSelector(name, params, multiplicityMode, cutValueMode) {
    // This function is only if we need a bit of additional string manipulation
    // Required for the btagging selector when we start doing "alg:wp"
    // If string is empty, do the split, otherwise just return the stored value

    std::vector<std::string> tokens;
    tokenize(m_cutvalueString, tokens, delim);
    if (tokens.size() > 1) {
      for (auto cutString : tokens) {
        m_cutvalueStringDelimReplace += cutString;
        m_cutvalueStringDelimReplace += replace;
      }
      // Tidy up the end of the string
      m_cutvalueStringDelimReplace = m_cutvalueStringDelimReplace.substr(0,
                                                                         m_cutvalueStringDelimReplace.length() -
                                                                         replace.length());
    }
    // We are kind, and keep some backwards compatablity, so incase you did not know the syntax changed, hopefully we
    // provided the most likely default
    else {
      ATH_MSG_INFO("SignValueSelector :: We are using a default_prefix as we could not split on a delimiter.");
      m_cutvalueStringDelimReplace = default_prefix + replace + m_cutvalueString;
      ATH_MSG_INFO("SignValueSelector :: The final result is " << m_cutvalueStringDelimReplace);
    }
  }

  std::string SignValueSelector::name() const {
    return m_name;
  }

  void SignValueSelector::checkValueIsInteger() {
    if (value() != floor(value())) {
      ATH_MSG_ERROR("The number " << value() << " is not an integer\n"
          << "As defined in " << name() << "\n");
      throw std::runtime_error("Failed parsing value for SignValueSelector");
    }
  }

  void SignValueSelector::checkMultiplicityIsInteger() {
    if (multiplicity() != floor(multiplicity())) {
      ATH_MSG_ERROR("The number " << multiplicity() << " is not an integer\n"
          << "As defined in " << name() << "\n");
      throw std::runtime_error("Failed parsing multiplicity cut for SignValueSelector");
    }
  }

  SignValueSelector::Sign SignValueSelector::sign() const {
    return m_sign;
  }

  double SignValueSelector::value() const {
    return m_cutvalue;
  }

  std::string SignValueSelector::valueString() const {
    return m_cutvalueString;
  }

  std::string SignValueSelector::valueStringDelimReplace() const {
    return m_cutvalueStringDelimReplace;
  }

  double SignValueSelector::multiplicity() const {
    return m_multiplicity;
  }

  SignValueSelector::Sign SignValueSelector::decodesign(std::string& params) {
    Sign tempsign(signNOIDEA);

    if (params.find("==") != std::string::npos) {
      tempsign = signEQ;
      params.erase(params.find("=="), 2);
    } else if (params.find("<=") != std::string::npos) {
      tempsign = signLTEQ;
      params.erase(params.find("<="), 2);
    } else if (params.find(">=") != std::string::npos) {
      tempsign = signGTEQ;
      params.erase(params.find(">="), 2);
    } else if (params.find("<") != std::string::npos) {
      tempsign = signLT;
      params.erase(params.find("<"), 1);
    } else if (params.find(">") != std::string::npos) {
      tempsign = signGT;
      params.erase(params.find(">"), 1);
    }

    return tempsign;
  }

  bool SignValueSelector::checkFloat(double value, double cut) const {
    switch (m_sign) {
    case signNOIDEA: {
      throw std::runtime_error("SignValueSelector::checkFloat: Can't recognise the sign");
    }

    case signEQ: {
      throw std::runtime_error("SignValueSelector::checkFloat: Can't compare floats with ==");
    }

    case signLT:
      return value < cut;

    case signGT:
      return value > cut;

    case signLTEQ:
      return value <= cut;

    case signGTEQ:
      return value >= cut;
    }

    return false;
  }

  bool SignValueSelector::checkInt(int value, int cut) const {
    switch (m_sign) {
    case signNOIDEA: {
      throw std::runtime_error("SignValueSelector::checkInt: Can't recognise the sign");
    }

    case signEQ:
      return value == cut;

    case signLT:
      return value < cut;

    case signGT:
      return value > cut;

    case signLTEQ:
      return value <= cut;

    case signGTEQ:
      return value >= cut;
    }

    return false;
  }

  const std::string SignValueSelector::signstring() const {
    switch (m_sign) {
    case signNOIDEA: {
      ATH_MSG_ERROR("SignValueSelector is confused\n"
          << "Check that your cuts have one of: "
          << "==, >, >=, <, <=\n");
      throw std::runtime_error("SignValueSelector::signstring: Can't recognise the comparison operator");
    }

    case signEQ:
      return "==";

    case signLT:
      return "<";

    case signGT:
      return ">";

    case signLTEQ:
      return "<=";

    case signGTEQ:
      return ">=";
    }

    return "";
  }
}
