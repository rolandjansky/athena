/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_MUCTPICTPPHASE1_H
#define TRIGT1INTERFACES_MUCTPICTPPHASE1_H

#include <vector>

namespace LVL1 {

   /**
    *  @short MuCTPI input class to the CTP simulation
    *
    *         This class is used to send the LVL1 muon multiplicity information
    *         from the MuCTPI simulation to the CTP simulation.
    *
    * $author Thomas SChoerner-Sadenius <thomas.schoerner@cern.ch>
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class MuCTPICTPPhase1 {

   public:
      /* constructor and destructor */
     MuCTPICTPPhase1( const std::vector<unsigned int>& words );
     ~MuCTPICTPPhase1();

     /**
      * Return the muon data i.e. <code>(xxxxx|thrN|...|thr2|thr1)</code>
      */
     std::vector<unsigned int> muCTPIWord() const;
     
   private:
      /// The only data member
     std::vector<unsigned int> m_MuCTPICTPWord;

   }; // class MuCTPICTPPhase1

} // namespace LVL1

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LVL1::MuCTPICTPPhase1, 98734023, 0 )

#endif // TRIGT1INTERFACES_MUCTPICTPPHASE1_H
