/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTDEFS_H
#define HIEVENTUTILS_HIEVENTDEFS_H

#include <utility>
#include <cmath>
#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeSummaryUtils.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace HI{

  enum BinningScheme{ TOWER = 100, COMPACT = 562 };

  namespace TowerBins{
    constexpr unsigned int numEtaBins() {return 100;}
    constexpr float etaMin() {return -5.;}
    constexpr float etaMax() {return 5.;}
    constexpr unsigned int numPhiBins() {return 64;}
    constexpr unsigned int numLayers() {return 24;}
    //these fail b/c Root doesn't know about constexpr
    //constexpr float PhiMin() {return -TMath::Pi();}
    //constexpr float PhiMax() {return TMath::Pi();}
    //use cmath's M_PI
    constexpr float PhiMin() {return -M_PI;}
    constexpr float PhiMax() {return M_PI;}

    constexpr float getBinSizeEta() {return (etaMax()-etaMin())/((float)numEtaBins());}
    constexpr float getBinSizePhi() {return (PhiMax()-PhiMin())/((float)numPhiBins());}

    constexpr float getBinArea() {return getBinSizeEta()*getBinSizePhi();}
    constexpr float getTotalArea() {return (etaMax()-etaMin())*(PhiMax()-PhiMin());}
    constexpr  unsigned int getNumEtaPhiBins() {return numEtaBins()*numPhiBins();}

    inline float getBinLowEdgeEta(unsigned int eb) {return etaMin()+eb*getBinSizeEta();}
    inline float getBinUpEdgeEta(unsigned int eb) {return etaMin()+(eb+1)*getBinSizeEta();}
    inline float getBinCenterEta(unsigned int eb) {return etaMin()+(eb+0.5)*getBinSizeEta();}

    inline float getBinLowEdgePhi(unsigned int pb) {return PhiMin()+pb*getBinSizePhi();}
    inline float getBinUpEdgePhi(unsigned int pb) {return PhiMin()+(pb+1)*getBinSizePhi();}
    inline float getBinCenterPhi(unsigned int pb) {return PhiMin()+(pb+0.5)*getBinSizePhi();}

    inline unsigned int findBinEta(float eta) {return std::floor((eta-etaMin())/getBinSizeEta());}
    inline unsigned int findBinPhi(float phi)
    {
      int pb=std::floor((phi-PhiMin()) /getBinSizePhi());
      return pb % numPhiBins();
    }
    inline unsigned int findEtaPhiBin(float eta, float phi) { return numPhiBins()*findBinEta(eta)+findBinPhi(phi);}
  }

  namespace SubCalo
  {

    namespace Lists
    {
      constexpr std::initializer_list<int> EMBarrel{CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3};
      constexpr std::initializer_list<int> EMEC{CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3};
      constexpr std::initializer_list<int> HEC{CaloSampling::HEC0, CaloSampling::HEC1, CaloSampling::HEC2, CaloSampling::HEC3};
      constexpr std::initializer_list<int> TileBarrel{CaloSampling::TileBar0, CaloSampling::TileBar1, CaloSampling::TileBar2};
      constexpr std::initializer_list<int> TileGap{CaloSampling::TileGap1, CaloSampling::TileGap2, CaloSampling::TileGap3};
      constexpr std::initializer_list<int> TileExt{CaloSampling::TileExt0, CaloSampling::TileExt1, CaloSampling::TileExt2};
      constexpr std::initializer_list<int> FCal{CaloSampling::FCAL0, CaloSampling::FCAL1, CaloSampling::FCAL2};

      constexpr std::initializer_list<int> EMCal{CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3,
	  CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3};
      constexpr std::initializer_list<int> Tile{CaloSampling::TileBar0, CaloSampling::TileBar1, CaloSampling::TileBar2,
	  CaloSampling::TileGap1, CaloSampling::TileGap2, CaloSampling::TileGap3,
	  CaloSampling::TileExt0, CaloSampling::TileExt1, CaloSampling::TileExt2};
      constexpr std::initializer_list<int> HCal{CaloSampling::TileBar0, CaloSampling::TileBar1, CaloSampling::TileBar2,
	  CaloSampling::TileGap1, CaloSampling::TileGap2, CaloSampling::TileGap3,
	  CaloSampling::TileExt0, CaloSampling::TileExt1, CaloSampling::TileExt2,
	  CaloSampling::HEC0, CaloSampling::HEC1, CaloSampling::HEC2, CaloSampling::HEC3};
      constexpr std::initializer_list<int> PreSampler{CaloSampling::PreSamplerB,CaloSampling::PreSamplerE};

      constexpr std::initializer_list<int> ALL{	CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3,
	  CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3,
	  CaloSampling::HEC0, CaloSampling::HEC1, CaloSampling::HEC2, CaloSampling::HEC3,
	  CaloSampling::TileBar0, CaloSampling::TileBar1, CaloSampling::TileBar2,
	  CaloSampling::TileGap1, CaloSampling::TileGap2, CaloSampling::TileGap3,
	  CaloSampling::TileExt0, CaloSampling::TileExt1, CaloSampling::TileExt2,
	  CaloSampling::FCAL0, CaloSampling::FCAL1, CaloSampling::FCAL2};

  }
  }
  //selectors
  namespace Sel
  {
    constexpr ByLayers EMBarrel=SubCalo::Lists::EMBarrel;
    constexpr ByLayers EMEC=SubCalo::Lists::EMEC;
    constexpr ByLayers HEC=SubCalo::Lists::HEC;
    constexpr ByLayers TileBarrel=SubCalo::Lists::TileBarrel;
    constexpr ByLayers TileGap=SubCalo::Lists::TileGap;
    constexpr ByLayers TileExt=SubCalo::Lists::TileExt;
    constexpr ByLayers FCal=SubCalo::Lists::FCal;
    constexpr ByLayers EMCal=SubCalo::Lists::EMCal;
    constexpr ByLayers Tile=SubCalo::Lists::Tile;
    constexpr ByLayers HCal=SubCalo::Lists::HCal;
    constexpr ByLayers PreSampler=SubCalo::Lists::PreSampler;
    constexpr ByLayers ALL=SubCalo::Lists::ALL;

    constexpr ByLayersP EMBarrel_P=SubCalo::Lists::EMBarrel;
    constexpr ByLayersP EMEC_P=SubCalo::Lists::EMEC;
    constexpr ByLayersP HEC_P=SubCalo::Lists::HEC;
    constexpr ByLayersP TileBarrel_P=SubCalo::Lists::TileBarrel;
    constexpr ByLayersP TileGap_P=SubCalo::Lists::TileGap;
    constexpr ByLayersP TileExt_P=SubCalo::Lists::TileExt;
    constexpr ByLayersP FCal_P=SubCalo::Lists::FCal;
    constexpr ByLayersP EMCal_P=SubCalo::Lists::EMCal;
    constexpr ByLayersP Tile_P=SubCalo::Lists::Tile;
    constexpr ByLayersP HCal_P=SubCalo::Lists::HCal;
    constexpr ByLayersP PreSampler_P=SubCalo::Lists::PreSampler;
    constexpr ByLayersP ALL_P=SubCalo::Lists::ALL;

    constexpr ByLayersN EMBarrel_N=SubCalo::Lists::EMBarrel;
    constexpr ByLayersN EMEC_N=SubCalo::Lists::EMEC;
    constexpr ByLayersN HEC_N=SubCalo::Lists::HEC;
    constexpr ByLayersN TileBarrel_N=SubCalo::Lists::TileBarrel;
    constexpr ByLayersN TileGap_N=SubCalo::Lists::TileGap;
    constexpr ByLayersN TileExt_N=SubCalo::Lists::TileExt;
    constexpr ByLayersN FCal_N=SubCalo::Lists::FCal;
    constexpr ByLayersN EMCal_N=SubCalo::Lists::EMCal;
    constexpr ByLayersN Tile_N=SubCalo::Lists::Tile;
    constexpr ByLayersN HCal_N=SubCalo::Lists::HCal;
    constexpr ByLayersN PreSampler_N=SubCalo::Lists::PreSampler;
    constexpr ByLayersN ALL_N=SubCalo::Lists::ALL;

  }


}

#endif
