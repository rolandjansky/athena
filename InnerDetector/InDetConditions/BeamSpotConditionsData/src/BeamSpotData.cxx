#include "BeamSpotConditionsData/BeamSpotData.h"

namespace InDet
{

  BeamSpotData::BeamSpotData( int status, float posX, float posY, float posZ,
			      float sigmaX, float sigmaY, float sigmaZ,
			      float tiltX, float tiltY, float sigmaXY ) :
    m_status { status }, m_posX { posX }, m_posY { posY }, m_posZ { posZ },
    m_sigmaX { sigmaX }, m_sigmaY { sigmaY }, m_sigmaZ { sigmaZ },
    m_tiltX { tiltX }, m_tiltY { tiltY }, m_sigmaXY { sigmaXY },
    m_errPar { sigmaX, sigmaY, sigmaZ, tiltX, tiltY, sigmaXY },
    m_beamPos { posX, posY, posZ }
  {  
#ifndef SIMULATIONBASE
    float sigmaX_tilt = tan(tiltX) * sigmaZ;
    float sigmaY_tilt = tan(tiltY) * sigmaZ;
    float sigmaX_all = sqrt(sigmaX*sigmaX + sigmaX_tilt*sigmaX_tilt);
    float sigmaY_all = sqrt(sigmaY*sigmaY + sigmaY_tilt*sigmaY_tilt);
    Amg::MatrixX beamErr(3, 3);
    beamErr.setZero();
    beamErr.fillSymmetric(0, 0, sigmaX_all*sigmaX_all);
    beamErr.fillSymmetric(0, 1, sigmaXY);
    beamErr.fillSymmetric(1, 1, sigmaY_all*sigmaY_all);
    beamErr.fillSymmetric(0, 2, tan(tiltX)*sigmaZ*sigmaZ);
    beamErr.fillSymmetric(1, 2, tan(tiltY)*sigmaZ*sigmaZ);
    beamErr.fillSymmetric(2, 2, sigmaZ*sigmaZ);
    m_vertex = Trk::RecVertex(m_beamPos, beamErr);
#endif
  }

}
