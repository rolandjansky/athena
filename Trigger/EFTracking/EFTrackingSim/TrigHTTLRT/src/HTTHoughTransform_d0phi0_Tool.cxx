// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTHoughTransform_d0phi0_Tool.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date October 31st, 2020
 * @brief See header file.
 */

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTConstants.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTSectorBank.h"
#include "HTTHoughTransform_d0phi0_Tool.h"

#include "TH2.h"

#include <sstream>
#include <cmath>
#include <algorithm>

static inline int quant(double min, double max, unsigned nSteps, double val);
static inline double unquant(double min, double max, unsigned nSteps, int step);
template <typename T>
static inline std::string to_string(std::vector<T> v);
static inline std::string instance_name(std::string const & s);



///////////////////////////////////////////////////////////////////////////////
// AthAlgTool

HTTHoughTransform_d0phi0_Tool::HTTHoughTransform_d0phi0_Tool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname, name, ifc),
  m_name(instance_name(name)),
  m_monitorFile((m_name + ".root").c_str(), "RECREATE")
{
  declareInterface<IHTTRoadFinderTool>(this);
}


StatusCode HTTHoughTransform_d0phi0_Tool::initialize()
{
  // Move temp variables over from properties to struct
  m_parMin.phi = m_tempMin_phi;
  m_parMin.d0 = m_tempMin_d0;
  m_parMax.phi = m_tempMax_phi;
  m_parMax.d0 = m_tempMax_d0;
  
  // Debug
  ATH_MSG_INFO("Image size: " << m_imageSize_x << " x " << m_imageSize_y);
  ATH_MSG_INFO("Convolution size: " << m_convSize_x << " x " << m_convSize_y);
  ATH_MSG_INFO("Convolution: " << to_string(const_cast<std::vector<int>&>(m_conv.value())));
  ATH_MSG_INFO("Hit Extend: " << to_string(const_cast<std::vector<unsigned>&>(m_hitExtend_x.value())));

  // Retrieve info
  ATH_CHECK(m_HTTBankSvc.retrieve());
  ATH_CHECK(m_HTTMapping.retrieve());
  m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();

  // Error checking
  bool ok = false;
  if (!m_imageSize_x || !m_imageSize_y)
    ATH_MSG_FATAL("initialize() Image size must be greater than 0");
  else if (m_conv.size() != m_convSize_x * m_convSize_y)
    ATH_MSG_FATAL("initialize() Convolution sizes don't match");
  else if (!m_conv.empty() && (m_convSize_x % 2 == 0 || m_convSize_y % 2 == 0))
    ATH_MSG_FATAL("initialize() Convolution sizes must be odd");
  else if (!m_hitExtend_x.empty() && m_hitExtend_x.size() != m_nLayers)
    ATH_MSG_FATAL("initialize() Hit extentsion list must have size == nLayers");
  else if (m_threshold.size() % 2 != 1)
    ATH_MSG_FATAL("initialize() Threshold size must be odd");
  else
    ok = true;
  for (unsigned const scale : m_binScale) {
    if (m_imageSize_y % scale != 0) {
      ATH_MSG_FATAL("initialize() The imagesize is not divisible by scale");
      ok = false;
    }
  }
  if (!ok) return StatusCode::FAILURE;
  
  // Warnings / corrections
  if (m_localMaxWindowSize && !m_traceHits) {
    ATH_MSG_WARNING("initialize() localMaxWindowSize requires tracing hits, turning on automatically");
    m_traceHits = true;
  }

  if (m_idealGeoRoads) {
    if (m_useSectors) {
      ATH_MSG_WARNING("initialize() idealGeoRoads conflicts with useSectors, switching off HTT sector matching");
      m_useSectors = false;
      
    }
    if (!m_traceHits) {
      ATH_MSG_WARNING("initialize() idealGeoRoads requires tracing hits, turning on automatically");
      m_traceHits = true;
    }
  }
  
  
  // Fill convenience variables
  m_step_x = (m_parMax[m_par_x] - m_parMin[m_par_x]) / m_imageSize_x;
  m_step_y = (m_parMax[m_par_y] - m_parMin[m_par_y]) / m_imageSize_y;
  for (unsigned i = 0; i <= m_imageSize_x; i++)
    m_bins_x.push_back(unquant(m_parMin[m_par_x], m_parMax[m_par_x], m_imageSize_x, i));
  for (unsigned i = 0; i <= m_imageSize_y; i++)
    m_bins_y.push_back(unquant(m_parMin[m_par_y], m_parMax[m_par_y], m_imageSize_y, i));

  // Initialize combine layers
  m_nCombineLayers = *std::max_element(m_combineLayers.begin(), m_combineLayers.end()) + 1;
  m_combineLayer2D.resize(m_nCombineLayers);
  for (unsigned i = 0; i < m_combineLayers.size(); i++) {
    m_combineLayer2D[m_combineLayers[i]].push_back(i);
  }

  // Initialize scaled binnings
  for (unsigned const scale : m_binScale) {
    auto iter = m_yBins_scaled.find(scale);

    // if scale not found in keys, then push the scaled binning
    if (iter == m_yBins_scaled.end()) {
      unsigned new_size_y  = m_imageSize_y / scale;
      std::vector<size_t> yBins_scaled;
      for (unsigned i = 0; i <= new_size_y; i++) {
	yBins_scaled.push_back(scale * i);
      }
      m_yBins_scaled[scale] = yBins_scaled;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode HTTHoughTransform_d0phi0_Tool::finalize()
{
  m_monitorFile.Write();
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Main Algorithm

StatusCode HTTHoughTransform_d0phi0_Tool::getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) 
{
  ATH_MSG_DEBUG("Event: " << m_event << ", # hits: " << hits.size());
  roads.clear();
  m_roads.clear();

  Image image = createImage(hits);
  if (m_event < 5) drawImage(image, m_name + "_" + std::to_string(m_event));
  if (!m_conv.empty()) image = convolute(image);

  for (unsigned y = 0; y < m_imageSize_y; y++) {
    for (unsigned x = 0; x < m_imageSize_x; x++) {
      if (passThreshold(image, x, y)) {
	m_roads.push_back(createRoad(image(y,x).second, x, y));
      }
    }
  }

  roads.reserve(m_roads.size());
  for (HTTRoad_Hough & r : m_roads) roads.push_back(&r);

  if (roads.empty() && m_event >= 5 && m_event < 200)
    drawImage(image, m_name + "_" + std::to_string(m_event));

  m_event++;
  return StatusCode::SUCCESS;
}


HTTHoughTransform_d0phi0_Tool::Image HTTHoughTransform_d0phi0_Tool::createLayerImage(std::vector<unsigned> const & combine_layers, std::vector<HTTHit const *> const & hits, unsigned const scale) const
{

  Image image(m_imageSize_y, m_imageSize_x);

  for (HTTHit const * hit : hits) {

    bool belong1 = false, belong2 = false;
    for (auto layer : combine_layers) {
      if (belong1 || belong2) break;
      if (hit->getLayer() == layer)
	belong1 = true;
      if (hit->getLayer() == layer + 1)
	belong2 = true;
    }

    if ((!belong1 && !m_stereo) ||
	(m_subRegion >= 0 && !m_HTTMapping->SubRegionMap()->isInRegion(m_subRegion, *hit))) {
      continue;
    }
    else if (m_stereo && belong1) { /* Intentionally blank */ }
    else if (m_stereo && !(hit->getLayer() > 1 && (hit->getLayer() % 2 == 1 && belong2))) {
      continue;
    }

    // This scans over y (d0) because that is more efficient in memory, in C.
    // Unknown if firmware will want to scan over x instead.
    unsigned new_size_y  = m_imageSize_y / scale;

    for (unsigned y_ = 0; y_ < new_size_y; y_++) {
      int y_bin_min = m_yBins_scaled.at(scale)[y_];
      int y_bin_max = m_yBins_scaled.at(scale)[y_+1];

      // Find the min/max x bins
      auto xBins = yToXBins(y_bin_min, y_bin_max, hit);
      // Update the image
      for (int y = y_bin_min; y < y_bin_max; y++) {
	for (unsigned x = xBins.first; x < xBins.second; x++) {
	  image(y, x).first++;
	  if (m_traceHits) image(y, x).second.insert(hit);
	}
      }
    }
  }

  return image;
}

HTTHoughTransform_d0phi0_Tool::Image HTTHoughTransform_d0phi0_Tool::createImage(std::vector<HTTHit const *> const & hits) const
{
  Image image(m_imageSize_y, m_imageSize_x);

  for (unsigned i = 0; i < m_nCombineLayers ; i++) {
    Image layerImage = createLayerImage(m_combineLayer2D[i], hits, m_binScale[i]);
    if (i > 1 && i % 2 == 0 && m_stereo) ++i; // not sure how to deal with this for Combine Layer setting
    for (unsigned x = 0; x < m_imageSize_x; ++x) {
      for (unsigned y = 0; y < m_imageSize_y; ++y) {
	if (layerImage(y, x).first > 0) {
	  image(y, x).first++;
	  image(y, x).second.insert(layerImage(y, x).second.begin(), layerImage(y, x).second.end());
	}
      }
    }
  }

  return image;
}

HTTHoughTransform_d0phi0_Tool::Image HTTHoughTransform_d0phi0_Tool::convolute(Image const & image) const
{
  Image out(m_imageSize_y, m_imageSize_x);
  
  for (unsigned y0 = 0; y0 < m_imageSize_y; y0++) { // Loop over out
    for (unsigned x0 = 0; x0 < m_imageSize_x; x0++) { //
      for (unsigned r = 0; r < m_convSize_y; r++) { // Loop over conv
	int y = -static_cast<int>(m_convSize_y) / 2 + r + y0; // Indices of input
	for (unsigned c = 0; c < m_convSize_x; c++) {
	  int x = -static_cast<int>(m_convSize_x) / 2 + c + x0; //

	  if (y >= 0 && y < static_cast<int>(m_imageSize_y) && x >= 0 && x < static_cast<int>(m_imageSize_x)) {
	    int val = m_conv[r * m_convSize_x + c] * image(y, x).first;
	    if (val > 0) {
	      out(y0, x0).first += val;
	      out(y0, x0).second.insert(image(y, x).second.begin(), image(y, x).second.end());
	    }
	  }
	}
      }
    }
  }

  return out;
}

bool HTTHoughTransform_d0phi0_Tool::passThreshold(Image const & image, unsigned x, unsigned y) const
{
  // Pass window threshold
  unsigned width = m_threshold.size() / 2;
  if (x < width || (image.size(1) - x) < width) return false;
  for (unsigned i = 0; i < m_threshold.size(); i++)
    if (image(y, x - width + i).first < m_threshold[i]) return false;
  
  // Pass local-maximum check
  if (m_localMaxWindowSize)
    for (int j = -m_localMaxWindowSize; j <= m_localMaxWindowSize; j++)
      for (int i = -m_localMaxWindowSize; i <= m_localMaxWindowSize; i++) {
	if (i == 0 && j == 0) continue;
	if (y + j < image.size(0) && x + i < image.size(1)) {
	  if (image(y+j, x+i).first > image(y, x).first) return false;
	  if (image(y+j, x+i).first == image(y, x).first) {
	    if (image(y+j, x+i).second.size() > image(y, x).second.size()) return false;
	    if (image(y+j, x+i).second.size() == image(y, x).second.size()
		&& j <= 0 && i <= 0) return false;
	  }
	}
      }
  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Helpers


// Quantizes val, given a range [min, max) split into nSteps. Returns the bin below.
static inline int quant(double min, double max, unsigned nSteps, double val)
{
  return static_cast<int>((val - min) / (max - min) * nSteps);
}

// Returns the lower bound of the bin specified by step
static inline double unquant(double min, double max, unsigned nSteps, int step)
{
  return min + (max - min) * step / nSteps;
}

template <typename T>
static inline std::string to_string(std::vector<T> v)
{
  std::ostringstream oss;
  oss << "[";
  if (!v.empty()) {
    std::copy(v.begin(), v.end()-1, std::ostream_iterator<T>(oss, ", "));
    oss << v.back();
  }
  oss << "]";
  return oss.str();
}

static inline std::string instance_name(std::string const & s)
{
  size_t pos = s.find_last_of('.');
  if (pos != std::string::npos)
    return s.substr(pos + 1);
  return s;
}

double HTTHoughTransform_d0phi0_Tool::yToX(double y, HTTHit const * hit) const
{
  double x = 0;

  if (m_par_x == HTTTrackPars::IPHI && m_par_y == HTTTrackPars::ID0) {
    double r = hit->getR(); // TODO check this, and units
    double phi_hit = hit->getGPhi(); // TODO check this, and units
    x = -y/r + phi_hit;
  }
  else {
    ATH_MSG_ERROR("yToX() not defined for the current m_par selection");
  }
  
  return x;
}

// Find the min/max x bins of the hit's line, in each y bin. Max is exclusive.
// Note this assumes yToX is monotonic. Returns {0, 0} if hit lies out of bounds.
std::pair<unsigned, unsigned> HTTHoughTransform_d0phi0_Tool::yToXBins(size_t yBin_min, size_t yBin_max, HTTHit const * hit) const
{
  // Get float values
  double x_min = yToX(m_bins_y[yBin_min], hit);
  double x_max = yToX(m_bins_y[yBin_max], hit);
  if (x_min > x_max) std::swap(x_min, x_max);
  if (x_max < m_parMin[m_par_x] || x_min > m_parMax[m_par_x])
    return { 0, 0 }; // out of bounds

  // Get bins
  int x_bin_min = quant(m_parMin[m_par_x], m_parMax[m_par_x], m_imageSize_x, x_min);
  int x_bin_max = quant(m_parMin[m_par_x], m_parMax[m_par_x], m_imageSize_x, x_max) + 1; // exclusive

  // Extend bins
  if (!m_hitExtend_x.empty()) x_bin_min -= m_hitExtend_x[hit->getLayer()];
  if (!m_hitExtend_x.empty()) x_bin_max += m_hitExtend_x[hit->getLayer()];

  // Clamp bins
  if (x_bin_min < 0) x_bin_min = 0;
  if (x_bin_max > static_cast<int>(m_imageSize_x)) x_bin_max = m_imageSize_x;

  return { x_bin_min, x_bin_max };
}

void HTTHoughTransform_d0phi0_Tool::matchIdealGeoSector(HTTRoad_Hough & r) const
{
  float pt = r.getY()*0.001; // convert to MeV
  auto bounds = std::equal_range(htt::QOVERPT_BINS.begin(),htt::QOVERPT_BINS.end(),pt);
  int sectorbin = bounds.first-htt::QOVERPT_BINS.begin()-1;

  // those bins are for tracks between the values, can't be below first value or more than the last value
  if (sectorbin < 0) sectorbin = 0;
  if (sectorbin > static_cast<int>(htt::QOVERPT_BINS.size()-2)) sectorbin =  htt::QOVERPT_BINS.size()-2;
  std::vector<module_t> modules;

  for (unsigned int il = 0; il < r.getNLayers(); il++) {
    if (r.getNHits_layer()[il] == 0) {
      modules.push_back(-1);
      layer_bitmask_t wc_layers = r.getWCLayers();
      wc_layers |= (0x1 << il);
      r.setWCLayers(wc_layers);

      std::unique_ptr<HTTHit> wcHit = std::unique_ptr<HTTHit>(new HTTHit());
      wcHit->setHitType(HitType::wildcard);
      wcHit->setLayer(il);
      wcHit->setDetType(m_HTTMapping->PlaneMap_1st()->getDetType(il));
      r.setHits(il,std::vector<const HTTHit*>({wcHit.get()}));

    }
    else
      modules.push_back(sectorbin);
  }
  const HTTSectorBank* sectorbank = m_HTTBankSvc->SectorBank_1st();
  r.setSector(sectorbank->findSector(modules));
}

// Create road via hits only
HTTRoad_Hough HTTHoughTransform_d0phi0_Tool::createRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y) const {
  // Get the road hits
  std::vector<const HTTHit*> road_hits;
  layer_bitmask_t hitLayers = 0;
  for (const HTTHit * hit : hits) {
    road_hits.push_back(hit);
    hitLayers |= 1 << hit->getLayer();
  }

  auto sorted_hits = ::sortByLayer(road_hits);
  sorted_hits.resize(m_nLayers); // If no hits in last layer, return from sortByLayer will be too short

  HTTRoad_Hough r;
  r.setRoadID(m_roads.size());
  r.setPID(y * m_imageSize_y + x);
  if (m_idealGeoRoads) matchIdealGeoSector(r);
  r.setHitLayers(hitLayers);
  r.setHits(sorted_hits);
  r.setSubRegion(m_subRegion);
  r.setX(m_bins_x[x] + m_step_x/2);
  r.setY(m_bins_y[y] + m_step_y/2);
  return r;
}

// Creates a road from hits that pass through the given bin (x, y), and pushes it onto m_roads
HTTRoad_Hough HTTHoughTransform_d0phi0_Tool::createRoad(std::vector<std::vector<const HTTHit*>> const & hits, layer_bitmask_t hitLayers, unsigned x, unsigned y) const
{
  HTTRoad_Hough r;
  r.setRoadID(m_roads.size());
  r.setPID(y * m_imageSize_y + x);
  if (m_useSectors) r.setSector(m_HTTBankSvc->SectorBank_1st()->findSector(hits));
  else if (m_idealGeoRoads) matchIdealGeoSector(r);
  r.setHitLayers(hitLayers);
  r.setHits(hits);
  r.setSubRegion(m_subRegion);
  r.setX(m_bins_x[x] + m_step_x/2);
  r.setY(m_bins_y[y] + m_step_y/2);

  return r;
}


// Creates a road from hits that pass through the given bin (x, y), and pushes it onto m_roads
void HTTHoughTransform_d0phi0_Tool::addRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y)
{
  layer_bitmask_t hitLayers = 0;
  for (HTTHit const * hit : hits) {
    hitLayers |= 1 << hit->getLayer();
  }

  auto sorted_hits = ::sortByLayer(hits);
  sorted_hits.resize(m_nLayers); // If no hits in last layer, return from sortByLayer will be too short
  m_roads.push_back(createRoad(sorted_hits, hitLayers, x, y));
}

// Use this version of addRoad when hit tracing is turned off
void HTTHoughTransform_d0phi0_Tool::addRoad(std::vector<const HTTHit*> const & hits, unsigned x, unsigned y)
{
  // Get the road hits
  std::vector<HTTHit const *> road_hits;
  layer_bitmask_t hitLayers = 0;
  for (const HTTHit * hit : hits) {
    if (m_subRegion >= 0 && !m_HTTMapping->SubRegionMap()->isInRegion(m_subRegion, *hit)) continue;

    // get bin scaling for the hit
    unsigned bin_scale = 0;
    for (unsigned i = 0; i < m_nCombineLayers; i++) {
      for (unsigned const layer : m_combineLayer2D[i]) {
	if (hit->getLayer() == layer) {
	  bin_scale = m_binScale[layer];
	}
      }
    }

    unsigned y_bin_min = floor(1.0 * y / bin_scale) * bin_scale;
    unsigned y_bin_max = ceil(1.0 * y / bin_scale) * bin_scale;
    if (y_bin_min == y_bin_max) y_bin_max++;

    // Find the min/max x bins
    auto xBins = yToXBins(y_bin_min, y_bin_max, hit);
	
    if (x >= xBins.first && x < xBins.second && y >= y_bin_min && y < y_bin_max) {
      road_hits.push_back(hit);
      hitLayers |= 1 << hit->getLayer();
    }
  }

  auto sorted_hits = ::sortByLayer(road_hits);
  sorted_hits.resize(m_nLayers); // If no hits in last layer, return from sortByLayer will be too short

  m_roads.push_back(createRoad(sorted_hits, hitLayers, x, y));
}


// For debug use
void HTTHoughTransform_d0phi0_Tool::drawImage(Image const & image, std::string const & name)
{
  m_monitorFile.cd();

  TH2I h(name.c_str(), "Hough Transform;phi;d0 (mm)",
	 m_imageSize_x, m_parMin[m_par_x], m_parMax[m_par_x],
	 m_imageSize_y, m_parMin[m_par_y], m_parMax[m_par_y]
	 );

  for (unsigned y = 0; y < m_imageSize_y; y++)
    for (unsigned x = 0; x < m_imageSize_x; x++)
      h.SetBinContent(x+1, y+1, image(y, x).first); // +1 since root bins are 1-indexed

  h.Write();
}

