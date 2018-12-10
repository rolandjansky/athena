
#ifndef TRIGHLTJETHYPO_KTDRINTERPRETER_H
#define TRIGHLTJETHYPO_KTDRINTERPRETER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetAttrsValueInterpreter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsStringToDouble.h"
class KtDRInterpreter: public IJetAttrsValueInterpreter
{
  constexpr static double m_cfact{0.1};
  JetAttrsStringToDouble m_converter;

public:
  virtual ~KtDRInterpreter(){}

  virtual std::pair<double, double> operator()(const std::string& low, const std::string& high) const override {

      //for debugging
      //std::cout << "in KtDRInterpreter \n";
      //std::cout << "got string inputs, returning doubles " << m_cfact * m_converter(low,high).first << " and " <<  m_cfact * m_converter(low,high).second << "\n";

       return std::make_pair(m_cfact * m_converter(low,high).first, m_cfact * m_converter(low,high).second);
  }
};

#endif

