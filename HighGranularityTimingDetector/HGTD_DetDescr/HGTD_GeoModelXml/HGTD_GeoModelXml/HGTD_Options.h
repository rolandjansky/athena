/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GeoModelXml_HGTD_OPTIONS_H
#define HGTD_GeoModelXml_HGTD_OPTIONS_H

#include <string>


namespace InDetDDSLHC {
//
//    Class to store run time options.
//
class HGTD_Options {
public:
    HGTD_Options();
    bool alignable() const;
    bool alignAtModuleLevel() const;
    std::string gmxFilename() const;
    std::string detectorName() const;
    void setAlignable(bool flag = false);
    void setAlignAtModuleLevel(bool flag = false);
    void setGmxFilename(std::string filename); 
    void setDetectorName(std::string detectorname);

private:
    bool m_alignable;
    bool m_alignModule;
    std::string m_gmxFilename;
    std::string m_detectorName;
};

// Inlined methods

 inline HGTD_Options::HGTD_Options() :
   m_alignable(false),
   m_alignModule(false), 
   m_gmxFilename(""),
   m_detectorName("HGTD")
{}

inline bool HGTD_Options::alignable() const {
    return m_alignable;
}

inline bool HGTD_Options::alignAtModuleLevel() const {
    return m_alignModule;
}

inline std::string HGTD_Options::gmxFilename() const {
    return m_gmxFilename;
}

inline std::string HGTD_Options::detectorName() const {
    return m_detectorName;
}

inline void HGTD_Options::setAlignable(bool flag) {
    m_alignable = flag;
}

inline void HGTD_Options::setAlignAtModuleLevel(bool flag) {
    m_alignModule = flag;
}

inline void HGTD_Options::setGmxFilename(std::string filename) {
    m_gmxFilename = filename;
}

inline void HGTD_Options::setDetectorName(std::string detectorname) {
    m_detectorName = detectorname;
}

} // end namespace InDetDDSLHC

#endif // HGTD_GeoModelXml_HGTD_OPTIONS_H
