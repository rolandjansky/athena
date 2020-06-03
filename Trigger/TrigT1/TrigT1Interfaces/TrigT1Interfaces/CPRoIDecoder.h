// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         CPRoIDecoder.h  -  description
                            -------------------
   begin                : Fri Apr 19 2002
   email                : moyse@ph.qmw.ac.uk
***************************************************************************/


#ifndef TRIGT1INTERFACES_CPROIDECODER_H
#define TRIGT1INTERFACES_CPROIDECODER_H

// Local include(s):
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/RoIDecoder.h"

namespace LVL1 {

   /**
    * A level 1 calorimeter trigger conversion service:
    * returns the Coordinate represented by a RoI word
    *
    * @author E. Moyse
    * @todo this should probably be a static class.
    *
    * $Revision: 615360 $
    * $Date: 2014-09-05 21:34:47 +0200 (Fri, 05 Sep 2014) $
    */
   class CPRoIDecoder : public RoIDecoder {

   public:
      CPRoIDecoder();
      virtual ~CPRoIDecoder();
      
      /** CP-RoI specific, but can distinguish Run 1/Run 2 */
      TrigT1CaloDefs::RoIType roiType( unsigned int word ) const;

      /** RoI coordinate information */
      CoordinateRange coordinate( const unsigned int roiWord );
      unsigned int crate( const unsigned int roiWord );
      unsigned int module( const unsigned int roiWord );
      unsigned int chip( const unsigned int roiWord );
      unsigned int localcoord( const unsigned int roiWord );

      /** Thresholds passed (Run 1 RoIs) */
      const std::vector< unsigned int >& thresholdsPassed( const unsigned int word );
      
      /** ET and Isolation information (Run 2 RoIs) */
      unsigned int et( const unsigned int roiWord );
      unsigned int isolationWord( const unsigned int roiWord );
      
   protected:
      /** get information from CP RoI word and store in member variables. */
      void decodeWord( const unsigned int word );

   private:
      unsigned int m_cpm;
      unsigned int m_cp;
      unsigned int m_lc;

   }; // class CPRoIDecoder

} // namespace LVL1

#endif // TRIGT1INTERFACES_CPROIDECODER_H
