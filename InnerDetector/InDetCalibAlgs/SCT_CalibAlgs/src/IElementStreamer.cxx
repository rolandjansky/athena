/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 *  IElementStreamer.cpp
 *  Formatter
 *
 *  Created by sroe on 12/01/2010.
 *
 */
#include "IElementStreamer.h"

namespace SCT_CalibAlgs {

  IElementStreamer::IElementStreamer(const std::string& name, const std::map<std::string, std::string>& attributeMap, std::ostream& os):
    m_name{name}, m_os{os}, m_depth{0}, m_nodeIndex{0}, m_nodeId{m_nodeIndex} {
    std::map<std::string, std::string>::const_iterator i{attributeMap.begin()};
    std::map<std::string, std::string>::const_iterator end{attributeMap.end()};
    for (; i!=end; ++i) {
      m_attributeNames.push_back(i->first);
      m_attributeValues.push_back(i->second);
    }
    ++m_depth;
    ++m_nodeIndex;
  }
  
  IElementStreamer::IElementStreamer(const std::string& name, const std::vector<std::string>& attributeNames, const std::vector<std::string>& attributeValues, std::ostream& os):
    m_name{name}, m_attributeNames{attributeNames}, m_attributeValues{attributeValues}, m_os{os}, m_nodeId{m_nodeIndex} {
    ++m_depth;
    ++m_nodeIndex;
    } 

  IElementStreamer::IElementStreamer(const std::string& name, const std::string& attributeName, const std::string& attributeValue, std::ostream& os):
    m_name{name}, m_os{os}, m_nodeId{m_nodeIndex} {
    m_attributeNames.push_back(attributeName);
    m_attributeValues.push_back(attributeValue);
    ++m_depth;
    ++m_nodeIndex;
  }

  IElementStreamer::~IElementStreamer() {
    --m_depth;
  }

}
