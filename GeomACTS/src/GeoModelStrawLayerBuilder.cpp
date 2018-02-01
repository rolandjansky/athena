// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <fstream>

#include "InDetReadoutGeometry/TRT_Numerology.h"

#include "GeomACTS/GeoModelStrawLayerBuilder.hpp"

#include "ACTS/Tools/ILayerBuilder.hpp"
#include "ACTS/Surfaces/CylinderSurface.hpp"

const Acts::LayerVector
Acts::GeoModelStrawLayerBuilder::negativeLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelStrawLayerBuilder*>(this);
  return mutableThis->endcapLayers(-1);
}

const Acts::LayerVector
Acts::GeoModelStrawLayerBuilder::centralLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelStrawLayerBuilder*>(this);
  return mutableThis->centralLayers();
}

const Acts::LayerVector
Acts::GeoModelStrawLayerBuilder::positiveLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelStrawLayerBuilder*>(this);
  return mutableThis->endcapLayers(1);

}


const Acts::LayerVector
Acts::GeoModelStrawLayerBuilder::centralLayers()
{
  ACTS_VERBOSE("Building central Straw layers")

  const InDetDD::TRT_Numerology* trtNums = m_cfg.mng->getNumerology();
  size_t    nBarrelRings  = trtNums->getNBarrelRings();
  size_t    nBarrelPhiSectors = trtNums->getNBarrelPhi();

  ACTS_VERBOSE("- Numerology reports: - " << nBarrelRings << " barrel rings");
  ACTS_VERBOSE("                      - " << nBarrelPhiSectors << " barrel phi sectors");

  Acts::LayerVector layers;

  size_t nVtx = 0;
  std::ofstream objout;
  objout.open("trt.obj");
  objout << "mtllib master.mtl" << std::endl;

  std::vector<ProtoLayer> protoLayers;

  for(size_t iring=0; iring < nBarrelRings;iring++) {
    ACTS_VERBOSE("- Collecting elements for ring " << iring);
  
    objout << "usemtl straw" << std::endl;

    // were calculating min/max radius while were at it.
    ProtoLayer pl;
    pl.minR = std::numeric_limits<double>::max();
    pl.maxR = std::numeric_limits<double>::lowest();
    pl.minZ = std::numeric_limits<double>::max();
    pl.maxZ = std::numeric_limits<double>::lowest();
    pl.minPhi = -M_PI;
    pl.maxPhi = M_PI;

    pl.envZ = {1, 1};
    pl.envR = {0.5, 0.5};

    // RING in TRT speak is translated to Layer in ACTS speak
    std::vector<const Surface*> layerSurfaces;

    size_t nBarrelLayers = trtNums->getNBarrelLayers(iring);
    ACTS_VERBOSE("  - Numerology reports: " << nBarrelLayers << " layers in ring " << iring);
    for (size_t ilayer=0;ilayer < nBarrelLayers;ilayer++) {
      for (size_t iphisec=0;iphisec < nBarrelPhiSectors;iphisec++) {
        // what is this?
        for (size_t iposneg=0;iposneg<2;iposneg++) {

          const InDetDD::TRT_BarrelElement* brlElem 
            = m_cfg.mng->getBarrelElement(iposneg, iring, iphisec, ilayer);

          int nStraws = brlElem->nStraws();

          for(int istraw=0;istraw<nStraws;istraw++) {

            Transform3D trf = brlElem->strawTransform(istraw);
            auto elem = std::make_shared<const Acts::GeoModelDetectorElement>();

            // calculate min/max R and Z
            Vector3D ctr = cylinder->center();
            pl.maxR = std::max(pl.maxR, ctr.perp() + innerTubeRadius);
            pl.minR = std::min(pl.minR, ctr.perp() - innerTubeRadius);
            pl.maxZ = std::max(pl.maxZ, ctr.z() + length);
            pl.minZ = std::min(pl.minZ, ctr.z() - length);

            layerSurfaces.push_back(cylinder);

            PolyhedronRepresentation ph = cylinder->polyhedronRepresentation(6);
            objout << ph.objString(nVtx);
            nVtx += ph.vertices.size();
            

          }
        }
      }
    }

    ACTS_VERBOSE("  - Collected " << layerSurfaces.size() << " straws");


    // figure out outer dimensions of this layer
    //ProtoLayer pl(layerSurfaces);

    if(iring > 0) {
      // match outer radius of previous ring
      ProtoLayer &prev = protoLayers.at(iring-1);
      pl.minR = prev.maxR + prev.envR.second + pl.envR.first;
    }

    //pl.minR = minR;
    //pl.maxR = maxR;
    pl.dump(std::cout);

    //std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    //for(const auto &srf: layerSurfaces) {
      //std::cout << "SRF: " << srf << std::endl;
      //Vector3D ctr = srf->center();
      //std::cout << "OK!" << std::endl;
    //}
    std::shared_ptr<Layer> layer = m_cfg.layerCreator->cylinderLayer(layerSurfaces, 100, 1, pl);

    protoLayers.push_back(pl);

    //objout << "usemtl green" << std::endl;
    //auto idTrf = std::make_shared<const Transform3D>(Transform3D::Identity());
    //CylinderSurface inner(idTrf, pl.minR-pl.envR.first, pl.maxZ+pl.envZ.second);
    //PolyhedronRepresentation ph = inner.polyhedronRepresentation(100);
    //objout << ph.objString(nVtx);
    //nVtx += ph.vertices.size();

    //objout << "usemtl red" << std::endl;
    //CylinderSurface outer(idTrf, pl.maxR+pl.envR.second, pl.maxZ+pl.envZ.second);
    //ph = outer.polyhedronRepresentation(100);
    //objout << ph.objString(nVtx);
    //nVtx += ph.vertices.size();


    
  }

    
  objout.close();


  return {};
}

const Acts::LayerVector
Acts::GeoModelStrawLayerBuilder::endcapLayers(int side)
{
  std::cout << __PRETTY_FUNCTION__ << " go" << std::endl;
  return {};
}

