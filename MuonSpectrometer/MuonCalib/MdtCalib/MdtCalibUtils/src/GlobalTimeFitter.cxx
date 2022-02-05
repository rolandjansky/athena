/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/GlobalTimeFitter.h"

#include <sstream>
#include <string>
namespace {
    constexpr double tdc_bc_conv = 25. / 32.;
    constexpr double max_drift_time_30mm = 750;
}  // namespace

namespace MuonCalib {

    double GlobalTimeFitter::GTFit(MuonCalibSegment* seg) {
        double timeShift = 0;
        float tdcmin = FLT_MAX;
        double timeoffset = 0.;
        int stepf, stepff[13], stepl, stepr, dtt, dttbuono(0);
        double chi2r, chi2l;
        double chi2min = FLT_MAX;
        double bestchi2 = FLT_MAX;

        stepff[0] = 512;
        stepff[1] = 256;
        stepff[2] = 128;
        stepff[3] = 64;
        stepff[4] = 32;
        stepff[5] = 16;
        stepff[6] = 8;
        stepff[7] = 4;
        stepff[8] = 2;
        stepff[9] = 1;
        stepff[10] = 1;
        stepff[11] = 1;
        stepff[12] = 1;

        for (const MuonCalibSegment::MdtHitPtr& mdt_hit : seg->mdtHOT()) {
            if (mdt_hit->tdcCount() < tdcmin) tdcmin = mdt_hit->tdcCount();
        }

        stepl = 512;
        stepr = 512;
        stepf = 512;
        for (int jj = 0; jj < 11; jj++) {
            bool outofrange{false};
            for (const MuonCalibSegment::MdtHitPtr& mdt_hit : seg->mdtHOT()) {
                double newtime = (mdt_hit->tdcCount() - tdcmin + stepl) * tdc_bc_conv;
                ///
                if (newtime < 0. || newtime > max_drift_time_30mm) { outofrange = true; }
                mdt_hit->setDriftTime(newtime);
                double r = m_rtRel->radius(newtime);
                double dr = getDefaultResolution(r);  //  get H8 resolution
                mdt_hit->setDriftRadius(r, dr);
            }
            if (!outofrange) {
                m_fitter->fit(*seg);
                chi2l = seg->chi2();
            } else {
                chi2l = FLT_MAX;
            }
            outofrange = false;
            for (const MuonCalibSegment::MdtHitPtr& mdt_hit : seg->mdtHOT()) {
                double newtime = (mdt_hit->tdcCount() - tdcmin + stepr) * tdc_bc_conv;

                if (newtime < 0. || newtime > max_drift_time_30mm) { outofrange = true; }
                mdt_hit->setDriftTime(newtime);
                double r = m_rtRel->radius(newtime);
                double dr = getDefaultResolution(r);  //  get H8 resolution
                mdt_hit->setDriftRadius(r, dr);
            }
            if (!outofrange) {
                m_fitter->fit(*seg);
                chi2r = seg->chi2();
            } else {
                chi2r = FLT_MAX;
            }
            chi2min = chi2l;
            stepf = stepl;
            dtt = stepl;
            if (chi2r < chi2l) {
                chi2min = chi2r;
                stepf = stepr;
                dtt = stepr;
            }
            if (chi2min < bestchi2) {
                bestchi2 = chi2min;
                dttbuono = dtt;
                stepl = stepf - stepff[jj];
                stepr = stepff[jj] + stepf;
                if (stepl < 0) stepl = 0;
                if (stepr < 0) stepr = 0;
            } else {
                stepl = stepf - stepff[jj];
                stepr = stepff[jj] + stepf;
                if (stepl < 0) stepl = 0;
                if (stepr < 0) stepr = 0;
            }
            timeoffset = dttbuono;
        }

        for (const MuonCalibSegment::MdtHitPtr& mdt_hit : seg->mdtHOT()) {
            double newtime = (mdt_hit->tdcCount() - tdcmin + timeoffset) * tdc_bc_conv;
            mdt_hit->setDriftTime(newtime);
            double r = m_rtRel->radius(newtime);
            double dr = getDefaultResolution(r);  //  get H8 resolution
            mdt_hit->setDriftRadius(r, dr);
        }
        m_fitter->fit(*seg);

        timeShift = -(timeoffset - tdcmin) * tdc_bc_conv;
        return timeShift;
    }

    double GlobalTimeFitter::GTFit2(MuonCalibSegment* seg) {
        double timeShift = 999.;
        for (const MuonCalibSegment::MdtHitPtr& mdt_hit : seg->mdtHOT()) {
            mdt_hit->setDriftTime(400.);
            double t = mdt_hit->driftTime();
            double r = m_rtRel->radius(t);
            double dr = mdt_hit->sigmaDriftRadius();
            mdt_hit->setDriftRadius(r, dr);

            timeShift = t;
        }

        return timeShift;
    }

    IRtRelation* GlobalTimeFitter::getDefaultRtRelation() {
        static const MuonCalib::CalibFunc::ParVec rtPars{

            -50.,  // t_Start
            8.,    // t_binSize

            0.2,  // r(-50)
            0.2,  // r(-50+8)
            0.2,  // ...
            0.25,    0.294403, 0.311703, 0.485873, 0.804235, 1.19624, 1.5562,  1.91466, 2.3147,  2.66996, 3.05436, 3.40371, 3.80574,
            4.13506, 4.45473,  4.77338,  5.05294,  5.33755,  5.58459, 5.8718,  6.09389, 6.37129, 6.56824, 6.73359, 6.96923, 7.16944,
            7.38872, 7.55627,  7.73885,  7.86403,  8.07223,  8.26818, 8.39531, 8.54069, 8.69377, 8.86487, 9.01724, 9.16516, 9.24791,
            9.45596, 9.56237,  9.69553,  9.80647,  9.9432,   10.039,  10.207,  10.2884, 10.3977, 10.6045, 10.6181, 10.7883, 10.9057,
            10.9908, 11.1615,  11.2359,  11.3409,  11.4451,  11.5412, 11.6896, 11.7487, 11.8853, 11.9571, 12.0381, 12.1739, 12.2601,
            12.3892, 12.4406,  12.5809,  12.6297,  12.7495,  12.8314, 12.9463, 13.0487, 13.1337, 13.2417, 13.2706, 13.3917, 13.5107,
            13.60,   13.69,    13.71,    13.8088,  13.8577,  14.0212, 14.0671, 14.1225, 14.2112, 14.3113, 14.3747, 14.3531, 14.4799,
            14.458,  14.516,   14.8312,  14.85,    14.86,    14.87

        };

        IRtRelation* rt = MuonCalib::MdtCalibrationFactory::createRtRelation("RtRelationLookUp", rtPars);

        return rt;
    }

}  // namespace MuonCalib
