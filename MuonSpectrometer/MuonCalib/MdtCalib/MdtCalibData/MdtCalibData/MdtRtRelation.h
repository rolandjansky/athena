/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTRTRELATION_H
#define MUONCALIB_MDTRTRELATION_H

#include <iostream>

namespace MuonCalib {

    class IRtRelation;
    class IRtResolution;
    class TrRelation;

    /** class which holds calibration constants per rt-region */
    class MdtRtRelation {
    public:
        MdtRtRelation(IRtRelation* rt, IRtResolution* reso, float t0);
        ~MdtRtRelation();
        inline const IRtRelation* rt() const { return m_rt; }          //!< rt relation
        inline const IRtResolution* rtRes() const { return m_rtRes; }  //!< resolution
        inline const TrRelation* tr() const { return m_tr; }           //!< t(r) relationship
        inline const float& t0Global() const { return m_t0; }          //!< global t0
    private:
        IRtRelation* m_rt;
        IRtResolution* m_rtRes;
        TrRelation* m_tr;
        float m_t0;

        /** private copy constructor, assignment operator w/o concrete implemenation */
        MdtRtRelation(const MdtRtRelation&);
        MdtRtRelation& operator=(const MdtRtRelation& right);
    };

}  // namespace MuonCalib

#endif
