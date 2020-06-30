/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloCellVecMon.cxx
//
// AUTHORS:   F Spano'    06/15/2006
// ********************************************************************
#include "ROIMon.h" 

#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <functional>
#include <set>


using CLHEP::Hep3Vector;


// a reference for 2d distance of a point from a line is in
// http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html

ROIMon::ROIMon( ComTime* comtime,float distance)
  : m_MinDist(0),
    m_tileMuDeltaEta(0),
    m_tileMuDeltaPhi(0)
{
  m_myComTime=comtime;
  m_maxDistance=distance;
  m_theTime=-9999.;
  m_theDistance=-9999.;

}


ROIMon::~ROIMon(){}


int ROIMon::Accept( Hep3Vector Point){

// get info from TileMuFitter

  int accept=0;
  //double m_tileComTime =0.;
  //double m_tileMuX = 0.;
  //double m_tileMuY = 0.;
  //double m_tileMuZ = 0.;

  //double m_tileMuCosTheta = 0.;
  //double m_tileMuSinThetaSinPhi = 0.;
  //double m_tileMuSinThetaCosPhi = 0.;
  
  Hep3Vector MuZeroPosition;
  Hep3Vector MuDirection;
  
  MuZeroPosition =  m_myComTime->GetCounterPosition() ;
  MuDirection =  m_myComTime->GetcosThetaDirection(); 
  
  if(m_myComTime->getTime()!=0 || MuZeroPosition.x()!=0 || MuZeroPosition.z()!=0  ) 
    {
      //m_tileComTime = m_myComTime->getTime();
      
      //m_tileMuSinThetaCosPhi = MuDirection.x();
      //m_tileMuSinThetaSinPhi = MuDirection.y();
      //m_tileMuCosTheta = MuDirection.z();
      
      //m_tileMuX = MuZeroPosition.x();
      //m_tileMuY = MuZeroPosition.y();
      //m_tileMuZ = MuZeroPosition.z();
    }


  //  m_theTime = m_tileComTime;
  
  // extract distance of minimum approach==============
  // for instance see a brief sicussion at http://math.mit.edu/~djk/18_022/chapter02/example02.html
  // bring everything back to the origin (but we would just need one point on the line) using the Zero Position of the track
   Hep3Vector TranslateToOrigin=Point-MuZeroPosition;
   // find orthogonal component of the point vector with respect to the line  
   Hep3Vector OrtVec=TranslateToOrigin - TranslateToOrigin.dot(MuDirection)*MuDirection;
   // its magnitude is the distance we need
   m_theDistance=OrtVec.mag();
  
  if (m_theDistance<m_maxDistance) accept=1;
  return accept;
}
// TOTRY: need to change it and put accept in the constructor so that 
// return does only send things back.


int ROIMon::AcceptTime(){

// get info from TileMuFitter

  int accept=0;
  double tileComTime =0.;
  
  Hep3Vector MuZeroPosition;
  Hep3Vector MuDirection;
  
  MuZeroPosition =  m_myComTime->GetCounterPosition() ;
  MuDirection =  m_myComTime->GetcosThetaDirection(); 
  
  if(m_myComTime->getTime()!=0 || MuZeroPosition.x()!=0 || MuZeroPosition.z()!=0  ) 
    {
      tileComTime = m_myComTime->getTime();
    }
  m_theTime = tileComTime;
  return accept;
}



float ROIMon::GetDistance(Hep3Vector Point){
   /*int acceptance=*/this->Accept(Point);
  return m_theDistance;
}

float ROIMon::GetTime(){
   /*int acceptance=*/this->AcceptTime();
  return m_theTime;
}

