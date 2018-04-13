/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/BaseTrigBtagEmulationChainJetIngredient.h"
#include "TVector2.h"

using namespace Trig;

//**********************************************************************
// *** Virtual Base Class
//**********************************************************************

std::map< std::string,std::vector< std::unique_ptr< TrigBtagEmulationJet > > > BaseTrigBtagEmulationChainJetIngredient::m_jetCollection = std::map< std::string,std::vector< std::unique_ptr< TrigBtagEmulationJet > > >();


BaseTrigBtagEmulationChainJetIngredient::BaseTrigBtagEmulationChainJetIngredient(std::string triggerName,std::string jetCollection)
  : m_triggerName(triggerName), 
    m_min_pt(0), 
    m_min_eta(0), 
    m_max_eta(0),
    m_min_mult(0),
    m_count(0),
    m_neededJetCollection(jetCollection) {}
BaseTrigBtagEmulationChainJetIngredient::BaseTrigBtagEmulationChainJetIngredient(const BaseTrigBtagEmulationChainJetIngredient& other)
  : m_triggerName(other.m_triggerName), 
    m_min_pt(other.m_min_pt), 
    m_min_eta(other.m_min_eta), 
    m_max_eta(other.m_max_eta), 
    m_min_mult(other.m_min_mult), 
    m_count(other.m_count),
    m_neededJetCollection(other.m_neededJetCollection)
{
  std::map< std::string,std::unique_ptr<TriggerFeature> >::const_iterator it = other.m_type_THRESHOLD_features.begin();
  for ( ; it != other.m_type_THRESHOLD_features.end(); it++ ) 
    m_type_THRESHOLD_features[ it->first.c_str() ] = it->second->uniqueClone();
  for ( it = other.m_type_SELECTION_features.begin(); it != other.m_type_SELECTION_features.end(); it++ )
    m_type_SELECTION_features[ it->first.c_str() ] = it->second->uniqueClone();
}
BaseTrigBtagEmulationChainJetIngredient::~BaseTrigBtagEmulationChainJetIngredient() {}

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

TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(std::string triggerName)
  : BaseTrigBtagEmulationChainJetIngredient(triggerName,"LVL1") { m_max_eta = 3.1; } 
TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(const TrigBtagEmulationChainJetIngredient_L1& other)
  : BaseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_L1::~TrigBtagEmulationChainJetIngredient_L1() {}

void TrigBtagEmulationChainJetIngredient_L1::initialize() {
  // Removing "EMUL_L1_"
  std::string input = "";
  if (m_triggerName.find("EMUL")!=std::string::npos) input = m_triggerName.substr( 8 , m_triggerName.length() - 8);
  else input = m_triggerName.substr( 3 , m_triggerName.length() - 3);

  std::vector< std::unique_ptr< TriggerFeature > > m_features;
  // MJJ triggers are ALWAYS L1_MJJ-XXX
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

  if ( input.find("s5")!=std::string::npos ) return std::unique_ptr< TriggerFeature >( new TriggerFeatureHtTop("L1","HT",m_ht,5) );
  else return std::unique_ptr< TriggerFeature >( new TriggerFeatureHt("L1","HT",m_ht) );
}
std::unique_ptr< TriggerFeature > TrigBtagEmulationChainJetIngredient_L1::setINVM(const std::string& input) { 
  m_max_eta=4.9; 

  int min_invm = 0;
  this->extract(input,"MJJ-",min_invm);

  if ( input.find("CF")==std::string::npos ) return std::unique_ptr< TriggerFeature >( new TriggerFeatureInvm("L1","MJJ",min_invm) );
  else return std::unique_ptr< TriggerFeature >( new TriggerFeatureInvmCF("L1","MJJ",min_invm) );
}

//**********************************************************************
// *** L1_JJ
//**********************************************************************

TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(std::string triggerName)
  : TrigBtagEmulationChainJetIngredient_L1(triggerName) {
  m_neededJetCollection = "LVL1_JJ"; }
TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(const TrigBtagEmulationChainJetIngredient_L1_JJ& other)
  : TrigBtagEmulationChainJetIngredient_L1(other) {}
TrigBtagEmulationChainJetIngredient_L1_JJ::~TrigBtagEmulationChainJetIngredient_L1_JJ() {}

//**********************************************************************
// *** HLT
//**********************************************************************

TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(std::string triggerName)
  : BaseTrigBtagEmulationChainJetIngredient(triggerName, triggerName.find("split") != std::string::npos ? "SPLIT" : "EF") {
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
    else if (subString.find("j")!=std::string::npos) this->setPT( subString );
    else if (subString.find("invm")!=std::string::npos) m_features.push_back( this->setINVM( subString ) );

  }

  for (unsigned int i = 0; i < m_features.size(); i++) {
    if ( ! m_features.at(i) ) continue;
    m_features.at(i)->setCuts(m_min_pt,m_min_eta,m_max_eta);
    this->addFeature( m_features.at(i)->name(), m_features.at(i) );
  }
}

bool TrigBtagEmulationChainJetIngredient_HLT::overlaps(const TrigBtagEmulationChainJetIngredient_HLT& other) const {
  bool isOnlyHT_this  = m_min_pt == 0 && hasFeature("HT") && !hasFeature("INVM");
  bool isOnlyHT_other = other.m_min_pt == 0 && other.hasFeature("HT") && !other.hasFeature("INVM");

  if (isOnlyHT_this || isOnlyHT_other) return false;
  if (other.m_min_eta > this->m_min_eta && other.m_min_eta < this->m_max_eta) return true;
  if (other.m_max_eta > this->m_min_eta && other.m_max_eta < this->m_max_eta) return true;
  if (other.m_min_eta < this->m_min_eta && other.m_max_eta > this->m_max_eta) return true;
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
  return std::unique_ptr< TriggerFeature >( new TriggerFeatureHt("HLT","HT",m_ht *1e3 ) );
}
std::unique_ptr< TriggerFeature > TrigBtagEmulationChainJetIngredient_HLT::setINVM(const std::string& input) { 
  int min_invm = 0;
  this->extract(input,"invm",min_invm);
  return std::unique_ptr< TriggerFeature >( new TriggerFeatureInvm("HLT","INVM",min_invm * 1e3) );
}
void TrigBtagEmulationChainJetIngredient_HLT::setBTAG(const std::string& input) {

  std::string m_bjetTagger = "";
  if( input.find("mv2c10") != std::string::npos )  
    m_bjetTagger = "MV2c10";
  else if( input.find("mv2c20") != std::string::npos )  
    m_bjetTagger = "MV2c20";
  else 
    m_bjetTagger = "COMB";


  double m_btag = -1000;
  if ( input.find("bcombloose")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BCOMBLOOSE ;
  else if ( input.find("bcombmedium")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BCOMBMEDIUM ;
  else if ( input.find("bcombtight")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BCOMBTIGHT ;
  else if ( input.find("bloose")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BMEDIUM ;
  else if ( input.find("bmedium")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BMEDIUM ;
  else if ( input.find("btight")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::BTIGHT ;
  // MV2c20 Tagger
  else if ( input.find("mv2c2040")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2040 ;
  else if ( input.find("mv2c2050")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2050 ;
  else if ( input.find("mv2c2060")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2060 ;
  else if ( input.find("mv2c2070")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2070 ;
  else if ( input.find("mv2c2077")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2077 ;
  else if ( input.find("mv2c2085")!=std::string::npos ) m_btag = Trig::TriggerFeatureBtag::MV2C2085 ;
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
    toBeAdded = std::unique_ptr< TriggerFeature >( new TriggerFeatureBtag( m_bjetTagger.c_str(),m_weight) );
  }
  if (m_ANTIweight != 1000) {
    featureName = "ANTI-BTAG";
    toBeAdded =std::unique_ptr< TriggerFeature >( new TriggerFeatureAntiBtag( m_bjetTagger.c_str(),m_ANTIweight) );
  }
  
  this->addFeature( featureName, toBeAdded );
}

//**********************************************************************
// *** GSC
//**********************************************************************

TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(std::string triggerName)
  : TrigBtagEmulationChainJetIngredient_HLT(triggerName),
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
  std::cout<<"   : Features\n";

  std::map< std::string,std::unique_ptr<TriggerFeature> >::const_iterator it=m_type_THRESHOLD_features.begin();
  for ( ; it != m_type_THRESHOLD_features.end(); it++ ) {
    std::cout<<"          : ["<< (it->first) << "] ";
    it->second->Print();
    std::cout<<"\n";
  }

  for ( it = m_type_SELECTION_features.begin(); it != m_type_SELECTION_features.end(); it++ ) {
    std::cout<<"          : ["<< (it->first) << "] ";
    it->second->Print();
    std::cout<<"\n";
  }
}

void TrigBtagEmulationChainJetIngredient_L1::Print() const {
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  std::cout<<"### L1 Trigger ["<< getName() <<"]"<<std::endl;  
  std::cout<<"   : min_pt  ="<< getPtThreshold() <<" [GeV]\n";
  std::cout<<"   : min_eta ="<< getMinEtaThreshold() <<"\n";
  std::cout<<"   : max_eta ="<< getMaxEtaThreshold() <<"\n";
  std::cout<<"   : mult    ="<< getMulteplicity() <<"/"<< getMulteplicityThreshold() <<"\n";

  printFeatures();
}

void TrigBtagEmulationChainJetIngredient_L1_JJ::Print() const {
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  std::cout<<"### L1 JJ Trigger ["<< getName() <<"]"<<std::endl;
  std::cout<<"   : min_pt  ="<< getPtThreshold() <<" [GeV]\n";
  std::cout<<"   : min_eta ="<< getMinEtaThreshold() <<"\n";
  std::cout<<"   : max_eta ="<< getMaxEtaThreshold() <<"\n";
  std::cout<<"   : mult    ="<< getMulteplicity() <<"/"<< getMulteplicityThreshold() <<"\n";

  printFeatures();
}

void TrigBtagEmulationChainJetIngredient_HLT::Print() const {
  TrigBtagEmulationChainJetIngredient_HLT emul = *this;

  std::cout<<"### HLT Trigger ["<< getName() <<"]"<<std::endl;
  std::cout<<"   : min_pt  ="<< getPtThreshold() <<" [GeV]\n";
  std::cout<<"   : min_eta ="<< getMinEtaThreshold() <<"\n";
  std::cout<<"   : max_eta ="<< getMaxEtaThreshold() <<"\n";
  std::cout<<"   : mult    ="<< getMulteplicity() <<"/"<< getMulteplicityThreshold() <<"\n";

  printFeatures();
}

void TrigBtagEmulationChainJetIngredient_GSC::Print() const {
  TrigBtagEmulationChainJetIngredient_GSC emul = *this;

  std::cout<<"### GSC Trigger ["<< getName() <<"]"<<std::endl;
  std::cout<<"   : min_pt  ="<< getPtThreshold() <<" [GeV]\n";
  std::cout<<"   : min_gsc ="<< getGscThreshold() <<"\n";
  std::cout<<"   : min_eta ="<< getMinEtaThreshold() <<"\n";
  std::cout<<"   : max_eta ="<< getMaxEtaThreshold() <<"\n";
  std::cout<<"   : mult    ="<< getMulteplicity() <<"/"<< getMulteplicityThreshold() <<"\n";
  std::cout<<"   : mult_gsc="<< getMulteplicityGsc() <<"/"<< getMulteplicityThreshold() <<"\n";

  printFeatures();
}

//**********************************************************************
