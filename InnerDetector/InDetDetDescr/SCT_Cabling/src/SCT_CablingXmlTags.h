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
 
 #include <iostream>
#include <string>
#include <sstream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <iomanip> // put_time


 
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
     //no 'put_time' in gcc48 or gcc49 :-(
     //ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
     std::time_t t = std::time(NULL);
     char mbstr[100];
     if (std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d", std::localtime(&t))) {
       ss << mbstr;
     }
  
     return ss.str();
	 }
	 std::string makeCablingFileName(){
	 	 const std::string prefix("SCT_");
	 	 const std::string suffix("Cabling_svc.dat");
	 	 const std::string meat(SCT_Cabling::dateTime());
	 	 return prefix+meat+suffix;
	 }
 }
 
 #endif
 
 