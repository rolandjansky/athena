/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscSplitClusterFitter.h"

#include <iomanip>
#include <sstream>

#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

using Muon::CscClusterStatus;
using Muon::CscPrepData;
using Muon::CscStripPrepData;
using MuonGM::CscReadoutElement;

typedef ICscClusterFitter::DataNames DataNames;
typedef ICscClusterFitter::Result    Result;
typedef std::vector<Result>          Results;

enum CscStation { UNKNOWN_STATION, CSS, CSL };
enum CscPlane { CSS_ETA, CSL_ETA, CSS_PHI, CSL_PHI, UNKNOWN_PLANE };

//******************************************************************************

CscSplitClusterFitter::CscSplitClusterFitter(std::string type, std::string aname, const IInterface* parent)
    : AthAlgTool(type, aname, parent)
{
    declareInterface<ICscClusterFitter>(this);
    declareProperty("min_dist", m_min_dist = 2);         // Minimum distance between peaks and valley
    declareProperty("max_qratio", m_max_qratio = 0.15);  // Maximum charge ratio between peak strip and valley strip
}

//**********************************************************************

StatusCode
CscSplitClusterFitter::initialize()
{
    ATH_MSG_DEBUG("Initalizing " << name());

    ATH_CHECK(m_idHelperSvc.retrieve());

    // Retrieve the default cluster fitting tool.
    ATH_CHECK_RECOVERABLE(m_pfitter_def.retrieve());

    ATH_MSG_DEBUG("CscClusterization " << name() << ": retrieved " << m_pfitter_def);

    // Retrieve the precision cluster fitting tool.
    ATH_CHECK_RECOVERABLE(m_pfitter_prec.retrieve());

    ATH_MSG_DEBUG("CscClusterization " << name() << ": retrieved " << m_pfitter_prec);

    ATH_MSG_DEBUG("Properties for " << name() << ":");
    ATH_MSG_DEBUG("  Default cluster fitter is " << m_pfitter_def.typeAndName());
    ATH_MSG_DEBUG("  Precision cluster fitter is " << m_pfitter_prec.typeAndName());
    ATH_MSG_DEBUG("    Min dist peak to valley: " << m_min_dist);
    ATH_MSG_DEBUG("    Max qratio qval / qpeak: " << m_max_qratio);

    return StatusCode::SUCCESS;
}
//**********************************************************************

Results
CscSplitClusterFitter::fit(const StripFitList& sfits) const
{
    Results results;
    // Check input has at least three strips.
    unsigned int nstrip = sfits.size();
    // Use the first strip to extract the layer parameters.
    const CscStripPrepData* pstrip   = sfits[0].strip;
    Identifier              idStrip0 = pstrip->identify();
    bool                    measphi  = m_idHelperSvc->cscIdHelper().CscIdHelper::measuresPhi(idStrip0);

    // Display input strips.
    ATH_MSG_DEBUG("CscStrip fittter input has " << nstrip << " strips");

    for (unsigned int istrip = 0; istrip < nstrip; ++istrip) {
        Identifier id = sfits[istrip].strip->identify();
        ATH_MSG_VERBOSE("  " << istrip << " " << m_idHelperSvc->cscIdHelper().strip(id) << " " << sfits[istrip].charge);
    }
    // Find the peak strip and valley strip
    // Loop over strips
    std::vector<float> istrip_peaks;
    std::vector<float> istrip_vals;


    // Start peak all the time...
    for (unsigned int istrip = 1; istrip < nstrip - 1; ++istrip) {
        StripFit sfit  = sfits[istrip];
        float    qthis = sfit.charge;
        float    qlast = sfits[istrip - 1].charge;
        float    qnext = sfits[istrip + 1].charge;
        // Peak at first
        if (istrip == 1 && qlast > qthis) istrip_peaks.push_back(0);
        // Peak at last
        if (istrip + 2 == nstrip && qthis < qnext) istrip_peaks.push_back(nstrip - 1);
        // Peak if the adjacent strips have less charge.
        bool ispeak = qthis > qlast && qthis > qnext;
        bool isval  = qthis < qlast && qthis < qnext;
        // Record if peak.

        if (ispeak) istrip_peaks.push_back(istrip);
        if (isval) istrip_vals.push_back(istrip);
        if (ispeak || isval) continue;

        // It's only for istrip==1 and 0th and 1st charges are same....
        if (istrip == 1 && qlast == qthis) {
            float        nstripsameCharge = 2.;
            unsigned int theStrip         = 0;
            for (unsigned int mstrip = istrip + 1; mstrip < nstrip - 1; ++mstrip) {
                theStrip = mstrip;
                if (qthis == sfits[mstrip].charge)
                    nstripsameCharge += 1.;
                else
                    break;
            }
            ispeak       = (qthis > sfits[theStrip].charge);
            isval        = (qthis < sfits[theStrip].charge);
            float offset = 0.5 * (nstripsameCharge - 1);
            if (ispeak) istrip_peaks.push_back(offset);
            // We want to start peak all the time...
            //      if ( isval ) istrip_vals.push_back(offset);
            istrip += int(nstripsameCharge) - 2;
            continue;
        }


        // Special case: next strip has the same charge.
        // Require the previous strip has less charge and the next following
        // strip be absent or have less charge.
        if (qthis == qnext) {
            float        nstripsameCharge = 2.;
            unsigned int theStrip         = 0;
            bool         sameCharge       = 1;
            for (unsigned int mstrip = istrip + 2; mstrip < nstrip - 1; ++mstrip) {
                theStrip = mstrip;
                if (qthis == sfits[mstrip].charge)
                    nstripsameCharge += 1.;
                else {
                    sameCharge = 0;
                    break;
                }
            }
            if (sameCharge) {
                ispeak = (qthis > qlast);
                isval  = (qthis < qlast);
            } else {
                ispeak = (qthis > qlast) && (qthis > sfits[theStrip].charge);
                isval  = (qthis < qlast) && (qthis < sfits[theStrip].charge);
            }
            float offset = 0.5 * (nstripsameCharge - 1);
            if (ispeak) istrip_peaks.push_back(istrip + offset);
            if (isval) istrip_vals.push_back(istrip + offset);
            istrip += int(nstripsameCharge) - 1;
            continue;
        }
    }

    ATH_MSG_DEBUG("  #Peak is " << istrip_peaks.size());

    // Decide in which valley strip we split cluster
    // Starting from Peak all the time!!!
    // Regular case : #peaks - #vals =1
    // If #peaks == $vals, avoid it at the end of vals..

    std::vector<unsigned int> splitOnValley;
    for (unsigned int ival = 0; ival < istrip_vals.size() && ival + 1 < istrip_peaks.size(); ++ival) {

        // Set initial strip position.
        float istrip_peak0 = istrip_peaks[ival];
        float istrip_peak1 = istrip_peaks[ival + 1];
        float istrip_val   = istrip_vals[ival];

        float dist_ptov = istrip_val - istrip_peak0;
        float dist_vtop = istrip_peak1 - istrip_val;

        ATH_MSG_DEBUG(" [ " << istrip_peak0 << ", " << istrip_val << ", " << istrip_peak1 << "] "
                            << "dist p2v:v2p " << dist_ptov << " : " << dist_vtop);


        if (dist_ptov < 0 || dist_vtop < 0) {
            ATH_MSG_WARNING("  Peak-to-Val dist is " << dist_ptov << "  Val-to-Peak dist is " << dist_vtop
                                                     << "  Shouldnot be negative value :" << istrip_peak0 << "  "
                                                     << istrip_val << " " << istrip_peak1);
        }

        float qlpeak = sfits[int(istrip_peak0)].charge;
        float qval   = sfits[int(istrip_val)].charge;
        float qrpeak = sfits[int(istrip_peak1)].charge;

        ATH_MSG_DEBUG("qlpk:qval:qrpk " << qlpeak << " " << qval << " " << qrpeak << "  " << qval / qlpeak << "  "
                                        << qval / qrpeak << "    " << m_max_qratio);

        if (dist_ptov < m_min_dist || dist_vtop < m_min_dist || qval / qlpeak > m_max_qratio
            || qval / qrpeak > m_max_qratio)
            splitOnValley.push_back(0);
        else
            splitOnValley.push_back(1);
    }

    unsigned int cnt = 0;
    for (unsigned int ii = 0; ii < splitOnValley.size(); ii++)
        if (splitOnValley[ii] == 1) cnt++;

    if (cnt == 0) {
        ATH_MSG_DEBUG("No split cluster ");
        ICscClusterFitter::Result res;
        res.fitStatus     = 6;
        res.clusterStatus = Muon::CscStatusWide;
        results.push_back(res);
        return results;
    }
    ATH_MSG_DEBUG("Cluster is split ");

    // For the last cluster and strip
    unsigned int nvals = splitOnValley.size();
    if (istrip_vals[nvals - 1] != nstrip - 1) {
        istrip_vals.push_back(nstrip - 1);
        splitOnValley.push_back(1);
        nvals = splitOnValley.size();
    }

    //  if (splitOnValley.size() != istrip_peaks.size())
    //    ATH_MSG_ERROR << "  splitOnValley.size() should be same as istrip_peaks.size()" );

    // Rearrange strips to submit in fitter
    StripFitList sfits_split;
    unsigned int firstStripID     = 0;
    unsigned int thisfirstStripID = 0;
    for (unsigned int isplit = 0; isplit < nvals; ++isplit) {

        if (splitOnValley[isplit]) {
            sfits_split.clear();

            for (unsigned int ist = firstStripID; ist <= istrip_vals[isplit]; ++ist) {
                ATH_MSG_DEBUG(ist << "  " << firstStripID << "  " << istrip_vals << " lsplit " << splitOnValley);

                sfits_split.push_back(sfits[ist]);
            }
            //      unsigned int split_nstrip = istrip_vals[isplit]-firstStripID;
            //      bool ledge = (isplit==0 && strip0 <=0) ? 1 : 0;
            //      bool redge = (isplit==nvals-1 && strip0+nstrip>maxstrip) ? 1 : 0;

            thisfirstStripID = firstStripID;
            firstStripID     = int(istrip_vals[isplit]);

            int                                    fitresult = 99;
            std::vector<ICscClusterFitter::Result> local_results;
            ICscClusterFitter::Result              res;
            // Precision fit.
            if (!measphi && m_pfitter_prec) {
                ATH_MSG_VERBOSE("      In CscSplit performing precision fit with " << m_pfitter_prec);

                local_results = m_pfitter_prec->fit(sfits_split);
                res           = local_results[0];
                fitresult     = res.fitStatus;
                if (fitresult) {
                    ATH_MSG_VERBOSE("        Precision fit failed: return=" << fitresult);
                } else {
                    ATH_MSG_VERBOSE("        Precision fit succeeded");
                }
            }

            int              prec_fitresult = fitresult;
            CscClusterStatus oldclustatus   = res.clusterStatus;
            // Default fit.
            if (fitresult) {  // including measphi case
                ATH_MSG_VERBOSE("      Performing default fit with " << m_pfitter_def);
                local_results = m_pfitter_def->fit(sfits_split);
                res           = local_results[0];
                fitresult     = res.fitStatus;
                if (fitresult) {
                    ATH_MSG_VERBOSE("        Default fit failed: return=" << fitresult);
                } else {
                    ATH_MSG_VERBOSE("        Default fit succeeded");
                }
                // Keep the status from the first fit if it is defined.
                if (oldclustatus != Muon::CscStatusUndefined) res.clusterStatus = oldclustatus;
            }

            if (prec_fitresult) {  // precision fitter is failed...
                res.fitStatus = 20 + prec_fitresult;

                if (oldclustatus == Muon::CscStatusSimple)
                    res.clusterStatus = Muon::CscStatusSplitSimple;
                else if (oldclustatus == Muon::CscStatusEdge)
                    res.clusterStatus = Muon::CscStatusSplitEdge;
                else if (oldclustatus == Muon::CscStatusMultiPeak)
                    res.clusterStatus = Muon::CscStatusSplitMultiPeak;
                else if (oldclustatus == Muon::CscStatusNarrow)
                    res.clusterStatus = Muon::CscStatusSplitNarrow;
                else if (oldclustatus == Muon::CscStatusWide)
                    res.clusterStatus = Muon::CscStatusSplitWide;
                else if (oldclustatus == Muon::CscStatusSkewed)
                    res.clusterStatus = Muon::CscStatusSplitSkewed;
                else if (oldclustatus == Muon::CscStatusQratInconsistent)
                    res.clusterStatus = Muon::CscStatusSplitQratInconsistent;
                else if (oldclustatus == Muon::CscStatusStripFitFailed)
                    res.clusterStatus = Muon::CscStatusSplitStripFitFailed;
                else if (oldclustatus == Muon::CscStatusSaturated)
                    res.clusterStatus = Muon::CscStatusSplitSaturated;

            } else {  // precision fit is successful
                res.fitStatus     = 20;
                res.clusterStatus = Muon::CscStatusSplitUnspoiled;
            }
            // strip in Result class is filled by Qrat or SimpleClusterFitter which is only relevant to the cluster
            // passed by CscSplitCluster. This should be corrected here.
            // fstrip and lstrip should be defined in CscSplitClusterFitter but not for the other fitter.
            res.strip  = res.strip + thisfirstStripID;
            res.fstrip = res.fstrip + thisfirstStripID;
            res.lstrip = res.lstrip + thisfirstStripID;

            ATH_MSG_DEBUG("    res.fitStatus " << res.fitStatus << "  fitresult " << fitresult);

            results.push_back(res);
        }
    }  // for


    return results;
}

//**********************************************************************

Results
CscSplitClusterFitter::fit(const StripFitList& sfits, double) const
{
    return fit(sfits);
}

//**********************************************************************
double
CscSplitClusterFitter::getCorrectedError(const CscPrepData* /*pclu*/, double /*slope*/) const
{
    return 0;
}
