/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagging_v1.cxx 797330 2017-02-15 14:25:13Z guirriec $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODBTagging/versions/BTagging_v1.h"
#include "BTaggingAccessors_v1.h"
#include "xAODBTagging/BTaggingEnums.h"

namespace xAOD {

   BTagging_v1::BTagging_v1(){
      

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the SV0 accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double,
                                         SV0_significance3D,
                                         setSV0_significance3D )

   // The accessor object(s):
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      sv0TPAcc1( "SV0_TrackParticles" );
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      sv0TPAcc2( "SV0_TrackParticleLinks" );

   const BTagging_v1::TPELVec_t& BTagging_v1::SV0_TrackParticleLinks() const {

      if( sv0TPAcc1.isAvailable( *this ) ) {
         return sv0TPAcc1( *this );
      }
      return sv0TPAcc2( *this );
   }

   void BTagging_v1::setSV0_TrackParticleLinks( const TPELVec_t& value ) {

      sv0TPAcc2( *this ) = value;
      return;
   }

   const xAOD::TrackParticle* BTagging_v1::SV0_TrackParticle( size_t i ) const {

      if( ( ! sv0TPAcc1.isAvailable( *this ) ) &&
          ( ! sv0TPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      const TPELVec_t& tpl = SV0_TrackParticleLinks();
      if( tpl.size() <= i ) {
         return 0;
      }
      if( ! tpl[ i ].isValid() ) {
         return 0;
      }
      return *( tpl[ i ] );
   }

   size_t BTagging_v1::nSV0_TrackParticles() const {

      if( ( ! sv0TPAcc1.isAvailable( *this ) ) &&
          ( ! sv0TPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      return SV0_TrackParticleLinks().size();
   }

   void BTagging_v1::
   addSV0_TrackParticle( const ElementLink< xAOD::TrackParticleContainer >& link ) {

      sv0TPAcc2( *this ).push_back( link );
      return;
   }

   void BTagging_v1::clearSV0_TrackParticleLinks() {

      sv0TPAcc2( *this ).clear();
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the SV1 accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, SV1_pb,
                                         setSV1_pb )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, SV1_pu,
                                         setSV1_pu )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, SV1_pc,
                                         setSV1_pc )

   // The accessor object(s):
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      sv1TPAcc1( "SV1_TrackParticles" );
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      sv1TPAcc2( "SV1_TrackParticleLinks" );

   const BTagging_v1::TPELVec_t& BTagging_v1::SV1_TrackParticleLinks() const {

      if( sv1TPAcc1.isAvailable( *this ) ) {
         return sv1TPAcc1( *this );
      }
      return sv1TPAcc2( *this );
   }

   void BTagging_v1::setSV1_TrackParticleLinks( const TPELVec_t& value ) {

      sv1TPAcc2( *this ) = value;
      return;
   }

   const xAOD::TrackParticle* BTagging_v1::SV1_TrackParticle( size_t i ) const {

      if( ( ! sv1TPAcc1.isAvailable( *this ) ) &&
          ( ! sv1TPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      const TPELVec_t& tpl = SV1_TrackParticleLinks();
      if( tpl.size() <= i ) {
         return 0;
      }
      if( ! tpl[ i ].isValid() ) {
         return 0;
      }
      return *( tpl[ i ] );
   }

   size_t BTagging_v1::nSV1_TrackParticles() const {

      if( ( ! sv1TPAcc1.isAvailable( *this ) ) &&
          ( ! sv1TPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      return SV1_TrackParticleLinks().size();
   }

   void BTagging_v1::
   addSV1_TrackParticle( const ElementLink< xAOD::TrackParticleContainer >& link ) {

      sv1TPAcc2( *this ).push_back( link );
      return;
   }

   void BTagging_v1::clearSV1_TrackParticleLinks() {

      sv1TPAcc2( *this ).clear();
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the IP2D accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, IP2D_pb,
                                         setIP2D_pb )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, IP2D_pu,
                                         setIP2D_pu )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, IP2D_pc,
                                         setIP2D_pc )

   // The accessor object(s):
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      ip2dTPAcc1( "IP2D_TrackParticles" );
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      ip2dTPAcc2( "IP2D_TrackParticleLinks" );

   const BTagging_v1::TPELVec_t& BTagging_v1::IP2D_TrackParticleLinks() const {

      if( ip2dTPAcc1.isAvailable( *this ) ) {
         return ip2dTPAcc1( *this );
      }
      return ip2dTPAcc2( *this );
   }

   void BTagging_v1::setIP2D_TrackParticleLinks( const TPELVec_t& value ) {

      ip2dTPAcc2( *this ) = value;
      return;
   }

   const xAOD::TrackParticle*
   BTagging_v1::IP2D_TrackParticle( size_t i ) const {

      if( ( ! ip2dTPAcc1.isAvailable( *this ) ) &&
          ( ! ip2dTPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      const TPELVec_t& tpl = IP2D_TrackParticleLinks();
      if( tpl.size() <= i ) {
         return 0;
      }
      if( ! tpl[ i ].isValid() ) {
         return 0;
      }
      return *( tpl[ i ] );
   }

   size_t BTagging_v1::nIP2D_TrackParticles() const {

      if( ( ! ip2dTPAcc1.isAvailable( *this ) ) &&
          ( ! ip2dTPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      return IP2D_TrackParticleLinks().size();
   }

   void BTagging_v1::
   addIP2D_TrackParticle( const ElementLink< xAOD::TrackParticleContainer >& link ) {

      ip2dTPAcc2( *this ).push_back( link );
      return;
   }

   void BTagging_v1::clearIP2D_TrackParticleLinks() {

      ip2dTPAcc2( *this ).clear();
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////
   
   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the IP3D accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, IP3D_pb,
                                         setIP3D_pb )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, IP3D_pu,
                                         setIP3D_pu )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, IP3D_pc,
                                         setIP3D_pc )

   // The accessor object(s):
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      ip3dTPAcc1( "IP3D_TrackParticles" );
   static const SG::AuxElement::Accessor< BTagging_v1::TPELVec_t >
      ip3dTPAcc2( "IP3D_TrackParticleLinks" );

   const BTagging_v1::TPELVec_t& BTagging_v1::IP3D_TrackParticleLinks() const {

      if( ip3dTPAcc1.isAvailable( *this ) ) {
         return ip3dTPAcc1( *this );
      }
      return ip3dTPAcc2( *this );
   }

   void BTagging_v1::setIP3D_TrackParticleLinks( const TPELVec_t& value ) {

      ip3dTPAcc2( *this ) = value;
      return;
   }

   const xAOD::TrackParticle*
   BTagging_v1::IP3D_TrackParticle( size_t i ) const {

      if( ( ! ip3dTPAcc1.isAvailable( *this ) ) &&
          ( ! ip3dTPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      const TPELVec_t& tpl = IP3D_TrackParticleLinks();
      if( tpl.size() <= i ) {
         return 0;
      }
      if( ! tpl[ i ].isValid() ) {
         return 0;
      }
      return *( tpl[ i ] );
   }

   size_t BTagging_v1::nIP3D_TrackParticles() const {

      if( ( ! ip3dTPAcc1.isAvailable( *this ) ) &&
          ( ! ip3dTPAcc2.isAvailable( *this ) ) ) {
         return 0;
      }
      return IP3D_TrackParticleLinks().size();
   }

   void BTagging_v1::
   addIP3D_TrackParticle( const ElementLink< xAOD::TrackParticleContainer >& link ) {

      ip3dTPAcc2( *this ).push_back( link );
      return;
   }

   void BTagging_v1::clearIP3D_TrackParticleLinks() {

      ip3dTPAcc2( *this ).clear();
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the JetFitter accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, JetFitter_pb,
                                         setJetFitter_pb )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, JetFitter_pu,
                                         setJetFitter_pu )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, JetFitter_pc,
                                         setJetFitter_pc )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the JetFitterComb accessor functions
   //

   //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, JetFitterCombNN_pb,
   //                                      setJetFitterCombNN_pb )
   //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, JetFitterCombNN_pu,
   //                                      setJetFitterCombNN_pu )
   //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, JetFitterCombNN_pc,
   //                                      setJetFitterCombNN_pc )

   //
   /////////////////////////////////////////////////////////////////////////////



   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of generic tagger weight accessor functions
   //                - returned value is the second argument
   //                - optional 3rd and 4th arguments for defining signal 
   //                and background hypotheses (default: b and light)

   bool BTagging_v1::loglikelihoodratio( const std::string& taggername, 
                                         double &value, // return value
                                         const std::string& signal,
                                         const std::string& bckgd ) const {
     value = -1.; // default value if tagger is undefined
     double pu = 1.;
     double pb = 1.;
     bool puvalid = variable<double>(taggername, bckgd , pu);
     bool pbvalid = variable<double>(taggername, signal, pb);
     if( !pbvalid || !puvalid ) return false;
     if("IP3D"==taggername&&pb==1.&&pu==1.e9) {
       value = 0.;
     } else {
       value = this->calcLLR(pb,pu);
     }
     return true;
   }


   double BTagging_v1::calcLLR(double numerator, double denominator) const {
     double val = 0.;
     if(numerator<=0.) {
       val = -30.;
     } else if(denominator<=0.) {
       val = +100.;
     } else {
       val = log(numerator/denominator);
     }
     return val;
   }
   
   bool BTagging_v1::pu(const std::string& taggername, double &value) const {
     return variable<double>(taggername, "pu", value);
   } 

   bool BTagging_v1::pb(const std::string& taggername, double &value) const {
     return variable<double>(taggername, "pb", value);
   }
 
   bool BTagging_v1::pc(const std::string& taggername, double &value) const {
     return variable<double>(taggername, "pc", value);
   }
 
   bool BTagging_v1::MVx_discriminant(const std::string& taggername, double &value) const {
     return variable<double>(taggername, "discriminant", value);
   }
 
   //
   /////////////////////////////////////////////////////////////////////////////


   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagging_v1, double, MV1_discriminant,
                                         setMV1_discriminant )

   /////////////////////////////////////////////////////////////////////////////
   //
   //      Implementation of the generic tagger info accessor functions
   //

   bool BTagging_v1::taggerInfo( int& value, const BTagInfo info ) const {

      const Accessor< int >* acc = taggerInfoAccessorV1int( info );
      if( ! acc ) return false;
      if( ! acc->isAvailable( *this ) ) return false;
      value = ( *acc )( *this );
      return true;
   }

   bool BTagging_v1::taggerInfo( float& value, const BTagInfo info) const {

      const Accessor< float >* acc = taggerInfoAccessorV1float( info );
      if( ! acc ) return false;
      if( ! acc->isAvailable( *this ) ) return false;
      value = ( *acc )( *this );
      return true;
   }

   bool BTagging_v1::taggerInfo( bool& value, const BTagInfo info ) const {

      const Accessor< uint8_t >* acc = taggerInfoAccessorV1uint8_t( info );
      if( ! acc ) return false;
      if( ! acc->isAvailable( *this ) ) return false;
      value = ( *acc )( *this );
      return true;
   }

   bool BTagging_v1::taggerInfo( std::string& value,
                                 const BTagInfo info ) const {

      const Accessor< std::string >* acc = taggerInfoAccessorV1string( info );
      if( ! acc ) return false;
      if( ! acc->isAvailable( *this ) ) return false;
      value = ( *acc )( *this );
      return true;
   }

   bool BTagging_v1::taggerInfo( std::vector<int>& value,
                                 const BTagInfo info ) const {

      const Accessor< std::vector<int> >* acc = taggerInfoAccessorV1vectorOfInts( info );
      if( ! acc ) return false;
      if( ! acc->isAvailable( *this ) ) return false;
      value = ( *acc )( *this );
      return true;
   }

   bool BTagging_v1::taggerInfo( std::vector<float>& value,
                                 const BTagInfo info ) const {

      const Accessor< std::vector<float> >* acc = taggerInfoAccessorV1vectorOfFloats( info );
      if( ! acc ) return false;
      if( ! acc->isAvailable( *this ) ) return false;
      value = ( *acc )( *this );
      return true;
   }

   bool BTagging_v1::taggerInfo( std::vector<bool>& value,
                                 const BTagInfo info ) const {

      const Accessor< std::vector<bool> >* acc = taggerInfoAccessorV1vectorOfBools( info );
      if( ! acc ) return false;
      if( ! acc->isAvailable( *this ) ) return false;
      value = ( *acc )( *this );
      return true;
   }

   void BTagging_v1::setTaggerInfo( int value, const BTagInfo info ) {

      const Accessor< int >* acc = taggerInfoAccessorV1int( info );
      if( ! acc ) return;
      ( *acc )( *this ) = value;
      return;
   }

   void BTagging_v1::setTaggerInfo( float value, const BTagInfo info ) {

      const Accessor< float >* acc = taggerInfoAccessorV1float( info );
      if( ! acc ) return;
      ( *acc )( *this ) = value;
      return;
   }

   void BTagging_v1::setTaggerInfo( bool value, const BTagInfo info ) {

      const Accessor< uint8_t >* acc = taggerInfoAccessorV1uint8_t( info );
      if( ! acc ) return;
      ( *acc )( *this ) = uint8_t(value);
      return;
   }

   void BTagging_v1::setTaggerInfo( const std::string& value,
                                    const BTagInfo info ) {

      const Accessor< std::string >* acc = taggerInfoAccessorV1string( info );
      if( ! acc ) return;
      ( *acc )( *this ) = value;
      return;
   }

   void BTagging_v1::setTaggerInfo( const std::vector<int>& value,
                                    const BTagInfo info ) {

      const Accessor< std::vector<int> >* acc = taggerInfoAccessorV1vectorOfInts( info );
      if( ! acc ) return;
      ( *acc )( *this ) = value;
      return;
   }

   void BTagging_v1::setTaggerInfo( const std::vector<float>& value,
                                    const BTagInfo info ) {

      const Accessor< std::vector<float> >* acc = taggerInfoAccessorV1vectorOfFloats( info );
      if( ! acc ) return;
      ( *acc )( *this ) = value;
      return;
   }

   void BTagging_v1::setTaggerInfo( const std::vector<bool>& value,
                                    const BTagInfo info ) {

      const Accessor< std::vector<bool> >* acc = taggerInfoAccessorV1vectorOfBools( info );
      if( ! acc ) return;
      ( *acc )( *this ) = value;
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////
 
 
  static std::map<std::string, SG::AuxElement::Accessor<BTagging_v1::TPELVec_t> > DynTPELVec;
  static std::map<std::string, SG::AuxElement::Accessor<BTagging_v1::VxELVec_t> > DynVxELVec;
  static std::map<std::string, SG::AuxElement::Accessor<BTagging_v1::BTagVxELVec_t> > DynBTagVxELVec;

   void BTagging_v1::toPersistent ATLAS_NOT_REENTRANT () {

      TPELVec_t::iterator itr;
      TPELVec_t::iterator end;

      if( sv0TPAcc2.isAvailableWritable( *this ) ) {
         itr = sv0TPAcc2( *this ).begin();
         end = sv0TPAcc2( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      if( sv1TPAcc2.isAvailableWritable( *this ) ) {
         itr = sv1TPAcc2( *this ).begin();
         end = sv1TPAcc2( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      if( ip2dTPAcc2.isAvailableWritable( *this ) ) {
         itr = ip2dTPAcc2( *this ).begin();
         end = ip2dTPAcc2( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      if( ip3dTPAcc2.isAvailableWritable( *this ) ) {
         itr = ip3dTPAcc2( *this ).begin();
         end = ip3dTPAcc2( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      std::map<std::string, SG::AuxElement::Accessor<BTagging_v1::TPELVec_t> >::iterator dyniter = DynTPELVec.begin();

      for(; dyniter != DynTPELVec.end(); ++dyniter){

        if( dyniter->second.isAvailableWritable( *this ) ) {
          itr = dyniter->second( *this ).begin();
          end = dyniter->second( *this ).end();
          for( ; itr != end; ++itr ) {
            itr->toPersistent();
          }
        }
      }


      VxELVec_t::iterator vxitr;
      VxELVec_t::iterator vxend;

      std::map<std::string, SG::AuxElement::Accessor<BTagging_v1::VxELVec_t> >::iterator dynvxiter = DynVxELVec.begin();

      for(; dynvxiter != DynVxELVec.end(); ++dynvxiter){

        if( dynvxiter->second.isAvailableWritable( *this ) ) {
          vxitr = dynvxiter->second( *this ).begin();
          vxend = dynvxiter->second( *this ).end();
          for( ; vxitr != vxend; ++vxitr ) {
            vxitr->toPersistent();
          }
        }
      }

      BTagVxELVec_t::iterator btagvxitr;
      BTagVxELVec_t::iterator btagvxend;

      std::map<std::string, SG::AuxElement::Accessor<BTagging_v1::BTagVxELVec_t> >::iterator dynbtagvxiter = DynBTagVxELVec.begin();

      for(; dynbtagvxiter != DynBTagVxELVec.end(); ++dynbtagvxiter){

        if( dynbtagvxiter->second.isAvailableWritable( *this ) ) {
          btagvxitr = dynbtagvxiter->second( *this ).begin();
          btagvxend = dynbtagvxiter->second( *this ).end();
          for( ; btagvxitr != btagvxend; ++btagvxitr ) {
            btagvxitr->toPersistent();
          }
        }
      }

      return;
   }

  
  void BTagging_v1::setDynTPELName ATLAS_NOT_REENTRANT ( const std::string &taggername,
				   const std::string &variablename) {
    
    std::string varname = taggername+ "_" + variablename;
    const Accessor< TPELVec_t > acc( varname );
    DynTPELVec.insert(std::make_pair(varname,acc));
    return;
  }

  void BTagging_v1::setDynVxELName ATLAS_NOT_REENTRANT ( const std::string &taggername,
				    const std::string &variablename) {
    
    std::string varname = taggername+ "_" + variablename;
    const Accessor< VxELVec_t > acc( varname );
    DynVxELVec.insert(std::make_pair(varname,acc));
    return;
  }


  void BTagging_v1::setDynBTagVxELName ATLAS_NOT_REENTRANT ( const std::string &taggername,
					const std::string &variablename) {
    
    std::string varname = taggername+ "_" + variablename;
    const Accessor< BTagVxELVec_t > acc( varname );
    DynBTagVxELVec.insert(std::make_pair(varname,acc));
    return;
  }


} // namespace xAOD
