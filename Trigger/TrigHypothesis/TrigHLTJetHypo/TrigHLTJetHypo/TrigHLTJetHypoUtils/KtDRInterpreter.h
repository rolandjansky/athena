
#ifndef TRIGHLTJETHYPO_KTDRINTERPRETER_H
#define TRIGHLTJETHYPO_KTDRINTERPRETER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetAttrsValueInterpreter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsStringToDouble.h"
class KtDRInterpreter: public IJetAttrsValueInterpreter
{
  constexpr static double m_cfact{1.0};
  JetAttrsStringToDouble m_converter;

public:
  virtual ~KtDRInterpreter(){}

  virtual std::pair<double, double> operator()(const std::string& low, const std::string& high) const override {

       return std::make_pair(m_cfact * m_converter(low,high).first, m_cfact * m_converter(low,high).second);
  }
};

#endif

