/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/MuonThresholdValue.h"
#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/XsThresholdValue.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/NimThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CaloInfo.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>

using namespace std;

TrigConf::TriggerThreshold::TriggerThreshold() :
   L1DataBaseclass()
{}

TrigConf::TriggerThreshold::TriggerThreshold(const TriggerThreshold& thr) :
   L1DataBaseclass(thr),
   m_type(thr.m_type), 
   m_SType(thr.m_SType), 
   m_Active(thr.m_Active), 
   m_Mapping(thr.m_Mapping), 
   m_CableName(thr.m_CableName), 
   m_CableCtpin(thr.m_CableCtpin), 
   m_CableConnector(thr.m_CableConnector), 
   m_CableStart(thr.m_CableStart), 
   m_CableEnd(thr.m_CableEnd),
   m_Clock(thr.m_Clock),
   m_ThresholdNumber(thr.m_ThresholdNumber),
   m_ZBSeedingThresholdName(thr.m_ZBSeedingThresholdName),
   m_ZBSeedingThresholdMulti(thr.m_ZBSeedingThresholdMulti),
   m_BCDelay(thr.m_BCDelay),
   m_Bitnum(thr.m_Bitnum),
   m_Input(thr.m_Input)
{
   // deep copy of the threshold values
   for(TriggerThresholdValue *thrV : thr.thresholdValueVector())
      addThresholdValue( thrV->createCopy() );
}
  
TrigConf::TriggerThreshold::~TriggerThreshold() {
   clearThresholdValues();
}


void
TrigConf::TriggerThreshold::setType(L1DataDef::TriggerType type ) {
   m_type = type;
   m_SType = L1DataDef::typeAsString(m_type);
}

void
TrigConf::TriggerThreshold::setType(const std::string& type) {
   m_SType = type;
   m_type = L1DataDef::stringAsType(m_SType);
}


void
TrigConf::TriggerThreshold::setInput(const std::string& input) {
   if(input!="ctpin" && input!="ctpcore")
      throw runtime_error( "Threshold input must be either 'ctpin' or 'ctpcore' but value was '"+input+"'" );
   m_Input = input;
}


bool
TrigConf::TriggerThreshold::isInternal() const {
   return L1DataDef::typeConfig(ttype()).internal;
}

TrigConf::TriggerThresholdValue*
TrigConf::TriggerThreshold::triggerThresholdValue(int eta_i,int phi_i) const {

   TriggerThresholdValue* ttv=0;

   float eta = float(eta_i)+0.5;
   float phi = float(phi_i)+0.5;

   for(TriggerThresholdValue* thrV : m_TriggerThresholdValueVector) {

      if( thrV->contains(eta, phi) ) {

         if( ttv != 0 && ttv->priority()==thrV->priority() ) {
            cerr << "TriggerThreshold '" << name() << "' has overlapping values of same priority at eta = " 
                 << eta << ", phi = " << phi << endl;
            throw runtime_error( "Overlapping thresholdValues of the same priority" );
         }
         if( ttv==0 || ttv->priority()<thrV->priority() ) {
            ttv = thrV;
         }
      }
   }
   if(ttv==0) {
      cerr << "TriggerThreshold::triggerThresholdValue: did not find threshold value at eta = " 
           << eta << ", phi = " << phi << " for threshold " << name() << endl;
      throw runtime_error( "Did not find threshold value" );
   }
   return ttv;
}
  

const
std::vector<TrigConf::TriggerThresholdValue*>&
TrigConf::TriggerThreshold::thresholdValueVector() const {
   return m_TriggerThresholdValueVector;    
}

void
TrigConf::TriggerThreshold::addThresholdValue(TriggerThresholdValue* value) {
   m_TriggerThresholdValueVector.push_back(value);
}


TrigConf::TriggerThresholdValue*
TrigConf::TriggerThreshold::addThresholdValue(const std::string& type) {
   TriggerThresholdValue *thrV = createThresholdValue(type);
   addThresholdValue(thrV);
   return thrV;
}


TrigConf::TriggerThresholdValue*
TrigConf::TriggerThreshold::createThresholdValue(const std::string& type) {
   TriggerThresholdValue* ttv = 0;
   if (type == L1DataDef::muonType()) {
      ttv = new MuonThresholdValue(); 
   } else if (type == L1DataDef::emType()  ||
              type == L1DataDef::tauType()) {
      ttv = new ClusterThresholdValue();
   } else if (type == L1DataDef::jetType() ||
              type == L1DataDef::jbType()  ||
              type == L1DataDef::jfType()) {
      ttv = new JetThresholdValue();
   } else if (type == L1DataDef::xeType()  ||
              type == L1DataDef::jeType()  ||
              type == L1DataDef::teType()) {
      ttv = new EtThresholdValue();
   } else if ( type== L1DataDef::xsType() ) {
      ttv = new XsThresholdValue();
   } else if (type == L1DataDef::nimType()    || type == L1DataDef::mbtsType()   || type == L1DataDef::mbtssiType() ||
              type == L1DataDef::bcmType()    || type == L1DataDef::bcmcmbType() || type == L1DataDef::lucidType()  ||
              type == L1DataDef::calreqType() || type == L1DataDef::zdcType()    || type == L1DataDef::trtType() ||
              type == L1DataDef::bptxType() ) {
      ttv = new NimThresholdValue();
   } else {
      cout << "TriggerThreshold::createThresholdValue:      ERROR Unknown trigger value type: " << type << endl;
      throw std::runtime_error("TriggerThreshold::createThresholdValue: Unknown trigger value type.");
   }
   return ttv;
}


void TrigConf::TriggerThreshold::clearThresholdValues() {
   for(TriggerThresholdValue* thrV : m_TriggerThresholdValueVector)
      delete thrV;
   m_TriggerThresholdValueVector.clear();
}

void TrigConf::TriggerThreshold::print(const std::string& indent, unsigned int detail) const {
   if(detail==2) {
      cout << indent << "TriggerThreshold " << name() << endl;
   }
   if(detail>=3) {
      if(detail>=4)
         cout << indent << "=================================="                      << endl; 
      cout << indent << "TriggerThreshold "; printNameIdV(indent);
      if(detail>=4) {
         cout << indent << "        Type:             " << type()            << endl;
         if(ttype()==L1DataDef::ZB) {
            cout << indent << "        seed:             " << m_ZBSeedingThresholdName      << endl;
            cout << indent << "        seed multi:       " << m_ZBSeedingThresholdMulti     << endl;
            cout << indent << "        BC delay:         " << m_BCDelay       << endl;
         }
         cout << indent << "        active:           " << m_Active          << endl;
         cout << indent << "        mapping           " << m_Mapping         << endl;
         cout << indent << "        number of values: " << numberofValues()  << endl;
         cout << indent << "        cable name:       " << m_CableName       << endl;
         cout << indent << "        cable ctpin:      " << m_CableCtpin      << endl;
         cout << indent << "        cable connector:  " << m_CableConnector  << endl;
         cout << indent << "        cable start:      " << m_CableStart      << endl;
         cout << indent << "        cable end:        " << m_CableEnd        << endl;
         cout << indent << "        threshold number: " << m_ThresholdNumber << endl;
         for(unsigned int i=0; i < m_TriggerThresholdValueVector.size();i++)
            m_TriggerThresholdValueVector[i]->print(indent + "  ");
      }
   }
}


namespace {
   
   bool compThrValues(TrigConf::TriggerThresholdValue *x, TrigConf::TriggerThresholdValue *y) { // strict weak ordering: x and y are equivalent if compMon(x,y) and compMon(y,x) are false
      if(x->priority() != y->priority())
         return x->priority() < y->priority();
      if(x->etamin() != y->etamin())
         return x->etamin() < y->etamin();
      return x->name() < y->name();
   }

}


void
TrigConf::TriggerThreshold::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThreshold active=\"" << m_Active
      << "\" bitnum=\"" << bitnum()
      << "\" id=\"" << id()
      << "\" mapping=\"" << m_Mapping
      << "\" name=\"" << name()
      << "\" type=\"" << type()
      << "\" input=\"" << input();

   if(ttype()==L1DataDef::ZB)
      xmlfile << "\" seed=\"" << m_ZBSeedingThresholdName
              << "\" seed_multi=\"" << m_ZBSeedingThresholdMulti
              << "\" bcdelay=\"" << m_BCDelay;

   xmlfile << "\" version=\"" << version() <<"\">" << endl;

   auto sortedThresholdValues = m_TriggerThresholdValueVector;
   sort(sortedThresholdValues.begin(),sortedThresholdValues.end(),compThrValues);

   for(TriggerThresholdValue* thrV : sortedThresholdValues)
      thrV->writeXML(xmlfile, indentLevel+1, indentWidth);
      
   if(m_CableName!="") {
      if(input()=="ctpin") {
         indent(xmlfile, indentLevel+1, indentWidth)
            << "<Cable connector=\"" << m_CableConnector << "\" input=\"" << m_CableCtpin << "\" name=\"" << m_CableName << "\">" << endl;
         indent(xmlfile, indentLevel+2, indentWidth)
            << "<Signal range_begin=\"" << m_CableStart << "\" range_end=\"" << m_CableEnd <<"\"/>" << endl;
      } else {
         indent(xmlfile, indentLevel+1, indentWidth)
            << "<Cable connector=\"" << m_CableConnector << "\" input=\"" << m_CableCtpin << "\" name=\"" << m_CableName << "\">" << endl;
         indent(xmlfile, indentLevel+2, indentWidth)
            << "<Signal range_begin=\"" << m_CableStart << "\" range_end=\"" << m_CableEnd <<"\" clock=\"" << m_Clock << "\"/>" << endl;
      }
      indent(xmlfile, indentLevel+1, indentWidth) << "</Cable>" << endl;
   }

   indent(xmlfile, indentLevel, indentWidth) 
      << "</TriggerThreshold>" << endl;
}

