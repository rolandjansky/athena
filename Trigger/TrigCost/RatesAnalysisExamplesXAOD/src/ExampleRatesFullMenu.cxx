#include "ExampleRatesFullMenu.h"

ExampleRatesFullMenu::ExampleRatesFullMenu( const std::string& name, ISvcLocator* pSvcLocator ) : RatesAnalysisAlg(name, pSvcLocator) {
  declareProperty( "TargetLuminosity", m_lumi = 1e34);
}

ExampleRatesFullMenu::~ExampleRatesFullMenu() {
}

StatusCode  ExampleRatesFullMenu::ratesInitialize() {
  ATH_MSG_INFO("In ratesInitialize()");

  // Here we assume a full-ring, other functions are available to change this assumption.
  // @see setTargetLumiMu(const double lumi, const double mu);
  // @see setTargetLumiBunches(const double lumi, const int32_t bunches);
  setTargetLumi( m_lumi );

  // Register all triggers in the menu. Will not include any which were prescaled out in Athena.
  ATH_CHECK( addAllExisting() );

  // Or, if you were only interested in part of the menu you could do e.g.
  // ATH_CHECK( addExisting("HLT_mu.*") );
  // ATH_CHECK( addExisting("L1_MU.*") );

  return StatusCode::SUCCESS;
}

StatusCode  ExampleRatesFullMenu::ratesExecute() {
  // Triggers added are of type kEXISTING so here we rely on the Trigger Decision Tool for pass/fail. 
  // This is automatic so there is nothing to do here.
  return StatusCode::SUCCESS;
}

StatusCode  ExampleRatesFullMenu::ratesFinalize() {
  ATH_MSG_INFO("In ratesFinalize()");
  return StatusCode::SUCCESS;
}
