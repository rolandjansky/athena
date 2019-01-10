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
    std::string EraseWhiteSpaces(std::string str);
    std::string ReplaceExpInString(std::string str, const std::string &exp, const std::string &rep);
    std::string LowerString(const std::string &str);    
    std::string RandomString(size_t s);
    std::unique_ptr<TH1> clone(TH1* H);
    
}
#endif // MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H
