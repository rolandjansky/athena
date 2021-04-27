/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/TauConstituent.h"

PanTau::TauConstituent::TauConstituent() :
  IParticle(),
  m_p4(), m_p4Cached( false ),
  m_TypeFlags(),
  m_BDTValue(PanTau::TauConstituent::DefaultBDTValue()),
  m_Charge(PanTau::TauConstituent::DefaultCharge()),
  m_PFOLink(0),
  m_Shots(),
  m_nPhotonsInShot(0)
{
}


PanTau::TauConstituent::TauConstituent(TLorentzVector   itsMomentum,
				       int              itsCharge,
				       std::vector<int> itsType,
				       double           itsBDTValue,
				       xAOD::PFO*       itsPFO) :
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


PanTau::TauConstituent::TauConstituent(const PanTau::TauConstituent& rhs) :
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


PanTau::TauConstituent::~TauConstituent()
{
  //delete the shot constituents
  for(unsigned int iShot=0; iShot<m_Shots.size(); iShot++) {
    PanTau::TauConstituent* curConst = m_Shots[iShot];
    delete curConst;
  }
}


PanTau::TauConstituent& PanTau::TauConstituent::operator=(const PanTau::TauConstituent& tauConst)
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


double PanTau::TauConstituent::pt() const {
  static const Accessor< float > acc( "pt" );
  return acc( *this );
}

double PanTau::TauConstituent::eta() const {
  static const Accessor<float > acc( "eta" );
  return acc( *this );
}

double PanTau::TauConstituent::phi() const {
  static const Accessor< float > acc( "phi" );
  return acc( *this );
}

double PanTau::TauConstituent::m() const {
  static const Accessor< float> acc( "m" );
  return acc( *this );
}

double PanTau::TauConstituent::e() const{
  return p4().E(); 
}

double PanTau::TauConstituent::rapidity() const {
  return p4().Rapidity(); 
}

PanTau::TauConstituent::FourMom_t PanTau::TauConstituent::p4() const {
  if( ! m_p4Cached ) {
    m_p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
    m_p4Cached=true;
  }
  return m_p4;
}

void PanTau::TauConstituent::setP4(float pt, float eta, float phi, float m){
  static const Accessor< float > acc1( "pt" );
  acc1( *this ) = pt;
  static const Accessor< float > acc2( "eta" );
  acc2( *this ) = eta;
  static const Accessor< float > acc3( "phi" );
  acc3( *this ) = phi;
  static const Accessor< float > acc4( "m" );
  acc4( *this ) = m;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent::setPt(float pt){
  static const Accessor< float > acc( "pt" );
  acc( *this ) = pt;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent::setEta(float eta){
  static const Accessor< float > acc( "eta" );
  acc( *this ) = eta;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent::setPhi(float phi){
  static const Accessor< float > acc( "phi" );
  acc( *this ) = phi;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::TauConstituent::setM(float m){
  static const Accessor< float > acc( "m" );
  acc( *this ) = m;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}


xAOD::Type::ObjectType PanTau::TauConstituent::type() const {
  return xAOD::Type::ParticleFlow;
}


void PanTau::TauConstituent::removeTypeFlag(TauConstituent::Type aType) {
  unsigned int typeIndex = (unsigned int)aType;
  m_TypeFlags.at(typeIndex) = 0;
  return;
}


//the static getTypeName which does a translation
std::string  PanTau::TauConstituent::getTypeName(PanTau::TauConstituent::Type aType) {
  switch(aType) {
  case PanTau::TauConstituent::t_Charged: return "Charged";
  case PanTau::TauConstituent::t_Neutral: return "Neutral";
  case PanTau::TauConstituent::t_Pi0Neut: return "Pi0Neut";
  case PanTau::TauConstituent::t_OutChrg: return "OuterChrg";
  case PanTau::TauConstituent::t_OutNeut: return "OuterNeut";
  case PanTau::TauConstituent::t_NeutLowA: return "NeutLowA";
  case PanTau::TauConstituent::t_NeutLowB: return "NeutLowB";
  case PanTau::TauConstituent::t_NoType: return "All";
  default: return "UnkownType";
  }
}


bool PanTau::TauConstituent::isNeutralType(int tauConstituentType) {
  PanTau::TauConstituent::Type type = (PanTau::TauConstituent::Type)tauConstituentType;
  switch(type) {
  case PanTau::TauConstituent::t_Neutral: return true;
  case PanTau::TauConstituent::t_Pi0Neut: return true;
  case PanTau::TauConstituent::t_OutNeut: return true;
  case PanTau::TauConstituent::t_NeutLowA: return true;
  case PanTau::TauConstituent::t_NeutLowB: return true;
  default: return false;
  }
  return false;
}


bool PanTau::TauConstituent::isCoreType(int tauConstituentType) {
  PanTau::TauConstituent::Type type = (PanTau::TauConstituent::Type)tauConstituentType;
  switch(type) {
  case PanTau::TauConstituent::t_Charged: return true;
  case PanTau::TauConstituent::t_Neutral: return true;
  case PanTau::TauConstituent::t_Pi0Neut: return true;
  case PanTau::TauConstituent::t_OutNeut: return false;
  case PanTau::TauConstituent::t_OutChrg: return false;
  case PanTau::TauConstituent::t_NeutLowA: return true;
  case PanTau::TauConstituent::t_NeutLowB: return true;
  default: return false;
  }
  return false;
}


//the non static getType name, which returns
std::vector<std::string> PanTau::TauConstituent::getTypeName() const {
  std::vector<std::string> res;
  for(unsigned int iType=0; iType<TauConstituent::t_nTypes; iType++) {
    if(m_TypeFlags[iType] == 1) {
      PanTau::TauConstituent::Type curType = (PanTau::TauConstituent::Type)iType;
      res.push_back( TauConstituent::getTypeName(curType) );
    }
  }
  return res;
}


std::string PanTau::TauConstituent::getTypeNameString() const {
  std::string res;
  for(unsigned int iType=0; iType<m_TypeFlags.size(); iType++) {
    if(m_TypeFlags[iType] == 1) {
      res += PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)(iType)) + ",";
    }
  }
  return res;
}


double PanTau::TauConstituent::getBDTValue() const {
  return m_BDTValue;
}


std::vector<int> PanTau::TauConstituent::getTypeFlags() const {
  return m_TypeFlags;
}


bool PanTau::TauConstituent::isOfType(PanTau::TauConstituent::Type theType) const {
  int typeIndex = (int)theType;
  if(theType >= (int)TauConstituent::t_nTypes) {
    return false;
  }
  if(m_TypeFlags.at(typeIndex) == 1) return true;
  return false;
}


int PanTau::TauConstituent::getCharge() const {
  return m_Charge;
}


xAOD::PFO* PanTau::TauConstituent::getPFO() const {
  return m_PFOLink;
}


void PanTau::TauConstituent::addShot(TauConstituent* shot) {
  if(shot != 0) m_Shots.push_back(shot);
}


std::vector<PanTau::TauConstituent*> PanTau::TauConstituent::getShots() {
  return m_Shots;
}


unsigned int PanTau::TauConstituent::getNShots() {
  return m_Shots.size();
}


void PanTau::TauConstituent::setNPhotonsInShot(int nPhotons) {
  m_nPhotonsInShot = nPhotons;
}


int PanTau::TauConstituent::getNPhotonsInShot() {
  return m_nPhotonsInShot;
}
