// Dear emacs, this is -*- c++ -*-
/***************************************************************************
                         EmTauCTP.h  -  description
                            -------------------
   begin                : Friday May 05 2002
   copyright            : (C) 2002 by moyse
   email                : e.moyse@qmul.ac.uk
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class EmTauCTP : public DataObject {

   public:
      EmTauCTP( unsigned int cableword0 = 0, unsigned int cableword1 = 0 );
      ~EmTauCTP();

      /** return the data that is sent on cable 0.
       *  i.e.
       *
       *  <code>|P|thr8|thr7|thr6|thr5|thr4|thr3|thr2|thr1|0|</code>
       *
       *  (e algorithm only)
       */
      unsigned int cableWord0() const;
      /** return the data sent on cable 1
       *  i.e.
       *
       *  <code>|P|thr8|thr7|thr6|thr5|thr4|thr3|thr2|thr1|0|</code>
       *
       *  (e or tau algorithms, as set in TriggerMenu) where each threshold count
       *  (thr8 etc) is 3 bits long, and represents the number of e or tau candidates
       *  passing that threshold. and where |0| represents a bit set to zero ...
       *  and P is an odd parity bit, so if the other bits mod 2=0, then P=1. i.e.
       *  if bits=0, P=1 and where |0| represents a bit set to zero
       */
      unsigned int cableWord1() const;

   private:
      const unsigned int m_cableWord0;
      const unsigned int m_cableWord1;

  }; // class EmTauCTP

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( LVL1::EmTauCTP, 6253, 1 )

#endif // TRIGT1INTERFACES_EMTAUCTP_H
