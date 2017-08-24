/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#include "TrigBtagEmulationTool/feature.h"
#include "algorithm"

#include "fstream"
#include "sstream"
#include "TSystem.h"
#include "TVector2.h"

using namespace Trig;

//**********************************************************************

bool Trig::sortEmulationJet_byPt(Trig::TrigBtagEmulationJet& a,Trig::TrigBtagEmulationJet& b) { return a.pt > b.pt; }

//**********************************************************************

feature::feature(int type, std::string name) : _type(type), _name(name) {}
feature::feature(const feature& other) : _type(other._type), _name(other._name) {}
feature::~feature() {}

feature_btag::feature_btag(std::string name,float weight)
  : feature(feature::THRESHOLD,name), _weight(weight) {}
feature_btag::feature_btag(const feature_btag& other)
  : feature(other), _weight(other._weight) {}
feature_btag::~feature_btag() {}

bool feature_btag::isPassed() {return true;}
bool feature_btag::evaluateJet(struct TrigBtagEmulationJet* jet) { return jet->weights[_name.c_str()] >= _weight; }
void feature_btag::clear() {}

// ***

feature_antibtag::feature_antibtag(std::string name,float weight)
  : feature_btag(name,weight) {}
feature_antibtag::feature_antibtag(const feature_antibtag& other)
  : feature_btag(other) {}
feature_antibtag::~feature_antibtag() {}

bool feature_antibtag::evaluateJet(struct TrigBtagEmulationJet* jet) { return !feature_btag::evaluateJet(jet); } 

// ***

feature_ht::feature_ht(std::string triggerLevel,std::string name, float ht)
  : feature(feature::SELECTION,name), _trigLevel(triggerLevel), _min_ht(ht), _count_ht(0),
    _cut_pt(0), _cut_min_eta(0), _cut_max_eta(9.0) {}
feature_ht::feature_ht(const feature_ht& other)
  : feature(other), _trigLevel(other._trigLevel), _min_ht(other._min_ht), _count_ht(other._count_ht), 
    _cut_pt(other._cut_pt), _cut_min_eta(other._cut_min_eta), _cut_max_eta(other._cut_max_eta) {}
feature_ht::~feature_ht() {}

void feature_ht::setCuts(float min_pt, float min_eta, float max_eta)
{_cut_pt = min_pt; _cut_min_eta = min_eta; _cut_max_eta = max_eta;}

bool feature_ht::satisfyCuts(struct TrigBtagEmulationJet* jet)
{
  if (jet->pt <= _cut_pt) return false;
  if (fabs(jet->eta) < _cut_min_eta) return false;
  if (fabs(jet->eta) > _cut_max_eta) return false;
  return true;
}

bool feature_ht::isPassed()
{
  if (_trigLevel == "L1" ) return isPassed_L1();
  if (_trigLevel == "HLT") return isPassed_HLT();
  return false;
}

bool feature_ht::evaluateJet(struct TrigBtagEmulationJet* jet)
{
  if (_trigLevel == "L1" ) return evaluateJet_L1(jet);
  if (_trigLevel == "HLT") return evaluateJet_HLT(jet);
  return false;
}

bool feature_ht::isPassed_L1()  { return _count_ht > _min_ht; }
bool feature_ht::isPassed_HLT() { return _count_ht >= _min_ht; }
bool feature_ht::evaluateJet_L1(struct TrigBtagEmulationJet* jet)
{
  if ( satisfyCuts(jet) )
    _count_ht+=jet->pt;
  return true;
}
bool feature_ht::evaluateJet_HLT(struct TrigBtagEmulationJet* jet)
{
  if ( jet->pt < 30000 ) return true;
  if ( fabs(jet->eta) > 3.2 ) return true;
  _count_ht+=jet->pt; 
  return true;
}

void feature_ht::clear() { _count_ht = 0; }

// ***

feature_ht_top::feature_ht_top(std::string triggerLevel,std::string name, float ht, unsigned int topEt)
  : feature_ht(triggerLevel,name,ht),
    _topEt(topEt) {}
feature_ht_top::feature_ht_top(const feature_ht_top& other)
  : feature_ht(other),
    _piorityQueue(other._piorityQueue.begin(),other._piorityQueue.end()),
    _topEt(other._topEt) {}
feature_ht_top::~feature_ht_top() {}

void feature_ht_top::clear()
{
  feature_ht::clear();
  _piorityQueue.clear();
}
bool feature_ht_top::evaluateJet_L1(struct TrigBtagEmulationJet* jet)
{
  if ( fabs(jet->eta) > 3.1) return true;
  _piorityQueue.push_back( *jet );
  calculateHT_L1();
  return true;
}

void feature_ht_top::calculateHT_L1()
{
  sort(_piorityQueue.begin(),_piorityQueue.end(),Trig::sortEmulationJet_byPt);
  while ( _topEt < _piorityQueue.size() )
    _piorityQueue.pop_back();

  _count_ht = 0;
  for (unsigned int i=0; i<_piorityQueue.size();i++)
      if ( feature_ht::satisfyCuts(&_piorityQueue.at(i)) ) 
	_count_ht += _piorityQueue.at(i).pt;
}

// ***

feature_invm::feature_invm(std::string triggerLevel,std::string name,float min_invm) 
  : feature(feature::SELECTION,name), 
    _trigLevel(triggerLevel), 
    _min_invm(min_invm),
    _count_invm(0),
    _cut_pt(0),
    _cut_min_eta(0),
    _cut_max_eta(4.9) 
{ this->initLUTs(); }
feature_invm::feature_invm(const feature_invm& other)
  : feature(other),
    _trigLevel(other._trigLevel),
    _min_invm(other._min_invm),
    _count_invm(other._count_invm),
    _cut_pt(other._cut_pt),
    _cut_min_eta(other._cut_min_eta),
    _cut_max_eta(other._cut_max_eta),
    _jetCollection_HLT(other._jetCollection_HLT.begin(),other._jetCollection_HLT.end()),
    _priorityQueue_20(other._priorityQueue_20.begin(),other._priorityQueue_20.end()),
    _priorityQueue_30(other._priorityQueue_30.begin(),other._priorityQueue_30.end()) 
{ this->initLUTs(); }
feature_invm::~feature_invm() {}

void feature_invm::initLUTs()
{
#ifdef ROOTCORE
  const char* inputFileFolder = gSystem->ExpandPathName ("${ROOTCOREBIN}");
#else
  const char* inputFileFolder = gSystem->ExpandPathName ("${WorkDir_DIR}");
#endif

  // *** cosh ( Deta )
  std::string nameFile_LUTcosh = Form("%s/data/TrigBtagEmulationTool/LUT_Hyperbolic.txt",inputFileFolder);

  std::ifstream fileLUTcosh; fileLUTcosh.open(nameFile_LUTcosh);
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
      _LUTcosh[key] = value;
    }
  fileLUTcosh.close();

  // *** cos ( Dphi )
  std::string nameFile_LUTcos = Form("%s/data/TrigBtagEmulationTool/LUT_Trigo.txt",inputFileFolder);

  std::ifstream fileLUTcos; fileLUTcos.open(nameFile_LUTcos);
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
      _LUTcos[key] = value;
    }
  fileLUTcos.close();
}

double feature_invm::cosh_LUT(double value) const
{
  int argument = int( value * 10 + 0.1 );
  if (_LUTcosh.find(argument) != _LUTcosh.end())
    return _LUTcosh.at( argument );
  return 1;
}
double feature_invm::cos_LUT(double value) const
{
  int argument = int( value * 1e2 );
  if (_LUTcos.find(argument) != _LUTcos.end())
    return _LUTcos.at( argument );
  return 1.0;
}

bool feature_invm::isPassed() { return _count_invm >= _min_invm; }
bool feature_invm::evaluateJet(struct TrigBtagEmulationJet* jet) 
{
  if (_trigLevel == "L1") return evaluateJet_L1( jet );
  if (_trigLevel == "HLT") return evaluateJet_HLT( jet );
  return false;
}

void feature_invm::clear() 
{
  _count_invm = 0;
  _jetCollection_HLT.clear();
  _priorityQueue_20.clear();
  _priorityQueue_30.clear();
}

void feature_invm::setCuts(float min_pt,float min_eta,float max_eta) 
{_cut_pt = min_pt; _cut_min_eta = min_eta; _cut_max_eta = max_eta;}
bool feature_invm::satisfyCuts(struct TrigBtagEmulationJet* jet) 
{
  if (jet->pt <= _cut_pt) return false;
  if (fabs(jet->eta) < _cut_min_eta) return false;
  if (fabs(jet->eta) > _cut_max_eta) return false;
  return true;
}

bool feature_invm::evaluateJet_L1(struct TrigBtagEmulationJet* jet) 
{  

  if ( jet->pt > 20 )
    _priorityQueue_20.push_back( *jet );
  if ( jet->pt > 30 )
    _priorityQueue_30.push_back( *jet );

  std::sort(_priorityQueue_20.begin(),_priorityQueue_20.end(),Trig::sortEmulationJet_byPt);
  std::sort(_priorityQueue_30.begin(),_priorityQueue_30.end(),Trig::sortEmulationJet_byPt);

  while ( _priorityQueue_20.size() > 6)
    _priorityQueue_20.pop_back();
  while ( _priorityQueue_30.size() > 6)
    _priorityQueue_30.pop_back();

  _count_invm = 0;
  for (unsigned int i=0; i<_priorityQueue_20.size(); i++)
    for (unsigned int j=0; j<_priorityQueue_30.size(); j++)
      {
        double mjj = calculateINVM( _priorityQueue_20.at(i),_priorityQueue_30.at(j) );
        _count_invm = std::max( _count_invm, sqrt(mjj) );
      }

  return true;
}

bool feature_invm::evaluateJet_HLT(struct TrigBtagEmulationJet* jet) 
{ 
  _jetCollection_HLT.push_back( *jet );

  _count_invm = 0;
  for (unsigned int i=0; i<_jetCollection_HLT.size();i++)
    for (unsigned int j=i+1; j<_jetCollection_HLT.size();j++)
      {
	double mjj = calculateINVM( _jetCollection_HLT.at(i),_jetCollection_HLT.at(j) );
	_count_invm = std::max( _count_invm, sqrt(mjj) );
      }
  
  return true;
}

double feature_invm::calculateINVM(struct TrigBtagEmulationJet const& jetA,struct TrigBtagEmulationJet const& jetB) const 
{
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

  double Deta = _trigLevel == "L1" ? fabs( eta_B - eta_A )/100. : fabs( jetA.eta - jetB.eta );
  double Dphi = fabs( jetA.phi - jetB.phi );
  if (Dphi > M_PI)
    Dphi = 2*M_PI - Dphi;

  double coshDeta = _trigLevel == "L1" ? cosh_LUT( Deta ) : cosh( Deta );
  double cosPhi   = _trigLevel == "L1" ? cos_LUT( Dphi )  : cos( Dphi ) ;
  double mjj = 2 * jetA.pt * jetB.pt * ( coshDeta - cosPhi);

  return  _trigLevel == "L1" ? mjj : round(mjj);
}

// ***
feature_invm_CF::feature_invm_CF(std::string triggerLevel,std::string name, float min_invm) : feature_invm(triggerLevel,name,min_invm) {}
feature_invm_CF::feature_invm_CF(const feature_invm_CF& other) : feature_invm(other) {}
feature_invm_CF::~feature_invm_CF() {}

bool feature_invm_CF::evaluateJet_L1(struct TrigBtagEmulationJet* jet) 
{
  if ( jet->pt > 20 )
    _priorityQueue_20.push_back( *jet );
  if ( jet->pt > 30 )
    _priorityQueue_30.push_back( *jet );

  std::sort(_priorityQueue_20.begin(),_priorityQueue_20.end(),Trig::sortEmulationJet_byPt);
  std::sort(_priorityQueue_30.begin(),_priorityQueue_30.end(),Trig::sortEmulationJet_byPt);

  while ( _priorityQueue_20.size() > 6)
    _priorityQueue_20.pop_back();
  while ( _priorityQueue_30.size() > 6)
    _priorityQueue_30.pop_back();

  _count_invm = 0;
  for (unsigned int i=0; i<_priorityQueue_20.size(); i++)
    for (unsigned int j=0; j<_priorityQueue_30.size(); j++)
      {
	double mjj = calculateINVM( _priorityQueue_20.at(i),_priorityQueue_30.at(j) );
        if ( fabs(_priorityQueue_20.at(i).eta) < 3.1 ) continue;
        if ( fabs(_priorityQueue_30.at(j).eta) > 3.1 ) continue;
        _count_invm = std::max( _count_invm, sqrt(mjj) );
      }

  return true;
}

//**********************************************************************

void feature_btag::Print() { std::cout<<"BTAG [tagger="<<this->_name<<"] : "<< this->_weight; }
void feature_antibtag::Print() { std::cout<<"ANTI-BTAG [tagger="<<this->_name<<"] : "<< this->_weight; }
void feature_ht::Print() { std::cout<<"HT: "<< this->_count_ht  <<"/"<< this->_min_ht ; }
void feature_ht_top::Print() { feature_ht::Print(); if (this->_topEt!=0) std::cout<<" [top "<<this->_topEt<<"]"; }
void feature_invm::Print() { std::cout << "MJJ: " << this->_count_invm << "/" << this->_min_invm ; }
void feature_invm_CF::Print() { std::cout << "MJJ-CF: " << this->_count_invm << "/" << this->_min_invm ; }

//**********************************************************************

