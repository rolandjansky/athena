// Dear emacs, this is -*- c++ -*-
/***************************************************************************
                         CoordinateRange.h  -  description
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

#ifndef TRIGT1INTERFACES_COORDINATERANGE_H
#define TRIGT1INTERFACES_COORDINATERANGE_H

// Local include(s):
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/Range.h"
#include "TrigT1Interfaces/PhiRange.h"

namespace LVL1 {

   /**
    * @short CoordinateRange class declaration
    *
    * Class defining a "window" in eta--phi space.
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class CoordinateRange : public Coordinate {

   public:
      CoordinateRange( double phiMin, double phiMax, double etaMin, double etaMax );
      CoordinateRange( const CoordinateRange& rhs );
      CoordinateRange();
      virtual ~CoordinateRange();

      void setRanges( double phiMin, double phiMax, double etaMin, double etaMax );
      PhiRange phiRange() const;
      Range etaRange() const;
      /** returns true if the coordinate falls inside the coordinate range */
      bool contains( Coordinate& coord ) const;

   private:
      /** calculates the centre of the ranges and sets the phi,eta coords to that */
      void setCentre();

      PhiRange m_phiRange;
      Range m_etaRange;

   }; // class CoordinateRange

} // namespace LVL1

#endif // TRIGT1INTERFACES_COORDINATERANGE_H
