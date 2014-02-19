/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_COLLIMATOR_H
#define FPTRACKER_COLLIMATOR_H

#include "IBeamElement.h"
#include "FPTrackerConstants.h"
#include "Point.h"
#include "boost/shared_ptr.hpp"
#include <vector>
#include <string>
#include <iosfwd>

namespace FPTracker{
  
  class IParticle;
  class TransversePoint;
  class Collimator:public IBeamElement{
  public:
    Collimator(double, double, Side);
    IBeamElement::ConstPtr_t clone()            const;
    double frontFace()                          const;
    double rearFace()                           const;
    double zabspos()                            const;
    double zsignedpos()                         const;
    Side   side()                               const;
    Point  position()                           const;
    std::string label()                         const;
    std::string str()                           const;

    bool isEndElement()                         const; 

    void track(IParticle&)                      const;
    //center collimator jaws on calibration particle trajectory
    void calibrate(IParticle&)                  const;


    typedef boost::shared_ptr< const Collimator >  ConstPtr_t;
    typedef std::vector< ConstPtr_t >              Container_t;

  private:

    static const std::string s_label;
    Point  m_position;
    double m_xaperture;
    mutable double m_xouter;
    mutable double m_xinner;
    Side   m_side;

    bool isOutOfAperture(const TransversePoint&) const;
  };



  class ostream;
  std::ostream& operator<<(std::ostream&, const Collimator&);
}
#endif
