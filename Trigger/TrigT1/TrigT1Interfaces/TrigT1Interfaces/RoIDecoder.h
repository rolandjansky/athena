// Dear emacs, this is -*- c++ -*-
// $Id: RoIDecoder.h 187728 2009-05-27 16:18:06Z krasznaa $
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          RoIDecoder.h  -  description
                             -------------------
    begin                : Fri Apr 19 2002
    email                : moyse@ph.qmw.ac.uk
 ***************************************************************************/

#ifndef TRIGT1INTERFACES_ROIDECODER_H
#define TRIGT1INTERFACES_ROIDECODER_H

// I test some of these classes outside Athena hence the following code.
#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#else
// running in TRIGGERSPACE
#include "CoordinateRange.h"
#include "TrigT1CaloDefs.h"

#endif

#include <vector>

/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

   /** A level 1 calorimeter trigger conversion service:
       returns the Coordinate represented by a RoI word
       *@author E.Moyse
       \todo this should probably be a static class.
   */
   class RoIDecoder {

   public:
      RoIDecoder();
      virtual ~RoIDecoder();
      virtual CoordinateRange coordinate( const unsigned int word ) = 0;
      /** returns the crate associated with the RoI word */
      unsigned int crate( const unsigned int word );
      /** returns a vector containing the numbers of threshold passed i.e. if the
          vector contains 1,3,5 it means that this RoI passed thresholds 1,3 and 5.*/
      const std::vector< unsigned int >& thresholdsPassed( const unsigned int word );
      /** examines an 32bit RoI word, extracts the RoI type and returns one of:
          - CpRoIWordType,
          - JetRoIWordType,
          - JetEtRoIWordType,
          - EnergyRoIWordType*/
      TrigT1CaloDefs::RoIType roiType( unsigned int word ) const;

   protected:
      /** get information from CP RoI word and store in member variables. */
      virtual void decodeWord( const unsigned int word ) = 0;
      /** returns the value of bits in word between bit "start" and bit "start" + "length" */
      unsigned int extractBits( unsigned int word, const unsigned int start,
                                const unsigned int length ) const;
      /** print thresholds passed */
      void dumpThresholdsPassed() const;

      unsigned int m_crate;
      unsigned int m_thresholdsPassed;
      std::vector< unsigned int > m_threshPassedVec;
      bool m_DEBUG;

   }; // class RoIDecoder

} // namespace LVL1

#endif // TRIGT1INTERFACES_ROIDECODER_H
