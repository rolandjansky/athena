/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/PanTauSeed.h"

#include "PanTauAlgs/TauConstituent.h"

#include <iostream>

#include "xAODTau/TauJet.h"

// default constructor
PanTau::PanTauSeed2::PanTauSeed2()
  :
  IParticle(),
  m_p4(), m_p4Cached( false ),
  m_IsValidSeed(false),
  m_TechnicalQuality(),
  m_NameInputAlgorithm("InvalidAlg"),
  m_TauJet(0),
  m_Constituents(),
  m_TypeHLVs(),
  m_ConstituentsList_Core(),
  m_ConstituentsList_Wide(),
  m_ConstituentsList_AllSelected(),
  m_ConstituentsList_All(),
  m_DecayMode_BySubAlg(0),
  m_DecayMode_ByPanTau(0),
  m_decayModeHack_CellBasedShots(false),
  m_Features(0)
{
}



// destructor
PanTau::PanTauSeed2::~PanTauSeed2()
{
  if(m_Features != 0) delete m_Features;

  //delete the constituents                                                                                                                                                                                                                      
  for(unsigned int iConst=0; iConst<m_ConstituentsList_All.size(); iConst++) {
    PanTau::TauConstituent2* curConst = m_ConstituentsList_All[iConst];
    if(curConst != 0) delete curConst;
  }
  m_ConstituentsList_All.clear();
  m_ConstituentsList_AllSelected.clear();
  m_ConstituentsList_Core.clear();
  m_ConstituentsList_Wide.clear();

  //also clear the constituent matrix                                                                                                                                                                                                            
  //dont delete the entries as they point to the (at this point already deleted) constituents in m_ConstituentsList                                                                                                                              
  for(unsigned int iType=0; iType<m_Constituents.size(); iType++) {
    std::vector<PanTau::TauConstituent2*> curConsts = m_Constituents[iType];
    curConsts.clear();
  }
  m_Constituents.clear();

}


//copy constructor
PanTau::PanTauSeed2::PanTauSeed2(const PanTau::PanTauSeed2& rhs)
  :
  IParticle(rhs),
  m_p4(rhs.m_p4), m_p4Cached(rhs.m_p4Cached),
  m_IsValidSeed(rhs.m_IsValidSeed),
  m_TechnicalQuality(rhs.m_TechnicalQuality),
  m_NameInputAlgorithm(rhs.m_NameInputAlgorithm),
  m_TauJet(rhs.m_TauJet),
  m_Constituents(rhs.m_Constituents),
  m_TypeHLVs(rhs.m_TypeHLVs),
  m_ConstituentsList_Core(rhs.m_ConstituentsList_Core),
  m_ConstituentsList_Wide(rhs.m_ConstituentsList_Wide),
  m_ConstituentsList_AllSelected(rhs.m_ConstituentsList_AllSelected),
  m_ConstituentsList_All(rhs.m_ConstituentsList_All),
  m_DecayMode_BySubAlg(rhs.m_DecayMode_BySubAlg),
  m_DecayMode_ByPanTau(rhs.m_DecayMode_ByPanTau),
  m_decayModeHack_CellBasedShots(rhs.m_decayModeHack_CellBasedShots),
  // is this not set on purpose?
  //m_ProtoMomentum_Wide = seed.m_ProtoMomentum_Wide;
  //m_ProtoMomentum_Core = seed.m_ProtoMomentum_Core;
  //m_FinalMomentum      = seed.m_FinalMomentum;
  m_Features( (rhs.m_Features ? new PanTau::TauFeature2(*rhs.m_Features) : 0) )
{
}



PanTau::PanTauSeed2& PanTau::PanTauSeed2::operator=(const PanTau::PanTauSeed2& seed)
{
  if (this != &seed){

    if (!this->container() && !this->hasStore() ) {      
      makePrivateStore();
    }
    this->IParticle::operator=( seed );
    this->m_p4 = seed.m_p4;
    this->m_p4Cached = seed.m_p4Cached;
    m_IsValidSeed           = seed.m_IsValidSeed;
    m_TechnicalQuality      = seed.m_TechnicalQuality;
    m_NameInputAlgorithm    = seed.m_NameInputAlgorithm;
    m_TauJet                = seed.m_TauJet;
    m_Constituents          = seed.m_Constituents;
    m_TypeHLVs              = seed.m_TypeHLVs;
    m_ConstituentsList_Core = seed.m_ConstituentsList_Core;
    m_ConstituentsList_Wide = seed.m_ConstituentsList_Wide;
    m_ConstituentsList_AllSelected  = seed.m_ConstituentsList_AllSelected;
    m_ConstituentsList_All          = seed.m_ConstituentsList_All;
    m_decayModeHack_CellBasedShots  = seed.m_decayModeHack_CellBasedShots;
    // is this not set on purpose?
    //m_ProtoMomentum_Wide = seed.m_ProtoMomentum_Wide;
    //m_ProtoMomentum_Core = seed.m_ProtoMomentum_Core;
    //m_FinalMomentum      = seed.m_FinalMomentum;
    if(m_Features) delete m_Features;
    m_Features              = (seed.m_Features ? new PanTau::TauFeature2(*seed.m_Features) : 0);
  }
  return *this;
}



double PanTau::PanTauSeed2::pt() const {
  // static Accessor< float > acc( "pt" );
  // return acc( *this );
  return p4().Pt();
}

double PanTau::PanTauSeed2::eta() const {
  // static Accessor<float > acc( "eta" );
  // return acc( *this );
  return p4().Eta();
}

double PanTau::PanTauSeed2::phi() const {
  // static Accessor< float > acc( "phi" );
  // return acc( *this );
  return p4().Phi();
}

double PanTau::PanTauSeed2::m() const {
  // static Accessor< float> acc( "m" );
  // return acc( *this );
  return p4().M();
}

double PanTau::PanTauSeed2::e() const{
  return p4().E(); 
}

double PanTau::PanTauSeed2::rapidity() const {
  return p4().Rapidity(); 
}

const PanTau::PanTauSeed2::FourMom_t& PanTau::PanTauSeed2::p4() const {
  if( ! m_p4Cached ) {
    m_p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
    m_p4Cached=true;
  }
  return m_p4;
}

void PanTau::PanTauSeed2::setP4(float pt, float eta, float phi, float m){
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

void PanTau::PanTauSeed2::setPt(float pt){
  static Accessor< float > acc( "pt" );
  acc( *this ) = pt;
  //Need to recalculate m_p4 if requested after update
   m_p4Cached=false;
}

void PanTau::PanTauSeed2::setEta(float eta){
  static Accessor< float > acc( "eta" );
  acc( *this ) = eta;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;  
}

void PanTau::PanTauSeed2::setPhi(float phi){
  static Accessor< float > acc( "phi" );
  acc( *this ) = phi;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}

void PanTau::PanTauSeed2::setM(float m){
  static Accessor< float > acc( "m" );
  acc( *this ) = m;
  //Need to recalculate m_p4 if requested after update
  m_p4Cached=false;
}


xAOD::Type::ObjectType PanTau::PanTauSeed2::type() const {
  return xAOD::Type::Tau;
}



/** Main constructor to be used */
PanTau::PanTauSeed2::PanTauSeed2( std::string                             nameInputAlgorithm,
                                const xAOD::TauJet*                     tauJet,
                                std::vector<PanTau::TauConstituent2*>    tauConstituents,
                                std::vector<PanTau::TauConstituent2*>    tauConstituentsAll,
                                std::vector<int>                        pantauSeed_TechnicalQuality
				)
 :
  IParticle()
{

  // *This may be a bug!!! change to Set*Pt*EtaPhiM?? ***
// PanTau::SetP4EEtaPhiM( m_p4, tauJet->ptIntermediateAxis() * cosh( tauJet->etaIntermediateAxis() ), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis() );
  m_p4.SetPtEtaPhiM(tauJet->ptIntermediateAxis(), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis() );
  m_p4Cached=true;
  m_IsValidSeed           = true;
  m_TechnicalQuality      = pantauSeed_TechnicalQuality;
  m_NameInputAlgorithm    = nameInputAlgorithm;
  m_TauJet                = tauJet;
  m_decayModeHack_CellBasedShots = false;
  m_Features              = new PanTau::TauFeature2;

  //save a copy of the flat constituents list and also split into core/wide constituents                                                                                                                                                         
  m_ConstituentsList_All = tauConstituentsAll;
  m_ConstituentsList_AllSelected = tauConstituents; //all types are in here, this is the complete list                                                                                                                                           
  for(unsigned int iConst=0; iConst<tauConstituents.size(); iConst++) {
    bool isCoreChrg = tauConstituents[iConst]->isOfType(PanTau::TauConstituent2::t_Charged);
    bool isCoreNeut = tauConstituents[iConst]->isOfType(PanTau::TauConstituent2::t_Neutral);
    bool isCorePi0  = tauConstituents[iConst]->isOfType(PanTau::TauConstituent2::t_Pi0Neut);

    if(isCoreChrg == true || isCoreNeut == true || isCorePi0 == true) m_ConstituentsList_Core.push_back(tauConstituents[iConst]); // Core only contains the currently used objects                                                             

    bool isWideChrg = tauConstituents[iConst]->isOfType(PanTau::TauConstituent2::t_OutChrg);
    bool isWideNeut = tauConstituents[iConst]->isOfType(PanTau::TauConstituent2::t_OutNeut);
    if(isWideChrg == true || isWideNeut == true) m_ConstituentsList_Wide.push_back(tauConstituents[iConst]); // Wide contains objectsin 0.2-0.4                                                                                                
  }

  //create the constituents lists                                                                                                                                                                                                                
  for(int iType=0; iType<PanTau::TauConstituent2::t_nTypes; iType++) {
    m_Constituents.push_back( std::vector<PanTau::TauConstituent2*>(0) );
    m_TypeHLVs.push_back( TLorentzVector(0,0,0,0) );
  }

  //assign tauConstituents                                                                                                                                                                                                                       
  TLorentzVector hlv_SumConstituents_Wide = TLorentzVector(0,0,0,0);
  TLorentzVector hlv_SumConstituents_Core = TLorentzVector(0,0,0,0);

  int nCharged = 0;
  int nPi0Neut = 0;

  for(unsigned int iConst=0; iConst<tauConstituents.size(); iConst++) {
    PanTau::TauConstituent2* curConst = tauConstituents.at(iConst);
    std::vector<int>        curTypes = curConst->getTypeFlags();

    for(unsigned int curType=0; curType<curTypes.size(); curType++) {
      if(curTypes.at(curType) == 0) continue;

      if(curType == (int)PanTau::TauConstituent2::t_Charged) nCharged++;
      if(curType == (int)PanTau::TauConstituent2::t_Pi0Neut) nPi0Neut++;

      if((unsigned int)curType >= m_Constituents.size()) {
	//std::cout << "PanTau::PanTauSeed\tERROR\tMore types in TauConstituent2 than reserved in PanTau seed constituent matrix!" << std::endl;
	continue;
      }

      //fill into sorted lists                                                                                                                                                                                                               
      m_Constituents.at(curType).push_back(curConst);
      m_TypeHLVs.at(curType) += curConst->p4();

    }//end loop over types                                                                                                                                                                                                                     

    //add each constituent to wide proto momentum                                                                                                                                                                                              
    hlv_SumConstituents_Wide += curConst->p4();

    //add all charged and neutral constituents (i.e. from core region) to core proto momentum                                                                                                                                                  
    if(curConst->isOfType(PanTau::TauConstituent2::t_Charged)) {hlv_SumConstituents_Core += curConst->p4(); continue;}
    if(curConst->isOfType(PanTau::TauConstituent2::t_Pi0Neut)) {hlv_SumConstituents_Core += curConst->p4(); continue;}

  }//end loop over constituents                                                                                                                                                                                                                  

  m_ProtoMomentum_Wide = hlv_SumConstituents_Wide;
  m_ProtoMomentum_Core = hlv_SumConstituents_Core;

  //set mode as obtained from subalg                                                                                                                                                                                                             
  m_DecayMode_BySubAlg = PanTau::PanTauSeed2::getDecayMode(nCharged, nPi0Neut);
  m_DecayMode_ByPanTau = xAOD::TauJetParameters::Mode_Error;

  if(isOfTechnicalQuality(PanTau::PanTauSeed2::t_NoValidInputTau) == true) {
    m_DecayMode_ByPanTau = xAOD::TauJetParameters::Mode_NotSet;
  }

}


/** Constructor for invalid seeds */
PanTau::PanTauSeed2::PanTauSeed2( std::string                          nameInputAlgorithm,
                                const xAOD::TauJet*                  tauJet,
                                std::vector<int>                     pantauSeed_TechnicalQuality
				)
 :
  IParticle(),
  m_Constituents(),
  m_TypeHLVs(),
  m_ConstituentsList_AllSelected(),
  m_ConstituentsList_All(),
  m_DecayMode_BySubAlg(0),
  m_DecayMode_ByPanTau(0),
  m_decayModeHack_CellBasedShots(false)
{
  // *This may be a bug!!! change to Set*Pt*EtaPhiM?? ***
//  PanTau::SetP4EEtaPhiM( m_p4, tauJet->ptIntermediateAxis() * cosh( tauJet->etaIntermediateAxis() ), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis() );
  m_p4.SetPtEtaPhiM(tauJet->ptIntermediateAxis(), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis() );
  m_p4Cached=true;
  m_IsValidSeed           = false;
  m_TechnicalQuality      = pantauSeed_TechnicalQuality;
  m_NameInputAlgorithm    = nameInputAlgorithm;
  m_TauJet                = tauJet;
  m_Features              = new PanTau::TauFeature2;
}



int PanTau::PanTauSeed2::getDecayMode(int nCharged, int nNeutral) {

  int decayMode;
  // 1 Prong modes                                                                                                                                                                                                                               
  if(nCharged == 1 && nNeutral == 0) decayMode = (int)xAOD::TauJetParameters::Mode_1p0n;
  else if(nCharged == 1 && nNeutral == 1) decayMode = (int)xAOD::TauJetParameters::Mode_1p1n;
  else if(nCharged == 1 && nNeutral >  1) decayMode = (int)xAOD::TauJetParameters::Mode_1pXn;

  // 3 prong modes                                                                                                                                                                                                                               
  else if(nCharged == 3 && nNeutral == 0) decayMode = (int)xAOD::TauJetParameters::Mode_3p0n;
  else if(nCharged == 3 && nNeutral >  0) decayMode = (int)xAOD::TauJetParameters::Mode_3pXn;

  // other mode                                                                                                                                                                                                                                  
  else if(nCharged == 2) decayMode = (int)xAOD::TauJetParameters::Mode_Other;
  else if(nCharged == 4) decayMode = (int)xAOD::TauJetParameters::Mode_Other;
  else if(nCharged == 5) decayMode = (int)xAOD::TauJetParameters::Mode_Other;

  else if(nCharged == 0) decayMode = (int)xAOD::TauJetParameters::Mode_NotSet;
  else if(nCharged >= 6) decayMode = (int)xAOD::TauJetParameters::Mode_NotSet;

  else decayMode = (int)xAOD::TauJetParameters::Mode_Error;

  return decayMode;
}


std::string PanTau::PanTauSeed2::getDecayModeName(int decayMode) {

  xAOD::TauJetParameters::DecayMode mode = (xAOD::TauJetParameters::DecayMode)decayMode;

  if(mode == xAOD::TauJetParameters::Mode_1p0n) return "1p0n";
  if(mode == xAOD::TauJetParameters::Mode_1p1n) return "1p1n";
  if(mode == xAOD::TauJetParameters::Mode_1pXn) return "1pXn";
  if(mode == xAOD::TauJetParameters::Mode_3p0n) return "3p0n";
  if(mode == xAOD::TauJetParameters::Mode_3pXn) return "3pXn";
  if(mode == xAOD::TauJetParameters::Mode_Other) return "Other";
  if(mode == xAOD::TauJetParameters::Mode_Error) return "Error";
  if(mode == xAOD::TauJetParameters::Mode_NotSet) return "NotSet";
  return "InvalidMode!!!";
}



bool                                                PanTau::PanTauSeed2::isOfTechnicalQuality(int pantauSeed_TechnicalQuality) const {
  if(pantauSeed_TechnicalQuality > PanTau::PanTauSeed2::t_nTechnicalQualities) {
    //std::cout << "PanTauSeed\tERROR\tunknown technical quality value: " << pantauSeed_TechnicalQuality << ". Maximum allowed is " << PanTau::PanTauSeed2::t_nTechnicalQualities << "! Check PanTauAlgs/PanTauSeed.h" << std::endl;
    return false;
  }
  if(m_TechnicalQuality[pantauSeed_TechnicalQuality] == 1) return true;
  return false;
}




std::vector<PanTau::TauConstituent2*>    PanTau::PanTauSeed2::getConstituentsOfType(int tauConstituent_Type, bool& foundit) {
    if(tauConstituent_Type > PanTau::TauConstituent2::t_nTypes) {
        foundit = false;
        //std::cout << "PanTauSeed\tERROR\tunknown constituent type: " << tauConstituent_Type << " -> it's larger than the known types: " << PanTau::TauConstituent2::t_nTypes << std::endl;
        return std::vector<TauConstituent2*>(0);
    }
    foundit = true;
    if(tauConstituent_Type == (int)PanTau::TauConstituent2::t_NoType) return m_ConstituentsList_AllSelected;
    return m_Constituents.at(tauConstituent_Type);
}



int                                     PanTau::PanTauSeed2::getNumberOfConstituentsOfType(int tauConstituent_Type) {
    bool isOK = false;
    std::vector<PanTau::TauConstituent2*> consts = this->getConstituentsOfType(tauConstituent_Type, isOK);
    if(isOK) {
        return (int)consts.size();
    }
    return -1;
}


TLorentzVector                PanTau::PanTauSeed2::getSubsystemHLV(int tauConstituent_Type, bool& foundit) {
    if(tauConstituent_Type > PanTau::TauConstituent2::t_nTypes) {
        foundit = false;
        return TLorentzVector(0,0,0,0);
    }
    foundit = true;
    return m_TypeHLVs.at(tauConstituent_Type);
}




