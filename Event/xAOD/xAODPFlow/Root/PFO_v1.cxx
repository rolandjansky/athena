/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "AthLinks/ElementLink.h"


// Local include(s):
#include "xAODPFlow/versions/PFO_v1.h"
#include "xAODPFlow/versions/PFOAttributesAccessor_v1.h"

namespace xAOD {

  struct PFOParticleTypeMapper_temp {
    static const SG::AuxElement::Accessor<std::vector<ElementLink<IParticleContainer > > >* getAccessor(PFODetails::PFOParticleType type) {
      switch (type){
      case PFODetails::CaloCluster:
	const static SG::AuxElement::Accessor<std::vector<ElementLink<IParticleContainer > > > acc_cc("pfo_ClusterLinks");
	return &acc_cc;
      case PFODetails::Track:
	const static SG::AuxElement::Accessor<std::vector<ElementLink<IParticleContainer > > > acc_tp("pfo_TrackLinks");
	return &acc_tp;
	// Note - clients should NEVER set both links to TauShot and HadroniCaloCluster - Tau WG agreed they NEVER will do such a thing (and they are only client)
      case PFODetails::TauShot:
      case PFODetails::HadronicCalo:
	const static SG::AuxElement::Accessor<std::vector<ElementLink<IParticleContainer > > > acc_ts("pfo_TauShotLinks");
	return &acc_ts;
      case PFODetails::ChargedPFO:
	const static SG::AuxElement::Accessor<std::vector<ElementLink<IParticleContainer > > > acc_cpfo("pfo_Charged");
	return &acc_cpfo;
      case PFODetails::NeutralPFO:
	const static SG::AuxElement::Accessor<std::vector<ElementLink<IParticleContainer > > > acc_npfo("pfo_Neutral");
	return &acc_npfo;
      case PFODetails::TauTrack:
	const static SG::AuxElement::Accessor<std::vector<ElementLink<IParticleContainer > > > acc_tt("pfo_TauTrack");
	return &acc_tt;	
      }//switch
      return NULL;
    }//getAccessor
  };

  PFO_v1::PFO_v1() : IParticle() {
  }

  PFO_v1::PFO_v1(const PFO_v1& other) : IParticle(){
    this->makePrivateStore(other);
  }


   double PFO_v1::pt() const {

     const static ConstAccessor<float> accPt("pt");
     float pt = accPt(*this);

     return pt;
   }

   double PFO_v1::eta() const {

     const static ConstAccessor<float> accEta("eta");
     float eta = accEta(*this);
     return eta;
   }

   double PFO_v1::phi() const {

     const static ConstAccessor<float> accPhi("phi");
     float phi = accPhi(*this);
     return phi;
   }

   double PFO_v1::m() const {

     const static ConstAccessor<float> accM("m");
     float M = accM(*this);
     return M;
   }

   double PFO_v1::e() const {

     const static ConstAccessor<float> accPt("pt");
     float pt = accPt(*this);

     if (pt < 0.0) return -genvecP4().E();
     else return genvecP4().E();
   }

   double PFO_v1::rapidity() const {

     return genvecP4().Rapidity();
   }

   PFO_v1::FourMom_t PFO_v1::p4() const {
     FourMom_t p4;
     p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
     return p4;	
   }

   PFO_v1::GenVecFourMom_t PFO_v1::genvecP4() const {
     return GenVecFourMom_t( pt(), eta(), phi(),m()); 
   }
  

  /// set the 4-vec
  void PFO_v1::setP4(const FourMom_t& vec) {

    const static Accessor<float> accPt("pt");
    accPt(*this) = vec.Pt();

    const static Accessor<float> accEta("eta");
    accEta(*this) = vec.Eta();

    const static Accessor<float> accPhi("phi");
    accPhi(*this) = vec.Phi();

    const static Accessor<float> accM("m");
    accM(*this) = vec.M();

  }
	 
  /// set the 4-vec
  void PFO_v1::setP4(float pt, float eta, float phi, float m) {
    
    const static Accessor<float> accPt("pt");
    accPt(*this) = pt;

    const static Accessor<float> accEta("eta");
    accEta(*this) = eta;

    const static Accessor<float> accPhi("phi");
    accPhi(*this) = phi;

    const static Accessor<float> accM("m");
    accM(*this) = m;
  }
  

   Type::ObjectType PFO_v1::type() const {
      return Type::ParticleFlow;
   }

  PFO_v1::FourMom_t PFO_v1::p4EM() const { 

    if (fabs(this->charge()) > FLT_MIN) return p4();

    FourMom_t p4EM;

    //change to use pt, eta, phi ,e 
    const static ConstAccessor<float> accPt("ptEM");
    const static ConstAccessor<float> accEta("eta");
    const static ConstAccessor<float> accPhi("phi");
    const static ConstAccessor<float> accM("mEM");
    
    p4EM.SetPtEtaPhiM(accPt(*this), accEta(*this), accPhi(*this), accM(*this));
    return p4EM;
  }

  PFO_v1::GenVecFourMom_t PFO_v1::genvecP4EM() const { 

    if (fabs(this->charge()) > FLT_MIN) return genvecP4();

    //change to use pt, eta, phi ,e 
    const static ConstAccessor<float> accPt("ptEM");
    const static ConstAccessor<float> accEta("eta");
    const static ConstAccessor<float> accPhi("phi");
    const static ConstAccessor<float> accM("mEM");
    
    return GenVecFourMom_t(accPt(*this), accEta(*this), accPhi(*this), accM(*this));
  }

  void PFO_v1::setP4EM(const FourMom_t& p4EM) {
    const static Accessor<float> accPt("ptEM");
    accPt(*this) = p4EM.Pt();

    const static Accessor<float> accEta("eta");
    accEta(*this) = p4EM.Eta();

    const static Accessor<float> accPhi("phi");
    accPhi(*this) = p4EM.Phi();

    const static Accessor<float> accM("mEM");
    accM(*this) = p4EM.M();

  }

  void PFO_v1::setP4EM(float pt, float eta, float phi, float m) {
    
    const static Accessor<float> accPt("ptEM");
    accPt(*this) = pt;

    const static Accessor<float> accEta("eta");
    accEta(*this) = eta;

    const static Accessor<float> accPhi("phi");
    accPhi(*this) = phi;

    const static Accessor<float> accM("mEM");
    accM(*this) = m;

  }

   double PFO_v1::ptEM() const {

     if (fabs(this->charge()) > FLT_MIN) return this->pt();

     const static ConstAccessor<float> accPt("ptEM");
     float pt = accPt(*this);

     return pt;
   }

   // JM: this seem to be idetical to eta(), right?
   double PFO_v1::etaEM() const {     
     return eta();
   }

   // JM: this seem to be idetical to phi(), right?
   double PFO_v1::phiEM() const {
     return phi();
   }

   double PFO_v1::mEM() const {

     if (fabs(this->charge()) > FLT_MIN) return this->m();
     const static ConstAccessor<float> accM("mEM");
     float M = accM(*this);

     return M;
   }

   double PFO_v1::eEM() const {

     if (fabs(this->charge()) > FLT_MIN) return this->e();

     const static ConstAccessor<float> accPt("ptEM");
     float pt = accPt(*this);

     if (pt < 0.0) return -p4EM().E();
     else return p4EM().E();
   }


  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(PFO_v1, float, bdtPi0Score, setBDTPi0Score)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(PFO_v1, float, centerMag, setCenterMag)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(PFO_v1, float, charge, setCharge)

  bool PFO_v1::isCharged() const{
     return (fabs(this->charge())>FLT_MIN);
  }

  /** specaial implementations for floats, for eflowRec JetETMiss variables, to reduce disk space usage */

  template<> void PFO_v1::setAttribute(const std::string& AttributeType, const xAOD::PFODetails::PFOLeptonType& anAttribute){
    uint16_t uint16_variable = static_cast<uint16_t>(anAttribute);
    this->setAttribute<uint16_t>(AttributeType, uint16_variable);
  }

  template<> bool PFO_v1::attribute(const std::string& AttributeType, xAOD::PFODetails::PFOLeptonType& anAttribute) const {
    bool isValid = false;
    uint16_t internalAttribute;
    isValid = attribute<uint16_t>(AttributeType,internalAttribute);
    if (false == isValid) return false;
    else{
      anAttribute = static_cast<xAOD::PFODetails::PFOLeptonType>(internalAttribute);
      return true;
    }
    
  }

  template<> void PFO_v1::setAttribute(PFODetails::PFOAttributes AttributeType, const float& anAttribute) {
    if (this->isJetETMissFloatForCompression(AttributeType)){
      float dummy = anAttribute*s_floatCompressionFactor;
      const static int maxIntSize = 1000000000;
      int internalAttribute = maxIntSize;
      if (dummy < 0) internalAttribute *= -1;//if we had a large -ve energy, then we should set the max size to a -ve value         
      if (dummy < maxIntSize && dummy > -maxIntSize) internalAttribute = static_cast<int>(dummy);
      setAttribute<int>(AttributeType, internalAttribute);
    }
    else {
      float internalAttribute = anAttribute;
      (*(PFOAttributesAccessor_v1<float>::accessor(AttributeType)))(*this) = internalAttribute;
    }
  }

  template<> bool PFO_v1::attribute(PFODetails::PFOAttributes AttributeType, float& anAttribute) const {
    bool isValid = false;
    if (this->isJetETMissFloatForCompression(AttributeType)){
      int internalAttribute;
      isValid = attribute<int>(AttributeType,internalAttribute);
      if (true == isValid && 0 != internalAttribute) anAttribute = static_cast<float>(internalAttribute)/s_floatCompressionFactor;
      else anAttribute = 0.0;
      return isValid;
    }
    else{
      const ConstAccessor<float>* p_acc = PFOAttributesAccessor_v1<float>::constAccessor(AttributeType);
      //check if accessor pointer is NULL
      if( ! p_acc ) {  return false ;}
      //check if variable is avaialable
      if( ! p_acc->isAvailable( *this ) ) return false;
      //set variable and return true
      anAttribute =( *p_acc )( *this );
      return true;
       
    }
    return isValid;
  }
  
  bool PFO_v1::isJetETMissFloatForCompression(PFODetails::PFOAttributes AttributeType) const{
    switch (AttributeType) {
    case PFODetails::PFOAttributes::eflowRec_LATERAL:
    case PFODetails::PFOAttributes::eflowRec_LONGITUDINAL:
    case PFODetails::PFOAttributes::eflowRec_SECOND_R:
    case PFODetails::PFOAttributes::eflowRec_CENTER_LAMBDA:
    case PFODetails::PFOAttributes::eflowRec_FIRST_ENG_DENS:
    case PFODetails::PFOAttributes::eflowRec_ENG_FRAC_MAX:
    case PFODetails::PFOAttributes::eflowRec_ISOLATION:
    case PFODetails::PFOAttributes::eflowRec_ENG_BAD_CELLS:
    case PFODetails::PFOAttributes::eflowRec_N_BAD_CELLS:
    case PFODetails::PFOAttributes::eflowRec_BADLARQ_FRAC:
    case PFODetails::PFOAttributes::eflowRec_ENG_POS:
    case PFODetails::PFOAttributes::eflowRec_SIGNIFICANCE:
    case PFODetails::PFOAttributes::eflowRec_AVG_LAR_Q:
    case PFODetails::PFOAttributes::eflowRec_AVG_TILE_Q:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EM:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EM3:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC0:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_Tile0:
    case PFODetails::PFOAttributes::eflowRec_TIMING:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_PreSamplerB:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB3:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_PreSamplerE:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME3:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC3:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar0:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap3:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt0:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL0:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL0:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL1:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL2:
    case PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL3:
      return true;
    default:
      return false;
    }
  }


  /** special implementations for doubles to prevent user from putting doubles in the aux store - convert to float in this case */

  

  template<> void PFO_v1::setAttribute(PFODetails::PFOAttributes AttributeType, const double& anAttribute) {
    float internalAttribute = static_cast<float>(anAttribute);
    setAttribute<float>(AttributeType, internalAttribute);
  }

  template<> bool PFO_v1::attribute(PFODetails::PFOAttributes AttributeType, double& anAttribute) const {

    float internalAttribute;
    bool isValid = attribute<float>(AttributeType,internalAttribute);
    if (true == isValid) anAttribute = static_cast<double>(internalAttribute);
    return isValid;

  }

  template<> void PFO_v1::setAttribute(const std::string& AttributeType, const double& anAttribute) {

    float internalAttribute = static_cast<float>(anAttribute);
    setAttribute<float>(AttributeType, internalAttribute);

  }

  template<> bool PFO_v1::attribute(const std::string& AttributeType, double& anAttribute) const {

    float internalAttribute;
    bool isValid = attribute<float>(AttributeType,internalAttribute);
    if (true == isValid) anAttribute = static_cast<float>(internalAttribute);
    return isValid;

  }

  bool PFO_v1::getClusterMoment(float& theMoment, xAOD::CaloCluster::MomentType momentType) const {

    const xAOD::CaloCluster *theCluster = this->cluster(0);

    double myMoment = -99.0;

    if (theCluster){
      bool gotMoment = theCluster->retrieveMoment( momentType, myMoment);
      if (!gotMoment) return false;
      else if (gotMoment) {
	theMoment = static_cast<float>(myMoment);
	return true;
      }
    }
    else{
      xAOD::PFODetails::PFOAttributes myAttribute;
      bool haveGotAttributeInMap = this->getAttributeName_FromClusterMoment(myAttribute,momentType);
      if (false == haveGotAttributeInMap) return false;
      else {
	bool isRetrieved = this->attribute(myAttribute, theMoment);
	return isRetrieved;
      }
    }
    return false;

  }

  bool PFO_v1::getAttributeName_FromClusterMoment(xAOD::PFODetails::PFOAttributes& myAttribute, xAOD::CaloCluster::MomentType& momentType) const{

    if (momentType ==  xAOD::CaloCluster::ENG_FRAC_CORE) {
      myAttribute = xAOD::PFODetails::eflowRec_ENG_FRAC_CORE;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::FIRST_ENG_DENS) {
      myAttribute = xAOD::PFODetails::eflowRec_FIRST_ENG_DENS;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::CENTER_LAMBDA) {
      myAttribute = xAOD::PFODetails::eflowRec_CENTER_LAMBDA;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::SECOND_R) {
      myAttribute = xAOD::PFODetails::eflowRec_SECOND_R;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::DELTA_ALPHA) {
      myAttribute = xAOD::PFODetails::eflowRec_DELTA_ALPHA;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::LATERAL) {
      myAttribute = xAOD::PFODetails::eflowRec_LATERAL;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::LONGITUDINAL) {
      myAttribute = xAOD::PFODetails::eflowRec_LONGITUDINAL;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::SECOND_LAMBDA) {
      myAttribute = xAOD::PFODetails::eflowRec_SECOND_LAMBDA;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::ISOLATION) {
      myAttribute = xAOD::PFODetails::eflowRec_ISOLATION;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::ENG_FRAC_MAX) {
      myAttribute = xAOD::PFODetails::eflowRec_ENG_FRAC_MAX;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::ENG_BAD_CELLS) {
      myAttribute = xAOD::PFODetails::eflowRec_ENG_BAD_CELLS;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::N_BAD_CELLS) {
      myAttribute = xAOD::PFODetails::eflowRec_N_BAD_CELLS;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::BADLARQ_FRAC) {
      myAttribute = xAOD::PFODetails::eflowRec_BADLARQ_FRAC;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::ENG_POS) {
      myAttribute = xAOD::PFODetails::eflowRec_ENG_POS;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::SIGNIFICANCE) {
      myAttribute = xAOD::PFODetails::eflowRec_SIGNIFICANCE;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::CELL_SIGNIFICANCE) {
      myAttribute = xAOD::PFODetails::eflowRec_CELL_SIGNIFICANCE;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::CELL_SIG_SAMPLING) {
      myAttribute = xAOD::PFODetails::eflowRec_CELL_SIG_SAMPLING;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::AVG_LAR_Q) {
      myAttribute = xAOD::PFODetails::eflowRec_AVG_LAR_Q;
      return true;
    }
    else if (momentType ==  xAOD::CaloCluster::AVG_TILE_Q) {
      myAttribute = xAOD::PFODetails::eflowRec_AVG_TILE_Q;
      return true;
    }
    else if (momentType == xAOD::CaloCluster::EM_PROBABILITY){
      myAttribute = xAOD::PFODetails::eflowRec_EM_PROBABILITY;
      return true;
    }    
    else if (momentType == xAOD::CaloCluster::ENG_CALIB_TOT){
      myAttribute = xAOD::PFODetails::eflowRec_ENG_CALIB_TOT;
      return true;
    }
    else if (momentType == xAOD::CaloCluster::ENG_CALIB_FRAC_EM){
      myAttribute = xAOD::PFODetails::eflowRec_ENG_CALIB_FRAC_EM;
      return true;
    }
    else if (momentType == xAOD::CaloCluster::ENG_CALIB_FRAC_HAD){
      myAttribute = xAOD::PFODetails::eflowRec_ENG_CALIB_FRAC_HAD;
      return true;
    }
    else if (momentType == xAOD::CaloCluster::ENG_CALIB_FRAC_REST){
      myAttribute = xAOD::PFODetails::eflowRec_ENG_CALIB_FRAC_REST;
      return true;
    }

    else return false;

  }

  bool PFO_v1::setVertexLink(const ElementLink< xAOD::VertexContainer>& theVertexLink){
    const static Accessor<ElementLink<xAOD::VertexContainer> > acc("pfo_vertex");
    acc(*this) = theVertexLink;
    acc(*this).toPersistent();
    return true;
  }

  bool PFO_v1::setTrackLink(const ElementLink<xAOD::TrackParticleContainer>& theTrack){
    ElementLink< xAOD::IParticleContainer > myTrackLink;
    this->convertLink(myTrackLink,theTrack);
    return this->setAssociatedParticleLink(PFODetails::Track, myTrackLink);
  }

  bool PFO_v1::setClusterLink(const ElementLink<xAOD::CaloClusterContainer>& theCluster){
     ElementLink< xAOD::IParticleContainer > myClusterLink;
     this->convertLink(myClusterLink,theCluster);
     return this->setAssociatedParticleLink(PFODetails::CaloCluster, myClusterLink);
  }

  bool PFO_v1::addClusterLink(const ElementLink<xAOD::CaloClusterContainer>& theCluster){
    ElementLink< xAOD::IParticleContainer > myClusterLink;
    this->convertLink(myClusterLink,theCluster);
    return this->addAssociatedParticleLink(PFODetails::CaloCluster, myClusterLink);
  }

  bool PFO_v1::setAssociatedParticleLink(PFODetails::PFOParticleType ParticleType, const ElementLink<IParticleContainer>& theParticle){
    std::vector<ElementLink<xAOD::IParticleContainer> > theLinks(1,theParticle);
    return this->setAssociatedParticleLinks(ParticleType,theLinks);
  }

  void PFO_v1::setAssociatedParticleLink(const std::string& ParticleType, const ElementLink<IParticleContainer>& theParticle){
    std::vector<ElementLink<xAOD::IParticleContainer> > theLinks(1,theParticle);
    this->setAssociatedParticleLinks(ParticleType,theLinks);
  }

  bool PFO_v1::addAssociatedParticleLink(PFODetails::PFOParticleType ParticleType,  const ElementLink<IParticleContainer>& theParticle) {
    const Accessor<std::vector<ElementLink<IParticleContainer > > >* p_acc = PFOParticleTypeMapper_temp::getAccessor(ParticleType);
    if (!p_acc) return false;
    else{
      if (!p_acc->isAvailable(*this)) return false;
      else{
	std::vector<ElementLink<IParticleContainer> > storedContainer = (*p_acc)(*this);
	storedContainer.push_back(theParticle);
	(*p_acc)(*this) = storedContainer;
	return true;
      }
    }
  }

  void PFO_v1::addAssociatedParticleLink(const std::string& ParticleType, const ElementLink<IParticleContainer>& theParticle) {

    const static Accessor<std::vector<ElementLink<IParticleContainer > > > acc(ParticleType);
    std::vector<ElementLink<IParticleContainer> > storedContainer = acc(*this);

    ElementLink<xAOD::IParticleContainer> newLink;
    newLink.setElement(*theParticle);
    newLink.setStorableObject(theParticle.getStorableObjectRef());
    newLink.toPersistent();
    storedContainer.push_back(newLink);

    acc(*this) = storedContainer;
    
  }
  
  bool PFO_v1::setAssociatedParticleLinks(PFODetails::PFOParticleType ParticleType,  const std::vector<ElementLink<IParticleContainer> >& theParticles) {

    const Accessor<std::vector<ElementLink<IParticleContainer > > >* p_acc = PFOParticleTypeMapper_temp::getAccessor(ParticleType);
    if (!p_acc) return false;
    else{
      (*p_acc)(*this) = theParticles;
      return true;
    }
  }
  
  bool PFO_v1::associatedParticles(PFODetails::PFOParticleType ParticleType, std::vector<const IParticle*>& theParticles ) const{

    const ConstAccessor<std::vector<ElementLink<IParticleContainer > > >* p_acc = PFOParticleTypeMapper_temp::getAccessor(ParticleType);
    if (!p_acc) return false;
    else{
      if (!p_acc->isAvailable(*this)) return false;
      else{
	const std::vector<ElementLink<IParticleContainer> >& theLinks = (*p_acc)(*this);
	std::vector<ElementLink<IParticleContainer> >::const_iterator firstLink = theLinks.begin();
	for (; firstLink != theLinks.end(); ++firstLink) {
	  if ( (*firstLink).isValid()) theParticles.push_back(**firstLink);
	  else theParticles.push_back(NULL);
	}
	return true;
      }
    }

  }

  void PFO_v1::setAssociatedParticleLinks(const std::string& ParticleType,  const std::vector<ElementLink<IParticleContainer> >& theParticles) {

    //Given we do not know in advance in the POOL convertors about these containers, we set toPersistent() internally here.

    std::vector<ElementLink<IParticleContainer> > storedContainer;
    std::vector<ElementLink<IParticleContainer> >::const_iterator firstParticle = theParticles.begin();
    for (; firstParticle != theParticles.end(); ++firstParticle){
      ElementLink<xAOD::IParticleContainer> myLink = *firstParticle;
      myLink.toPersistent();
      storedContainer.push_back( myLink );
    }

    const static Accessor<std::vector<ElementLink<IParticleContainer > > > acc(ParticleType);
    acc(*this) = storedContainer;
  }

  bool PFO_v1::associatedParticles(const std::string& ParticleType, std::vector<const IParticle*>& theParticles ) const{
    const static ConstAccessor<std::vector<ElementLink<IParticleContainer > > > acc(ParticleType);
    if (!acc.isAvailable(*this)) return false;
    else{
      const std::vector<ElementLink<IParticleContainer> >& theLinks = acc(*this);
      std::vector<ElementLink<IParticleContainer> >::const_iterator firstLink = theLinks.begin();
      for (; firstLink != theLinks.end(); ++firstLink) {
	if ( (*firstLink).isValid()) theParticles.push_back(**firstLink);
	else theParticles.push_back(NULL);
	}
      return true;
    }
  }

  const CaloCluster* PFO_v1::cluster(unsigned int index) const {
    
    const ConstAccessor<std::vector<ElementLink<IParticleContainer > > >* p_acc = PFOParticleTypeMapper_temp::getAccessor(PFODetails::CaloCluster);
    if (!p_acc) return nullptr;
    else if (!p_acc->isAvailable(*this)) {return nullptr;}
    else {
      const std::vector<ElementLink<IParticleContainer> >& theLinks = (*p_acc)(*this);
      if(index<theLinks.size()) {
	ElementLink<IParticleContainer> theLink = theLinks[index];
	if (theLink.isValid()){
	  const IParticle *theParticle = *theLink;
	  if (NULL == theParticle) return NULL;
	  else if (Type::CaloCluster == theParticle->type()) return static_cast<const CaloCluster*>(theParticle);
	  else return NULL;
	}
	else return NULL;
      }
      else return NULL;
    }

  }

  const TrackParticle* PFO_v1::track(unsigned int index) const {

    const ConstAccessor<std::vector<ElementLink<IParticleContainer > > >* p_acc = PFOParticleTypeMapper_temp::getAccessor(PFODetails::Track);
    if (!p_acc) return nullptr;
    else if (!p_acc->isAvailable(*this)) {return nullptr;}
    else {
      const std::vector<ElementLink<IParticleContainer> >& theLinks = (*p_acc)(*this);
      if(index<theLinks.size()) {
	ElementLink<IParticleContainer> theLink = theLinks[index];
	if (theLink.isValid()){
	  const IParticle *theParticle = *theLinks[index];
	  if (NULL == theParticle) return NULL;
	  else if (Type::TrackParticle == theParticle->type()) return static_cast<const TrackParticle*>(theParticle);
	  else return NULL;
	}
	else return NULL;
      }
      else return NULL;
    }
  }

  const xAOD::Vertex* PFO_v1::vertex() const{
    const static ConstAccessor<ElementLink<xAOD::VertexContainer> > acc("pfo_vertex");
    if (!acc.isAvailable(*this)) return nullptr;
    else{
      ElementLink<xAOD::VertexContainer> tempVertexLink = acc(*this);
      if (tempVertexLink.isValid()) return *acc(*this);
      else return nullptr;
    }
  }
  
  TLorentzVector PFO_v1::GetVertexCorrectedFourVec(const xAOD::Vertex& vertexToCorrectTo) const{
    TVector3 theVertexVector(vertexToCorrectTo.x(), vertexToCorrectTo.y(), vertexToCorrectTo.z());
    return GetVertexCorrectedFourVec(theVertexVector);
  }

  TLorentzVector PFO_v1::GetVertexCorrectedFourVec(const TVector3& vertexToCorrectTo) const{

    TLorentzVector theNewVector(0.0,0.0,0.0,0.0);
    theNewVector.SetPtEtaPhiM(this->pt(), this->eta(), this->phi(), this->m());

    this->VertexCorrectTheFourVector(vertexToCorrectTo,theNewVector);
    return theNewVector;

  }

  TLorentzVector PFO_v1::GetVertexCorrectedEMFourVec(const xAOD::Vertex& vertexToCorrectTo) const{
    TVector3 theVertexVector(vertexToCorrectTo.x(), vertexToCorrectTo.y(), vertexToCorrectTo.z());
    return GetVertexCorrectedEMFourVec(theVertexVector);
  }

  TLorentzVector PFO_v1::GetVertexCorrectedEMFourVec(const TVector3& vertexToCorrectTo) const{

    TLorentzVector theNewVector(0.0,0.0,0.0,0.0);
    theNewVector.SetPtEtaPhiM(this->ptEM(), this->etaEM(), this->phiEM(), this->mEM());

    this->VertexCorrectTheFourVector(vertexToCorrectTo,theNewVector);
    return theNewVector;

  }

  void PFO_v1::VertexCorrectTheFourVector(const TVector3& vertexToCorrectTo, TLorentzVector& theFourVector) const{

    float clusterEta = theFourVector.Eta();
    float centerMag = this->centerMag();

    float radius = centerMag/cosh(clusterEta);

    float EtaVertexCorrection = 0.0, PhiVertexCorrection = 0.0;
    float clusterPhi = theFourVector.Phi();

    if (radius > 1.0 && centerMag > 1e-3){
      EtaVertexCorrection = (-vertexToCorrectTo.Z()/cosh(clusterEta) + (vertexToCorrectTo.X()*cos(clusterPhi) + vertexToCorrectTo.Y()*sin(clusterPhi))*tanh(clusterEta))/radius;
      PhiVertexCorrection = (vertexToCorrectTo.X()*sin(clusterPhi) - vertexToCorrectTo.Y()*cos(clusterPhi))/radius;
    }

    float etaVertexCorrected = clusterEta + EtaVertexCorrection;
    float p = std::sqrt(theFourVector.E()*theFourVector.E()-theFourVector.M()*theFourVector.M());
    float ptVertexCorrected = p/cosh(etaVertexCorrected); 
    theFourVector.SetPtEtaPhiM(ptVertexCorrected, etaVertexCorrected, clusterPhi + PhiVertexCorrection, theFourVector.M());

  }

  void PFO_v1::toPersistent(){

    //setup known links for persistification, if they exist
    //if you added your own consituents the links will not be correctly persistified

    //clusters
    const Accessor<std::vector<ElementLink<IParticleContainer > > >* p_accClusters = PFOParticleTypeMapper_temp::getAccessor(PFODetails::CaloCluster);
    if (p_accClusters){
      const static Accessor<std::vector<ElementLink<IParticleContainer > > >& accClusters = *p_accClusters;
      if ( accClusters.isAvailableWritable(*this) ){
	std::vector<ElementLink<IParticleContainer> >& theClusterLinks = accClusters(*this);
	std::vector< ElementLink< IParticleContainer > >::iterator  firstClus = theClusterLinks.begin();
	std::vector< ElementLink< IParticleContainer > >::iterator  lastClus = theClusterLinks.end();
	for (; firstClus != lastClus; ++firstClus) firstClus->toPersistent();
      }
    }
    
    //tracks
    const Accessor<std::vector<ElementLink<IParticleContainer > > >* p_accTracks = PFOParticleTypeMapper_temp::getAccessor(PFODetails::Track);
    if (p_accTracks){
      const static Accessor<std::vector<ElementLink<IParticleContainer > > >& accTracks = *p_accTracks;
      if ( accTracks.isAvailableWritable(*this) ){
	std::vector<ElementLink<IParticleContainer> >& theTrackLinks = accTracks(*this);
	std::vector< ElementLink< IParticleContainer > >::iterator  firstTrack = theTrackLinks.begin();
	std::vector< ElementLink< IParticleContainer > >::iterator  lastTrack = theTrackLinks.end();
	for (; firstTrack != lastTrack; ++firstTrack)  firstTrack->toPersistent();

      }
    }

    //shots    
    const Accessor<std::vector<ElementLink<IParticleContainer > > >* p_accShots = PFOParticleTypeMapper_temp::getAccessor(PFODetails::TauShot);
    if (p_accShots){
      const static Accessor<std::vector<ElementLink<IParticleContainer > > >& accShots = *p_accShots;
      if ( accShots.isAvailableWritable(*this) ){
	std::vector<ElementLink<IParticleContainer> >& theShotLinks = accShots(*this);
	std::vector< ElementLink< IParticleContainer > >::iterator  firstShot = theShotLinks.begin();
	std::vector< ElementLink< IParticleContainer > >::iterator  lastShot = theShotLinks.end();
	for (; firstShot != lastShot; ++firstShot) firstShot->toPersistent();
      }
    }

  }
  
  
} // namespace xAOD
