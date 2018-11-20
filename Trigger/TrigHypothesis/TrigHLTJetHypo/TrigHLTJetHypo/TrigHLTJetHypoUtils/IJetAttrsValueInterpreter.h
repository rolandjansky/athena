
#ifndef TRIGHLTJETHYPO_IJETATTRSVALUEINTERPRETER_H
#define TRIGHLTJETHYPO_IJETATTRSVALUEINTERPRETER_H

class IJetAttrsValueInterpreter
{
public:
  virtual ~IJetAttrsValueInterpreter(){};

  virtual std::pair<double,double> operator() (const std::string& ,
                                               const std::string& ) const = 0;
};

#endif

