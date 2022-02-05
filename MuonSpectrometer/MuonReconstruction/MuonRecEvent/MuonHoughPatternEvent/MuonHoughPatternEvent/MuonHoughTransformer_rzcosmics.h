/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_RZCOSMICS_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_RZCOSMICS_H

#include "MuonHoughPatternEvent/MuonHoughTransformer.h"

class MuonHoughTransformer_rzcosmics : public MuonHoughTransformer {
public:
    MuonHoughTransformer_rzcosmics(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle, double threshold_histo,
                                   int number_of_sectors = 1);
    ~MuonHoughTransformer_rzcosmics();

    virtual void fillHit(MuonHoughHit* hit, double weight = 1.);
    virtual int fillHisto(double rz0, double theta, double weight = 1., int sector = 0);

    virtual float weightHoughTransform(double r0) const;

    virtual int sector(MuonHoughHit* hit) const;  // 0..15 same as atlas sector 1..16 // returns 0 if number_of_sectors == 0
protected:
    virtual MuonHoughPattern* hookAssociateHitsToMaximum(const MuonHoughHitContainer* event, std::pair<double, double> coordsmaximum,
                                                         double residu_mm, double residu_grad, int sector = 0, bool which_segment = 0,
                                                         int printlevel = 999) const;

private:
    /** recalculate trackparameters of pattern */
    static void updateParameters(MuonHoughPattern*);

    /** weight in transform, dotprod is the phi angle between the normal and the phisector */
    float weightHoughTransform(double r0, double sintheta, double sinphi, double dotprod) const;

    /** arrays that store values of transform */
    double* m_phisec;
    double* m_sinphisec;
    double* m_cosphisec;
    double* m_theta_in_grad;
    double* m_sintheta;
    double* m_costheta;

    MuonHoughTransformer_rzcosmics& operator=(const MuonHoughTransformer_rzcosmics& right);
    MuonHoughTransformer_rzcosmics(const MuonHoughTransformer_rzcosmics&);
};

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_RZCOSMICS_H
