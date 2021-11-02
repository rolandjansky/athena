/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtCalibRt_MultilayerRtDifference_h
#define MdtCalibRt_MultilayerRtDifference_h

class TF1;
class TDirectory;

#include <cstddef>

#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "vector"

namespace MuonCalib {

    class MuonCalibSegment;
    class IRtRelation;
    class MdtCalibHitBase;
    class MultilayerRtDifference_Histograms;

    class MultilayerRtDifference {
    public:
        MultilayerRtDifference(int min_hits, TDirectory *control_histogram_dir = NULL);
        MultilayerRtDifference(const MultilayerRtDifference &MLRTD);
        virtual ~MultilayerRtDifference();
        MultilayerRtDifference &operator=(const MultilayerRtDifference &MLRTD);

        void Fill(const MdtCalibHitBase &hit, const IRtRelation &rt_relation);

        bool DoFit(IRtRelation *rt_relation, const IMdtCalibration::MuonSegVec &seg);

        inline const TF1 *GetFunction() const { return m_polfun; }

    private:
        TF1 *m_polfun;
        MultilayerRtDifference_Histograms *m_histograms;
        int m_min_number_of_hits;
    };

}  // namespace MuonCalib

#endif
