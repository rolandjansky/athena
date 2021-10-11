/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelRadiationDamageFluenceMapData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel constant parameters in PixelRadiationDamageFluenceMapData.
 */

#ifndef PIXELRADIATIONDAMAGEFLUENCEMAPDATA_H
#define PIXELRADIATIONDAMAGEFLUENCEMAPDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

#include "AthenaKernel/CondCont.h"
#include "PixelConditionsData/PixelHistoConverter.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

class PixelRadiationDamageFluenceMapData {
  public:
    PixelRadiationDamageFluenceMapData();
    virtual ~PixelRadiationDamageFluenceMapData();

    // Map for radiation damage simulation
    void setFluenceLayer(std::vector<double> fluenceLayer);
    double getFluenceLayer(int layer) const;

    void setLorentzMap_e(std::vector<PixelHistoConverter> lorentzMap_e);
    void setLorentzMap_h(std::vector<PixelHistoConverter> lorentzMap_h);
    void setDistanceMap_e(std::vector<PixelHistoConverter> distanceMap_e);
    void setDistanceMap_h(std::vector<PixelHistoConverter> distanceMap_h);
    void setRamoPotentialMap(std::vector<PixelHistoConverter> ramoPotentialMap);

    const PixelHistoConverter& getLorentzMap_e(int layer) const;
    const PixelHistoConverter& getLorentzMap_h(int layer) const;
    const PixelHistoConverter& getDistanceMap_e(int layer) const;
    const PixelHistoConverter& getDistanceMap_h(int layer) const;
    const PixelHistoConverter& getRamoPotentialMap(int layer) const;

    // Map for radiation damage simulation for 3D sensor
    // The implementation for 3D radiation damage is different from the one for planar sensor.
    // Thus, define separately. In future, coherent treatment is preferrable.
    void setFluenceLayer3D(std::vector<double> fluenceLayer);
    double getFluenceLayer3D(int layer) const;

    void setRamoPotentialMap3D(std::vector<PixelHistoConverter> ramoPotentialMap3D);
    void setEFieldMap3D(std::vector<PixelHistoConverter> eFieldMap3D);
    void setXPositionMap3D_e(std::vector<PixelHistoConverter> xPositionMap3D_e);
    void setXPositionMap3D_h(std::vector<PixelHistoConverter> xPositionMap3D_h);
    void setYPositionMap3D_e(std::vector<PixelHistoConverter> yPositionMap3D_e);
    void setYPositionMap3D_h(std::vector<PixelHistoConverter> yPositionMap3D_h);
    void setTimeMap3D_e(std::vector<PixelHistoConverter> timeMap3D_e);
    void setTimeMap3D_h(std::vector<PixelHistoConverter> timeMap3D_h);
    void setAvgChargeMap3D_e(const PixelHistoConverter& avgChargeMap3D_e);
    void setAvgChargeMap3D_h(const PixelHistoConverter& avgChargeMap3D_h);

    const PixelHistoConverter& getRamoPotentialMap3D(int layer) const;
    const PixelHistoConverter& getEFieldMap3D(int layer) const;
    const PixelHistoConverter& getXPositionMap3D_e(int layer) const;
    const PixelHistoConverter& getXPositionMap3D_h(int layer) const;
    const PixelHistoConverter& getYPositionMap3D_e(int layer) const;
    const PixelHistoConverter& getYPositionMap3D_h(int layer) const;
    const PixelHistoConverter& getTimeMap3D_e(int layer) const;
    const PixelHistoConverter& getTimeMap3D_h(int layer) const;
    const PixelHistoConverter& getAvgChargeMap3D_e() const;
    const PixelHistoConverter& getAvgChargeMap3D_h() const;

    void clear();

  private:
    std::vector<double> m_fluenceLayer;
    std::vector<PixelHistoConverter> m_lorentzMap_e;
    std::vector<PixelHistoConverter> m_lorentzMap_h;
    std::vector<PixelHistoConverter> m_distanceMap_e;
    std::vector<PixelHistoConverter> m_distanceMap_h;
    std::vector<PixelHistoConverter> m_ramoPotentialMap;

    std::vector<double> m_fluenceLayer3D;
    std::vector<PixelHistoConverter> m_ramoPotentialMap3D;
    std::vector<PixelHistoConverter> m_eFieldMap3D;
    std::vector<PixelHistoConverter> m_xPositionMap3D_e;
    std::vector<PixelHistoConverter> m_xPositionMap3D_h;
    std::vector<PixelHistoConverter> m_yPositionMap3D_e;
    std::vector<PixelHistoConverter> m_yPositionMap3D_h;
    std::vector<PixelHistoConverter> m_timeMap3D_e;
    std::vector<PixelHistoConverter> m_timeMap3D_h;
    PixelHistoConverter m_avgChargeMap3D_e;
    PixelHistoConverter m_avgChargeMap3D_h;
};

CLASS_DEF( PixelRadiationDamageFluenceMapData , 71942873 , 1 )

CONDCONT_DEF( PixelRadiationDamageFluenceMapData, 71948393 );

#endif
