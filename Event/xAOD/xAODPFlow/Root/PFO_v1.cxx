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

   PFO_v1::PFO_v1()
     : IParticle(), m_p4(), m_p4Cached( false ), m_p4EM(),  m_p4EMCached(false), m_floatCompressionFactor(1000) {

   }

  PFO_v1::PFO_v1(const PFO_v1& other) :  IParticle(), m_p4(), m_p4Cached( false ), m_p4EM(),  m_p4EMCached(false), m_floatCompressionFactor(1000) {
    this->makePrivateStore(other);
  }


   double PFO_v1::pt() const {

     const static Accessor<float> accPt("pt");
     float pt = accPt(*this);

     return pt;
   }

   double PFO_v1::eta() const {

     return p4().Eta();
   }

   double PFO_v1::phi() const {

     return p4().Phi();
   }

   double PFO_v1::m() const {

     return p4().M();
   }

   double PFO_v1::e() const {

     const static Accessor<float> accPt("pt");
     float pt = accPt(*this);

     if (pt < 0.0) return -p4().E();
     else return p4().E();
   }

   double PFO_v1::rapidity() const {

     return p4().Rapidity();
   }

   const PFO_v1::FourMom_t& PFO_v1::p4() const {

     // Check if we need to reset the cached object:
     if( ! m_p4Cached ) {

       const static Accessor<float> accPt("pt");
       float pt = accPt(*this);

       const static Accessor<float> accEta("eta");
       float eta = accEta(*this);

       const static Accessor<float> accPhi("phi");
       float phi = accPhi(*this);

       const static Accessor<float> accM("m");
       float M = accM(*this);

       m_p4.SetPtEtaPhiM(pt,eta,phi,M);
       m_p4Cached = true;
     }

     // Return the cached object:
     return m_p4;
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

    m_p4Cached = false;

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

    m_p4Cached = false;
  }
  

   Type::ObjectType PFO_v1::type() const {
      return Type::ParticleFlow;
   }

  const PFO_v1::FourMom_t& PFO_v1::p4EM() const { 
    
    if (0.0 != this->charge()) return this->p4();

    if (!m_p4EMCached){

      //change to use pt, eta, phi ,e 
      const static Accessor<float> accPt("ptEM");
      const static Accessor<float> accEta("eta");
      const static Accessor<float> accPhi("phi");
      const static Accessor<float> accM("mEM");

      m_p4EM.SetPtEtaPhiM(accPt(*this), accEta(*this), accPhi(*this), accM(*this));
      m_p4EMCached = true;
    }

    return m_p4EM;
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

    m_p4EMCached = false;

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

    m_p4Cached = false;
  }

   double PFO_v1::ptEM() const {

     if (0.0 != this->charge()) return this->pt();

     const static Accessor<float> accPt("ptEM");
     float pt = accPt(*this);

     return pt;
   }

   double PFO_v1::etaEM() const {

     if (0.0 != this->charge()) return this->eta();

     return p4EM().Eta();
   }

   double PFO_v1::phiEM() const {

     if (0.0 != this->charge()) return this->phi();

     return p4EM().Phi();
   }

   double PFO_v1::mEM() const {

     if (0.0 != this->charge()) return this->m();

     return p4EM().M();
   }

   double PFO_v1::eEM() const {

     if (0.0 != this->charge()) return this->e();

     const static Accessor<float> accPt("ptEM");
     float pt = accPt(*this);

     if (pt < 0.0) return -p4EM().E();
     else return p4EM().E();
   }


  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(PFO_v1, float, bdtPi0Score, setBDTPi0Score)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(PFO_v1, float, centerMag, setCenterMag)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(PFO_v1, float, charge, setCharge)

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
      float dummy = anAttribute*m_floatCompressionFactor;
      int maxIntSize = 1000000000;
      int internalAttribute = maxIntSize;
      if (dummy < maxIntSize) internalAttribute = static_cast<int>(dummy);
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
      if (true == isValid && 0 != internalAttribute) anAttribute = static_cast<float>(internalAttribute)/m_floatCompressionFactor;
      else anAttribute = 0.0;
      return isValid;
    }
    else{
      Accessor<float>* acc = PFOAttributesAccessor_v1<float>::accessor(AttributeType);
      //check if accessor pointer is NULL
      if( ! acc ) {  return false ;}
      //check if variable is avaialable
      if( ! acc->isAvailable( *this ) ) return false;
      //set variable and return true
      anAttribute =( *acc )( *this );
      return true;
       
    }
    return isValid;
  }
  
  bool PFO_v1::isJetETMissFloatForCompression(PFODetails::PFOAttributes AttributeType) const{
    if (PFODetails::PFOAttributes::eflowRec_LATERAL == AttributeType || PFODetails::PFOAttributes::eflowRec_LONGITUDINAL == AttributeType || PFODetails::PFOAttributes::eflowRec_SECOND_R == AttributeType || PFODetails::PFOAttributes::eflowRec_CENTER_LAMBDA == AttributeType || PFODetails::PFOAttributes::eflowRec_FIRST_ENG_DENS == AttributeType || PFODetails::PFOAttributes::eflowRec_ENG_FRAC_MAX == AttributeType || PFODetails::PFOAttributes::eflowRec_ISOLATION == AttributeType || PFODetails::PFOAttributes::eflowRec_ENG_BAD_CELLS == AttributeType || PFODetails::PFOAttributes::eflowRec_N_BAD_CELLS == AttributeType || PFODetails::PFOAttributes::eflowRec_BADLARQ_FRAC == AttributeType || PFODetails::PFOAttributes::eflowRec_ENG_POS == AttributeType || PFODetails::PFOAttributes::eflowRec_SIGNIFICANCE == AttributeType || PFODetails::PFOAttributes::eflowRec_AVG_LAR_Q == AttributeType || PFODetails::PFOAttributes::eflowRec_AVG_TILE_Q == AttributeType || PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EM3 == AttributeType || PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC0 == AttributeType || PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC == AttributeType || PFODetails::PFOAttributes::eflowRec_LAYERENERGY_Tile0 == AttributeType || PFODetails::PFOAttributes::eflowRec_TIMING == AttributeType ) {
      return true;
    }
    else {
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
    else return false;

  }

  bool PFO_v1::setVertexLink(const ElementLink< xAOD::VertexContainer>& theVertexLink){
    ElementLink< xAOD::VertexContainer > tempVertexLink;

    if (theVertexLink.isValid()){
      tempVertexLink.setElement(*theVertexLink);
      tempVertexLink.setStorableObject(theVertexLink.getStorableObjectRef());
      tempVertexLink.toPersistent();
    }
    const static Accessor<ElementLink<xAOD::VertexContainer> > acc("pfo_vertex");
    acc(*this) = tempVertexLink;
    return true;
  }
  

  bool PFO_v1::setTrackLink(const ElementLink<xAOD::TrackParticleContainer>& theTrack){
    ElementLink< xAOD::TrackParticleContainer > tempTrackLink;
    tempTrackLink.setElement(*theTrack);
    tempTrackLink.setStorableObject(theTrack.getStorableObjectRef());
    tempTrackLink.toPersistent();
    ElementLink< xAOD::IParticleContainer > myTrackLink;
    myTrackLink.resetWithKeyAndIndex( tempTrackLink.persKey(), tempTrackLink.persIndex() ); 
    return this->setAssociatedParticleLink(PFODetails::Track, myTrackLink);
  }

   bool PFO_v1::setClusterLink(const ElementLink<xAOD::CaloClusterContainer>& theCluster){
     ElementLink< xAOD::CaloClusterContainer > tempClusterLink;
     tempClusterLink.setElement(*theCluster);
     tempClusterLink.setStorableObject(theCluster.getStorableObjectRef());
     tempClusterLink.toPersistent();
     ElementLink< xAOD::IParticleContainer > myClusterLink;
     myClusterLink.resetWithKeyAndIndex( tempClusterLink.persKey(), tempClusterLink.persIndex() ); 
     return this->setAssociatedParticleLink(PFODetails::CaloCluster, myClusterLink);
  }

  bool PFO_v1::setAssociatedParticleLink(PFODetails::PFOParticleType ParticleType, const ElementLink<IParticleContainer>& theParticle){
    std::vector<ElementLink<xAOD::IParticleContainer> > theLinks(1,theParticle);
    return this->setAssociatedParticleLinks(ParticleType,theLinks);
  }

  void PFO_v1::setAssociatedParticleLink(const std::string& ParticleType, const ElementLink<IParticleContainer>& theParticle){
    std::vector<ElementLink<xAOD::IParticleContainer> > theLinks(1,theParticle);
    this->setAssociatedParticleLinks(ParticleType,theLinks);
  }

  bool PFO_v1::setAssociatedParticleLinks(PFODetails::PFOParticleType ParticleType,  const std::vector<ElementLink<IParticleContainer> >& theParticles) {

    PFOParticleTypeMapper_v1 typeMapper;
    std::string particleTypeName = "";
    bool result = typeMapper.getValue(ParticleType, particleTypeName);
    if (false == result) return false;
    else{
      Accessor<std::vector<ElementLink<IParticleContainer > > > acc(particleTypeName);
      acc(*this) = theParticles;
      return true;
    }
  }
  
  bool PFO_v1::associatedParticles(PFODetails::PFOParticleType ParticleType, std::vector<const IParticle*>& theParticles ) const{

    PFOParticleTypeMapper_v1 typeMapper;
    std::string particleTypeName = "";
    bool result = typeMapper.getValue(ParticleType, particleTypeName);
    if (false == result) return false;
    else{
      Accessor<std::vector<ElementLink<IParticleContainer > > > acc(particleTypeName);
      if (!acc.isAvailable(*this)) return false;
      else{
	std::vector<ElementLink<IParticleContainer> > theLinks = acc(*this);
	std::vector<ElementLink<IParticleContainer> >::iterator firstLink = theLinks.begin();
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
      ElementLink<xAOD::IParticleContainer> newLink;
      newLink.setElement(**firstParticle);
      newLink.setStorableObject((*firstParticle).getStorableObjectRef());
      newLink.toPersistent();
      storedContainer.push_back(newLink);
    }

    Accessor<std::vector<ElementLink<IParticleContainer > > > acc(ParticleType);
    acc(*this) = storedContainer;
  }

  bool PFO_v1::associatedParticles(const std::string& ParticleType, std::vector<const IParticle*>& theParticles ) const{
    Accessor<std::vector<ElementLink<IParticleContainer > > > acc(ParticleType);
    if (!acc.isAvailable(*this)) return false;
    else{
      std::vector<ElementLink<IParticleContainer> > theLinks = acc(*this);
      std::vector<ElementLink<IParticleContainer> >::iterator firstLink = theLinks.begin();
      for (; firstLink != theLinks.end(); ++firstLink) {
	if ( (*firstLink).isValid()) theParticles.push_back(**firstLink);
	else theParticles.push_back(NULL);
	}
      return true;
    }
  }

  const CaloCluster* PFO_v1::cluster(unsigned int index) const {

    std::vector<const IParticle*> theParticles;
    bool haveGot = this->associatedParticles(PFODetails::CaloCluster,theParticles);

    if (false == haveGot) return NULL;
    else{
      if (index < theParticles.size()) {

	const IParticle *theParticle = theParticles[index];

	if (NULL == theParticle) return NULL;
	else if (Type::CaloCluster == theParticle->type()) return dynamic_cast<const CaloCluster*>(theParticle);
	else return NULL; 
      }
      else return NULL;
    }


  }

  const TrackParticle* PFO_v1::track(unsigned int index) const {

    std::vector<const IParticle*> theParticles;
    bool haveGot = this->associatedParticles(PFODetails::Track,theParticles);

    if (false == haveGot) return NULL;
    else{
      if (index < theParticles.size()) {

        const IParticle *theParticle = theParticles[index];

	if (NULL == theParticle) return NULL;
	else if (Type::TrackParticle == theParticle->type()) return dynamic_cast<const TrackParticle*>(theParticle);
        else return NULL;
      }
      else return NULL;
    }
  }

  const xAOD::Vertex* PFO_v1::vertex() const{
    const static Accessor<ElementLink<xAOD::VertexContainer> > acc("pfo_vertex");
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
    PFOParticleTypeMapper_v1 typeMapper;
    std::string particleTypeName = "";
    bool result = typeMapper.getValue(PFODetails::CaloCluster, particleTypeName);
    if (true == result){
      Accessor<std::vector<ElementLink<IParticleContainer > > > accClusters(particleTypeName);
      if ( accClusters.isAvailableWritable(*this) ){
	std::vector<ElementLink<IParticleContainer> >& theClusterLinks = accClusters(*this);
	std::vector< ElementLink< IParticleContainer > >::iterator  firstClus = theClusterLinks.begin();
	std::vector< ElementLink< IParticleContainer > >::iterator  lastClus = theClusterLinks.end();
	for (; firstClus != lastClus; ++firstClus) firstClus->toPersistent();
      }
    }
    
    //tracks
    result = typeMapper.getValue(PFODetails::Track, particleTypeName);
    if (true == result){
      Accessor<std::vector<ElementLink<IParticleContainer > > > accTracks(particleTypeName);
      if ( accTracks.isAvailableWritable(*this) ){
	std::vector<ElementLink<IParticleContainer> >& theTrackLinks = accTracks(*this);
	std::vector< ElementLink< IParticleContainer > >::iterator  firstTrack = theTrackLinks.begin();
	std::vector< ElementLink< IParticleContainer > >::iterator  lastTrack = theTrackLinks.end();
	for (; firstTrack != lastTrack; ++firstTrack)  firstTrack->toPersistent();

      }
    }

    //shots    
    result = typeMapper.getValue(PFODetails::TauShot, particleTypeName);
    if (true == result){
      Accessor<std::vector<ElementLink<IParticleContainer > > > accShots(particleTypeName);
      if ( accShots.isAvailableWritable(*this) ){
	std::vector<ElementLink<IParticleContainer> >& theShotLinks = accShots(*this);
	std::vector< ElementLink< IParticleContainer > >::iterator  firstShot = theShotLinks.begin();
	std::vector< ElementLink< IParticleContainer > >::iterator  lastShot = theShotLinks.end();
	for (; firstShot != lastShot; ++firstShot) firstShot->toPersistent();
      }
    }


  }
  
  
} // namespace xAOD
