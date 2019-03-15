/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/BaseTrigBtagEmulationChainJetIngredient.h"
#include "TVector2.h"
#include "regex"

using namespace Trig;

//**********************************************************************
// *** Virtual Base Class
//**********************************************************************

std::map< std::string,std::vector< std::unique_ptr< TrigBtagEmulationJet > > > BaseTrigBtagEmulationChainJetIngredient::m_jetCollection = std::map< std::string,std::vector< std::unique_ptr< TrigBtagEmulationJet > > >();


BaseTrigBtagEmulationChainJetIngredient::BaseTrigBtagEmulationChainJetIngredient(MsgStream &msg,std::string triggerName,std::string jetCollection)
  : m_triggerName(triggerName), 
    m_min_pt(0), 
    m_min_eta(0), 
    m_max_eta(0),
    m_min_mult(0),
    m_count(0),
    m_neededJetCollection(jetCollection),
    m_msg( msg ) {}
BaseTrigBtagEmulationChainJetIngredient::BaseTrigBtagEmulationChainJetIngredient(const BaseTrigBtagEmulationChainJetIngredient& other)
  : m_triggerName(other.m_triggerName), 
    m_min_pt(other.m_min_pt), 
    m_min_eta(other.m_min_eta), 
    m_max_eta(other.m_max_eta), 
    m_min_mult(other.m_min_mult), 
    m_count(other.m_count),
    m_neededJetCollection(other.m_neededJetCollection),
    m_msg( other.m_msg )
{
  std::map< std::string,std::unique_ptr<TriggerFeature> >::const_iterator it = other.m_type_THRESHOLD_features.begin();
  for ( ; it != other.m_type_THRESHOLD_features.end(); it++ ) 
    m_type_THRESHOLD_features[ it->first.c_str() ] = it->second->uniqueClone();
  for ( it = other.m_type_SELECTION_features.begin(); it != other.m_type_SELECTION_features.end(); it++ )
    m_type_SELECTION_features[ it->first.c_str() ] = it->second->uniqueClone();
}
BaseTrigBtagEmulationChainJetIngredient::~BaseTrigBtagEmulationChainJetIngredient() {}

MsgStream& BaseTrigBtagEmulationChainJetIngredient::msg() const { return m_msg; }
MsgStream& BaseTrigBtagEmulationChainJetIngredient::msg( const MSG::Level lvl ) const { return msg() << lvl; }
bool BaseTrigBtagEmulationChainJetIngredient::msgLvl ( const MSG::Level lvl ) const { return msg().level() <= lvl; }

// =================================================== //

bool BaseTrigBtagEmulationChainJetIngredient::addJet(std::string key,std::vector< std::unique_ptr< TrigBtagEmulationJet > > jets) {
  if ( m_jetCollection.find( key ) != m_jetCollection.end() ) m_jetCollection.at( key ).clear();

  m_jetCollection[ key ] = std::vector< std::unique_ptr< TrigBtagEmulationJet > >();
  for ( std::unique_ptr< TrigBtagEmulationJet >& jet : jets ) 
    m_jetCollection.at( key ).push_back( std::unique_ptr< TrigBtagEmulationJet >( new TrigBtagEmulationJet( *jet ) ) );
  return true;
}

void BaseTrigBtagEmulationChainJetIngredient::clearJet() {
  std::map< std::string,std::vector< std::unique_ptr< TrigBtagEmulationJet > > >::iterator it = m_jetCollection.begin();
  for ( ; it != m_jetCollection.end(); it++ )
    it->second.clear();
}

// =================================================== //

bool BaseTrigBtagEmulationChainJetIngredient::evaluateJet(const TrigBtagEmulationJet& jet) {
  bool exiting = false;

  std::map< std::string,std::unique_ptr<TriggerFeature> >::iterator it = m_type_THRESHOLD_features.begin();
  for ( ; it != m_type_THRESHOLD_features.end(); it++ )
    if ( ! it->second->evaluateJet(jet) ) exiting = true;
  for ( it = m_type_SELECTION_features.begin(); it != m_type_SELECTION_features.end(); it++ )
    if ( ! it->second->evaluateJet(jet) ) exiting = true;

  if (exiting) return false;

  if (jet.pt() <= m_min_pt) return false;
  if (fabs(jet.eta()) < m_min_eta) return false;
  if (fabs(jet.eta()) > m_max_eta) return false;

  m_count++;
  return true;
}

bool BaseTrigBtagEmulationChainJetIngredient::needsJet(const std::string& item) const { return m_neededJetCollection == item; } 

bool BaseTrigBtagEmulationChainJetIngredient::evaluate() {
  std::string name = m_neededJetCollection;
  for (unsigned int index(0); index < m_jetCollection.at( name ).size(); index++) 
    this->evaluateJet( *m_jetCollection.at( name ).at(index) );
  return true;
}

void BaseTrigBtagEmulationChainJetIngredient::addFeature(std::string name,std::unique_ptr< TriggerFeature >& toBeAdded) {
  if ( !toBeAdded ) return;

  if (toBeAdded->m_type == TriggerFeature::THRESHOLD) 
    m_type_THRESHOLD_features[ name.c_str() ] = std::move( toBeAdded ) ; 
  else if (toBeAdded->m_type == TriggerFeature::SELECTION) 
    m_type_SELECTION_features[ name.c_str() ] = std::move( toBeAdded ) ; 
}

bool BaseTrigBtagEmulationChainJetIngredient::isPassed() const {
  if ( m_count < m_min_mult ) return false;

  bool exit = false;
  std::map< std::string,std::unique_ptr<TriggerFeature> >::const_iterator it=m_type_SELECTION_features.begin();
  for ( ; it != m_type_SELECTION_features.end(); it++ )
    if ( ! it->second->isPassed() ) exit = true;

  if (exit) return false;
  return true;
}

bool BaseTrigBtagEmulationChainJetIngredient::hasFeature(const std::string& feature) const {
  if ( m_type_THRESHOLD_features.find( feature.c_str() ) != m_type_THRESHOLD_features.end() ) return true;
  if ( m_type_SELECTION_features.find( feature.c_str() ) != m_type_SELECTION_features.end() ) return true;
  return false;
}

void BaseTrigBtagEmulationChainJetIngredient::clear() {
  m_count = 0;
  std::map< std::string,std::unique_ptr<TriggerFeature> >::iterator it=m_type_THRESHOLD_features.begin();
  for ( ; it != m_type_THRESHOLD_features.end(); it++ )
    it->second->clear();
  for ( it=m_type_SELECTION_features.begin(); it != m_type_SELECTION_features.end(); it++ )
    it->second->clear();
}

template<typename T> void BaseTrigBtagEmulationChainJetIngredient::extract(const std::string& input,const std::string& target,T &valA) {
  if (input.find( target.c_str() ) == std::string::npos) return;
  valA = std::stoi( input.substr( input.find( target.c_str() ) + target.length(),input.length() - target.length() ) );
}

template<typename T,typename U> void BaseTrigBtagEmulationChainJetIngredient::extract(const std::string& input,const std::string& target,T &valA,U &valB) {
  if (input.find( target.c_str() ) == std::string::npos) return;
  valA = input.find( target.c_str() )!=0 ? std::stoi( input.substr( 0,input.find( target.c_str() ) ) ) : 0;
  valB = std::stoi( input.substr( input.find( target.c_str() ) + target.length(),input.length() - target.length() ) );
}

// Getters
std::string BaseTrigBtagEmulationChainJetIngredient::getName() const { return m_triggerName; }
double BaseTrigBtagEmulationChainJetIngredient::getPtThreshold() const { return m_min_pt; }
double BaseTrigBtagEmulationChainJetIngredient::getMinEtaThreshold() const { return m_min_eta; }
double BaseTrigBtagEmulationChainJetIngredient::getMaxEtaThreshold() const { return m_max_eta; }
int BaseTrigBtagEmulationChainJetIngredient::getMulteplicityThreshold() const { return m_min_mult; }
int BaseTrigBtagEmulationChainJetIngredient::getMulteplicity() const { return m_count; }

//**********************************************************************
// *** L1
//**********************************************************************

TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(MsgStream &msg,std::string triggerName)
  : BaseTrigBtagEmulationChainJetIngredient(msg,triggerName,"LVL1") { m_max_eta = 3.1; } 
TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(const TrigBtagEmulationChainJetIngredient_L1& other)
  : BaseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_L1::~TrigBtagEmulationChainJetIngredient_L1() {}

void TrigBtagEmulationChainJetIngredient_L1::initialize() {
  // Removing "EMUL_L1_"
  std::string input = "";
  if (m_triggerName.find("EMUL")!=std::string::npos) input = m_triggerName.substr( 8 , m_triggerName.length() - 8);
  else input = m_triggerName.substr( 3 , m_triggerName.length() - 3);

  std::vector< std::unique_ptr< TriggerFeature > > m_features;
  // MJJ triggers are ALWAYS L1_MJJ-XXX-KKK
  if ( input.find("MJJ")!=std::string::npos ) {
    m_features.push_back( this->setINVM( input ) );
    input = "";
  }

  while( !input.empty() ) {
    std::string delimiter = "";
    if (input.find(".") != std::string::npos && input.find("-") != std::string::npos)
      delimiter = input.find(".") < input.find("-") ? "." : "-";
    else if (input.find(".") != std::string::npos) 
      delimiter = ".";
    else if (input.find("-") != std::string::npos) 
      delimiter = "-";

    std::string subString = delimiter.empty() ? input : input.substr( 0,input.find( delimiter.c_str() ) ) ;
    input = delimiter.empty() ? "" : input.substr( subString.length() + 1 , input.length() - subString.length() - 1);

    if (subString.find("HT")!=std::string::npos) m_features.push_back( this->setHT( subString ) );
    else if (subString.find("ETA")!=std::string::npos) this->setEta( subString ); 
    else if (subString.find("J")!=std::string::npos) this->setPT( subString );
  }

  // For L1Topo HT triggers we must manually set the multeplicity to 0
  if( m_triggerName.find("_HT")!=std::string::npos ) m_min_mult = 0;

  for (unsigned int i = 0; i < m_features.size(); i++) {
    if ( ! m_features.at(i) ) continue;
    m_features.at(i)->setCuts(m_min_pt,m_min_eta,m_max_eta);
    this->addFeature( m_features.at(i)->name(), m_features.at(i) ); 
  }
}

void TrigBtagEmulationChainJetIngredient_L1::setPT(const std::string& input) {
  // The JJ chack is necessary in case we are dealing with a L1Topo trigger that uses et4x4()
  if (input.find("JJ")!=std::string::npos) this->extract(input,"JJ",m_min_mult,m_min_pt); 
  else this->extract(input,"J",m_min_mult,m_min_pt);
  if (m_min_mult == 0) m_min_mult = 1;
}
void TrigBtagEmulationChainJetIngredient_L1::setEta(const std::string& input) { 
  this->extract(input,"ETA",m_min_eta,m_max_eta); 
  m_min_eta /= 10.;
  m_max_eta /= 10.;
}
std::unique_ptr< TriggerFeature > TrigBtagEmulationChainJetIngredient_L1::setHT(const std::string& input) { 
  int m_ht = 0;
  this->extract(input,"HT",m_ht);

  if ( input.find("s5")!=std::string::npos ) return std::unique_ptr< TriggerFeature >( new TriggerFeatureHtTop(msg(),"L1","HT",m_ht,5) );
  else return std::unique_ptr< TriggerFeature >( new TriggerFeatureHt(msg(),"L1","HT",m_ht) );
}
std::unique_ptr< TriggerFeature > TrigBtagEmulationChainJetIngredient_L1::setINVM(const std::string& input) { 
  m_max_eta=4.9; 

  int min_invm = 0;
  this->extract(input,"MJJ-",min_invm);

  if ( input.find("CF")!=std::string::npos ) return std::unique_ptr< TriggerFeature >( new TriggerFeatureInvmCF(msg(),"L1","MJJ",min_invm) );
  else if ( input.find("NFF")!=std::string::npos ) return std::unique_ptr< TriggerFeature >( new TriggerFeatureInvmNFF(msg(),"L1","MJJ",min_invm) );
  else return std::unique_ptr< TriggerFeature >( new TriggerFeatureInvm(msg(),"L1","MJJ",min_invm) );
}

//**********************************************************************
// *** L1_JJ
//**********************************************************************

TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(MsgStream &msg,std::string triggerName)
  : TrigBtagEmulationChainJetIngredient_L1(msg,triggerName) {
  m_neededJetCollection = "LVL1_JJ"; }
TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(const TrigBtagEmulationChainJetIngredient_L1_JJ& other)
  : TrigBtagEmulationChainJetIngredient_L1(other) {}
TrigBtagEmulationChainJetIngredient_L1_JJ::~TrigBtagEmulationChainJetIngredient_L1_JJ() {}

//**********************************************************************
// *** HLT
//**********************************************************************

TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(MsgStream &msg,std::string triggerName)
  : BaseTrigBtagEmulationChainJetIngredient(msg,triggerName, triggerName.find("split") != std::string::npos ? "SPLIT" : "EF") {
  m_max_eta = 3.20;
}
TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(const TrigBtagEmulationChainJetIngredient_HLT& other) : BaseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_HLT::~TrigBtagEmulationChainJetIngredient_HLT() {}

void TrigBtagEmulationChainJetIngredient_HLT::initialize() {
  // Removing EMUL_HLT_
  std::string input = "";
  if (m_triggerName.find("EMUL")!=std::string::npos) input = m_triggerName.substr( 9 , m_triggerName.length() - 9);
  else input = m_triggerName.substr( 4 , m_triggerName.length() - 4);

  std::vector< std::unique_ptr< TriggerFeature > > m_features;
  while( !input.empty() ) {
    std::string delimiter = "";
    if (input.find("_") != std::string::npos) delimiter = "_";

    std::string subString = delimiter.empty() ? input : input.substr( 0,input.find( delimiter.c_str() ) ) ;
    input = delimiter.empty() ? "" : input.substr( subString.length() + 1 , input.length() - subString.length() - 1);

    if (subString.find("b")!=std::string::npos) this->setBTAG( subString );     
    else if (subString.find("ht")!=std::string::npos) m_features.push_back( this->setHT( subString ) );
    else if (subString.find("eta")!=std::string::npos) this->setEta( subString );
    else if (subString.find("invm")!=std::string::npos) m_features.push_back( this->setINVM( subString ) );
    else if (subString.find("j")!=std::string::npos) this->setPT( subString );

  }

  for (unsigned int i = 0; i < m_features.size(); i++) {
    if ( ! m_features.at(i) ) continue;
    m_features.at(i)->setCuts(m_min_pt,m_min_eta,m_max_eta);
    this->addFeature( m_features.at(i)->name(), m_features.at(i) );
  }
}

bool TrigBtagEmulationChainJetIngredient_HLT::overlaps(const TrigBtagEmulationChainJetIngredient_HLT& other) const {
  if ( m_min_pt == 0 || other.m_min_pt == 0 ) return false;

  bool overlapping = false;
  if (other.m_min_eta >= this->m_min_eta && other.m_min_eta < this->m_max_eta) overlapping = true;
  else if (other.m_max_eta > this->m_min_eta && other.m_max_eta <= this->m_max_eta) overlapping = true;
  else if (other.m_min_eta <= this->m_min_eta && other.m_max_eta >= this->m_max_eta) overlapping = true;

  if ( !overlapping ) return false;

  double min_btag_this  = this->hasFeature("BTAG") ? m_type_THRESHOLD_features.at( "BTAG" )->getCut() : -1;
  double max_btag_this  = this->hasFeature("ANTI-BTAG") ? m_type_THRESHOLD_features.at( "ANTI-BTAG" )->getCut() : 1;
  double min_btag_other = other.hasFeature("BTAG") ? other.m_type_THRESHOLD_features.at( "BTAG" )->getCut() : -1;
  double max_btag_other = other.hasFeature("ANTI-BTAG") ? other.m_type_THRESHOLD_features.at( "ANTI-BTAG" )->getCut() : 1;

  if (min_btag_other >= min_btag_this && min_btag_other < max_btag_this) return true;
  if (max_btag_other > min_btag_this && max_btag_other <= max_btag_this) return true;
  if (min_btag_other <= min_btag_this && max_btag_other >= max_btag_this) return true;

  return false;
}
bool TrigBtagEmulationChainJetIngredient_HLT::contains(const TrigBtagEmulationChainJetIngredient_HLT& other) const {
  bool isOnlyHT_this  = m_min_pt == 0 && this->hasFeature("HT") && !this->hasFeature("INVM");
  bool isOnlyHT_other = other.m_min_pt == 0 && other.hasFeature("HT") && !other.hasFeature("INVM");

  if (isOnlyHT_this || isOnlyHT_other) return false; 
  if (other.m_min_pt < this->m_min_pt) return false;
  if (other.m_min_eta < this->m_min_eta) return false;
  if (other.m_max_eta > this->m_max_eta) return false;

  if (!this->hasFeature("BTAG")) return true;
  if (!other.hasFeature("BTAG")) return false;

  // TO-DO move the method to the feature class for future
  double cutBtag_this = m_type_THRESHOLD_features.at( "BTAG" )->getCut();
  double cutBtag_other = other.m_type_THRESHOLD_features.at( "BTAG" )->getCut();

  if (cutBtag_other < cutBtag_this) return false;
  return true;
}
bool TrigBtagEmulationChainJetIngredient_HLT::isContained(const TrigBtagEmulationChainJetIngredient_HLT& other) const { return other.contains( *this ); }

std::vector< std::vector< std::string > > TrigBtagEmulationChainJetIngredient_HLT::resolveConflict( const TrigBtagEmulationChainJetIngredient_HLT& other ) const {
  std::vector< std::vector< std::string > > output;
  if ( not this->hasFeature("BTAG") && not other.hasFeature("BTAG") ) return output;

  // Retrieve b-tagging weight 
  double this__btag = this->hasFeature("BTAG") ? m_type_THRESHOLD_features.at( "BTAG" )->getCut() : -1; 
  double other_btag = other.hasFeature("BTAG") ? other.m_type_THRESHOLD_features.at( "BTAG" )->getCut() : -1; 

  const TrigBtagEmulationChainJetIngredient_HLT &trig_A = this__btag > other_btag ? other : *this;
  const TrigBtagEmulationChainJetIngredient_HLT &trig_B = this__btag > other_btag ? *this : other;
  
  // Conflict can be resolved with use b-tag regions
  const std::string trigA_name = trig_A.getName();
  const std::string trigB_name = trig_B.getName();

  // Extract b-tagging
  std::regex btagExtractor(".*_(?:ANTI)?b([^_]*)(?:_split)?");
  std::smatch match_A;
  std::smatch match_B;
  std::regex_search(trigA_name.begin(), trigA_name.end(), match_A, btagExtractor);
  std::regex_search(trigB_name.begin(), trigB_name.end(), match_B, btagExtractor);

  std::string trigA_btagging = match_A[1].str();
  std::string trigB_btagging = match_B[1].str();

  int trigA_mult = trig_A.getMulteplicityThreshold();
  int trigB_mult = trig_B.getMulteplicityThreshold();

  // newTrigA_name_p1 b-tagging changes with the inclusive one   
  std::string newTrigA_name_p1 = trigA_name.find("b") != std::string::npos ?
    std::regex_replace( trigA_name,std::regex("(b[^_]*)(_split)?"),"b"+trigB_btagging+"$2" ) :
    std::regex_replace( trigA_name,std::regex("HLT_([^_]*(?:_gsc[0-9]+)?)(_split)?"),"HLT_$1_b"+trigB_btagging+(trigB_name.find("_split")!=std::string::npos?"_split":"") ) ;
  // newTrigB_name_p1 changes multeplicity
  std::string newTrigB_name_p1 = std::regex_replace( trigB_name,std::regex("([0-9]*)(j[0-9]+)"),std::to_string( trigA_mult + trigB_mult )+"$2" );
  // newTrigA_name_p2 requires the anti-b-tagging 
  std::string newTrigA_name_p2 = trigA_name.find("b") != std::string::npos ?
    std::regex_replace( trigA_name,std::regex("(b[^_]*)(_split)?"),
			trigA_name.find("boffperf") != std::string::npos ? "ANTIb"+trigB_btagging+"$2" : 
			"$1_ANTIb"+trigB_btagging+"$2" ) : 
    std::regex_replace( trigA_name,std::regex("HLT_([^_]*(?:_gsc[0-9]+)?)(_split)?"),"HLT_$1_ANTIb"+trigB_btagging+(trigB_name.find("_split")!=std::string::npos?"_split":"") ) ;
  // newTrigB_name_p2 does not change 
  std::string newTrigB_name_p2 = trigB_name;

  output.push_back( std::vector< std::string >() );
  output.push_back( std::vector< std::string >() );

  output.at(0).push_back( newTrigA_name_p1 );
  output.at(0).push_back( newTrigB_name_p1 );

  output.at(1).push_back( newTrigA_name_p2 );
  output.at(1).push_back( newTrigB_name_p2 );

  return output;
}

void TrigBtagEmulationChainJetIngredient_HLT::addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT& other) {

  m_min_mult += other.m_min_mult;
  if (m_triggerName.find("j") == std::string::npos) return;

  int startIndex = m_triggerName.find("HLT_") + 4;
  int endIndex = m_triggerName.find("j");
  if ( fabs(endIndex - startIndex) > 2 ) return;
  m_triggerName.replace( startIndex , endIndex-startIndex, std::to_string(m_min_mult) );
}

void TrigBtagEmulationChainJetIngredient_HLT::setPT(const std::string& input) { 
  this->extract(input,"j",m_min_mult,m_min_pt);
  m_min_pt *= 1e3;
  if(m_min_mult == 0) m_min_mult = 1;
}
void TrigBtagEmulationChainJetIngredient_HLT::setEta(const std::string& input)  { 
  this->extract(input,"eta",m_min_eta,m_max_eta); 
  m_min_eta /= 100.;
  m_max_eta /= 100.;
}
std::unique_ptr< TriggerFeature > TrigBtagEmulationChainJetIngredient_HLT::setHT(const std::string& input) { 
  int m_ht = 0;
  this->extract(input,"ht",m_ht);
  return std::unique_ptr< TriggerFeature >( new TriggerFeatureHt(msg(),"HLT","HT",m_ht *1e3 ) );
}
std::unique_ptr< TriggerFeature > TrigBtagEmulationChainJetIngredient_HLT::setINVM(const std::string& input) { 
  int min_invm = 0;
  int min_pTlist = 0;

  if ( input.find("j") != std::string::npos ) this->extract(input,"j",min_pTlist);
  this->extract(input,"invm",min_invm);

  return std::unique_ptr< TriggerFeature >( new TriggerFeatureInvm(msg(),"HLT","INVM",min_invm * 1e3,min_pTlist * 1e3) );
}
void TrigBtagEmulationChainJetIngredient_HLT::setBTAG(const std::string& input) {

  std::string m_bjetTagger = "";
  if( input.find("hmv2c10") != std::string::npos )
    m_bjetTagger = "MV2c00";
  else if( input.find("mv2c10") != std::string::npos )  
    m_bjetTagger = "MV2c10";
  else if( input.find("mv2c20") != std::string::npos )  
    m_bjetTagger = "MV2c20";
  else 
    m_bjetTagger = "COMB";


  double m_btag = -1000;
  if ( input.find("bcombloose")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BCOMBLOOSE ;
  else if ( input.find("bcombmedium")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BCOMBMEDIUM ;
  else if ( input.find("bcombtight")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BCOMBTIGHT ;
  else if ( input.find("bloose")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BLOOSE ;
  else if ( input.find("bmedium")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BMEDIUM ;
  else if ( input.find("btight")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BTIGHT ;
  // MV2c20 Tagger
  else if ( input.find("mv2c2040")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2040 ;
  else if ( input.find("mv2c2050")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2050 ;
  else if ( input.find("mv2c2060")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2060 ;
  else if ( input.find("mv2c2070")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2070 ;
  else if ( input.find("mv2c2077")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2077 ;
  else if ( input.find("mv2c2085")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2085 ;
  // MV2c00 (Hybrid MV2c10) Tagger
  else if ( input.find("hmv2c1040")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C0040 ;
  else if ( input.find("hmv2c1050")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C0050 ;
  else if ( input.find("hmv2c1060")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C0060 ;
  else if ( input.find("hmv2c1070")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C0070 ;
  else if ( input.find("hmv2c1077")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C0077 ;
  else if ( input.find("hmv2c1085")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C0085 ;
  // MV2c10 Tagger
  else if ( input.find("mv2c1040")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C1040 ;
  else if ( input.find("mv2c1050")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C1050 ;
  else if ( input.find("mv2c1060")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C1060 ;
  else if ( input.find("mv2c1070")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C1070 ;
  else if ( input.find("mv2c1077")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C1077 ;
  else if ( input.find("mv2c1085")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C1085 ;

  double m_weight = -1000;
  double m_ANTIweight = 1000;

  if ( input.find("ANTI")==std::string::npos ) m_weight = m_btag;
  else if ( m_btag != -1000 ) m_ANTIweight = m_btag;

  std::unique_ptr< TriggerFeature > toBeAdded;
  std::string featureName = "";

  if (m_weight != -1000) {
    featureName = "BTAG";
    toBeAdded = std::unique_ptr< TriggerFeature >( new TriggerFeatureBtag( msg(),m_bjetTagger.c_str(),m_weight) );
  }
  if (m_ANTIweight != 1000) {
    featureName = "ANTI-BTAG";
    toBeAdded =std::unique_ptr< TriggerFeature >( new TriggerFeatureAntiBtag( msg(),m_bjetTagger.c_str(),m_ANTIweight) );
  }
  
  this->addFeature( featureName, toBeAdded );
}

//**********************************************************************
// *** GSC
//**********************************************************************

TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(MsgStream &msg,std::string triggerName)
  : TrigBtagEmulationChainJetIngredient_HLT(msg,triggerName),
    m_min_gsc(0),
    m_min_mult_gsc(0),
    m_count_gsc(0),
    m_supportingJetCollection("GSC") {
  m_neededJetCollection = "SPLIT_GSC";
}
TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(const TrigBtagEmulationChainJetIngredient_GSC& other)
  : TrigBtagEmulationChainJetIngredient_HLT(other),
    m_min_gsc(other.m_min_gsc),
    m_min_mult_gsc(other.m_min_mult),
    m_count_gsc(other.m_count_gsc),
    m_supportingJetCollection(other.m_supportingJetCollection) {}
TrigBtagEmulationChainJetIngredient_GSC::~TrigBtagEmulationChainJetIngredient_GSC() {}

bool TrigBtagEmulationChainJetIngredient_GSC::needsJet(const std::string& item) const {
  if (item == "GSC") return true;
  return BaseTrigBtagEmulationChainJetIngredient::needsJet(item);
}

bool TrigBtagEmulationChainJetIngredient_GSC::evaluateJet(const TrigBtagEmulationJet& jet) {
  if ( !TrigBtagEmulationChainJetIngredient_HLT::evaluateJet(jet) ) return false;

  unsigned int index_GSC = matchingJets(jet);
  if (index_GSC == m_jetCollection.at( "GSC" ).size()) return false;

  bool exiting = false;
  std::map< std::string,std::unique_ptr<TriggerFeature> >::iterator it = m_type_THRESHOLD_features.begin();
  for ( ; it != m_type_THRESHOLD_features.end(); it++ )
    if ( ! it->second->evaluateJet( *m_jetCollection.at( "GSC" ).at(index_GSC)) ) exiting = true;

  if (exiting) return false;

  if (m_jetCollection.at( "GSC" ).at(index_GSC)->pt() < m_min_gsc) return false;
  if (fabs(m_jetCollection.at( "GSC" ).at(index_GSC)->eta()) < m_min_eta) return false;
  if (fabs(m_jetCollection.at( "GSC" ).at(index_GSC)->eta()) > m_max_eta) return false;

  m_count_gsc++;
  return true;
}

bool TrigBtagEmulationChainJetIngredient_GSC::isPassed() const {
  if (m_count_gsc < m_min_mult_gsc) return false;
  return TrigBtagEmulationChainJetIngredient_HLT::isPassed();
}

bool TrigBtagEmulationChainJetIngredient_GSC::hasFeature(const std::string& feature) const {
  if (feature == "GSC") return true;
  return TrigBtagEmulationChainJetIngredient_HLT::hasFeature(feature);
}
void TrigBtagEmulationChainJetIngredient_GSC::clear() {
  m_count_gsc=0;
  TrigBtagEmulationChainJetIngredient_HLT::clear();
}

unsigned int TrigBtagEmulationChainJetIngredient_GSC::matchingJets(const TrigBtagEmulationJet& jet) {
  unsigned int output = m_jetCollection.at( "GSC" ).size();
  double min_distance = 1e4;

  for (unsigned int i(0); i < m_jetCollection.at( "GSC" ).size(); i++) {
    double deltaEta = jet.eta() - m_jetCollection.at( "GSC" ).at(i)->eta();
    double deltaPhi = TVector2::Phi_mpi_pi( jet.phi() - m_jetCollection.at( "GSC" ).at(i)->phi() );
    
    double distance = sqrt( pow(deltaEta,2) + pow(deltaPhi,2) ); 
    if (distance < 0.1 && distance < min_distance) {output=i;min_distance=distance;}
  }
  
  return output;
}

void TrigBtagEmulationChainJetIngredient_GSC::initialize() {
  // Removing EMUL_HLT_
  std::string input = "";
  if (m_triggerName.find("EMUL")!=std::string::npos) input = m_triggerName.substr( 9 , m_triggerName.length() - 9);
  else input = m_triggerName.substr( 4 , m_triggerName.length() - 4);

  std::vector< std::unique_ptr< TriggerFeature > > m_features;
  while( !input.empty() ) {
    std::string delimiter = "";
    if (input.find("_") != std::string::npos) delimiter = "_";
    
    std::string subString = delimiter.empty() ? input : input.substr( 0,input.find( delimiter.c_str() ) ) ;
    input = delimiter.empty() ? "" : input.substr( subString.length() + 1 , input.length() - subString.length() - 1);
    
    if (subString.find("b")!=std::string::npos) this->setBTAG( subString );
    else if (subString.find("ht")!=std::string::npos) m_features.push_back( this->setHT( subString ) );
    else if (subString.find("eta")!=std::string::npos) this->setEta( subString );
    else if (subString.find("j")!=std::string::npos) this->setPT( subString );
    else if (subString.find("gsc")!=std::string::npos) this->setPT( subString );
    else if (subString.find("invm")!=std::string::npos) m_features.push_back( this->setINVM( subString ) );
  }
  
  for (unsigned int i = 0; i < m_features.size(); i++) {
    if ( ! m_features.at(i) ) continue;
    m_features.at(i)->setCuts(m_min_pt,m_min_eta,m_max_eta);
    this->addFeature( m_features.at(i)->name(), m_features.at(i) );
  }
}

void TrigBtagEmulationChainJetIngredient_GSC::setPT(const std::string& input) {
  if ( input.find("j")!=std::string::npos)  
    TrigBtagEmulationChainJetIngredient_HLT::setPT(input);
  else if ( input.find("gsc")!=std::string::npos ) 
    { this->extract(input,"gsc",m_min_gsc); m_min_gsc *= 1e3; }
  m_min_mult_gsc = m_min_mult;
}

void TrigBtagEmulationChainJetIngredient_GSC::addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT& other) {
  TrigBtagEmulationChainJetIngredient_HLT::addJetsMulteplicity(other);
  m_min_mult_gsc = m_min_mult;
}

// Getters
double TrigBtagEmulationChainJetIngredient_GSC::getGscThreshold() const { return m_min_gsc; }
int TrigBtagEmulationChainJetIngredient_GSC::getMulteplicityThreshold() const { return m_min_mult_gsc; }
int TrigBtagEmulationChainJetIngredient_GSC::getMulteplicityGsc() const { return m_count_gsc; }

//**********************************************************************
// *** Print Methods
//**********************************************************************

void BaseTrigBtagEmulationChainJetIngredient::printFeatures() const {
  if ( m_type_THRESHOLD_features.size() == 0 && m_type_SELECTION_features.size() == 0 ) return;
  ATH_MSG_DEBUG( "   : Features" );

  std::map< std::string,std::unique_ptr<TriggerFeature> >::const_iterator it=m_type_THRESHOLD_features.begin();
  for ( ; it != m_type_THRESHOLD_features.end(); it++ ) {
    it->second->print();
  }

  for ( it = m_type_SELECTION_features.begin(); it != m_type_SELECTION_features.end(); it++ ) {
    it->second->print();
  }
}

void TrigBtagEmulationChainJetIngredient_L1::print() const {
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  ATH_MSG_DEBUG( "### L1 Trigger ["<< getName() <<"]" );
  ATH_MSG_DEBUG( "   : min_pt  = "<< getPtThreshold() <<" [MeV]" );
  ATH_MSG_DEBUG( "   : min_eta = "<< getMinEtaThreshold() );
  ATH_MSG_DEBUG( "   : max_eta = "<< getMaxEtaThreshold() );
  ATH_MSG_DEBUG( "   : mult    = "<< getMulteplicity() <<"/"<< getMulteplicityThreshold() );

  printFeatures();
}

void TrigBtagEmulationChainJetIngredient_L1_JJ::print() const {
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  ATH_MSG_DEBUG( "### L1 JJ Trigger ["<< getName() <<"]" );
  ATH_MSG_DEBUG( "   : min_pt  = "<< getPtThreshold() <<" [MeV]" );
  ATH_MSG_DEBUG( "   : min_eta = "<< getMinEtaThreshold() );
  ATH_MSG_DEBUG( "   : max_eta = "<< getMaxEtaThreshold() );
  ATH_MSG_DEBUG( "   : mult    = "<< getMulteplicity() <<"/"<< getMulteplicityThreshold() );

  printFeatures();
}

void TrigBtagEmulationChainJetIngredient_HLT::print() const {
  TrigBtagEmulationChainJetIngredient_HLT emul = *this;

  ATH_MSG_DEBUG( "### HLT Trigger ["<< getName() <<"]" );
  ATH_MSG_DEBUG( "   : min_pt  = "<< getPtThreshold() <<" [MeV]"  );
  ATH_MSG_DEBUG( "   : min_eta = "<< getMinEtaThreshold() );
  ATH_MSG_DEBUG( "   : max_eta = "<< getMaxEtaThreshold() );
  ATH_MSG_DEBUG( "   : mult    = "<< getMulteplicity() <<"/"<< getMulteplicityThreshold() );

  printFeatures();
}

void TrigBtagEmulationChainJetIngredient_GSC::print() const {
  TrigBtagEmulationChainJetIngredient_GSC emul = *this;

  ATH_MSG_DEBUG( "### GSC Trigger ["<< getName() <<"]" );
  ATH_MSG_DEBUG( "   : min_pt  = "<< getPtThreshold() <<" [MeV]" );
  ATH_MSG_DEBUG( "   : min_gsc = "<< getGscThreshold() );
  ATH_MSG_DEBUG( "   : min_eta = "<< getMinEtaThreshold() );
  ATH_MSG_DEBUG( "   : max_eta = "<< getMaxEtaThreshold() );
  ATH_MSG_DEBUG( "   : mult    = "<< getMulteplicity() <<"/"<< getMulteplicityThreshold() );
  ATH_MSG_DEBUG( "   : mult_gsc= "<< getMulteplicityGsc() <<"/"<< getMulteplicityThreshold() );

  printFeatures();
}

//**********************************************************************
