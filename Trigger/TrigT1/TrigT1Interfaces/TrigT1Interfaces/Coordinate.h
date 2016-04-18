// Dear emacs, this is -*- c++ -*-
/***************************************************************************
                         Coordinate.h  -  description
                            -------------------
   begin                : 28/02/2002
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

#ifndef TRIGT1INTERFACES_COORDINATE_H
#define TRIGT1INTERFACES_COORDINATE_H

// STL include(s):
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

/**
 * @short LVL1 namespace.
 *
 * Most of the LVL1 interface classes are put into this namespace, showing that
 * they're general LVL1 classes.
 *
 * $Revision: 707787 $
 * $Date: 2015-11-13 16:09:29 +0100 (Fri, 13 Nov 2015) $
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
    * $Revision: 707787 $
    * $Date: 2015-11-13 16:09:29 +0100 (Fri, 13 Nov 2015) $
    */
   class Coordinate {

   public:
      Coordinate( double phi, double eta );
      Coordinate();
      virtual ~Coordinate();

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
