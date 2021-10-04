/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/RtFullInfo.h"

#include <cstdio>

namespace MuonCalib {

    std::istream &operator>>(std::istream &is, RtFullInfo &data) { return data.read(is); }

    std::ostream &operator<<(std::ostream &os, const RtFullInfo &data) { return data.write(os); }

    void RtFullInfo::write_forDB(FILE *frt) const {
        fprintf(frt, "%d,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%f,%d\n", m_numSeg, m_meanAng, m_rmsAng, 0., 0., m_convEst, 0, 0, 0, 0, 0, m_qualEst,
                0);
        return;
    }

}  // namespace MuonCalib
