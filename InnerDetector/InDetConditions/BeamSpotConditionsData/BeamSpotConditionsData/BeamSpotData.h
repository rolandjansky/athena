#ifndef BEAMSPOTCONDITIONSDATA_BEAMSPOTDATA_H
#define BEAMSPOTCONDITIONSDATA_BEAMSPOTDATA_H

#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace InDet
{

  class BeamSpotData
  {
  public:
    BeamSpotData(int status, float posX, float posY, float posZ,
		 float sigmaX, float sigmaY, float sigmaZ,
		 float tiltX, float tiltY, float sigmaXY);

    virtual ~BeamSpotData() { }

    inline const Amg::Vector3D& beamPos() const { return m_beamPos; }

  private:

    int m_status;
    float m_posX;
    float m_posY;
    float m_posZ;
    float m_sigmaX;
    float m_sigmaY;
    float m_sigmaZ;
    float m_tiltX;
    float m_tiltY;
    float m_sigmaXY;

    Amg::Vector3D m_beamPos;

  };

}

CLASS_DEF(InDet::BeamSpotData,188569010,1)
CONDCONT_DEF(InDet::BeamSpotData,34275740);


#endif
