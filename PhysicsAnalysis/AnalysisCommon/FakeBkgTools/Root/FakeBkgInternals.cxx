/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeBkgTools/FakeBkgInternals.h"
#include "FakeBkgTools/BaseFakeBkgTool.h"

#include <string>
#include <sstream>
#include <regex>
#include <algorithm>

using namespace FakeBkgTools;
using namespace CP;


float Efficiency::value(const CP::BaseFakeBkgTool* tool) const
{
    float val = nominal;
    for(const auto* su = tool->m_selectedUncertainties; su; su = su->next())
    {
        auto unc = uncertainties.find(su->UID);
        if(unc != uncertainties.end())
        {
            val += su->sigma * ((su->sigma>=0)? unc->second.up : unc->second.down);
        }
    }
    return val;
}

FinalState::FinalState(size_t h, const unsigned nparticles, const std::string& strSelection, const std::string& strProc, std::string& error) : m_hash(h) 
{
    error.clear();
    if(!parseProcess(strProc, error)) return;
    if(!parseSelection(nparticles, strSelection, error)) return;
}

bool FinalState::parseProcess(std::string strProc, std::string& error)
{
    strProc.erase(std::remove_if(strProc.begin(), strProc.end(),
            [](char c){ return std::isspace(c); }), strProc.end());
    std::stringstream ss(strProc);
    std::string token;
    std::regex rx0("([<>]=?)([0-9])([FR])(?:\\[(L|!?T)\\])?");
    std::regex rx1("=?([0-9])(-[0-9])?([FR])(?:\\[(L|!?T)\\])?");
    std::smatch sm;
    bool empty = true;
    m_wmin = 0x0;
    m_wmax = 0x3FFFFFFF;
    while(std::getline(ss,token,','))
    {
        uint8_t nmin = 0, nmax = 0x7F;
        empty = false;
        if(std::regex_match(token, sm, rx0))
        {
            bool strict = (sm[1].str()[1] != '=');
            if(sm[1].str()[0]=='>') nmin = sm[2].str()[0] - '0' + strict*1;
            else
            {
                nmax = sm[2].str()[0] - '0';
                if(strict && !nmax)
                {
                    error = "number of leptons constrained to be negative in the 'process' argument: \"" + strProc + "\"";
                }
                nmax -= strict*1;
            }
        }
        else if(std::regex_match(token, sm, rx1))
        {
            nmin = sm[1].str()[0] - '0';
            if(sm[2].length()) nmax = sm[2].str()[1] - '0';
            else nmax = nmin;
        }
        else
        {
            error = "unable to understand the specified 'process' argument: \"" + strProc + "\"";
            return false;
        }
        /// needs to be kept consistent with format used by acceptCombination() and parseSelection();
        /// said format is the following: uint32_t w = nRT | (nRL<<5) | (nR<<10) | (nFT<<15) | (nFL<<20) | (nF<<25); // 4 bits per number + extra bit for carry
        unsigned char offset = ((sm[3]=="F")? 15 : 0);
        if(sm[4] == "!T") offset += 5;
        else if(sm[4] == "L") offset += 10;
        nmin  = std::max(nmin, uint8_t((m_wmin>>offset)&0xF));
        nmax = std::min(nmax, uint8_t((m_wmax>>offset)&0xF));
        /// Replace the 4-bits digit at position 'offset' by the new value (nmin/nmax):
        m_wmin = (m_wmin&~(0xF<<offset)) | (nmin<<offset);
        m_wmax = (m_wmax&~(0xF<<offset)) | (nmax<<offset);
        if(nmin > nmax)
        {
            error = "unexpected error (nmin>nmax) while parsing the specified 'process' argument: \"" + strProc + "\"";
            return false;
        }
    }
    if(empty)
    {
        error = "the 'process' argument is empty";
        return false;
    }
    m_wmin &= ~0x21084210; /// clear carry bits
    m_wmax &= ~0x21084210; /// clear carry bits
    return true;
}

bool FinalState::parseSelection(const unsigned short nparticles, std::string strSelection, std::string& error)
{
    /// Note: it is important that parseProcess() is called before this function!
    const unsigned nc = (1 << nparticles);
    selection.reset();
    strSelection.erase(std::remove_if(strSelection.begin(), strSelection.end(),
            [](char c){ return std::isspace(c); }), strSelection.end());
    std::stringstream ss(strSelection);

    std::string word = "(?:!?T){" + std::to_string(nparticles) + "}(\\+(?:!?T){" + std::to_string(nparticles) + "})*";

    if(std::regex_match(strSelection, std::regex(word)))
    {
        while(std::getline(ss, word, '+'))
        {
            if(!word.length()) continue;
            std::size_t pos;
            while((pos=word.find("!T")) != std::string::npos) word.replace(pos, 2, 1, 'A'); /// replace all "!T" by "A"
            std::reverse(word.begin(), word.end());
            std::bitset<FakeBkgTools::maxParticles()> combination(word, 0, std::string::npos, 'A', 'T');
            selection.set(combination.to_ulong());
        }
        return true;
    }

    std::regex rx0("([<>]=?)([0-9])(!?T)");
    std::regex rx1("=?([0-9])(-[0-9])?(!?T)");
    std::regex rx2("(OS|SS)");
    std::smatch sm;
    for(unsigned i=0;i<nc;++i) selection.set(i);
    bool empty = true;
    uint8_t maxT=15, maxL=15, minT=0, minL=0;
    while(std::getline(ss, word, ','))
    {
        uint8_t nmin = 0, nmax = 0x7F;
        if(std::regex_match(word, sm, rx0))
        {
            uint8_t o = (sm[1].length()<=1)? 1 : 0;
            if(sm[1].str()[0]=='>') nmin = sm[2].str()[0] - '0' + o;
            else
            {
                nmax = sm[2].str()[0] - '0';
                if(o && !nmax)
                {
                    error = "number of leptons constrained to be negative in the 'selection' argument: \"" + strSelection + "\"";
                    selection.reset();
                    return false;
                }
                nmax -= o;
            }
        }
        else if(std::regex_match(word, sm, rx1))
        {
            nmin = sm[1].str()[0] - '0';
            if(sm[2].length()) nmax = sm[2].str()[1] - '0';
            else nmax = nmin;
        }
        else if(std::regex_match(word, sm, rx2))
        {
            if(hasChargeRequirement())
            {
                error = "both OS and SS requirements were specifed in the 'selection' argument: \"" + strSelection + "\"; this is not allowed";
                selection.reset();
                return false;
            }
            if(sm[0].str()[0]=='S') setSS();
            else setOS();
            continue;
        }
        else
        {
            error = "unable to understand the specified 'selection' argument: \"" + strSelection + "\"";
            selection.reset();
            return false;
        }
        if(nmin > nmax)
        {
            error = "unexpected error (nmin>nmax) while parsing the specified 'selection' argument: \"" + strSelection + "\"";
            selection.reset();
            return false;
        }
        bool count_loose = sm[sm.size()-1] == "!T";
        if(count_loose)
        {
            minL = std::max(minL, nmin);
            maxL = std::min(maxL, nmax);
        }
        else
        {
            minT = std::max(minT, nmin);
            maxT = std::min(maxT, nmax);
        }
        for(unsigned i=0;i<nc;++i)
        {
            auto n = FSBitset(i).count();
            if(count_loose) n = nparticles - n;
            if(n<nmin || n>nmax) selection.reset(i);
        }
        empty = false;
    }
    if(empty)
    {
        error = "the 'selection' argument is empty";
        selection.reset();
        return false;
    }
    
    /// Partial check that the selection and process arguments are consistent
    if((((m_wmin+(m_wmin>>15))&0x1F) > maxT) // minRT + minFT > maxT
        || (((m_wmax+(m_wmax>>15))&0x1F) < minT) // maxRT + maxFT < minT
        || ((((m_wmin>>5)+(m_wmin>>20))&0x1F) > maxL) // minRL + minFL > maxL
        || ((((m_wmax>>5)+(m_wmax>>20))&0x1F) < minL) // maxRL + maxFL < maxL
        || ((((m_wmin>>10)+(m_wmin>>25))&0x1F) > maxL+maxT) // minR + minF > maxL + maxT
        ||((((m_wmax>>10)+(m_wmax>>25))&0x1F) < minL+minT)) // maxR + maxF < minL + minT
    {
        error = "the specified 'selection' and 'process' arguments are not consistent";
        selection.reset();
        return false;
    }

    return true;
}
