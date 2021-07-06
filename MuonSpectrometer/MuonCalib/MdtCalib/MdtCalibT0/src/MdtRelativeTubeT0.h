/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MdtRelativeTubeT0_h
#define MuonCalib_MdtRelativeTubeT0_h

#include <map>

namespace MuonCalib {

    class MdtCalibHitBase;
    class MuonFixedId;

    class MdtRelativeTubeT0 {
    public:
        enum TubeGroup { CHAMBER, MULTILAYER, LAYER, MEZZ_CARD, UNKNOWN };

        inline MdtRelativeTubeT0() {}
        inline ~MdtRelativeTubeT0()=default;

        void AddHit(const MdtCalibHitBase *hit);

        double GetRelativeOffset(const MuonFixedId &id, TubeGroup grp) ;

    private:
        std::map<MuonFixedId, double> m_tube_t0;

        std::map<TubeGroup, std::map<MuonFixedId, double> > m_relative_offset;

        inline void calculate_relative_t0s(const TubeGroup &grp) ;
    };

}  // namespace MuonCalib

#endif
