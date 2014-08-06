// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PointerComparisonMetric.h 362406 2011-04-29 12:38:56Z krasznaa $
#ifndef TRIGGERD3PDMAKER_POINTERCOMPARISONMETRIC_H
#define TRIGGERD3PDMAKER_POINTERCOMPARISONMETRIC_H

// Trigger include(s):
#include "TrigObjectMatching/DistanceFunctor.h"

namespace D3PD {

   /**
    *  @short Custom metric for TriggerObjectDecisionFillerTool
    *
    *         TriggerObjectDecisionFillerTool in the end just checks if a
    *         specific trigger object is from a list of objects that passed
    *         a given trigger. It checks if the object is from that list
    *         by using some sort of metric. Usually it's the default dR
    *         metric. But since we're giving the actual trigger objects
    *         as input, it's much easier to just compare the pointers of
    *         the objects.
    *
    *         Have a look at the TrigObjectMatching package for more
    *         details.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 362406 $
    * $Date: 2011-04-29 14:38:56 +0200 (Fri, 29 Apr 2011) $
    */
   template< class T >
   class PointerComparisonMetric : public DistanceFunctor< T, T > {

   public:
      /// Virtual destructor to make vtable happy
      virtual ~PointerComparisonMetric() {}

   private:
      /// The function calculating the "distance" of two objects
      virtual float calculateDistance( const T* left,
                                       const T* right ) const;

   }; // class PointerComparisonMetric

} // namespace D3PD

// Include the implementation:
#include "PointerComparisonMetric.icc"

#endif // TRIGGERD3PDMAKER_POINTERCOMPARISONMETRIC_H
