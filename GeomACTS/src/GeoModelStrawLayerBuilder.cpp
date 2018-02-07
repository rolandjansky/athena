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
#include "ACTS/Surfaces/StrawSurface.hpp"
#include "ACTS/Surfaces/DiscSurface.hpp"

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

  //size_t nVtx = 0;
  //std::ofstream objout;
  //objout.open("trt.obj");
  //objout << "mtllib master.mtl" << std::endl;

  std::vector<ProtoLayer> protoLayers;

  for(size_t iring=0; iring < nBarrelRings;iring++) {
    ACTS_VERBOSE("- Collecting elements for ring " << iring);
  

    // were calculating min/max radius while were at it.
    ProtoLayer pl;
    pl.minR = std::numeric_limits<double>::max();
    pl.maxR = std::numeric_limits<double>::lowest();
    pl.minZ = std::numeric_limits<double>::max();
    pl.maxZ = std::numeric_limits<double>::lowest();
    pl.minPhi = -M_PI;
    pl.maxPhi = M_PI;

    pl.envZ = {1, 1};
    pl.envR = {0, 0};

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

            auto trf = std::make_shared<const Transform3D>(brlElem->strawTransform(istraw));
            // @TODO: ID IS WRONG!!!!!
            auto elem = std::make_shared<const Acts::GeoModelDetectorElement>(trf, brlElem);

            m_cfg.elementStore->push_back(elem);

            auto straw = dynamic_cast<const Acts::StrawSurface*>(&elem->surface());
            auto strawBounds = dynamic_cast<const Acts::LineBounds*>(&straw->bounds());
            double radius = strawBounds->r();
            double length = strawBounds->halflengthZ();

            // calculate min/max R and Z
            Vector3D ctr = straw->center();
            pl.maxR = std::max(pl.maxR, ctr.perp() + radius);
            pl.minR = std::min(pl.minR, ctr.perp() - radius);
            pl.maxZ = std::max(pl.maxZ, ctr.z() + length);
            pl.minZ = std::min(pl.minZ, ctr.z() - length);

            layerSurfaces.push_back(straw);

            //PolyhedronRepresentation ph = cylinder->polyhedronRepresentation(6);
            //objout << "usemtl straw" << std::endl;
            //objout << ph.objString(nVtx);
            //nVtx += ph.vertices.size();
            

          }
        }
      }
    }

    ACTS_VERBOSE("  - Collected " << layerSurfaces.size() << " straws");

    if(iring > 0) {
      // match outer radius of previous ring
      ProtoLayer &prev = protoLayers.at(iring-1);
      pl.minR = prev.maxR + prev.envR.second + pl.envR.first;
    }

    std::shared_ptr<Layer> layer = m_cfg.layerCreator->cylinderLayer(layerSurfaces, 100, 1, pl);
    layers.push_back(layer);

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

    
  //objout.close();


  return layers;
  //return {};
}

const Acts::LayerVector
Acts::GeoModelStrawLayerBuilder::endcapLayers(int side)
{
  ACTS_VERBOSE("Building endcap Straw layers")

  const InDetDD::TRT_Numerology* trtNums = m_cfg.mng->getNumerology();
  size_t    nEndcapWheels  = trtNums->getNEndcapWheels();
  size_t    nEndcapPhiSectors = trtNums->getNEndcapPhi();

  ACTS_VERBOSE("- Numerology reports: - " << nEndcapWheels<< " endcap wheels");
  ACTS_VERBOSE("                      - " << nEndcapPhiSectors << " endcap phi sectors");

  Acts::LayerVector layers;

  //size_t nVtx = 0;
  //std::ofstream objout;
  //objout.open("trt.obj");
  //objout << "mtllib master.mtl" << std::endl;

  //std::vector<ProtoLayer> protoLayers;

  for(size_t iwheel=0;iwheel<nEndcapWheels;++iwheel) {
    ACTS_VERBOSE("- Collecting elements for wheel " << iwheel);


    size_t nEndcapLayers = trtNums->getNEndcapLayers(iwheel);
    ACTS_VERBOSE("  - Numerology reports: " << nEndcapLayers << " layers in wheel " << iwheel);
    for(size_t ilayer=0;ilayer<nEndcapLayers;++ilayer) {
      std::vector<const Surface*> wheelSurfaces;

            
      ProtoLayer pl;
      pl.minR = std::numeric_limits<double>::max();
      pl.maxR = std::numeric_limits<double>::lowest();
      pl.minZ = std::numeric_limits<double>::max();
      pl.maxZ = std::numeric_limits<double>::lowest();
      pl.minPhi = -M_PI;
      pl.maxPhi = M_PI;
      pl.envR = {0, 0};

      for (unsigned int iphisec=0; iphisec<nEndcapPhiSectors; ++iphisec) {
      
        size_t iposneg = side < 0 ? 0 : 1;
        const InDetDD::TRT_EndcapElement* ecElem = m_cfg.mng->getEndcapElement(iposneg, iwheel, ilayer, iphisec);
        int nStraws = ecElem->nStraws();

        for(int istraw=0;istraw<nStraws;istraw++) {

          auto trf = std::make_shared<const Transform3D>(ecElem->strawTransform(istraw));
          // @TODO: ID IS WRONG!!!!!
          auto elem = std::make_shared<const Acts::GeoModelDetectorElement>(trf, ecElem);

          m_cfg.elementStore->push_back(elem);

          auto straw = dynamic_cast<const Acts::StrawSurface*>(&elem->surface());
          auto strawBounds = dynamic_cast<const Acts::LineBounds*>(&straw->bounds());
          double radius = strawBounds->r();
          double length = strawBounds->halflengthZ();

          Vector3D ctr = straw->center();
          pl.maxZ = std::max(pl.maxZ, ctr.z() + radius);
          pl.minZ = std::min(pl.minZ, ctr.z() - radius);
          pl.maxR = std::max(pl.maxR, ctr.perp() + length);
          pl.minR = std::min(pl.minR, ctr.perp() - length);
          pl.envZ = {radius/2., radius/2.};

          wheelSurfaces.push_back(straw);

          //PolyhedronRepresentation ph = cylinder->polyhedronRepresentation(6);
          //objout << "usemtl straw" << std::endl;
          //objout << ph.objString(nVtx);
          //nVtx += ph.vertices.size();
        }
      }

      std::shared_ptr<Layer> layer = m_cfg.layerCreator->discLayer(wheelSurfaces, 1, 100, pl);
      layers.push_back(layer);
      ACTS_VERBOSE("  - Collected " << wheelSurfaces.size() << " straws");
    }
    

    //pl.dump(std::cout);
    //protoLayers.push_back(pl);
    
    //objout << "usemtl green" << std::endl;
    //auto minTrf = std::make_shared<const Transform3D>(Translation3D(Vector3D(0, 0, pl.minZ - pl.envZ.first)));
    //DiscSurface inner(minTrf, pl.minR - pl.envR.first, pl.maxR + pl.envR.second);
    //PolyhedronRepresentation ph = inner.polyhedronRepresentation(100);
    //objout << ph.objString(nVtx);
    //nVtx += ph.vertices.size();

    //objout << "usemtl red" << std::endl;
    //auto maxTrf = std::make_shared<const Transform3D>(Translation3D(Vector3D(0, 0, pl.maxZ + pl.envZ.second)));
    //DiscSurface outer(maxTrf, pl.minR - pl.envR.first, pl.maxR + pl.envR.second);
    //ph = outer.polyhedronRepresentation(100);
    //objout << ph.objString(nVtx);
    //nVtx += ph.vertices.size();
    

  }
  
  ACTS_VERBOSE(" - Built " << layers.size() << " straw endcap layers");

  //objout.close();
  return layers;
}

