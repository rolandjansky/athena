#ifndef RATESANALYSIS_EXAMPLERATESEMULATION_H
#define RATESANALYSIS_EXAMPLERATESEMULATION_H 1

#include "RatesAnalysis/RatesAnalysisAlg.h"

class ExampleRatesEmulation: public ::RatesAnalysisAlg { 
 public: 
  ExampleRatesEmulation( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~ExampleRatesEmulation(); 

  virtual StatusCode  ratesInitialize() override;
  virtual StatusCode  ratesExecute() override;
  virtual StatusCode  ratesFinalize() override;

 private:

  float m_lumi; //!< Targer inst. luminosity

}; 

#endif //> !RATESANALYSIS_EXAMPLERATESEMULATION_H
