/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBHitPlane_H
#define TBEVENT_TBHitPlane_H

/** @class TBHitPlane TBHitPlane.h "TBEvent/TBHitPlane.h"
This class is used to store hit information from BPC & MWPC for one plane. The modivation was to create a track object using both BPC data & MWPC data but given that the MWPC is plane specific (zx plane and yx plane) we have to make a linear fit on each plane with the correct hit objects.
*/

#include "AthenaKernel/CLASS_DEF.h"
#include "TBEvent/TBBeamDetector.h"

class TBHitPlane 
{
 public:
  
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////
    
  TBHitPlane();
  ~TBHitPlane();
  
  // /////////////////////
  // Data Manipulation //
  // /////////////////////
  
  
  double getPosu() const {return m_u;}
  double getPosw() const {return m_w;}
  double getErroru() const {return m_eu;}
  double getErrorw() const {return m_ew;}

  bool IsValid() const {return m_IsValid;}

  TBBeamDetector * getBeamDetector() const {return m_beamdetect;}

  void setPosu(double u){m_u = u;}
  void setPosw(double w){m_w = w;}
  void setErroru(double eu){m_eu = eu;}
  void setErrorw(double ew){m_ew = ew;}

  void setValidFlag(bool valid){m_IsValid=valid;}

  void setBeamDetector(TBBeamDetector * beamdetect){m_beamdetect=beamdetect;}

 private:

  //////////////////
 // Private Data //
  //////////////////

  // A pointer to the detector that recorded this hit :
  TBBeamDetector * m_beamdetect;

  // Position of the hit in the detectors given in global 
  //'reconstruction' coordonates.
  double m_u, m_w;

  // Error associated with the detector resolution and position
  // measurement error
  double m_eu, m_ew;  

  // A flag that tells if the hit should be included in track
  // reconstruction or not
  bool m_IsValid;


};
CLASS_DEF( TBHitPlane , 229616155 , 1 )
#endif
