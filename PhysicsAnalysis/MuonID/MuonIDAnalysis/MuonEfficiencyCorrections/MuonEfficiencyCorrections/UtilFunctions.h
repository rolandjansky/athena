/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTIONS_UTILFUNCTIONS_H
#define MUONEFFICIENCYCORRECTIONS_UTILFUNCTIONS_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <TH1.h>

namespace CP {
    /// Removes beginning and trailing white spaces from a string
    std::string EraseWhiteSpaces(std::string str);
    ///  Replaces all expressions an string by something else
    std::string ReplaceExpInString(std::string str, const std::string &exp, const std::string &rep);
    /// Lowers all characters in the string
    std::string LowerString(const std::string &str);    
    /// Generates a random string consisting of capital & small letters + numbers
    std::string RandomString(size_t s);
    /// Clones a TH1 and handles the ownership then to a unique_ptr
    std::unique_ptr<TH1> clone(TH1* H);
    /// Checks whether a global bin number belongs to over/underflow or is in the bulk
    bool isOverflowBin(const TH1 *Histo, int bin);
     
    
}
#endif // MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H
