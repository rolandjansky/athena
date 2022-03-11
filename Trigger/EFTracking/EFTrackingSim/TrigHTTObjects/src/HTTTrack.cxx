/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <TMath.h>
#include <AsgMessaging/MessageCheck.h>

#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTConstants.h"

using namespace asg::msgUserCode;
using namespace std;

ClassImp(HTTTrack)
// first stage only

HTTTrack::HTTTrack() :
  m_doDeltaGPhis(false),
  m_bankID(-1), m_patternID(0), m_firstSectorID(-1), m_secondSectorID(-1), m_trackID(-1),
  m_qoverpt(0), m_d0(0), m_phi(0), m_z0(0),
  m_eta(0), m_chi2(0), m_origchi2(0),
  m_nmissing(0),
  m_typemask(0), m_hitmap(0),
  m_hits(0x0),
  m_eventindex(-1), m_barcode(-1), m_barcode_frac(0.), m_ORcode(1)
{
  // nothing to do here
}

HTTTrack::~HTTTrack()
{
  if (m_hits.size() > 0) m_hits.clear();
}

std::vector<float> HTTTrack::getCoords(unsigned ilayer) const
{
  std::vector<float> coords;
  if (ilayer >= m_hits.size())
    throw std::range_error("HTTTrack::getCoords() out of bounds");

  if (m_trackCorrType == TrackCorrType::None)
  {
    coords.push_back(m_hits[ilayer].getEtaCoord());
    coords.push_back(m_hits[ilayer].getPhiCoord());
  }
  else
  {
    coords = computeIdealCoords(ilayer);
  }

  return coords;
}

std::vector<float> HTTTrack::computeIdealCoords(unsigned ilayer) const
{
  std::vector<float> coords;

  // rho = 0.33 m * (pT / GeV) / (B/T)
  // B = 2 T so
  // rho = 0.33 m * (pT / GeV) / (2)
  // and then 2*rho =  0.33 m * (pT / GeV)
  // but distances for us are in mm, so 2*rho = 330 * (pT / GeV)
  // and 1/(2*rho) = (1 / (pT in GeV)) / 330
  double const trackTwoRhoInv = getQOverPt() / (330);
  double target_r = (m_trackStage == TrackStage::SECOND) ? htt::TARGET_R_2STAGE[ilayer] : htt::TARGET_R_1STAGE[ilayer];
  double hitGPhi = m_hits[ilayer].getGPhi();

  if (m_doDeltaGPhis) {
    double houghRho = 0.0003 * getHoughY(); //A*q/pT
    double expectedGPhi = getHoughX();

    hitGPhi += (m_hits[ilayer].getR() - target_r) * houghRho; //first order
    expectedGPhi -= target_r * houghRho; //first order

    if (m_trackCorrType == TrackCorrType::Second) {
      hitGPhi += (pow(m_hits[ilayer].getR() * houghRho, 3.0) / 6.0); //higher order
      expectedGPhi -= (pow(target_r * houghRho, 3.0) / 6.0); //higher order
    }

    double hitZ = m_hits[ilayer].getZ();
    if (m_hits[ilayer].getR() > 1e-8) {
      hitZ -= m_hits[ilayer].getGCotTheta() * (m_hits[ilayer].getR() - htt::TARGET_R_1STAGE[ilayer]); //first order
      if (m_trackCorrType == TrackCorrType::Second)
        hitZ -= (m_hits[ilayer].getGCotTheta() * pow(m_hits[ilayer].getR(), 3.0) * houghRho * houghRho) / 6.0; //higher order
    }

    coords.push_back(hitZ);
    coords.push_back(hitGPhi - expectedGPhi);
  }
  else {
    hitGPhi += (m_hits[ilayer].getR() - target_r) * trackTwoRhoInv; //first order
    if (m_trackCorrType == TrackCorrType::Second)
      hitGPhi += (pow(m_hits[ilayer].getR() * trackTwoRhoInv, 3.0) / 6.0); //higher order

    double z = m_hits[ilayer].getZ();
    if (m_hits[ilayer].getR() > 1e-8) {
      z -= m_hits[ilayer].getGCotTheta() * (m_hits[ilayer].getR() - target_r); //first order
      if (m_trackCorrType == TrackCorrType::Second)
        z -= m_hits[ilayer].getGCotTheta() * (pow(m_hits[ilayer].getR(), 3.0) * trackTwoRhoInv * trackTwoRhoInv) / 6.0; //higher order
    }

    coords.push_back(z);
    coords.push_back(hitGPhi);
  }

  return coords;
}

float HTTTrack::getEtaCoord(int ilayer) const {
  auto coords = getCoords(ilayer);
  if (coords.size() > 0) {
    return coords.at(0);
  }
  else {
    throw std::range_error("HTTTrack::getCoord(layer,coord) out of bounds");
  }
}

float HTTTrack::getPhiCoord(int ilayer) const {
  auto coords = getCoords(ilayer);
  if (coords.size() > 1) {
    return coords.at(1);
  }
  else {
    throw std::range_error("HTTTrack::getCoord(layer,coord) out of bounds");
  }
}

int HTTTrack::getNCoords() const {
  int nCoords = 0;
  for (auto& hit : m_hits) {
    nCoords += hit.getDim();
  }
  return nCoords;
}

//set a specific position in m_hits
void HTTTrack::setHTTHit(unsigned i, const HTTHit& hit)
{
  if (m_hits.size() > i)
    m_hits[i] = hit;
  else
    throw std::range_error("HTTTrack::setHTTHit() out of bounds");
}

/** set the number of layers in the track. =0 is used to clear the track */
void HTTTrack::setNLayers(int dim)
{
  if (m_hits.size() > 0) m_hits.clear();
  m_hits.resize(dim);
}


// if ForceRange==true, then phi = [-pi..pi)
void HTTTrack::setPhi(float phi, bool ForceRange) {
  if (ForceRange) {
    // when phi is ridiculously large, there is no point in adjusting it
    if (std::abs(phi) > 100) {
      if (m_chi2 < 100) { // this is a BAD track, so fail it if chi2 hasn't done so already
        ANA_MSG_INFO("HTTTrack warning: fitted phi = " << phi << ", but chi2 = " << m_chi2
          << ". Adjusting to chi2+=100!");
        m_chi2 += 100; // we want to fail this event anyway
      }
    }
    else {
      while (phi >= TMath::Pi()) phi -= TMath::TwoPi();
      while (phi < -TMath::Pi()) phi += TMath::TwoPi();
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


ostream& operator<<(ostream& out, const HTTTrack& track)
{

  out << "TRACK: ID=" << std::left << setw(8) << track.m_trackID;
  out << " SECTOR1=" << std::left << setw(8) << track.m_firstSectorID;
  out << " BANK=" << std::left << setw(8) << track.m_bankID;
  out << " BARCODE=" << std::left << setw(6) << track.m_barcode;
  out << " BARCODE_F=" << std::left << setw(9) << track.m_barcode_frac;
  out << " EVENT=" << std::left << setw(6) << track.m_eventindex;
  out << " HITMAP=" << std::left << setw(8) << track.getHitMap();
  out << " TYPE=" << std::left << setw(3) << track.m_typemask;
  out << " NMISS=" << std::left << setw(3) << track.getNMissing();
  out << "\n";
  streamsize oldprec = out.precision();
  out.precision(4);
  out << "    PHI=" << std::left << setw(10) << track.m_phi;
  out.setf(ios_base::scientific);
  out.precision(2);
  out << " Q/PT=" << std::left << setw(10) << track.m_qoverpt;
  out.unsetf(ios_base::scientific);
  out.precision(4);
  out << " d0=" << std::left << setw(10) << track.m_d0;
  out << " ETA=" << std::left << setw(10) << track.m_eta;
  out << " z0=" << std::left << setw(10) << track.m_z0;
  out << " Chi2=" << std::left << setw(12) << track.m_chi2;
  out << " OChi2=" << std::left << setw(12) << track.m_origchi2;

  out << endl;
  out.precision(oldprec);

  out << endl;

  // print the hits
  int iter = 0;
  for (auto& hit : track.m_hits) {
    out << "Hit " << iter << ": " << hit << "\n";
    iter++;
  }

  return out;
}


void HTTTrack::Print(Option_t*) const
{
  ANA_MSG_INFO(*this);
}

void HTTTrack::calculateTruth()
{
  static vector<HTTMultiTruth> mtv;
  mtv.clear();
  mtv.reserve(m_hits.size());

  // don't loop over coordinates, since we only calculate truth *per hit* and not per coordinate, though hitmap is saved for coordinates, so be careful
  for (auto& thishit : m_hits)
  {
    if (thishit.isReal())
    {
      HTTMultiTruth this_mt(thishit.getTruth());
      this_mt.assign_equal_normalization();
      if (thishit.isPixel())
        for (auto& x : this_mt)
          x.second *= 2;
      mtv.push_back(this_mt);
    }
  }

  // compute the best geant match, the barcode with the largest number of hits contributing to the track.
  // frac is then the fraction of the total number of hits on the track attributed to the barcode.
  HTTMultiTruth mt(std::accumulate(mtv.begin(), mtv.end(), HTTMultiTruth(), HTTMultiTruth::AddAccumulator()));
  HTTMultiTruth::Barcode tbarcode;
  HTTMultiTruth::Weight tfrac;
  const bool ok = mt.best(tbarcode, tfrac);
  if (ok)
  {
    setEventIndex(tbarcode.first);
    setBarcode(tbarcode.second);
    setBarcodeFrac(tfrac);
  }
  else
  {
    setEventIndex(-1);
    setBarcode(-1);
    setBarcodeFrac(0);
  }
}

void HTTTrack::setPassedOR(unsigned int code)
{
  m_ORcode = code;
}


