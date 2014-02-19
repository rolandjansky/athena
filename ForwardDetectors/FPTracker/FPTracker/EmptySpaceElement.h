/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTACKER_EMPTYSPACEELEMENT_H
#define FPTACKER_EMPTYSPACEELEMENT_H

#include "IBeamElement.h"
#include "FPTrackerConstants.h"
#include "Point.h"
#include <ostream>

namespace FPTracker{
  class IParticle;
  class IBender;
  class Point;
  class TransversePoint;
  class EmptySpaceElement:public IBeamElement{
    // class EmptySpaceElement{
  public:

    EmptySpaceElement(double front, double back, const TransversePoint& beamAxis, Side side);
    EmptySpaceElement(
		      const IBeamElement::ConstPtr_t&,
		      const IBeamElement::ConstPtr_t&
		      );

    IBeamElement::ConstPtr_t clone() const;
    
    ~EmptySpaceElement();
    double frontFace()    const;
    double rearFace()     const;
    double zabspos()      const;
    double zsignedpos()   const;
    Point  position()     const;
    Side   side()         const;

    bool   isEndElement() const;
    std::string str()     const;
    
    std::string label() const;

    void track(IParticle&) const;
    void calibrate(IParticle&) const;

    typedef boost::shared_ptr< const EmptySpaceElement >  ConstPtr_t;

  private:

    double m_frontFace;
    double m_rearFace;
    double m_signedLength;
    double m_absLength;
    Point  m_pos;
    std::string m_label;
    Side m_side;

  };
}
#endif
