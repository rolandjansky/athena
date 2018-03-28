#include "BeamSpotConditionsData/BeamSpotData.h"

namespace InDet
{

  BeamSpotData::BeamSpotData( int status, float posX, float posY, float posZ,
			      float sigmaX, float sigmaY, float sigmaZ,
			      float tiltX, float tiltY, float sigmaXY ) :
    m_status { status }, m_posX { posX }, m_posY { posY }, m_posZ { posZ },
    m_sigmaX { sigmaX }, m_sigmaY { sigmaY }, m_sigmaZ { sigmaZ },
    m_tiltX { tiltX }, m_tiltY { tiltY }, m_sigmaXY { sigmaXY },
    m_beamPos { posX, posY, posZ }
  {  }

}
