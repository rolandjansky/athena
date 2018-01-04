/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKPool/FTKAthTrack.h"

#include <iostream>
using namespace std;

#include <TMath.h>

FTKAthTrack::FTKAthTrack() :
   m_bankID(-1), m_roadID(-1),
   m_patternID(0),
   m_sectorID(-1), m_trackID(-1),
   m_invpt(0), m_d0(0), m_phi(0), m_z0(0),
   m_ctheta(0), m_chi2(0), m_origchi2(0), m_nmissing(0), 
   m_typemask(0), m_bitmask(0), m_ncoords(0), m_coord(0),
   m_HF_rejected(0),m_HW_rejected(0),m_HW_track(-1),
   m_eventindex(-1), m_barcode(-1), m_barcode_frac(0.)
{
   // nothing to do
}

FTKAthTrack::FTKAthTrack(int ncoords) :
   m_bankID(-1), m_roadID(-1),
   m_patternID(0),
   m_sectorID(-1), m_trackID(-1),
   m_invpt(0), m_d0(0), m_phi(0), m_z0(0),
   m_ctheta(0), m_chi2(0), m_origchi2(0), m_nmissing(0), 
   m_typemask(0), m_bitmask(0), m_ncoords(ncoords),
   m_HF_rejected(0),m_HW_rejected(0),m_HW_track(-1),
   m_eventindex(-1), m_barcode(-1), m_barcode_frac(0.)
{
  m_coord = new float[m_ncoords];
  for (int ix=0;ix!=m_ncoords;++ix) m_coord[ix] = 0.f;
}


FTKAthTrack::FTKAthTrack(const FTKAthTrack &cpy)
{
   m_bankID   = cpy.m_bankID;
   m_roadID   = cpy.m_roadID;
   m_sectorID = cpy.m_sectorID;
   m_trackID  = cpy.m_trackID;
   m_invpt    = cpy.m_invpt;
   m_d0       = cpy.m_d0;
   m_phi      = cpy.m_phi;
   m_ctheta   = cpy.m_ctheta;
   m_z0       = cpy.m_z0;
   m_chi2     = cpy.m_chi2;
   m_origchi2 = cpy.m_origchi2;
   m_nmissing = cpy.m_nmissing;
   m_typemask = cpy.m_typemask;
   m_bitmask  = cpy.m_bitmask;
   m_ncoords  = cpy.m_ncoords;

   m_eventindex   = cpy.m_eventindex;
   m_eventindex   = cpy.m_eventindex;
   m_barcode      = cpy.m_barcode;
   m_barcode_frac = cpy.m_barcode_frac;

   m_HF_rejected = cpy.m_HF_rejected;
   
   m_HW_rejected = cpy.m_HW_rejected;
   m_HW_track    = cpy.m_HW_track;

   m_patternID = cpy.m_patternID;

   m_coord = new float[m_ncoords];
   for (int i=0;i<m_ncoords;++i)
      m_coord[i] = cpy.m_coord[i];
}



FTKAthTrack::~FTKAthTrack()
{
   if (m_ncoords>0) delete [] m_coord;
}

// if ForceRange==true, then phi = [-pi..pi)
void FTKAthTrack::setPhi(float phi, bool ForceRange) {
  if (ForceRange) {
    // when phi is ridiculously large, there is no point in adjusting it
    if(fabs(phi)>100) {
      if(m_chi2<100) { // this is a BAD track, so fail it if chi2 hasn't done so already
	std::cout << "FTKAthTrack warning: fitted phi = " << phi << ", but chi2 = " << m_chi2 
		  << ". Adjusting to chi2+=100!" << std::endl;
	m_chi2+=100; // we want to fail this event anyway
      }
    }
    else {
      while (phi>= TMath::Pi()) phi -= TMath::TwoPi(); 
      while (phi< -TMath::Pi()) phi += TMath::TwoPi();
    }
  }
  m_phi = phi;
}


/** set the number of coordinates connected with this track,
    a dim=0 is used to cleanup array content */
void FTKAthTrack::setNCoords(int dim)
{
  if (m_ncoords>0) delete [] m_coord;

  m_ncoords = dim;
  m_coord = new float[m_ncoords];
  for (int i=0;i<m_ncoords;++i)
    m_coord[i] = 0;
}

float FTKAthTrack::getParameter(int ipar) const
{
  switch (ipar) {
  case 0:
    return m_invpt;
    break;
  case 1:
    return m_d0;
    break;
  case 2:
    return m_phi;
    break;
  case 3:
    return m_z0;
    break;
  case 4:
    return m_ctheta;
    break;
  }

  return 0.;
}


/** return the number of the common hits */
unsigned int FTKAthTrack::getNCommonHits(const FTKAthTrack &track, const float *HWdev) const
{
  unsigned int ncommon_hits(0);

  // match over the hits list
  for (int ix=0;ix!=m_ncoords;++ix) {
    if ( !((m_bitmask & track.m_bitmask) & (1<<ix)) ) {
      // majority hits are always common
      ncommon_hits += 1;
      continue;
    }

    double dist = TMath::Abs(getCoord(ix)-track.getCoord(ix));
    if ( dist < HWdev[ix] ) {
      ++ncommon_hits; // a common hit
    }
  } // end loop over hits

  return ncommon_hits;
}


/** this function compare this track with a different tracks and returns:
     0 if according HW setup the two tracks are different
     1 are similar and the other has a worse quality parameter
     -1 are similar and the other has a better quality parameter */
int FTKAthTrack::HWChoice(const FTKAthTrack &other, const float *HW_dev,
		       const unsigned int HW_ndiff, int HW_level) const
{
  int accepted(0);

  // Choose hitwarrior severity level: 1=in-road, 2=global
  if(HW_level<2) {
    if(getBankID()!=other.getBankID()) return accepted;
    if(getRoadID()!=other.getRoadID()) return accepted;
  }

  unsigned int ncommon_hits = getNCommonHits(other,HW_dev);

  // FlagAK - simplistic hitwarrior. Makes no distinction between 1D and 2D
  // If this doesn't work, we'll need to come up with something smarter
  // check the criteria for considering two tracks the same
  if ( m_ncoords-ncommon_hits<=HW_ndiff) { 
    // the track matches
    if ( ( getHWRejected() && !other.getHWRejected()) ||
	 (!getHWRejected() &&  other.getHWRejected()) ) {
	    
      // keep the track in the accepted road
      if (getHWRejected()) {
	accepted = -1;
      }
      else {
	accepted = 1;
      }
	    
    }
    else if (other.getNMissing()==getNMissing()) {
      // keep the track with the best chi2
      if (other.getChi2() > getChi2()) {
	accepted = 1;
      }
      else {
	accepted = -1;
      }
    }
    else if (other.getNMissing() < getNMissing()) {
      // keep the track using more real points
      accepted = -1;
    }
    else if (other.getNMissing() > getNMissing()) {
      accepted = 1;
    }
  }

  return accepted;
}


void  FTKAthTrack::setParameter(int ipar, float val)
{
  switch (ipar) {
  case 0:
    m_invpt = val;
    break;
  case 1:
    m_d0 = val;
    break;
  case 2:
    m_phi = val;
    break;
  case 3:
    m_z0 = val;
    break;
  case 4:
    m_ctheta = val;
    break;
  }
}


float FTKAthTrack::getEta() const
{
  return TMath::ASinH(m_ctheta);
}



FTKAthTrack& FTKAthTrack::operator=(const FTKAthTrack &tocpy)
{
#ifdef PROTECT_SA // speedup
  if (this != &tocpy) 
#endif
    {
      m_bankID   = tocpy.m_bankID;
      m_roadID   = tocpy.m_roadID;
      m_sectorID = tocpy.m_sectorID;
      m_trackID  = tocpy.m_trackID;
      m_invpt    = tocpy.m_invpt;
      m_d0       = tocpy.m_d0;
      m_phi      = tocpy.m_phi;
      m_z0       = tocpy.m_z0;
      m_ctheta   = tocpy.m_ctheta;
      m_chi2     = tocpy.m_chi2;
      m_origchi2 = tocpy.m_origchi2;
      m_nmissing = tocpy.m_nmissing;
      m_typemask = tocpy.m_typemask;
      m_bitmask  = tocpy.m_bitmask;

      m_eventindex   = tocpy.m_eventindex;
      m_barcode      = tocpy.m_barcode;
      m_barcode_frac = tocpy.m_barcode_frac;
      
      m_HF_rejected = tocpy.m_HF_rejected;
      
      m_HW_rejected = tocpy.m_HW_rejected;
      m_HW_track    = tocpy.m_HW_track;
      
      if (m_ncoords!=tocpy.m_ncoords) {    
	m_ncoords  = tocpy.m_ncoords;   
	if (m_coord) delete [] m_coord;
	m_coord = new float[m_ncoords];
      }
      
      for (int i=0;i<m_ncoords;++i) {
	m_coord[i] = tocpy.m_coord[i];
      }
      
      m_eventindex   = tocpy.m_eventindex;
      m_barcode      = tocpy.m_barcode;
      m_barcode_frac = tocpy.m_barcode_frac;
    }
  
  return *this;
}
