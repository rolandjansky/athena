// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
      virtual CoordinateRange coordinate( const unsigned int roiWord ) const override;
      unsigned int crate( const unsigned int roiWord ) const;
      unsigned int module( const unsigned int roiWord ) const;
      unsigned int chip( const unsigned int roiWord ) const;
      unsigned int localcoord( const unsigned int roiWord ) const;

      /** Thresholds passed (Run 1 RoIs) */
      const std::vector< unsigned int > thresholdsPassed( const unsigned int word ) const;
      
      /** ET and Isolation information (Run 2 RoIs) */
      unsigned int et( const unsigned int roiWord ) const;
      unsigned int isolationWord( const unsigned int roiWord ) const;
   }; // class CPRoIDecoder

} // namespace LVL1

#endif // TRIGT1INTERFACES_CPROIDECODER_H
