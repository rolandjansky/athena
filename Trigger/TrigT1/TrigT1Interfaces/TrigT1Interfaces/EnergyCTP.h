// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         EnergyCTP.h  -  description
                            -------------------
   begin                : Friday May 05 2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/

#ifndef TRIGT1INTERFACES_ENERGYCTP_H
#define TRIGT1INTERFACES_ENERGYCTP_H

// Gaudi kernel stuff.
#include "GaudiKernel/DataObject.h"

namespace LVL1 {

   /**
    *  @short "Energy" input class to the CTP simulation
    *
    *          This class defines the Energy CTP
    *          which is generated by the Energy Trigger.
    *
    * @author E. Moyse
    *
    * $Revision: 615360 $
    * $Date: 2014-09-05 21:34:47 +0200 (Fri, 05 Sep 2014) $
    */
   class EnergyCTP : public DataObject {

   public:
      EnergyCTP( unsigned int cableword0 = 0, unsigned int cableword1 = 0 );
      ~EnergyCTP();

      /** return the data for the full-range triggers
       *
       * <code>|P|8b METSig Map|8b EtMiss Hit Map|8b Et Sum Map|</code>
       */
      unsigned int cableWord0() const;

      /** return the data for the restricted eta triggers
       *
       * <code>|P|8b EtMiss Hit Map|8b Et Sum Map|</code>
       */
      unsigned int cableWord1() const;

   private:
      const unsigned int m_cableWord0;
      const unsigned int m_cableWord1;

   }; // class EnergyCTP

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( LVL1::EnergyCTP, 6254, 1 )

#endif // TRIGT1INTERFACES_ENERGYCTP_H
