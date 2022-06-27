// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTHough1DShiftTool.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date October 31st, 2020
 * @brief See header file.
 */

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTConfTools/HTTRegionSlices.h"
#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTConstants.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "HTTHough1DShiftTool.h"

#include "TH2.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <iostream>

static inline std::string instance_name(std::string const & s);
static inline float deltaPhi(float r, float qPt);
static inline boost::dynamic_bitset<> lshift(boost::dynamic_bitset<> const & b, int n);
static inline boost::dynamic_bitset<> rshift(boost::dynamic_bitset<> const & b, int n);
static inline void updateBinHits(std::vector<boost::dynamic_bitset<>> & binHits, unsigned layer, boost::dynamic_bitset<> const & b);

///////////////////////////////////////////////////////////////////////////////
// AthAlgTool

HTTHough1DShiftTool::HTTHough1DShiftTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname, name, ifc), 
  m_name(instance_name(name)),
  m_monitorFile((m_name + ".root").c_str(), "RECREATE")
{
  declareInterface<IHTTRoadFinderTool>(this);
}


StatusCode HTTHough1DShiftTool::initialize()
{
  // Config printout
  ATH_MSG_INFO("Phi range: (" << m_phiMin << "," << m_phiMax << "," << m_phiBins << ")");
  if (m_useDiff) ATH_MSG_INFO("useDiff Set True");
  if (m_variableExtend) ATH_MSG_INFO("variableExtend Set True");

  // Retrieve info
  ATH_CHECK(m_HTTBankSvc.retrieve());
  ATH_CHECK(m_HTTMapping.retrieve());
  m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();

  // Error checking
  if (m_phiMin >= m_phiMax || m_phiBins == 0) {      
    ATH_MSG_FATAL("initialize() Phi range invalid");
    return StatusCode::FAILURE;
  }

  // Fix inputs
  if (!m_hitExtendProperty.value().empty()) {
    if (m_hitExtendProperty.value().size() != m_nLayers) {
      ATH_MSG_FATAL("initialize() Hit extend must have size == nLayers");
      return StatusCode::FAILURE;
    } else {
      m_hitExtend = m_hitExtendProperty.value();
    }
  } else {
    m_hitExtend.value().resize(m_nLayers,0); // all 0
  }
  if (m_iterStep == 0) m_iterStep = m_hitExtend[m_iterLayer] * 2 + 1; // default 1

  // Copy correct r values
  auto pmap = m_HTTMapping->PlaneMap_1st();
  m_r.resize(m_nLayers);
  if (m_radii_file.value().empty()) {
    for (unsigned logilyr = 0; logilyr < m_nLayers; logilyr++)
      {
	if (pmap->getNSections(logilyr)>1) ATH_MSG_FATAL("initialsize() Can't handle multiple sections as of now");
	auto lyrinfo = pmap->getLayerInfo(logilyr,0);
	m_r[logilyr]=m_r_phys[int(lyrinfo.siTech)][lyrinfo.physLayer];
	ATH_MSG_INFO("Constant Radius Layer: " << logilyr << " = " << m_r[logilyr]);
      }
  } else {
    readRadii(m_radii_file);
  }

  // Fill convenience variables
  m_phiStep = (m_phiMax - m_phiMin) / m_phiBins;
  for (unsigned i = 0; i <= m_phiBins; i++)
    m_bins.push_back(m_phiMin + m_phiStep * i);
  m_regionMin = m_EvtSel->getRegions()->getMin(m_EvtSel->getRegionID());
  m_regionMax = m_EvtSel->getRegions()->getMax(m_EvtSel->getRegionID());

  // Calculate the shift amounts
  if (m_bitShift_path.value().empty()) calculateShifts();
  else readShifts(m_bitShift_path);
  printShifts();

  return StatusCode::SUCCESS;
}


// Fills m_shifts
void HTTHough1DShiftTool::calculateShifts()
{
  // Calculated d0shift if selected
  if (m_d0spread>0) calculated0Shifts();
    

  // Find start first
  int iterShift = 0;
  if (m_qptMin<0) {
    while (m_qptMin<qPt(m_r[m_iterLayer], m_phiStep * iterShift)) {
      iterShift-= m_iterStep;
    }
  } else {
    while (m_qptMin>qPt(m_r[m_iterLayer], m_phiStep * iterShift)) {
      iterShift+= m_iterStep;
    }
  }
  ATH_MSG_DEBUG("Starting shift" << iterShift);

  // Iterate over a steps of m_iterStep bin shift in m_iterLayer

  float qpt = qPt(m_r[m_iterLayer], m_phiStep * iterShift);
  while (qpt <= m_qptMax) 
    {
      std::vector<int> shifts; // Fill both +iterShift and -iterShift here
      for (unsigned i = 0; i < m_nLayers; i++)
        {
	  float dPhi = deltaPhi(m_r[i], qpt);
	  if (m_useDiff){
	    dPhi -= deltaPhi(m_r[0], qpt);
	  }

	  int shift = static_cast<int>(round(dPhi / m_phiStep));
	  if (shift >= (int)m_phiBins || shift <= -(int)m_phiBins) return;
	  shifts.push_back(shift);
        }

      // if there are d0shifts, apply them
      for (auto d0shift: m_d0shifts){	  
	m_shifts.push_back(applyVariation(shifts,d0shift,1));
	m_shifts.push_back(applyVariation(shifts,d0shift,-1));
      }
	
      m_shifts.push_back(shifts);
      iterShift += m_iterStep;
      qpt = qPt(m_r[m_iterLayer], m_phiStep * iterShift);
    }
  ATH_MSG_DEBUG("Ending shift" << iterShift);

  // figure out which patterns are the same after dropping a hit
  calculateDropable();
}


// Fills m_shifts
//
// This reads a bitshift pattern file, which is a simple space-separated text file
// where each row respresents one shift set / pattern. Each row contains
// (nLayers - 1) columns representing the shifts in layers [1, nLayers)
// (implicit 0 shift in layer 0), followed by a column that is a 'droplist'.
// The droplist is a string of 0s and 1s indicating which layers can be dropped
// for 4/5 matching, i.e (layer 0 is right-most bit).
//
// See /eos/atlas/atlascerngroupdisk/det-htt/sectors_constants/BitShiftHoughPatterns/2021-02-20/
void HTTHough1DShiftTool::readShifts(std::string const & filepath)
{
  // Open the file
  std::ifstream fin(filepath);
  if (!fin.is_open())
    {
      ATH_MSG_FATAL("Couldn't open " << filepath);
      throw ("HTTHough1DShiftTool couldn't open " + filepath);
    }

  // Variables to fill
  std::string line, droplist;
  int shift = 0;
  bool ok = true;

  // Parse the file
  while (getline(fin, line))
    {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream sline(line);
      std::vector<int> shifts;

      // Shifts
      shifts.push_back(0); // layer 0 is implicit 0 TODO
      for (unsigned layer = 1; layer < m_nLayers; layer++)
        {
	  ok = ok && (sline >> shift);
	  if (!ok) break;
	  shifts.push_back(shift);
        }
      if (!ok) break;
      m_shifts.push_back(shifts);

      // Dropable
      ok = ok && (sline >> droplist);
      ok = ok && (droplist.size() == m_nLayers);
      if (!ok) break;

      boost::dynamic_bitset<> drops(m_nLayers);
      for (unsigned i = 0; i < m_nLayers; i++)
	if (droplist[i] == '1')
	  drops[m_nLayers - i - 1] = true; // Droplist is printed with MSB on left
      m_dropable.push_back(drops);
    }

  if (!ok)
    {
      ATH_MSG_FATAL("Found error reading file at line: " << line);
      throw "HTTHough1DShiftTool read error";
    }

  ATH_MSG_INFO("Read " << m_shifts.size() << " patterns from " << filepath);
}


void HTTHough1DShiftTool::readRadii(std::string const & filepath)
{
  // Open the file
  std::ifstream fin(filepath);
  if (!fin.is_open())
    {
      ATH_MSG_FATAL("Couldn't open " << filepath);
      throw ("HTTHough1DShiftTool couldn't open " + filepath);
    }

  // Variables to fill
  std::string line;
  bool ok = true;

  // Parse the file
  while (getline(fin, line))
    {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream sline(line);
      std::vector<int> shifts;

      int subregion;
      ok = ok && (sline >> subregion);
      if (subregion==m_subRegion) {
	for (unsigned layer = 0; layer < m_nLayers; layer++)
	  {
	    float r;
	    ok = ok && (sline >> r);
	    if (!ok) break;
	    if (r==-1) {
	      ATH_MSG_WARNING("Radius in radiiFile is -1 for layer:" << layer);
	      r = 500.0; // dummy value that won't cause a crash, but won't work anywhere.
	    }
	    m_r[layer]=r;
		
	  }
      }
      if (!ok) break;
    }

  if (!ok)
    {
      ATH_MSG_FATAL("Found error reading file at line: " << line);
      throw "HTTHough1DShiftTool read error";
    }
}




///////////////////////////////////////////////////////////////////////////////
// Main Algorithm

StatusCode HTTHough1DShiftTool::getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) 
{
  roads.clear();
  m_roads.clear();

  // Get hit masks
  std::vector<boost::dynamic_bitset<>> hitMasks(makeHitMasks(hits));
  if (m_event < 5) drawHitMasks(hitMasks, m_name + "_e" + std::to_string(m_event));

  // Iterate over shifts
  for (size_t iShift = 0; iShift < m_shifts.size(); iShift++)
    {
      // Track hit layers in each bin
      std::vector<boost::dynamic_bitset<>> binHits(m_phiBins, boost::dynamic_bitset<>(m_nLayers));

      // Add the counts
      for (unsigned i = 0; i < m_nLayers; i++)
	updateBinHits(binHits, i, lshift(hitMasks[i], m_shifts[iShift][i]));

      // Check the threshold
      for (unsigned bin = 0; bin < m_phiBins; bin++)
        {
	  // First check we're not missing hits in not allowed layers
	  if (!m_dropable.empty() && (~(binHits[bin] | m_dropable[iShift])).any()) continue;

	  if (binHits[bin].count() >= m_threshold)
            {
	      int inner_bin = int(bin)-int(m_shifts[iShift][0]);
	      int outer_bin = int(bin)-int(m_shifts[iShift][m_nLayers-1]);
	      if ((inner_bin < 0) || (inner_bin >= int(m_phiBins))) continue;
	      if ((outer_bin < 0) || (outer_bin >= int(m_phiBins))) continue;
	      float phi1 = m_bins[inner_bin];
	      float phi2 = m_bins[outer_bin];
	      float phi_track = phitrkDiff(m_r[0],phi1,m_r[m_nLayers-1],phi2);
	      if (phi_track > m_regionMax.phi || phi_track < m_regionMin.phi) continue;

	      m_roads.push_back(makeRoad(hits, bin, m_shifts[iShift]));

            }
        }
    }

  roads.reserve(m_roads.size());
  for (HTTRoad_Hough & r : m_roads) roads.push_back(&r);

  m_event++;
  return StatusCode::SUCCESS;
}


std::vector<boost::dynamic_bitset<>> HTTHough1DShiftTool::makeHitMasks(const std::vector<const HTTHit*> & hits)
{
  std::vector<boost::dynamic_bitset<>> hitMasks(m_nLayers, boost::dynamic_bitset<>(m_phiBins));
  for (const HTTHit* hit : hits)
    {
      if (m_subRegion >= 0 && !m_HTTMapping->SubRegionMap()->isInRegion(m_subRegion, *hit)) continue;

      auto bins = getBins(hit);

      for (int i = std::max(bins.first, 0); i <= std::min(bins.second, (int)m_phiBins - 1); i++)
	hitMasks[hit->getLayer()][i] = true;
    }

  return hitMasks;
}


HTTRoad_Hough HTTHough1DShiftTool::makeRoad(const std::vector<const HTTHit*>& hits, int bin_track, std::vector<int> const & shifts)
{
  std::vector<const HTTHit*> road_hits;
  layer_bitmask_t hitLayers = 0;

  for (const HTTHit* hit : hits)
    {
      if (m_subRegion >= 0 && !m_HTTMapping->SubRegionMap()->isInRegion(m_subRegion, *hit)) continue;

      // Get the shifted bins of the hit
      auto bins = getBins(hit);
      bins.first += shifts[hit->getLayer()]; // note boost::dynamic_bitset stores [0] as the LSB, i.e. rightmost. So leftshift = +bin
      bins.second += shifts[hit->getLayer()];

      // Check if it's the same bin as the track
      if (bin_track >= bins.first && bin_track <= bins.second)
        {
	  road_hits.push_back(hit);
	  hitLayers |= 1 << hit->getLayer();
        }
    }

  auto sorted_hits = ::sortByLayer(road_hits);
  sorted_hits.resize(m_nLayers); // If no hits in last layer, return from sortByLayer will be too short

  HTTRoad_Hough r;
  r.setHitLayers(hitLayers);
  r.setHits(sorted_hits);
  r.setSubRegion(m_subRegion);
  r.setX(phitrk(bin_track,shifts).first); // will be a large negative number if invalid
  r.setY(qPt(m_r[m_iterLayer], m_phiStep * shifts[m_iterLayer]));
  return r;
}


///////////////////////////////////////////////////////////////////////////////
// Helpers

// Given a relative shift between iterLayer and layer 0, returns the corresponding qpt.
// This does a linear approximation of the Hough transform equation.
float HTTHough1DShiftTool::getPtFromShiftDiff(int shift) const
{
  if (m_iterLayer == 0) ATH_MSG_FATAL("getPtFromShiftDiff() iterLayer can't be 0");
  return (shift * m_phiStep / htt::A) / (m_r[m_iterLayer] - m_r[0]);
}


// Returns the range of bins (inclusive) given a phi and an extension in number of bins
std::pair<int, int> HTTHough1DShiftTool::getBins(const HTTHit* hit) const
{
  float phi = hit->getGPhi();
  float bin_extend = m_hitExtend[hit->getLayer()];
  float center = (phi - m_phiMin) / m_phiStep;
    
  if (m_variableExtend){
    float r = hit->getR();
    float dphimax = deltaPhi(r,m_qptMax); 
    float maxshift = +1*((sin(dphimax)*(r-m_r[hit->getLayer()]))/r)/m_phiStep;
    float dphimin = deltaPhi(r,m_qptMin); 
    float minshift = +1*((sin(dphimin)*(r-m_r[hit->getLayer()]))/r)/m_phiStep;

    center += (maxshift+minshift)/2.0;
    bin_extend += abs((maxshift-minshift)/2.0);

    ATH_MSG_DEBUG("Variable Extend: lyr=" << hit->getLayer() << " r=" << r << " m_r=" << m_r[hit->getLayer()] << " dr=" << r-m_r[hit->getLayer()] << " ext=" << bin_extend);      
  }

  int low = std::max(static_cast<int>(center - bin_extend),0);
  int high = std::min(static_cast<int>(center + bin_extend),static_cast<int>(m_phiBins-1));
  return { low, high };
}

float HTTHough1DShiftTool::qPt(float r, float deltaPhi) const
{
  if (m_useDiff) {
    float r1=m_r[0];
    float r2=r;
    float sign = deltaPhi>0 ? 1 : -1;
    return sign*1/(2*htt::A) * sqrt((4*sin(deltaPhi)*sin(deltaPhi))/( r1*r1 + r2*r2 -2*r2*r1*cos(deltaPhi)));
  }
  return sin(deltaPhi) / (htt::A * r);
}

float HTTHough1DShiftTool::phitrkDiff(float r1, float phi1,  float r2,  float phi2) const
{
  float phi_track = phi1+ atan2(r2-r1*cos(phi2-phi1),r1*sin(phi2-phi1)) - TMath::Pi()/2.0;
  return phi_track;
}


std::pair<float, bool> HTTHough1DShiftTool::phitrk(int bin, std::vector<int> const &  shifts ) const
{
  int inner_bin = int(bin)-int(shifts[0]);
  int outer_bin = int(bin)-int(shifts[m_nLayers-1]);
  if ((inner_bin < 0) || (inner_bin >= int(m_phiBins))) return {-100000,false};
  if ((outer_bin < 0) || (outer_bin >= int(m_phiBins))) return {-100000,false};
  float phi1 = m_bins[inner_bin];
  float phi2 = m_bins[outer_bin];	  
  float phi_track = phitrkDiff(m_r[0],phi1,m_r[m_nLayers-1],phi2)+0.5*m_phiStep;
  return {phi_track,true};
}

static inline std::string instance_name(std::string const & s)
{
  size_t pos = s.find_last_of(".");
  if (pos != std::string::npos)
    return s.substr(pos + 1);
  return s;
}

static inline float deltaPhi(float r, float qPt)
{
  return asin(htt::A * r * qPt);
}


static inline boost::dynamic_bitset<> lshift(boost::dynamic_bitset<> const & b, int n)
{
  if (n < 0) return rshift(b, -n);
  return b << n;
}

static inline boost::dynamic_bitset<> rshift(boost::dynamic_bitset<> const & b, int n)
{
  if (n < 0) return lshift(b, -n);

  boost::dynamic_bitset<> out(b >> n);

  // Force 0-padding
  for (int i = 0; i < n; i++)
    out[out.size() - 1 - i] = false;

  return out;
}

static inline void updateBinHits(std::vector<boost::dynamic_bitset<>> & binHits, unsigned layer, boost::dynamic_bitset<> const & b)
{
  for (size_t i = 0; i < b.size(); i++)
    if (b[i]) binHits[i].set(layer);
}

void HTTHough1DShiftTool::printHitMasks(std::vector<boost::dynamic_bitset<>> const & hitMasks) const
{
  std::stringstream ss;
  ss << "Hit Masks:\n";
  for (auto const & b : hitMasks)
    {
      ss << "\t";
      for (size_t i = 0; i < b.size(); i++)
        {
	  if (b[i]) ss << 1;
	  else ss << ".";
        }
      ss << "\n";
    }
  ATH_MSG_DEBUG(ss.str() << "\n\n");
}

void HTTHough1DShiftTool::drawHitMasks(std::vector<boost::dynamic_bitset<>> const & hitMasks, std::string const & name)
{
  m_monitorFile.cd();

  TH2I h(name.c_str(), "Hough 1D Shift;phi;layer",
	 m_phiBins, m_phiMin, m_phiMax,
	 m_nLayers, 0, m_nLayers
	 );

  for (size_t layer = 0; layer < m_nLayers; layer++)
    for (size_t i = 0; i < m_phiBins; i++)
      h.SetBinContent(i+1, layer+1, hitMasks[layer][i]); // +1 since root bins are 1-indexed

  h.Write();
}

void HTTHough1DShiftTool::drawHitMasks(std::vector<boost::dynamic_bitset<>> const & hitMasks, std::string const & name, std::vector<int> const & shifts)
{
  std::vector<boost::dynamic_bitset<>> shiftedMasks;
  for (size_t layer = 0; layer < m_nLayers; layer++)
    shiftedMasks.push_back(lshift(hitMasks[layer], shifts[layer]));

  drawHitMasks(shiftedMasks, name);
}

void HTTHough1DShiftTool::printShifts() const
{
  std::stringstream ss;
  ss << "Shifts:\n";
  for (size_t i = 0; i < m_shifts.size(); i++)
    {
      float qpt = qPt(m_r[m_iterLayer], m_phiStep * m_shifts[i][m_iterLayer]); // q/pT represented by this shift
      ss << "q/pT=" << qpt << "; ";
      for (int shift : m_shifts[i]) ss << shift << " ";
      if (!m_dropable.empty()) ss << m_dropable[i];
      ss << "\n";
    }
  ATH_MSG_INFO(ss.str());
}

void HTTHough1DShiftTool::calculated0Shifts()
{   
  int steps=1;
  while (true) {	
    float d0step = steps*m_phiStep*m_r[0];
    ATH_MSG_DEBUG("d0step = " << d0step);
    std::vector<int> d0shift;
    for (unsigned lyr = 0; lyr < m_nLayers; lyr++) {
      float phi_for_d0step = d0step/m_r[lyr];
      int shift = static_cast<int>(round(phi_for_d0step/ m_phiStep));
      d0shift.push_back(shift);
    }
    m_d0shifts.push_back(d0shift);
    if (d0step>m_d0spread) break;
    ++steps;
  }
  ATH_MSG_INFO("d0 Shifts Found = " << m_d0shifts.size());
}

std::vector<int> HTTHough1DShiftTool::applyVariation(std::vector<int> &base, std::vector<int> &var, int sign) const
{
  std::vector<int> retv;

  if (base.size()!=var.size()) {
    ATH_MSG_ERROR("Base and Var lengths must match " << base.size() << " " << var.size() );
  }
    
  for (unsigned i = 0; i < base.size(); i++) {
    retv.push_back(base[i]+sign*var[i]);
  }
  return retv;
}

std::vector<int> HTTHough1DShiftTool::shiftWithDrop(std::vector<int>& shift,unsigned droplayer) const
{
  std::vector<int> retv(m_nLayers-1);
  for (unsigned lyr = 0; lyr < m_nLayers; lyr++) {
    if (lyr !=droplayer) retv.push_back(shift[lyr]);
  }
  return retv;
}

void HTTHough1DShiftTool::calculateDropable()
{

  // one set per layer with shift set 
  std::vector< std::set< std::vector<int> > > reducedShifts(m_nLayers);

  m_dropable.resize(m_shifts.size());
  
  for (size_t iShift = 0; iShift < m_shifts.size(); iShift++)
    {      
      boost::dynamic_bitset<> drops(m_nLayers);
      for (unsigned droplayer =0; droplayer < m_nLayers; droplayer++) {
	if (reducedShifts[droplayer].insert(shiftWithDrop(m_shifts[iShift],droplayer)).second) {
	  // true if the is a new shift in the set
	  drops.set(droplayer);
	}
      }
      
      m_dropable[iShift]=drops;      
      
    }
  
}
