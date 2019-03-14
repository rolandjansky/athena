/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 *  XmlStreamer.cpp
 *  Formatter
 *
 *  Created by sroe on 12/01/2010.
 *
 */

#include "XmlStreamer.h"

typedef std::map<std::string, std::string> Attributes_t;
typedef std::vector<std::string> NameValue_t;
static Attributes_t empty;

namespace SCT_CalibAlgs {

  XmlStreamer::XmlStreamer(const std::string& name, const Attributes_t& attributes, std::ostream& os):
    IElementStreamer(name, attributes, os) {
    m_os << "\n<" << m_name;
    if (not attributes.empty()) {
      Attributes_t::const_iterator i{attributes.begin()};
      const Attributes_t::const_iterator end{attributes.end()};
      for (; i!=end; ++i) {
        m_os << " " << i->first << "=\"" << i->second << "\"";
      }
    }
    m_os << ">";
    m_os.flush();
  }

  XmlStreamer::XmlStreamer(const std::string& name, const NameValue_t& attributeNames, const NameValue_t& attributeValues, std::ostream& os):
    IElementStreamer(name, attributeNames, attributeValues, os) {
    m_os << "\n<" << m_name;
    for (unsigned int i{0}; i!=attributeNames.size(); ++i) {
      m_os << " " << attributeNames[i] << "=\"" << attributeValues[i] << "\"";
    }
    m_os << ">";
    m_os.flush();
  }

  XmlStreamer::XmlStreamer(const std::string& name, std::ostream& os)
    :IElementStreamer(name, empty, os) {
    m_os << "\n<" << m_name << ">";
    m_os.flush();
  }

  XmlStreamer::XmlStreamer(const std::string& name, const std::string& attributeName, const std::string& attributeValue, std::ostream& os):
    IElementStreamer(name, attributeName, attributeValue, os) {
    m_os << "\n<" << m_name;
    for (unsigned int i{0}; i!=m_attributeNames.size(); ++i) {
      m_os << " " << m_attributeNames[i] << "=\"" << m_attributeValues[i] << "\"";
    }
    m_os << ">";
    m_os.flush();
  }

  XmlStreamer::~XmlStreamer() {
    m_os << "</" << m_name << ">";
    if (m_depth==1) m_os << "\n";
  }

}
