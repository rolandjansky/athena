/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           RecVertex.cxx  -  Description
                             -------------------
    begin   : Autumn 2003
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    comments: original version by M. Elsing
    changes : 05/12/06 Kirill.Prokofiev@cern.ch EDM cleanup, switch to the fit
              quality instead of direct use of chi2 and ndf.

 ***************************************************************************/

#include "VxVertex/RecVertex.h"
#include "GaudiKernel/MsgStream.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"

namespace Trk {

  RecVertex::RecVertex() :
    Vertex(), m_fitQuality(Trk::FitQuality(0.,0.)) {}

  RecVertex::RecVertex(const Vertex& pos) :
      Vertex(pos),
      m_positionCovariance(3,3), 
      m_fitQuality(Trk::FitQuality(0.,0.))
  {
    m_positionCovariance.setZero();
  }

  RecVertex::RecVertex(const Amg::Vector3D &pos, const AmgSymMatrix(3) &cov,
                const double ndf, const double chi2) :
                Vertex(pos), m_positionCovariance(cov), m_fitQuality(Trk::FitQuality(chi2,ndf))
                {}

  RecVertex::RecVertex(const Amg::Vector3D &pos, const double ndf, const double chi2) :
    Vertex(pos), m_positionCovariance(3,3),
    m_fitQuality(Trk::FitQuality(chi2,ndf)) 
  {
    m_positionCovariance.setZero();
  }

  MsgStream& RecVertex::dump(MsgStream& sl) const {
    sl << "Trk::RecVertex: " << std::endl;
    Vertex::dump(sl);
    sl << Amg::toString(m_positionCovariance) << std::endl;
    sl << m_fitQuality << endmsg;
    return sl;
  }

  std::ostream& RecVertex::dump(std::ostream& sl) const {
    sl << "Trk::RecVertex: " << std::endl;
    Vertex::dump(sl);
    sl << Amg::toString(m_positionCovariance) << std::endl;
    sl << m_fitQuality << std::endl;
    return sl;
  }

  RecVertex::~RecVertex() = default;

  const Trk::FitQuality& RecVertex::fitQuality() const
  {return m_fitQuality;}
  
  const AmgSymMatrix(3)& RecVertex::covariancePosition() const
  {return m_positionCovariance; }
  
  void RecVertex::scaleError(float factor) {
    //scale covariance matrix in *weight* representation
    m_positionCovariance *= 1./factor;
  }
  
} // end of namespace
