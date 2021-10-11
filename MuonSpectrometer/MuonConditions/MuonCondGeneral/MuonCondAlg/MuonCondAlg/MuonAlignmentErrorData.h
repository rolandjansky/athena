// Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   MuonAlignmentErrorData is condition data which is derived and recorded by MuonAlignmentErrorDbAlg
 */

#ifndef MUONALIGNMENTERRORDATA_H
#define MUONALIGNMENTERRORDATA_H

#include <boost/regex.hpp>
#include <vector>

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
    class RIO_OnTrack;
}

// Struct for per-Station Deviations Information

struct deviationStr {
    deviationStr();
    ~deviationStr();
    double traslation;
    double rotation;
    boost::regex stationName;
    boost::regex multilayer;
};

class MuonAlignmentErrorData {
    friend class MuonAlignmentErrorDbAlg;

public:
    MuonAlignmentErrorData();
    virtual ~MuonAlignmentErrorData();
    void setVec(const std::vector<deviationStr>& vec);
    void getVec(std::vector<deviationStr>& vec) const;
    void clearVec();

private:
    int m_test;
    std::vector<deviationStr> m_vec;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(MuonAlignmentErrorData, 115867308, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<MuonAlignmentErrorData>, 265772564, 0)

#endif
