/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_PARTICLE_H
#define FPTRACKER_PARTICLE_H

#include "IParticle.h"
#include "Point.h"
#include "TransversePoint.h"
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace FPTracker{

  class IBeamElement;
 class Particle: virtual public IParticle{
  public:
    Particle();
    Particle(double x, double y, double z, double px, double py, double pz);
  
    Point   position()                   const;
    Point&  position();
    Point   positionRelBeam()            const;
    Point   direction()                  const;
    Point&  direction();

    TransversePoint displacement()       const;

    void 
      setBeamCoordinateShift(const TransversePoint& );
    void
      setBeamCoordinateShift(const IBeamElement*);


    double  z()                          const;
    double&  z();
    double  momentum()                   const;
    bool   isOutOfAperture()             const;
    TransversePoint transversePosition() const;
    std::string str()                    const;

    void updatePositionFromDisplacement(const TransversePoint& ) ;

    void setOutOfAperture(bool out);
    void addMsg( const std::string& ); // get the messages back through << operator 

  private:
    Point  m_position;

    // ref point to calculate a displacement , eg relative to a magnet
    TransversePoint  m_beamCoordinateShift;
    double m_momentum;
    Point  m_direction;
    bool   m_outOfAperture;

    std::string m_msg;
  };

}
#endif
