/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkSurfaces/DiscBounds.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>

Trk::DiscBounds::DiscBounds() :
    m_boundValues(DiscBounds::bv_length, 0.)
{}

Trk::DiscBounds::DiscBounds(double minrad, double maxrad, double hphisec) :
    m_boundValues(DiscBounds::bv_length, 0.)
{
    m_boundValues[DiscBounds::bv_rMin]          = minrad;
    m_boundValues[DiscBounds::bv_rMax]          = maxrad;    
    m_boundValues[DiscBounds::bv_averagePhi]    = 0.;    
    m_boundValues[DiscBounds::bv_halfPhiSector] = hphisec;    
    if (m_boundValues[DiscBounds::bv_rMin] >  m_boundValues[DiscBounds::bv_rMax]) 
        swap(m_boundValues[DiscBounds::bv_rMin],  m_boundValues[DiscBounds::bv_rMax]);
}

Trk::DiscBounds::DiscBounds(double minrad, double maxrad, double avephi, double hphisec) :
    m_boundValues(DiscBounds::bv_length, 0.)
{
    m_boundValues[DiscBounds::bv_rMin]          = minrad;
    m_boundValues[DiscBounds::bv_rMax]          = maxrad;    
    m_boundValues[DiscBounds::bv_averagePhi]    = avephi;    
    m_boundValues[DiscBounds::bv_halfPhiSector] = hphisec;    
    if (m_boundValues[DiscBounds::bv_rMin] >  m_boundValues[DiscBounds::bv_rMax]) 
        swap(m_boundValues[DiscBounds::bv_rMin],  m_boundValues[DiscBounds::bv_rMax]);
}


Trk::DiscBounds::~DiscBounds()
{}

bool Trk::DiscBounds::operator==(const Trk::SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::DiscBounds* discbo = dynamic_cast<const Trk::DiscBounds*>(&sbo);
  if (!discbo) return false;
  return ( m_boundValues == discbo->m_boundValues);
}

double Trk::DiscBounds::minDistance(const Amg::Vector2D& pos ) const
{
  const double pi2  = 2.*M_PI;

  double r  = pos[locR];  if(r==0.) return m_boundValues[DiscBounds::bv_rMin];
  double sf = 0.       ;
  double dF = 0.       ;

  if(m_boundValues[DiscBounds::bv_halfPhiSector] < M_PI) {

    dF = fabs(pos[locPhi]-m_boundValues[DiscBounds::bv_averagePhi]);
    if (dF>M_PI) dF=pi2-dF;
    dF-=m_boundValues[DiscBounds::bv_halfPhiSector];
    sf=r*sin(dF);
    if (dF > 0.) r*=cos(dF);

  }
  else {
    sf =-1.e+10;
  }

  if(sf <=0.) {

    double sr0 = m_boundValues[DiscBounds::bv_rMin]-r; if(sr0 > 0.) return sr0;
    double sr1 = r-m_boundValues[DiscBounds::bv_rMax]; if(sr1 > 0.) return sr1;
    if(sf < sr0) sf = sr0;
    if(sf < sr1) sf = sr1;
    return sf;
  }

  double sr0 = m_boundValues[DiscBounds::bv_rMin]-r; if(sr0 > 0.) return sqrt(sr0*sr0+sf*sf);
  double sr1 = r-m_boundValues[DiscBounds::bv_rMax]; if(sr1 > 0.) return sqrt(sr1*sr1+sf*sf);
  return sf;
}

// ostream operator overload

MsgStream& Trk::DiscBounds::dump( MsgStream& sl ) const
{
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << "Trk::DiscBounds:  (innerRadius, outerRadius, averagePhi, hPhiSector) = ";
    sl << "(" << this->rMin() << ", " << this->rMax() << ", " << this->averagePhi() << ", "<< this->halfPhiSector() << ")";
    sl << std::setprecision(-1);
    return sl;
}

std::ostream& Trk::DiscBounds::dump( std::ostream& sl ) const
{
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << "Trk::DiscBounds:  (innerRadius, outerRadius, hPhiSector) = ";
    sl << "(" << this->rMin() << ", " << this->rMax() << ", " << this->averagePhi() << ", "<< this->halfPhiSector() << ")";
    sl << std::setprecision(-1);
    return sl;
}

