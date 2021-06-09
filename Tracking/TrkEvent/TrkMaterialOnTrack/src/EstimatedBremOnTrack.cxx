/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EstimatedBremOnTrack.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <string>


Trk::EstimatedBremOnTrack::EstimatedBremOnTrack() :
  Trk::MaterialEffectsBase(),
  m_retainedEnFraction(0.),
  m_sigmaRetainedEnFraction(0.),
  m_sigmaQoverPsquared(0.),
  m_bremSearchDirection(Trk::undefinedDirection)
{
}

Trk::EstimatedBremOnTrack::EstimatedBremOnTrack(double tInX0, double retainedEnFraction,
                                                double sEnFr,
                                                double sQoPs, const Surface& sf,
                                                SearchDirection searchDirection)  :
  Trk::MaterialEffectsBase(tInX0, sf,
                           1<<static_cast<int>(Trk::MaterialEffectsBase::BremPoint)),
  m_retainedEnFraction(retainedEnFraction),
  m_sigmaRetainedEnFraction(sEnFr),
  m_sigmaQoverPsquared(sQoPs),
  m_bremSearchDirection(searchDirection)
{  }

Trk::EstimatedBremOnTrack::EstimatedBremOnTrack(const Trk::EstimatedBremOnTrack& ebrot) :
  Trk::MaterialEffectsBase(ebrot),
  m_retainedEnFraction(ebrot.m_retainedEnFraction),
  m_sigmaRetainedEnFraction(ebrot.m_sigmaRetainedEnFraction),
  m_sigmaQoverPsquared(ebrot.m_sigmaQoverPsquared),
  m_bremSearchDirection(ebrot.m_bremSearchDirection)
{
}

Trk::EstimatedBremOnTrack& Trk::EstimatedBremOnTrack::operator= (const Trk::EstimatedBremOnTrack& rhs)
{
  if (this!=&rhs) {
    Trk::MaterialEffectsBase::operator=(rhs);
    m_retainedEnFraction = rhs.m_retainedEnFraction;
    m_sigmaRetainedEnFraction = rhs.m_sigmaRetainedEnFraction;
    m_sigmaQoverPsquared = rhs.m_sigmaQoverPsquared;
    m_bremSearchDirection = rhs.m_bremSearchDirection;
  }
  return *this;
}

Trk::EstimatedBremOnTrack::~EstimatedBremOnTrack()
{ }

// Overload of << operator for MsgStream for debug output
MsgStream& Trk::EstimatedBremOnTrack::dump ( MsgStream& sl ) const
{
  sl << "EstimatedBremOnTrack based on" << endmsg;
  Trk::MaterialEffectsBase::dump(sl);
  sl <<"EstBremOT retained p fraction z: " <<retainedEnFraction()<<endmsg;
  sl <<"EstBremOT sigma(z)             : " <<sigmaRetainedEnFraction()<<endmsg;
  sl <<"EstBremOT q/p noise term to cov: " <<sigmaQoverPsquared()<<endmsg;
  sl <<"EstBremOT brem search direction: " <<searchDirection()<<endmsg;
  return sl; 
}
//Overload of << operator for std::ostream for debug output
std::ostream& Trk::EstimatedBremOnTrack::dump ( std::ostream& sl ) const
{
  sl << "EstimatedBremOnTrack based on" <<std::endl;
  Trk::MaterialEffectsBase::dump(sl);
  sl <<"\t EstBremOT retained p fraction z: "<<retainedEnFraction()<<std::endl;
  sl <<"\t EstBremOT sigma(z)             : "<<sigmaRetainedEnFraction()<<std::endl;
  sl <<"\t EstBremOT q/p noise term to cov: "<<sigmaQoverPsquared()<<std::endl;
  sl <<"\t EstBremOT brem search direction: "<<searchDirection()<<std::endl;
  return sl; 
}

