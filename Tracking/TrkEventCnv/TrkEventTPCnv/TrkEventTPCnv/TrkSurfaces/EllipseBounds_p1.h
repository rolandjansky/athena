/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_ELLIPSEBOUNDS_P1_H
#define TRKEVENTTPCNV_ELLIPSEBOUNDS_P1_H

namespace Trk {

/** Persistent representation of the transient Trk::EllipseBounds_p1 class.
@author Edward.Moyse@cern.ch
*/
class EllipseBounds_p1 {
public:
/**Default Constructor*/
    EllipseBounds_p1() {};  

    float m_rMinX;
    float m_rMinY;
    float m_rMaxX;
    float m_rMaxY;
    float m_avePhi;
    float m_hPhiSec;
};

} // end of namespace


#endif 


