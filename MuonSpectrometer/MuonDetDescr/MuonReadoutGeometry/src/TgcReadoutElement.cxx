/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 tgc
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TgcReadoutElement.cxx,v 1.4 2009-03-28 09:18:38 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/GenericTGCCache.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#include <vector>

#define TgcReadout_verbose false

namespace MuonGM {

TgcReadoutElement::TgcReadoutElement(GeoVFullPhysVol* pv, std::string stName,
                                     int zi, int fi, bool is_mirrored,
                                     MuonDetectorManager* mgr)
  : MuonClusterReadoutElement(pv, stName, zi, fi, is_mirrored, mgr),
    m_readout_type(-1), m_readoutParams(NULL)
{
    m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:TgcReadoutElement");
    //std::string gVersion = manager()->geometryVersion();
  setStationName(stName);
  // get the setting of the caching flag from the manager
  setCachingFlag(mgr->cachingFlag());
    
  m_ngasgaps     = 0;
  m_nstripplanes = 0;
  m_nwireplanes  = 0;
}


TgcReadoutElement::~TgcReadoutElement()
{
  clearCache();
}   


const Amg::Transform3D TgcReadoutElement::localToGlobalTransf(int gasGap) const
{
  const Amg::Vector3D gasgapP = localGasGapPos(gasGap);
  const Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
  return absTransform()*xfp;
}

const Amg::Transform3D TgcReadoutElement::localToGlobalTransf(Identifier id) const
{
  const Amg::Vector3D gasgapP = localGasGapPos(id);
  const Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
  return absTransform()*xfp;
}

const Amg::Vector3D TgcReadoutElement::localToGlobalCoords(Amg::Vector3D x, Identifier id) const
{
  const Amg::Vector3D gasgapP = localGasGapPos(id);
  const Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
  return absTransform()*xfp*x;
}

const Amg::Transform3D TgcReadoutElement::globalToLocalTransf(Identifier id) const
{
  return localToGlobalTransf(id).inverse();
}

const Amg::Vector3D TgcReadoutElement::globalToLocalCoords(Amg::Vector3D x, Identifier id) const
{
  return globalToLocalTransf(id)*x;
}

const Amg::Vector3D TgcReadoutElement::localGasGapPos(Identifier id) const
{
  const TgcIdHelper* idh = manager()->tgcIdHelper();
  int gasgap   = idh->gasGap(id);
  return localGasGapPos(gasgap);    
}

const Amg::Vector3D TgcReadoutElement::localGasGapPos(int gg) const
{
    Amg::Vector3D localP( m_wireplanez[gg-1], 0., 0. );
    if (TgcReadout_verbose) std::cerr<<" TgcReadoutElement::localGasGapPos("<<gg<<") is "<<localP<<std::endl;
    return localP;
}

const Amg::Vector3D TgcReadoutElement::gasGapPos(Identifier id) const
{
    const TgcIdHelper* idh = manager()->tgcIdHelper();
    int gasgap   = idh->gasGap(id);
    return gasGapPos(gasgap);    
}

const Amg::Vector3D TgcReadoutElement::gasGapPos(int gg) const
{
    const Amg::Vector3D localP = localGasGapPos(gg);
    const Amg::Transform3D tgcTrans = absTransform();
    if (TgcReadout_verbose) {
        std::cerr<<"TgcReadoutElement::gasGapPos got localGasGapPos "<<localP<<std::endl;
    }
    Amg::Vector3D globalP = tgcTrans * localP;
    if (TgcReadout_verbose) std::cerr<<"TgcReadoutElement::gasGapPos got globalGasGapPos "
                                     <<globalP<<std::endl;
    return globalP;
}

const Amg::Vector3D TgcReadoutElement::channelPos(int gasGap, int isStrip, int channel) const
{
  const Amg::Transform3D tgcTrans = absTransform();

  if (0 == isStrip) {
    return tgcTrans * localGangPos(gasGap, channel);
  }
  else {
    return tgcTrans * localStripPos(gasGap, channel);
  }
}

const Amg::Vector3D TgcReadoutElement::channelPos(Identifier id) const
{
  const TgcIdHelper* idh = manager()->tgcIdHelper();
  int isStrip = idh->isStrip(id);
  if (0 == isStrip) {
    return gangPos(id);
  }
  else {
    return stripPos(id);
  }
}

const Amg::Vector3D TgcReadoutElement::localChannelPos(int gasGap, int isStrip, int channel) const
{
  if (0 == isStrip) {
    return localGangPos(gasGap, channel);
  }
  else {
    return localStripPos(gasGap, channel);
  }
}

const Amg::Vector3D TgcReadoutElement::localChannelPos(Identifier id) const
{
  const TgcIdHelper* idh = manager()->tgcIdHelper();
  int isStrip = idh->isStrip(id);
  if (0 == isStrip) {
    return localGangPos(id);
  }
  else {
    return localStripPos(id);
  }
}

const Amg::Vector3D TgcReadoutElement::gangPos(int gasGap, int gang) const
{
  const Amg::Transform3D tgcTrans = absTransform();

  return tgcTrans * localGangPos(gasGap, gang);
}

const Amg::Vector3D TgcReadoutElement::gangPos(Identifier id) const
{
  const Amg::Transform3D tgcTrans = absTransform();

  return tgcTrans * localGangPos(id);
}

const Amg::Vector3D TgcReadoutElement::localGangPos(int gasGap, int gang) const
{
//  if (getReadoutVersion() != 1) {
//    std::cerr << "TgcReadoutElement::localGangPos FATAL Readout Version " << getReadoutVersion() << " is not supported at the moment! Version 1 is supported only." << std::endl;
//    return Amg::Vector3D(-9999,-9999,-9999);
//  }

  float x = localGasGapPos(gasGap).x();
  float y = 0.;

  int nWires=0;
  for (int igang=1; igang < gang; ++igang) {
    nWires += getNWires(gasGap,igang);
  }
  // measured from the first gang (short base) to the last gang (long base) as in TGC_Digitization
  //  float z = (0.5+nWires+getNWires(gasGap,gang)/2.)*wirePitch()-(getRsize()/2.-frameZwidth()); // Units of wirePitch are CLHEP::mm!

  // measured by centering the area covered by wires as in Amdc
  float z = (nWires+getNWires(gasGap,gang)/2.-getTotalWires(gasGap)/2.)*wirePitch(); // Units of wirePitch are CLHEP::mm!

  return Amg::Vector3D(x,y,z);
}

const Amg::Vector3D TgcReadoutElement::localGangPos(Identifier id) const
{
  const TgcIdHelper * tgcidh = manager()->tgcIdHelper();
  int gasGap = tgcidh->gasGap(id);
  int gang = tgcidh->channel(id);

  return localGangPos(gasGap, gang);
}

const Amg::Vector3D TgcReadoutElement::stripPos(int gasGap, int strip) const
{
  const Amg::Transform3D tgcTrans = absTransform();

  return tgcTrans * localStripPos(gasGap, strip);

}

const Amg::Vector3D TgcReadoutElement::stripPos(Identifier id) const
{
  const Amg::Transform3D tgcTrans = absTransform();

  return tgcTrans * localStripPos(id);

}

// Local position of center of strip wrt center of chamber

const Amg::Vector3D TgcReadoutElement::localStripPos(int gasGap, int strip) const
{
//  if (getReadoutVersion() != 1) {
//    std::cerr << "TgcReadoutElement::localStripPos FATAL Readout Version " << getReadoutVersion() << " is not supported at the moment! Version 1 is supported only." << std::endl;
//    return Amg::Vector3D(-9999,-9999,-9999);
//  }

  float x = localGasGapPos(gasGap).x();
  float y;
  float z = 0;
  if (0 > getStationEta()) {
    y = stripCtrX(gasGap, strip, z);
  }
  else { // left-handed coordinate in A side (global +z)
    y = -stripCtrX(gasGap, strip, z);
  }
  return Amg::Vector3D(x,y,z);
}

const Amg::Vector3D TgcReadoutElement::localStripPos(Identifier id) const
{
  const TgcIdHelper * tgcidh = manager()->tgcIdHelper();
  int gasGap = tgcidh->gasGap(id);
  int strip = tgcidh->channel(id);

  return localStripPos(gasGap, strip);
}

const std::string TgcReadoutElement::stationType() const
{
    return getStationType();
}

int TgcReadoutElement::chamberType() const
{
    return m_readout_type;
}

bool TgcReadoutElement::endcap() const
{
    return ("E" == stationType().substr(2,1));
}

bool TgcReadoutElement::forward() const
{
    return ("F" == stationType().substr(2,1));
}

bool TgcReadoutElement::doublet() const
{
    return (2 == Ngasgaps());
}

bool TgcReadoutElement::triplet() const
{
    return (3 == Ngasgaps());
}

float TgcReadoutElement::shortWidth() const
{
    return m_Ssize;
}

float TgcReadoutElement::longWidth() const
{
    return m_LongSsize;
}

float TgcReadoutElement::length() const
{
  const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
  return m_Rsize - (tgc->frame_h)*2.;
}

float TgcReadoutElement::thickness() const
{
  return m_Zsize;
}

float TgcReadoutElement::frameZwidth() const
{
  const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
  return tgc->frame_h;
}

float TgcReadoutElement::frameXwidth() const
{
  const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
  return tgc->frame_ab;
}

float TgcReadoutElement::chamberWidth(float z) const
{
    return shortWidth() + z*(longWidth() - shortWidth())/length();
}

float TgcReadoutElement::chamberMinPhi() const
{
    return -atan2((longWidth() - shortWidth())/2, length());
}

float TgcReadoutElement::chamberMaxPhi() const
{
    return atan2((longWidth() - shortWidth())/2, length());
}

float TgcReadoutElement::wirePitch() const
{
  return m_readoutParams->wirePitch();
}

int TgcReadoutElement::nPhiChambers() const
{
    return m_readoutParams->nPhiChambers();
}

int TgcReadoutElement::nPhiSectors() const
{
    int nPhiSec = nPhiChambers();
    if ("T4E" == stationType()) nPhiSec = 36;
    return nPhiSec;
}

int TgcReadoutElement::nGaps() const
{
    return m_readoutParams->nGaps();
}

int TgcReadoutElement::nVolumes() const
{
  const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
  return tgc->nlayers;
}

// Access to subvolume properties
// current implementation does not make sense (no clients on LXR) since TGC type for the 
// specific chamber should be used (not the genericCache)
// 
// std::string TgcReadoutElement::volumeType(int volume) const
// {
//   const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
//   return tgc->materials[volume];
// }
//
// float TgcReadoutElement::volumeYpos(int volume) const
// {
//   const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
//   return tgc->positions[volume];
// }
//
// float TgcReadoutElement::volumeThickness(int volume) const
// {
//   const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
//   return tgc->tck[volume];
// }

// Access to wire gang properties


int TgcReadoutElement::nGangs(int gasGap) const
{
    if(! validGap(gasGap) ) throw;
    return m_readoutParams->nGangs(gasGap) + 1;
}

int TgcReadoutElement::nWires(int gasGap, int gang) const
{
    if( ! validGang(gasGap, gang) ) throw;
    return getNWires(gasGap,gang);
}

int TgcReadoutElement::nWiresTotal(int gasGap) const
{
    if(! validGap(gasGap) ) throw;
    int total = 0;
    for (int i = 1; i <= nGangs(gasGap); ++i)
    {
        total += getNWires(gasGap, i);
    }
    return total;
}

int TgcReadoutElement::gangOffset(int gasGap) const
{
    if(! validGap(gasGap) ) throw;
    return (int)WireOffset(gasGap);
}

float TgcReadoutElement::wireCoverage(int gasGap) const
{
    if(! validGap(gasGap) ) throw;
    return wirePitch()*nWiresTotal(gasGap);
}

float TgcReadoutElement::gangCtrZ(int gasGap, int gang) const
{
    // recursively calculates position of gang in local Z coordinate

    if( ! validGang(gasGap, gang) ) throw;
    float z;
    if (1 == gang)
    {
      z = - length()/2 + frameZwidth() + wirePitch()*0.5
	  + gangLength(gasGap, gang)/2;
    }
    else
    {
        z = gangCtrZ(gasGap, gang-1)
            + gangLength(gasGap, gang-1)/2
            + gangLength(gasGap, gang)/2;
    }
    return z;
}

float TgcReadoutElement::gangMinZ(int gasGap, int gang) const
{
    if( ! validGang(gasGap, gang) ) throw;
    return gangCtrZ(gasGap, gang) - gangLength(gasGap, gang)/2;
}

float TgcReadoutElement::gangMaxZ(int gasGap, int gang) const
{
    if( ! validGang(gasGap, gang) ) throw;
    return gangCtrZ(gasGap, gang) + gangLength(gasGap, gang)/2;
}

// Access to wire gang dimensions (simple trapezoid with length along z)

float TgcReadoutElement::gangLength(int gasGap, int gang) const
{
    if( ! validGang(gasGap, gang) ) throw;
    return wirePitch()*nWires(gasGap, gang);
}

float TgcReadoutElement::gangShortWidth(int gasGap, int gang) const
{
    if( ! validGang(gasGap, gang) ) throw;
    return chamberWidth(gangMinZ(gasGap, gang))-frameXwidth()*2.;
}

float TgcReadoutElement::gangLongWidth(int gasGap, int gang) const
{
    if( ! validGang(gasGap, gang) ) throw;
    return chamberWidth(gangMaxZ(gasGap, gang))-frameXwidth()*2.;
}

#ifdef NDEBUG
float TgcReadoutElement::gangThickness(int /*gasGap*/, int /*gang*/) const
#else
float TgcReadoutElement::gangThickness(int gasGap, int gang) const
#endif
{
    assert(validGang(gasGap, gang));
    return 0.05*CLHEP::mm;
}

// Access to strip properties

int TgcReadoutElement::nStrips(int gasGap) const
{
    if(! validGap(gasGap) ) throw;
    return m_readoutParams->nStrips(gasGap);
}

#ifdef NDEBUG
float TgcReadoutElement::stripDeltaPhi(int /*gasGap*/) const
#else
float TgcReadoutElement::stripDeltaPhi(int gasGap) const
#endif
{
  assert(validGap(gasGap));

  if (reLog().level()<=MSG::DEBUG) {
    reLog()<<MSG::DEBUG<< "stripDeltaPhi WARINIG delta phi varies according to strip # for layout Q and following" <<endmsg;
    reLog()<< "therefore stripDeltaPhi does NOT correctly return delta phi."<<endmsg;
  }

  // number of strips in exclusive phi coverage of a chamber in T[1-3] and T4
  const float nDivInChamberPhi[4] = {29.5, 29.5, 29.5, 31.5};
  float dphi;

  int iStation = atoi(getStationType().substr(1,1).c_str());
  if (iStation != 4 || "E" != getStationType().substr(2,1)) { // except for station T4E
    dphi = 360.*CLHEP::degree/((float) getNPhiChambers())/nDivInChamberPhi[iStation-1];
  }
  else { // T4E
    dphi = 360.*CLHEP::degree/36./nDivInChamberPhi[iStation-1];
  }

  return dphi;
}

float TgcReadoutElement::stripDeltaPhi(int gasGap, int strip) const
{

  float dphi;
  dphi = stripDeltaPhi(gasGap);

  // half strip
  if (   (strip >= 31 && ((getStationEta() > 0 && gasGap == 1) || (getStationEta() < 0 && gasGap != 1)))
         || (strip <=  2 && ((getStationEta() > 0 && gasGap != 1) || (getStationEta() < 0 && gasGap == 1))))
    dphi = dphi/2.;

  return dphi;
}

float TgcReadoutElement::stripStaggerPhi(int gasGap) const
{
    // strips in first gap are staggered by one-half pitch angle

    if(! validGap(gasGap) ) throw;
    float stagger = 0.0;
    if ((1 == gasGap && 0 < getStationEta()) || (1 != gasGap && 0 > getStationEta()))
    {
        stagger = -stripDeltaPhi(gasGap, 16)/2;
    }
    return stagger;
}

float TgcReadoutElement::stripMinPhi(int gasGap, int strip) const
{
  // angle of lower edge of strip wrt center of chamber

  if( ! validStrip(gasGap, strip) ) throw;
  float stripMinPhi;

  // layout Q and following 
  double z = (getRsize()-2.*getPhysicalDistanceFromBase())/2.;
  Amg::Vector3D ctrChamber  = globalPosition();
  double minPhiLargeBase = atan(stripMinX(gasGap,strip, z)/(ctrChamber.perp()+z));
  double minPhiShortBase = atan(stripMinX(gasGap,strip,-z)/(ctrChamber.perp()-z));
  if (minPhiLargeBase < minPhiShortBase) {
    stripMinPhi = minPhiLargeBase;
  }
  else {
    stripMinPhi = minPhiShortBase;
  }
  return stripMinPhi;
}

float TgcReadoutElement::stripMaxPhi(int gasGap, int strip) const
{
    // angle of upper edge of strip wrt center of chamber

    if( ! validStrip(gasGap, strip) ) throw;
    float stripMaxPhi;

    // layout Q and following 
    double z = (getRsize()-2.*getPhysicalDistanceFromBase())/2.;
    Amg::Vector3D ctrChamber  = globalPosition();
    double maxPhiLargeBase = atan(stripMaxX(gasGap,strip, z)/(ctrChamber.perp()+z));
    double maxPhiShortBase = atan(stripMaxX(gasGap,strip,-z)/(ctrChamber.perp()-z));
    if (maxPhiLargeBase > maxPhiShortBase) {
      stripMaxPhi = maxPhiLargeBase;
    }
    else {
      stripMaxPhi = maxPhiShortBase;
    }
    return stripMaxPhi;
}
float TgcReadoutElement::stripCtrPhi(int gasGap, int strip) const
{
    // angle of center of strip wrt center of chamber

    if( ! validStrip(gasGap, strip) ) throw;
    float stripCtrPhi;

    stripCtrPhi = (stripMaxPhi(gasGap,strip) + stripMinPhi(gasGap,strip))/2.;
    return stripCtrPhi;
}

float TgcReadoutElement::stripCtrX(int gasGap, int strip, float z) const
{
    if( ! validStrip(gasGap, strip) ) throw;
    return (stripMinX(gasGap,strip,z)+stripMaxX(gasGap,strip,z))/2.;
}

float TgcReadoutElement::stripMinX(int gasGap, int strip, float z) const
{
    if( ! validStrip(gasGap, strip) ) throw;
    if ((getStationEta() > 0 && gasGap == 1) ||
	(getStationEta() < 0 && gasGap != 1)   )
      {
	return (getStripPositionOnLargeBase(strip)-getStripPositionOnShortBase(strip))/(getRsize()-2.*getPhysicalDistanceFromBase())*z
	  +(getStripPositionOnLargeBase(strip)+getStripPositionOnShortBase(strip))/2.;
      }
    else
      {
	return -(getStripPositionOnLargeBase(33-(strip-1))-getStripPositionOnShortBase(33-(strip-1)))/(getRsize()-2.*getPhysicalDistanceFromBase())*z
	  -(getStripPositionOnLargeBase(33-(strip-1))+getStripPositionOnShortBase(33-(strip-1)))/2.;
      }
}

float TgcReadoutElement::stripMaxX(int gasGap, int strip, float z) const
{
      if( ! validStrip(gasGap, strip) ) throw;
      if ((getStationEta() > 0 && gasGap == 1) ||
	  (getStationEta() < 0 && gasGap != 1)   )
      {
	return (getStripPositionOnLargeBase(strip+1)-getStripPositionOnShortBase(strip+1))/(getRsize()-2.*getPhysicalDistanceFromBase())*z
	  + (getStripPositionOnLargeBase(strip+1)+getStripPositionOnShortBase(strip+1))/2.;
      }
      else
      {
	return -(getStripPositionOnLargeBase(33-strip)-getStripPositionOnShortBase(33-strip))/(getRsize()-2.*getPhysicalDistanceFromBase())*z
	  - (getStripPositionOnLargeBase(33-strip)+getStripPositionOnShortBase(33-strip))/2.;
      }
}

// Access to strip dimensions (skewed trapezoid with length along z)
#ifdef NDEBUG
float TgcReadoutElement::stripLength(int /*gasGap*/, int /*strip*/) const
#else
float TgcReadoutElement::stripLength(int gasGap, int strip) const
#endif
{
    // projection of strip on local Z axis
    
    assert(validStrip(gasGap, strip));
    return length();
}

double TgcReadoutElement::stripWidth(int gasGap, int strip) const
{
    //!< stripWidth depends on global R (almost pointing geometry+trapezoidal strips); returns the width at the center of the plane    
    return (stripShortWidth(gasGap,strip)+stripLongWidth(gasGap,strip))/2.;
}
double TgcReadoutElement::StripWidth(int gasGap) const
{
    if ( nStrips(gasGap) > 1 )
    {
        // give the size of the central strip
        return stripWidth(gasGap, (int)nStrips(gasGap)/2);
    }
    else return 0.;
}
double TgcReadoutElement::StripPitch(int gasGap) const
{
    if ( nStrips(gasGap) > 1 )
    {
        // give the size of the central strip
        return stripPitch(gasGap, (int)nStrips(gasGap)/2);
    }
    else return 0.;
}

    
float TgcReadoutElement::stripShortWidth(int gasGap, int strip) const
{
    // projection of strip on local X axis at min(Z)

    if( ! validStrip(gasGap, strip) ) throw;
    if ((getStationEta() > 0 && gasGap == 1) ||
	(getStationEta() < 0 && gasGap != 1)   )
      {
	return getStripPositionOnShortBase(strip+1)-getStripPositionOnShortBase(strip);
      }
    else
      {
	return -(getStripPositionOnShortBase(33-strip)-getStripPositionOnShortBase(33-(strip-1)));
      }
}

float TgcReadoutElement::stripLongWidth(int gasGap, int strip) const
{
    // projection of strip on local X axis at max(Z)

    if( ! validStrip(gasGap, strip) ) throw;
    if ((getStationEta() > 0 && gasGap == 1) ||
	(getStationEta() < 0 && gasGap != 1)   )
      {
	return getStripPositionOnLargeBase(strip+1)-getStripPositionOnLargeBase(strip);
      }
    else
      {
	return -(getStripPositionOnLargeBase(33-strip)-getStripPositionOnLargeBase(33-(strip-1)));
      }
}

float TgcReadoutElement::stripSkew(int gasGap, int strip) const
{
    // angle of center of strip wrt center of chamber

    if( ! validStrip(gasGap, strip) ) throw;
    return stripCtrPhi(gasGap, strip);
}

double TgcReadoutElement::stripPitch(int gasGap, int strip) const
{
    //!< stripPitch depends on global R (almost pointing geometry+trapezoidal strips); returns the pitch at the center of the plane
    double z = 0.; 
    return StripPitch(gasGap, strip, z);
}

double TgcReadoutElement::StripPitch(int gasGap, int strip, float z) const
{
    //!< strip pitch depending on local z position (R in global coordinate)

   if( ! validStrip(gasGap, strip) ) throw;
   if (1 < strip && strip < 32 )
   {
     double pitchlow = (double) stripCtrX(gasGap,strip+1,z)-stripCtrX(gasGap,strip,z);
     double pitchhigh = (double) stripCtrX(gasGap,strip,z)-stripCtrX(gasGap,strip-1,z);
     if (pitchlow > pitchhigh)
     {
       return pitchlow;
     }
     else
     {
       return pitchhigh;
     }
   }
   else if (strip == 1)
   {
     return (double) stripCtrX(gasGap,strip+1,z)-stripCtrX(gasGap,strip,z);
   }
   else if (strip == 32)
   {
     return (double) stripCtrX(gasGap,strip,z)-stripCtrX(gasGap,strip-1,z);
   }
   else return 0.;
}

// Monte Carlo debug (returns strip or gang corresponding with the position)

int TgcReadoutElement::findChannel(int gasGap, int isStrip,
                                   Amg::Vector3D localPos) const
{
    if(! validGap(gasGap) ) throw;
    float x = localPos.x();
    float z = localPos.z();
    //float phi = atan2(x, globalPosition().x() + z);

    if (0 == isStrip)
    {
        for (int gang = 1; gang <= getNGangs(gasGap); ++gang)
        {
            if (z < gangMaxZ(gasGap, gang)) return gang;
        }
        return getNGangs(gasGap);
    }
    else
    {
      for (int strip = 1; strip <= getNStrips(gasGap); ++strip)
        {
	  if (stripMinX(gasGap,strip,z) < x && x < stripMaxX(gasGap,strip,z)) return strip;
	}
      return getNStrips(gasGap);
    }

}


int TgcReadoutElement::findGang(int gasGap, Amg::Vector3D localPos) const
{
    if(! validGap(gasGap) ) throw;
    float z = localPos.z();
    for (int gang = 1; gang <= getNGangs(gasGap); ++gang)
    {
        if (z < gangMaxZ(gasGap, gang)) return gang;
    }
    return getNGangs(gasGap);
}

int TgcReadoutElement::findStrip(int gasGap,
                                 Amg::Vector3D localPos, Amg::Vector3D /*globalPos*/) const
{
    if(! validGap(gasGap) ) throw;

    float x = localPos.x();

    float z = localPos.z();
    for (int strip = 1; strip <= getNStrips(gasGap); ++strip) {
      if (stripMinX(gasGap,strip,z) < x && x < stripMaxX(gasGap,strip,z)) return strip;
    }
    return getNStrips(gasGap);
}

// Utility methods

int TgcReadoutElement::gapVolume(int gasGap) const
{
    // returns the volume index corresponding to the gas gap index

    if(! validGap(gasGap) ) throw;
    int gap_index =  0;
    // int vol_index = -1;
    const GenericTGCCache* tgc = manager()->getGenericTgcDescriptor();
    for (int i = 0; i < tgc->nlayers; ++i)
    {
        if (isAgap(tgc->materials[i])) ++gap_index;
        if (gasGap == gap_index) return i;
    }

    throw;
}

bool TgcReadoutElement::isAgap(std::string volumeMaterial) const
{
    return ("TGCGas" == volumeMaterial);
}

bool TgcReadoutElement::validGap(int gasGap) const
{
    bool isValid = (1 <= gasGap && gasGap <= nGaps());
    if (!isValid) std::cerr<<" gas gap is out of range; limits are 1-"<<nGaps()<<std::endl;
    return isValid;
}

bool TgcReadoutElement::validGang(int gasGap, int gang) const
{
    if(! validGap(gasGap) ) throw;
    bool isValid =  (1 <= gang && gang <= (getNGangs(gasGap) + 1));
    return isValid;
}

bool TgcReadoutElement::validStrip(int gasGap, int strip) const
{
    if(! validGap(gasGap) ) throw;
    bool isValid = (1 <= strip && strip <= getNStrips(gasGap));
    if (!isValid) std::cerr<<"TgcReadoutElement::validStrip("<<gasGap<<","<<strip
                           <<") is false: strip out of range; limits are 1-"<<getNStrips(gasGap)<<std::endl;
    return isValid;
}

void TgcReadoutElement::print() const
{
  std::cout << getReadoutName() << std::endl;
}

// ******************************* sin stereo*************************************
double TgcReadoutElement::sinStereo(const Identifier & id) const {

  // sin stereo for R-strips
  Amg::Vector3D posStrip    = stripPos(id);
  const TgcIdHelper* idh = manager()->tgcIdHelper();
  int measPhi            = idh->isStrip(id);
  if (measPhi == 0) return 1.0;
  
  // return zero for phi strips in layout P
  //std::string gVersion = manager()->geometryVersion();

  int strip  = idh -> channel(id);
  int gasGap = idh -> gasGap(id);

  double z = (getRsize()-2.*getPhysicalDistanceFromBase())/2.;
  double tan_theta_2 = (stripCtrX(gasGap,strip,z)-stripCtrX(gasGap,strip,-z))
                      /(getRsize()-2.*getPhysicalDistanceFromBase());

  Amg::Vector3D ctrChamber = globalPosition();
  double cos_theta_1 = (ctrChamber.x()*posStrip.x()+ctrChamber.y()*posStrip.y())/ctrChamber.perp()/posStrip.perp();

  double theta = atan(tan_theta_2) - acos(cos_theta_1);
  if (tan_theta_2 < 0.) {
    theta = atan(tan_theta_2) - (-acos(cos_theta_1));
  }

  return sin(theta);
}
void  TgcReadoutElement::setIdentifier(Identifier id)
{
    m_id = id;
    const TgcIdHelper* idh = manager()->tgcIdHelper();
    IdentifierHash collIdhash = 0;
    IdentifierHash detIdhash  = 0;
    // set parent data collection hash id 
    int gethash_code = idh->get_module_hash(id, collIdhash);
    if (gethash_code != 0) 
	reLog()<<MSG::WARNING
	       <<"TgcReadoutElement --  collection hash Id NOT computed for id = "
	       <<idh->show_to_string(id)<<endmsg;
    m_idhash = collIdhash;
    // set RE hash id 
    gethash_code = idh->get_detectorElement_hash(id, detIdhash);
    if (gethash_code != 0) 
	reLog()<<MSG::WARNING
	       <<"TgcReadoutElement --  detectorElement hash Id NOT computed for id = "
	       <<idh->show_to_string(id)<<endmsg;
    m_detectorElIdhash = detIdhash;
}


void TgcReadoutElement::fillCache() const {

  if( !m_surfaceData ) m_surfaceData = new SurfaceData();
  else{
    reLog()<<MSG::WARNING<<"calling fillCache on an already filled cache" << endmsg;
    return;
  }
  const TgcIdHelper* idh = manager()->tgcIdHelper();
  Identifier parentID = idh->parentID(identify());
  
  // loop over all gas gaps 
  for( int gp=1;gp<=m_ngasgaps;++gp ){
    
    // loop over phi/eta projections
    for( int mp=1;mp>=0;--mp ){

      Identifier id = idh->channelID(parentID, gp, mp, 1);
      const Amg::Vector3D gasgapP = localGasGapPos(gp);
      const Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
      Amg::Transform3D trans3D = absTransform()*xfp;
      Amg::RotationMatrix3D muonTRotation(trans3D.rotation());
      Amg::RotationMatrix3D surfaceTRotation;
      surfaceTRotation.col(0) = muonTRotation.col(1);
      surfaceTRotation.col(1) = muonTRotation.col(2);
      surfaceTRotation.col(2) = muonTRotation.col(0);
      Amg::Transform3D trans(surfaceTRotation);
      if( mp==0 ) trans *= Amg::AngleAxis3D(M_PI/2.,Amg::Vector3D(0.,0.,1.));
      Amg::Translation3D t(trans3D.translation());
      
      trans.pretranslate(trans3D.translation());

      m_surfaceData->m_layerTransforms.push_back( trans );
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
	  
      if( mp == 1 ) {
	m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
	m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,1.));
      }
    }	
  }

  m_surfaceData->m_surfBounds.push_back(new Trk::TrapezoidBounds( m_Ssize/2.,m_LongSsize/2.,m_Rsize/2. ));     // phi measurement
  m_surfaceData->m_surfBounds.push_back(new Trk::RotatedTrapezoidBounds( m_Rsize/2.,m_Ssize/2.,m_LongSsize/2. ));  // eta measurement
}

bool TgcReadoutElement::containsId(Identifier id) const
{
  const TgcIdHelper* idh = manager()->tgcIdHelper();

  int gasGap  = idh->gasGap(id);
  if (gasGap<1 || gasGap >Ngasgaps()) return false;
    
  int isStrip = idh->isStrip(id);
  int ch = idh->channel(id);
  if (isStrip == 0) {
    if ( ch<1 || ch > nGangs(gasGap) ) return false;
  } else if (isStrip == 1) {
    if ( ch<1 || ch > nStrips(gasGap) ) return false;
  } else 
    return false;
    
  return true;
}

} // namespace MuonGM
