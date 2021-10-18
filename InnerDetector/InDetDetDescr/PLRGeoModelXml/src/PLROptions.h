/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLRGeoModelXml_PLROptions_H
#define PLRGeoModelXml_PLROptions_H
#include <string>

namespace InDetDD
{
  //
  //    Class to store run time options.
  //
  class PLROptions {
  public:
    PLROptions();
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

  inline PLROptions::PLROptions() :
    m_alignable(true),
    m_alignModule(true),
    m_gmxFilename(""),
    m_detectorName("PLR")
      {}

  inline bool PLROptions::alignable() const {
    return m_alignable;
  }

  inline bool PLROptions::alignAtModuleLevel() const {
    return m_alignModule;
  }

  inline std::string PLROptions::gmxFilename() const {
    return m_gmxFilename;
  }

  inline std::string PLROptions::detectorName() const {
    return m_detectorName;
  }

  inline void PLROptions::setAlignable(bool flag) {
    m_alignable = flag;
  }

  inline void PLROptions::setAlignAtModuleLevel(bool flag) {
    m_alignModule = flag;
  }

  inline void PLROptions::setGmxFilename(std::string filename) {
    m_gmxFilename = filename;
  }

  inline void PLROptions::setDetectorName(std::string detectorname) {
    m_detectorName = detectorname;
  }

} // namespace InDetDD

#endif // PLRGeoModelXml_PLROptions_H
