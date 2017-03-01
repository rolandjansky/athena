#ifndef RATESANALYSIS_EXAMPLERATESFULLMENU_H
#define RATESANALYSIS_EXAMPLERATESFULLMENU_H 1

#include "RatesAnalysis/RatesAnalysisAlg.h"

class ExampleRatesFullMenu: public ::RatesAnalysisAlg { 
 public: 
  ExampleRatesFullMenu( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~ExampleRatesFullMenu(); 

  virtual StatusCode  ratesInitialize() override;
  virtual StatusCode  ratesExecute() override;
  virtual StatusCode  ratesFinalize() override;

 private:

  float m_lumi; //!< Targer inst. luminosity

}; 

#endif //> !RATESANALYSIS_EXAMPLERATESFULLMENU_H
