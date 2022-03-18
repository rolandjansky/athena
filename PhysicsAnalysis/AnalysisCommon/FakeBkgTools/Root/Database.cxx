/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AsgAnalysisInterfaces/IFakeBkgTool.h"
#include "FakeBkgTools/Database.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <limits>
#include <functional>
#include <cmath>
 
#include "TFile.h"
#include "TKey.h"
#include "TH1.h"
#include "TH2.h"

using namespace FakeBkgTools;
using namespace CP;

Database::Database(Client client, bool useGeV, bool convertWhenMissing) : 
    m_useGeV(useGeV),
    m_typesToFill(selectTypesToFill(client)),
    m_convertWhenMissing(convertWhenMissing)
{
    reset();
}

void Database::reset()
{
    m_tables.clear();
    m_systs.clear();
    m_stats.clear();
    m_params.clear();
    m_params.emplace_back("eta", Param::Type::PREDEFINED_FLOAT, Param::Level::PARTICLE);
    m_params.emplace_back("|eta|", Param::Type::PREDEFINED_FLOAT, Param::Level::PARTICLE);
    m_params.emplace_back("pt", Param::Type::PREDEFINED_FLOAT, Param::Level::PARTICLE);
    m_params.emplace_back("phi", Param::Type::PREDEFINED_FLOAT, Param::Level::PARTICLE);
}

bool Database::ready() const
{
    /// Could do some more careful check here; for the moment just check that there's at least one table
    for(auto& kv : m_tables)
    {
        if(kv.second.size()) return true;
    }
    return false;
}

bool Database::needEventInfo() const
{
    for(auto& param : m_params)
    {
        if(param.level == Param::Level::EVENT) return true; /// will no  longer work if some event-level parameteres are predefined
    }
    return false;
}

bool Database::fillEfficiencies(ParticleData& pd, const xAOD::IParticle& p, const xAOD::EventInfo& eventInfo, std::string& error) const
{
    std::map<unsigned, EfficiencyTable::BoundType> cachedParamVals;
    /// Loop over all the type of efficiencies (real, fake, fake factor) that were requested to be filled
    for(int wt=0;wt<N_EFFICIENCY_TYPES;++wt)
    {
        EfficiencyType wantedType = static_cast<EfficiencyType>(wt);
        if(!m_typesToFill[wantedType]) continue; 
        Efficiency* eff = selectEfficiency(pd, p, wantedType); /// pointer to the proper member of ParticleData that needs to be filled 
        if(!eff) continue;
        
        /// The efficiency tables to look for might be for a different type (if a Fake efficiency <--> Fake factor conversion is needed)
        EfficiencyType type = getSourceType(wantedType);

        bool found_central = false;
        eff->nominal = 1.f;
        eff->uncertainties.clear();
        
        auto relevantTables = m_tables.find(type);
        if(relevantTables == m_tables.end())
        {
            error = "missing table for " + getTypeAsString(type);
            return false;
        }

        /// Loop over all the tables found for that type of efficiency
        for(auto& table : relevantTables->second)
        {
            int status = readEfficiencyFromTable(*eff, table, cachedParamVals, p, eventInfo, error);
            if(status < 0) return false; /// an error occurred
            if(status == 1) /// this table contained the central values
            {
                if(found_central) /// only a single table should be providing central values (the other tables might provide corrections)
                { 
                    error = "while retrieving " + getTypeAsString(type) + ", found two non-orthogonal tables providing the central value";
                    return false;
                }
                found_central = true;
            }
        }
        if(!found_central)
        {
            error = "didn't find central value for " + getTypeAsString(type);
            return false;
        }
        /// Transform the fake efficiency into fake factor (or reciprocally)
        if(type != wantedType)
        {
            if(eff == &pd.fake_factor)
            {
                float f = eff->nominal/(1.f-eff->nominal), k = pow(f/eff->nominal, 2);
                eff->nominal =  f;
                for(auto& kv : eff->uncertainties) kv.second *= k;
            }
            else if(eff == &pd.fake_efficiency)
            {
                float e = eff->nominal/(1.f+eff->nominal), k = pow(e/eff->nominal, 2);
                eff->nominal =  e;
                for(auto& kv : eff->uncertainties) kv.second *= k;
            }
        }
    }
    return true;
}

int Database::readEfficiencyFromTable(Efficiency& eff, const EfficiencyTable& table, std::map<unsigned, EfficiencyTable::BoundType>& cachedParamVals, const xAOD::IParticle& p, const xAOD::EventInfo& eventInfo, std::string& error) const
{
    /// Check if the particle falls in one bin of this table
    int bin = 0;
    for(const auto& dim : table.m_dimensions)
    {
        auto& param = m_params[dim.paramUID];
        auto ins = cachedParamVals.emplace(dim.paramUID, EfficiencyTable::BoundType{});
        auto& val = ins.first->second;
        if(ins.second)
        {
            if(!retrieveParameterValue(p, eventInfo, param, val))
            {
                error = "can't retrieve value of parameter \"" + param.name + "\"";
                return -1;
            }
        }
        auto first = table.m_bounds.begin()+dim.iMinBound, last = first+dim.nBounds;
        auto ubound = std::upper_bound(first, last, val,
            [=](auto x, auto y){ return param.integer() ? (x.as_int<y.as_int) : (x.as_float<y.as_float); });
        if(ubound==first || ubound==last)
        {
            bin = -1;
            break;
        }
        bin = bin * (dim.nBounds-1) + (ubound - first - 1);
    }
    if(bin < 0) return 0; /// if not, return
    
    /// Yes! So read the central value / correction / uncertainties
    if(table.inputType != InputType::CENTRAL_VALUE && table.inputType != InputType::CORRECTION)
    {
        error = "unknown table type (tool implementation incomplete!)";
        return -1;
    }
    auto& ref = table.m_efficiencies[bin];
    for(auto& kv : eff.uncertainties) kv.second *= ref.nominal;
    for(auto& kv : ref.uncertainties)
    {
        if(m_storeAllUncertainties) /// => tool will return weight + all uncertainties at once
        {
            if(!eff.uncertainties.emplace(kv.first, eff.nominal*kv.second).second)
            {
                error = "central values and corrections must use different systematic uncertainties";
                return -1;
            }
        }
    }
    eff.nominal *= ref.nominal;
    return (table.inputType==InputType::CENTRAL_VALUE)? 1 : 2;
}

/*
 *  Loading from XML
 */

void Database::importXML(std::string filename)
{
    filename = PathResolverFindCalibFile(filename);

    std::ifstream xml;
    xml.open(filename, std::ios_base::binary);
    auto begpos = xml.tellg();
    xml.seekg(0, std::ios_base::end);
    std::size_t bufferSize = 1.05 * static_cast<std::size_t>(xml.tellg() - begpos);
    /// not guaranteed that the above works, so truncating the buffer to a reasonable size if seemingly too large
    if(bufferSize > 0x100000) bufferSize = 0x100000;
    xml.close();
    
    xml.open(filename, std::ios_base::in);
    if(!xml.is_open()) throw(GenericError() << "unable to open file " << filename);

    std::string line;
    m_xmlBuffer.reserve(bufferSize);
    m_lineOffset.clear();
    m_lineOffset.push_back(0);
    while(std::getline(xml, line))
    {
        while(line.length() && (line.back()=='\n' || line.back()=='\r')) line.pop_back();
        m_xmlBuffer += line + ' ';
        m_lineOffset.push_back(m_xmlBuffer.length());
    }
    xml.close();

    dropRootTag(m_xmlBuffer);    
    dropXmlComments(m_xmlBuffer);
    
    AttributesMap attributes;
    resetAttributes(attributes);
    StringRef stream(m_xmlBuffer.data(), m_xmlBuffer.length()), contents, tag;
    while(stream.length())
    {
        readNextTag(stream, tag, attributes, contents);
        if(tag=="electron" || tag=="muon" || tag=="tau") addTables(tag, attributes, contents);
        else if(tag=="param") addParams(tag, contents, attributes);
        else if(tag=="syst") addSysts(tag, contents, attributes);
        else if(tag=="ROOT") importCustomROOT(tag, contents, attributes);
        else throw(XmlError(stream) << "unknown/unexpected XML tag \"" << tag.str() << "\"");
    }
    m_lineOffset.clear();
    m_lineOffset.shrink_to_fit();
    m_xmlBuffer.clear();
    m_xmlBuffer.shrink_to_fit();
}

void Database::readNextTag(StringRef& stream, StringRef& tag, AttributesMap& attributes, StringRef& contents)
{
    tag.clear();
    contents.clear();
    for(auto& kv : attributes) kv.second.clear();
    /// Note: when modifying the pattern, make sure to also change readTagAttributes() if required!
    std::string pattern = "^\\s*(<([[:alnum:]]+)((?:\\s+\\|?[_[:alnum:]]+\\|?\\s*=\\s*\"[_[:alnum:]\\s,-\\[\\]\\.\\|]+\")*)\\s*>)(.*?)</\\2>\\s*";
    std::cmatch cmr;
    if(!std::regex_search(stream.ptr, stream.endptr, cmr, std::regex(pattern)))
    {
        /// Perhaps here do some looser pattern-matching to figure out where the format was not good: missing quotation marks? missing attribute value? no closing tag?
        throw(XmlError(stream) << "unable to find next tag");
    }
    tag.set(stream.ptr+cmr.position(2), cmr.length(2));
    
    readTagAttributes(StringRef(stream.ptr+cmr.position(3), cmr.length(3)), tag.str(), attributes);

    auto cpos = cmr.size()-1;
    contents.set(stream.ptr+cmr.position(cpos), cmr.length(cpos));
    
    stream.ptr += cmr.length();
}

void Database::readTagAttributes(StringRef stream, const std::string& tag, AttributesMap& attributes)
{
    auto nAttr = std::count(stream.ptr, stream.endptr, '=');
    if(!nAttr) return;
    
    std::string pattern = "";
    for(int i=0;i<nAttr;++i) pattern += "\\s+(\\|?[_[:alnum:]]+\\|?)\\s*=\\s*\"([_[:alnum:]\\s,-\\[\\]\\.\\|]+)\"";
    
    std::cmatch cmr;
    if(!std::regex_match(stream.ptr, stream.endptr, cmr, std::regex(pattern)))
    {
        throw(XmlError(stream) << "unexpected error (internal))");
    }
    for(unsigned i=1;i<cmr.size();i+=2)
    {
        auto attr = cmr[i].str();
        auto itr = attributes.find(tag + '/' + attr);
        if(itr == attributes.end())
        {
            throw(XmlError(stream.ptr+cmr.position(i), attr.length()) << "invalid attribute \"" << attr << "\"");
        }
        if(!cmr.length(i+1))
        {
            throw(XmlError(stream.ptr+cmr.position(i), attr.length()) << "empty value for attribute \"" << attr << "\"");
        }
        auto& attrVal = itr->second;
        if(attrVal) throw(XmlError(stream.ptr+cmr.position(i), attr.length()) << "the attribute \"" << attr << "\" has already been specified for that tag");
        attrVal.set(stream.ptr+cmr.position(i+1), cmr.length(i+1));
    }
}

void Database::dropXmlComments(std::string& buffer)
{
    std::regex rx("<!--.*?-->");
    std::smatch smr;
    while(std::regex_search(buffer, smr, rx))
    {
        std::size_t pos = smr.position(0), length = smr.length(), endpos = pos + length;
        for(auto& offset : m_lineOffset)
        {
            if(offset > endpos) offset -= length;
            else if(offset > pos) offset = pos;
        }
        buffer = smr.prefix().str() + smr.suffix().str();
    }
}

void Database::dropRootTag(std::string& buffer)
{
    for(const std::string& key : {"<efficiencies>", "</efficiencies>"})
    {
        std::size_t ipos;
        while((ipos = buffer.find(key)) != std::string::npos)
        {
            buffer.erase(ipos, key.length()-1);
            buffer[ipos] = ' ';
        }
    }
}

std::vector<std::string> Database::getListOfNames(const StringRef& stream)
{
    std::vector<std::string> words;
    std::stringstream ss(std::string(stream.ptr, stream.endptr));
    std::string w;
    while(std::getline(ss, w, ','))
    {
        std::size_t i = w.find_first_not_of(" \t");
        std::size_t j = w.find_last_not_of(" \t");
        if(i == std::string::npos)
        {
            throw(XmlError(stream) << "this should be a comma-separated list of names");
        }
        words.push_back(w.substr(i, j-i+1));
    }
    return words;
}

template<typename ReturnValue>
ReturnValue Database::getAttribute(const StringRef& attr, const char* ref, ReturnValue rv)
{
    if(attr.str() == ref) return rv;
    throw(XmlError(attr) << "unsupported parameter type \"" << attr.str() << "\"");
}

template<typename ReturnValue, typename... Args>
ReturnValue Database::getAttribute(const StringRef& attr, const char* ref, ReturnValue rv, Args... args)
{
    if(attr.str() == ref) return rv;
    return getAttribute(attr, args...);
}

template<typename ReturnValue, typename... Args>
ReturnValue Database::getAttribute(const StringRef& tag, const AttributesMap& attributes, const std::string& type, const char* ref, ReturnValue rv, Args... args)
{
    std::string attrname = tag.str() + "/" + type;
    auto& attr = attributes.at(attrname);
    if(!attr) throw(XmlError(tag) << "unspecified value for attribute \"" << type << "\"");
    return getAttribute(attr, ref, rv, args...);
}

void Database::addParams(const StringRef& tag, const StringRef& contents, AttributesMap& attributes)
{
    auto type = getAttribute(tag, attributes, "type", "int", Param::Type::CUSTOM_INT, "float", Param::Type::CUSTOM_FLOAT);
    auto level = getAttribute(tag, attributes, "level", "particle", Param::Level::PARTICLE, "event", Param::Level::EVENT);

    for(auto& name: getListOfNames(contents))
    {
        if(std::any_of(m_params.begin(), m_params.end(), [&](const Param& p){ return p.name == name; }))
        {
            throw(XmlError(contents) << "parameter \"" << name << "\" was already declared");
        }
        m_params.emplace_back(name, type, level);
        attributes["bin/" + name];
        attributes["table/" + name];
    }
}

void Database::addSysts(const StringRef& tag, const StringRef& contents, const AttributesMap& attributes)
{
    std::bitset<N_EFFICIENCY_TYPES> affects;
    for(auto& target : getListOfNames(attributes.at("syst/affects")))
    {
        auto targetMatches = [&](const char* a, const char* b) -> bool
            { return target==a || target==b || target==std::string(a)+'-'+b; };
        int matched = 0;
        if(targetMatches("electron", "real-efficiency")) { affects.set(ELECTRON_REAL_EFFICIENCY); ++matched; }
        if(targetMatches("muon", "real-efficiency")) { affects.set(MUON_REAL_EFFICIENCY); ++matched; }
        if(targetMatches("tau", "real-efficiency")) { affects.set(TAU_REAL_EFFICIENCY); ++matched; }
        if(targetMatches("electron", "fake-efficiency")) { affects.set(ELECTRON_FAKE_EFFICIENCY); ++matched; }
        if(targetMatches("muon", "fake-efficiency")) { affects.set(MUON_FAKE_EFFICIENCY); ++matched; }
        if(targetMatches("tau", "fake-efficiency")) { affects.set(TAU_FAKE_EFFICIENCY); ++matched; }
        if(targetMatches("electron", "fake-factor")) { affects.set(ELECTRON_FAKE_FACTOR); ++matched; }
        if(targetMatches("muon", "fake-factor")) { affects.set(MUON_FAKE_FACTOR); ++matched; }
        if(targetMatches("tau", "fake-factor")) { affects.set(TAU_FAKE_FACTOR); ++matched; }
        if(!matched) throw(XmlError(tag) << "the value \"" << target << "\" specified for the attribute \"affects\" is not recognized");
    }
    if(affects.none()) throw(XmlError(tag) << "missing or empty attribute \"affects\"");
    for(auto& name: getListOfNames(contents))
    {
        if(name == "stat") throw(XmlError(contents) << "systematics can't be named \"stat\"");
        if(std::any_of(m_systs.begin(), m_systs.end(), [&](const SystDef& s){ return s.name==name && (affects&s.affects).any(); }))
        {
            throw(XmlError(contents) << "the systematic \"" << name << "\" was already declared previously; duplicates are only allowed if their \"affects\" attributes do not overlap, which is not the case here");
        }
        if(m_systs.size() >= maxIndex()) throw(XmlError(contents) << "exceeded max number of systematic uncertainties");
        m_systs.emplace_back(name, affects);
    }
}

unsigned short Database::addStat(EfficiencyType type, const StringRef& pos)
{
    if(m_stats.size() >= maxIndex())
    {
        if(pos) throw(XmlError(pos) << "exceeded max number of statistical uncertainties");
        else throw(GenericError() << "exceeded max number of statistical uncertainties");
    }
    m_stats.emplace_back(1 << type);
    return statIndexToUID(m_stats.size() - 1);
}

void Database::addTables(const StringRef& particle, const AttributesMap& attributes, const StringRef& contents, TFile* source)
{
    EfficiencyType type;
    if(particle == "muon") type = getAttribute(particle, attributes, "type", 
        "real-efficiency", MUON_REAL_EFFICIENCY, "fake-efficiency", MUON_FAKE_EFFICIENCY, "fake-factor", MUON_FAKE_FACTOR);
    else if(particle == "electron") type = getAttribute(particle, attributes, "type", 
        "real-efficiency", ELECTRON_REAL_EFFICIENCY, "fake-efficiency", ELECTRON_FAKE_EFFICIENCY, "fake-factor", ELECTRON_FAKE_FACTOR);
    else if(particle == "tau") type = getAttribute(particle, attributes, "type", 
        "real-efficiency", TAU_REAL_EFFICIENCY, "fake-efficiency", TAU_FAKE_EFFICIENCY, "fake-factor", TAU_FAKE_FACTOR);
    else throw(XmlError(particle) << "unexpected error: unsupported particle type " << particle.str());
    auto statMode = attributes.at(particle.str()+"/stat") ? getAttribute(particle, attributes, "stat", 
        "global", StatMode::GLOBAL, "per-bin", StatMode::PER_BIN, "none", StatMode::NONE) : StatMode::UNSPECIFIED;
    auto inputType = getAttribute(particle, attributes, "input", "central-value", InputType::CENTRAL_VALUE, "correction", InputType::CORRECTION);
    
    unsigned short globalStatUID = 0;
    
    AttributesMap subattributes;
    StringRef stream = contents, subcontents;
    while(stream.length())
    {
        StringRef tag;
        resetAttributes(subattributes);
        readNextTag(stream, tag, subattributes, subcontents);
        const TH1* hist = nullptr;
        if(tag=="bin" || tag=="table" || tag=="TH1")
        {
            if(tag == "TH1")
            {
                if(!source) throw(XmlError(tag) << "histograms can only be imported inside <ROOT>...</ROOT> blocks!");
                if(!subattributes.at("TH1/X")) throw(XmlError(tag) << "the attribute 'X' should be specified (as well as 'Y' for 2D histograms, 'Z' for 3D histograms)");
                auto name = subcontents.trim();
                hist = static_cast<const TH1*>(source->Get(name.c_str()));
                if(!hist) throw(XmlError(subcontents) << "can't find any histogram named \"" << name << "\" in the file " << source->GetName());
                auto& norm = subattributes.at("TH1/norm");
                if(inputType!=InputType::CORRECTION && (norm && norm!="none"))
                    throw(XmlError(norm) << "normalization of input histograms is only accepted for 'input=\"correction\"'");
                float scale = getNormalizationFactor(hist, type, norm, subcontents);
                importNominalTH1(hist, type, subattributes.at("TH1/X"), subattributes.at("TH1/Y"), subattributes.at("TH1/Z"), scale, statMode, globalStatUID, subcontents);
            }
            else m_tables[type].emplace_back();
            auto& table = m_tables[type].back();
            table.inputType = inputType;
            
            auto initialNumberOfBins = table.numberOfBins();
            std::map<StringRef, unsigned> dimBins;
            for(unsigned uid=0; uid<m_params.size(); ++uid)
            {
                auto& binning = subattributes.at(tag + "/" + m_params[uid].name);
                if(binning) dimBins.emplace(binning, uid); /// using the StringRef as map key ensures that dimensions will be listed in the same order as they were written
            }
            for(auto& kv : dimBins) addDimension(table, kv.second, kv.first);
        
            if(tag != "TH1")
            {
                if(tag == "bin" && table.numberOfBins() > 1) throw(XmlError(tag) << "use a <table> instead of a <bin> tag to hold several values");
                addValues(subcontents, table, type, statMode, globalStatUID);
            }
            else if(table.numberOfBins() != initialNumberOfBins) throw(XmlError(tag) << "extra binned dimensions do not make sense");
            if(tag=="TH1" || tag=="bin")
            {
                auto& label = subattributes.at(tag + "/label");
                if(label)
                {
                    float normFactor {0};
                    if(tag == "TH1") normFactor = 1.f / getWeightedAverage(hist, stream);
                    else
                    {
                      assert (tag == "bin");
                      normFactor = 1.f / table.m_efficiencies[0].nominal;
                    }
                    if(!std::isnormal(normFactor) || normFactor<=0.) throw(XmlError(label) << "computed normalization factor is 0 / NaN / infinite / negative");
                    if(!m_normFactors.emplace(label.str()+"-"+std::to_string(type), normFactor).second)
                        throw(XmlError(label) << "label \"" << label.str() << "\" has already been used");
                }
            }
        }
        else throw(XmlError(tag) << "unknown/unexpected XML tag \"" << tag.str() << "\"");
    }
}

void Database::assertNoLeftover(std::stringstream& ss, const StringRef& pos)
{
    ss >> std::ws;
    if(!ss.eof())
    {
        std::string line;
        std::getline(ss, line);
        throw(XmlError(pos) << "unexpected parsing error (leftover data \"" << line << "\")");
    }
}

void Database::addDimension(EfficiencyTable& table, unsigned paramUID, const StringRef& contents)
{
    if(!contents) return;
    auto& param = m_params[paramUID];
    const bool integer = param.integer();
    const std::string fp = "[+-]?[0-9]*\\.?[0-9]+(?:[Ee][+-]?[0-9]+)?";
    const std::string pattern = "^\\s*\\[\\s*(?:(?:-inf\\s*,\\s*|-?)inf|(?:-inf\\s*,\\s*)?" 
        + fp + "(?:\\s*,\\s*" + fp + ")*(?:\\s*,\\s*inf)?)\\s*\\]\\s*$";
    if(!std::regex_match(contents.ptr, contents.endptr, std::regex(pattern)))
    {
        /// Also accept a simpler syntax for integers (e.g. nJets = "2")
        if(!integer || !std::regex_match(contents.ptr, contents.endptr, std::regex("\\s*[+-]?[0-9]+\\s*")))
        {
            throw(XmlError(contents) << "invalid format for the range of the parameter " << param.name);
        }
    }
    
    auto& bounds = table.m_bounds;
    table.m_dimensions.emplace_back();
    auto& dim = table.m_dimensions.back();
    dim.paramUID = paramUID;
    dim.iMinBound = table.m_bounds.size();
    auto line = contents.str();
    dim.nBounds = std::count(line.begin(), line.end(), ',') + 1;
    if(integer && dim.nBounds < 1) throw(XmlError(contents) << "should specify at least one bin boundary for parameter " << param.name);
    if(!integer && (dim.nBounds < 2)) throw(XmlError(contents) << "should specify at least two bin boundaries for parameter " << param.name);
    for(auto&c : line) if(c==',' || c=='[' || c==']') c = ' ';
    std::stringstream ss(line);
    for(int i=0;i<dim.nBounds;++i)
    {
        EfficiencyTable::BoundType x;
        if(integer) ss >> x.as_int;
        else ss >> x.as_float;
        if(ss.fail())
        {
            if(i==0 || i==(dim.nBounds-1))
            {
                ss.clear();
                ss.unget(); /// the stream might have consumed the leading '-' in its failed attempt to read a number, so recover it
                ss.clear();
                std::string x_s;
                ss >> x_s;
                if(x_s=="inf" || x_s=="-inf")
                {
                    bool defMax = (x_s.front() != '-');
                    if(integer) x.as_int = defMax ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min();
                    else x.as_float = defMax ? std::numeric_limits<float>::max() : std::numeric_limits<float>::lowest();
                }
                else throw(XmlError(contents) << "parsing error (invalid 'inf' string)");
            }
            if(ss.fail()) throw(XmlError(contents) << "parsing error (can't read int/float boundary)");
        }
        if(i)
        {
            if(integer ? (bounds.back().as_int > x.as_int) : (bounds.back().as_float > x.as_float))
            {
                throw(XmlError(contents) << "bin boundaries must be sorted in increasing order");
            }
        }
        bounds.push_back(x);
    }
    assertNoLeftover(ss, contents);
    if(integer && dim.nBounds==1)
    {
        dim.nBounds = 2;
        bounds.push_back(bounds.back());
        bounds.back().as_int += 1;
    }
}

void Database::addValues(const StringRef& contents, EfficiencyTable& table, EfficiencyType type, StatMode statMode, unsigned short& globalStatUID)
{
    const std::string fpv = "(?:[0-9]+\\.)?[0-9]+(?:[Ee][+-]?[0-9]+)?", fpu = fpv + "\\s*\\%?";
    const std::string pattern = "^\\s*" + fpv + "(?:\\s*(?:\\+(?:\\s*" + fpu + "\\s*)?-|-(?:\\s*" + fpu + "\\s*)?\\+)\\s*" + fpu + "\\s*\\([_[:alnum:]]+\\))*\\s*";
    auto rxValidFormat = std::regex(pattern);
    std::stringstream ssCSV(contents.str()), ss;
    const char* ptr = contents.ptr;
    
    if(statMode==StatMode::GLOBAL && !globalStatUID) globalStatUID = addStat(type, contents);
    
    while(ptr && ptr<contents.endptr)
    {
        std::cmatch cm;
        if(!std::regex_search(ptr, contents.endptr, cm, rxValidFormat))
        {
            StringRef lineref{ptr, contents.endptr};
            throw(XmlError(lineref) << "the central value(s) and uncertainties are not in the expected format; first issue found with value " << lineref.str().substr(0, 32) << " [...]");
        }
        StringRef valref{ptr, static_cast<std::size_t>(cm.length())};
        ptr += cm.length();
        std::string value = valref.str();
        value.erase(std::remove_if(value.begin(), value.end(), [](char c){ return std::isspace(c); }), value.end());
        unsigned nErrs = std::count(value.begin(), value.end(), '(');
        for(auto& c : value) if(c=='(' || c==')') c = ' ';
        ss.clear();
        ss.str(value);
        table.m_efficiencies.emplace_back();
        auto& eff = table.m_efficiencies.back();
        ss >> eff.nominal;
        bool foundStat = false;
        for(unsigned i=0;i<nErrs;++i)
        {
            FakeBkgTools::Uncertainty uncval;
            std::string sysname;
            ss >> std::ws;
            char c1 = ss.get(), c2 = ss.peek();
            if(c2=='+' || c2=='-')
            {
                ss >> c2 >> uncval.up >> sysname;
                if(sysname == "%") 
                {
                    uncval.up *= 0.01f * eff.nominal;
                    ss >> sysname;
                }
                uncval.down = uncval.up;
            }
            else
            {
                ss >> uncval.up >> c2;
                if(c2 == '%')
                {
                    uncval.up *= 0.01f * eff.nominal;
                    ss >> c2;
                }
                ss >> uncval.down >> sysname;
                if(sysname == "%")
                {
                    uncval.down *= 0.01f * eff.nominal;
                    ss >> sysname;
                }
            }
            if(ss.bad()) throw(XmlError(valref) << "unexpected parsing error");
            if(std::signbit(uncval.up) != std::signbit(uncval.down)) throw(XmlError(valref) << "one-sided up/down errors");
            if(c1 == '-')
            {
                uncval.up = -uncval.up;
                uncval.down = -uncval.down;
            }
            
            uint16_t uid;
            if(sysname == "stat")
            {
                if(foundStat) throw(XmlError(valref) << "there can be only one source of statistical uncertainty per bin");
                if(statMode==StatMode::UNSPECIFIED || statMode==StatMode::NONE) throw(XmlError(valref) << "when using statistical uncertainties, the \"stat\" attribute must be specified (and not set to \"none\")");
                foundStat = true;
                uid = (statMode == StatMode::GLOBAL)? globalStatUID : addStat(type, contents);
            }
            else
            {
                auto sys = std::find_if(m_systs.begin(), m_systs.end(),
                    [&](const SystDef& sd){ return sd.name==sysname && sd.affects[type]; });
                if(sys == m_systs.end()) throw(XmlError(valref) << "the systematic \"" << sysname << "\" has either not been defined, or does not affect this type of efficiency");
                unsigned index = sys - m_systs.begin();
                uid = systIndexToUID(index);
            }
            if(!eff.uncertainties.emplace(uid, uncval).second)
            {
                throw(XmlError(valref) << "source of uncertainty \"" << sysname << "\" specified twice");
            }
        }
        assertNoLeftover(ss, contents);
    }
    if(table.m_efficiencies.size() != table.numberOfBins())
    {
        throw(XmlError(contents) << "the number of tabulated efficiencies (" << table.m_efficiencies.size() 
            << ") is inconsistent with the number of bins (" << table.numberOfBins() << ")");
    }
}

void Database::importCustomROOT(const StringRef& rootTag, const StringRef& contents, const AttributesMap& attributes)
{
    std::string filename = attributes.at("ROOT/source").str();
    if(!filename.length()) throw(XmlError(rootTag) << "the 'file' attribute must be specified!");
    filename = PathResolverFindCalibFile(filename);
    
    TFile* file = TFile::Open(filename.c_str(), "READ");
    if(!file || !file->IsOpen())
    {
        delete file;
        throw(XmlError(rootTag) << "unable to locate/open the file " << filename);
    }
    
    AttributesMap subattributes;
    StringRef stream = contents, tag, subcontents;
    while(stream.length())
    {
        resetAttributes(subattributes);
        readNextTag(stream, tag, subattributes, subcontents);
        if(tag=="electron" || tag=="muon" || tag=="tau") addTables(tag, subattributes, subcontents, file);
        else throw(XmlError(stream) << "unknown/unexpected XML tag \"" << tag << "\"");
    }
    
    file->Close();
    delete file;
}

void Database::resetAttributes(AttributesMap& attributes)
{
    attributes["ROOT/source"];
    attributes["param/type"];
    attributes["param/level"];
    attributes["syst/affects"];
    for(const std::string& p : {"electron", "muon", "tau"})
    {
        attributes[p + "/type"];
        attributes[p + "/input"];
        attributes[p + "/stat"];
    }
    attributes["TH1/X"];
    attributes["TH1/Y"];
    attributes["TH1/Z"];
    attributes["TH1/label"];
    attributes["bin/label"];
    attributes["TH1/norm"]; /// for now only a TH1 attribute
    for(auto& p : m_params)
    {
        attributes["bin/" + p.name];
        attributes["table/" + p.name];
        attributes["TH1/" + p.name];
        /// NB. the list of attributes allowed for bin or table tags can be further extended by new param tags.
        /// This is done inside addParams().
    }
}

/*
 *  Loading from ROOT
 */

void Database::importDefaultROOT(std::string filename) 
{
    const std::string prefix = "^(FakeFactor|FakeEfficiency|RealEfficiency)", suffix = "_([[:w:]][^_]+)(__[[:w:]]+)?$";
    const std::regex rxTH1(prefix + "_(el|mu|tau)" + suffix);
    const std::regex rxTH2(prefix + "2D_(el|mu|tau)_([[:alnum:]]+)" + suffix);
    const std::regex rxTH3(prefix + "3D_(el|mu|tau)_([[:alnum:]]+)_([[:alnum:]]+)" + suffix);
    
    filename = PathResolverFindCalibFile(filename);
    TFile* file = TFile::Open(filename.c_str(), "READ");
    if(!file || !file->IsOpen())
    {
        throw(GenericError() << "unable to locate/open the file " << filename);
    }
    
    auto keys = file->GetListOfKeys();
    if(!keys) throw(GenericError() << "unable to list keys in the file " << filename << " (corrupted?)");
    
    const StringRef nullStream;
    unsigned short dummy;
    for(unsigned step=0;step<2;++step)
    {
        /// step 0 = central value + stats
        /// step 1 = systs
        for(int i=0;i<keys->GetSize();++i)
        {
            TKey* key = static_cast<TKey*>(keys->At(i));
            std::cmatch mr;
            std::string keyType = key->GetClassName();
            unsigned nDims = 0;
            if(keyType=="TH1F" || keyType=="TH1D") nDims = 1 * std::regex_match(key->GetName(), mr, rxTH1);
            else if(keyType=="TH2F" || keyType=="TH2D") nDims = 2 * std::regex_match(key->GetName(), mr, rxTH2);
            else if(keyType=="TH3F" || keyType=="TH3D") nDims = 3 * std::regex_match(key->GetName(), mr, rxTH3);
            else continue;
            if(nDims < 1) throw(GenericError() << "don't know what to do with histogram named \"" << key->GetName() << "\" (please check naming conventions)");
            TH1* hist = static_cast<TH1*>(key->ReadObj());
            /// Efficiency type
            std::string sss = mr[1].str() + "-" + mr[2].str();
            auto type = getAttribute(StringRef(sss.data(), sss.length()),
                "FakeFactor-el", ELECTRON_FAKE_FACTOR, "FakeFactor-mu", MUON_FAKE_FACTOR, "FakeFactor-tau", TAU_FAKE_FACTOR,
                "FakeEfficiency-el", ELECTRON_FAKE_EFFICIENCY, "FakeEfficiency-mu", MUON_FAKE_EFFICIENCY, "FakeEfficiency-tau", TAU_FAKE_EFFICIENCY,
                "RealEfficiency-el", ELECTRON_REAL_EFFICIENCY, "RealEfficiency-mu", MUON_REAL_EFFICIENCY, "RealEfficiency-tau", TAU_REAL_EFFICIENCY
                );
            bool systTH1 = (mr[mr.size()-1].str() != "");
            if(step==0 && !systTH1)
            {
                StringRef paramX = StringRef(mr[3].first, mr[3].second);
                StringRef paramY = (nDims>1) ? StringRef(mr[4].first, mr[4].second) : StringRef();
                StringRef paramZ = (nDims>2) ? StringRef(mr[5].first, mr[5].second) : StringRef();
                importNominalTH1(hist, type, paramX, paramY, paramZ, 1.f, StatMode::PER_BIN, dummy, nullStream);
                m_tables[type].back().inputType = InputType::CENTRAL_VALUE;
            }
            else if(step==1 && systTH1) importSystTH1(hist, type, mr[nDims+3].str().substr(2));
            else continue;
        }
    }
    
    file->Close();
    delete file;
}

float Database::getWeightedAverage(const TH1* hist, const StringRef& xmlStream)
{
    float avg = 1.f;
    if(hist->GetNbinsX()!=1 || hist->GetNbinsY()!=1 || hist->GetNbinsZ()!=1)
    {
        /// compute weighted average of all bins
        double sum = 0., denom = 0.;
        for(int i=1;i<=hist->GetNbinsX();++i)
        for(int j=1;j<=hist->GetNbinsY();++j)
        for(int k=1;k<=hist->GetNbinsZ();++k)
        {
            double x = hist->GetBinContent(i, j, k);
            if(x == 0.) continue;
            double w = hist->GetBinError(i, j, k);
            if(w == 0.) throw(XmlError(xmlStream) << "bin with error = 0 encountered when trying to normalize histogram " << hist->GetName() << " to weighted bins average");
            w = 1./(w*w);
            sum += w * x;
            denom += w;
        }
        avg = sum / denom;
    }
    else avg = 1. / hist->GetBinContent(1); /// central value of the correction will be 1, but uncertainties may matter, so this is also a valid case
    if(!std::isnormal(avg) || avg<=0.) throw(XmlError(xmlStream) << "something bad happened when trying to compute the weighted average of histogram \"" 
            << hist->GetName() << "\" bins, the result ended up 0 / NaN / infinite / negative");
    return avg;
}

float Database::getNormalizationFactor(const TH1* hist, EfficiencyType type, const StringRef& norm, const StringRef& xmlStream)
{
    /// Should be called only when processing XMLs
    if(!norm) return 1.f;
    auto normType = norm.str();
    if(normType == "auto") return 1.f / getWeightedAverage(hist, xmlStream); /// in principle the denominator has been checked not to be 0
    else if(normType != "none")
    {
        auto itr = m_normFactors.find(normType + "-" + std::to_string(type));
        if(itr == m_normFactors.end()) throw(XmlError(norm) << "unknown normalization tag \"" << normType << "\"");
        return itr->second;
    }
    return 1.f;
}

void Database::importNominalTH1(const TH1* hist, EfficiencyType type, const StringRef& paramX, const StringRef& paramY,  const StringRef& paramZ, 
        float scale, StatMode statMode, unsigned short& globalStatUID, const StringRef& xmlStream)
{
    const bool useDefaults = !xmlStream;
    
    if(useDefaults && m_tables[type].size()) throw(GenericError() << "already filled that table, please use an XML to describe how to interpret the more complex ROOT files");
    m_tables[type].emplace_back();
    auto& table = m_tables[type].back();

    const int nDims = paramZ? 3 : paramY? 2 : 1;
    if(hist->GetDimension() != nDims)
    {
        if(xmlStream) throw(XmlError(xmlStream) << "histogram " << hist->GetName() << " doesn't have the expected dimension");
        else throw(GenericError() << "histogram " << hist->GetName() << " doesn't have the expected dimension");
    }
 
    /// Parameters and ranges
    for(int j=0;j<nDims;++j)
    {
        std::string name = ((j==2)? paramZ : (j==1)? paramY : paramX).str();
        const TAxis* axis = (j==2)? hist->GetZaxis() : (j==1)? hist->GetYaxis() : hist->GetXaxis();
        if(useDefaults && name == "eta" && axis->GetBinLowEdge(1) >= 0) name = "|eta|";
        table.m_dimensions.emplace_back();
        auto& dim = table.m_dimensions.back();
        auto itr = std::find_if(m_params.begin(), m_params.end(), [&](const Param& p){ return p.name == name; });
        bool integer;
        if(itr == m_params.end())
        {
            if(useDefaults)
            {
                dim.paramUID = m_params.size();
                m_params.emplace_back(name, Param::Type::CUSTOM_FLOAT, Param::Level::PARTICLE);
                integer = false;
            }
            else throw(XmlError(j? paramY : paramX) << "parameter \"" << name << "\" has not been defined beforehand");
        }
        else
        {
            dim.paramUID = itr - m_params.begin();
            integer = itr->integer();
        }
        dim.iMinBound = table.m_bounds.size();
        dim.nBounds = axis->GetNbins() + 1;
        /// Copy, and include under/overflow into first/last bin:
        table.m_bounds.emplace_back();
        if(integer) table.m_bounds.back().as_int = std::numeric_limits<int>::min();
        else table.m_bounds.back().as_float = std::numeric_limits<float>::lowest();
        for(int k=1;k<dim.nBounds-1;++k)
        {
            table.m_bounds.emplace_back();
            if(integer) table.m_bounds.back().as_int = std::ceil(axis->GetBinUpEdge(k));
            else table.m_bounds.back().as_float = axis->GetBinUpEdge(k);
        }
        table.m_bounds.emplace_back();
        if(integer) table.m_bounds.back().as_int = std::numeric_limits<int>::max();
        else table.m_bounds.back().as_float = std::numeric_limits<float>::max();
    }
    
    /// Efficiencies + stat uncertainties
    if(statMode==StatMode::GLOBAL && !globalStatUID) globalStatUID = addStat(type, xmlStream);
    const unsigned xmax = table.m_dimensions.front().nBounds;
    const unsigned ymax = table.m_dimensions.size()>1? table.m_dimensions[1].nBounds : 2;
    const unsigned zmax = table.m_dimensions.size()>2? table.m_dimensions[2].nBounds : 2;
    for(unsigned x=1;x<xmax;++x)
    for(unsigned y=1;y<ymax;++y)
    for(unsigned z=1;z<zmax;++z)
    {
        table.m_efficiencies.emplace_back();
        auto& eff = table.m_efficiencies.back();
        eff.nominal = scale * hist->GetBinContent(x, y, z);
        if(statMode != StatMode::NONE)
        {
            uint16_t uid = (statMode==StatMode::GLOBAL)? globalStatUID : addStat(type, xmlStream);
            float err = hist->GetBinError(x, y, z);
            FakeBkgTools::Uncertainty uncdata{scale*err, scale*err};
            eff.uncertainties.emplace(uid, uncdata);
        }
    }
}

void Database::importSystTH1(const TH1* hist, EfficiencyType type, const std::string& sysname)
{
    if(!m_tables[type].size()) throw(GenericError() << "there should be another histogram containing central values to accompany the histogram " << hist->GetName());
    auto& table = m_tables[type].back();
    const int xmax = table.m_dimensions.front().nBounds;
    const int ymax = table.m_dimensions.size()>1? table.m_dimensions[1].nBounds : 2;
    const int zmax = table.m_dimensions.size()>2? table.m_dimensions[2].nBounds : 2;
    if(xmax!=hist->GetNbinsX()+1 || ymax!=hist->GetNbinsY()+1 || zmax!=hist->GetNbinsZ()+1)
    {
        throw(GenericError() << "binning mismatch between the nominal histogram and " << hist->GetName());
    }
    
    uint16_t uid;
    auto itr = std::find_if(m_systs.begin(), m_systs.end(), [&](const SystDef& sys){ return sys.name==sysname; });
    if(itr != m_systs.end())
    {
        uid = systIndexToUID(itr - m_systs.begin());
        itr->affects.set(type);
    }
    else
    {
        uid = systIndexToUID(m_systs.size());
        m_systs.emplace_back(sysname, (1 << type));
    }
    
    //loop through all bins once, to check whether all bins are zero error, 
    //or have the same central value as the nominal

    bool syst_central_equal_nom_central = true;
    bool syst_errors_equal_zero = true;
    bool syst_errors_equal_nom_errors = true;

    auto eff = table.m_efficiencies.begin();
    for(int x=1;x<xmax;++x)
    for(int y=1;y<ymax;++y)
    for(int z=1;z<zmax;++z)
    {
        if (fabs ((float)eff->nominal - (float)hist->GetBinContent(x, y, z)) > 0.001 ){ syst_central_equal_nom_central = false;}
        if ( hist->GetBinError(x, y, z) != 0 ) { syst_errors_equal_zero = false;}
        float stat_up = 0;
        for(auto& kv : eff->uncertainties)
        {
           if(!isStatUID(kv.first)) continue;
           stat_up = kv.second.up; break;
        }
        if ( fabs((float) hist->GetBinError(x, y, z) - (float) stat_up ) > 0.001) { syst_errors_equal_nom_errors = false;}
        ++eff;
    }

    // loop bins a second time and determine proceedure using above heuristics
    eff = table.m_efficiencies.begin();
    for(int x=1;x<xmax;++x)
    for(int y=1;y<ymax;++y)
    for(int z=1;z<zmax;++z)
    {
        
        float err =0;
        //want to support several possible notations:
        // a) central values are not the same as nominal: then we can assume
        // that the central values of the syst histos are the errors, 
        // (default nomenclature from the documentation)
        // b) if the central values for nominal and this hist are the same
        // then probably the errors are to be taken from the error bars!
        // but need to watch out for ambiguous cases
        //
        if (syst_central_equal_nom_central){  //central values are the same in nom and sys
          if (syst_errors_equal_nom_errors ){ // this case is ambiguous. Is it a 100% uncertainty?
            throw(GenericError() << "The central values and uncertainties for this systematic are identical to the nominal+stat uncertainties. This is ambiguous: did you mean to assign a 100% uncertainty? If so, please set all (unused) error bars to zero. ");
          } else if (syst_errors_equal_zero ) { //assume here that it was intended as 100% uncertainty
            err = hist->GetBinContent(x, y, z);
          } else {
            err = hist->GetBinError(x, y, z);
          }
        } else { // central values are different in nom and sys
          err = hist->GetBinContent(x, y, z);
        }
        

        FakeBkgTools::Uncertainty uncdata{err, err};
        if(!eff->uncertainties.emplace(uid, uncdata).second)
        {
            throw(GenericError() << "unexpected error: tried filling twice the same systematic");
        }
        ++eff;
    }
}

bool Database::retrieveParameterValue(const xAOD::IParticle& p, const xAOD::EventInfo& eventInfo, const Param& param, EfficiencyTable::BoundType& val) const
{
  #ifdef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
    float energy_scale = (m_useGeV? 0.001f : 1.f);  
  #else
    float energy_scale = 1;  
  #endif
    if(param.level == Param::Level::PARTICLE)
    {
        if(param.type==Param::Type::PREDEFINED_FLOAT || param.type==Param::Type::PREDEFINED_INT)
        {
            if(param.name=="pt") val.as_float = energy_scale * p.pt();
            else if(param.name=="eta") val.as_float = p.eta();
            else if(param.name=="|eta|") val.as_float = fabs(p.eta());
            else if(param.name=="phi") val.as_float = p.phi();
            else return false;
        }
        else if(param.type == Param::Type::CUSTOM_FLOAT) val.as_float = p.auxdataConst<float>(param.name);
        else if(param.type == Param::Type::CUSTOM_INT) val.as_int = p.auxdataConst<int>(param.name);
        else return false;
    }
    else if(param.level == Param::Level::EVENT)
    {
        if(param.type == Param::Type::CUSTOM_FLOAT) val.as_float = eventInfo.auxdataConst<float>(param.name);
        else if(param.type == Param::Type::CUSTOM_INT) val.as_int = eventInfo.auxdataConst<int>(param.name);
        else return false;
    }
    else return false;
    return true;
}

Efficiency* Database::selectEfficiency(ParticleData& pd, const xAOD::IParticle& p, EfficiencyType type)
{
    switch(p.type())
    {
      case xAOD::Type::Electron:
        if(type==ELECTRON_FAKE_FACTOR) return &pd.fake_factor;
        else if(type==ELECTRON_FAKE_EFFICIENCY) return &pd.fake_efficiency;
        else if(type==ELECTRON_REAL_EFFICIENCY) return &pd.real_efficiency;
        break;
      case xAOD::Type::Muon:
        if(type==MUON_FAKE_FACTOR) return &pd.fake_factor;
        else if(type==MUON_FAKE_EFFICIENCY) return &pd.fake_efficiency;
        else if(type==MUON_REAL_EFFICIENCY) return &pd.real_efficiency;
        break;
      case xAOD::Type::Tau:
        if(type==TAU_FAKE_FACTOR) return &pd.fake_factor;
        else if(type==TAU_FAKE_EFFICIENCY) return &pd.fake_efficiency;
        else if(type==TAU_REAL_EFFICIENCY) return &pd.real_efficiency;
        break;
      default:;
    }
    return nullptr;
}

/// This function is only called by the Database constructor
auto Database::selectTypesToFill(Client client) -> std::bitset<N_EFFICIENCY_TYPES> 
{
    std::bitset<N_EFFICIENCY_TYPES> result;
    if(client==Client::MATRIX_METHOD || client==Client::ALL_METHODS)
    {
        result[ELECTRON_REAL_EFFICIENCY] = true;
        result[MUON_REAL_EFFICIENCY] = true;
        result[TAU_REAL_EFFICIENCY] = true;
        result[ELECTRON_FAKE_EFFICIENCY] = true;
        result[MUON_FAKE_EFFICIENCY] = true;
        result[TAU_FAKE_EFFICIENCY] = true;
    }
    if(client==Client::FAKE_FACTOR || client==Client::ALL_METHODS)
    {
        result[ELECTRON_FAKE_FACTOR] = true;
        result[MUON_FAKE_FACTOR] = true;
        result[TAU_FAKE_FACTOR] = true;
    }
    if(result.none()) throw(GenericError() << "unrecognized client type, implementation incomplete");
    return result;
}

Database::EfficiencyType Database::getSourceType(EfficiencyType wantedType) const
{
    auto tables = m_tables.find(wantedType);
    if((tables==m_tables.end() || !tables->second.size()) && m_convertWhenMissing)
    {
        switch(wantedType)
        {
          case ELECTRON_FAKE_EFFICIENCY: return ELECTRON_FAKE_FACTOR;
          case MUON_FAKE_EFFICIENCY: return MUON_FAKE_FACTOR;
          case TAU_FAKE_EFFICIENCY: return TAU_FAKE_FACTOR;
          case ELECTRON_FAKE_FACTOR: return ELECTRON_FAKE_EFFICIENCY;
          case MUON_FAKE_FACTOR: return MUON_FAKE_EFFICIENCY;
          case TAU_FAKE_FACTOR: return TAU_FAKE_EFFICIENCY;
          default:;
        }
    }
    return wantedType;
}

unsigned Database::getXmlLineNumber(const char* pos) const
{
    if(!pos || !m_xmlBuffer.size() || !m_lineOffset.size()) return 0;
    if(pos < m_xmlBuffer.data()) return 0;
    unsigned offset = pos - m_xmlBuffer.data();
    if(offset >= m_xmlBuffer.size()) return 0;
    return std::upper_bound(m_lineOffset.begin(), m_lineOffset.end(), offset) - m_lineOffset.begin();
}

std::string Database::getTypeAsString(EfficiencyType type)
{
    /// This function is only meant to be used to display meaningful error messages
    switch(type)
    {
        case ELECTRON_REAL_EFFICIENCY: return "real efficiency (electrons)";
        case ELECTRON_FAKE_EFFICIENCY: return "fake efficiency (electrons)";
        case ELECTRON_FAKE_FACTOR: return "fake factor (electrons)";
        case MUON_REAL_EFFICIENCY: return "real efficiency (muons)";
        case MUON_FAKE_EFFICIENCY: return "fake efficiency (muons)";
        case MUON_FAKE_FACTOR: return "fake factor (muons)";
        case TAU_REAL_EFFICIENCY: return "real efficiency (taus)";
        case TAU_FAKE_EFFICIENCY: return "fake efficiency (taus)";
        case TAU_FAKE_FACTOR: return "fake factor (taus)";
        default:;
    }
    return "???";
}

std::string Database::StringRef::trim() const
{
    if(!ptr) return "";
    auto beg=ptr, end=endptr-1;
    while(std::isspace(*beg) && beg<end) ++beg;
    while(std::isspace(*end) && end>beg) --end;
    return std::string(beg, end+1);
}
