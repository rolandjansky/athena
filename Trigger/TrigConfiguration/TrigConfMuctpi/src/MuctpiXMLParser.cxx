/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// libraries
#include "TrigConfMuctpi/MioctTopoCellGeometry.h"
#include "TrigConfMuctpi/MioctROIGeometry.h"
#include "TrigConfMuctpi/MioctSectorGeometry.h"
#include "TrigConfMuctpi/MioctGeometry.h"
#include "TrigConfMuctpi/MuCTPiGeometry.h"
#include "TrigConfMuctpi/MuctpiXMLParser.h"


// cpp libraries
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <boost/lexical_cast.hpp> 

// boost libraries
#include <boost/foreach.hpp>

using namespace std;
using boost::property_tree::ptree;

namespace pt = boost::property_tree;


MuctpiXMLParser::MuctpiXMLParser() :
  TrigConf::TrigConfMessaging("MuctpiXMLParser")
{}

MuctpiXMLParser::~MuctpiXMLParser()
{}


void
MuctpiXMLParser::printConfiguration() const {
  cout << "************************** MuctpiXMLParser **********************" << endl;
  m_muctpi.print();
  cout << "*****************************************************************" << endl;
}


void
MuctpiXMLParser::readConfiguration(const std::string & inputfile)
{
   // populate the ptree m_configuration with the structure of the xml input file
   ptree inputTree;
   read_xml(inputfile, inputTree); // 3 --> trim_whitespace

   m_muctpiPT = inputTree.get_child("MuCTPiGeometry");

   TRG_MSG_INFO("Read " << inputfile << " successfully!");
}


void MuctpiXMLParser::parseConfiguration()
{

   // iterate through elements of topomenu
   for(const boost::property_tree::ptree::value_type &x: m_muctpiPT) {

      string menuElementName = x.first;
      if (menuElementName=="<xmlattr>" || menuElementName=="<xmlcomment>") continue;
      ptree menuElement = x.second;

      // get branch attributes
      if( menuElementName=="MIOCT" ) {

         MioctGeometry octant;
         setOctant(menuElement, octant);
         m_muctpi.addOctant(octant);

      } else if( menuElementName=="PtEncoding" ) {

         L1MuonPtEncoding ptEncoding;
         setPtEncoding(menuElement, ptEncoding);
         m_muctpi.setPtEncoding(ptEncoding);

      } else {
         // TRG_MSG_FATAL("Unknown element" << menuElementName); // validation is checked by the dtd
      }
   }


   TRG_MSG_INFO("Parsing Successful!");

   // m_muctpi.print();

}


void MuctpiXMLParser::setPtEncoding(  boost::property_tree::ptree menuElement, L1MuonPtEncoding & ptEncoding)
{
   
   for(const auto & x : menuElement ) {

      if( x.first == "PtCodeElement" ) {

         unsigned int idx = m_xmlHelper.getUIntAttribute(x.second, "pt") - 1;
         unsigned int ptCode = m_xmlHelper.getUIntAttribute(x.second, "code");
         unsigned int thresholdValue = m_xmlHelper.getUIntAttribute(x.second, "value");

         ptEncoding.setCodingInfo(idx, ptCode, thresholdValue);

      }
   }
}


void MuctpiXMLParser::setOctant(  boost::property_tree::ptree menuElement, MioctGeometry &octant)
{
   octant.setMioctId(  m_xmlHelper.getUIntAttribute(menuElement,"id"));
   octant.setSlot(     m_xmlHelper.getUIntAttribute(menuElement,"slot"));
   
   // <Sector>'s
   //unsigned int counter = 0;
   for(const boost::property_tree::ptree::value_type &x:menuElement ) {
      //cout << "reading "<< counter<< " octant\n"; counter++;
      string menuElementNameO = x.first;
      if (menuElementNameO=="<xmlattr>" || menuElementNameO=="<xmlcomment>") continue;
      ptree menuElementO = x.second;
      // get branch attributes

      if( menuElementNameO== "Sector" ){
         MioctSectorGeometry  sector;
         setSector(menuElementO, sector);
         octant.addSector(sector);
      }
      // Decode/TopoCell
      else if( menuElementNameO == "Decode" ){
         for(const boost::property_tree::ptree::value_type &y: menuElementO) {
            string menuElementName1 = y.first;
            if (menuElementName1=="<xmlattr>" || menuElementName1=="<xmlcomment>") continue;
            ptree menuElement1 = y.second;
            MioctTopoCellGeometry  tcell;
            setTopoCell(menuElement1, tcell);
            octant.addTopoCell(tcell);
         }
      } // end of decode
   }// end of menuEleement loop
}


void MuctpiXMLParser::setSector(  boost::property_tree::ptree menuElement, MioctSectorGeometry &sector)
{
   sector.setConnector(m_xmlHelper.getUIntAttribute(menuElement,"connector") );
   sector.setName(m_xmlHelper.getAttribute(menuElement,"name") );
   
   //unsigned int counter = 0;
   for(const boost::property_tree::ptree::value_type &x:menuElement ) {

      string menuElementNameO = x.first;
      if (menuElementNameO=="<xmlattr>" || menuElementNameO=="<xmlcomment>") continue;
      ptree menuElementO = x.second;
      // get branch attributes

      if( menuElementNameO== "ROI" ){
         //cout << "reading "<< counter<<" ROI\n"; counter++;
         MioctROIGeometry  roi;
         setROI(menuElementO, roi);
         sector.addROI(roi);
      }
   }
}

void MuctpiXMLParser::setROI(     boost::property_tree::ptree menuElement, MioctROIGeometry &data)
{
    unsigned int x = 0;
    std::stringstream ss; ss  << std::hex ;

    data.setEtamin(m_xmlHelper.getFloatAttribute(menuElement, "etamin"));
    data.setEtamax(m_xmlHelper.getFloatAttribute(menuElement, "etamax"));
    data.setEta(m_xmlHelper.getFloatAttribute(menuElement, "eta"));
    data.setPhimin(m_xmlHelper.getFloatAttribute(menuElement, "phimin"));
    data.setPhimax(m_xmlHelper.getFloatAttribute(menuElement, "phimax"));
    data.setPhi(m_xmlHelper.getFloatAttribute(menuElement, "phi"));
    data.setRoiid(m_xmlHelper.getUIntAttribute(menuElement, "roiid"));

    ss<< m_xmlHelper.getAttribute(menuElement, "etacode");
    ss>> x;
    data.setEtacode( x );
    ss.clear();
    ss<< m_xmlHelper.getAttribute(menuElement, "phicode");
    ss>> x;
    data.setPhicode( x );
    //data.print("    ");
}

void MuctpiXMLParser::setTopoCell(boost::property_tree::ptree menuElement, MioctTopoCellGeometry &data)
{
   unsigned int x = 0;
   std::stringstream ss; ss  << std::hex ;
   data.setEtamin(m_xmlHelper.getFloatAttribute(menuElement, "etamin"));
   data.setEtamax(m_xmlHelper.getFloatAttribute(menuElement, "etamax"));
   data.setEta(m_xmlHelper.getFloatAttribute(menuElement, "eta"));

   data.setPhimin(m_xmlHelper.getFloatAttribute(menuElement, "phimin"));
   data.setPhimax(m_xmlHelper.getFloatAttribute(menuElement, "phimax"));
   data.setPhi(m_xmlHelper.getFloatAttribute(menuElement, "phi"));
   data.setIEta(m_xmlHelper.getIntAttribute(menuElement, "ieta"));
   data.setIPhi(m_xmlHelper.getIntAttribute(menuElement, "iphi"));

   ss<< m_xmlHelper.getAttribute(menuElement, "etacode");
   ss>> x;  ss.clear();
   data.setEtacode( x );
   ss<< m_xmlHelper.getAttribute(menuElement, "phicode");
   ss>> x;
   data.setPhicode( x );
   //data.print("      ");
}


