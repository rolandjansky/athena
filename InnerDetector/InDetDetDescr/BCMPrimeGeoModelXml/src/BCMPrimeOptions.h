/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEGEOMODELXML_BCMPRIMEOPTIONS_H
#define BCMPRIMEGEOMODELXML_BCMPRIMEOPTIONS_H
#include <string>

namespace InDetDD
{

  //
  //    Class to store run time options.
  //
  class BCMPrimeOptions {
  public:
    BCMPrimeOptions();
    bool alignable() const;
    bool alignAtModuleLevel() const;
    std::string gmxFilename() const;
    std::string detectorName() const;
    void setAlignable(bool flag = true);
    void setAlignAtModuleLevel(bool flag = true);
    void setGmxFilename(const std::string & filename);
    void setDetectorName(const std::string & detectorname);

  private:
    bool m_alignable;
    bool m_alignModule;
    std::string m_gmxFilename;
    std::string m_detectorName;
  };

  //
  // Inlined methods
  //
  inline BCMPrimeOptions::BCMPrimeOptions() :
    m_alignable(true),
    m_alignModule(true),
    m_gmxFilename(""),
    m_detectorName("BCMPrime")
      {}

  inline bool BCMPrimeOptions::alignable() const {
    return m_alignable;
  }

  inline bool BCMPrimeOptions::alignAtModuleLevel() const {
    return m_alignModule;
  }

  inline std::string BCMPrimeOptions::gmxFilename() const {
    return m_gmxFilename;
  }

  inline std::string BCMPrimeOptions::detectorName() const {
    return m_detectorName;
  }

  inline void BCMPrimeOptions::setAlignable(bool flag) {
    m_alignable = flag;
  }

  inline void BCMPrimeOptions::setAlignAtModuleLevel(bool flag) {
    m_alignModule = flag;
  }

  inline void BCMPrimeOptions::setGmxFilename(const std::string & filename) {
    m_gmxFilename = filename;
  }

  inline void BCMPrimeOptions::setDetectorName(const std::string & detectorname) {
    m_detectorName = detectorname;
  }

} // namespace InDetDD

#endif // BCMPRIMEGEOMODELXML_BCMPRIMEOPTIONS_H
