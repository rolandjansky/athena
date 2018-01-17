/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBHit_H
#define TBEVENT_TBHit_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Hit position of the beam in one detector
// 
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


//#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TBEvent/TBBeamDetector.h"



class TBHit 
{
 public:
  

  
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////
    
   TBHit();

   ~TBHit(){};
  
  // /////////////////////
  // Data Manipulation //
  // /////////////////////

  float getPosx(){return m_x;}
  float getPosy(){return m_y;}
  float getPosz(){return m_z;}


  double getErrorx(){return m_ex;}
  double getErrory(){return m_ey;}
  double getErrorz(){return m_ez;}

  bool IsValid(){return m_IsValid;}

  TBBeamDetector * getBeamDetector(){return m_beamdetect;}

  // 

  void setPosx(float x){m_x=x;}
  void setPosy(float y){m_y=y;}
  void setPosz(float z){m_z=z;}
  void setErrorx(double ex){m_ex = ex;}
  void setErrory(double ey){m_ex = ey;}
  void setErrorz(double ez){m_ez = ez;}



  void setValidFlag(bool valid){m_IsValid=valid;}

  void setBeamDetector(TBBeamDetector * beamdetect){m_beamdetect=beamdetect;}

 private:

  //////////////////
 // Private Data //
  //////////////////

  // A pointer to the detector that recorded this hit :
  TBBeamDetector * m_beamdetect;

  // Position of the hit in the detectors given in global 'reconstruction' coordonates.
  float m_x,m_y,m_z;  

  // Error associated with the detector resolution and position
  // measurement error
  double m_ex,m_ey, m_ez;  


  // A flag that tells if the hit should be included in track reconstruction or not
  bool m_IsValid;


};
CLASS_DEF( TBHit , 18028812 , 1 )
#endif

