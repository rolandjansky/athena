/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"

#include <stdexcept>
#include <iostream>
#include <sys/types.h>
#include <stdio.h>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace TrigConf;

ThresholdConfig::ThresholdConfig() :
   L1DataBaseclass(),
   m_ctpVersion( 0 ),
   m_l1Version( 0 )
{
   // create a vector for each trigger type registered in L1DataDef.cxx 
   // size of vector is 0 and grows while being filled
   for( L1DataDef::TypeConfigMap_t::value_type tc: L1DataDef::typeConfigs() )
      m_thresholdVectors.insert( thrVecMap_t::value_type(tc.first , thrVec_t(0) ) );
}


const vector<TriggerThreshold*>& 
TrigConf::ThresholdConfig::getThresholdVector(L1DataDef::TriggerType type) const {
   thrVecMap_t::const_iterator res = m_thresholdVectors.find(type);
   if(res == m_thresholdVectors.end()) {
      cerr << "Unknown triggertype '" << L1DataDef::typeAsString(type) << "' in ThresholdConfig::getThresholdVector encountered" << endl;
      throw std::runtime_error("Unknown triggertype in ThresholdConfig::getThresholdVector encountered" );
   }
   return res->second;
}

std::vector<TrigConf::TriggerThreshold*>& 
TrigConf::ThresholdConfig::thresholdVector(L1DataDef::TriggerType type) {
   return const_cast<thrVec_t&>( getThresholdVector(type) );
}


bool
TrigConf::ThresholdConfig::insertInPosition(std::vector<TriggerThreshold*>& thrVec, TriggerThreshold* thr, unsigned int pos) {
   thr->print();

   // extend if necessary (filled with 0)
   if(pos >= thrVec.size()) thrVec.resize(pos+1, 0);

   if(thrVec[pos] != 0) {
      cerr << "ThresholdConfig::insertInPosition: position " << pos
           << " already filled for type " << thr->type() << " [" << thr->ttype() << "]" << ", abort!!" << endl;
      cerr << endl << "Existing threshold" << endl;
      thrVec[pos]->print("",5);
      cerr << endl << "New threshold" << endl;
      thr->print("",5);
      throw runtime_error("ThresholdConfig::insertInPosition position already filled" );
   }
   thrVec[pos] = thr;

   return true;
}


bool
TrigConf::ThresholdConfig::addTriggerThreshold(TriggerThreshold* thr) {

   // put the threshold into the vector of all thresholds
   m_TriggerThresholdVector.push_back(thr);
   
   // put the threshold to the correct vector
   L1DataDef::TriggerType ttype = thr->ttype();

   vector<TriggerThreshold*>& thrVec = thresholdVector(ttype);

   // check if maximum is exceeded
   unsigned int max_thr = L1DataDef::typeConfig(ttype).max;
   if(thr->mapping()>0 && (unsigned int)thr->mapping() >= max_thr) {
      cerr << "ThresholdConfig::addTriggerThreshold: threshold mapping " << thr->mapping()
           << " exceeds maximum for type '" << thr->type() << "' , abort!" << endl;
      throw runtime_error( "ThresholdConfig::addTriggerThreshold: threshold mapping exceeds limit" );
   }
      
   thr->setThresholdNumber(thr->mapping());
   insertInPosition(thrVec, thr, thr->mapping() );


   // BPTX threshold is type NIM but gets inserted as BPTX as well (in run 2 not anymore) - no easy way to make this backward compatible
//    if(thr->name().find("BPTX")!=string::npos) {
//       string::size_type pos = thr->name().find_first_of("0123456789");
//       int mapping = boost::lexical_cast<int,string>(thr->name().substr(pos));
//       insertInPosition( thresholdVector(L1DataDef::BPTX), thr, mapping );
//    }

   // for backward compatibility a copy of all EM and TAU thresholds
   // is safed in the cluster threshold vector

   if(l1Version()==0) {
      if(ttype==L1DataDef::EM) {
         insertInPosition( m_ClusterThresholdVector, thr, thr->mapping() );
      }
      if(ttype==L1DataDef::TAU) {
         unsigned int max_em = L1DataDef::typeConfig(L1DataDef::EM).max;
         unsigned int max_tau = 0; //L1DataDef::typeConfig(L1DataDef::TAU).max;
         unsigned int pos = max_em + max_tau - thr->mapping() - 1; 
         insertInPosition( m_ClusterThresholdVector, thr, pos );
      }
   }
   return true;
}



string
TrigConf::ThresholdConfig::getClusterThresholdType ( int thresholdnumber ) const {
   TriggerThreshold* thr = getThresholdVector(L1DataDef::EM)[thresholdnumber];
   return thr?thr->type():"EM";
}


/********************************
 * cluster isolation and veto
 ********************************/
float TrigConf::ThresholdConfig::getClusterEmIsolation ( int eta, int phi, int thresholdnumber ) const {
   ClusterThresholdValue* ctv = dynamic_cast<ClusterThresholdValue*>
      (getThresholdVector(L1DataDef::EM)[thresholdnumber]->triggerThresholdValue(eta,phi));
   return ctv?ctv->emIsolation():0;
}

float TrigConf::ThresholdConfig::getClusterHadIsolation ( int eta, int phi, int thresholdnumber ) const {
   ClusterThresholdValue* ctv = dynamic_cast<ClusterThresholdValue*>
      (getThresholdVector(L1DataDef::EM)[thresholdnumber]->triggerThresholdValue(eta,phi));
   return ctv?ctv->hadIsolation():0;
}

float TrigConf::ThresholdConfig::getClusterHadVeto ( int eta, int phi, int thresholdnumber ) const {
   ClusterThresholdValue* ctv = dynamic_cast<ClusterThresholdValue*>
      (getThresholdVector(L1DataDef::EM)[thresholdnumber]->triggerThresholdValue(eta,phi));
   return ctv?ctv->hadVeto():0;
}


/****************
 * get window
 ****************/
int TrigConf::ThresholdConfig::getWindow(TrigConf::L1DataDef::TriggerType type, int eta, int phi, int thresholdnumber) const {
   return getThresholdVector(type)[thresholdnumber]->triggerThresholdValue(eta,phi)->window();
}

int TrigConf::ThresholdConfig::getJetWindow( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::JET, eta, phi, thresholdnumber);
}

int TrigConf::ThresholdConfig::getJfWindow( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::JF, eta, phi, thresholdnumber);
}

int TrigConf::ThresholdConfig::getJbWindow( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::JB, eta, phi, thresholdnumber);
}



/****************
 * get ptcut()
 ****************/
float TrigConf::ThresholdConfig::getThreshold(TrigConf::L1DataDef::TriggerType type, int eta, int phi, int thresholdnumber) const {
   return getThresholdVector(type)[thresholdnumber]->triggerThresholdValue(eta,phi)->ptcut();
}

float TrigConf::ThresholdConfig::getClusterThreshold(int eta, int phi, int thresholdnumber) const {
   return getThreshold(TrigConf::L1DataDef::EM, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getMuonThreshold( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::MUON, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getJetEtThreshold( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::JE, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getMissEtThreshold( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::XE, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getMissEtSigThreshold( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::XS, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getJetThreshold( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::JET, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getJbThreshold(int eta, int phi, int thresholdnumber) const {
   return getThreshold(TrigConf::L1DataDef::JB, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getJfThreshold  ( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::JF, eta, phi, thresholdnumber);
}

float TrigConf::ThresholdConfig::getTotEtThreshold( int eta, int phi, int thresholdnumber ) const {
   return getThreshold(TrigConf::L1DataDef::TE, eta, phi, thresholdnumber);
}



void
TrigConf::ThresholdConfig::attributeThresholdNumbers() {
   // set the thresholdnumber for each threshold

   // Run 2:
   // this has been changed, the thresholdnumbers now correspond to the position in the vector (the index)
   
   // Run 1:
   // EM/TAU thresholds were an exception:

   // The EM and TAU vector are related, 
   // 
   // the total number of EM and TAU thresholds is less than EM.max, the maximum number of
   // tau thresholds is TAU.max
   // 
   // the arrangement is such that the EM thresholds above 7 are after the TAU thresholds in reverse order

   // for instance if there would be 6 TAU thresholds and 10 EM thresholds
   // EM-TAU vector  : [EM0,EM1,...,EM7,TAU0,TAU1,...,TAU5,EM9,EM8]
   // thresholdNumber: [  0,  1,...,  7,   8,   9,...,  13, 14, 15]

   
   for(TriggerThreshold* p: thresholdVector(L1DataDef::EM)) {
      if(p==0) continue;
      const unsigned int max_em  = L1DataDef::typeConfig(L1DataDef::EM).max;

      if ( p->mapping()>=8 )
         p->setThresholdNumber(max_em - 1 - (p->mapping()-8) );
   }

   for(TrigConf::TriggerThreshold* p: thresholdVector(L1DataDef::TAU)) {
      if(p==0) continue;
      if( p->ttype()==L1DataDef::TAU )
         p->setThresholdNumber(p->mapping() + 8);
   }

}



TrigConf::TriggerThreshold*
TrigConf::ThresholdConfig::findTriggerThreshold(unsigned int id) {
   for(TrigConf::TriggerThreshold *thr: m_TriggerThresholdVector)
      if(thr->id()==id) return thr;
   return 0;
}


void printVectorSummary(const TrigConf::ThresholdConfig::thrVec_t& vec,
                        const string& name, const string& indent, unsigned int detail) {
   cout << indent << "=========================================" << endl
        << indent << " The " << name << ": " << vec.size() << " elements" << endl
        << indent << "=========================================" << endl;
   for(TrigConf::TriggerThreshold *thr: vec)
      if(thr) thr->print(indent + " ", detail);
      else cout << indent << " " << 0 << endl;
}

void
TrigConf::ThresholdConfig::print(const string& indent, unsigned int detail) const {
   if(detail>=2) {
      cout << indent << "Threshold configuration: " << getThresholdVector().size() << " thresholds" << endl;
      for(L1DataDef::TriggerType tt : L1DataDef::types()) {
         cout << indent << "   " << L1DataDef::typeConfig(tt).name << " thresholds: " 
              << getThresholdVector(tt).size() << " thresholds" << endl;
      }
      cout << "Threshold configuration: " << getThresholdVector().size() << " thresholds" << endl;
   }
   if(detail>=3) {
      cout << indent << "=========================================" << endl
           << indent << "== ThresholdConfig for MT_id = " << lvl1MasterTableId()  << endl
           << indent << "=========================================" << endl;
      printVectorSummary(getThresholdVector(L1DataDef::EM),   "EMThresholdVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::TAU),  "TauThresholdVector", indent, detail);
      printVectorSummary(m_ClusterThresholdVector,            "ClusterThresholdVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::JET),  "JetThresholdVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::JF),   "JFThresholdVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::JB),   "JBThresholdVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::TE),   "TotEtVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::JE),   "JetEtVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::XE),   "MissEtVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::MUON), "MuonThresholdVector", indent, detail);
      printVectorSummary(getThresholdVector(L1DataDef::NIM),  "NimThresholdVector", indent, detail);
   }
   m_CaloInfo.print(indent, detail);
}

void
TrigConf::ThresholdConfig::printTriggerThresholdVector(const string& indent) const {  
   printVectorSummary(getThresholdVector(),"complete threshold vector", indent, 5);
}



void
TrigConf::ThresholdConfig::printTtvSummary(const TrigConf::ThresholdConfig::thrVec_t& vec, const string& name) const {
   cout << "#=========================================" << endl
        << "# " << name << ":" << endl
        << "#=========================================" << endl;
   for(TriggerThreshold* thr: vec) printTtvMap(thr);
}

void
TrigConf::ThresholdConfig::printThresholdValueMap() const {
   printTtvSummary(getThresholdVector(L1DataDef::EM),   "EM threshold values");
   printTtvSummary(getThresholdVector(L1DataDef::TAU),  "Tau threshold values");
   printTtvSummary(m_ClusterThresholdVector,            "Cluster threshold values (EM + Tau)");
   printTtvSummary(getThresholdVector(L1DataDef::JET),  "JetThreshold values");
   printTtvSummary(getThresholdVector(L1DataDef::FJET), "FJetThreshold values");
   printTtvSummary(getThresholdVector(L1DataDef::TE),   "TotEt values");
   printTtvSummary(getThresholdVector(L1DataDef::JE),   "JetEt values");
   printTtvSummary(getThresholdVector(L1DataDef::XE),   "MissEt values");
   printTtvSummary(getThresholdVector(L1DataDef::MUON), "MuonThreshold values");
   printTtvSummary(getThresholdVector(L1DataDef::NIM),  "NimThreshold values");
}


void TrigConf::ThresholdConfig::printTtvMap(const TriggerThreshold* thr) const {
   char line[1000];

   cout << "#---------------------------------------------------------" << endl
        << "# TTV(type,name)         eta   phi :    pt  window emIsol hadIsol hadVeto" << endl
        << "# Number of theshold values: " << thr->thresholdValueVector().size() << endl
        << "#---------------------------------------------------------" << endl;
   for (int ieta = -49; ieta <= 49; ++ieta) {
      for (int iphi = 0; iphi < 64; ++iphi) {
         TriggerThresholdValue* ttv = thr->triggerThresholdValue(ieta, iphi);
         float emiso(-1), hadiso(-1), hadveto(-1);
         const ClusterThresholdValue* ctv = dynamic_cast<const ClusterThresholdValue*>(ttv);
         if(ctv) {
            emiso = ctv->emIsolation();
            hadiso = ctv->hadIsolation();
            hadveto = ctv->hadVeto();
         }
         sprintf(line, "TTV(%-4s,%-10s) eta[%2d] phi[%2d]: %3f %6i %6f %6f %6f", 
                 thr->type().c_str(), thr->name().c_str(), ieta, iphi, 
                 ttv->ptcut(), ttv->window(), emiso, hadiso, hadveto);
         cout << line << endl;
      }
   }
   return;
}

vector<TrigConf::TriggerThreshold*>
TrigConf::ThresholdConfig::getNimThresholdVectorByType(const string& type) const {
   vector<TriggerThreshold*> v;
   for( TriggerThreshold* thr: getThresholdVector(L1DataDef::NIM) )
      if ( thr->type() == type ) v.push_back(thr);
   return v;
}

void
TrigConf::ThresholdConfig::clear() {

   for(TriggerThreshold* thr: m_TriggerThresholdVector) delete thr;
   m_TriggerThresholdVector.clear();

   for(thrVecMap_t::value_type & p: m_thresholdVectors)
      p.second.clear();

   m_CaloInfo.clear();
}



