/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODMDETDESCR_CALODMDESCRAREA_H
#define CALODMDETDESCR_CALODMDESCRAREA_H

/**
 * @class CaloDmDescrArea
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 27-January-2010
 * @brief This object is used to compute calibration energy deposits in the dead material
 *
 * It serves to centralize the naming conventions for dead material areas outside of
 * calorimeter instrumented regions. 
 */


class CaloDmDescrArea
{
  public:
    // these are keys for specific groups of dead material hits
    enum DmArea {
      DMA_ALL,              // 0,  total energy in the dead material (all calibration hits sum)
      DMA_EMB0,             // 1,  energy before PreSamplerB, |eta|<1.5
      DMA_EMB1,             // 2,  energy between PreSamplerB and EMB1
      DMA_EMB_HALVES,       // 3,  energy between two barrel halves
      DMA_EMB3_TILE0,       // 4,  energy between EMB3 and TILE0
      DMA_SCN,              // 5,  energy before scintillator
      DMA_EME_EXT,          // 6,  energy between (emec+hec) and TileExt0
      DMA_EME0,             // 7,  energy before PreSamplerE |eta|<1.5
      DMA_EME11,            // 8,  energy before EME1 |eta|<1.5
      DMA_EME12,            // 9,  energy before EME1 1.5<|eta|<1.8
      DMA_EME13,            // 10, energy before EME1 1.8<|eta|<2.5
      DMA_EME14,            // 11, energy before EME1 2.5<|eta|<3.2
      DMA_EME_INNER_OUTER,  // 12, energy between inner and outer wheel
      DMA_EME3_HEC0,        // 13, energy between EME3 and HEC0
      DMA_HECWHEEL,         // 14, energy between HEC1 and HEC2
      DMA_HEC_AZIMUTH,      // 15, energy in HEC azimuthal cracks
      DMA_HEC_FCAL,         // 16, energy between HEC and FCAL
      DMA_FCAL0_FCAL1,      // 17, energy between FCAL0 and FCAL1
      DMA_FCAL1_FCAL2,      // 18, energy between FCAL1 and FCAL2
      DMA_FCAL0,            // 19, energy before FCAL0
      DMA_LEAK,             // 20, leakage and energy flow (+TILE girder)
      DMA_DEFCALC,          // 21, default calclulator energy
      DMA_UNCLASS,          // 22, energy in hits out of this classification
      DMA_MAX               // 23.
    };

    CaloDmDescrArea();
    ~CaloDmDescrArea();

    static int getArea(const int &dmkey, const float &eta);
};


#endif
