// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchTrain.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_BUNCHTRAIN_H
#define TRIGBUNCHCROSSINGTOOL_BUNCHTRAIN_H

// STL include(s):
#include <set>

// Local include(s):
#include "BunchCrossing.h"
#include "LogWriter.h"

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
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   class BunchTrain : public std::set< BunchCrossing >,
                      public LogWriter {

   public:
      /// Default constructor
      BunchTrain();
      /// Copy constructor
      BunchTrain( const BunchTrain& parent );

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

   /// Output operator for BunchTrain objects
   MsgLogger& operator<< ( MsgLogger& stream, const BunchTrain& bt );
   /// Comparison operator for BunchTrain objects
   bool operator< ( const BunchTrain& bt1, const BunchTrain& bt2 );

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_BUNCHTRAIN_H
