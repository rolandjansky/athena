/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/TriggerFeature.h"
#include "PathResolver/PathResolver.h"
#include "algorithm"

#include "fstream"
#include "sstream"
#include "TSystem.h"
#include "TVector2.h"

using namespace Trig;

//**********************************************************************

bool Trig::sortEmulationJetByPt(Trig::TrigBtagEmulationJet& a,Trig::TrigBtagEmulationJet& b) { return a.pt > b.pt; }

//**********************************************************************

TriggerFeature::TriggerFeature(int type, std::string name) : m_type(type), m_name(name) {}
TriggerFeature::TriggerFeature(const TriggerFeature& other) : m_type(other.m_type), m_name(other.m_name) {}
TriggerFeature::~TriggerFeature() {}

std::string TriggerFeature::name() const { return m_name; }

TriggerFeatureBtag::TriggerFeatureBtag(std::string name,float weight)
  : TriggerFeature(TriggerFeature::THRESHOLD,name), m_weight(weight) {}
TriggerFeatureBtag::TriggerFeatureBtag(const TriggerFeatureBtag& other)
  : TriggerFeature(other), m_weight(other.m_weight) {}
TriggerFeatureBtag::~TriggerFeatureBtag() {}

float TriggerFeatureBtag::getCut() const {return m_weight;}

bool TriggerFeatureBtag::isPassed() {return true;}
bool TriggerFeatureBtag::evaluateJet(struct TrigBtagEmulationJet* jet) { return jet->weights[m_name.c_str()] >= m_weight; }
void TriggerFeatureBtag::clear() {}

// ***

TriggerFeatureAntiBtag::TriggerFeatureAntiBtag(std::string name,float weight)
  : TriggerFeatureBtag(name,weight) {}
TriggerFeatureAntiBtag::TriggerFeatureAntiBtag(const TriggerFeatureAntiBtag& other)
  : TriggerFeatureBtag(other) {}
TriggerFeatureAntiBtag::~TriggerFeatureAntiBtag() {}

bool TriggerFeatureAntiBtag::evaluateJet(struct TrigBtagEmulationJet* jet) { return !TriggerFeatureBtag::evaluateJet(jet); } 

// ***

TriggerFeatureHt::TriggerFeatureHt(std::string triggerLevel,std::string name, float ht)
  : TriggerFeature(TriggerFeature::SELECTION,name), m_trigLevel(triggerLevel), m_min_ht(ht), m_count_ht(0),
    m_cut_pt(0), m_cut_min_eta(0), m_cut_max_eta(9.0) {}
TriggerFeatureHt::TriggerFeatureHt(const TriggerFeatureHt& other)
  : TriggerFeature(other), m_trigLevel(other.m_trigLevel), m_min_ht(other.m_min_ht), m_count_ht(other.m_count_ht), 
    m_cut_pt(other.m_cut_pt), m_cut_min_eta(other.m_cut_min_eta), m_cut_max_eta(other.m_cut_max_eta) {}
TriggerFeatureHt::~TriggerFeatureHt() {}

float TriggerFeatureHt::getCut() const {return m_min_ht;}
void TriggerFeatureHt::setCuts(float min_pt, float min_eta, float max_eta)
{m_cut_pt = min_pt; m_cut_min_eta = min_eta; m_cut_max_eta = max_eta;}

bool TriggerFeatureHt::satisfyCuts(struct TrigBtagEmulationJet* jet) {
  if (jet->pt <= m_cut_pt) return false;
  if (fabs(jet->eta) < m_cut_min_eta) return false;
  if (fabs(jet->eta) > m_cut_max_eta) return false;
  return true;
}

bool TriggerFeatureHt::isPassed() {
  if (m_trigLevel == "L1" ) return isPassed_L1();
  if (m_trigLevel == "HLT") return isPassed_HLT();
  return false;
}

bool TriggerFeatureHt::evaluateJet(struct TrigBtagEmulationJet* jet) {
  if (m_trigLevel == "L1" ) return evaluateJet_L1(jet);
  if (m_trigLevel == "HLT") return evaluateJet_HLT(jet);
  return false;
}

bool TriggerFeatureHt::isPassed_L1()  { return m_count_ht > m_min_ht; }
bool TriggerFeatureHt::isPassed_HLT() { return m_count_ht >= m_min_ht; }
bool TriggerFeatureHt::evaluateJet_L1(struct TrigBtagEmulationJet* jet) {
  if ( satisfyCuts(jet) )
    m_count_ht+=jet->pt;
  return true;
}
bool TriggerFeatureHt::evaluateJet_HLT(struct TrigBtagEmulationJet* jet) {
  if ( jet->pt < 30000 ) return true;
  if ( fabs(jet->eta) > 3.2 ) return true;
  m_count_ht+=jet->pt; 
  return true;
}

void TriggerFeatureHt::clear() { m_count_ht = 0; }

// ***

TriggerFeatureHtTop::TriggerFeatureHtTop(std::string triggerLevel,std::string name, float ht, unsigned int topEt)
  : TriggerFeatureHt(triggerLevel,name,ht),
    m_topEt(topEt) {}
TriggerFeatureHtTop::TriggerFeatureHtTop(const TriggerFeatureHtTop& other)
  : TriggerFeatureHt(other),
    m_piorityQueue(other.m_piorityQueue.begin(),other.m_piorityQueue.end()),
    m_topEt(other.m_topEt) {}
TriggerFeatureHtTop::~TriggerFeatureHtTop() {}

void TriggerFeatureHtTop::clear() {
  TriggerFeatureHt::clear();
  m_piorityQueue.clear();
}
bool TriggerFeatureHtTop::evaluateJet_L1(struct TrigBtagEmulationJet* jet) {
  if ( fabs(jet->eta) > 3.1) return true;
  m_piorityQueue.push_back( *jet );
  calculateHT_L1();
  return true;
}

void TriggerFeatureHtTop::calculateHT_L1() {
  sort(m_piorityQueue.begin(),m_piorityQueue.end(),Trig::sortEmulationJetByPt);
  while ( m_topEt < m_piorityQueue.size() )
    m_piorityQueue.pop_back();

  m_count_ht = 0;
  for (unsigned int i=0; i<m_piorityQueue.size();i++)
      if ( TriggerFeatureHt::satisfyCuts(&m_piorityQueue.at(i)) ) 
	m_count_ht += m_piorityQueue.at(i).pt;
}

// ***

TriggerFeatureInvm::TriggerFeatureInvm(std::string triggerLevel,std::string name,float min_invm) 
  : TriggerFeature(TriggerFeature::SELECTION,name), 
    m_trigLevel(triggerLevel), 
    m_min_invm(min_invm),
    m_count_invm(0),
    m_cut_pt(0),
    m_cut_min_eta(0),
    m_cut_max_eta(4.9) 
{ this->initLUTs(); }
TriggerFeatureInvm::TriggerFeatureInvm(const TriggerFeatureInvm& other)
  : TriggerFeature(other),
    m_trigLevel(other.m_trigLevel),
    m_min_invm(other.m_min_invm),
    m_count_invm(other.m_count_invm),
    m_cut_pt(other.m_cut_pt),
    m_cut_min_eta(other.m_cut_min_eta),
    m_cut_max_eta(other.m_cut_max_eta),
    m_jetCollection_HLT(other.m_jetCollection_HLT.begin(),other.m_jetCollection_HLT.end()),
    m_priorityQueue_20(other.m_priorityQueue_20.begin(),other.m_priorityQueue_20.end()),
    m_priorityQueue_30(other.m_priorityQueue_30.begin(),other.m_priorityQueue_30.end()) 
{ this->initLUTs(); }
TriggerFeatureInvm::~TriggerFeatureInvm() {}

float TriggerFeatureInvm::getCut() const {return m_min_invm;}

void TriggerFeatureInvm::initLUTs() {
  const std::string nameFile_LUTcosh = PathResolverFindDataFile( "TrigBtagEmulationTool/LUT_Hyperbolic.txt" );

  std::ifstream fileLUTcosh; fileLUTcosh.open(nameFile_LUTcosh.c_str());
  if ( !fileLUTcosh ) return;
  while ( !fileLUTcosh.eof() )
    {
      std::string line;
      int key;
      double value;

      getline (fileLUTcosh,line);
      if (line.empty()) continue;
      std::istringstream is_line(line);
      is_line >> key >> value;
      m_LUTcosh[key] = value;
    }
  fileLUTcosh.close();

  // *** cos ( Dphi )
  const std::string nameFile_LUTcos = PathResolverFindDataFile( "TrigBtagEmulationTool/LUT_Trigo.txt" );

  std::ifstream fileLUTcos; fileLUTcos.open(nameFile_LUTcos.c_str());
  if ( !fileLUTcos ) return;
  while ( !fileLUTcos.eof() )
    {
      std::string line;
      int key;
      double value;

      getline (fileLUTcos,line);
      if (line.empty()) continue;
      std::istringstream is_line(line);
      is_line >> key >> value;
      m_LUTcos[key] = value;
    }
  fileLUTcos.close();
}

double TriggerFeatureInvm::cosh_LUT(double value) const {
  int argument = int( value * 10 + 0.1 );
  if (m_LUTcosh.find(argument) != m_LUTcosh.end())
    return m_LUTcosh.at( argument );
  return 1;
}
double TriggerFeatureInvm::cos_LUT(double value) const {
  int argument = int( value * 1e2 );
  if (m_LUTcos.find(argument) != m_LUTcos.end())
    return m_LUTcos.at( argument );
  return 1.0;
}

bool TriggerFeatureInvm::isPassed() { return m_count_invm >= m_min_invm; }
bool TriggerFeatureInvm::evaluateJet(struct TrigBtagEmulationJet* jet) {
  if (m_trigLevel == "L1") return evaluateJet_L1( jet );
  if (m_trigLevel == "HLT") return evaluateJet_HLT( jet );
  return false;
}

void TriggerFeatureInvm::clear() {
  m_count_invm = 0;
  m_jetCollection_HLT.clear();
  m_priorityQueue_20.clear();
  m_priorityQueue_30.clear();
}

void TriggerFeatureInvm::setCuts(float min_pt,float min_eta,float max_eta) 
{ m_cut_pt = min_pt; m_cut_min_eta = min_eta; m_cut_max_eta = max_eta; }
bool TriggerFeatureInvm::satisfyCuts(struct TrigBtagEmulationJet* jet) {
  if (jet->pt <= m_cut_pt) return false;
  if (fabs(jet->eta) < m_cut_min_eta) return false;
  if (fabs(jet->eta) > m_cut_max_eta) return false;
  return true;
}

bool TriggerFeatureInvm::evaluateJet_L1(struct TrigBtagEmulationJet* jet) {   
  if ( jet->pt > 20 )
    m_priorityQueue_20.push_back( *jet );
  if ( jet->pt > 30 )
    m_priorityQueue_30.push_back( *jet );

  std::sort(m_priorityQueue_20.begin(),m_priorityQueue_20.end(),Trig::sortEmulationJetByPt);
  std::sort(m_priorityQueue_30.begin(),m_priorityQueue_30.end(),Trig::sortEmulationJetByPt);

  while ( m_priorityQueue_20.size() > 6)
    m_priorityQueue_20.pop_back();
  while ( m_priorityQueue_30.size() > 6)
    m_priorityQueue_30.pop_back();

  m_count_invm = 0;
  for (unsigned int i=0; i<m_priorityQueue_20.size(); i++)
    for (unsigned int j=0; j<m_priorityQueue_30.size(); j++)
      {
        double mjj = calculateINVM( m_priorityQueue_20.at(i),m_priorityQueue_30.at(j) );
        m_count_invm = std::max( m_count_invm, sqrt(mjj) );
      }

  return true;
}

bool TriggerFeatureInvm::evaluateJet_HLT(struct TrigBtagEmulationJet* jet) { 
  m_jetCollection_HLT.push_back( *jet );

  m_count_invm = 0;
  for (unsigned int i=0; i<m_jetCollection_HLT.size();i++)
    for (unsigned int j=i+1; j<m_jetCollection_HLT.size();j++)
      {
	double mjj = calculateINVM( m_jetCollection_HLT.at(i),m_jetCollection_HLT.at(j) );
	m_count_invm = std::max( m_count_invm, sqrt(mjj) );
      }
  
  return true;
}

double TriggerFeatureInvm::calculateINVM(struct TrigBtagEmulationJet const& jetA,struct TrigBtagEmulationJet const& jetB) const {
  // change eta accordingly to L1Topo
  // https://svnweb.cern.ch/trac/atlasgroups/browser/Trigger/L1Topo/L1TopoValidation/trunk/L1TopoCheck/Root/JetTOB.cxx#L56
  double eta_A = int( fabs(jetA.eta*100) + 0.1) * jetA.eta / fabs(jetA.eta);
  double eta_B = int( fabs(jetB.eta*100) + 0.1) * jetB.eta / fabs(jetB.eta);
  
  if ( eta_A == 245 ) { eta_A = 250; }
  else if ( eta_A == -245 ) { eta_A = -250; }
  else if ( eta_A ==  265 ) { eta_A =  270; }
  else if ( eta_A == -265 ) { eta_A = -270; } 
  else if ( eta_A ==  295 ) { eta_A =  290; } 
  else if ( eta_A == -295 ) { eta_A = -290; }
  
  if ( eta_B == 245 ) { eta_B = 250; }
  else if ( eta_B == -245 ) { eta_B = -250; }
  else if ( eta_B ==  265 ) { eta_B =  270; } 
  else if ( eta_B == -265 ) { eta_B = -270; }
  else if ( eta_B ==  295 ) { eta_B =  290; } 
  else if ( eta_B == -295 ) { eta_B = -290; }
    
  // ***

  double Deta = m_trigLevel == "L1" ? fabs( eta_B - eta_A )/100. : fabs( jetA.eta - jetB.eta );
  double Dphi = fabs( jetA.phi - jetB.phi );
  if (Dphi > M_PI)
    Dphi = 2*M_PI - Dphi;

  double coshDeta = m_trigLevel == "L1" ? cosh_LUT( Deta ) : cosh( Deta );
  double cosPhi   = m_trigLevel == "L1" ? cos_LUT( Dphi )  : cos( Dphi ) ;
  double mjj = 2 * jetA.pt * jetB.pt * ( coshDeta - cosPhi);

  return  m_trigLevel == "L1" ? mjj : round(mjj);
}

// ***
TriggerFeatureInvmCF::TriggerFeatureInvmCF(std::string triggerLevel,std::string name, float min_invm) : TriggerFeatureInvm(triggerLevel,name,min_invm) {}
TriggerFeatureInvmCF::TriggerFeatureInvmCF(const TriggerFeatureInvmCF& other) : TriggerFeatureInvm(other) {}
TriggerFeatureInvmCF::~TriggerFeatureInvmCF() {}

bool TriggerFeatureInvmCF::evaluateJet_L1(struct TrigBtagEmulationJet* jet) {
  if ( jet->pt > 20 )
    m_priorityQueue_20.push_back( *jet );
  if ( jet->pt > 30 )
    m_priorityQueue_30.push_back( *jet );

  std::sort(m_priorityQueue_20.begin(),m_priorityQueue_20.end(),Trig::sortEmulationJetByPt);
  std::sort(m_priorityQueue_30.begin(),m_priorityQueue_30.end(),Trig::sortEmulationJetByPt);

  while ( m_priorityQueue_20.size() > 6)
    m_priorityQueue_20.pop_back();
  while ( m_priorityQueue_30.size() > 6)
    m_priorityQueue_30.pop_back();

  m_count_invm = 0;
  for (unsigned int i=0; i<m_priorityQueue_20.size(); i++)
    for (unsigned int j=0; j<m_priorityQueue_30.size(); j++)
      {
	double mjj = calculateINVM( m_priorityQueue_20.at(i),m_priorityQueue_30.at(j) );
        if ( fabs(m_priorityQueue_20.at(i).eta) < 3.1 ) continue;
        if ( fabs(m_priorityQueue_30.at(j).eta) > 3.1 ) continue;
        m_count_invm = std::max( m_count_invm, sqrt(mjj) );
      }

  return true;
}

//**********************************************************************

void TriggerFeatureBtag::Print() { std::cout<<"BTAG [tagger="<<this->m_name<<"] : "<< this->m_weight; }
void TriggerFeatureAntiBtag::Print() { std::cout<<"ANTI-BTAG [tagger="<<this->m_name<<"] : "<< this->m_weight; }
void TriggerFeatureHt::Print() { std::cout<<"HT: "<< this->m_count_ht  <<"/"<< this->m_min_ht ; }
void TriggerFeatureHtTop::Print() { TriggerFeatureHt::Print(); if (this->m_topEt!=0) std::cout<<" [top "<<this->m_topEt<<"]"; }
void TriggerFeatureInvm::Print() { std::cout << "MJJ: " << this->m_count_invm << "/" << this->m_min_invm ; }
void TriggerFeatureInvmCF::Print() { std::cout << "MJJ-CF: " << this->m_count_invm << "/" << this->m_min_invm ; }

//**********************************************************************

