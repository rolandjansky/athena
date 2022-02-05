/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTTUBECALIBCONTAINER_H
#define MUONCALIB_MDTTUBECALIBCONTAINER_H

#include <iostream>
#include <string>
#include <vector>

namespace MuonCalib {

    /** Holds single-tube calibration constants of one chamber */
    class MdtTubeCalibContainer {
    public:
        struct SingleTubeCalib {
            float t0;                //!< relative t0 in chamber (ns)
            float inversePropSpeed;  //!< inverse propagation speed (ns/mm)
            float adcCal;            //!< multiplicative correction factor for ADC measurement w.r.t. multilayer average (around 1.0)
            unsigned int
                statusCode;  //!< quality flag for the SingleTubeCalib constants: 0 all ok, 1 no hits found, 2 too few hits, 3 bad chi2
            SingleTubeCalib() : t0(0.0), inversePropSpeed(1. / 300.), adcCal(1.0), statusCode(1){};
        };

        /** nMl = number of multilayres, nLayers = number of layers in multilayer (3 or 4); nTubes = number of tubes in one layer */
        MdtTubeCalibContainer(std::string region, unsigned int nMl, unsigned int nLayers, unsigned int nTubes) :
            m_regionKey(region), m_nMl(nMl), m_nLayers(nLayers), m_nTubes(nTubes), m_data(nMl * nLayers * nTubes) {}

        /** return calibration constants of a single tube */
        const SingleTubeCalib* getCalib(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex) const {
            unsigned int idx = vectorIndex(nMl, layerIndex, tubeIndex);
            if (idx >= m_data.size()) return 0;
            return &m_data[idx];
        };
        SingleTubeCalib* getCalib(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex) {
            unsigned int idx = vectorIndex(nMl, layerIndex, tubeIndex);
            if (idx >= m_data.size()) return 0;
            return &m_data[idx];
        };

        /** set the calibration constants of a single tube */
        bool setCalib(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex, const SingleTubeCalib& val) {
            unsigned int idx = vectorIndex(nMl, layerIndex, tubeIndex);
            if (idx >= m_data.size()) return false;
            m_data[idx] = val;
            return true;
        }

        /** return container name and dimensions */
        std::string regionKey() const { return m_regionKey; }
        unsigned int size() const { return m_data.size(); }
        unsigned int numMultilayers() const { return m_nMl; };
        unsigned int numLayers() const { return m_nLayers; };
        unsigned int numTubes() const { return m_nTubes; };

    protected:
        /** calculate postion of tube in vector */
        unsigned int vectorIndex(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex) const {
            return nMl * m_nLayers * m_nTubes + m_nTubes * layerIndex + tubeIndex;
        }

        std::string m_regionKey;  //!< a string identifying the chamber (StationName_eta_phi)
        unsigned int m_nMl;       //!< number of multilayers in chamber
        unsigned int m_nLayers;   //!< number of layer
        unsigned int m_nTubes;    //!< number of tubes
        std::vector<SingleTubeCalib> m_data;
    };

}  // namespace MuonCalib

#endif
