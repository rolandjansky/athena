/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/CaloInfo.h"
#include <iostream>

using namespace std;
using namespace TrigConf;

// Internal variable
static TrigConf::CaloInfo fgCaloInfo;
static const TrigConf::CaloInfo* fgCaloInfoPointer=0;

const int TriggerThresholdValue::fgCaloClusterOff   = 0xff; // 255
const int TriggerThresholdValue::fgCaloJetOff       = 0x3ff; // 1023
const int TriggerThresholdValue::fgCaloEtSumOff     = 0x7fff; // 16383
const int TriggerThresholdValue::fgCaloEtMissOff    = 0x7fff; // 2895
const int TriggerThresholdValue::fgCaloJetEtOff     = 0x33e6; // 13286
const int TriggerThresholdValue::fgCaloIsolationOff = 0x3f; // 63



// Static stuffs
void
TriggerThresholdValue::setCaloInfo(const CaloInfo& ci) {
   fgCaloInfo = ci;
   fgCaloInfoPointer = &fgCaloInfo;
}

const CaloInfo &
TriggerThresholdValue::caloInfo() const {
   return fgCaloInfo;
}


float
TrigConf::TriggerThresholdValue::caloGlobalScale() {
   if (fgCaloInfoPointer)
      return fgCaloInfoPointer->globalScale();
   return 1.0;
}

float
TrigConf::TriggerThresholdValue::caloGlobalScaleGeVToCount() {
   float x = caloGlobalScaleCountToGeV();
   if (x == 0)
      return 1.0;
   else
      return 1.0/x;
}

float
TrigConf::TriggerThresholdValue::caloGlobalScaleCountToGeV() {
   return caloGlobalScale();
}

// constructors
TrigConf::TriggerThresholdValue::TriggerThresholdValue( )
   : L1DataBaseclass()
   , m_Type("")
   , m_Ptcut(0.0)
   , m_Priority(0.0)
   , m_Window(0)
   , m_WindowSize( JetWindowSize::NONE )
   , m_PhiMin(0)
   , m_PhiMax(64)
   , m_EtaMin(-49)
   , m_EtaMax(49)
{}

int
TrigConf::TriggerThresholdValue::thresholdValueCount() const {
   return (int) ptcut(); // overriden in Cluster/JetThresholdValue
}

bool
TrigConf::TriggerThresholdValue::contains(float eta, float phi) const{
   return ( eta >= etamin() && eta <= etamax() &&
            phi >= phimin() && phi <= phimax());
}

void
TrigConf::TriggerThresholdValue::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "-----------------------" << endl; 
   cout << indent << "TriggerThresholdValue: " << endl; 
   cout << indent << "\t id:       " << id() << endl; 
   cout << indent << "\t Name:     " << name() << endl;
   cout << indent << "\t Version:  " << version() << endl;
   cout << indent << "\t Priority: " << m_Priority << endl;
   cout << indent << "\t Type:     " << m_Type << endl;
   cout << indent << "\t ptcut:    " << m_Ptcut << endl;
}

void
TrigConf::TriggerThresholdValue::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThresholdValue " << m_Type << " NOT IMPLEMENTED/>"
      << endl;
}


