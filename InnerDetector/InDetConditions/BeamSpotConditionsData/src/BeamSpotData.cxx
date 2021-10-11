/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <cmath>



#include "BeamSpotConditionsData/BeamSpotData.h"

namespace InDet
{

  BeamSpotData::BeamSpotData( int status, float posX, float posY, float posZ,
			      float sigmaX, float sigmaY, float sigmaZ,
			      float tiltX, float tiltY, float sigmaXY ) :
    m_status { status },
    m_errPar { sigmaX, sigmaY, sigmaZ, tiltX, tiltY, sigmaXY },
    m_beamPos { posX, posY, posZ }
  {  
#ifndef SIMULATIONBASE
    float sigmaX_tilt = std::tan(tiltX) * sigmaZ;
    float sigmaY_tilt = std::tan(tiltY) * sigmaZ;
    float sigmaX_all = std::sqrt(sigmaX*sigmaX + sigmaX_tilt*sigmaX_tilt);
    float sigmaY_all = std::sqrt(sigmaY*sigmaY + sigmaY_tilt*sigmaY_tilt);
    Amg::MatrixX beamErr(3, 3);
    beamErr.setZero();
    beamErr.fillSymmetric(0, 0, sigmaX_all*sigmaX_all);
    beamErr.fillSymmetric(0, 1, sigmaXY);
    beamErr.fillSymmetric(1, 1, sigmaY_all*sigmaY_all);
    beamErr.fillSymmetric(0, 2, std::tan(tiltX)*sigmaZ*sigmaZ);
    beamErr.fillSymmetric(1, 2, std::tan(tiltY)*sigmaZ*sigmaZ);
    beamErr.fillSymmetric(2, 2, sigmaZ*sigmaZ);
    m_vertex = Trk::RecVertex(m_beamPos, beamErr);
#endif
  }

}
