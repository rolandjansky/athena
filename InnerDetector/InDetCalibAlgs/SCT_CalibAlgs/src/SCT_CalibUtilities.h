/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibUtilities.h
 * header file for the SCTCalibUtilities
 *
 * @author Shaun Roe shaun.roe@cern.ch
 **/

#ifndef SCT_CalibAlgs_SCT_CalibUtilities_h
#define SCT_CalibAlgs_SCT_CalibUtilities_h

#include <string>
#include <boost/lexical_cast.hpp>

class Identifier;
class SCT_ID;
class EventInfo;

namespace SCT_CalibAlgs {

  std::string eventInfoAsString(const EventInfo* theEvent);
  std::string formatPosition(const Identifier& waferId, const SCT_ID* helper, const std::string& delimiter="_", const bool includeSide=true);
  std::string chipList2LinkList(const std::string& chipList);
  unsigned int bec2Index(const int bec);

  template <class T>
    std::string xmlValue(const std::string& name, const T value) {
    std::ostringstream os;
    os << "  <value name=\"" << name << "\">" << value << "</value>";
    return os.str();
  }

  template <class T>
    std::string xmlOpenChannel(const long id, const T since, const T until) {
    std::string opener=std::string{"  <channel id=\""}+boost::lexical_cast<std::string>(id)+"\"";
    std::string sinceAttr=std::string{" since=\""}+boost::lexical_cast<std::string>(since)+"\"";
    std::string untilAttr=std::string{" until=\""}+boost::lexical_cast<std::string>(until)+"\"";
    return opener+sinceAttr+untilAttr+">";
  }

  std::string xmlCloseChannel();

} //end of namespace

#endif
