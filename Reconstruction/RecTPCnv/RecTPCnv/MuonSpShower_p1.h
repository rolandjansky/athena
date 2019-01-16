/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


 #ifndef RECTPCNV_MUONSPSHOWER_P1_H
 #define RECTPCNV_MUONSPSHOWER_P1_H

class MuonSpShowerCnv_p1;

/** Persisent representation of the transient Rec::MuonSpShower class.
@author Edward.Moyse@cern.ch
*/ 
class MuonSpShower_p1 {

public:
    //friend class MuonSpShowerCnv_p1;

     /** Default Constructor  needed for persistency */
    MuonSpShower_p1();

     /** Destructor */
    virtual ~MuonSpShower_p1() = default;

//private:

    float m_eta;
    float m_phi;
    unsigned short m_numberOfTriggerHits;
    unsigned short m_numberOfInnerHits;
    unsigned short m_numberOfInnerSegments;
    unsigned short m_numberOfMiddleHits;
    unsigned short m_numberOfMiddleSegments;
    unsigned short m_numberOfOuterHits;
    unsigned short m_numberOfOuterSegments;

};

 #endif


