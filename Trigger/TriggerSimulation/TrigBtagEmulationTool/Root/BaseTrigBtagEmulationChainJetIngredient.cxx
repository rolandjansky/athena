/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/BaseTrigBtagEmulationChainJetIngredient.h"

using namespace Trig;

//**********************************************************************
// *** Virtual Base Class
//**********************************************************************

BaseTrigBtagEmulationChainJetIngredient::BaseTrigBtagEmulationChainJetIngredient(std::string triggerName)
  : m_triggerName(triggerName), m_min_pt(0), m_min_eta(0), m_max_eta(0), m_min_mult(0), m_count(0) {}
BaseTrigBtagEmulationChainJetIngredient::BaseTrigBtagEmulationChainJetIngredient(const BaseTrigBtagEmulationChainJetIngredient& other)
  : m_triggerName(other.m_triggerName), 
    m_min_pt(other.m_min_pt), m_min_eta(other.m_min_eta), m_max_eta(other.m_max_eta), m_min_mult(other.m_min_mult), m_count(other.m_count),
    m_type_THRESHOLD_features(other.m_type_THRESHOLD_features.begin(),other.m_type_THRESHOLD_features.end()), 
    m_type_SELECTION_features(other.m_type_SELECTION_features.begin(),other.m_type_SELECTION_features.end()),
    m_neededJetCollection(other.m_neededJetCollection.begin(),other.m_neededJetCollection.end()) {}
BaseTrigBtagEmulationChainJetIngredient::~BaseTrigBtagEmulationChainJetIngredient() {}

std::string BaseTrigBtagEmulationChainJetIngredient::getName() const { return m_triggerName; }

bool BaseTrigBtagEmulationChainJetIngredient::evaluateJet(struct TrigBtagEmulationJet& jet) {
  bool exiting = false;

  for (unsigned int index(0); index < m_type_THRESHOLD_features.size(); index++)
    if (!m_type_THRESHOLD_features.at(index).second->evaluateJet(&jet)) exiting = true;
  for (unsigned int index(0); index < m_type_SELECTION_features.size(); index++)
    if (!m_type_SELECTION_features.at(index).second->evaluateJet(&jet)) exiting = true;

  if (exiting) return false;

  if (jet.pt <= m_min_pt) return false;
  if (fabs(jet.eta) < m_min_eta) return false;
  if (fabs(jet.eta) > m_max_eta) return false;

  m_count++;
  return true;
}

bool BaseTrigBtagEmulationChainJetIngredient::needsJet(std::string item) { return m_neededJetCollection["MAIN"] == item; } 
bool BaseTrigBtagEmulationChainJetIngredient::addJet(std::string key,std::vector< struct TrigBtagEmulationJet >& jets) {
  m_jetCollection[key] = &jets;
  return true;
}

bool BaseTrigBtagEmulationChainJetIngredient::evaluate() {
  std::string name = m_neededJetCollection["MAIN"];
  for (unsigned int index(0); index < m_jetCollection[name]->size(); index++)
    this->evaluateJet( m_jetCollection[name]->at(index) );
  return true;
}

void BaseTrigBtagEmulationChainJetIngredient::addFeature(std::string name,TriggerFeature* toBeAdded) {
  if (toBeAdded->m_type == TriggerFeature::THRESHOLD) m_type_THRESHOLD_features.push_back( std::make_pair(name,toBeAdded) );
  else if (toBeAdded->m_type == TriggerFeature::SELECTION) m_type_SELECTION_features.push_back( std::make_pair(name,toBeAdded) );
}

bool BaseTrigBtagEmulationChainJetIngredient::isPassed() {
  if ( m_count < m_min_mult ) return false;

  bool exit = false;
  for (unsigned int index(0); index < m_type_SELECTION_features.size(); index++)
    if (!m_type_SELECTION_features.at(index).second->isPassed()) exit = true;
  if (exit) return false;

  return true;
}

bool BaseTrigBtagEmulationChainJetIngredient::hasFeature(std::string feature) const {
  for (unsigned int index(0); index < m_type_THRESHOLD_features.size(); index++)
    if (m_type_THRESHOLD_features.at(index).first == feature) return true;
  for (unsigned int index(0); index < m_type_SELECTION_features.size(); index++)
    if (m_type_SELECTION_features.at(index).first == feature) return true;
  return false;
}

void BaseTrigBtagEmulationChainJetIngredient::clear() {
  m_count = 0;
  for (unsigned int index(0); index < m_type_THRESHOLD_features.size(); index++)
    m_type_THRESHOLD_features.at(index).second->clear();
  for (unsigned int index(0); index < m_type_SELECTION_features.size(); index++)
    m_type_SELECTION_features.at(index).second->clear();
}

template<typename T> void BaseTrigBtagEmulationChainJetIngredient::extract(std::string input,std::string target,T &valA) {
  valA = std::stoi( input.substr( input.find( target.c_str() ) + target.length(),input.length() - target.length() ) );
}

template<typename T,typename U> void BaseTrigBtagEmulationChainJetIngredient::extract(std::string input,std::string target,T &valA,U &valB) {
  valA = input.find( target.c_str() )!=0 ? std::stoi( input.substr( 0,input.find( target.c_str() ) ) ) : 0;
  valB = std::stoi( input.substr( input.find( target.c_str() ) + target.length(),input.length() - target.length() ) );
}

//**********************************************************************
// *** L1
//**********************************************************************

TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(std::string triggerName)
  : BaseTrigBtagEmulationChainJetIngredient(triggerName) { m_max_eta = 3.1; m_neededJetCollection["MAIN"] = "LVL1"; } 
TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(const TrigBtagEmulationChainJetIngredient_L1& other)
  : BaseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_L1::~TrigBtagEmulationChainJetIngredient_L1() {}

void TrigBtagEmulationChainJetIngredient_L1::initialize() {
  // Removing "EMUL_L1_"
  std::string input = "";
  if (m_triggerName.find("EMUL")!=std::string::npos) input = m_triggerName.substr( 8 , m_triggerName.length() - 8);
  else input = m_triggerName.substr( 3 , m_triggerName.length() - 3);

  std::vector< TriggerFeature* > m_features;
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

  for (unsigned int i = 0; i < m_features.size(); i++) {
    if (m_features.at(i) == nullptr) continue;
    m_features.at(i)->setCuts(m_min_pt,m_min_eta,m_max_eta);
    this->addFeature( m_features.at(i)->name(), m_features.at(i) );
  }
}

void TrigBtagEmulationChainJetIngredient_L1::setPT(std::string& input) {
  if (input.find("JJ")!=std::string::npos) this->extract(input,"JJ",m_min_mult,m_min_pt); 
  else this->extract(input,"J",m_min_mult,m_min_pt);
  if (m_min_mult == 0) m_min_mult = 1;
}
void TrigBtagEmulationChainJetIngredient_L1::setEta(std::string& input) { 
  this->extract(input,"ETA",m_min_eta,m_max_eta); 
  m_min_eta /= 10.;
  m_max_eta /= 10.;
}
TriggerFeature* TrigBtagEmulationChainJetIngredient_L1::setHT(std::string& input) { 
  int m_ht = 0;
  this->extract(input,"HT",m_ht);

  if ( input.find("s5")!=std::string::npos ) return new TriggerFeatureHtTop("L1","HT",m_ht,5);
  else return new TriggerFeatureHt("L1","HT",m_ht);
}
TriggerFeature* TrigBtagEmulationChainJetIngredient_L1::setINVM(std::string& input) { 
  m_max_eta=49; 

  int min_invm = 0;
  this->extract(input,"MJJ-",min_invm);

  if ( input.find("CF")==std::string::npos ) return new TriggerFeatureInvm("L1","MJJ",min_invm);
  else return new TriggerFeatureInvmCF("L1","MJJ",min_invm);
}

//**********************************************************************
// *** L1_JJ
//**********************************************************************

TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(std::string triggerName)
  : TrigBtagEmulationChainJetIngredient_L1(triggerName) { m_neededJetCollection["MAIN"] = "LVL1_JJ"; }
TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(const TrigBtagEmulationChainJetIngredient_L1_JJ& other)
  : TrigBtagEmulationChainJetIngredient_L1(other) {}
TrigBtagEmulationChainJetIngredient_L1_JJ::~TrigBtagEmulationChainJetIngredient_L1_JJ() {}

//**********************************************************************
// *** HLT
//**********************************************************************

TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(std::string triggerName)
  : BaseTrigBtagEmulationChainJetIngredient(triggerName) {
  m_max_eta = 3.20;
  if (triggerName.find("split")!=std::string::npos) m_neededJetCollection["MAIN"] = "SPLIT";
  else m_neededJetCollection["MAIN"] = "EF";
}
TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(const TrigBtagEmulationChainJetIngredient_HLT& other) : BaseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_HLT::~TrigBtagEmulationChainJetIngredient_HLT() {}

void TrigBtagEmulationChainJetIngredient_HLT::initialize() {
  // Removing EMUL_HLT_
  std::string input = "";
  if (m_triggerName.find("EMUL")!=std::string::npos) input = m_triggerName.substr( 9 , m_triggerName.length() - 9);
  else input = m_triggerName.substr( 4 , m_triggerName.length() - 4);

  std::vector< TriggerFeature* > m_features;
  while( !input.empty() ) {
    std::string delimiter = "";
    if (input.find("_") != std::string::npos) delimiter = "_";

    std::string subString = delimiter.empty() ? input : input.substr( 0,input.find( delimiter.c_str() ) ) ;
    input = delimiter.empty() ? "" : input.substr( subString.length() + 1 , input.length() - subString.length() - 1);
     
    if (subString.find("ht")!=std::string::npos) m_features.push_back( this->setHT( subString ) );
    else if (subString.find("eta")!=std::string::npos) this->setEta( subString );
    else if (subString.find("j")!=std::string::npos) this->setPT( subString );
    else if (subString.find("invm")!=std::string::npos) m_features.push_back( this->setINVM( subString ) );
    else if (subString.find("b")!=std::string::npos) this->setBTAG( subString );
  }

  for (unsigned int i = 0; i < m_features.size(); i++) {
    if (m_features.at(i) == nullptr) continue;
    m_features.at(i)->setCuts(m_min_pt,m_min_eta,m_max_eta);
    this->addFeature( m_features.at(i)->name(), m_features.at(i) );
  }
}

bool TrigBtagEmulationChainJetIngredient_HLT::overlaps(const TrigBtagEmulationChainJetIngredient_HLT* other) const {
  bool isOnlyHT_this  = m_min_pt == 0 && hasFeature("HT") && !hasFeature("INVM");
  bool isOnlyHT_other = other->m_min_pt == 0 && other->hasFeature("HT") && !other->hasFeature("INVM");

  if (isOnlyHT_this || isOnlyHT_other) return false;
  if (other->m_min_eta > this->m_min_eta && other->m_min_eta < this->m_max_eta) return true;
  if (other->m_max_eta > this->m_min_eta && other->m_min_eta < this->m_max_eta) return true;
  return false;
}
bool TrigBtagEmulationChainJetIngredient_HLT::contains(const TrigBtagEmulationChainJetIngredient_HLT* other) const {
  bool isOnlyHT_this  = m_min_pt == 0 && this->hasFeature("HT") && !this->hasFeature("INVM");
  bool isOnlyHT_other = other->m_min_pt == 0 && other->hasFeature("HT") && !other->hasFeature("INVM");

  if (isOnlyHT_this || isOnlyHT_other) return false; 
  if (other->m_min_pt < this->m_min_pt) return false;
  if (other->m_min_eta < this->m_min_eta) return false;
  if (other->m_max_eta > this->m_max_eta) return false;

  if (!this->hasFeature("BTAG")) return true;
  if (!other->hasFeature("BTAG")) return false;

  // TO-DO move the method to the feature class for future
  // or change std::vector< std::pair<std::string,Feature*> > to std::map< std::string,Feature* >
  double cutBtag_this = -1000;
  for (unsigned int i=0; i<m_type_THRESHOLD_features.size(); i++) 
    if (m_type_THRESHOLD_features.at(i).first == "BTAG") cutBtag_this = m_type_THRESHOLD_features.at(i).second->getCut();
  double cutBtag_other = -1000;
  for (unsigned int i=0; i<other->m_type_THRESHOLD_features.size(); i++) 
    if (other->m_type_THRESHOLD_features.at(i).first == "BTAG") cutBtag_this = other->m_type_THRESHOLD_features.at(i).second->getCut();

  if (cutBtag_other < cutBtag_this) return false;

  return true;
}
bool TrigBtagEmulationChainJetIngredient_HLT::isContained(const TrigBtagEmulationChainJetIngredient_HLT* other) const { return other->contains( this ); }
void TrigBtagEmulationChainJetIngredient_HLT::addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT* other) {

  m_min_mult += other->m_min_mult;
  if (m_triggerName.find("j") == std::string::npos) return;

  int startIndex = m_triggerName.find("HLT_") + 4;
  int endIndex = m_triggerName.find("j");
  if ( fabs(endIndex - startIndex) > 2 ) return;
  m_triggerName.replace( startIndex , endIndex-startIndex, std::to_string(m_min_mult) );
}

void TrigBtagEmulationChainJetIngredient_HLT::setPT(std::string& input) { 
  this->extract(input,"j",m_min_mult,m_min_pt);
  m_min_pt *= 1e3;
  if(m_min_mult == 0) m_min_mult = 1;
}
void TrigBtagEmulationChainJetIngredient_HLT::setEta(std::string& input)  { 
  this->extract(input,"eta",m_min_eta,m_max_eta); 
  m_min_eta /= 100.;
  m_max_eta /= 100.;
}
TriggerFeature* TrigBtagEmulationChainJetIngredient_HLT::setHT(std::string& input) { 
  int m_ht = 0;
  this->extract(input,"ht",m_ht);
  return new TriggerFeatureHt("HLT","HT",m_ht *1e3 );
}
TriggerFeature* TrigBtagEmulationChainJetIngredient_HLT::setINVM(std::string& input) { 
  int min_invm = 0;
  this->extract(input,"invm",min_invm);
  return new TriggerFeatureInvm("HLT","INVM",min_invm * 1e3);
}
void TrigBtagEmulationChainJetIngredient_HLT::setBTAG(std::string& input) {

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

  if (m_weight != -1000) this->addFeature("BTAG",new TriggerFeatureBtag( m_bjetTagger.c_str(),m_weight) );
  if (m_ANTIweight != 1000) this->addFeature("ANTI-BTAG",new TriggerFeatureAntiBtag( m_bjetTagger.c_str(),m_ANTIweight) );
}

//**********************************************************************
// *** GSC
//**********************************************************************

TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(std::string triggerName)
  : TrigBtagEmulationChainJetIngredient_HLT(triggerName), 
    m_min_gsc(0),m_min_mult_gsc(0),m_count_gsc(0) {
  m_neededJetCollection["GSC"] = "GSC"; 
  if (m_min_pt >= 35000) return; 
  m_neededJetCollection["MAIN"] = "SPLIT_GSC";
}
TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(const TrigBtagEmulationChainJetIngredient_GSC& other)
  : TrigBtagEmulationChainJetIngredient_HLT(other),m_min_gsc(other.m_min_gsc),
    m_min_mult_gsc(other.m_min_mult),m_count_gsc(other.m_count_gsc) {}
TrigBtagEmulationChainJetIngredient_GSC::~TrigBtagEmulationChainJetIngredient_GSC() {}

bool TrigBtagEmulationChainJetIngredient_GSC::needsJet(std::string item) {
  if (item == "GSC") return true;
  return BaseTrigBtagEmulationChainJetIngredient::needsJet(item);
}

bool TrigBtagEmulationChainJetIngredient_GSC::evaluateJet(struct TrigBtagEmulationJet& jet) {

  if ( !TrigBtagEmulationChainJetIngredient_HLT::evaluateJet(jet) ) return false;

  unsigned int index_GSC = matchingJets(jet);
  if (index_GSC == m_jetCollection["GSC"]->size()) return false;

  bool exiting = false;
  for (unsigned int index(0); index < m_type_THRESHOLD_features.size(); index++)
    if (!m_type_THRESHOLD_features.at(index).second->evaluateJet( &m_jetCollection["GSC"]->at(index_GSC)) ) exiting = true;
  if (exiting) return false;

  if (m_jetCollection["GSC"]->at(index_GSC).pt < m_min_gsc) return false;
  if (fabs(m_jetCollection["GSC"]->at(index_GSC).eta) < m_min_eta) return false;
  if (fabs(m_jetCollection["GSC"]->at(index_GSC).eta) > m_max_eta) return false;

  m_count_gsc++;
  return true;
}

bool TrigBtagEmulationChainJetIngredient_GSC::isPassed() {
  if (m_count_gsc < m_min_mult_gsc) return false;
  return TrigBtagEmulationChainJetIngredient_HLT::isPassed();
}

bool TrigBtagEmulationChainJetIngredient_GSC::hasFeature(std::string feature) const {
  if (feature == "GSC") return true;
  return TrigBtagEmulationChainJetIngredient_HLT::hasFeature(feature);
}
void TrigBtagEmulationChainJetIngredient_GSC::clear() {
  m_count_gsc=0;
  TrigBtagEmulationChainJetIngredient_HLT::clear();
}

unsigned int TrigBtagEmulationChainJetIngredient_GSC::matchingJets(struct TrigBtagEmulationJet& jet) {
  unsigned int output = m_jetCollection["GSC"]->size();
  double min_distance = 1e4;

  for (unsigned int i(0); i < m_jetCollection["GSC"]->size(); i++)
    {
      double distance = sqrt(pow(jet.eta-m_jetCollection["GSC"]->at(i).eta,2)+pow(jet.phi-m_jetCollection["GSC"]->at(i).phi,2));
      if (distance < 0.1 && distance < min_distance) {output=i;min_distance=distance;}
    }

return output;
}

void TrigBtagEmulationChainJetIngredient_GSC::initialize() {
  // Removing EMUL_HLT_
  std::string input = "";
  if (m_triggerName.find("EMUL")!=std::string::npos) input = m_triggerName.substr( 9 , m_triggerName.length() - 9);
  else input = m_triggerName.substr( 4 , m_triggerName.length() - 4);

  std::vector< TriggerFeature* > m_features;
  while( !input.empty() ) {
    std::string delimiter = "";
    if (input.find("_") != std::string::npos) delimiter = "_";
    
    std::string subString = delimiter.empty() ? input : input.substr( 0,input.find( delimiter.c_str() ) ) ;
    input = delimiter.empty() ? "" : input.substr( subString.length() + 1 , input.length() - subString.length() - 1);
    
    if (subString.find("ht")!=std::string::npos) m_features.push_back( this->setHT( subString ) );
    else if (subString.find("eta")!=std::string::npos) this->setEta( subString );
    else if (subString.find("j")!=std::string::npos) this->setPT( subString );
    else if (subString.find("gsc")!=std::string::npos) this->setPT( subString );
    else if (subString.find("invm")!=std::string::npos) m_features.push_back( this->setINVM( subString ) );
    else if (subString.find("mv2c10")!=std::string::npos) this->setBTAG( subString );
  }
  
  for (unsigned int i = 0; i < m_features.size(); i++) {
    if (m_features.at(i) == nullptr) continue;
    m_features.at(i)->setCuts(m_min_pt,m_min_eta,m_max_eta);
    this->addFeature( m_features.at(i)->name(), m_features.at(i) );
  }
}

void TrigBtagEmulationChainJetIngredient_GSC::setPT(std::string& input) {
  if ( input.find("j")!=std::string::npos)  
    TrigBtagEmulationChainJetIngredient_HLT::setPT(input);
  else if ( input.find("gsc")!=std::string::npos ) 
    { this->extract(input,"gsc",m_min_gsc); m_min_gsc *= 1e3; }
  m_min_mult_gsc = m_min_mult;
}

void TrigBtagEmulationChainJetIngredient_GSC::addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT* other) {
  TrigBtagEmulationChainJetIngredient_HLT::addJetsMulteplicity(other);
  m_min_mult_gsc = m_min_mult;
}

//**********************************************************************
// *** Print Methods
//**********************************************************************

void TrigBtagEmulationChainJetIngredient_L1::Print() {
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  std::cout<<"### L1 Trigger ["<<emul.m_triggerName<<"]"<<std::endl;  
  std::cout<<"   : min_pt  ="<<emul.m_min_pt<<" [GeV]\n";
  std::cout<<"   : min_eta ="<<emul.m_min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul.m_max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul.m_count<<"/"<<emul.m_min_mult<<"\n";
  std::cout<<"   : Features\n";

  for (unsigned int index(0); index < emul.m_type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_THRESHOLD_features.at(index).first<<"] ";
      emul.m_type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.m_type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_SELECTION_features.at(index).first<<"] ";
      emul.m_type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }
}
void TrigBtagEmulationChainJetIngredient_L1_JJ::Print() {
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  std::cout<<"### L1 JJ Trigger ["<<emul.m_triggerName<<"]"<<std::endl;
  std::cout<<"   : min_pt  ="<<emul.m_min_pt<<" [GeV]\n";
  std::cout<<"   : min_eta ="<<emul.m_min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul.m_max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul.m_count<<"/"<<emul.m_min_mult<<"\n";
  std::cout<<"   : Features\n";

  for (unsigned int index(0); index < emul.m_type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_THRESHOLD_features.at(index).first<<"] ";
      emul.m_type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.m_type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_SELECTION_features.at(index).first<<"] ";
      emul.m_type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }

}
void TrigBtagEmulationChainJetIngredient_HLT::Print() {
  TrigBtagEmulationChainJetIngredient_HLT emul = *this;

  std::cout<<"### HLT Trigger ["<<emul.m_triggerName<<"]"<<std::endl;

  std::cout<<"   : min_pt  ="<<emul.m_min_pt <<" [GeV]\n";
  std::cout<<"   : min_eta ="<<emul.m_min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul.m_max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul.m_count<<"/"<<emul.m_min_mult<<"\n";
  std::cout<<"   : Features\n";


  for (unsigned int index(0); index < emul.m_type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_THRESHOLD_features.at(index).first<<"] ";
      emul.m_type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.m_type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_SELECTION_features.at(index).first<<"] ";
      emul.m_type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }
  
}

void TrigBtagEmulationChainJetIngredient_GSC::Print() {
  TrigBtagEmulationChainJetIngredient_GSC emul = *this;

  std::cout<<"### GSC Trigger ["<<emul.m_triggerName<<"]"<<std::endl;

  std::cout<<"   : min_pt  ="<<emul.m_min_pt <<" [GeV]\n";
  std::cout<<"   : min_gsc ="<<emul.m_min_gsc<<"\n";
  std::cout<<"   : min_eta ="<<emul.m_min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul.m_max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul.m_count<<"/"<<emul.m_min_mult<<"\n";
  std::cout<<"   : mult_gsc="<<emul.m_count_gsc<<"/"<<emul.m_min_mult<<"\n";
  std::cout<<"   : Features\n";


  for (unsigned int index(0); index < emul.m_type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_THRESHOLD_features.at(index).first<<"] ";
      emul.m_type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.m_type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.m_type_SELECTION_features.at(index).first<<"] ";
      emul.m_type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }
  
}

//**********************************************************************
