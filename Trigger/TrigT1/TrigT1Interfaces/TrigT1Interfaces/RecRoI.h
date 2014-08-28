/***************************************************************************
                         RecRoi.h  -  description
                            -------------------
   begin                : Fri Oct 4 2002
   copyright            : (C) 2002 by moyse
   email                : edward.moyse@cern.ch
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRIGT1INTERFACES_RECROI_H
#define TRIGT1INTERFACES_RECROI_H

namespace LVL1 {

  // Is this class used at all???

  /**this is the base class for "reconstructed" or "recreated RoIs" formed from slink words out by Lvl1
    *@author Edward Moyse
    */
  class RecRoI {

  public:
    RecRoI();
    virtual ~RecRoI();

    /**  returns the 32bit ROI word. This is defined
    in the ATLAS note daq-98-089*/
    virtual unsigned int roiWord() const ;

  private:
    unsigned int m_roiWord;

  }; // class RecRoI

} // namespace LVL1

#endif // TRIGT1INTERFACES_RECROI_H
