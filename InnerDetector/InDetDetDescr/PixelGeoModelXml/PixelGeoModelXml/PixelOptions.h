/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeoModelXml_PixelOptions_H
#define PixelGeoModelXml_PixelOptions_H
#include <string>

namespace InDetDDSLHC {
  //
  //    Class to store run time options.
  //
  class PixelOptions {
  public:
    PixelOptions();
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

  inline PixelOptions::PixelOptions() :
    m_alignable(true),
    m_alignModule(true), 
    m_gmxFilename(""),
    m_detectorName("ITkPixel")
      {}

  inline bool PixelOptions::alignable() const {
    return m_alignable;
  }

  inline bool PixelOptions::alignAtModuleLevel() const {
    return m_alignModule;
  }

  inline std::string PixelOptions::gmxFilename() const {
    return m_gmxFilename;
  }

  inline std::string PixelOptions::detectorName() const {
    return m_detectorName;
  }

  inline void PixelOptions::setAlignable(bool flag) {
    m_alignable = flag;
  }

  inline void PixelOptions::setAlignAtModuleLevel(bool flag) {
    m_alignModule = flag;
  }

  inline void PixelOptions::setGmxFilename(std::string filename) {
    m_gmxFilename = filename;
  }

  inline void PixelOptions::setDetectorName(std::string detectorname) {
    m_detectorName = detectorname;
  }

}
#endif // PixelGeoModelXml_PixelOptions_H
