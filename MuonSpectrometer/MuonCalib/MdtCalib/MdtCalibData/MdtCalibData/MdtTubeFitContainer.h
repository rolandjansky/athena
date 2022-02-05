/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONFIT_MDTTUBECALIBCONTAINER_H
#define MUONFIT_MDTTUBECALIBCONTAINER_H

#include <vector>

#include "MdtCalibData/MdtTubeCalibContainer.h"

namespace MuonCalib {

    /** Holds single-tube full calibration information of one chamber */
    class MdtTubeFitContainer : public MdtTubeCalibContainer {
    public:
        struct SingleTubeFit {
            int statistics;            //!< number of hits used for the fit
            int n_hits;                //!< number of hits in the tube (differs from statistics in case of grouped fit
            int n_hits_above_adc_cut;  //! number of hits above adc cut
            float chi2Tdc;             //!< chisquared of the fit to the tdc spectrum
            float chi2TdcEnd;          //! for MTT0 chi2 of trailing edge fit
            float par[8];
            float cov[36];
            float adc_par[4];
            float adc_err[4];
            float adc_chi2;
            std::string group_by;
            SingleTubeFit() :
                statistics(0), n_hits(0), n_hits_above_adc_cut(0), chi2Tdc(0.), chi2TdcEnd(0.), adc_chi2(0), group_by("UNKNOWN") {
                for (int i = 0; i < 8; i++) par[i] = 0;
                for (int i = 0; i < 36; i++) cov[i] = 0;
                for (int i = 0; i < 4; i++) adc_par[i] = 0;
                for (int i = 0; i < 4; i++) adc_err[i] = 0;
            };
        };

        /** nMl = number of multilayres, nLayers = number of layers in multilayer (3 or 4); nTubes = number of tubes in one layer */
        MdtTubeFitContainer(std::string region, unsigned int nMl, unsigned int nLayers, unsigned int nTubes) :
            MdtTubeCalibContainer(region, nMl, nLayers, nTubes),
            m_info(nMl * nLayers * nTubes),
            m_name("MdtTubeFitContainer"),
            m_implementation("implementation"),
            m_group_by("UNKNOWN") {}

        /** nMl = number of multilayres, nLayers = number of layers in multilayer (3 or 4); nTubes = number of tubes in one layer */
        MdtTubeFitContainer(const MdtTubeCalibContainer& calibContainer) :
            MdtTubeCalibContainer(calibContainer),
            m_info(calibContainer.size()),
            m_name("MdtTubeFitContainer"),
            m_implementation("implementation") {}

        /** return calibration constants of a single tube */
        const SingleTubeFit* getFit(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex) const {
            unsigned int idx = vectorIndex(nMl, layerIndex, tubeIndex);
            if (idx >= m_info.size()) return 0;
            return &m_info[idx];
        };
        SingleTubeFit* getFit(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex) {
            unsigned int idx = vectorIndex(nMl, layerIndex, tubeIndex);
            if (idx >= m_info.size()) return 0;
            return &m_info[idx];
        };

        /** set the calibration constants of a single tube */
        bool setFit(unsigned int nMl, unsigned int layerIndex, unsigned int tubeIndex, const SingleTubeFit& val) {
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

        inline void setGroupBy(const std::string& group_by) {
            for (std::vector<SingleTubeFit>::iterator it = m_info.begin(); it != m_info.end(); it++) { it->group_by = group_by; }
        }
        inline const std::string& GroupBy() const {
            if (!m_info.size()) return m_group_by;
            return m_info.begin()->group_by;
        }

    private:
        std::vector<SingleTubeFit> m_info;
        std::string m_name;
        std::string m_implementation;
        std::string m_group_by;
    };

}  // namespace MuonCalib

#endif
