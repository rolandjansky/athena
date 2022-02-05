/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONFULLINFO_MDTTUBECALIBCONTAINER_H
#define MUONFULLINFO_MDTTUBECALIBCONTAINER_H

#include <vector>

#include "MdtCalibData/MdtTubeCalibContainer.h"

namespace MuonCalib {

    /** Holds single-tube full calibration information of one chamber */
    class MdtTubeFullInfoContainer : public MdtTubeCalibContainer {
    public:
        struct SingleTubeFullInfo {
            int statistics;  //!< number of hits
            float chi2Tdc;   //!< chisquared of the fit to the tdc spectrum
            float t0Err;     //!< error on t0 from the fit to the tdc spectrum
            float tMax;      //!< tmax from the fit to the tdc spectrum
            float tMaxErr;   //!< error on tmax from the fit to the tdc spectrum
            float noise;     //!< noise level from the fit to the tdc spectrum
            float noiseErr;  //!< error on noise from the fit to the tdc spectrum
            float slope;     //!< slope of the tdc spectrum near t0
            float slopeErr;  //!< error on the slope of the tdc spectrum near t0
            SingleTubeFullInfo() :
                statistics(0), chi2Tdc(0.), t0Err(0.), tMax(0.), tMaxErr(0.), noise(0.), noiseErr(0.), slope(0.), slopeErr(0.){};
        };

        /** nMl = number of multilayres, nLayers = number of layers in multilayer (3 or 4); nTubes = number of tubes in one layer */
        MdtTubeFullInfoContainer(std::string region, unsigned int nMl, unsigned int nLayers, unsigned int nTubes) :
            MdtTubeCalibContainer(region, nMl, nLayers, nTubes),
            m_info(nMl * nLayers * nTubes),
            m_name("MdtTubeFullInfoContainer"),
            m_implementation("implementation") {}

        /** nMl = number of multilayres, nLayers = number of layers in multilayer (3 or 4); nTubes = number of tubes in one layer */
        MdtTubeFullInfoContainer(const MdtTubeCalibContainer& calibContainer) :
            MdtTubeCalibContainer(calibContainer),
            m_info(calibContainer.size()),
            m_name("MdtTubeFullInfoContainer"),
            m_implementation("implementation") {}

        /** return calibration constants of a single tube */
        const SingleTubeFullInfo* getFullInfo(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex) const {
            unsigned int idx = vectorIndex(nMl, layerIndex, tubeIndex);
            if (idx >= m_info.size()) return 0;
            return &m_info[idx];
        };

        /** set the calibration constants of a single tube */
        bool setFullInfo(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex, const SingleTubeFullInfo& val) {
            unsigned int idx = vectorIndex(nMl, layerIndex, tubeIndex);
            if (idx >= m_info.size()) return false;
            m_info[idx] = val;
            return true;
        }

        /** set the name of the implementation used to fill this class */
        void setImplementation(std::string impl) { m_implementation = impl; }

        /** return the name of this class */
        std::string name() const { return m_name; }

        /** return the name of the implementation filling this class */
        std::string implementation() const { return m_implementation; }

    private:
        std::vector<SingleTubeFullInfo> m_info;
        std::string m_name;
        std::string m_implementation;
    };

}  // namespace MuonCalib

#endif
