/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VolumeExcluder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/VolumeExcluder.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

// Default constructor
Trk::VolumeExcluder::VolumeExcluder()
  : m_vol(nullptr)
{}

// constructor with volume
Trk::VolumeExcluder::VolumeExcluder(Trk::Volume* vol)
  : m_vol(vol)
{}

// copy constructor
Trk::VolumeExcluder::VolumeExcluder(const VolumeExcluder& ex)
  : Trk::AreaExcluder(ex)
  , m_vol(new Volume(*(ex.m_vol)))
{}

// destructor
Trk::VolumeExcluder::~VolumeExcluder()
{
  delete m_vol;
}

/** Assignment operator */
Trk::VolumeExcluder&
Trk::VolumeExcluder::operator=(const VolumeExcluder& vol)
{
  if (&vol != this) {
    delete m_vol;
    AreaExcluder::operator=(vol);
    m_vol = new Volume(*(vol.m_vol));
  }
  return *this;
}

Trk::VolumeExcluder*
Trk::VolumeExcluder::clone() const
{
  return new Trk::VolumeExcluder(*this);
}

