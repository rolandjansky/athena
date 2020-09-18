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
    std::string detectorName() const;
    void setAlignable(bool flag = true);
    void setAlignAtModuleLevel(bool flag = true);
    void setGmxFilename(std::string filename); 
    void setDetectorName(std::string detectorname);

private:
    bool m_alignable;
    bool m_alignModule;
    std::string m_gmxFilename;
    std::string m_detectorName;
};

// Inlined methods

 inline StripOptions::StripOptions() :
   m_alignable(true),
   m_alignModule(true), 
   m_gmxFilename(""),
   m_detectorName("ITkStrip")
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

inline std::string StripOptions::detectorName() const {
    return m_detectorName;
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

inline void StripOptions::setDetectorName(std::string detectorname) {
    m_detectorName = detectorname;
}

}
#endif // StripGeoModelXml_StripOptions_H
