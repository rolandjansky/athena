/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : RegularFormula                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __RegularFormula__
#define __RegularFormula__

#include "TFormula.h"
#include "TString.h"
#include <list>

namespace Root {

   class RegularFormula : public TFormula {
      
   public:

      RegularFormula();      
      RegularFormula(const char* name, const char* expression);
      virtual ~RegularFormula();

      RegularFormula(const Root::RegularFormula& other) ;
      RegularFormula& operator=(const RegularFormula& other) ;

      Int_t setFormula(const char *expression);

      inline unsigned int getNPars() const { return m_par.size(); }
      const std::list<TString>& getParNames() const { return m_par; }

   private:

      void parseExpression(const char* expression, TString& expr) ;

      TString m_expr;
      std::list<TString> m_par;

      ClassDef(RegularFormula,0)
   };
}

#endif

