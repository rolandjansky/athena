// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         EmTauCTP.h  -  description
                            -------------------
   begin                : Friday May 05 2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/


#ifndef TRIGT1INTERFACES_EMTAUCTP_H
#define TRIGT1INTERFACES_EMTAUCTP_H

// Gaudi kernel stuff.
#include "GaudiKernel/DataObject.h"

namespace LVL1 {

   /**
    *  @short Em/Tau input class to the CTP simulation
    *
    *         This class holds the multiplicities of the various Em/Tau RoIs,
    *         which are calculated by the LVL1 calorimeter simulation.
    *
    * @author E. Moyse
    *
    * $Revision: 615360 $
    * $Date: 2014-09-05 21:34:47 +0200 (Fri, 05 Sep 2014) $
    */
   class EmTauCTP : public DataObject {

   public:
      EmTauCTP( unsigned int cableword0 = 0, unsigned int cableword1 = 0,
                unsigned int cableword2 = 0, unsigned int cableword3 = 0 );
      ~EmTauCTP();

      /** return the data that are sent on cables
       *
       *  <code>|P|thr8|thr7|thr6|thr5|thr4|thr3|thr2|thr1|</code>
       *
       *  For Run 2 Words 0 and 1 are EM results, 2 and 3 are TAU
       */
      unsigned int cableWord0() const;
      unsigned int cableWord1() const;
      unsigned int cableWord2() const;
      unsigned int cableWord3() const;

   private:
      const unsigned int m_cableWord0;
      const unsigned int m_cableWord1;
      const unsigned int m_cableWord2;
      const unsigned int m_cableWord3;

  }; // class EmTauCTP

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( LVL1::EmTauCTP, 6253, 1 )

#endif // TRIGT1INTERFACES_EMTAUCTP_H
