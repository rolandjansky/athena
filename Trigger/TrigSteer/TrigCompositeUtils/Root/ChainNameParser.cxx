/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <regex>
#include <array>

#include "TrigCompositeUtils/ChainNameParser.h"

// returns sig token (or empty string of token does not correspond to a sig) and multiplicity
// this parsing follws trigger menu convention
std::pair<std::string, int> getSigToken(const std::string& token) {
    static const std::array<std::regex, 18> sig{std::regex("(\\d*)(e)\\d.*"),
                                                std::regex("(\\d*)(g)\\d.*"),
                                                std::regex("(\\d*)(j)\\d.*"),
                                                std::regex("(\\d*)(mu)\\d.*"),
                                                std::regex("(\\d*)(tau)\\d.*"),
                                                std::regex("()(xe)\\d.*"),
                                                std::regex("()(xs)\\d.*"),
                                                std::regex("()(te)\\d.*"),
                                                std::regex("()(ht)\\d.*"),
                                                std::regex("()(noalg)"), 
                                                std::regex("()(mb)"), 
                                                std::regex("()(l1calocalib).*"), 
                                                std::regex("()(lar).*"),
                                                std::regex("()(zdc).*"),
                                                std::regex("()(lumipeb)"),
                                                std::regex("()(alfacalib)"),
                                                std::regex("()(calibAFP)"),
                                                std::regex("()(afp)")};
    for (const auto& s : sig) {
        std::smatch match;
        if (std::regex_match(token, match, s)) {
            if (match[1] == "")
                return { match[2], 1 };
            else
                return { match[2], std::stoi(match[1]) };

        }
    }
    return { "", 0 };
}

std::vector<int> ChainNameParser::multiplicities(const std::string& chain) {
    static const std::regex regex("_");
    std::vector<std::string> parts(
        std::sregex_token_iterator(chain.begin(), chain.end(), regex, -1),
        std::sregex_token_iterator()
    );
    std::vector<int> mult;
    for (const auto& token : parts) {
        auto found = getSigToken(token);
        if ( !found.first.empty() ) mult.push_back(found.second);
    }
    if ( mult.empty() )
        return {1};
    return mult;
}


std::vector<std::string> ChainNameParser::signatures(const std::string& chain) {
    static const std::regex regex("_");
    std::vector<std::string> parts(
        std::sregex_token_iterator(chain.begin(), chain.end(), regex, -1),
        std::sregex_token_iterator()
    );
    std::vector<std::string> sigs;
    for (const auto& token : parts) {
        auto sig = getSigToken(token).first;
        if ( not sig.empty())
            sigs.emplace_back(sig);
    }
    return sigs;

}
