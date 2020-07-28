/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "PpmMappingTool.h"

namespace LVL1 {

// Static constants

const int PpmMappingTool::s_crates;
const int PpmMappingTool::s_modules;
const int PpmMappingTool::s_channels;

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode PpmMappingTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;
  setupMap();
  setupInverseMap();

  return StatusCode::SUCCESS;
}

StatusCode PpmMappingTool::finalize()
{
  return StatusCode::SUCCESS;
}
 
// Return eta, phi and layer mapping for given crate/module/channel

bool PpmMappingTool::mapping(const int crate, const int module,
               const int channel, double& eta, double& phi, int& layer) const
{
  if (crate < 0 || crate >= s_crates || module < 0 || module >= s_modules ||
      channel < 0 || channel >= s_channels) return false;

  const ModuleMap& modMap = m_crateInfo[crate];
  const ModuleInfo& modInfo = modMap[module];
  const double etaOffset     = modInfo.first.first;
  const double phiOffset     = modInfo.first.second;
  const CoordinateMap* currentMap    = modInfo.second;

  if (!currentMap) return false;

  // Set the output

  ChannelCoordinate relCoord = (*currentMap)[channel];
  if (relCoord.layer() == ChannelCoordinate::NONE) return false;

  relCoord.setEta(relCoord.eta() + etaOffset);
  eta   = etaSim(relCoord);
  phi   = relCoord.phi() + phiOffset;
  layer = (relCoord.layer() == ChannelCoordinate::EM) ? 0 : 1;
  return true;
}

// Return crate, module and channel mapping for given eta/phi/layer

bool PpmMappingTool::mapping(const double eta, const double phi,
                        const int layer, int& crate, int& module, int& channel) const
{
  const unsigned int invalidChanId = s_crates*s_modules*s_channels;

  const unsigned int key = etaPhiKey(eta, phi);
  EtaPhiMap::const_iterator iter = m_etaPhiMap.find(key);
  if (iter == m_etaPhiMap.end()) {
    msg(MSG::WARNING) << "Invalid eta/phi: " << eta
                      << "/" << phi << endmsg;
    return false;
  }
  const ChannelIds& ids(iter->second);
  const unsigned int chanId = (layer == 0) ? ids.first : ids.second;
  if (chanId == invalidChanId) {
    msg(MSG::WARNING) << "Invalid ChanId - shouldn't happen" << endmsg;
    return false;
  }
  crate   = chanId / (s_channels * s_modules);
  module  = (chanId / s_channels) % s_modules;
  channel = chanId % s_channels;
  return true;
}

// Set up crate/module map

void PpmMappingTool::setupMap()
{
  // Input to Output channel mappings.
  // Inputs are numbered 1-16 (x4) and outputs 0-63
  // There are four output sets obtained by adding 0,4,8,12 to out

  //          input =    1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
  const int out[16] = { 19,51,35, 3,18,50,34, 2,17,49,33, 1,16,48,32, 0 };

  // Mapping types.
  //
  // Type 1.
  // Eta -2.4 to 2.4.  Inputs for 4x4 block
  //
  //   +----+----+----+----+
  //   | 16 | 15 | 12 | 11 |
  //   +----+----+----+----+
  //   | 13 | 14 |  9 | 10 |
  //   +----+----+----+----+ PI/8
  //   |  4 |  3 |  8 |  7 |
  //   +----+----+----+----+
  //   |  1 |  2 |  5 |  6 |
  //   +----+----+----+----+
  //            0.4

  const int in1[16] = { 16,15,12,11,13,14, 9,10, 4, 3, 8, 7, 1, 2, 5, 6 };

  // Type 2.
  // Eta 2.4 to 2.9.  4x1 plus 2x2 block
  //
  //   +----+--------+--------+
  //   | 16 |        |        |
  //   +----+   15   |   12   |
  //   | 13 |        |        |
  //   +----+--------+--------+ PI/8
  //   |  4 |        |        |
  //   +----+    3   |    8   |
  //   |  1 |        |        |
  //   +----+--------+--------+
  //              0.5

  const int in2a[4] = { 16,13, 4, 1 };
  const int in2b[4] = { 15,12, 3, 8 };

  // Type 3.
  // Eta -2.9 to -2.4.  2x2 plus 4x1 block
  //
  //   +--------+--------+----+
  //   |        |        | 11 |
  //   |   16   |   12   +----+
  //   |        |        | 10 |
  //   +--------+--------+----+ PI/8
  //   |        |        |  7 |
  //   |    4   |    8   +----+
  //   |        |        |  6 |
  //   +--------+--------+----+
  //              0.5

  const int in3a[4] = { 16,12, 4, 8 };
  const int in3b[4] = { 11,10, 7, 6 };

  // Type 4.
  // Eta 2.9 to 3.2 and -3.2 to -2.9.  4x1 plus 4x1
  //          +                 -
  //   +--------+----+   +----+--------+
  //   |        |    |   |    |        |
  //   |   16   | 15 |   | 16 |   15   |
  //   |        |    |   |    |        |
  //   +--------+----+   +----+--------+
  //   |        |    |   |    |        |
  //   |   12   | 11 |   | 12 |   11   |
  //   |        |    |   |    |        |
  //   +--------+----+   +----+--------+ PI/4
  //   |        |    |   |    |        |
  //   |    8   |  7 |   |  8 |    7   |
  //   |        |    |   |    |        |
  //   +--------+----+   +----+--------+
  //   |        |    |   |    |        |
  //   |    4   |  3 |   |  4 |    3   |
  //   |        |    |   |    |        |
  //   +--------+----+   +----+--------+
  //         0.3               0.3

  const int in4a[4]  = { 16,12, 8, 4 };
  const int in4b[4]  = { 15,11, 7, 3 };

  // Type 5.
  // Eta 3.2 to 4.9 and -4.9 to -3.2 EM FCAL.  4x4
  //
  //   +----+----+----+----+
  //   |    |    |    |    |
  //   | 16 | 15 | 14 | 13 |
  //   |    |    |    |    |
  //   +----+----+----+----+
  //   |    |    |    |    |
  //   | 12 | 11 | 10 |  9 |
  //   |    |    |    |    |
  //   +----+----+----+----+ PI/2
  //   |    |    |    |    |
  //   |  8 |  7 |  6 |  5 |
  //   |    |    |    |    |
  //   +----+----+----+----+
  //   |    |    |    |    |
  //   |  4 |  3 |  2 |  1 |
  //   |    |    |    |    |
  //   +----+----+----+----+
  //            1.7

  const int in5[16]  = { 16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

  // Type 6.
  // Eta 3.2 to 4.9 and -4.9 to -3.2 Had FCAL2 and FCAL3.  4x2 plus 4x2
  // NB. FCAL2 and FCAL3 have the same eta/phi coordinates.
  //          FCAL2                 FCAL3
  //   +--------+--------+   +--------+--------+
  //   |        |        |   |        |        |
  //   |   16   |   14   |   |   15   |   13   |
  //   |        |        |   |        |        |
  //   +--------+--------+   +--------+--------+
  //   |        |        |   |        |        |
  //   |   12   |   10   |   |   11   |    9   |
  //   |        |        |   |        |        |
  //   +--------+--------+   +--------+--------+ PI/2
  //   |        |        |   |        |        |
  //   |    8   |    6   |   |    7   |    5   |
  //   |        |        |   |        |        |
  //   +--------+--------+   +--------+--------+
  //   |        |        |   |        |        |
  //   |    4   |    2   |   |    3   |    1   |
  //   |        |        |   |        |        |
  //   +--------+--------+   +--------+--------+
  //           1.7                   1.7

  const int in6a[8]  = { 16,14,12,10, 8, 6, 4, 2 };
  const int in6b[8]  = { 15,13,11, 9, 7, 5, 3, 1 };
  
  // Construct coordinate maps for each module type
  // Four blocks of each type make up the complete map

  for (int i = 0; i < 12; ++i) m_coordMaps.push_back(std::make_unique<CoordinateMap>());
  for (int block = 0; block < 4; ++block) {
    const int incr = block * 4;
    std::vector<std::unique_ptr<CoordinateMap> >::iterator pos = m_coordMaps.begin();

    // Map 0 : Type 1 EM
    addCoords(4,4,0.1,M_PI/32.,0.,block*M_PI/8.,in1,out,incr,
              ChannelCoordinate::EM, **pos);
    ++pos;

    // Map 1 : Type 1 Had
    addCoords(4,4,0.1,M_PI/32.,0.,block*M_PI/8.,in1,out,incr,
              ChannelCoordinate::HAD, **pos);
    ++pos;

    // Map 2 : Type 2 EM
    addCoords(4,1,0.1,M_PI/32.,0.0,block*M_PI/8.,in2a,out,incr,
              ChannelCoordinate::EM, **pos);
    addCoords(2,2,0.2,M_PI/16.,0.1,block*M_PI/8.,in2b,out,incr,
              ChannelCoordinate::EM, **pos);
    ++pos;

    // Map 3 : Type 2 Had
    addCoords(4,1,0.1,M_PI/32.,0.0,block*M_PI/8.,in2a,out,incr,
              ChannelCoordinate::HAD, **pos);
    addCoords(2,2,0.2,M_PI/16.,0.1,block*M_PI/8.,in2b,out,incr,
              ChannelCoordinate::HAD, **pos);
    ++pos;

    // Map 4 : Type 3 EM
    addCoords(2,2,0.2,M_PI/16.,0.0,block*M_PI/8.,in3a,out,incr,
              ChannelCoordinate::EM, **pos);
    addCoords(4,1,0.1,M_PI/32.,0.4,block*M_PI/8.,in3b,out,incr,
              ChannelCoordinate::EM, **pos);
    ++pos;

    // Map 5 : Type 3 Had
    addCoords(2,2,0.2,M_PI/16.,0.0,block*M_PI/8.,in3a,out,incr,
              ChannelCoordinate::HAD, **pos);
    addCoords(4,1,0.1,M_PI/32.,0.4,block*M_PI/8.,in3b,out,incr,
              ChannelCoordinate::HAD, **pos);
    ++pos;

    // Map 6 : Type 4 EM positive eta
    addCoords(4,1,0.2,M_PI/16.,0.0,block*M_PI/4.,in4a,out,incr,
              ChannelCoordinate::EM, **pos);
    addCoords(4,1,0.1,M_PI/16.,0.2,block*M_PI/4.,in4b,out,incr,
              ChannelCoordinate::EM, **pos);
    ++pos;

    // Map 7 : Type 4 Had positive eta
    addCoords(4,1,0.2,M_PI/16.,0.0,block*M_PI/4.,in4a,out,incr,
              ChannelCoordinate::HAD, **pos);
    addCoords(4,1,0.1,M_PI/16.,0.2,block*M_PI/4.,in4b,out,incr,
              ChannelCoordinate::HAD, **pos);
    ++pos;

    // Map 8 : Type 4 EM negative eta
    addCoords(4,1,0.1,M_PI/16.,0.0,block*M_PI/4.,in4a,out,incr,
              ChannelCoordinate::EM, **pos);
    addCoords(4,1,0.2,M_PI/16.,0.1,block*M_PI/4.,in4b,out,incr,
              ChannelCoordinate::EM, **pos);
    ++pos;

    // Map 9 : Type 4 Had negative eta
    addCoords(4,1,0.1,M_PI/16.,0.0,block*M_PI/4.,in4a,out,incr,
              ChannelCoordinate::HAD, **pos);
    addCoords(4,1,0.2,M_PI/16.,0.1,block*M_PI/4.,in4b,out,incr,
              ChannelCoordinate::HAD, **pos);
    ++pos;

    // Map 10 : Type 5 EM FCAL
    addCoords(4,4,0.425,M_PI/8.,0.0,block*M_PI/2.,in5,out,incr,
              ChannelCoordinate::EM, **pos);
    ++pos;

    // Map 11 : Type 6 Had FCAL2 and FCAL3
    addCoords(4,2,0.85,M_PI/8.,0.0,block*M_PI/2.,in6a,out,incr,
              ChannelCoordinate::FCAL2, **pos);
    addCoords(4,2,0.85,M_PI/8.,0.0,block*M_PI/2.,in6b,out,incr,
              ChannelCoordinate::FCAL3, **pos);
  }

  // Fill crate map

  std::vector<std::unique_ptr<CoordinateMap> >::const_iterator pos = m_coordMaps.begin();

  // Map 0 : all of crates 0,1
  //         crate 2 modules 1,2,5,6,9,10,13,14
  //         crate 3 modules 2,3,6,7,10,11,14,15
  addMods(0,0,4,4, 0.0,0.0,0.4,M_PI/2.,pos->get());
  addMods(1,0,4,4,-1.6,0.0,0.4,M_PI/2.,pos->get());
  addMods(2,1,4,2, 1.6,0.0,0.4,M_PI/2.,pos->get());
  addMods(3,2,4,2,-2.4,0.0,0.4,M_PI/2.,pos->get());
  ++pos;

  // Map 1 : crate 4 modules 1,2,5,6,9,10,13,14
  //         crate 5 modules 2,3,6,7,10,11,14,15
  //         all of crates 6,7
  addMods(4,1,4,2, 1.6,0.0,0.4,M_PI/2.,pos->get());
  addMods(5,2,4,2,-2.4,0.0,0.4,M_PI/2.,pos->get());
  addMods(6,0,4,4, 0.0,0.0,0.4,M_PI/2.,pos->get());
  addMods(7,0,4,4,-1.6,0.0,0.4,M_PI/2.,pos->get());
  ++pos;

  // Map 2 : crate 2 modules 3,7,11,15
  addMods(2,3,4,1, 2.4,0.0,0.5,M_PI/2.,pos->get());
  ++pos;

  // Map 3 : crate 4 modules 3,7,11,15
  addMods(4,3,4,1, 2.4,0.0,0.5,M_PI/2.,pos->get());
  ++pos;

  // Map 4 : crate 3 modules 1,5,9,13
  addMods(3,1,4,1,-2.9,0.0,0.5,M_PI/2.,pos->get());
  ++pos;

  // Map 5 : crate 5 modules 1,5,9,13
  addMods(5,1,4,1,-2.9,0.0,0.5,M_PI/2.,pos->get());
  ++pos;

  // Map 6 : crate 2 modules 4,12
  addMods(2, 4,1,1, 2.9,0.0,0.3,M_PI,pos->get());
  addMods(2,12,1,1, 2.9,M_PI,0.3,M_PI,pos->get());
  ++pos;

  // Map 7 : crate 4 modules 4,12
  addMods(4, 4,1,1, 2.9,0.0,0.3,M_PI,pos->get());
  addMods(4,12,1,1, 2.9,M_PI,0.3,M_PI,pos->get());
  ++pos;

  // Map 8 : crate 3 modules 4,12
  addMods(3, 4,1,1,-3.2,0.0,0.3,M_PI,pos->get());
  addMods(3,12,1,1,-3.2,M_PI,0.3,M_PI,pos->get());
  ++pos;

  // Map 9 : crate 5 modules 4,12
  addMods(5, 4,1,1,-3.2,0.0,0.3,M_PI,pos->get());
  addMods(5,12,1,1,-3.2,M_PI,0.3,M_PI,pos->get());
  ++pos;

  // Map 10 : crate 4 module 0, crate 5 module 0
  addMods(4,0,1,1, 3.2,0.0,1.7,2*M_PI,pos->get());
  addMods(5,0,1,1,-4.9,0.0,1.7,2*M_PI,pos->get());
  ++pos;

  // Map 11 : crate 4 module 8, crate 5 module 8
  addMods(4,8,1,1, 3.2,0.0,1.7,2*M_PI,pos->get());
  addMods(5,8,1,1,-4.9,0.0,1.7,2*M_PI,pos->get());
}

// Set up eta/phi map

void PpmMappingTool::setupInverseMap()
{
  const unsigned int invalidChanId = s_crates*s_modules*s_channels;

  for (int cr = 0; cr < s_crates; ++cr) {
    for (int mod = 0; mod < s_modules; ++mod) {
      for (int chan = 0; chan < s_channels; ++chan) {
        double tmpEta, tmpPhi;
	int tmpLayer;
	if (mapping(cr, mod, chan, tmpEta, tmpPhi, tmpLayer)) {
	  const unsigned int key    = etaPhiKey(tmpEta, tmpPhi);
	  const unsigned int chanId = (cr * s_modules + mod) * s_channels + chan;
	  EtaPhiMap::iterator iter = m_etaPhiMap.find(key);
	  if (iter == m_etaPhiMap.end()) {
	    ChannelIds ids(invalidChanId, invalidChanId);
	    if (tmpLayer == 0) ids.first  = chanId;
            else               ids.second = chanId;
	    m_etaPhiMap.insert(std::make_pair(key, ids));
          } else {
	    ChannelIds& ids(iter->second);
	    if (tmpLayer == 0) ids.first  = chanId;
            else               ids.second = chanId;
          }
        }
      }
    }
  }
}

// Add entries to a coordinate map

void PpmMappingTool::addCoords(const int nrows, const int ncols,
     const double etaGran, const double phiGran, const double etaOffset,
     const double phiOffset, const int* in, const int* out, const int incr,
     const ChannelCoordinate::CaloLayer layer, CoordinateMap& coordMap)
{
  for (int row = 0; row < nrows; ++row) {
    const double phi = (double(row) + 0.5) * phiGran + phiOffset;
    for (int col = 0; col < ncols; ++col) {
      const double eta = (double(col) + 0.5) * etaGran + etaOffset;
      const int channel = out[in[row*ncols+col]-1] + incr;
      assert (channel < s_channels);
      coordMap[channel] = ChannelCoordinate(layer, eta, phi, etaGran, phiGran);
    }
  }
}

// Add a block of similar modules to a crate

void PpmMappingTool::addMods(const int crate, const int modOffset,
     const int nrows, const int ncols, const double etaBase,
     const double phiBase, const double etaRange, const double phiRange,
     const CoordinateMap* coordMap)
{
  assert (crate < static_cast<int>(m_crateInfo.size()));
  ModuleMap& modMap = m_crateInfo[crate];
  for (int row = 0; row < nrows; ++row) {
    for (int col = 0; col < ncols; ++col) {
      const int module = row*4 + col + modOffset;
      const double etaOffset = etaRange * double(col) + etaBase;
      const double phiOffset = phiRange * double(row) + phiBase;
      Offsets off(etaOffset, phiOffset);
      modMap[module] = ModuleInfo (off, coordMap);
    }
  }
}

// Correction for Had FCAL eta which is adjusted to EM value in TriggerTower

double PpmMappingTool::etaSim(const ChannelCoordinate& coord) const
{
  double eta = coord.eta();
  const ChannelCoordinate::CaloLayer layer = coord.layer();
  if (layer == ChannelCoordinate::FCAL2 || layer == ChannelCoordinate::FCAL3) {
    const double etaCorrection = coord.etaGranularity()/4.;
    if (layer == ChannelCoordinate::FCAL2) eta -= etaCorrection;
    else eta += etaCorrection;
  }
  return eta;
}

// Simple eta/phi key

unsigned int PpmMappingTool::etaPhiKey(double eta, double phi) const
{
  const double etaOffsets[5] = { 0., 2.5, 3.1, 3.2, 4.9 };
  const double etaGrans[4]   = { 0.1, 0.2, 0.1, 0.425 };
  const double phiGrans[4]   = { M_PI/32., M_PI/16., M_PI/16., M_PI/8. };
  const int side = (eta < 0.) ? 0 : 1;
  int region = 0;
  int ieta   = 0;
  int iphi   = 0;
  const double absEta = fabs(eta);
  for (int i = 0; i < 4; ++i) {
    if (absEta < etaOffsets[i+1]) {
      region = i;
      ieta   = int((absEta - etaOffsets[i]) / etaGrans[i]);
      iphi   = int(phi / phiGrans[i]);
      break;
    }
  }
  unsigned int key = (side << 20) + (region << 16) + (ieta << 8) + iphi;
  return key;
}

} // end namespace
