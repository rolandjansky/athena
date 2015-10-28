/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenu_v1.cxx 647153 2015-02-16 13:34:19Z krasznaa $

// System include(s):
#include <iostream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TriggerMenu_v1.h"

namespace xAOD {

   TriggerMenu_v1::TriggerMenu_v1()
      : SG::AuxElement() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //   Implementation of the configuration identifier accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerMenu_v1, uint32_t, smk,
                                         setSMK )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerMenu_v1, uint32_t, l1psk,
                                         setL1psk )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerMenu_v1, uint32_t, hltpsk,
                                         setHLTpsk )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //       Implementation of the LVL1 configuration accessor functions
   //

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< uint16_t >,
                                      itemCtpIds, setItemCtpIds )

   bool TriggerMenu_v1::itemCtpIdsAvailable() const {

      static Accessor< std::vector< uint16_t > > acc( "itemCtpIds" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1,
                                      std::vector< std::string >,
                                      itemNames, setItemNames )

   bool TriggerMenu_v1::itemNamesAvailable() const {

      static Accessor< std::vector< std::string > > acc( "itemNames" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< float >,
                                      itemPrescales, setItemPrescales )

   bool TriggerMenu_v1::itemPrescalesAvailable() const {

      static Accessor< std::vector< float > > acc( "itemPrescales" );
      return acc.isAvailable( *this );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //         Implementation of the HLT configuration accessor functions
   //

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< uint16_t >,
                                      chainIds, setChainIds )

   bool TriggerMenu_v1::chainIdsAvailable() const {

      static Accessor< std::vector< uint16_t > > acc( "chainIds" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1,
                                      std::vector< std::string >,
                                      chainNames, setChainNames )

   bool TriggerMenu_v1::chainNamesAvailable() const {

      static Accessor< std::vector< std::string > > acc( "chainNames" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1,
                                      std::vector< std::string >,
                                      chainParentNames, setChainParentNames )

   bool TriggerMenu_v1::chainParentNamesAvailable() const {

      static Accessor< std::vector< std::string > > acc( "chainParentNames" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< float >,
                                      chainPrescales, setChainPrescales )

   bool TriggerMenu_v1::chainPrescalesAvailable() const {

      static Accessor< std::vector< float > > acc( "chainPrescales" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< float >,
                                      chainRerunPrescales,
                                      setChainRerunPrescales )

   bool TriggerMenu_v1::chainRerunPrescalesAvailable() const {

      static Accessor< std::vector< float > > acc( "chainRerunPrescales" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< float >,
                                      chainPassthroughPrescales,
                                      setChainPassthroughPrescales )

   bool TriggerMenu_v1::chainPassthroughPrescalesAvailable() const {

      static Accessor< std::vector< float > >
         acc( "chainPassthroughPrescales" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< std::vector< uint32_t> >,
                                      chainSignatureCounters, setChainSignatureCounters )

   bool TriggerMenu_v1::chainSignatureCountersAvailable() const {

      static Accessor< std::vector< std::vector< uint32_t> > > acc( "chainSignatureCounters" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< std::vector< int > >,
                                      chainSignatureLogics, setChainSignatureLogics )

   bool TriggerMenu_v1::chainSignatureLogicsAvailable() const {

      static Accessor< std::vector< std::vector< int > > > acc( "chainSignatureLogics" );
      return acc.isAvailable( *this );
   }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector<std::vector< std::vector< std::string> > >,
                                      chainSignatureOutputTEs, setChainSignatureOutputTEs )

   bool TriggerMenu_v1::chainSignatureOutputTEsAvailable() const {
     
    static Accessor< std::vector<std::vector< std::vector< std::string> > > > acc( "chainSignatureOutputTEs" );
     return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector<std::vector< std::string > >,
				      chainSignatureLabels, setChainSignatureLabels )

   bool TriggerMenu_v1::chainSignatureLabelsAvailable() const {

      static Accessor< std::vector< std::vector< std::string > > > acc( "chainSignatureLabels" );
      return acc.isAvailable( *this );
   }


   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the HLT sequence accessor functions
   //

   
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< std::vector< std::string > >,
                                       sequenceInputTEs, setSequenceInputTEs )

   bool TriggerMenu_v1::sequenceInputTEsAvailable() const {

      static Accessor< std::vector< std::vector< std::string > > > acc( "sequenceInputTEs" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< std::string >,
                                      sequenceOutputTEs, setSequenceOutputTEs )

   bool TriggerMenu_v1::sequenceOutputTEsAvailable() const {

      static Accessor< std::vector< std::string > > acc( "sequenceOutputTEs" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1, std::vector< std::vector< std::string > >,
                                      sequenceAlgorithms, setSequenceAlgorithms )

   bool TriggerMenu_v1::sequenceAlgorithmsAvailable() const {
     
     static Accessor< std::vector<std::vector< std::string > > > acc( "sequenceAlgorithms" );
     return acc.isAvailable( *this );
   }



   //
   /////////////////////////////////////////////////////////////////////////////


   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the bunch group accessor functions
   //

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerMenu_v1,
                                      TriggerMenu_v1::BunchGroup_t,
                                      bunchGroupBunches, setBunchGroupBunches )

   bool TriggerMenu_v1::bunchGroupBunchesAvailable() const {

      static Accessor< BunchGroup_t > acc( "bunchGroupBunches" );
      return acc.isAvailable( *this );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
