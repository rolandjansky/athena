// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         Coordinate.h  -  description
                            -------------------
   begin                : 28/02/2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/


#ifndef TRIGT1INTERFACES_COORDINATE_H
#define TRIGT1INTERFACES_COORDINATE_H

// STL include(s):
#include <iosfwd>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

/**
 * @short LVL1 namespace.
 *
 * Most of the LVL1 interface classes are put into this namespace, showing that
 * they're general LVL1 classes.
 *
 * $Revision: 782811 $
 * $Date: 2016-11-07 18:20:40 +0100 (Mon, 07 Nov 2016) $
 */
namespace LVL1 {

   class Coordinate;
   std::ostream& operator<< ( std::ostream& theStream, const Coordinate& theCoord );
   MsgStream&    operator<< ( MsgStream& theStream, const Coordinate& theCoord );

   /**
    * @short Coordinate class declaration
    *
    * Simple class defining an eta--phi coordinate in the detector.
    *
    * $Revision: 782811 $
    * $Date: 2016-11-07 18:20:40 +0100 (Mon, 07 Nov 2016) $
    */
   class Coordinate {

   public:
      Coordinate( double phi, double eta );
      Coordinate();
      virtual ~Coordinate() = default;

      void setCoords( double phi, double eta );
      double eta() const;
      double phi() const;

      /** overload << operator so coordinate can be easily displayed...
          i.e. <code>cout << coord</code> */
      friend std::ostream& operator<< ( std::ostream& theStream, const Coordinate& theCoord );
      friend MsgStream&    operator<< ( MsgStream& theStream, const Coordinate& theCoord );

   protected:
      void checkBounds();

   private:
      double m_phi;
      double m_eta;
      static const double m_twoPi;

   }; // class Coordinate

} // namespace LVL1

#endif // TRIGT1INTERFACES_COORDINATE_H
