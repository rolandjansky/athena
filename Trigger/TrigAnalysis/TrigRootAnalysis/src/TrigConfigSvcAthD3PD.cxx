/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfigSvcAthD3PD.cxx 502443 2012-05-25 10:43:25Z krasznaa $

// ROOT include(s):
#include <TROOT.h>
#include <TSeqCollection.h>
#include <TFile.h>
#include <TTree.h>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigConfigSvcAthD3PD.h"

namespace D3PD {

   TrigConfigSvcAthD3PD::TrigConfigSvcAthD3PD( const std::string& name,
                                               ISvcLocator* svcloc )
      : AthService( name, svcloc ),
        m_svc() {

      declareProperty( "SMK",
                       m_smk = SG::RVar< unsigned int >( "trig_DB_SMK" ),
                       "Name of the Super Master Key variable" );
      declareProperty( "L1PSK",
                       m_l1psk = SG::RVar< unsigned int >( "trig_DB_L1PSK" ),
                       "Name of the LVL1 prescale key variable" );
      declareProperty( "HLTPSK",
                       m_hltpsk = SG::RVar< unsigned int >( "trig_DB_HLTPSK" ),
                       "Name of the HLT prescale key variable" );
   }

   StatusCode TrigConfigSvcAthD3PD::queryInterface( const InterfaceID& riid,
                                                    void** ppvInterface ) {

      if( ITrigConfigSvcD3PD::interfaceID().versionMatch( riid ) ) {
         *ppvInterface = static_cast< ITrigConfigSvcD3PD* >( this );
      } else if( IIncidentListener::interfaceID().versionMatch( riid ) ) {
         *ppvInterface = static_cast< IIncidentListener* >( this );
      } else {
         return AthService::queryInterface( riid, ppvInterface );
      }

      addRef();

      return StatusCode::SUCCESS;
   }

   StatusCode TrigConfigSvcAthD3PD::initialize() {

      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      //
      // Set up the incidents that the service should listen to:
      //
      ServiceHandle< IIncidentSvc > incidentSvc( "IncidentSvc", name() );
      CHECK( incidentSvc.retrieve() );
      incidentSvc->addListener( this, IncidentType::BeginEvent );
      // Unfortunately Begin Input File incident is never fired by the D3PD
      // reading services

      //
      // Get the input file list. This is a bit ugly, but we need to get
      // the names of the input files and the name of the main TTree from
      // the event selector service. Later on there may be a less convoluted
      // access to the metadata trees, but right now there isn't.
      //
      m_files.clear();
      m_tupleName = "";
      ServiceHandle< IJobOptionsSvc > jobOSvc( "JobOptionsSvc", name() );
      CHECK( jobOSvc.retrieve() );
      const std::vector< const Gaudi::Details::PropertyBase* >* evSelProp =
         jobOSvc->getProperties( "EventSelector" );
      if( ! evSelProp ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Didn't find the input file list";
         return StatusCode::FAILURE;
      }
      std::vector< const Gaudi::Details::PropertyBase* >::const_iterator itr = evSelProp->begin();
      std::vector< const Gaudi::Details::PropertyBase* >::const_iterator end = evSelProp->end();
      for( ; itr != end; ++itr ) {

         // Look for the "InputCollections" property:
         if( ( *itr )->name() == "InputCollections" ) {

            // Translate it to a list of file names:
            StringArrayProperty files;
            if( ! files.assign( **itr ) ) {
               REPORT_MESSAGE( MSG::FATAL )
                  << "The EventSelector.InputCollections property is of "
                  << "the wrong type";
               return StatusCode::FAILURE;
            }
            m_files = files.value();

         }
         // Also look for the "TupleName" property:
         else if( ( *itr )->name() == "TupleName" ) {

            // Translate to a simple string:
            StringProperty tupleName;
            if( ! tupleName.assign( **itr ) ) {
               REPORT_MESSAGE( MSG::FATAL )
                  << "The EventSelector.TupleName property is of "
                  << "the wrong type";
               return StatusCode::FAILURE;
            }
            m_tupleName = tupleName.value();

         }
      }
      if( ( ! m_files.size() ) || ( m_tupleName == "" ) ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Couldn't access the needed jobOption properties";
         return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
   }

   void TrigConfigSvcAthD3PD::handle( const Incident& inc ) {

      // Some debug information:
      ATH_MSG_DEBUG( "Incident received: type = " << inc.type()
                     << ", source = " << inc.source() );
      ATH_MSG_DEBUG( "SMK = " << *m_smk << ", L1PSK = " << *m_l1psk
                     << ", HLTPSK = " << *m_hltpsk );

      // Check if this configuration is available:
      if( ! m_svc.IsAvailable( *m_smk, *m_l1psk, *m_hltpsk ) ) {

         // If not, let's try to access the input file, and read in the
         // configuration from the metadata tree.

         // Check if one of the input files is open at the moment.
         // Notice that we need to iterate through the file list
         // backwards. This is because ROOT/Athena doesn't close the first
         // file after it opened the second one, and so on. So, the current
         // file is always the last one from the list that's already open.
         TSeqCollection* files = gROOT->GetListOfFiles();
         std::vector< std::string >::const_reverse_iterator file_itr =
            m_files.rbegin();
         std::vector< std::string >::const_reverse_iterator file_end =
            m_files.rend();
         TFile* file = 0;
         for( ; file_itr != file_end; ++file_itr ) {
            file =
               dynamic_cast< TFile* >( files->FindObject( file_itr->c_str() ) );
            if( ! file ) continue;
            ATH_MSG_DEBUG( "Found open file: " << file->GetName() );
            break;
         }
         if( ! file ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Couldn't find any of the input files being open!";
            return;
         }

         // Find the metadata tree:
         TTree* metaTree =
            dynamic_cast< TTree* >( file->Get( ( m_tupleName +
                                                 "Meta/TrigConfTree" ).c_str() ) );
         if( ! metaTree ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Couldn't access the trigger configuration metadata tree in file: "
               << file->GetName();
            return;
         }

         // Cache the configuration from the input file:
         if( ! m_svc.SetConfigTree( metaTree ) ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "There was a problem with caching the trigger configuration";
            return;
         }
      }

      // Finally, let's try to load the configuration:
      if( ! m_svc.Load( *m_smk, *m_l1psk, *m_hltpsk ) ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "Couldn't load configuration with SMK = "
            << *m_smk << ", L1PSK = " << *m_l1psk
            << ", HLTPSK = " << *m_hltpsk;
         return;
      }

      return;
   }

   ::Bool_t
   TrigConfigSvcAthD3PD::IsConfigured( const std::string& name ) const {

      return m_svc.IsConfigured( name );
   }

   ::Int_t TrigConfigSvcAthD3PD::GetCTPId( const std::string& name ) const {

      return m_svc.GetCTPId( name );
   }

   ::Int_t TrigConfigSvcAthD3PD::GetChainId( const std::string& name ) const {

      return m_svc.GetChainId( name );
   }

   ::Float_t
   TrigConfigSvcAthD3PD::GetPrescale( const std::string& name ) const {

      return m_svc.GetPrescale( name );
   }

   ::Float_t
   TrigConfigSvcAthD3PD::GetRerunPrescale( const std::string& name ) const {

      return m_svc.GetRerunPrescale( name );
   }

   ::Float_t
   TrigConfigSvcAthD3PD::GetPassthrough( const std::string& name ) const {

      return m_svc.GetPassthrough( name );
   }

   const std::string&
   TrigConfigSvcAthD3PD::GetLowerChainName( const std::string& name ) const {

      return m_svc.GetLowerChainName( name );
   }

   const std::string&
   TrigConfigSvcAthD3PD::GetNameFromCTPId( ::Int_t ctpid ) const {

      return m_svc.GetNameFromCTPId( ctpid );
   }

   const std::string&
   TrigConfigSvcAthD3PD::GetL2NameFromChainId( ::Int_t chainid ) const {

      return m_svc.GetL2NameFromChainId( chainid );
   }

   const std::string&
   TrigConfigSvcAthD3PD::GetEFNameFromChainId( ::Int_t chainid ) const {

      return m_svc.GetEFNameFromChainId( chainid );
   }

   const std::vector< std::string >&
   TrigConfigSvcAthD3PD::GetLVL1Items() const {

      return m_svc.GetLVL1Items();
   }

   const std::vector< std::string >&
   TrigConfigSvcAthD3PD::GetHLTChains() const {

      return m_svc.GetHLTChains();
   }

   void TrigConfigSvcAthD3PD::PrintSummary() const {

      m_svc.PrintSummary();
      return;
   }

   void TrigConfigSvcAthD3PD::PrintConfig() const {

      m_svc.PrintConfig();
      return;
   }

} // namespace D3PD
