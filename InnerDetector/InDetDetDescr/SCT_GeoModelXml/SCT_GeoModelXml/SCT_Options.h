/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModelXml_SCT_Options_H
#define SCT_GeoModelXml_SCT_Options_H
#include <string>

namespace InDetDDSLHC {
//
//    Class to store run time options.
//
class SCT_Options {
public:
    SCT_Options();
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

inline SCT_Options::SCT_Options() :
    m_alignable(true),
    m_alignModule(true), 
    m_gmxFilename("")
{}

inline bool SCT_Options::alignable() const {
    return m_alignable;
}

inline bool SCT_Options::alignAtModuleLevel() const {
    return m_alignModule;
}

inline std::string SCT_Options::gmxFilename() const {
    return m_gmxFilename;
}

inline void SCT_Options::setAlignable(bool flag) {
    m_alignable = flag;
}

inline void SCT_Options::setAlignAtModuleLevel(bool flag) {
    m_alignModule = flag;
}

inline void SCT_Options::setGmxFilename(std::string filename) {
    m_gmxFilename = filename;
}

}
#endif // SCT_GeoModelXml_SCT_Options_H
