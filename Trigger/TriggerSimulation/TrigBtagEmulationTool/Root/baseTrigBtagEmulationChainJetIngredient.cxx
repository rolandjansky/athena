/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/baseTrigBtagEmulationChainJetIngredient.h"

using namespace Trig;

//**********************************************************************

baseTrigBtagEmulationChainJetIngredient::baseTrigBtagEmulationChainJetIngredient()
  : m_triggerName("NoTrigger"), m_min_pt(0), m_min_eta(0), m_max_eta(0), m_min_mult(0), m_count(0) {}
baseTrigBtagEmulationChainJetIngredient::baseTrigBtagEmulationChainJetIngredient(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : m_triggerName(name), m_min_pt(min_pt), m_min_eta(min_eta), m_max_eta(max_eta), m_min_mult(min_mult), m_count(0) {}
baseTrigBtagEmulationChainJetIngredient::baseTrigBtagEmulationChainJetIngredient(const baseTrigBtagEmulationChainJetIngredient& other)
  : m_triggerName(other.m_triggerName), 
    m_min_pt(other.m_min_pt), m_min_eta(other.m_min_eta), m_max_eta(other.m_max_eta), m_min_mult(other.m_min_mult), m_count(other.m_count),
    m_type_THRESHOLD_features(other.m_type_THRESHOLD_features.begin(),other.m_type_THRESHOLD_features.end()), 
    m_type_SELECTION_features(other.m_type_SELECTION_features.begin(),other.m_type_SELECTION_features.end()),
    m_neededJetCollection(other.m_neededJetCollection.begin(),other.m_neededJetCollection.end()) {}
baseTrigBtagEmulationChainJetIngredient::~baseTrigBtagEmulationChainJetIngredient() {}

bool baseTrigBtagEmulationChainJetIngredient::evaluateJet(struct TrigBtagEmulationJet& jet)
{
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

bool baseTrigBtagEmulationChainJetIngredient::needsJet(std::string item) { return m_neededJetCollection["MAIN"] == item; } 
bool baseTrigBtagEmulationChainJetIngredient::addJet(std::string key,std::vector< struct TrigBtagEmulationJet >& jets)
{
  m_jetCollection[key] = &jets;
  return true;
}

bool baseTrigBtagEmulationChainJetIngredient::evaluate()
{
  std::string name = m_neededJetCollection["MAIN"];
  for (unsigned int index(0); index < m_jetCollection[name]->size(); index++)
    this->evaluateJet( m_jetCollection[name]->at(index) );
  return true;
}

void baseTrigBtagEmulationChainJetIngredient::addFeature(std::string name,feature* toBeAdded)
{
  if (toBeAdded->m_type == feature::THRESHOLD) m_type_THRESHOLD_features.push_back( std::make_pair(name,toBeAdded) );
  else if (toBeAdded->m_type == feature::SELECTION) m_type_SELECTION_features.push_back( std::make_pair(name,toBeAdded) );
}

bool baseTrigBtagEmulationChainJetIngredient::isPassed()
{
  if ( m_count < m_min_mult ) return false;

  bool exit = false;
  for (unsigned int index(0); index < m_type_SELECTION_features.size(); index++)
    if (!m_type_SELECTION_features.at(index).second->isPassed()) exit = true;
  if (exit) return false;

  return true;
}

bool baseTrigBtagEmulationChainJetIngredient::hasFeature(std::string feature)
{
  for (unsigned int index(0); index < m_type_THRESHOLD_features.size(); index++)
    if (m_type_THRESHOLD_features.at(index).first == feature) return true;
  for (unsigned int index(0); index < m_type_SELECTION_features.size(); index++)
    if (m_type_SELECTION_features.at(index).first == feature) return true;
  return false;
}

void baseTrigBtagEmulationChainJetIngredient::clear() 
{
  m_count = 0;
  for (unsigned int index(0); index < m_type_THRESHOLD_features.size(); index++)
    m_type_THRESHOLD_features.at(index).second->clear();
  for (unsigned int index(0); index < m_type_SELECTION_features.size(); index++)
    m_type_SELECTION_features.at(index).second->clear();
}

// ***** //

TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1() 
{m_neededJetCollection["MAIN"] = "LVL1";}
TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : baseTrigBtagEmulationChainJetIngredient(name,min_pt,min_eta,max_eta,min_mult) 
{m_neededJetCollection["MAIN"] = "LVL1";}
TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(const TrigBtagEmulationChainJetIngredient_L1& other)
  : baseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_L1::~TrigBtagEmulationChainJetIngredient_L1() {}

// ***** //

TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ() 
{m_neededJetCollection["MAIN"] = "LVL1_JJ";}
TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : TrigBtagEmulationChainJetIngredient_L1(name,min_pt,min_eta,max_eta,min_mult) 
{m_neededJetCollection["MAIN"] = "LVL1_JJ";}
TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(const TrigBtagEmulationChainJetIngredient_L1_JJ& other)
  : TrigBtagEmulationChainJetIngredient_L1(other) {}
TrigBtagEmulationChainJetIngredient_L1_JJ::~TrigBtagEmulationChainJetIngredient_L1_JJ() {}

// ***** //

TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT() {}
TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : baseTrigBtagEmulationChainJetIngredient(name,min_pt,min_eta,max_eta,min_mult) 
{
  if (name.find("split")!=std::string::npos) m_neededJetCollection["MAIN"] = "SPLIT";
  else m_neededJetCollection["MAIN"] = "EF";
}
TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(const TrigBtagEmulationChainJetIngredient_HLT& other) : baseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_HLT::~TrigBtagEmulationChainJetIngredient_HLT() {}

// ***** //

TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC()
  : m_min_gsc(0),m_min_mult_gsc(0),m_count_gsc(0) {m_neededJetCollection["GSC"] = "GSC";}
TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(std::string name,float min_pt,float min_gsc,float min_eta,float max_eta,unsigned int min_mult) 
  : TrigBtagEmulationChainJetIngredient_HLT(name,min_pt,min_eta,max_eta,min_mult),
    m_min_gsc(min_gsc),m_min_mult_gsc(min_mult),m_count_gsc(0) 
{
  m_neededJetCollection["GSC"] = "GSC";
  if (min_pt >= 35000) return;
  if (name.find("split")!=std::string::npos) m_neededJetCollection["MAIN"] = "SPLIT_GSC";
  else m_neededJetCollection["MAIN"] = "EF_GSC";
}
TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(const TrigBtagEmulationChainJetIngredient_GSC& other)
  : TrigBtagEmulationChainJetIngredient_HLT(other),m_min_gsc(other.m_min_gsc),
    m_min_mult_gsc(other.m_min_mult),m_count_gsc(other.m_count_gsc) {}
TrigBtagEmulationChainJetIngredient_GSC::~TrigBtagEmulationChainJetIngredient_GSC() {}

bool TrigBtagEmulationChainJetIngredient_GSC::needsJet(std::string item)
{
  if (item == "GSC") return true;
  return baseTrigBtagEmulationChainJetIngredient::needsJet(item);
}

bool TrigBtagEmulationChainJetIngredient_GSC::evaluateJet(struct TrigBtagEmulationJet& jet)
{

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

bool TrigBtagEmulationChainJetIngredient_GSC::isPassed()
{
  if (m_count_gsc < m_min_mult_gsc) return false;
  return TrigBtagEmulationChainJetIngredient_HLT::isPassed();
}

bool TrigBtagEmulationChainJetIngredient_GSC::hasFeature(std::string feature)
{
  if (feature == "GSC") return true;
  return TrigBtagEmulationChainJetIngredient_HLT::hasFeature(feature);
}
void TrigBtagEmulationChainJetIngredient_GSC::clear()
{
  m_count_gsc=0;
  TrigBtagEmulationChainJetIngredient_HLT::clear();
}

unsigned int TrigBtagEmulationChainJetIngredient_GSC::matchingJets(struct TrigBtagEmulationJet& jet) 
{
  unsigned int output = m_jetCollection["GSC"]->size();
  double min_distance = 1e4;

  for (unsigned int i(0); i < m_jetCollection["GSC"]->size(); i++)
    {
      double distance = sqrt(pow(jet.eta-m_jetCollection["GSC"]->at(i).eta,2)+pow(jet.phi-m_jetCollection["GSC"]->at(i).phi,2));
      if (distance < 0.1 && distance < min_distance) {output=i;min_distance=distance;}
    }

return output;
}

//**********************************************************************

void TrigBtagEmulationChainJetIngredient_L1::Print()
{
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
void TrigBtagEmulationChainJetIngredient_L1_JJ::Print() 
{
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
void TrigBtagEmulationChainJetIngredient_HLT::Print()
{
  TrigBtagEmulationChainJetIngredient_HLT emul = *this;

  std::cout<<"### HLT Trigger ["<<emul.m_triggerName<<"]"<<std::endl;

  std::cout<<"   : min_pt  ="<<emul.m_min_pt * 1e-3<<" [GeV]\n";
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

void TrigBtagEmulationChainJetIngredient_GSC::Print()
{
  TrigBtagEmulationChainJetIngredient_GSC emul = *this;

  std::cout<<"### GSC Trigger ["<<emul.m_triggerName<<"]"<<std::endl;

  std::cout<<"   : min_pt  ="<<emul.m_min_pt*1e-3<<" [GeV]\n";
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
