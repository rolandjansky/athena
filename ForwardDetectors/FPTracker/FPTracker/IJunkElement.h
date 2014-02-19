/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_IJUNKELEMENT_H
#define FPTRACKER_IJUNKELEMENT_H

#include <string>

namespace FPTracker{
  class Particle;
  class Point;
  class IJunkElement{
  public:
    virtual ~IJunkElement(){};
    virtual double frontFace()    const = 0;
    virtual double rearFace()     const = 0;
    virtual double zsignedpos()   const = 0;
    virtual double zabspos()      const = 0;
    virtual Point position()      const = 0;
    virtual int    side()         const = 0;
    virtual std::string label()   const = 0;
    virtual bool isEndElement()   const = 0;
    virtual void track(Particle&) const = 0;
    virtual std::string str()     const = 0;
  };
}
#endif
