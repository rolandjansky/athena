/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
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

bool Trig::sortJetTupleByPt(const std::tuple<double,double,double>& a, const std::tuple<double,double,double>& b )
{ return std::get< 0 >(a) > std::get< 0 >(b); }

//**********************************************************************

TriggerFeature::TriggerFeature(MsgStream& msg,int type, std::string name) : m_type(type), m_name(name), m_msg( msg ) {}
TriggerFeature::TriggerFeature(const TriggerFeature& other) : m_type(other.m_type), m_name(other.m_name), m_msg( other.m_msg ) {}
TriggerFeature::~TriggerFeature() {}

MsgStream& TriggerFeature::msg() const { return m_msg; }
MsgStream& TriggerFeature::msg( const MSG::Level lvl ) const { return msg() << lvl; }

const std::string& TriggerFeature::name() const { return m_name; }

TriggerFeatureBtag::TriggerFeatureBtag(MsgStream& msg,std::string name,float weight)
  : TriggerFeature(msg,TriggerFeature::THRESHOLD,name), m_weight(weight) {}
TriggerFeatureBtag::TriggerFeatureBtag(const TriggerFeatureBtag& other)
  : TriggerFeature(other), m_weight(other.m_weight) {}
TriggerFeatureBtag::~TriggerFeatureBtag() {}

float TriggerFeatureBtag::getCut() const {return m_weight;}

bool TriggerFeatureBtag::isPassed() const { return true; }
bool TriggerFeatureBtag::evaluateJet(const TrigBtagEmulationJet& jet) { return jet.weights( m_name.c_str() ) >= m_weight; }
void TriggerFeatureBtag::clear() {}

std::unique_ptr< TriggerFeature > TriggerFeatureBtag::uniqueClone() const { return std::unique_ptr< TriggerFeatureBtag >( new TriggerFeatureBtag( *this ) ); }

// ***

TriggerFeatureAntiBtag::TriggerFeatureAntiBtag(MsgStream& msg,std::string name,float weight)
  : TriggerFeatureBtag(msg,name,weight) {}
TriggerFeatureAntiBtag::TriggerFeatureAntiBtag(const TriggerFeatureAntiBtag& other)
  : TriggerFeatureBtag(other) {}
TriggerFeatureAntiBtag::~TriggerFeatureAntiBtag() {}

bool TriggerFeatureAntiBtag::evaluateJet(const TrigBtagEmulationJet& jet) { return !TriggerFeatureBtag::evaluateJet(jet); } 

// ***

TriggerFeatureHt::TriggerFeatureHt(MsgStream& msg,std::string triggerLevel,std::string name, float ht)
  : TriggerFeature(msg,TriggerFeature::SELECTION,name), m_trigLevel(triggerLevel), m_min_ht(ht), m_count_ht(0),
    m_cut_pt(0), m_cut_min_eta(0), m_cut_max_eta(9.0) {}
TriggerFeatureHt::TriggerFeatureHt(const TriggerFeatureHt& other)
  : TriggerFeature(other), m_trigLevel(other.m_trigLevel), m_min_ht(other.m_min_ht), m_count_ht(other.m_count_ht), 
    m_cut_pt(other.m_cut_pt), m_cut_min_eta(other.m_cut_min_eta), m_cut_max_eta(other.m_cut_max_eta) {}
TriggerFeatureHt::~TriggerFeatureHt() {}

float TriggerFeatureHt::getCut() const {return m_min_ht;}
void TriggerFeatureHt::setCuts(float min_pt, float min_eta, float max_eta)
{m_cut_pt = min_pt; m_cut_min_eta = min_eta; m_cut_max_eta = max_eta;}

bool TriggerFeatureHt::satisfyCuts(const double pt, const double eta) const {
  if ( pt <= m_cut_pt) return false;
  if (fabs( eta ) < m_cut_min_eta) return false;
  if (fabs( eta ) > m_cut_max_eta) return false;
  return true;
}

bool TriggerFeatureHt::isPassed() const {
  if (m_trigLevel == "L1" ) return isPassed_L1();
  if (m_trigLevel == "HLT") return isPassed_HLT();
  return false;
}

bool TriggerFeatureHt::evaluateJet(const TrigBtagEmulationJet& jet) {
  if (m_trigLevel == "L1" ) return evaluateJet_L1(jet);
  if (m_trigLevel == "HLT") return evaluateJet_HLT(jet);
  return false;
}

bool TriggerFeatureHt::isPassed_L1() const  { return m_count_ht > m_min_ht; }
bool TriggerFeatureHt::isPassed_HLT() const { return m_count_ht >= m_min_ht; }
bool TriggerFeatureHt::evaluateJet_L1(const TrigBtagEmulationJet& jet) {
  if ( satisfyCuts( jet.pt(),jet.eta() ) )
    m_count_ht += jet.pt();
  return true;
}
bool TriggerFeatureHt::evaluateJet_HLT(const TrigBtagEmulationJet& jet) {
  if ( jet.pt() < 30000 ) return true;
  if ( fabs(jet.eta()) > 3.2 ) return true;
  m_count_ht += jet.pt(); 
  return true;
}

void TriggerFeatureHt::clear() { m_count_ht = 0; }

std::unique_ptr< TriggerFeature > TriggerFeatureHt::uniqueClone() const { return std::unique_ptr< TriggerFeatureHt >( new TriggerFeatureHt( *this ) ); }
// ***

TriggerFeatureHtTop::TriggerFeatureHtTop(MsgStream& msg,std::string triggerLevel,std::string name, float ht, unsigned int topEt)
  : TriggerFeatureHt(msg,triggerLevel,name,ht),
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
bool TriggerFeatureHtTop::evaluateJet_L1(const TrigBtagEmulationJet& jet) {
  if ( fabs(jet.eta()) > 3.1) return true;
  m_piorityQueue.push_back( std::make_tuple( jet.pt(),jet.eta(),jet.phi() ) );
  calculateHT_L1();
  return true;
}

void TriggerFeatureHtTop::calculateHT_L1() {
  sort(m_piorityQueue.begin(),m_piorityQueue.end(),Trig::sortJetTupleByPt);
  while ( m_topEt < m_piorityQueue.size() )
    m_piorityQueue.pop_back();

  m_count_ht = 0;
  for (unsigned int i=0; i<m_piorityQueue.size();i++)
    if ( TriggerFeatureHt::satisfyCuts( std::get< JetElement::PT  >( m_piorityQueue.at(i) ),
					std::get< JetElement::ETA >( m_piorityQueue.at(i) ) ) )
      m_count_ht += std::get< JetElement::PT  >( m_piorityQueue.at(i) );
}

std::unique_ptr< TriggerFeature > TriggerFeatureHtTop::uniqueClone() const { return std::unique_ptr< TriggerFeatureHtTop >( new TriggerFeatureHtTop( *this ) );}

// ***

TriggerFeatureInvm::TriggerFeatureInvm(MsgStream& msg,std::string triggerLevel,std::string name,float min_invm) 
  : TriggerFeature(msg,TriggerFeature::SELECTION,name), 
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

bool TriggerFeatureInvm::isPassed() const { return m_count_invm >= m_min_invm; }
bool TriggerFeatureInvm::evaluateJet(const TrigBtagEmulationJet& jet) {
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

bool TriggerFeatureInvm::satisfyCuts(const double pt,const double eta) const {
  if (pt <= m_cut_pt) return false;
  if (fabs( eta ) < m_cut_min_eta) return false;
  if (fabs( eta ) > m_cut_max_eta) return false;
  return true;
}

bool TriggerFeatureInvm::evaluateJet_L1(const TrigBtagEmulationJet& jet) {   
  if ( jet.pt() > 20 )
    m_priorityQueue_20.push_back( std::make_tuple(jet.pt(),jet.eta(),jet.phi()) );
  if ( jet.pt() > 30 )
    m_priorityQueue_30.push_back( std::make_tuple(jet.pt(),jet.eta(),jet.phi()) );

  std::sort(m_priorityQueue_20.begin(),m_priorityQueue_20.end(),Trig::sortJetTupleByPt);
  std::sort(m_priorityQueue_30.begin(),m_priorityQueue_30.end(),Trig::sortJetTupleByPt);

  while ( m_priorityQueue_20.size() > 6)
    m_priorityQueue_20.pop_back();
  while ( m_priorityQueue_30.size() > 6)
    m_priorityQueue_30.pop_back();

  m_count_invm = 0;
  for (unsigned int i=0; i<m_priorityQueue_20.size(); i++)
    for (unsigned int j=0; j<m_priorityQueue_30.size(); j++) {
      double mjj = calculateINVM( m_priorityQueue_20.at(i),m_priorityQueue_30.at(j) );
      m_count_invm = std::max( m_count_invm, sqrt(mjj) );
    }

  return true;
}

bool TriggerFeatureInvm::evaluateJet_HLT(const TrigBtagEmulationJet& jet) { 
  m_jetCollection_HLT.push_back( std::make_tuple(jet.pt(),jet.eta(),jet.phi()) );

  m_count_invm = 0;
  for (unsigned int i=0; i<m_jetCollection_HLT.size();i++)
    for (unsigned int j=i+1; j<m_jetCollection_HLT.size();j++) {
      double mjj = calculateINVM( m_jetCollection_HLT.at(i),m_jetCollection_HLT.at(j) );
      m_count_invm = std::max( m_count_invm, sqrt(mjj) );
    }
  
  return true;
}

double TriggerFeatureInvm::calculateINVM(const std::tuple<double,double,double>& jetA,const std::tuple<double,double,double>& jetB) const {
  // change eta accordingly to L1Topo
  // https://svnweb.cern.ch/trac/atlasgroups/browser/Trigger/L1Topo/L1TopoValidation/trunk/L1TopoCheck/Root/JetTOB.cxx#L56
  double eta_A = int( fabs( std::get< JetElement::ETA >(jetA) *100) + 0.1) * std::get< JetElement::ETA >(jetA) / fabs( std::get< JetElement::ETA >(jetA) );
  double eta_B = int( fabs( std::get< JetElement::ETA >(jetB) *100) + 0.1) * std::get< JetElement::ETA >(jetB) / fabs( std::get< JetElement::ETA >(jetB) );
  
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

  double Deta = m_trigLevel == "L1" ? fabs( eta_B - eta_A )/100. : fabs( std::get< JetElement::ETA >(jetA) - std::get< JetElement::ETA >(jetB) );
  double Dphi = fabs( TVector2::Phi_mpi_pi( std::get< JetElement::PHI >(jetA) - std::get< JetElement::PHI >(jetB) ) );

  double coshDeta = m_trigLevel == "L1" ? cosh_LUT( Deta ) : cosh( Deta );
  double cosPhi   = m_trigLevel == "L1" ? cos_LUT( Dphi )  : cos( Dphi ) ;
  double mjj = 2 * std::get< JetElement::PT >(jetA) * std::get< JetElement::PT >(jetB) * ( coshDeta - cosPhi);

  return  m_trigLevel == "L1" ? mjj : round(mjj);
}

std::unique_ptr< TriggerFeature > TriggerFeatureInvm::uniqueClone() const { return std::unique_ptr< TriggerFeatureInvm >( new TriggerFeatureInvm( *this ) ); }

// ***
TriggerFeatureInvmCF::TriggerFeatureInvmCF(MsgStream& msg,std::string triggerLevel,std::string name, float min_invm)
  : TriggerFeatureInvm(msg,triggerLevel,name,min_invm) {}
TriggerFeatureInvmCF::TriggerFeatureInvmCF(const TriggerFeatureInvmCF& other) : TriggerFeatureInvm(other) {}
TriggerFeatureInvmCF::~TriggerFeatureInvmCF() {}

bool TriggerFeatureInvmCF::evaluateJet_L1(const TrigBtagEmulationJet& jet) {
  if ( jet.pt() > 20 )
    m_priorityQueue_20.push_back( std::make_tuple(jet.pt(),jet.eta(),jet.phi()) );
  if ( jet.pt() > 30 )
    m_priorityQueue_30.push_back( std::make_tuple(jet.pt(),jet.eta(),jet.phi()) );

  std::sort(m_priorityQueue_20.begin(),m_priorityQueue_20.end(),Trig::sortJetTupleByPt);
  std::sort(m_priorityQueue_30.begin(),m_priorityQueue_30.end(),Trig::sortJetTupleByPt);

  while ( m_priorityQueue_20.size() > 6)
    m_priorityQueue_20.pop_back();
  while ( m_priorityQueue_30.size() > 6)
    m_priorityQueue_30.pop_back();

  m_count_invm = 0;
  for (unsigned int i=0; i<m_priorityQueue_20.size(); i++)
    for (unsigned int j=0; j<m_priorityQueue_30.size(); j++) {
      double mjj = calculateINVM( m_priorityQueue_20.at(i),m_priorityQueue_30.at(j) );
      if ( fabs( std::get< JetElement::ETA >( m_priorityQueue_20.at(i) ) ) < 3.1 ) continue;
      if ( fabs( std::get< JetElement::ETA >( m_priorityQueue_30.at(j) ) ) > 3.1 ) continue;
      m_count_invm = std::max( m_count_invm, sqrt(mjj) );
    }
  
  return true;
}

//**********************************************************************

void TriggerFeatureBtag::print() { ATH_MSG_INFO( "      BTAG [tagger="<<this->m_name<<"] : "<< this->m_weight ); }
void TriggerFeatureAntiBtag::print() { ATH_MSG_INFO( "      ANTI-BTAG [tagger="<<this->m_name<<"] : "<< this->m_weight ); }
void TriggerFeatureHt::print() { ATH_MSG_INFO( "      HT: "<< this->m_count_ht  <<"/"<< this->m_min_ht ); }
void TriggerFeatureHtTop::print() { 
  std::string message = Form( "      HT: %f/%f",this->m_count_ht,this->m_min_ht );
  if (this->m_topEt!=0) message += Form(" [top %d",this->m_topEt);
  ATH_MSG_INFO( message );
}
void TriggerFeatureInvm::print() { ATH_MSG_INFO( "      MJJ: " << this->m_count_invm << "/" << this->m_min_invm ); }
void TriggerFeatureInvmCF::print() { ATH_MSG_INFO( "      MJJ-CF: " << this->m_count_invm << "/" << this->m_min_invm ); }

//**********************************************************************

