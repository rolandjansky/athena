/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         Coordinate.cxx  -  description
                            -------------------
   begin                : 26/02/2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/

#include <cmath>

#include "TrigT1Interfaces/Coordinate.h"

namespace LVL1 {

  const double Coordinate::m_twoPi = M_PI * 2.0;

  Coordinate::Coordinate( double phi, double eta )
    : m_phi( phi ), m_eta( eta ) {

    checkBounds();
  }

  Coordinate::Coordinate()
    : m_phi( 0.0 ), m_eta( 0.0 ) {

  }

  /** change coords of an existing Coordinate object*/
  void Coordinate::setCoords( double phi, double eta ) {

    m_phi = phi; m_eta = eta;
    checkBounds();
    return;

  }

  /** return eta*/
  double Coordinate::eta() const {
    return m_eta;
  }

  /** return phi*/
  double Coordinate::phi() const {
    return m_phi;
  }

  /** Ensure coords are sensible
      this could be done more intelligently, but at least it's readable!
      \todo Tidy up and finish */
  void Coordinate::checkBounds() {
    if ( m_phi < 0 ) m_phi += m_twoPi;
    if ( m_phi > m_twoPi ) m_phi -= m_twoPi;
    return;
  }

} // namespace LVL1

/** overload << operator so coordinate can be easily displayed... i.e. std::cout << coord */
std::ostream& LVL1::operator<< ( std::ostream& theStream, const Coordinate& theCoord ) {
  theStream << "(" << theCoord.phi() << ", " << theCoord.eta() << ")";
  return theStream;
}

/** overload << operator so coordinate can be easily displayed... i.e. std::cout << coord */
MsgStream& LVL1::operator<< ( MsgStream& theStream, const Coordinate& theCoord ) {
  theStream << "(" << theCoord.phi() << ", " << theCoord.eta() << ")";
  return theStream;
}
