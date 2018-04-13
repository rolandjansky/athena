/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagValidationTest.h"
#include "GaudiKernel/Property.h"

using std::string;

//**********************************************************************
namespace Trig{

  TrigBtagValidationTest::TrigBtagValidationTest(const std::string& name, 
					       ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ),
      m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
      m_emulationTool("Trig::TrigBtagEmulationTool/TrigBtagEmulationTool",this),
      m_retrieveRetaggedJets(false),
      m_chain("HLT_j15_gsc35_boffperf_split"),
      m_eventCount(0),
      m_min_eventCount(-1),
      m_max_eventCount(-1)
  {
    declareProperty("TrigBtagEmulationTool",m_emulationTool        );
    declareProperty("ToBeEmulatedTriggers" ,m_toBeEmulatedTriggers );
    declareProperty("RetrieveRetaggedJets" ,m_retrieveRetaggedJets );
    declareProperty("InputChain"           ,m_chain                );
    declareProperty("MinEvent"             ,m_min_eventCount       );
    declareProperty("MaxEvent"             ,m_max_eventCount       );
  }
  
  //**********************************************************************
  
  TrigBtagValidationTest::~TrigBtagValidationTest() { }
  
  //**********************************************************************
  
  StatusCode TrigBtagValidationTest:: initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");
    
    // RETRIEVE TOOLS
    if ( (m_trigdec.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
      return StatusCode::FAILURE;
    }
    if ( (m_emulationTool.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Emulation Tool! Can't work");
      return StatusCode::FAILURE;
    }
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
      return sc;
    }

    ATH_MSG_INFO("Retrieved tools...");

    // CHAIN CONFIGURATION IN ATHENA IS PERFORMED VIA JOB OPTION

    ATH_MSG_INFO("Initializing TrigBtagEmulationTool ...");
    if( m_emulationTool->initialize().isFailure() ) {
      ATH_MSG_ERROR( "Unable to initialize TrigBtagEmulationTool" );
      return StatusCode::FAILURE;
    }


    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++) {
      std::string triggerName = m_toBeEmulatedTriggers.at(index);
      m_counters[ triggerName.c_str() ] = std::make_tuple( 0,0,0,0,0 );
    }
    
    return StatusCode::SUCCESS;
  }
  
  //**********************************************************************
  
  StatusCode TrigBtagValidationTest::finalize() {

    ATH_MSG_INFO("TrigBtagEmulationTool::finalize()");
    ATH_MSG_INFO("Trigger Validation Outcome");

    long int TotalMismatches = 0;
    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++) {
      std::string triggerName = m_toBeEmulatedTriggers.at(index);
      std::string message = Form("  --- Trigger %s : TDT = %ld [EMUL = %ld] : mismatches = %ld [TDT=1;EMUL=0 = %ld] [TDT=0;EMUL=1 = %ld]",
				 triggerName.c_str(),
				 std::get<TDT>( m_counters[ triggerName.c_str() ] ),
				 std::get<EMUL>( m_counters[ triggerName.c_str() ] ),
				 std::get<mismatchesTOT>( m_counters[ triggerName.c_str() ] ),
				 std::get<mismatchesTDT1EMUL0>( m_counters[ triggerName.c_str() ] ),
				 std::get<mismatchesTDT0EMUL1>( m_counters[ triggerName.c_str() ] ) );
				 
      ATH_MSG_INFO(message.c_str());
      TotalMismatches += std::get<mismatchesTOT>( m_counters[ triggerName.c_str() ] );
    }
    ATH_MSG_INFO("Total Mismatches : " << TotalMismatches);
    
    return StatusCode::SUCCESS;
  }
  
  //**********************************************************************
  StatusCode TrigBtagValidationTest::execute() {

    ATH_MSG_DEBUG( "TrigBtagEmulationTool::execute()" );

    if ( m_eventCount < m_min_eventCount ) return StatusCode::SUCCESS;
    if ( m_max_eventCount != -1 && m_eventCount >= m_max_eventCount ) return StatusCode::SUCCESS;
    ATH_MSG_INFO( "Processing Event n. " << m_eventCount );
    m_eventCount++;

    // CHECK RESUT FOR INDIVIDUAL CHAINS
    for (unsigned int index(0); index < m_toBeEmulatedTriggers.size(); index++) {
      std::string triggerName = m_toBeEmulatedTriggers.at(index);
      std::string message = Form( "TRIGGER=%s ",triggerName.c_str() );	
      
      bool passTDT  = m_trigdec->isPassed( triggerName );
      bool passEmul = m_emulationTool->isPassed( triggerName );

      if (passTDT) std::get<TDT>( m_counters[ triggerName.c_str() ] )++;
      if (passEmul) std::get<EMUL>( m_counters[ triggerName.c_str() ] )++;
      if (passTDT != passEmul) std::get<mismatchesTOT>( m_counters[ triggerName.c_str() ] )++;
      if (passTDT && !passEmul) std::get<mismatchesTDT1EMUL0>( m_counters[ triggerName.c_str() ] )++;
      if (!passTDT && passEmul) std::get<mismatchesTDT0EMUL1>( m_counters[ triggerName.c_str() ] )++;
       
      message += Form("TDT=%d EMUL=%d MISMATCH=%d",passTDT?1:0,passEmul?1:0,passTDT != passEmul?1:0);
      ATH_MSG_INFO( message.c_str() );
    }
    
    ATH_MSG_INFO( "" );

    if ( !m_retrieveRetaggedJets ) return StatusCode::SUCCESS;


    // RETRIEVE ORIGINAL JETS VIA NAVIGATION
    ATH_MSG_INFO( "Retrieving GSC original Jets" );
    std::vector< const xAOD::Jet* > originalJets;
    std::vector< const xAOD::BTagging* > originalBtagging;

    Trig::FeatureContainer features = m_trigdec->features(m_chain);
    const std::vector< Trig::Combination >& combinations = features.getCombinations();

    for ( Trig::Combination combo : combinations ) {
      getFromCombo( originalJets     ,combo ,"GSCJet" );
      getFromCombo( originalBtagging ,combo );
    }

    // RETRIEVE RETAGGED JETS
    ATH_MSG_INFO( "Retrieving GSC retagged Jets" );
    const xAOD::JetContainer *retaggedJets = m_emulationTool->retaggedJets("GSC");

    // *** Check size of the jets we are going to compare.
    ATH_MSG_INFO( "Retrieved " << originalJets.size() << " original GSC jets and "<< retaggedJets->size() << " retagged GSC jets" );
    if (originalJets.size() != retaggedJets->size()) {
      ATH_MSG_ERROR( "Original and Retagged jets have different sizes." );
      delete retaggedJets;
      return StatusCode::FAILURE;
    }

    std::vector< const xAOD::Jet* >::const_iterator itJet = originalJets.begin();
    std::vector< const xAOD::BTagging* >::const_iterator itBtagging = originalBtagging.begin();

    for ( const xAOD::Jet* jet : *retaggedJets ) {
      std::unique_ptr< const xAOD::BTagging > bjet( jet->btagging() );
      ATH_MSG_INFO( "Dumping ORIGINAL and RETAGGED jet collections" );
      ATH_MSG_INFO( "  -- GSC Jet : pt="<< jet->p4().Et()*1e-03 << " eta="<< jet->eta() <<" phi="<< jet->phi() );

      double value_mv2c10  = -1;
      double value_mv2c20  = -1;
      double value_ip3dsv1 = -1000;
      double value_comb    = -1000;

      bjet->variable< double >("MV2c10"  ,"discriminant" ,value_mv2c10 );
      bjet->variable< double >("MV2c20"  ,"discriminant" ,value_mv2c20 );
      bjet->variable< double >("IP3DSV1" ,"discriminant" ,value_ip3dsv1);
      bjet->variable< double >("COMB"    ,"discriminant" ,value_comb   );

      ATH_MSG_INFO( "    ** MV2C10  = " << value_mv2c10  );
      ATH_MSG_INFO( "    ** MV2C20  = " << value_mv2c20  );
      ATH_MSG_INFO( "    ** IP3DSV1 = " << value_ip3dsv1 );
      ATH_MSG_INFO( "    ** COMB    = " << value_comb    );

      // *** 

      const xAOD::Jet *referenceJet = *itJet;
      const xAOD::BTagging *referenceBTagging = *itBtagging;
      ATH_MSG_INFO( "  -- ORIGINAL GSC Jet : pt="<< referenceJet->p4().Et()*1e-03 << " eta="<< referenceJet->eta() <<" phi="<< referenceJet->phi() );

      double reference_mv2c10  = -1;
      double reference_mv2c20  = -1;
      double reference_ip3dsv1 = -1000;
      double reference_comb    = -1000;

      referenceBTagging->variable< double >("MV2c10"  ,"discriminant" ,reference_mv2c10 );
      referenceBTagging->variable< double >("MV2c20"  ,"discriminant" ,reference_mv2c20 );
      reference_ip3dsv1 = referenceBTagging->SV1plusIP3D_discriminant();
      reference_comb = (referenceBTagging->IP3D_pb()/referenceBTagging->IP3D_pu()) * (referenceBTagging->SV1_pb()/referenceBTagging->SV1_pu());
      if( reference_comb/(1+reference_comb) < 1 ) 
	reference_comb = -1.0*TMath::Log10(1-(reference_comb/(1+reference_comb)) );

      ATH_MSG_INFO( "    ** MV2C10  = " << reference_mv2c10  );
      ATH_MSG_INFO( "    ** MV2C20  = " << reference_mv2c20  );
      ATH_MSG_INFO( "    ** IP3DSV1 = " << reference_ip3dsv1 );
      ATH_MSG_INFO( "    ** COMB    = " << reference_comb    );

      // *** Compare jets
      ATH_MSG_INFO( "Comparing ORIGINAL and RETAGGED jet collections" );
      bool passes = true;
      if ( referenceJet->p4().Et() != jet->p4().Et() ) passes = false;
      else if ( referenceJet->eta() != jet->eta() ) passes = false;
      else if ( referenceJet->phi() != jet->phi() ) passes = false;
      else if ( value_mv2c10 != reference_mv2c10 ) passes = false;
      else if ( value_mv2c20 != reference_mv2c20 ) passes = false;
      else if ( value_ip3dsv1 != reference_ip3dsv1 ) passes = false;
      else if ( value_comb != reference_comb ) passes = false;

      itJet++;
      itBtagging++;

      if ( !passes ) {
	ATH_MSG_ERROR( "RETAGGED and ORIGINAL GSC jets are dissimilar." );
	delete retaggedJets;
	return StatusCode::FAILURE;
      } else { ATH_MSG_INFO( "  ** SUCCESS" ); }


    }
    
    ATH_MSG_INFO( "" );

    delete retaggedJets;
    return StatusCode::SUCCESS;
  }

  // ***

  template<typename T> bool TrigBtagValidationTest::getFromCombo(std::vector<const T*> &output,const Trig::Combination& combo,std::string key) {
    const DataVector<T> *tmpContainer = nullptr;
    const std::vector< Trig::Feature< DataVector<T> > > tmpFeatures = combo.containerFeature< DataVector<T> >(key.c_str());
  
    if (tmpFeatures.size())
      tmpContainer = tmpFeatures[0].cptr();
    if (tmpContainer == nullptr)
      return false;
  
    output.push_back( (*tmpContainer)[0] );
    return true;
  }

}///namespace
//**********************************************************************


