// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchTrain.h 618129 2014-09-23 11:37:00Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_BUNCHTRAIN_H
#define TRIGBUNCHCROSSINGTOOL_BUNCHTRAIN_H

// System include(s):
#include <set>
#include <iosfwd>

// Gaudi/Athena include(s):
#include "AsgMessaging/MsgStream.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossing.h"

namespace Trig {

   /**
    *  @short A smart set of BunchCrossing objects
    *
    *         This class is used internally to describe a bunch train. It's basically
    *         just a list of BunchCrossing objects with a couple of convenience
    *         functions.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 618129 $
    * $Date: 2014-09-23 13:37:00 +0200 (Tue, 23 Sep 2014) $
    */
   class BunchTrain : public std::set< BunchCrossing > {

   public:
      /// Default constructor
      BunchTrain();
      /// Copy constructor
      BunchTrain( const BunchTrain& parent );

      /// Assignment operator
      BunchTrain& operator= ( const BunchTrain& rhs );

      /// Spacing of the bunches in this train in nanoseconds
      int spacing() const;
      /// "Distance" of a bunch crossing from this bunch train
      int distance( const BunchCrossing& bc ) const;
      /// Check if a bunch crossing is inside this train
      bool isInside( const BunchCrossing& bc ) const;

      /// Iterator pointing to the first bunch in the train
      const_iterator train_front() const;
      /// Iterator pointing to the last bunch in the train
      const_iterator train_back() const;

      /// Check the spacing of the bunches in the train
      bool validate();

   private:
      int m_spacing; ///< Spacing of the bunches in nanoseconds
      const_iterator m_front; ///< Iterator pointing to the first bunch
      const_iterator m_back; ///< Iterator pointing to the last bunch
      bool m_gapFound; ///< Flag specifying if the train spreads over the "BCID turnover"

   }; // class BunchTrain

   /// Comparison operator for BunchTrain objects
   bool operator< ( const BunchTrain& bt1, const BunchTrain& bt2 );

} // namespace Trig

/// Output operator for BunchTrain objects
std::ostream& operator<< ( std::ostream& stream, const Trig::BunchTrain& bt );
/// Output operator for BunchTrain objects
MsgStream& operator<< ( MsgStream& stream, const Trig::BunchTrain& bt );

#endif // TRIGBUNCHCROSSINGTOOL_BUNCHTRAIN_H
