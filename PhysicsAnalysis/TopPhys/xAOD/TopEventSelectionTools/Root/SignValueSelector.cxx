/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

SignValueSelector::SignValueSelector(const std::string& name, std::string params, bool multiplicityMode, bool cutValueMode) :
                m_sign(signNOIDEA),
                m_cutvalue(0),
                m_cutvalueString(""),
                m_multiplicity(-1) {
    //cutValueMode always set to false if multiplicityMode is false
    if (!multiplicityMode) cutValueMode=false;
    //get the sign and remove it from the params text
    m_sign = decodesign(params);
    if (!cutValueMode) std::istringstream(params) >> m_cutvalue >> m_multiplicity;
    else std::istringstream(params) >> m_cutvalueString >> m_multiplicity;

    //form the name string for the tool
    std::stringstream ss;
    if (!multiplicityMode)
        ss << name << " " << signstring() << " " << m_cutvalue;
    else {
        if (m_multiplicity < 0) {
            std::cout << "Cut value and multiplicity must be set for " << name << std::endl;
            exit(1);
        }

        if (!cutValueMode)
          ss << name << " " << m_cutvalue << " " << signstring() << " " << m_multiplicity;
        else
          ss << name << " " << m_cutvalueString << " " << signstring() << " " << m_multiplicity;
    }

    m_name = ss.str();
}

std::string SignValueSelector::name() const {
    return m_name;
}

void SignValueSelector::checkValueIsInteger() {
    if (value() != floor(value())) {
        std::cout << "The number " << value() << " is not an integer\n";
        std::cout << "As defined in " << name() << "\n";
        std::cout << "Please fix and try again\n";
        exit(1);
    }
}

void SignValueSelector::checkMultiplicityIsInteger() {
    if (multiplicity() != floor(multiplicity())) {
        std::cout << "The number " << value() << " is not an integer\n";
        std::cout << "As defined in " << name() << "\n";
        std::cout << "Please fix and try again\n";
        exit(1);
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
        std::cout << "Do not recognise the sign\n";
        exit(1);
    } case signEQ: {
        std::cout << "Can't compare floats with ==\n";
        exit(1);
    } case signLT:
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
        std::cout << "Do not recognise the sign\n";
        exit(1);
    } case signEQ:
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
        std::cout << "SignValueSelector is confused\n";
        std::cout << "Please check your cuts have all the correct cuts\n";
        std::cout << "e.g. ==, >, >=, <, <=\n";
        exit(1);
    } case signEQ:
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
