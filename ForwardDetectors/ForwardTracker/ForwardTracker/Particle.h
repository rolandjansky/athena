/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_PARTICLE_H
#define FORWARDTRACKER_PARTICLE_H

#include "IBeamElement.h"
#include "IParticle.h"
#include "Point.h"
#include "TransversePoint.h"

namespace ForwardTracker {

  class IBeamElement;
  
  class Particle: virtual public IParticle {
  
  public:
    
    Particle();
    Particle(double x, double y, double z, double px, double py, double pz, bool charge);
    
    Point           position()           const { return m_position; }
    Point&          position()                 { return m_position; }
    Point           positionRelBeam()    const { return m_position + m_beamCoordinateShift; }
    Point           direction()          const { return m_direction; }
    Point&          direction()                { return m_direction; }
    TransversePoint displacement()       const { return m_position.transverse() + m_beamCoordinateShift; }
    TransversePoint transversePosition() const { return m_position.transverse(); }
    double          z()                  const { return m_position.z(); }
    double&         z()                        { return m_position[2]; }
    double          momentum()           const { return m_momentum; }
    bool            charged()            const { return m_charged; }
    bool            isOutOfAperture()    const { return m_outOfAperture; }
    bool            debug()              const { return m_debug; }

    void setBeamCoordinateShift(const TransversePoint& p) { m_beamCoordinateShift = p; }
    void setBeamCoordinateShift(const IBeamElement*    b) { m_beamCoordinateShift = -1.*(b->position()).transverse(); }  
    void setOutOfAperture      (bool out)                 { if (out) m_outOfAperture = true; } // once out, always out
    void setDebug              (bool flag)                { m_debug = flag; }
    void addMsg                (const std::string& msg)   { m_msg += msg; }
    
    void updatePositionFromDisplacement(const TransversePoint& disp) {
      
      m_position[0] = disp[0] - m_beamCoordinateShift[0]; // when the displacement from the magnet center is equal to the distance from the beam axis,
      m_position[1] = disp[1] - m_beamCoordinateShift[1]; // particle position position is 0 in global coordinates.
    }

    std::string str() const;

  private:

    Point            m_position; // reference point to calculate a displacement, eg relative to a magnet
    TransversePoint  m_beamCoordinateShift;
    double           m_momentum;
    bool             m_charged;
    Point            m_direction;
    bool             m_outOfAperture;
    bool             m_debug;
    std::string      m_msg;
  };
}
#endif
