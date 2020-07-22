/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StripGeoModelXml_StripOptions_H
#define StripGeoModelXml_StripOptions_H
#include <string>

namespace InDetDDSLHC {
//
//    Class to store run time options.
//
class StripOptions {
public:
    StripOptions();
    bool alignable() const;
    bool alignAtModuleLevel() const;
    std::string gmxFilename() const;
    void setAlignable(bool flag = true);
    void setAlignAtModuleLevel(bool flag = true);
    void setGmxFilename(std::string filename);  

private:
    bool m_alignable;
    bool m_alignModule;
    std::string m_gmxFilename;
};

// Inlined methods

inline StripOptions::StripOptions() :
    m_alignable(true),
    m_alignModule(true), 
    m_gmxFilename("")
{}

inline bool StripOptions::alignable() const {
    return m_alignable;
}

inline bool StripOptions::alignAtModuleLevel() const {
    return m_alignModule;
}

inline std::string StripOptions::gmxFilename() const {
    return m_gmxFilename;
}

inline void StripOptions::setAlignable(bool flag) {
    m_alignable = flag;
}

inline void StripOptions::setAlignAtModuleLevel(bool flag) {
    m_alignModule = flag;
}

inline void StripOptions::setGmxFilename(std::string filename) {
    m_gmxFilename = filename;
}

}
#endif // StripGeoModelXml_StripOptions_H
