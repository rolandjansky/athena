/***************************************************************************
                         JetEtRoIDecoder.h  -  description
                            -------------------
   begin                : Fri Apr 19 2002
   copyright            : (C) 2002 by E.Moyse
   email                : moyse@ph.qmw.ac.uk
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRIGT1INTERFACES_JETETROIDECODER_H
#define TRIGT1INTERFACES_JETETROIDECODER_H

namespace LVL1 {

  /**
   *  A level 1 calorimeter trigger conversion service:
   *  returns the Coordinate represented by a RoI word
   *
   * @author E.Moyse
   * \todo this should probably be a static class.
   */
  class JetEtRoIDecoder {

  public:
    JetEtRoIDecoder();
    virtual ~JetEtRoIDecoder();

    /** returns a 4b number which represents the Jet Et energy in GeV */
    unsigned int energy( unsigned int roiWord ) const;

  private:
    unsigned int extractBits( unsigned int word, const unsigned int start, const unsigned int length ) const;

  }; // class JetEtRoIDecoder

} // namespace LVL1

#endif // TRIGT1INTERFACES_JETETROIDECODER_H
