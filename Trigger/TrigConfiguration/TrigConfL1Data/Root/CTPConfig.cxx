/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/DiffStruct.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace TrigConf;

CTPConfig::CTPConfig() : L1DataBaseclass(),
                         m_ctpVersion( 0 ),
                         m_l1Version( 0 ),
                         m_PrescaleSetId( 0 ),
                         m_BunchGroupSetId( 0 ),
                         m_LoadCtpFiles( true ),
                         m_LoadCtpcoreFiles( true ),
                         m_LoadCtpinFiles( true ),
                         m_LoadCtpmonFiles( true ),
                         m_CTPFiles( nullptr )
{}

CTPConfig::~CTPConfig() {
   delete m_CTPFiles;
}



void
CTPConfig::setCTPVersion( unsigned int v ) { 
   m_ctpVersion = v; 
   menu().thresholdConfig().m_ctpVersion = v;
}

void
CTPConfig::setL1Version( unsigned int v ) {
   m_l1Version = v;
   menu().thresholdConfig().m_l1Version = v;
}

void
CTPConfig::clearPrescaleSets() { 
   for(int i=0; i<4; i++)
      m_PrescaleSets[i].reset();
}

void
CTPConfig::setPrescaleSet( const PrescaleSet& pss ) {
   if(pss.partition()>3) {
      cout << "Error: Partition " << pss.partition() << " of PrescaleSet " << pss.name() << " is larger than 3" << endl;
   } else {
      m_PrescaleSets[pss.partition()] = pss;
   }
}


bool
CTPConfig::equals(const CTPConfig* other, const std::string& filename) const {
   bool equals = true;
   DiffStruct* ds = compareTo(other);
   if(ds) {
      equals = false;
      std::ofstream xmlfile;
      xmlfile.open(filename.c_str());
      // file will be empty if there are no differences
      xmlfile << "<?xml version=\"1.0\" ?>" << std::endl;
      ds->writeXML(xmlfile);
      xmlfile.close();
      delete ds;
   }
   return equals;
}

DiffStruct*
CTPConfig::compareTo(const CTPConfig* o) const {
   DiffStruct * ds = new DiffStruct("LVL1Config");
   ds->name = name();
   ds->check("name", name(), o->name());

   ds->addSub( menu().compareTo( & o->menu()) );
   //    ds->addSub( getHLTChainList().compareTo( & o->getHLTChainList()) );

   if(ds->empty()) {
      delete ds; ds=0;
   }
   return ds;
}

void
CTPConfig::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "================================================================================" << endl;
      cout << indent << "CTP Configuration";
      if(smk()>0) cout << " (SMK = " << smk() << ")";
      cout << ": "; printNameIdV(indent);
      cout << indent << "CTP Version " << ctpVersion() << endl;
      cout << indent << "L1 Version " << l1Version() << endl;
      menu().print(indent, detail);
      prescaleSet().print(indent, detail);
      bunchGroupSet().print(indent, detail);
      random().print(indent, detail);
      cout << indent << "================================================================================" << endl;
   }
}


void
CTPConfig::writeXML(const std::string & filename, int indentWidth) const {
   std::ofstream xmlfile;
   xmlfile.open( filename );
   int indentLevel=1;
   xmlfile << "<?xml version=\"1.0\"?>" << endl
           << "<LVL1Config name=\"" << menu().name() << "\""
           << " ctpVersion=\"" << ctpVersion() << "\""
           << " l1Version=\"" << l1Version() << "\""
           << ">" << endl
           << "  <!--File is generated by TrigConfReadWrite-->" << endl
           << "  <!--No. L1 thresholds defined: " << menu().thresholdConfig().size() << "-->" << endl
           << "  <!--No. L1 items defined: "  << menu().size() << "-->" << endl;
   writeXML(xmlfile,indentLevel,indentWidth);
   xmlfile << "</LVL1Config>" << endl;
   xmlfile.close();
  
}


void
CTPConfig::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   menu().writeXMLItems(xmlfile, indentLevel, indentWidth);
   for(const PrescaleSet& pss: m_PrescaleSets) {
      if( ! pss.isNull()) {
         pss.writeXML(xmlfile, indentLevel, indentWidth);
      }
   }
   menu().writeXMLThresholds(xmlfile, indentLevel, indentWidth);
   menu().writeXMLMonCounters(xmlfile, indentLevel, indentWidth);
   random().writeXML(xmlfile, indentLevel, indentWidth);
   bunchGroupSet().writeXML(xmlfile, indentLevel, indentWidth);
   prescaledClock().writeXML(xmlfile, indentLevel, indentWidth);
   menu().caloInfo().writeXML(xmlfile, indentLevel, indentWidth);
   muCTPi().writeXML(xmlfile,indentLevel,indentWidth);
}

