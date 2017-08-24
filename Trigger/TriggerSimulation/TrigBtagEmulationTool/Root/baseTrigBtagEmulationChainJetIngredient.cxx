/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#include "TrigBtagEmulationTool/baseTrigBtagEmulationChainJetIngredient.h"

using namespace Trig;

//**********************************************************************

baseTrigBtagEmulationChainJetIngredient::baseTrigBtagEmulationChainJetIngredient()
  : _triggerName("NoTrigger"), _min_pt(0), _min_eta(0), _max_eta(0), _min_mult(0), _count(0) {}
baseTrigBtagEmulationChainJetIngredient::baseTrigBtagEmulationChainJetIngredient(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : _triggerName(name), _min_pt(min_pt), _min_eta(min_eta), _max_eta(max_eta), _min_mult(min_mult), _count(0) {}
baseTrigBtagEmulationChainJetIngredient::baseTrigBtagEmulationChainJetIngredient(const baseTrigBtagEmulationChainJetIngredient& other)
  : _triggerName(other._triggerName), 
    _min_pt(other._min_pt), _min_eta(other._min_eta), _max_eta(other._max_eta), _min_mult(other._min_mult), _count(other._count),
    type_THRESHOLD_features(other.type_THRESHOLD_features.begin(),other.type_THRESHOLD_features.end()), 
    type_SELECTION_features(other.type_SELECTION_features.begin(),other.type_SELECTION_features.end()),
    _neededJetCollection(other._neededJetCollection.begin(),other._neededJetCollection.end()) {}
baseTrigBtagEmulationChainJetIngredient::~baseTrigBtagEmulationChainJetIngredient() {}

bool baseTrigBtagEmulationChainJetIngredient::evaluateJet(struct TrigBtagEmulationJet& jet)
{
  bool exiting = false;

  for (unsigned int index(0); index < type_THRESHOLD_features.size(); index++)
    if (!type_THRESHOLD_features.at(index).second->evaluateJet(&jet)) exiting = true;
  for (unsigned int index(0); index < type_SELECTION_features.size(); index++)
    if (!type_SELECTION_features.at(index).second->evaluateJet(&jet)) exiting = true;

  if (exiting) return false;

  if (jet.pt <= _min_pt) return false;
  if (fabs(jet.eta) < _min_eta) return false;
  if (fabs(jet.eta) > _max_eta) return false;

  _count++;
  return true;
}

bool baseTrigBtagEmulationChainJetIngredient::needsJet(std::string item) { return _neededJetCollection["MAIN"] == item; } 
bool baseTrigBtagEmulationChainJetIngredient::addJet(std::string key,std::vector< struct TrigBtagEmulationJet >& jets)
{
  _jetCollection[key] = &jets;
  return true;
}

bool baseTrigBtagEmulationChainJetIngredient::evaluate()
{
  std::string name = _neededJetCollection["MAIN"];
  for (unsigned int index(0); index < _jetCollection[name]->size(); index++)
    this->evaluateJet( _jetCollection[name]->at(index) );
  return true;
}

void baseTrigBtagEmulationChainJetIngredient::addFeature(std::string name,feature* toBeAdded)
{
  if (toBeAdded->_type == feature::THRESHOLD) type_THRESHOLD_features.push_back( std::make_pair(name,toBeAdded) );
  else if (toBeAdded->_type == feature::SELECTION) type_SELECTION_features.push_back( std::make_pair(name,toBeAdded) );
}

bool baseTrigBtagEmulationChainJetIngredient::isPassed()
{
  if ( _count < _min_mult ) return false;

  bool exit = false;
  for (unsigned int index(0); index < type_SELECTION_features.size(); index++)
    if (!type_SELECTION_features.at(index).second->isPassed()) exit = true;
  if (exit) return false;

  return true;
}

bool baseTrigBtagEmulationChainJetIngredient::hasFeature(std::string feature)
{
  for (unsigned int index(0); index < type_THRESHOLD_features.size(); index++)
    if (type_THRESHOLD_features.at(index).first == feature) return true;
  for (unsigned int index(0); index < type_SELECTION_features.size(); index++)
    if (type_SELECTION_features.at(index).first == feature) return true;
  return false;
}

void baseTrigBtagEmulationChainJetIngredient::clear() 
{
  _count = 0;
  for (unsigned int index(0); index < type_THRESHOLD_features.size(); index++)
    type_THRESHOLD_features.at(index).second->clear();
  for (unsigned int index(0); index < type_SELECTION_features.size(); index++)
    type_SELECTION_features.at(index).second->clear();
}

// ***** //

TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1() 
{_neededJetCollection["MAIN"] = "LVL1";}
TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : baseTrigBtagEmulationChainJetIngredient(name,min_pt,min_eta,max_eta,min_mult) 
{_neededJetCollection["MAIN"] = "LVL1";}
TrigBtagEmulationChainJetIngredient_L1::TrigBtagEmulationChainJetIngredient_L1(const TrigBtagEmulationChainJetIngredient_L1& other)
  : baseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_L1::~TrigBtagEmulationChainJetIngredient_L1() {}

// ***** //

TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ() 
{_neededJetCollection["MAIN"] = "LVL1_JJ";}
TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : TrigBtagEmulationChainJetIngredient_L1(name,min_pt,min_eta,max_eta,min_mult) 
{_neededJetCollection["MAIN"] = "LVL1_JJ";}
TrigBtagEmulationChainJetIngredient_L1_JJ::TrigBtagEmulationChainJetIngredient_L1_JJ(const TrigBtagEmulationChainJetIngredient_L1_JJ& other)
  : TrigBtagEmulationChainJetIngredient_L1(other) {}
TrigBtagEmulationChainJetIngredient_L1_JJ::~TrigBtagEmulationChainJetIngredient_L1_JJ() {}

// ***** //

TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT() {}
TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(std::string name,float min_pt,float min_eta,float max_eta,unsigned int min_mult)
  : baseTrigBtagEmulationChainJetIngredient(name,min_pt,min_eta,max_eta,min_mult) 
{
  if (name.find("split")!=std::string::npos) _neededJetCollection["MAIN"] = "SPLIT";
  else _neededJetCollection["MAIN"] = "EF";
}
TrigBtagEmulationChainJetIngredient_HLT::TrigBtagEmulationChainJetIngredient_HLT(const TrigBtagEmulationChainJetIngredient_HLT& other) : baseTrigBtagEmulationChainJetIngredient(other) {}
TrigBtagEmulationChainJetIngredient_HLT::~TrigBtagEmulationChainJetIngredient_HLT() {}

// ***** //

TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC()
  : _min_gsc(0),_min_mult_gsc(0),_count_gsc(0) {_neededJetCollection["GSC"] = "GSC";}
TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(std::string name,float min_pt,float min_gsc,float min_eta,float max_eta,unsigned int min_mult) 
  : TrigBtagEmulationChainJetIngredient_HLT(name,min_pt,min_eta,max_eta,min_mult),
    _min_gsc(min_gsc),_min_mult_gsc(min_mult),_count_gsc(0) 
{
  _neededJetCollection["GSC"] = "GSC";
  if (min_pt >= 35000) return;
  if (name.find("split")!=std::string::npos) _neededJetCollection["MAIN"] = "SPLIT_GSC";
  else _neededJetCollection["MAIN"] = "EF_GSC";
}
TrigBtagEmulationChainJetIngredient_GSC::TrigBtagEmulationChainJetIngredient_GSC(const TrigBtagEmulationChainJetIngredient_GSC& other)
  : TrigBtagEmulationChainJetIngredient_HLT(other),_min_gsc(other._min_gsc),
    _min_mult_gsc(other._min_mult),_count_gsc(other._count_gsc) {}
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
  if (index_GSC == _jetCollection["GSC"]->size()) return false;

  bool exiting = false;
  for (unsigned int index(0); index < type_THRESHOLD_features.size(); index++)
    if (!type_THRESHOLD_features.at(index).second->evaluateJet( &_jetCollection["GSC"]->at(index_GSC)) ) exiting = true;
  if (exiting) return false;

  if (_jetCollection["GSC"]->at(index_GSC).pt < _min_gsc) return false;
  if (fabs(_jetCollection["GSC"]->at(index_GSC).eta) < _min_eta) return false;
  if (fabs(_jetCollection["GSC"]->at(index_GSC).eta) > _max_eta) return false;

  _count_gsc++;
  return true;
}

bool TrigBtagEmulationChainJetIngredient_GSC::isPassed()
{
  if (_count_gsc < _min_mult_gsc) return false;
  return TrigBtagEmulationChainJetIngredient_HLT::isPassed();
}

bool TrigBtagEmulationChainJetIngredient_GSC::hasFeature(std::string feature)
{
  if (feature == "GSC") return true;
  return TrigBtagEmulationChainJetIngredient_HLT::hasFeature(feature);
}
void TrigBtagEmulationChainJetIngredient_GSC::clear()
{
  _count_gsc=0;
  TrigBtagEmulationChainJetIngredient_HLT::clear();
}

unsigned int TrigBtagEmulationChainJetIngredient_GSC::matchingJets(struct TrigBtagEmulationJet& jet) 
{
  unsigned int output = _jetCollection["GSC"]->size();
  double min_distance = 1e4;

  for (unsigned int i(0); i < _jetCollection["GSC"]->size(); i++)
    {
      double distance = sqrt(pow(jet.eta-_jetCollection["GSC"]->at(i).eta,2)+pow(jet.phi-_jetCollection["GSC"]->at(i).phi,2));
      if (distance < 0.1 && distance < min_distance) {output=i;min_distance=distance;}
    }

return output;
}

//**********************************************************************

void TrigBtagEmulationChainJetIngredient_L1::Print()
{
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  std::cout<<"### L1 Trigger ["<<emul._triggerName<<"]"<<std::endl;  
  std::cout<<"   : min_pt  ="<<emul._min_pt<<" [GeV]\n";
  std::cout<<"   : min_eta ="<<emul._min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul._max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul._count<<"/"<<emul._min_mult<<"\n";
  std::cout<<"   : Features\n";

  for (unsigned int index(0); index < emul.type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_THRESHOLD_features.at(index).first<<"] ";
      emul.type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_SELECTION_features.at(index).first<<"] ";
      emul.type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }
}
void TrigBtagEmulationChainJetIngredient_L1_JJ::Print() 
{
  TrigBtagEmulationChainJetIngredient_L1 emul = *this;

  std::cout<<"### L1 JJ Trigger ["<<emul._triggerName<<"]"<<std::endl;
  std::cout<<"   : min_pt  ="<<emul._min_pt<<" [GeV]\n";
  std::cout<<"   : min_eta ="<<emul._min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul._max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul._count<<"/"<<emul._min_mult<<"\n";
  std::cout<<"   : Features\n";

  for (unsigned int index(0); index < emul.type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_THRESHOLD_features.at(index).first<<"] ";
      emul.type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_SELECTION_features.at(index).first<<"] ";
      emul.type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }

}
void TrigBtagEmulationChainJetIngredient_HLT::Print()
{
  TrigBtagEmulationChainJetIngredient_HLT emul = *this;

  std::cout<<"### HLT Trigger ["<<emul._triggerName<<"]"<<std::endl;

  std::cout<<"   : min_pt  ="<<emul._min_pt * 1e-3<<" [GeV]\n";
  std::cout<<"   : min_eta ="<<emul._min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul._max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul._count<<"/"<<emul._min_mult<<"\n";
  std::cout<<"   : Features\n";


  for (unsigned int index(0); index < emul.type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_THRESHOLD_features.at(index).first<<"] ";
      emul.type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_SELECTION_features.at(index).first<<"] ";
      emul.type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }
  
}

void TrigBtagEmulationChainJetIngredient_GSC::Print()
{
  TrigBtagEmulationChainJetIngredient_GSC emul = *this;

  std::cout<<"### GSC Trigger ["<<emul._triggerName<<"]"<<std::endl;

  std::cout<<"   : min_pt  ="<<emul._min_pt*1e-3<<" [GeV]\n";
  std::cout<<"   : min_gsc ="<<emul._min_gsc<<"\n";
  std::cout<<"   : min_eta ="<<emul._min_eta<<"\n";
  std::cout<<"   : max_eta ="<<emul._max_eta<<"\n";
  std::cout<<"   : mult    ="<<emul._count<<"/"<<emul._min_mult<<"\n";
  std::cout<<"   : mult_gsc="<<emul._count_gsc<<"/"<<emul._min_mult<<"\n";
  std::cout<<"   : Features\n";


  for (unsigned int index(0); index < emul.type_THRESHOLD_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_THRESHOLD_features.at(index).first<<"] ";
      emul.type_THRESHOLD_features.at(index).second->Print();
      std::cout<<"\n";
    }
  for (unsigned int index(0); index < emul.type_SELECTION_features.size(); index++)
    {
      std::cout<<"          : ["<<emul.type_SELECTION_features.at(index).first<<"] ";
      emul.type_SELECTION_features.at(index).second->Print();
      std::cout<<"\n";
    }
  
}

//**********************************************************************
