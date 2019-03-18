/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 *  IElementStreamer.h
 *  Formatter
 *
 *  Created by sroe on 12/01/2010.
 *
 */
#ifndef IElementStreamer_h
#define IElementStreamer_h
#include <iostream>
#include <string>
#include <map>
#include <vector>
namespace SCT_CalibAlgs {

  class IElementStreamer {

  protected:
    std::string m_name;
    std::vector<std::string> m_attributeNames;
    std::vector<std::string> m_attributeValues;
    std::ostream& m_os;
    unsigned int m_depth;
    unsigned int m_nodeIndex;
    unsigned int m_nodeId;

  public:
    IElementStreamer(const std::string& name, const std::map<std::string, std::string>& attributeMap, std::ostream& os=std::cout);
    IElementStreamer(const std::string& name, const std::string& attributeName, const std::string& attributeValue, std::ostream& os=std::cout);
    IElementStreamer(const std::string& name, const std::vector<std::string>& attributeNames, const std::vector<std::string>& attributeValues, std::ostream& os=std::cout);
    ~IElementStreamer();
    template<class T>
      void value(const T& v) {
      m_os<<v;
    }
  };

}
#endif
