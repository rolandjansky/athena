/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_IPARTICLE_H
#define FPTRACKER_IPARTICLE_H

#include <ostream>
#include <string>

namespace FPTracker
{
  class IBeamElement;
  class Point;
  class TransversePoint;
  class IParticle
  {
  public:
    virtual ~IParticle();
    virtual Point   position()        const = 0;
    virtual Point&  position()              = 0;
    virtual Point   positionRelBeam() const = 0;
    virtual Point   direction()       const = 0;
    virtual Point&  direction()             = 0;
    
    virtual TransversePoint 
      displacement()                  const = 0;


    virtual void 
      setBeamCoordinateShift(const TransversePoint& ) = 0;
    virtual void
      setBeamCoordinateShift(const IBeamElement*) = 0;


    
    virtual double  z()                                                  const = 0;
    virtual double&  z()                                                       = 0; 
    virtual double  momentum()                                           const = 0;
    virtual bool   isOutOfAperture()                                     const = 0;
    virtual TransversePoint transversePosition()                         const = 0;
    virtual void updatePositionFromDisplacement(const TransversePoint& )       = 0;
    virtual std::string str()                                            const = 0;

    
    virtual void setOutOfAperture( bool out )          = 0;
    virtual void addMsg( const std::string& )          = 0;
  };
  
  std::ostream& operator<<(std::ostream&, const IParticle&);
}
#endif
