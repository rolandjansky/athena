/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

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

