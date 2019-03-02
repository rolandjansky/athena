/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKTrack.h"

#include <iostream>
#include <iomanip>
#include <TMath.h>

using namespace std;

ClassImp(FTKTrack)

FTKTrack::FTKTrack() :
   m_bankID(-1), m_roadID(-1), m_patternID(0), m_sectorID(-1), m_trackID(-1),
   m_combid(-1), m_extrid(-1),
   m_invpt(0), m_d0(0), m_rawd0(0) , m_phi(0), m_rawphi(0), m_z0(0), m_rawz0(0),
   m_ctheta(0), m_chi2(0), m_origchi2(0), 
   m_invptfw(0), m_d0fw(0), m_phifw(0), m_z0fw(0), m_cthetafw(0), m_chi2fw(0),
   m_nmissing(0),
   m_typemask(0), m_bitmask(0), m_ncoords(0), m_coord(0),
   m_nplanes(0), m_hits(0x0),
   m_nplanes_ignored(0), m_ssid(0),
   m_HF_rejected(0),m_HW_rejected(0),m_HW_track(-1),
   m_eventindex(-1), m_barcode(-1), m_barcode_frac(0.),
   m_connindex(-1)
{
   // nothing to do
}


FTKTrack::FTKTrack(const FTKTrack &cpy) :
   TObject(cpy)
{
   m_bankID   = cpy.m_bankID;
   m_roadID   = cpy.m_roadID;
   m_sectorID = cpy.m_sectorID;
   m_trackID  = cpy.m_trackID;
   m_combid   = cpy.m_combid;
   m_extrid   = cpy.m_extrid;
   m_invpt    = cpy.m_invpt;
   m_d0       = cpy.m_d0;
   m_rawd0    = cpy.m_rawd0;
   m_phi      = cpy.m_phi;
   m_rawphi   = cpy.m_rawphi;
   m_ctheta   = cpy.m_ctheta;
   m_z0       = cpy.m_z0;
   m_rawz0    = cpy.m_rawz0;
   m_chi2     = cpy.m_chi2;
   m_origchi2 = cpy.m_origchi2;
   m_invptfw  = cpy.m_invptfw;
   m_d0fw     = cpy.m_d0fw;
   m_phifw    = cpy.m_phifw;
   m_z0fw     = cpy.m_z0fw;
   m_cthetafw = cpy.m_cthetafw;
   m_chi2fw   = cpy.m_chi2fw; 
   m_nmissing = cpy.m_nmissing;
   m_typemask = cpy.m_typemask;
   m_bitmask  = cpy.m_bitmask;
   m_ncoords  = cpy.m_ncoords;

   m_eventindex   = cpy.m_eventindex;
   m_barcode      = cpy.m_barcode;
   m_barcode_frac = cpy.m_barcode_frac;
   m_connindex = cpy.m_connindex;

   m_HF_rejected = cpy.m_HF_rejected;
   
   m_HW_rejected = cpy.m_HW_rejected;
   m_HW_track    = cpy.m_HW_track;

   m_coord = new float[m_ncoords];
   for (int i=0;i<m_ncoords;++i)
      m_coord[i] = cpy.m_coord[i];

   m_patternID = cpy.m_patternID;

   m_nplanes = cpy.m_nplanes;
   m_hits = new FTKHit[m_nplanes];
   for (int i=0;i!=m_nplanes;++i)
     m_hits[i] = cpy.m_hits[i];

   m_nplanes_ignored = cpy.m_nplanes_ignored;
   m_ssid = new int[m_nplanes_ignored];
   for (int i=0;i<m_nplanes_ignored;++i) m_ssid[i] = cpy.m_ssid[i];
}


FTKTrack::FTKTrack(const int &ncoords, const int &nplanes) :
   m_bankID(-1), m_roadID(-1), m_patternID(0), m_sectorID(-1), m_trackID(-1),
   m_combid(-1), m_extrid(-1),
   m_invpt(0), m_d0(0), m_rawd0(0), m_phi(0), m_rawphi(0), m_z0(0), m_rawz0(0),
   m_ctheta(0), m_chi2(0), m_origchi2(0), 
   m_invptfw(0), m_d0fw(0), m_phifw(0), m_z0fw(0), m_cthetafw(0), m_chi2fw(0),
   m_nmissing(0),
   m_typemask(0), m_bitmask(0),   m_ncoords(ncoords),
   m_nplanes(nplanes), m_nplanes_ignored(0), m_ssid(0),
   m_HF_rejected(0),m_HW_rejected(0),m_HW_track(-1),
   m_eventindex(-1), m_barcode(-1), m_barcode_frac(0.),
   m_connindex(-1) 
{
  m_coord = new float[m_ncoords];
  for (int i=0;i<m_ncoords;++i) m_coord[i] = 0;

  m_hits = new FTKHit[m_nplanes];
}


FTKTrack::~FTKTrack()
{
   if (m_ncoords>0) delete [] m_coord;
   if (m_nplanes>0) delete [] m_hits;
   if (m_nplanes_ignored>0) delete [] m_ssid;
}

// if ForceRange==true, then phi = [-pi..pi)
void FTKTrack::setPhi(float phi, bool ForceRange) {
  if (ForceRange && fabs(phi)>100 && m_chi2<100) {
    // when phi is ridiculously large, there is no point in adjusting it. Also, since this is a BAD track, fail it if chi2 hasn't done so already
    FTKSetup::PrintMessageFmt(ftk::warn, "FTKTrack warning: fitted phi = %f, but chi2 = %f. Adjusting to chi2+=100!", phi, m_chi2);
    m_chi2+=100; // we want to fail this event anyway
  }
  else if (ForceRange && fabs(phi)<100){
    while (phi>= TMath::Pi()) phi -= TMath::TwoPi(); 
    while (phi< -TMath::Pi()) phi += TMath::TwoPi();
  }
  m_phi = phi;
}

// if ForceRange==true, then phi = [-pi..pi)
void FTKTrack::setPhiRaw(float phi, bool ForceRange) {
  if (ForceRange && fabs(phi)>100 && m_chi2<100) {
    // when phi is ridiculously large, there is no point in adjusting it. Also, since this is a BAD track, fail it if chi2 hasn't done so already
    FTKSetup::PrintMessageFmt(ftk::warn, "FTKTrack warning: fitted phi = %f, but chi2 = %f. Adjusting to chi2+=100!", phi, m_chi2);
    m_chi2+=100; // we want to fail this event anyway
  }
  else if (ForceRange && fabs(phi)<100){
    while (phi>= TMath::Pi()) phi -= TMath::TwoPi(); 
    while (phi< -TMath::Pi()) phi += TMath::TwoPi();
  }
  m_rawphi = phi;
}

// if ForceRange==true, then phifw = [-3..3)
void FTKTrack::setPhiFW(int phifw, bool ForceRange) {
  // when phifw is ridiculously large, there is no point in adjusting it. Also, since this is a BAD track, fail it if chi2 hasn't done so already
  if (ForceRange && abs(phifw)>100 && m_chi2fw<100) {
    FTKSetup::PrintMessageFmt(ftk::warn, "FTKTrack warning: fitted phifw = %d, but chi2fw = %d. Adjusting to chi2fw+=100!", phifw, m_chi2fw);
    m_chi2fw+=100; // we want to fail this event anyway
  }
  else if (ForceRange && abs(phifw)<100) {
    while (phifw>= round(TMath::Pi())) phifw -= round(TMath::TwoPi()); 
    while (phifw< -round(TMath::Pi())) phifw += round(TMath::TwoPi());
  }
  m_phifw = phifw;
}

/** set the number of coordinates connected with this track,
    a dim=0 is used to cleanup array content */
void FTKTrack::setNCoords(int dim)
{
  if (m_ncoords>0) delete [] m_coord;

  m_ncoords = dim;
  m_coord = new float[m_ncoords];
  for (int i=0;i<m_ncoords;++i)
    m_coord[i] = 0;
}


/** set the number of planes, dim=0 is used to clear the track */
void FTKTrack::setNPlanes(int dim)
{
  if (m_nplanes>0) delete [] m_hits;

  m_nplanes = dim;
  m_hits = new FTKHit[m_nplanes];
}

/** set the number of planes to extrapolate the 7L tracks,
    a dim=0 is used to cleanup array content */
void FTKTrack::setNPlanesIgnored(int dim)
{
  if (m_nplanes_ignored>0) delete [] m_ssid;

  m_nplanes_ignored = dim;
  m_ssid = new int[m_nplanes_ignored];
  for (int i=0; i<m_nplanes_ignored; ++i) m_ssid[i] = 0;
}


float FTKTrack::getParameter(int ipar, bool useraw) const
{
  switch (ipar) {
  case 0:
    return m_invpt;
    break;
  case 1:
    if (useraw) return m_rawd0;
    else return m_d0;
    break;
  case 2:
    if (useraw) return m_rawphi;
    else return m_phi;
    break;
  case 3:
    if (useraw) return m_rawz0;
    else return m_z0;
    break;
  case 4:
    return m_ctheta;
    break;
  }

  return 0.;
}


void  FTKTrack::setParameter(int ipar, float val, bool useraw)
{
  switch (ipar) {
  case 0:
    m_invpt = val;
    break;
  case 1:
    if (useraw) m_rawd0 = val;
    else m_d0 = val;
    break;
  case 2:
    if (useraw) m_rawphi = val;
    else m_phi = val;
    break;
  case 3:
    if (useraw) m_rawz0 = val;
    else m_z0 = val;
    break;
  case 4:
    m_ctheta = val;
    break;
  }
}

/** return the number of the common hits */
unsigned int FTKTrack::getNCommonHits(const FTKTrack &track, const float *HWdev) const
{
  unsigned int ncommon_hits(0);

  // match over the hits list
  int hit_ready=0;
  for (int ix=0;ix!=m_ncoords;++ix) {
    //Only want it to count as match if pixel x and y both match
    if (ix>(m_ncoords == 11 ? 5 : 7)) { //SCT hit (3 pixel hits when 11 coord, 4 when 16 coord)
        hit_ready=1;
    } else if (ix==0 || ix==2 || ix==4) {//Beginning of pixel hit
        hit_ready=0;
    }

    if ( !((m_bitmask & track.m_bitmask) & (1<<ix)) ) {
      // majority hits are always common. Actually, firmwire says they never are.
      //ncommon_hits += 1;
      continue;
    }

    double dist = TMath::Abs(getCoord(ix)-track.getCoord(ix));
    if ( dist < HWdev[ix] ) {
      if ( hit_ready == 1) {
        ++ncommon_hits; // a common hit
      } else {
        hit_ready=1;
      }
    }
  } // end loop over hits

  return ncommon_hits;
}

/** this function compare this track with a different tracks and returns:
     0 if according HW setup the two tracks are different
     1 are similar and the other has a worse quality parameter
     -1 are similar and the other has a better quality parameter */
int FTKTrack::HWChoice(const FTKTrack &other, const float *HW_dev,
		       const unsigned int HW_ndiff, int HW_level, bool FirstStage)
{
  int accepted(0);

  // Choose hitwarrior severity level: 1=in-road, 2=global
  //I don't think this actually works because the parent function is run at road-level
  if(HW_level<2) {
    if(getBankID()!=other.getBankID()) return accepted;
    if(getRoadID()!=other.getRoadID()) return accepted;
  }

  unsigned int ncommon_hits = getNCommonHits(other,HW_dev);
  // FlagAK - simplistic hitwarrior. Makes no distinction between 1D and 2D
  // If this doesn't work, we'll need to come up with something smarter
  // check the criteria for considering two tracks the same
  if (ncommon_hits>=HW_ndiff) {//Simple comparison to threshold number of matched hits
  //if ( m_ncoords-ncommon_hits<=HW_ndiff) { 
    // the track matches
    // We still want to compare to rejected tracks, so don't want the next bit
    /*
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
    //}
    */

    //We should never have more than 1 missing hit in the AUX HW. Necessary because NMissing double-counts pixel hits
    if ( FirstStage ? ((other.getNMissing()==0 && getNMissing()==0) || (other.getNMissing()>0 && getNMissing()>0)) : (other.getNMissing()==getNMissing())) {
      // keep the track with the best chi2
      if (other.getChi2() > getChi2()) {
	accepted = 1;
      }
      else {
	accepted = -1;
      }
    }
    //If first stage, then the track with 0 with fewer missing hits must have 0 missing
    else if (FirstStage ? (other.getNMissing() == 0) : (other.getNMissing() < getNMissing())) {
      // keep the track using more real points
      accepted = -1;
    }
    else if (FirstStage ? (getNMissing()==0) : (other.getNMissing() > getNMissing())) {
      accepted = 1;
    }
  }

  return accepted;
}


FTKTrack& FTKTrack::operator=(const FTKTrack &tocpy)
{
#ifdef PROTECT_SA // speedup
  if (this != &tocpy) 
#endif
    {
      m_bankID   = tocpy.m_bankID;
      m_roadID   = tocpy.m_roadID;
      m_sectorID = tocpy.m_sectorID;
      m_trackID  = tocpy.m_trackID;
      m_combid   = tocpy.m_combid;
      m_extrid   = tocpy.m_extrid;
      m_invpt    = tocpy.m_invpt;
      m_d0       = tocpy.m_d0;
      m_rawd0    = tocpy.m_rawd0;
      m_phi      = tocpy.m_phi;
      m_rawphi   = tocpy.m_rawphi;
      m_z0       = tocpy.m_z0;
      m_rawz0    = tocpy.m_rawz0;
      m_ctheta   = tocpy.m_ctheta;
      m_chi2     = tocpy.m_chi2;
      m_origchi2 = tocpy.m_origchi2;
      m_invptfw  = tocpy.m_invptfw;
      m_d0fw     = tocpy.m_d0fw;
      m_phifw    = tocpy.m_phifw;
      m_z0fw     = tocpy.m_z0fw;
      m_cthetafw = tocpy.m_cthetafw;
      m_chi2fw   = tocpy.m_chi2fw;

      m_nmissing = tocpy.m_nmissing;
      m_typemask = tocpy.m_typemask;
      m_bitmask  = tocpy.m_bitmask;

      m_eventindex   = tocpy.m_eventindex;
      m_barcode      = tocpy.m_barcode;
      m_barcode_frac = tocpy.m_barcode_frac;
      m_connindex = tocpy.m_connindex;

      m_HF_rejected = tocpy.m_HF_rejected;
      
      m_HW_rejected = tocpy.m_HW_rejected;
      m_HW_track    = tocpy.m_HW_track;
      
      if (m_ncoords!=tocpy.m_ncoords) {    
        // different number of coordinates
        // the old array has to be removed
        m_ncoords  = tocpy.m_ncoords;   
        if (m_coord) delete [] m_coord;
        m_coord = new float[m_ncoords];
      }
      else {
        // same number of coordinates, just updating the coordinates
        for (int i=0;i<m_ncoords;++i) {
          m_coord[i] = tocpy.m_coord[i];
        }
      }
      
      if (m_nplanes!=tocpy.m_nplanes) {
          // different number of planes, need to update the array
        m_nplanes = tocpy.m_nplanes;
        m_hits = new FTKHit[m_nplanes];
        for (int i=0;i!=m_nplanes;++i)
          m_hits[i] = tocpy.m_hits[i];
      }
      else {
        // same number of coordinates, updating the hit content
        for (int i=0;i<m_nplanes;++i) {
          m_hits[i] = tocpy.m_hits[i];
        }
      }

      if (m_nplanes_ignored!=tocpy.m_nplanes_ignored) {    
        m_nplanes_ignored  = tocpy.m_nplanes_ignored;   
        if (m_ssid) delete [] m_ssid;
        m_ssid = new int[m_nplanes_ignored];
      }
      else {
        for (int i=0;i<m_nplanes_ignored;++i) {
          m_ssid[i] = tocpy.m_ssid[i];
        }
      }
    }

  return *this;
}

ostream& operator<<(ostream &out, const FTKTrack &track)
{
  out << setw(8) << track.m_trackID;
  out << setw(8) << track.m_combid;
  out << setw(8) << track.m_extrid;
  out << setw(8) << track.m_bankID;
  out << setw(8) << track.m_roadID;
  out << setw(6) << track.m_barcode;
  out << setw(9) << track.m_barcode_frac;
  out << setw(6) << track.m_eventindex;
  out << setw(10) << track.m_HF_rejected;
  out << setw(8) << track.m_HW_rejected;
  out << setw(8) << track.m_HW_track;
  out << setw(8) << track.getBitmask();
  out << setw(3) << track.m_typemask;
  out << setw(3) << track.m_connindex;
  out << setw(3) << track.getNMissing();
  out << "\n";
  streamsize oldprec = out.precision();
  out.precision(4);
  out << setw(10) << track.m_phi;
  out.setf(ios_base::scientific);
  out.precision(2);
  out << setw(10) << track.m_invpt;
  out.unsetf(ios_base::scientific);
  out.precision(4);
  out << setw(10) << track.m_d0;
  out << setw(10) << track.m_ctheta;
  out << setw(10) << track.m_z0;
  out << setw(12) << track.m_chi2;
  out << setw(12) << track.m_origchi2;
  
  out << endl;
  // print the coordinates
  for (int ix=0;ix!=track.m_ncoords;++ix) {
    out << setw(8) << track.m_coord[ix];
  }
  out.precision(oldprec);

  out << endl;

  // print the hits  
  for (int ip=0;ip!=track.m_nplanes;++ip) {
    if (track.m_hits[ip].getSector()==-1) out << " X";
    else out << ' ' << track.m_hits[ip].getCoord();
  }
  
  return out;
}


void FTKTrack::Print(Option_t *) const
{
  cout << *this << endl;
}
