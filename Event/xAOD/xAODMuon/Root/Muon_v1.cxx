/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Misc includes
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODPrimitives/tools/getIsolationAccessor.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackSummaryAccessors_v1.h"

#include "xAODPrimitives/tools/getIsolationAccessor.h"
#include "xAODPrimitives/tools/getIsolationCorrectionAccessor.h"

// Local include(s):
#include "xAODMuon/versions/Muon_v1.h"
#include "MuonAccessors_v1.h"
#include "MuonTrackSummaryAccessors_v1.h"
// Athena-only includes
#ifndef XAOD_ANALYSIS
// #include "TrkParameters/MeasuredPerigee.h"
#endif

namespace xAOD {

  Muon_v1::Muon_v1()
  : IParticle() {
  }
  
  Muon_v1::Muon_v1(const Muon_v1& rhs)
    : IParticle(rhs) //IParticle does not have a copy constructor. AuxElement has one with same behavior as default ctor
  {
    this->makePrivateStore(rhs);
  }
  
  Muon_v1& Muon_v1::operator=(const Muon_v1& rhs ){
    if(this == &rhs) return *this;
 
    if( ( ! hasStore() ) && ( ! container() ) ) {
       makePrivateStore();
    }
    this->IParticle::operator=( rhs );
    
    return *this;
  }
  
  Muon_v1::~Muon_v1(){
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, phi)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Muon_v1, float, charge, setCharge )

  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, e)
  
  double Muon_v1::e() const {
    // FIXME - optimise?
    return genvecP4().E();
  }
  
  double Muon_v1::m() const {
    return 105.6583715;//FIXME - get this from someplace central? How heavy is TDatabasePDG::Instance()->GetParticle(pdg)?
  }

  void Muon_v1::setP4(double pt, double eta, double phi)  {
    static const Accessor< float > acc1( "pt" );
    static const Accessor< float > acc2( "eta" );
    static const Accessor< float > acc3( "phi" );
    acc1( *this )=pt;
    acc2( *this )=eta;
    acc3( *this )=phi;
  }

  double Muon_v1::rapidity() const {
    return genvecP4().Rapidity();
  }

  Muon_v1::FourMom_t Muon_v1::p4() const {
    Muon_v1::FourMom_t p4;
    p4.SetPtEtaPhiM( pt(), eta(), phi(), m() ); 
    return p4;
  }

  // depend on return value optimization
  Muon_v1::GenVecFourMom_t Muon_v1::genvecP4() const {
    return GenVecFourMom_t(pt(), eta(), phi(), m());
  }

//  float Muon_v1::charge() const {
//    if (primaryTrackParticle()) return primaryTrackParticle()->charge();
//    // something has gone wrong!
//    throw std::runtime_error("No link to primary TrackParticle!");
//  }

  Type::ObjectType Muon_v1::type() const {
    return Type::Muon;
  }  

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::Author, author) 
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::Author, author, setAuthor) 

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Muon_v1, uint16_t, allAuthors, setAllAuthors)


  void Muon_v1::addAllAuthor ( const Author author ){
    static const Accessor< uint16_t > acc( "allAuthors" );
    acc(*this) |= 1<<static_cast<unsigned int>(author);
  }

  bool Muon_v1::isAuthor ( const Author author ) const{
    static const Accessor< uint16_t > acc( "allAuthors" );
    return (acc(*this)& (1<<static_cast<unsigned int>(author)));
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::MuonType, muonType)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::MuonType, muonType, setMuonType)

  bool Muon_v1::summaryValue(uint8_t& value, const SummaryType information)  const {
    // Here we want to check if this information has been added to the Muon, and use this first if so.
    // @todo ?Could further optimise the below, to see first if the SummaryType value is one of the ones we write to Muons?
    // @todo ?Is there a better way than catching the exception?
    try {
      const Muon_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information ); 
      value = ( *acc )( *this );
      return true;
    } catch ( SG::ExcBadAuxVar& ) {}
    
    // Okay - fallback: try to get from TrackParticle.
    const ElementLink< TrackParticleContainer >& el= primaryTrackParticleLink();
    if (!el.isValid()) return false;
    return (*el)->summaryValue(value,information);
  }  

  void Muon_v1::setSummaryValue( uint8_t  value, const SummaryType 	information ) {
    const Muon_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information ); ///FIXME!
    // Set the value:
    ( *acc )( *this ) = value;
  }

  // No set method for 'float' values as not expected to be needed
   
  bool Muon_v1::summaryValue(float& value, const SummaryType information)  const {
    const ElementLink< TrackParticleContainer >& el= primaryTrackParticleLink();
    if (!el.isValid()) return false;
    return (*el)->summaryValue(value,information);
  }  
  
  float Muon_v1::floatSummaryValue(const SummaryType information) const {
    const Muon_v1::Accessor< float >* acc = trackSummaryAccessorV1< float >( information );
  	return ( *acc )( *this );
  }

  uint8_t Muon_v1::uint8SummaryValue(const SummaryType information) const{
    const Muon_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1< uint8_t >( information );
    return ( *acc )( *this );  	
  }
  
  bool Muon_v1::summaryValue(uint8_t& value, const MuonSummaryType information)  const {
    const Muon_v1::Accessor< uint8_t >* acc = muonTrackSummaryAccessorV1( information );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;
    
  // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
  
  float Muon_v1::uint8MuonSummaryValue(const MuonSummaryType information) const{
	  const Muon_v1::Accessor< uint8_t >* acc = muonTrackSummaryAccessorV1( information );
	  return ( *acc )( *this );
  }
  

  void Muon_v1::setSummaryValue(uint8_t value, const MuonSummaryType information) {
    const Muon_v1::Accessor< uint8_t >* acc = muonTrackSummaryAccessorV1( information );
    // Set the value:
    ( *acc )( *this ) =  value;
  }
  
  bool Muon_v1::parameter(float& value, const Muon_v1::ParamDef information)  const {
    const xAOD::Muon_v1::Accessor< float >* acc = parameterAccessorV1<float>( information );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;
    
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
	
  float xAOD::Muon_v1::floatParameter(xAOD::Muon_v1::ParamDef information) const{
    const xAOD::Muon_v1::Accessor< float >* acc = parameterAccessorV1<float>( information );
    return ( *acc )( *this );
  }

  void Muon_v1::setParameter(float value, const Muon_v1::ParamDef information){
    const xAOD::Muon_v1::Accessor< float >* acc = parameterAccessorV1<float>( information );
    if( ! acc ) throw std::runtime_error("Muon_v1::setParameter - no float accessor for paramdef number: "+std::to_string(information));
    
    // Set the value:
    ( *acc )( *this ) = value;
  }
  
  bool Muon_v1::parameter(int& value, const Muon_v1::ParamDef information)  const {
    const xAOD::Muon_v1::Accessor< int >* acc = parameterAccessorV1<int>( information );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;
    
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
	
  int xAOD::Muon_v1::intParameter(xAOD::Muon_v1::ParamDef information) const{
    const xAOD::Muon_v1::Accessor< int >* acc = parameterAccessorV1<int>( information );
    return ( *acc )( *this );
  }

  void Muon_v1::setParameter(int value, const Muon_v1::ParamDef information){
    const xAOD::Muon_v1::Accessor< int >* acc = parameterAccessorV1<int>( information );
    if( ! acc ) throw std::runtime_error("Muon_v1::setParameter - no int accessor for paramdef number: "+std::to_string(information));
    
    // Set the value:
    ( *acc )( *this ) = value;
  }

  xAOD::Muon_v1::Quality Muon_v1::quality() const {
    static const Accessor< uint8_t > acc( "quality" );
    uint8_t temp =  acc( *this );
    return static_cast<Quality>(temp&3);     
  }
  
  void Muon_v1::setQuality(xAOD::Muon_v1::Quality value) {
    static const Accessor< uint8_t > acc( "quality" );
    uint8_t temp = static_cast< uint8_t >(value);
    acc( *this ) = acc( *this ) & ~(0x7); // Reset the first 3 bits.
    acc( *this ) |= temp;
    return;      
  }
  
  bool Muon_v1::passesIDCuts() const {
    static const Accessor< uint8_t > acc( "quality" );
    uint8_t temp =  acc( *this );
    // We use 4th bit for 'passesIDCuts'
    return temp&8;     
  }
  
  void Muon_v1::setPassesIDCuts(bool value) {
    static const Accessor< uint8_t > acc( "quality" );
    // We use 4th bit for 'passesIDCuts'
    if (value) acc( *this ) |= 8;
    else       acc( *this ) &= 247;
    return;      
  }
  
  bool Muon_v1::passesHighPtCuts() const {
    static const Accessor< uint8_t > acc( "quality" );
    uint8_t temp =  acc( *this );
    // We use 5th bit for 'passesHighPtCuts'
    return temp&16;    
  }
  
  void Muon_v1::setPassesHighPtCuts(bool value) {
    static const Accessor< uint8_t > acc( "quality" );
    // We use 5th bit for 'passesHighPtCuts'
    if (value) acc( *this ) |= 16;
    else       acc( *this ) &= 239;
    return;      
  }

  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, uint8_t, Muon_v1::Quality,
  //                                      quality )
  // AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Muon_v1, uint8_t, Muon_v1::Quality,
  //                                      quality, setQuality )

  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Muon_v1, bool, passesIDCuts,      setPassesIDCuts)
  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Muon_v1, bool, passesHighPtCuts,  setPassesHighPtCuts)
  
  bool Muon_v1::isolation(float& value, const Iso::IsolationType information)  const {
    const SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    
    if( ! acc ) return false;
    if( !acc->isAvailable( *this) ) return  false;
    
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
  
  float Muon_v1::isolation( const Iso::IsolationType information)  const {
    const SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    if( !acc ) throw std::runtime_error( "Unknown/Unavailable Isolation type requested" );
    return  ( *acc )( *this );
  }
  
  void Muon_v1::setIsolation(float value, const Iso::IsolationType information){
    const SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    if( !acc ) throw std::runtime_error( "Unknown/Unavailable Isolation type requested" );
    // Set the value:
    ( *acc )( *this ) = value;
  }
  
bool Muon_v1::isolationCaloCorrection(  float& value, const Iso::IsolationFlavour flavour, 
                                        const Iso::IsolationCaloCorrection type,
                                        const Iso::IsolationCorrectionParameter param) const{
    const SG::AuxElement::Accessor< float > acc = getIsolationCorrectionAccessor(flavour,type,param);
    if( !acc.isAvailable( *this) ) return false;
    // Retrieve the value:
    value = acc( *this );
    return true;
  }

  float Muon_v1::isolationCaloCorrection(const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection type,
  const Iso::IsolationCorrectionParameter param) const{

    const SG::AuxElement::Accessor< float > acc = getIsolationCorrectionAccessor(flavour,type,param);
    if( !acc.isAvailable( *this) ) throw std::runtime_error( "Unknown/Unavailable Isolation correction requested" );
    return  acc( *this );
  }

  bool Muon_v1::setIsolationCaloCorrection(float value, const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection type,
  const Iso::IsolationCorrectionParameter param){
    const SG::AuxElement::Accessor< float > acc = getIsolationCorrectionAccessor(flavour,type,param); 
    // Set the value:
    acc( *this ) = value;
    return true;
  }

  bool Muon_v1::isolationTrackCorrection(float& value, const Iso::IsolationFlavour flavour, const Iso::IsolationTrackCorrection type) const{
    const SG::AuxElement::Accessor< float > acc = getIsolationCorrectionAccessor(flavour,type);
    if( !acc.isAvailable( *this) ) return  false;
    // Retrieve the value:
    value = acc( *this );
    return true;
  }

  float Muon_v1::isolationTrackCorrection(const Iso::IsolationFlavour flavour, const Iso::IsolationTrackCorrection type) const{

    const SG::AuxElement::Accessor< float > acc = getIsolationCorrectionAccessor(flavour,type);
    if( !acc.isAvailable( *this) ) throw std::runtime_error( "Unknown/Unavailable Isolation correction requested" );
    return  acc( *this );
  }

  bool Muon_v1::setIsolationTrackCorrection(float value, const Iso::IsolationFlavour flavour, const Iso::IsolationTrackCorrection type){
    const SG::AuxElement::Accessor< float > acc = getIsolationCorrectionAccessor(flavour,type);
    // Set the value:
    acc( *this ) = value;
    return true;
  }

  bool Muon_v1::isolationCorrectionBitset(std::bitset<32>& value, const Iso::IsolationFlavour flavour ) const{
    const SG::AuxElement::Accessor< uint32_t > acc = getIsolationCorrectionBitsetAccessor( flavour );
    if( !acc.isAvailable( *this) ) return false;
    // Retrieve the value:
    value = std::bitset<32>(acc( *this ));
    return true;
  }

  std::bitset<32> Muon_v1::isolationCorrectionBitset(const Iso::IsolationFlavour flavour ) const{
    const SG::AuxElement::Accessor< uint32_t > acc = getIsolationCorrectionBitsetAccessor( flavour );
    if( !acc.isAvailable( *this) ) throw std::runtime_error( "Unknown/Unavailable Isolation BitSet requested" );
    return  std::bitset<32>( acc( *this ) );
  }

  bool Muon_v1::setIsolationCorrectionBitset(uint32_t value, const Iso::IsolationFlavour flavour ) {
    const SG::AuxElement::Accessor< uint32_t > acc = getIsolationCorrectionBitsetAccessor( flavour );
    // Set the value:
    acc( *this ) = value;
    return true;
  }

  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, inDetTrackParticleLink)
  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, muonSpectrometerTrackParticleLink)
  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, extrapolatedMuonSpectrometerTrackParticleLink)
  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, msOnlyExtrapolatedMuonSpectrometerTrackParticleLink)
  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, combinedTrackParticleLink)

  const ElementLink< TrackParticleContainer >& Muon_v1::primaryTrackParticleLink() const{
    MuonType type = muonType();
    switch ( type ) {
      case Combined :
      case SiliconAssociatedForwardMuon :
        return combinedTrackParticleLink();
        break;
      case SegmentTagged :
      case CaloTagged :
        return inDetTrackParticleLink();
        break;
      case MuonStandAlone :
        {          
          // Not checking if links are valid here - this is the job of the client (as per the cases above).
          // But we DO check that the link is available, so we can check for both types of links.
          
          static const Accessor< ElementLink< TrackParticleContainer > > acc1( "extrapolatedMuonSpectrometerTrackParticleLink" );
          if ( acc1.isAvailable( *this ) && acc1( *this ).isValid() ) {
            return acc1( *this );
          }

          static const Accessor< ElementLink< TrackParticleContainer > > acc2( "msOnlyExtrapolatedMuonSpectrometerTrackParticleLink" );
          if ( acc2.isAvailable( *this ) && acc2( *this ).isValid() ) {
            return acc2( *this );
          }
          
          static const Accessor< ElementLink< TrackParticleContainer > > acc3( "muonSpectrometerTrackParticleLink" );
          if ( acc3.isAvailable( *this ) && acc3( *this ).isValid()) {            
            return acc3( *this );
          }
          // We could also just return a dummy EL here, but the link is part of the aux store, and so it might be that something bad has happened...?
          throw std::runtime_error("Type is MuonStandAlone but no available link to return!");
        }
      default:
        throw std::runtime_error("Unknown primary type - not sure which track particle to return!");
    }
    // static ElementLink< TrackParticleContainer > dummy;
    // return dummy;
  }
  
  const xAOD::TrackParticle* Muon_v1::primaryTrackParticle() const{
    
    MuonType type = muonType();      
    switch( type ) {
      case Combined:
      case SiliconAssociatedForwardMuon :
         {
            static const Accessor< ElementLink< TrackParticleContainer > > acc( "combinedTrackParticleLink" );
            if( ! acc.isAvailable( *this ) ) return 0;
          
            const ElementLink< TrackParticleContainer >& link = acc( *this );
            if( ! link.isValid() ) return 0;
            
            return *link;
         }
      case SegmentTagged:
      case CaloTagged :
        {
           static const Accessor< ElementLink< TrackParticleContainer > > acc( "inDetTrackParticleLink" );
           if( ! acc.isAvailable( *this ) ) return 0;
           
           const ElementLink< TrackParticleContainer >& link = acc( *this );
           if( ! link.isValid() ) return 0;
           
           return *link;
        }
      case MuonStandAlone :
        {
          // Want to return link to extrapolated MS track particle if possible.
          static const Accessor< ElementLink< TrackParticleContainer > > acc1( "extrapolatedMuonSpectrometerTrackParticleLink" );
          if ( acc1.isAvailable( *this ) ) {            
            const ElementLink< TrackParticleContainer >& link = acc1( *this );
            if ( link.isValid() ) return *link;
          }

	  //if no, maybe the MS-only extrapolated track particle?
          static const Accessor< ElementLink< TrackParticleContainer > > acc2( "msOnlyExtrapolatedMuonSpectrometerTrackParticleLink" );
          if ( acc2.isAvailable( *this ) ) {
            const ElementLink< TrackParticleContainer >& link = acc2( *this );
            if ( link.isValid() ) return *link;
          }
          
          // Try fallback (non-extrapolated MS track particle)...
          static const Accessor< ElementLink< TrackParticleContainer > > acc3( "muonSpectrometerTrackParticleLink" );
          if ( acc3.isAvailable( *this ) ) {            
            const ElementLink< TrackParticleContainer >& link = acc3( *this );
            if ( link.isValid() ) return *link;
          }

          return 0;
        }
      default:
        {
          // No valid link.
          return 0;
        }
      }
  }
  
  const ElementLink< TrackParticleContainer >& Muon_v1::trackParticleLink( Muon_v1::TrackParticleType type) const{
    switch ( type ) {
      case Primary :
        return primaryTrackParticleLink();
        break;
      case CombinedTrackParticle :
        return combinedTrackParticleLink();
        break;
      case InnerDetectorTrackParticle :
        return inDetTrackParticleLink();
        break;
      case MuonSpectrometerTrackParticle :
        return muonSpectrometerTrackParticleLink();
        break;
      case ExtrapolatedMuonSpectrometerTrackParticle :
        return extrapolatedMuonSpectrometerTrackParticleLink();
        break;
      case MSOnlyExtrapolatedMuonSpectrometerTrackParticle :
	return msOnlyExtrapolatedMuonSpectrometerTrackParticleLink();
	break;
      default:
        throw std::runtime_error("Unknown TrackParticleType - not sure which track particle to return!");
    }
    // static ElementLink< TrackParticleContainer > dummy;
    // return dummy;
  }
  
  const xAOD::TrackParticle* Muon_v1::trackParticle( Muon_v1::TrackParticleType type) const{
    // TODO - perhaps we can get rid of this try/catch clause?
    try {
      // Get the ElementLink pointing to the requested track particle:
      const ElementLink< TrackParticleContainer >& el =
        trackParticleLink( type );
      
      // If it's invalid, return a null pointer:
      if( ! el.isValid() ) {
        return 0;
      }
      
      // If it's valid, let's de-reference it:
      return *el;
    } catch ( SG::ExcBadAuxVar& ) {
      return 0;
    }
  }

  void Muon_v1::setTrackParticleLink(TrackParticleType type, const ElementLink< TrackParticleContainer >& link){
    switch ( type ) {
      case InnerDetectorTrackParticle :
        static const Accessor< ElementLink< TrackParticleContainer > > acc1( "inDetTrackParticleLink" );
        acc1(*this)=link;
        break;
      case MuonSpectrometerTrackParticle :
        static const Accessor< ElementLink< TrackParticleContainer > > acc2( "muonSpectrometerTrackParticleLink" );
        acc2(*this)=link;
        break;
      case CombinedTrackParticle :
        static const Accessor< ElementLink< TrackParticleContainer > > acc3( "combinedTrackParticleLink" );
        acc3(*this)=link;          
        break;
      case ExtrapolatedMuonSpectrometerTrackParticle :
        static const Accessor< ElementLink< TrackParticleContainer > > acc4( "extrapolatedMuonSpectrometerTrackParticleLink" );
        acc4(*this)=link;
        break;
      case MSOnlyExtrapolatedMuonSpectrometerTrackParticle :
        static const Accessor< ElementLink< TrackParticleContainer > > acc5( "msOnlyExtrapolatedMuonSpectrometerTrackParticleLink" );
	acc5(*this)=link;
	break;
      case Primary :
      default:
        throw std::runtime_error("Unknown or Primary TrackParticleType - not sure which track particle to set!");
    }
  }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( Muon_v1, ElementLink<CaloClusterContainer>, clusterLink, setClusterLink)
  const CaloCluster* Muon_v1::cluster() const { 
    
    static const Accessor< ElementLink< TrackParticleContainer > > acc( "inDetTrackParticleLink" );
    if( ! acc.isAvailable( *this ) ) {
       return 0;
    }
    const ElementLink< TrackParticleContainer >& link = acc( *this );
    if( ! link.isValid() ) {
       return 0;
    }
    
    // Get the ElementLink pointing to the calo cluster: 
    const ElementLink< CaloClusterContainer >& el = clusterLink(); 
    // If it's invalid, return a null pointer: 
    if( ! el.isValid() ) { 
      return 0; 
    } 
    // If it's valid, let's de-reference it: 
    return *el; 
  } 

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, uint8_t, Muon_v1::EnergyLossType,
                                       energyLossType )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Muon_v1, uint8_t, Muon_v1::EnergyLossType,
                                       energyLossType, setEnergyLossType )

  AUXSTORE_OBJECT_SETTER_AND_GETTER( Muon_v1, std::vector< ElementLink< xAOD::MuonSegmentContainer > >, muonSegmentLinks, setMuonSegmentLinks)

  static const SG::AuxElement::Accessor< std::vector< ElementLink< MuonSegmentContainer > > > muonSegmentsAcc( "muonSegmentLinks" ); 
  size_t Muon_v1::nMuonSegments() const {
        // If a link was not set (yet), return zero.
    if( ! muonSegmentsAcc.isAvailable( *this ) ) {
      return 0;
    }
    return muonSegmentsAcc(*this).size();
  }

  const ElementLink< MuonSegmentContainer >& Muon_v1::muonSegmentLink( size_t i ) const {
        // If a Trk::Track link was not set (yet), return a dummy object:
      // FIXME - maybe 
    if( ! muonSegmentsAcc.isAvailable( *this ) ) {
      static const ElementLink< MuonSegmentContainer > dummy;
      return dummy;
    }
    return muonSegmentsAcc(*this).at(i);
  }

  const MuonSegment* Muon_v1::muonSegment( size_t i ) const{	
      // Get the ElementLink pointing to the requested muon segment:
    const ElementLink< MuonSegmentContainer >& el =
      muonSegmentLink( i );
      // If it's invalid, return a null pointer:
    if( ! el.isValid() ) {
      return 0;
    }
      // If it's valid, let's de-reference it:
    return *el;
  }
} // namespace xAOD

