/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTreeFormula.h"

class APRTreeFormula : public TTreeFormula
{
public:
  
  APRTreeFormula(const std::string& exp, TTree *tree) :
        TTreeFormula("APRTreeFormula", exp.c_str(), tree),
        m_aprConst( 0 )
  {
     readConstants();
  }

  virtual ~APRTreeFormula();

  APRTreeFormula (const APRTreeFormula&) = delete;
  APRTreeFormula& operator= (const APRTreeFormula&) = delete;
  
  
  typedef long double LDouble_t;

  void          readConstants();
  Double_t      EvalInstance(Int_t instance = 0, const char *stringStackArg[]=0);

protected:

  LDouble_t     *m_aprConst;
};

