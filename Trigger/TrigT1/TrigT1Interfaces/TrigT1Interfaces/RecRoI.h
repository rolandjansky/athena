/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         RecRoi.h  -  description
                            -------------------
   begin                : Fri Oct 4 2002
   email                : edward.moyse@cern.ch
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
