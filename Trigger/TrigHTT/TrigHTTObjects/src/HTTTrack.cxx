/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTTrack.h"

#include <iostream>
#include <iomanip>
#include <TMath.h>

using namespace std;

ClassImp(HTTTrack)

HTTTrack::HTTTrack() :
   m_bankID(-1), m_patternID(0), m_firstSectorID(-1),  m_secondSectorID(-1), m_trackID(-1),
   m_qoverpt(0), m_d0(0), m_phi(0), m_z0(0),
   m_eta(0), m_chi2(0), m_origchi2(0),
   m_nmissing(0),
   m_typemask(0), m_hitmap(0),
   m_hits(0x0),
   m_eventindex(-1), m_barcode(-1), m_barcode_frac(0.)
{
   // nothing to do here
}

HTTTrack::~HTTTrack()
{
  if (m_hits.size()>0) m_hits.clear();
}

std::vector<float> HTTTrack::getCoords(unsigned ilayer) const
{
    std::vector<float> coords;
    if (m_hits.size() > ilayer)
    {
        coords.push_back(m_hits[ilayer].getEtaCoord());
        coords.push_back(m_hits[ilayer].getPhiCoord());
        return coords;
    }
    else
        throw std::range_error("HTTTrack::getCoords() out of bounds");
}

float HTTTrack::getEtaCoord(int ilayer) const {
  auto coords = getCoords(ilayer);
  if (coords.size() > 0){
    return coords.at(0);
  } else {
    throw std::range_error("HTTTrack::getCoord(layer,coord) out of bounds");
  }
}

float HTTTrack::getPhiCoord(int ilayer) const {
  auto coords = getCoords(ilayer);
  if (coords.size() > 1){
    return coords.at(1);
  } else {
    throw std::range_error("HTTTrack::getCoord(layer,coord) out of bounds");
  }
}

int HTTTrack::getNCoords() const {
  int nCoords = 0;
  for (auto& hit : m_hits){
    nCoords += hit.getDim();
  }
  return nCoords;
}

//set a specific position in m_hits
void HTTTrack::setHTTHit(int i, const HTTHit &hit) {
  if (m_hits.size() > (unsigned)i) {
    m_hits.at(i) = hit;
  }else {
    throw std::range_error("HTTTrack::setHTTHit() out of bounds");
  }
}

/** set the number of layers in the track. =0 is used to clear the track */
void HTTTrack::setNLayers(int dim)
{
  if (m_hits.size()>0) m_hits.clear();
  m_hits.resize(dim);
}


// if ForceRange==true, then phi = [-pi..pi)
void HTTTrack::setPhi(float phi, bool ForceRange) {
  if (ForceRange) {
    // when phi is ridiculously large, there is no point in adjusting it
    if(std::abs(phi)>100) {
      if(m_chi2<100) { // this is a BAD track, so fail it if chi2 hasn't done so already
	std::cout << "HTTTrack warning: fitted phi = " << phi << ", but chi2 = " << m_chi2
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

float HTTTrack::getParameter(int ipar) const
{
  switch (ipar) {
  case 0:
    return m_qoverpt;
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
    return m_eta;
    break;
  }

  return 0.;
}


void  HTTTrack::setParameter(int ipar, float val)
{
  switch (ipar) {
  case 0:
    m_qoverpt = val;
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
    m_eta = val;
    break;
  }
}


ostream& operator<<(ostream &out, const HTTTrack &track)
{
  out << setw(8) << track.m_trackID;
  out << setw(8) << track.m_bankID;
  out << setw(6) << track.m_barcode;
  out << setw(9) << track.m_barcode_frac;
  out << setw(6) << track.m_eventindex;
  out << setw(8) << track.getHitMap();
  out << setw(3) << track.m_typemask;
  out << setw(3) << track.getNMissing();
  out << "\n";
  streamsize oldprec = out.precision();
  out.precision(4);
  out << setw(10) << track.m_phi;
  out.setf(ios_base::scientific);
  out.precision(2);
  out << setw(10) << track.m_qoverpt;
  out.unsetf(ios_base::scientific);
  out.precision(4);
  out << setw(10) << track.m_d0;
  out << setw(10) << track.m_eta;
  out << setw(10) << track.m_z0;
  out << setw(12) << track.m_chi2;
  out << setw(12) << track.m_origchi2;

  out << endl;
  out.precision(oldprec);

  out << endl;

  // print the hits
  int iter = 0;
  for (auto& hit:track.m_hits) {
    out << "Hit " << iter << ": " << hit;
    iter++;
  }

  return out;
}


void HTTTrack::Print(Option_t *) const
{
  cout << *this << endl;
}

void HTTTrack::calculateTruth()
{
  static vector<HTTMultiTruth> mtv;
  mtv.clear();
  mtv.reserve( m_hits.size() );
  // don't loop over coordinates, since we only calculate truth *per hit* and not per coordinate, though hitmap is saved for coordinates, so be careful
  int icoord = 0;
  for (auto& thishit : m_hits) {
    if ( (m_hitmap & (0x1 << icoord))) { // only do if there is a hit here
      HTTMultiTruth this_mt( thishit.getTruth() );
      this_mt.assign_equal_normalization();
      mtv.push_back( this_mt );
    }
    icoord++;
    if (thishit.getDim() == 2) icoord++; // move up twice if we had a 2d hit
  }

  // compute the best geant match, the barcode with the largest number of hits contributing to the track.
  // frac is then the fraction of the total number of hits on the track attributed to the barcode.
  HTTMultiTruth mt( std::accumulate(mtv.begin(),mtv.end(),HTTMultiTruth(),HTTMultiTruth::AddAccumulator()) );
  // retrieve the best barcode and frac and store in the track.
  HTTMultiTruth::Barcode tbarcode;
  HTTMultiTruth::Weight tfrac;
  const bool ok = mt.best(tbarcode,tfrac);
  if( ok ) {
    setEventIndex(tbarcode.first);
    setBarcode(tbarcode.second);
    setBarcodeFrac(tfrac);
  }
}

void HTTTrack::setPassedOR(unsigned int code)
{
  m_ORcode=code;
}
