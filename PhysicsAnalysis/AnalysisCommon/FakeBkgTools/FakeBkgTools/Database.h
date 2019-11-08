/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKEBKGTOOLS_DATABASE_H
#define FAKEBKGTOOLS_DATABASE_H

#include "FakeBkgTools/FakeBkgInternals.h"
#include "xAODBase/IParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <bitset>
#include <sstream>
#include <exception>

class TFile;
class TH1;

namespace FakeBkgTools
{

class Database
{
public:
    Database(Client client, bool useGeV, bool convertWhenMissing);
    
    void importXML(std::string filename);
    void importDefaultROOT(std::string filename);
    bool ready() const;
    void reset();
    bool needEventInfo() const;
    
    bool fillEfficiencies(ParticleData& pd, const xAOD::IParticle& p, const xAOD::EventInfo& eventInfo, std::string& error) const;
    unsigned getXmlLineNumber(const char* pos) const;
    
    enum EfficiencyType
    {
        ELECTRON_REAL_EFFICIENCY = 0,
        ELECTRON_FAKE_EFFICIENCY,
        ELECTRON_FAKE_FACTOR,
        MUON_REAL_EFFICIENCY,
        MUON_FAKE_EFFICIENCY,
        MUON_FAKE_FACTOR,
        TAU_REAL_EFFICIENCY,
        TAU_FAKE_EFFICIENCY,
        TAU_FAKE_FACTOR,
        N_EFFICIENCY_TYPES
    };
    
    enum class InputType
    {
        CENTRAL_VALUE, CORRECTION
    };
    enum class StatMode
    {
        GLOBAL, PER_BIN, NONE, UNSPECIFIED
    };
    
    struct EfficiencyTable
    {
        struct Dimension
        {
            unsigned short paramUID = 0, iMinBound = 0, nBounds = 0;
        };
        
        union BoundType
        {
            int as_int;
            float as_float;
        };
        std::vector<Efficiency> m_efficiencies;
        std::vector<BoundType> m_bounds;
        std::vector<Dimension> m_dimensions;
        InputType inputType;
        unsigned numberOfBins() const
        {
            unsigned nBins = 1;
            for(auto& dim : m_dimensions) nBins *= (dim.nBounds - 1);
            return nBins;
        }
    };
    
    /// Note: the following structure is used (instead of a simple std::string)
    /// so that XML line numbers can be printed in the error message, which requires 
    /// keeping track of the position of the string in the original file.
    /// Might be rewritten around a std::string_view one day
    struct StringRef 
    {
        const char* ptr = nullptr, *endptr = nullptr;
        operator bool() const { return ptr && endptr && (ptr!=endptr); }
        bool operator==(const char* s) const
        {
          #if __cplusplus >= 201402L /// C++14
            return std::equal(ptr, endptr, s, s+std::strlen(s));
          #else /// if people don't use a C++14-compliant compiler
            return str() == s;
          #endif
        }
        bool operator!=(const char* s) const { return !operator==(s); }
        bool operator<(const StringRef& rhs) const { return endptr < rhs.ptr; } /// used to check if a piece of text appeared before another piece of text
        template<class Str> std::string operator+(Str s) { return str() + s; }
        std::size_t length() const { return (endptr - ptr); }
        void clear() { ptr = endptr = nullptr; }
        std::string str() const { return std::string(ptr, endptr); }
        StringRef() {}
        StringRef(const char* newptr, const char* newendptr) : ptr(newptr), endptr(newendptr) {}
        StringRef(const char* newptr, std::size_t length) : ptr(newptr), endptr(newptr+length) {}
        void set(const char* newptr, const char* newendptr) { *this = StringRef(newptr, newendptr); }
        void set(const char* newptr, std::size_t length) { *this = StringRef(newptr, length); }
        std::string trim() const;
    };
    
    /// This propagates an error message + the reference to the faulty piece of XML
    /// when an exception is raised
    struct XmlError : public std::exception
    {
        std::string reason;
        const StringRef location;
        XmlError(const StringRef& l) : reason(""), location(l) {}
        XmlError(const char* s, std::size_t length) : reason(""), location(s, s+length) {}
        XmlError& operator<<(const char* msg) { reason += msg; return *this; }
        XmlError& operator<<(const std::string& msg) { reason += msg; return *this; }
        XmlError& operator<<(int msg) { reason += std::to_string(msg); return *this; }
        virtual const char* what() const noexcept override { return reason.c_str(); }
    };
    /// This propagates an error message
    struct GenericError : public std::exception
    {
        std::string reason;
        GenericError& operator<<(const char* msg) { reason += msg; return *this; }
        GenericError& operator<<(const std::string& msg) { reason += msg; return *this; }
        GenericError& operator<<(int msg) { reason += std::to_string(msg); return *this; }
        virtual const char* what() const noexcept override { return reason.c_str(); }
    };
    
    struct Param
    {
        enum class Type
        {
            PREDEFINED_INT, PREDEFINED_FLOAT, CUSTOM_INT, CUSTOM_FLOAT
        };
        enum class Level
        {
            PARTICLE, EVENT
        };
        std::string name;
        Type type;
        Level level;
        Param(const std::string& n, Type t, Level l) : name(n), type(t), level(l) {}
        bool integer() const { return type==Type::PREDEFINED_INT || type==Type::CUSTOM_INT; }
    };
    
    struct SystDef
    {
        const std::string name;
        std::bitset<N_EFFICIENCY_TYPES> affects;
        SystDef(const std::string& n, const std::bitset<N_EFFICIENCY_TYPES>& a) : name(n), affects(a) {}
        SystDef(SystDef&& rhs) = default;
    };
    struct StatDef /// could try to keep track of the bin number, for the description
    {
        const std::bitset<N_EFFICIENCY_TYPES> affects;
        StatDef(long a) : affects(a) {}
    };
    
protected:
    using AttributesMap = std::map<std::string, StringRef>;
    
    /// Methods used to parse XML files
    void dropXmlComments(std::string& buffer);
    void dropRootTag(std::string& buffer);
    void readNextTag(StringRef& stream, StringRef& tag, AttributesMap& attributes, StringRef& contents);
    void readTagAttributes(StringRef stream, const std::string& tag, AttributesMap& attributes);
    void resetAttributes(AttributesMap& attributes);
    void importCustomROOT(const StringRef& tag, const StringRef& contents, const AttributesMap& attributes);
    void addParams(const StringRef& tag, const StringRef& contents, AttributesMap& attributes);
    void addSysts(const StringRef& tag, const StringRef& contents, const AttributesMap& attributes);
    unsigned short addStat(EfficiencyType type, const StringRef& pos = StringRef());
    void addTables(const StringRef& particleType, const AttributesMap& attributes, const StringRef& contents, TFile* source = nullptr);
    void addDimension(EfficiencyTable& table, unsigned paramUID, const StringRef& contents);
    void addValues(const StringRef& contents, EfficiencyTable& table, EfficiencyType type, StatMode statMode, unsigned short& globalStatUID);
    
    /// Methods used to load from ROOT files
    void importNominalTH1(const TH1* hist, EfficiencyType type, const StringRef& paramX, const StringRef& paramY, const StringRef& paramZ, 
            float scale, StatMode statMode, unsigned short& globalStatUID, const StringRef& xmlStream);
    void importSystTH1(const TH1* hist, EfficiencyType type, const std::string& sysname);
    float getWeightedAverage(const TH1* hist, const StringRef& xmlStream);
    float getNormalizationFactor(const TH1* hist, EfficiencyType type, const StringRef& norm, const StringRef& xmlStream);
    
    /// Methods used to fill efficiencies
    static FakeBkgTools::Efficiency* selectEfficiency(FakeBkgTools::ParticleData& pd, const xAOD::IParticle& p, EfficiencyType type);
    bool retrieveParameterValue(const xAOD::IParticle& p, const xAOD::EventInfo& eventInfo, const Param& param, EfficiencyTable::BoundType& val) const;
    EfficiencyType getSourceType(EfficiencyType wantedType) const;
    int readEfficiencyFromTable(Efficiency& eff, const EfficiencyTable& table, std::map<unsigned, EfficiencyTable::BoundType>& cachedParamVals, const xAOD::IParticle& p, const xAOD::EventInfo& eventInfo, std::string& error) const;

    /// Helper methods
    template<typename ReturnValue, typename... Args>
    static inline ReturnValue getAttribute(const StringRef& tag, const AttributesMap& attributes, const std::string& type, const char* ref, ReturnValue rv, Args... args);
    static inline void assertNoLeftover(std::stringstream& ss, const StringRef& pos);
    std::vector<std::string> getListOfNames(const StringRef& stream);
    static constexpr unsigned short maxIndex() { return 0x4000; }
    static std::string getTypeAsString(EfficiencyType type);

private: /// don't call these methods directly, use getAttribute(AttributesMap, string, ...) instead
    template<typename ReturnValue>
    static inline ReturnValue getAttribute(const StringRef& attr, const char* ref, ReturnValue rv);
    template<typename ReturnValue, typename... Args>
    static inline ReturnValue getAttribute(const StringRef& attr, const char* ref, ReturnValue rv, Args... args);

public:
    static constexpr unsigned short indexFromUID(unsigned short uid) { return uid&0x3FFF; }
    static constexpr unsigned short systIndexToUID(unsigned short index) { return 0x4000 | index; }
    static constexpr unsigned short statIndexToUID(unsigned short index) { return 0x8000 | index; }
    static constexpr bool isSystUID(unsigned short uid) { return ((uid&0xC000) == 0x4000); }
    static constexpr bool isStatUID(unsigned short uid) { return ((uid&0xC000) == 0x8000); }
    unsigned numberOfStats() const { return m_stats.size(); }
    unsigned numberOfSysts() const { return m_systs.size(); }
    const SystDef* findSyst(unsigned short uid) const
    {
        if(!isSystUID(uid)) return nullptr;
        unsigned index = indexFromUID(uid);
        return (index<m_systs.size())? &m_systs[index] : nullptr;
    }
    const StatDef* findStat(unsigned short uid) const
    {
        if(!isStatUID(uid)) return nullptr;
        unsigned index = indexFromUID(uid);
        return (index<m_stats.size())? &m_stats[index] : nullptr;
    }

protected:
    ///
    const bool m_useGeV;
    const std::bitset<N_EFFICIENCY_TYPES> m_typesToFill;
    static std::bitset<N_EFFICIENCY_TYPES> selectTypesToFill(Client client);
    const bool m_convertWhenMissing;
    bool m_storeAllUncertainties = true;
    
    /// Permanent buffers
    std::vector<Param> m_params;
    std::vector<SystDef> m_systs;
    std::vector<StatDef> m_stats;
    std::map<int, std::vector<EfficiencyTable>> m_tables;
    
    /// Temporary buffers (only used while importing data)
    std::string m_xmlBuffer;
    std::vector<std::size_t> m_lineOffset;
    std::map<std::string, float> m_normFactors;
    
};

}

#endif
