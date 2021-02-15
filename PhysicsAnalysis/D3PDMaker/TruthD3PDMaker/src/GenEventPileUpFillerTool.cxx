/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventPileUpFillerTool.cxx 504001 2012-06-05 16:30:11Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "GeneratorObjects/McEventCollection.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"

// Local include(s):
#include "GenEventPileUpFillerTool.h"

namespace D3PD {

   GenEventPileUpFillerTool::GenEventPileUpFillerTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
      : BlockFillerTool< HepMC::GenEvent >( type, name, parent ),
        m_registry( "dummy" ) {

      declareProperty( "GetterLabel", m_getterLabel = "" );
      declareProperty( "CollectionGetterRegistry", m_registry );

      book().ignore();  // Avoid coverity warnings.
   }

   StatusCode GenEventPileUpFillerTool::configureD3PD( IAddVariable* tree,
                                                       const std::type_info& ti ) {

      CHECK ( BlockFillerTool< HepMC::GenEvent >::configureD3PD( tree, ti ) );

      CHECK( m_registry.retrieve() );

      CHECK( m_registry->get( m_getterLabel, this, m_getter ) );

      return StatusCode::SUCCESS;
   }

   StatusCode GenEventPileUpFillerTool::book() {

      CHECK( addVariable( "nparticle", m_nparticle,
                          "Number of particles in the event" ) );
      CHECK( addVariable( "pileUpType", m_pileUpType,
                          "0 = signal MC Event; 1 = in-time pileup MC Event; "
                          "2 = out-of-time pileup MC Event in [-2BC, +2BC]; "
                          "3 = the out-of-time pileup MC Event in [-800ns, -2BC] and [+2BC, +800ns]; "
                          "4 = cavern background MC Event; 5 = dummy event used to separate types; "
                          "-1 = not filled" ) );

      return StatusCode::SUCCESS;
   }

   ///// pileup type is defined as described here:
   ///// https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PileupPerformance#MC_Truth_Task_Force_Recommendati
   /// pileUpType=0: the signal MC Event    
   /// pileUpType=1: the in-time pileup MC Events   
   /// pileUpType=2: the out-of-time pileup MC Events in [-2BC, +2BC]
   /// pileUpType=3: the out-of-time pileup MC Events in [-800ns, -2BC] and  [+2BC, +800ns]
   /// pileUpType=4: the cavern background MC Events 
   /// pileUpType=5: dummy event used to separate types
   /// pileUpType=-1: not filled
   StatusCode GenEventPileUpFillerTool::fill( const HepMC::GenEvent& p ) {
#ifdef HEPMC3
      *m_nparticle = p.particles().size();
#else
      *m_nparticle = p.particles_size();
#endif

      *m_pileUpType = -1;

      if( ( p.event_number() == -1 ) && ( HepMC::signal_process_id(p) == 0 ) ) {
         *m_pileUpType = 5;
         return StatusCode::SUCCESS;
      }

      const McEventCollection* mc = m_getter->get< McEventCollection >( false );
  
      if( ! mc ) {
         ATH_MSG_WARNING( "could not get mc collection" );
         return StatusCode::SUCCESS;
      }

      int gotzero = 1;
      McEventCollection::const_iterator iter = mc->begin();
      McEventCollection::const_iterator end = mc->end();
 
      if( &p == *iter ) {
         *m_pileUpType = 0;
         return StatusCode::SUCCESS;
      }

      for( ;iter != end; ++iter ) {

         if( ( ( ( *iter )->event_number() == -1 ) &&
               ( HepMC::signal_process_id(( *iter )) == 0 ) ) ) {
            ++gotzero;
         }
         if( &p == *iter ) break;
      }

      *m_pileUpType = gotzero;

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
