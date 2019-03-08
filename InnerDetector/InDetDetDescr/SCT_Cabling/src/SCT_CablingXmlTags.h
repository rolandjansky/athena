/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CablingXmlTags_H
#define SCT_CablingXmlTags_H
/**
 *  @file SCT_CablingXmlTags.h
 *  Header file for tags to be used in Xml output from the test algorithm
 *
 *  @author Shaun Roe 
 *  @date 14/12/2014.
 *
 */

#include "GaudiKernel/time_r.h" // localtime_r

#include <ctime> // std::time_t, std::tm
#include <iomanip> // put_time
#include <iostream>
#include <string>
#include <sstream>
 
namespace SCT_Cabling{
  const std::string XmlHeader("<?xml version=\"1.0\"?>");
  const std::string OpenRootTag("<SCT_textFile>");
  const std::string OpenRodMappingTag("<textList name=\"SCT Rod-Rob mapping\" headings=\"RodId RobId\">");
  const std::string CloseRodMappingTag("</textList>");
  const std::string OpenModuleMappingTag("<textList name=\"SCT cabling mapping\" headings=\"Rod Fibre Bec LayerDisk Eta Phi Side RobId Sn\">");
  const std::string CloseModuleMappingTag("</textList>");
  const std::string CloseRootTag("</SCT_textFile>");
   
  std::string intToHex(const unsigned int i){
    std::stringstream stream;
    stream << "0x" << std::setfill ('0') << std::setw(6) << std::hex << i;
    return stream.str();
  }   
   
  std::string formatRodOutput(const unsigned int rodNum){
    const std::string rodString(SCT_Cabling::intToHex(rodNum));
    const std::string sep(" ");
    return rodString+sep+rodString;
  }
  
  std::string formatModuleOutput(const unsigned int rod, const unsigned int fibre, 
                                 const int bec,const int layerDisk, const int eta, const int phi, const int side, const int robId,
                                 const std::string fullsn){
    const std::string sep(" ");
    using std::to_string;
    std::string result= to_string(rod)+sep+to_string(fibre)+sep+to_string(bec)+sep+to_string(layerDisk)+sep;
    result+=(to_string(eta)+sep+to_string(phi)+sep+to_string(side)+sep+SCT_Cabling::intToHex(robId)+sep+fullsn);
    return result;
  }
     
  std::string dateTime(){
    std::stringstream ss;
    std::time_t t = std::time(nullptr);
    std::tm lt;
    ss << std::put_time(localtime_r(&t, &lt), "%Y-%m-%d %X");
    return ss.str();
  }
  std::string makeCablingFileName(){
    const std::string prefix("SCT_");
    const std::string suffix("Cabling_svc.dat");
    const std::string meat(SCT_Cabling::dateTime());
    return prefix+meat+suffix;
  }
}
 
#endif // SCT_CablingXmlTags_H
