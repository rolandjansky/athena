/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_v1.cxx 616892 2014-09-16 08:59:08Z emoyse $
// Misc includes
#include <bitset>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODPrimitives/tools/getIsolationAccessor.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackSummaryAccessors_v1.h"

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
  : m_p4(), m_p4Cached1( false ) {
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, phi)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, float, double, e)
  
  double Muon_v1::e() const {
    // FIXME - optimise?
    return p4().E();
  }
  
  double Muon_v1::m() const {
    return 105.6583715;//FIXME - get this from someplace central? How heavy is TDatabasePDG::Instance()->GetParticle(pdg)?
  }

  void Muon_v1::setP4(double pt, double eta, double phi)  {
    static Accessor< float > acc1( "pt" );
    static Accessor< float > acc2( "eta" );
    static Accessor< float > acc3( "phi" );
    acc1( *this )=pt;
    acc2( *this )=eta;
    acc3( *this )=phi;
  }

  double Muon_v1::rapidity() const {
    return p4().Rapidity();
  }

  const Muon_v1::FourMom_t& Muon_v1::p4() const {
    using namespace std;
  // Check if we need to reset the cached object:
    if( ! m_p4Cached1 ) {
      m_p4.SetPtEtaPhiM( pt(), eta(), phi(), m() ); 
      m_p4Cached1 = true;
    }
  // Return the cached object:
    return m_p4;
  }

  float Muon_v1::charge() const {
    if (primaryTrackParticle()) return primaryTrackParticle()->charge();
    // something has gone wrong!
    throw std::runtime_error("No link to primary TrackParticle!");
  }

  Type::ObjectType Muon_v1::type() const {
    return Type::Muon;
  }  

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::Author, author) 
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::Author, author, setAuthor) 

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Muon_v1, uint16_t, allAuthors, setAllAuthors)


  void Muon_v1::addAllAuthor ( const Author author ){
    static Accessor< uint16_t > acc( "allAuthors" );
    acc(*this) |= 1<<static_cast<unsigned int>(author);
  }

  bool Muon_v1::isAuthor ( const Author author ) const{
    static Accessor< uint16_t > acc( "allAuthors" );
    return (acc(*this)& (1<<static_cast<unsigned int>(author)));
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::MuonType, muonType)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Muon_v1, uint16_t, Muon_v1::MuonType, muonType, setMuonType)

  bool Muon_v1::summaryValue(uint8_t& value, const SummaryType &information)  const {
    // Here we want to check if this information has been added to the Muon, and use this first if so.
    // @todo ?Could further optimise the below, to see first if the SummaryType value is one of the ones we write to Muons?
    // @todo ?Is there a better way than catching the exception?
    try {
      Muon_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information ); 
      value = ( *acc )( *this );
      return true;
    } catch ( SG::ExcBadAuxVar& ) {}
    
    // Okay - fallback: try to get from TrackParticle.
    const ElementLink< TrackParticleContainer >& el= primaryTrackParticleLink();
    if (!el.isValid()) return false;
    return (*el)->summaryValue(value,information);
  }  

  void Muon_v1::setSummaryValue( uint8_t  value, const SummaryType & 	information ) {
    Muon_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information ); ///FIXME!
    // Set the value:
    ( *acc )( *this ) = value;
  }
   
  bool Muon_v1::summaryValue(float& value, const SummaryType &information)  const {
    const ElementLink< TrackParticleContainer >& el= primaryTrackParticleLink();
    if (!el.isValid()) return false;
    return (*el)->summaryValue(value,information);
  }  

  // No set method for 'float' values as not expected to be needed
  
  bool Muon_v1::summaryValue(uint8_t& value, const MuonSummaryType &information)  const {
    Muon_v1::Accessor< uint8_t >* acc = muonTrackSummaryAccessorV1( information );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;
    
  // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  void Muon_v1::setSummaryValue(uint8_t value, const MuonSummaryType &information) {
    Muon_v1::Accessor< uint8_t >* acc = muonTrackSummaryAccessorV1( information );
    // Set the value:
    ( *acc )( *this ) =  value;
  }
  
  bool Muon_v1::parameter(float& value, const Muon_v1::ParamDef &information)  const {
    xAOD::Muon_v1::Accessor< float >* acc = parameterAccessorV1<float>( information );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;
    
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  void Muon_v1::setParameter(float& value, const Muon_v1::ParamDef &information){
    xAOD::Muon_v1::Accessor< float >* acc = parameterAccessorV1<float>( information );
    if( ! acc ) throw std::runtime_error("Muon_v1::setParameter - no accessor for paramdef number: "+information);
    
    // Set the value:
    ( *acc )( *this ) = value;
  }

  xAOD::Muon_v1::Quality Muon_v1::quality() const {
    static Accessor< uint8_t > acc( "quality" );
    uint8_t temp =  acc( *this );
    return static_cast<Quality>(temp&3);     
  }
  
  void Muon_v1::setQuality(xAOD::Muon_v1::Quality value) {
    static Accessor< uint8_t > acc( "quality" );
    uint8_t temp = static_cast< uint8_t >(value);
    acc( *this ) |= temp;
    return;      
  }
  
  bool Muon_v1::passesIDCuts() const {
    static Accessor< uint8_t > acc( "quality" );
    uint8_t temp =  acc( *this );
    // We use 4th bit for 'passesIDCuts'
    return temp&8;     
  }
  
  void Muon_v1::setPassesIDCuts(bool value) {
    static Accessor< uint8_t > acc( "quality" );
    // We use 4th bit for 'passesIDCuts'
    if (value) acc( *this ) |= 8;
    else       acc( *this ) &= 247;
    return;      
  }
  
  bool Muon_v1::passesHighPtCuts() const {
    static Accessor< uint8_t > acc( "quality" );
    uint8_t temp =  acc( *this );
    // We use 5th bit for 'passesHighPtCuts'
    return temp&16;    
  }
  
  void Muon_v1::setPassesHighPtCuts(bool value) {
    static Accessor< uint8_t > acc( "quality" );
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
  
  
  
  bool Muon_v1::isolation(float& value, const Iso::IsolationType &information)  const {
    SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    if( ! acc ) return false;
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
  
  void Muon_v1::setIsolation(float& value, const Iso::IsolationType &information){
    SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    // Set the value:
    ( *acc )( *this ) = value;
  }

  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, inDetTrackParticleLink)
  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, muonSpectrometerTrackParticleLink)
  AUXSTORE_OBJECT_GETTER( Muon_v1, ElementLink< TrackParticleContainer >, combinedTrackParticleLink)

  const ElementLink< TrackParticleContainer >& Muon_v1::primaryTrackParticleLink() const{
    MuonType type = muonType();
    switch ( type ) {
      case Combined :
      return combinedTrackParticleLink();
      break;
      case SegmentTagged :
      case CaloTagged :
      case SiliconAssociatedForwardMuon :
      return inDetTrackParticleLink();
      break;
      case MuonStandAlone :
      return muonSpectrometerTrackParticleLink();
      break;
      default:
      throw std::runtime_error("Unknown primary type - not sure which track particle to return!");
    }
    static ElementLink< TrackParticleContainer > dummy;
    return dummy;
  }
  
  const xAOD::TrackParticle* Muon_v1::primaryTrackParticle() const{
    //the ElementLink pointing to the primary track particle: 
    const ElementLink< TrackParticleContainer >& el = 
      primaryTrackParticleLink(); 
    // If it's invalid, return a null pointer: 
    if( ! el.isValid() ) { 
      return 0; 
    } 
    // If it's valid, let's de-reference it: 
    return *el; 
  }
  
  const ElementLink< TrackParticleContainer >& Muon_v1::trackParticleLink( Muon_v1::TrackParticleType type) const{
    switch ( type ) {
      case Primary :
        return primaryTrackParticleLink();
        break;
      case InnerDetectorTrackParticle :
        return inDetTrackParticleLink();
        break;
      case MuonSpectrometerTrackParticle :
        return muonSpectrometerTrackParticleLink();
        break;
      case CombinedTrackParticle :
        return combinedTrackParticleLink();
        break;
      default:
        throw std::runtime_error("Unknown TrackParticleType - not sure which track particle to return!");
    }
    static ElementLink< TrackParticleContainer > dummy;
    return dummy;
  }
  
  const xAOD::TrackParticle* Muon_v1::trackParticle( Muon_v1::TrackParticleType type) const{
    // Get the ElementLink pointing to the requested track particle:
    const ElementLink< TrackParticleContainer >& el =
      trackParticleLink( type );
    // If it's invalid, return a null pointer:
    if( ! el.isValid() ) {
      return 0;
    }
    // If it's valid, let's de-reference it:
    return *el;
  }

  void Muon_v1::setTrackParticleLink(TrackParticleType type, const ElementLink< TrackParticleContainer >& link){
    switch ( type ) {
      case InnerDetectorTrackParticle :
        static Accessor< ElementLink< TrackParticleContainer > > acc1( "inDetTrackParticleLink" );
        acc1(*this)=link;
        break;
      case MuonSpectrometerTrackParticle :
        static Accessor< ElementLink< TrackParticleContainer > > acc2( "muonSpectrometerTrackParticleLink" );
        acc2(*this)=link;
        break;
      case CombinedTrackParticle :
        static Accessor< ElementLink< TrackParticleContainer > > acc3( "combinedTrackParticleLink" );
        acc3(*this)=link;          
        break;
      case Primary :
      default:
        throw std::runtime_error("Unknown or Primary TrackParticleType - not sure which track particle to set!");
    }
  }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( Muon_v1, ElementLink<CaloClusterContainer>, clusterLink, setClusterLink)
  const CaloCluster* Muon_v1::cluster() const { 
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

  static SG::AuxElement::Accessor< std::vector< ElementLink< MuonSegmentContainer > > > muonSegmentsAcc( "muonSegmentLinks" ); 
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

