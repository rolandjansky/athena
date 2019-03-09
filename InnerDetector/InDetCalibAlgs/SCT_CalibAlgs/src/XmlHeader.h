/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file XmlHeader.h
 * header file for the XmlHeader class
 *
 * @author Shaun Roe shaun.roe@cern.ch
 **/

#ifndef SCT_CalibAlgs_XmlHeader_h
#define SCT_CalibAlgs_XmlHeader_h
#include <iosfwd>
#include <string>

namespace SCT_CalibAlgs {
  class XmlHeader {
  public:
    XmlHeader(std::ostream& output);
    void addStylesheet(const std::string& stylesheetPath);

  private:
    std::ostream& m_os;
  };
}

#endif
