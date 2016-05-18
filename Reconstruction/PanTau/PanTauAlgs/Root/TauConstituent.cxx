/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/TauConstituent.h"

#include <iostream>

PanTau::TauConstituent2::TauConstituent2()
  :
  IParticle(),
  m_p4(), m_p4Cached( false ),
  m_TypeFlags(),
  m_BDTValue(PanTau::TauConstituent2::DefaultBDTValue()),
  m_Charge(PanTau::TauConstituent2::DefaultCharge()),
  m_PFOLink(0),
  m_Shots(),
  m_nPhotonsInShot(0)
{
}



PanTau::TauConstituent2::TauConstituent2(TLorentzVector   itsMomentum,
					 int              itsCharge,
					 std::vector<int> itsType,
					 double           itsBDTValue,
					 xAOD::PFO*       itsPFO
					 )
  :
  IParticle(),
  m_p4(itsMomentum), m_p4Cached(true),
  m_TypeFlags(itsType),
  m_BDTValue(itsBDTValue),
  m_Charge(itsCharge),
  m_PFOLink(itsPFO),
  m_Shots(),
  m_nPhotonsInShot(0)
{
}



PanTau::TauConstituent2::TauConstituent2(
					 const PanTau::TauConstituent2& rhs
					 ):
  IParticle(rhs),
  m_p4(rhs.m_p4), m_p4Cached(rhs.m_p4Cached),
  m_TypeFlags(rhs.m_TypeFlags),
  m_BDTValue(rhs.m_BDTValue),
  m_Charge(rhs.m_Charge),
  m_PFOLink(rhs.m_PFOLink),
  m_Shots(rhs.m_Shots),
  m_nPhotonsInShot(rhs.m_nPhotonsInShot)
{
}



PanTau::TauConstituent2::~TauConstituent2()
{
  //delete the shot constituents
  for(unsigned int iShot=0; iShot<m_Shots.size(); iShot++) {
    PanTau::TauConstituent2* curConst = m_Shots[iShot];
    delete curConst;
  }
}



PanTau::TauConstituent2& PanTau::TauConstituent2::operator=(const PanTau::TauConstituent2& tauConst)
{
  if (this!=&tauConst){

    if (!this->container() && !this->hasStore() ) {      
    	makePrivateStore();
    }
    this->IParticle::operator=( tauConst );
    this->m_p4 = tauConst.m_p4;
    this->m_p4Cached = tauConst.m_p4Cached;
    m_TypeFlags = tauConst.m_TypeFlags;
    m_BDTValue  = tauConst.m_BDTValue;
    m_Charge    = tauConst.m_Charge;
    m_PFOLink   = tauConst.m_PFOLink;
    m_Shots     = tauConst.m_Shots;
    m_nPhotonsInShot = tauConst.m_nPhotonsInShot;
  }
  return *this;
}


double PanTau::TauConstituent2::pt() const {
  static Accessor< float > acc( "pt" );
  return acc( *this );
}

double PanTau::TauConstituent2::eta() const {
  static Accessor<float > acc( "eta" );
  return acc( *this );
}

double PanTau::TauConstituent2::phi() const {
  static Accessor< float > acc( "phi" );
  return acc( *this );
}

double PanTau::TauConstituent2::m() const {
  static Accessor< float> acc( "m" );
  return acc( *this );
}

double PanTau::TauConstituent2::e() const{
  return p4().E(); 
}

double PanTau::TauConstituent2::rapidity() const {
  return p4().Rapidity(); 
}

const PanTau::TauConstituent2::FourMom_t& PanTau::TauConstituent2::p4() const {
  if( ! m_p4Cached ) {
    m_p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
    m_p4Cached=true;
  }
  return m_p4;
}

void PanTau::TauConstituent2::setP4(float pt, float eta, float phi, float m){
  static Accessor< float > acc1( "pt" );
  acc1( *this ) = pt;
  static Accessor< float > acc2( "eta" );
  acc2( *this ) = eta;
  static Accessor< float > acc3( "phi" );
  acc3( *this ) = phi;
  static Accessor< float > acc4( "m" );
  acc4( *this ) = m;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent2::setPt(float pt){
  static Accessor< float > acc( "pt" );
  acc( *this ) = pt;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent2::setEta(float eta){
  static Accessor< float > acc( "eta" );
  acc( *this ) = eta;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent2::setPhi(float phi){
  static Accessor< float > acc( "phi" );
  acc( *this ) = phi;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent2::setM(float m){
  static Accessor< float > acc( "m" );
  acc( *this ) = m;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}


xAOD::Type::ObjectType PanTau::TauConstituent2::type() const {
  return xAOD::Type::ParticleFlow;
}




void             PanTau::TauConstituent2::removeTypeFlag(TauConstituent2::Type aType) {
  unsigned int typeIndex = (unsigned int)aType;
  m_TypeFlags.at(typeIndex) = 0;
  return;
}


//the static getTypeName which does a translation
std::string      PanTau::TauConstituent2::getTypeName(PanTau::TauConstituent2::Type aType) {
  switch(aType) {
  case PanTau::TauConstituent2::t_Charged: return "Charged";
  case PanTau::TauConstituent2::t_Neutral: return "Neutral";
  case PanTau::TauConstituent2::t_Pi0Neut: return "Pi0Neut";
  case PanTau::TauConstituent2::t_OutChrg: return "OuterChrg";
  case PanTau::TauConstituent2::t_OutNeut: return "OuterNeut";
  case PanTau::TauConstituent2::t_NeutLowA: return "NeutLowA";
  case PanTau::TauConstituent2::t_NeutLowB: return "NeutLowB";
  case PanTau::TauConstituent2::t_NoType: return "All";
  default: return "UnkownType";
  }
}



bool         PanTau::TauConstituent2::isNeutralType(int tauConstituentType) {
  PanTau::TauConstituent2::Type type = (PanTau::TauConstituent2::Type)tauConstituentType;
  switch(type) {
  case PanTau::TauConstituent2::t_Neutral: return true;
  case PanTau::TauConstituent2::t_Pi0Neut: return true;
  case PanTau::TauConstituent2::t_OutNeut: return true;
  case PanTau::TauConstituent2::t_NeutLowA: return true;
  case PanTau::TauConstituent2::t_NeutLowB: return true;
  default: return false;
  }
  return false;
}



bool         PanTau::TauConstituent2::isCoreType(int tauConstituentType) {
  PanTau::TauConstituent2::Type type = (PanTau::TauConstituent2::Type)tauConstituentType;
  switch(type) {
  case PanTau::TauConstituent2::t_Charged: return true;
  case PanTau::TauConstituent2::t_Neutral: return true;
  case PanTau::TauConstituent2::t_Pi0Neut: return true;
  case PanTau::TauConstituent2::t_OutNeut: return false;
  case PanTau::TauConstituent2::t_OutChrg: return false;
  case PanTau::TauConstituent2::t_NeutLowA: return true;
  case PanTau::TauConstituent2::t_NeutLowB: return true;
  default: return false;
  }
  return false;
}



//the non static getType name, which returns
std::vector<std::string>            PanTau::TauConstituent2::getTypeName() const {
  std::vector<std::string> res;
  for(unsigned int iType=0; iType<TauConstituent2::t_nTypes; iType++) {
    if(m_TypeFlags[iType] == 1) {
      PanTau::TauConstituent2::Type curType = (PanTau::TauConstituent2::Type)iType;
      res.push_back( TauConstituent2::getTypeName(curType) );
    }
  }
  return res;
}



std::string                         PanTau::TauConstituent2::getTypeNameString() const {
  std::string res;
  for(unsigned int iType=0; iType<m_TypeFlags.size(); iType++) {
    if(m_TypeFlags[iType] == 1) {
      res += PanTau::TauConstituent2::getTypeName((PanTau::TauConstituent2::Type)(iType)) + ",";
    }
  }
  return res;
}



double PanTau::TauConstituent2::getBDTValue() const {
  return m_BDTValue;
}



std::vector<int> PanTau::TauConstituent2::getTypeFlags() const {
  return m_TypeFlags;
}


bool                                      PanTau::TauConstituent2::isOfType(PanTau::TauConstituent2::Type theType) const {
  int typeIndex = (int)theType;
  if(theType >= (int)TauConstituent2::t_nTypes) {
    return false;
  }
  if(m_TypeFlags.at(typeIndex) == 1) return true;
  return false;
}


int PanTau::TauConstituent2::getCharge() const {
  return m_Charge;
}



xAOD::PFO* PanTau::TauConstituent2::getPFO() const {
  return m_PFOLink;
}



void                      PanTau::TauConstituent2::addShot(TauConstituent2* shot) {
  if(shot != 0) m_Shots.push_back(shot);
}


std::vector<PanTau::TauConstituent2*>            PanTau::TauConstituent2::getShots() {
  return m_Shots;
}

unsigned int                PanTau::TauConstituent2::getNShots() {
  return m_Shots.size();
}

void                        PanTau::TauConstituent2::setNPhotonsInShot(int nPhotons) {
  m_nPhotonsInShot = nPhotons;
}
int                         PanTau::TauConstituent2::getNPhotonsInShot() {
  return m_nPhotonsInShot;
}

