/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialEffectsBase.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

Trk::MaterialEffectsBase::MaterialEffectsBase() :
  m_tInX0(0.0),
  m_associatedSurface(0),
  m_typeFlags(0)
{
}

Trk::MaterialEffectsBase::MaterialEffectsBase(double tInX0,
                                              const Surface& sf,
                                              const std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern)  :
  m_tInX0(tInX0),
  m_associatedSurface(sf.associatedDetectorElement() ?
                      &sf                            :
                      sf.clone()                     ),
  m_typeFlags(typePattern.to_ulong())
{}

Trk::MaterialEffectsBase::MaterialEffectsBase(const Trk::MaterialEffectsBase& meot) :
  m_tInX0(meot.m_tInX0),
  m_associatedSurface(meot.m_associatedSurface!=0?
                      ( meot.m_associatedSurface->associatedDetectorElement()!=0?
                        meot.m_associatedSurface :
                        meot.m_associatedSurface->clone() ) :
                      0 ),
  m_typeFlags(meot.m_typeFlags)
{}

Trk::MaterialEffectsBase& Trk::MaterialEffectsBase::operator= (const Trk::MaterialEffectsBase& rhs)
{
  if (this!=&rhs) {
    m_tInX0 = rhs.m_tInX0;
    m_typeFlags = rhs.m_typeFlags;
    // copy only if assoc. surface is free and not part of detStore
    if (m_associatedSurface && !m_associatedSurface->associatedDetectorElement())
      delete m_associatedSurface;
    if (rhs.m_associatedSurface) {
      m_associatedSurface = (!rhs.m_associatedSurface->associatedDetectorElement()) ?
        rhs.m_associatedSurface->clone() : rhs.m_associatedSurface;
    } else { m_associatedSurface = 0; }
  }
  return *this;
}

Trk::MaterialEffectsBase::~MaterialEffectsBase()
{
  if (m_associatedSurface!=0 && m_associatedSurface->associatedDetectorElement()==0)
    delete m_associatedSurface;
}

std::string Trk::MaterialEffectsBase::dumpType() const{
  std::string type;
  for (int i=0; i<NumberOfMaterialEffectsTypes; ++i)
  {
    if ( (1<<i)&m_typeFlags )
    {
      switch ( i ) {
      case Trk::MaterialEffectsBase::ScatteringEffects:
        type+="Scattering ";
        break;
      case Trk::MaterialEffectsBase::EnergyLossEffects:
        type+="EnergyLoss ";
        break;
      case Trk::MaterialEffectsBase::MaterialThickness:
        type+="UpdatorInput ";
        break;
      case Trk::MaterialEffectsBase::BremPoint:
        type+="BremPoint ";
        break;
      case Trk::MaterialEffectsBase::UsesMeasurement:
        type+="UsesMeasurement ";
        break;
      case Trk::MaterialEffectsBase::FittedMaterialEffects:
        type+="FittedMaterialEffects ";
        break;
      default:
        type+="Unknown ";
        break;
      }
    } 
  } 
  return type;
}


// Overload of << operator for MsgStream for debug output
MsgStream& Trk::MaterialEffectsBase::dump ( MsgStream& sl ) const
{
	std::string name("MaterialEffects ");
	sl << name<<"type        : " << dumpType() <<endreq;
	sl << name<<"thickness/X0: " << thicknessInX0()<<endreq;
    sl << name<<"Surface     : " ;
    if (m_associatedSurface==NULL) {
      sl << "NULL";
    } else {
      if (m_associatedSurface->associatedDetectorElement()!=NULL)
        sl << "from detector Element";
      else sl << associatedSurface() << endreq;
    }
    sl << endreq;
	return sl; 
}
//Overload of << operator for std::ostream for debug output
std::ostream& Trk::MaterialEffectsBase::dump ( std::ostream& sl ) const
{
	sl <<"MaterialEffects type        : " <<dumpType() <<std::endl;
	sl <<"MaterialEffects thickness/X0: " <<thicknessInX0()<<std::endl;
    sl <<"MaterialEffects Surface     : ";
    if (m_associatedSurface==NULL) {
      sl << "NULL";
    } else {
      if (m_associatedSurface->associatedDetectorElement()!=NULL)
        sl << "from detector Element";
      else sl << associatedSurface() << std::endl;
    }
    sl << std::endl;
	return sl; 
}

