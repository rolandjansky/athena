/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaPrecisionElectronHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaPrecisionElectronHypoAlgMT::TrigEgammaPrecisionElectronHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigEgammaPrecisionElectronHypoAlgMT::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << "..." );

  
  ATH_CHECK( m_hypoTools.retrieve() );

  // Now we try to retrieve the ElectronPhotonSelectorTools that we will use to apply the electron Identification. This is a *must*
  ATH_MSG_DEBUG( "Retrieving egammaElectronLHTool..."  );
  ATH_CHECK(m_egammaElectronLHTool_vloose.retrieve());
  ATH_CHECK(m_egammaElectronLHTool_loose.retrieve());
  ATH_CHECK(m_egammaElectronLHTool_medium.retrieve());   
  ATH_CHECK(m_egammaElectronLHTool_tight.retrieve());
  
  // Retrieving Luminosity info
  ATH_MSG_DEBUG( "Retrieving luminosityCondData..."  );
  ATH_CHECK( m_avgMuKey.initialize() );

  
  ATH_CHECK( m_electronsKey.initialize() );
  renounce( m_electronsKey );// electrons are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaPrecisionElectronHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");


  // new decisions

  // new output decisions
  SG::WriteHandle<TCU::DecisionContainer> outputHandle = TCU::createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEgammaPrecisionElectronHypoTool::ElectronInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {

    //get updated RoI  
    auto roiELInfo = TCU::findLink<TrigRoiDescriptorCollection>( previousDecision, TCU::roiString() );

    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);
    const auto viewEL = previousDecision->objectLink<ViewContainer>( TCU::viewString() );
    ATH_CHECK( viewEL.isValid() );
    auto electronHandle = ViewHelper::makeHandle( *viewEL, m_electronsKey, context);
    ATH_CHECK( electronHandle.isValid() );
    ATH_MSG_DEBUG ( "Electron handle size: " << electronHandle->size() << "..." );
    // Loop over the electronHandles
    size_t validelectrons=0;
    for (size_t cl=0; cl< electronHandle->size(); cl++){
	{
	    auto ph = ViewHelper::makeLink( *viewEL, electronHandle, cl );
	    ATH_MSG_DEBUG ( "Checking ph.isValid()...");
	    if( !ph.isValid() ) {
		ATH_MSG_DEBUG ( "ElectronHandle in position " << cl << " -> invalid ElemntLink!. Skipping...");
	    }
	    ATH_CHECK(ph.isValid());

	    ATH_MSG_DEBUG ( "ElectronHandle in position " << cl << " processing...");
	    auto d = TCU::newDecisionIn( decisions, TCU::hypoAlgNodeName() );
	    d->setObjectLink( TCU::featureString(),  ph );
	    TCU::linkToPrevious( d, decisionInput().key(), counter );

            ///////LHTool////////////
            float avg_mu = 0;
            float lhval_vloose=0;
            float lhval_loose=0;
            float lhval_medium=0;
            float lhval_tight=0;

            bool pass_vloose = false;
       	    bool pass_loose = false;
       	    bool pass_medium = false;
       	    bool pass_tight = false;

            SG::ReadDecorHandle<xAOD::EventInfo,float> eventInfoDecor(m_avgMuKey, context);
            if(eventInfoDecor.isPresent()) {
             avg_mu = eventInfoDecor(0);
             ATH_MSG_DEBUG("Average mu " << avg_mu);
             asg::AcceptData accept_vloose =  m_egammaElectronLHTool_vloose->accept(context, electronHandle.cptr()->at(cl),avg_mu);
             asg::AcceptData accept_loose =  m_egammaElectronLHTool_loose->accept(context, electronHandle.cptr()->at(cl),avg_mu);
             asg::AcceptData accept_medium =  m_egammaElectronLHTool_medium->accept(context, electronHandle.cptr()->at(cl),avg_mu);
             asg::AcceptData accept_tight =  m_egammaElectronLHTool_tight->accept(context, electronHandle.cptr()->at(cl),avg_mu);
             pass_vloose = (bool) accept_vloose;
             pass_loose = (bool) accept_loose;
             pass_medium = (bool) accept_medium;
             pass_tight = (bool) accept_tight;
  
             // Monitor the LH value
             lhval_vloose = m_egammaElectronLHTool_vloose->calculate(context, electronHandle.cptr()->at(cl),avg_mu);
             lhval_loose = m_egammaElectronLHTool_loose->calculate(context, electronHandle.cptr()->at(cl),avg_mu);
             lhval_medium = m_egammaElectronLHTool_medium->calculate(context, electronHandle.cptr()->at(cl),avg_mu);
             lhval_tight = m_egammaElectronLHTool_tight->calculate(context, electronHandle.cptr()->at(cl),avg_mu);
             ATH_MSG_DEBUG("LHValue_vloose with avgmu " << lhval_vloose);
             ATH_MSG_DEBUG("LHValue_loose with avgmu " << lhval_loose);
             ATH_MSG_DEBUG("LHValue_medium with avgmu " << lhval_medium);
             ATH_MSG_DEBUG("LHValue_tight with avgmu " << lhval_tight);
            }  
            else{
             ATH_MSG_WARNING("EventInfo decoration not available!");
             asg::AcceptData accept_vloose = m_egammaElectronLHTool_vloose->accept(context, electronHandle.cptr()->at(cl));
             asg::AcceptData accept_loose = m_egammaElectronLHTool_loose->accept(context, electronHandle.cptr()->at(cl));
             asg::AcceptData accept_medium = m_egammaElectronLHTool_medium->accept(context, electronHandle.cptr()->at(cl));
             asg::AcceptData accept_tight = m_egammaElectronLHTool_tight->accept(context, electronHandle.cptr()->at(cl));
             pass_vloose = (bool) accept_vloose;
       	     pass_loose = (bool) accept_loose;
       	     pass_medium = (bool) accept_medium;
       	     pass_tight = (bool) accept_tight;

             // Monitor the LH value
             lhval_vloose = m_egammaElectronLHTool_vloose->calculate(context, electronHandle.cptr()->at(cl));
             lhval_loose = m_egammaElectronLHTool_loose->calculate(context, electronHandle.cptr()->at(cl));
             lhval_medium = m_egammaElectronLHTool_medium->calculate(context, electronHandle.cptr()->at(cl));
             lhval_tight = m_egammaElectronLHTool_tight->calculate(context, electronHandle.cptr()->at(cl));
             ATH_MSG_DEBUG("LHValue_vloose without avgmu " << lhval_vloose);
             ATH_MSG_DEBUG("LHValue_loose without avgmu " << lhval_loose);
       	     ATH_MSG_DEBUG("LHValue_medium without avgmu " << lhval_medium);
       	     ATH_MSG_DEBUG("LHValue_tight without avgmu " << lhval_tight);
            }
            const float avg_mu_const = avg_mu;
            const float lhval_const_vloose = lhval_vloose;
            const float lhval_const_loose = lhval_loose;
            const float lhval_const_medium = lhval_medium;
            const float lhval_const_tight = lhval_tight;

            const bool pass_const_vloose = pass_vloose;
            const bool pass_const_loose = pass_loose;
            const bool pass_const_medium = pass_medium;
            const bool pass_const_tight = pass_tight;
            
	    toolInput.emplace_back( d, roi, electronHandle.cptr()->at(cl), pass_const_vloose, pass_const_loose, pass_const_medium, pass_const_tight,
                                      lhval_const_vloose,lhval_const_loose, lhval_const_medium, lhval_const_tight, avg_mu_const, previousDecision );
	    validelectrons++;


	}
    }
    ATH_MSG_DEBUG( "Electrons with valid links: " << validelectrons );
    
    ATH_MSG_DEBUG( "roi, electron, previous decision to new decision " << counter << " for roi " );
    counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}
