// Dear emacs, this is -*- c++ -*-
// $Id: RoIDecoder.h 187728 2009-05-27 16:18:06Z krasznaa $
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
      virtual CoordinateRange coordinate( const unsigned int word ) const = 0;

   protected:
      /** returns the value of bits in word between bit "start" and bit "start" + "length" */
      unsigned int extractBits( unsigned int word, const unsigned int start,
                                const unsigned int length ) const;

      bool m_DEBUG;

   }; // class RoIDecoder

} // namespace LVL1

#endif // TRIGT1INTERFACES_ROIDECODER_H
