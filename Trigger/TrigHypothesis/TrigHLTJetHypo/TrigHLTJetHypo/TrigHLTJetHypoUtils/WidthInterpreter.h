
#ifndef TRIGHLTJETHYPO_WIDTHINTERPRETER_H
#define TRIGHLTJETHYPO_WIDTHINTERPRETER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetAttrsValueInterpreter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsStringToDouble.h"

class WidthInterpreter: public IJetAttrsValueInterpreter
{
  constexpr static double m_cfact{0.1};
  JetAttrsStringToDouble m_converter;

public:
  virtual ~WidthInterpreter(){}

  virtual std::pair<double, double> operator()(const std::string& low, const std::string& high) const override {

      std::cout << "amanda - in WidthInterpreter \n";
      std::cout << "amanda - got string inputs, returning doubles " << m_cfact * m_converter(low,high).first << " and " <<  m_cfact * m_converter(low,high).second;
      return std::make_pair(m_cfact * m_converter(low,high).first, m_cfact * m_converter(low,high).second);
  }
};

#endif
