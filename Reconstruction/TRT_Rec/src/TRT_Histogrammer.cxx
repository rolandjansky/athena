/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header:
//  Author: Jim Loken
//

#include <algorithm>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
// #include "GaudiKernel/MsgStream.h"
#include "TRT_Rec/TRT_Histogrammer.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TRT_Histogrammer::TRT_Histogrammer(const std::string&	type,
				   const std::string&	name, 
				   const IInterface* 	parent)
    :	AthAlgTool			(type, name, parent),
	m_muonTrThreshold     		(120.*Gaudi::Units::GeV),
	m_driftCircleOnTrackMaker	(
	    "InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"),
	m_intersector			("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_strawOnTrackMaker		(
	    "InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool"),
    	m_dCorr				(3.0*Gaudi::Units::nm),	// step width for curvature scan
	m_maxBin			(140),    	// see comment below
        m_maxHit			(100),
	m_minPeak			(6),		// min entries in histo peak
        m_n1Corr			(5),
        m_n2Corr			(14),
        m_wbin0				(0.0010),	// basic bin width in deltaPhi (1 mrad)
        m_wbinPt			(0.3),          // low pt bin width increase term
	m_fit				(0),
	m_hMaxPt			(0),
	m_iCorrMax			(0),
	m_inversePt			(0.),
	m_numLayers			(0),
	m_numTrHits			(0),
	m_pBelowMuonTrThreshold		(true),
	m_roadData			(nullptr),
	m_scaleFactor			(0.)
{
    declareInterface<ITRT_Histogrammer>(this);
    declareProperty("muonTrThreshold",		m_muonTrThreshold);
    declareProperty("DriftCircleOnTrackMaker",	m_driftCircleOnTrackMaker);
    declareProperty("Intersector",		m_intersector);
    declareProperty("StrawOnTrackMaker",	m_strawOnTrackMaker);
    
    // NOTE: maxBin and maxHit in makeHistograms MUST have the same values as
    // m_maxBin and m_maxHit. Dynamic array allocation not allowed in C++.
    
    // set max residual to 500um on first peak selection pass, 400 um on second
    m_maxResidual.push_back(0.50*Gaudi::Units::mm);
    m_maxResidual.push_back(0.40*Gaudi::Units::mm);
}

TRT_Histogrammer::~TRT_Histogrammer()
{
    delete m_fit;
}
    
//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TRT_Histogrammer::initialize()
{
    // print where you are
    ATH_MSG_INFO( "TRT_Histogrammer::initialize() - package version " << PACKAGE_VERSION );

    // get the Tools
    if (m_driftCircleOnTrackMaker.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_driftCircleOnTrackMaker );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO(  "Retrieved tool " << m_driftCircleOnTrackMaker );
    }
    
    if (m_strawOnTrackMaker.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_strawOnTrackMaker );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO(  "Retrieved tool " << m_strawOnTrackMaker );
    }
    
    return StatusCode::SUCCESS;
}
 
StatusCode
TRT_Histogrammer::finalize()
{      
    return StatusCode::SUCCESS;
}
    
bool
TRT_Histogrammer::bremLike (double rMax, double rPhiMax) const
{
    if (! m_fit) return false;

    // include approximate fit error
    double errorTRT = 0.3*Gaudi::Units::mm;
    double deflectionSignificance = 0.;
    if (rPhiMax > 0.)
    {
	deflectionSignificance = m_fit->rPhi(rMax)/sqrt(rPhiMax*rPhiMax + errorTRT*errorTRT);
    }
    else
    {
	deflectionSignificance = -m_fit->rPhi(rMax)/sqrt(rPhiMax*rPhiMax + errorTRT*errorTRT);
    }
    
    // bremLike curvature increase (require lower significance when track is tr-rich)
    if (trRich())
    {
	if (deflectionSignificance < 0.5)	return false;
    }
    else
    {
	if (deflectionSignificance < 1.0)	return false;
    }

//     std::cout << " brem-like: pt " << 1./(m_inversePt*Gaudi::Units::GeV) << "   nPeak " << m_hMaxPt
// 	      << "   r,rLast " << rMax << "  "  << (**(m_hitPt[m_hMaxPt-1])).rhoRoad()
// 	      << "   rPhi,rPhiMax " << m_fit->rPhi(rMax)
// 	      << "  " << rPhiMax << std::endl;

    return true;
}

bool
TRT_Histogrammer::confirmSegment (const Trk::TrackParameters&		trkParameters,
				  const std::vector<TRT_RoadData*>&	roadData)
{
    ATH_MSG_DEBUG(  "confirmSegment " );

    // reset counters and delete any previous fit result
    m_numLayers = 0;
    m_numTrHits = 0;
    delete m_fit;
    m_fit	= 0;
    
    // check sufficient hits available
    m_roadData = &roadData;
    if (m_roadData->size() < m_minPeak) return false;

    // set momentum related parameters
    m_pBelowMuonTrThreshold = true;
    if (trkParameters.momentum().mag() > m_muonTrThreshold) m_pBelowMuonTrThreshold = false;
    m_inversePt = 1./trkParameters.momentum().perp();
    
    // do the PR work using histogram method
    ATH_MSG_VERBOSE( "makeHistograms " );
    makeHistograms();
    if (m_hMaxPt < m_minPeak) return false;
    
    // now, clean up the peak data with a fit to r*phi vs r.
    // After first fit, make a narrow selection to get better fit for the final, wider, selection.
    ATH_MSG_VERBOSE( "fitPeak " );
    fitPeak();
    if (m_hMaxPt < m_minPeak) return false;
 
    // finally, make associations to closest drift hit in each straw layer - if compatible,
    // otherwise retain a straw hit provided the straw is intersected.
    ATH_MSG_VERBOSE( "flagAssociations " );
    // double	electronFactor	= 1.0;
    // if (electronMode == 3) electronFactor = 2.5;
    flagAssociations();
    return true;
}

int
TRT_Histogrammer::numLayers (void) const
{
    return m_numLayers;
}

int
TRT_Histogrammer::numTrHits (void) const
{
    return m_numTrHits;
}

double
TRT_Histogrammer::residual (const TRT_RoadData& roadData) const
{
    return (roadData.rPhiRoad() - m_fit->rPhi(roadData.rhoRoad()))*roadData.projectionFactor();
}

bool
TRT_Histogrammer::trRich (void) const
{
    // particles below TrThreshold having at least 4 tr hits: >90% electrons, <10% non-electrons
    return (m_pBelowMuonTrThreshold && m_numTrHits > 3);
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
TRT_Histogrammer::fitPeak (void)
{
    // Now, clean up the peak data with a fit allowing for an angle and curvature change
    // wrt to the exit parameters from the last silicon measurement
    // After first fit, make a narrow selection to get better fit for final, wider selection.
    m_fit	= new TRT_Fit;
    
//     std::cout << "m_hMaxPt = " << m_hMaxPt << std::endl;
    for (std::vector<double>::const_iterator maxResid = m_maxResidual.begin();
	 maxResid != m_maxResidual.end();
	 ++maxResid)
    {
	// perform 2-parameter fit 
	// high weights for large drift distance as
	//   1) left/right drift sign better resolved in histogram bin
	//   2) background peaks at low drift times
	m_fit->init();
        for (unsigned i = 0; i != m_hMaxPt; ++i)
	{
            TRT_RoadData& roadData = **m_hitPt[i];
	    double	weight	= roadData.rhoRoad()*roadData.driftWeight();
            m_fit->sum(roadData.rhoRoad(), roadData.phiRoad(), weight);
        }
	m_fit->solve();
	
        // Check all roadData to see if close to peak
//  	std::cout << " fit params: " << 10000.*m_fit->aFit()
//  		  << "  " << 10000.*m_fit->bFit() 
//  		  << "   drphi (um) at dr = 100,500:    "
//  		  << static_cast<int>(1000.*m_fit->rPhi(100.))
//  		  << "  " << static_cast<int>(1000.*m_fit->rPhi(500.)) << std::endl;
	
        m_hMaxPt = 0;
        for (std::vector<TRT_RoadData*>::const_iterator dit = m_roadData->begin();
	     dit != m_roadData->end();
	     ++dit)
	{
            TRT_RoadData& roadData = **dit;
	    if (roadData.strawStatus() == TRT_RoadData::pendingStraw) continue;
	    double dRPhi = residual(roadData);
// 	    std::cout << m_hMaxPt << "  r  " << roadData.rhoRoad()
// 		      << "  rPhi " << static_cast<int>(1000.*roadData.rPhiRoad())
// 		      << "  drift " << static_cast<int>(1000.*roadData.driftDistance())
// 		      << "  dRPhi " << static_cast<int>(1000.*dRPhi);

	    if (fabs(dRPhi) <= m_scaleFactor*(*maxResid))
	    {
		m_hitPt[m_hMaxPt++] = dit;
// 		std::cout << " stored " << m_hMaxPt;
	    }
// 	    std::cout << std::endl;
	}
//     std::cout << "Fit, hMax " << "  " << m_hMaxPt << std::endl;
    }
}

void
TRT_Histogrammer::flagAssociations (void)
{
    // Now, set strawStatus for peak data
    // make list of selected drift hits
    std::list<TRT_RoadData*> driftHits;
    for (unsigned i = 0; i != m_hMaxPt; ++i)
    {
        TRT_RoadData* roadData = *m_hitPt[i];
// 	if (electronMode == 3)
// 	{
// 	    roadData.strawStatus(TRT_RoadData::confirmedStraw);
// 	}
// 	else if (roadData.strawStatus() == TRT_RoadData::pendingDrift)
	if (roadData->strawStatus() == TRT_RoadData::pendingDrift)
	{
	    roadData->strawStatus(TRT_RoadData::confirmedDrift);
	    driftHits.push_back(roadData);
	}
	else if (roadData->strawStatus() == TRT_RoadData::pendingWire)
	{
	    roadData->strawStatus(TRT_RoadData::confirmedWire);
	    driftHits.push_back(roadData);
	}
    }

    // remove any outliers by local smoothing
    outlierRemoval(driftHits);
    
    // find all intersected straws
    for (std::vector<TRT_RoadData*>::const_iterator dit = m_roadData->begin();
	 dit != m_roadData->end();
	 ++dit)
    {
	TRT_RoadData& roadData = **dit;
	if (roadData.strawStatus() != TRT_RoadData::pendingStraw) continue;
	
	// look for any drift or wire hits
	std::vector<TRT_RoadData*>::const_iterator next = dit;
	int selected = 0;
	if (fabs(residual(roadData)) < 2.0*Gaudi::Units::mm)
	{
	    // straw selected: increment counters
	    ++m_numLayers;
	    if (roadData.trThreshold()) ++m_numTrHits;

	    // keep straw if no drift hit selected
	    while (++next != m_roadData->end()
		   && (**next).strawStatus() != TRT_RoadData::pendingStraw)
		if ((**next).strawStatus() > TRT_RoadData::notConfirmed) ++selected;
	    
	    if (selected == 0)
	    {
		roadData.strawStatus(TRT_RoadData::confirmedStraw);
	    }
	}

	// also use as straw hit if both drift signs selected
	// TODO:: shouldn't this be drift wire ???
	if (selected > 1)
	{
	    roadData.strawStatus(TRT_RoadData::confirmedStraw);
	    next = dit;
	    while (++next != m_roadData->end()
		   && (**next).strawStatus() != TRT_RoadData::pendingStraw)
		(**next).strawStatus(TRT_RoadData::rejectedDoublet);
	}
    }
}

void
TRT_Histogrammer::makeHistograms (void)
{
    // Do the PR work using histogram method
    std::vector<unsigned> hist(m_maxBin), histD(m_maxBin-1);
    const int maxBin = 140;	// must == m_maxBin
    const int maxHit = 100;	// must == m_maxHit
    std::vector<TRT_RoadData*>::const_iterator hit[maxHit][maxBin];

    // increase number and width of bins at low pt,
    // plus an additional width factor during electron search
    double	ptInv		= fabs(1000.*m_inversePt);
    int		nCorr		= m_n1Corr + 2*static_cast<int>(0.5*m_n2Corr*ptInv+0.5);
    int		nCorr2		= nCorr/2 + 1;
    int		nCorr1		= nCorr2 - nCorr;
    m_scaleFactor = 1.;
    
//     double	electronFactor	= 1.0;
//     if (electronMode == 3) electronFactor = 2.5;
//     //    m_scaleFactor = electronFactor*(1.0 + m_wbinPt*ptInv);
//     m_scaleFactor = electronFactor;
    //    std::cout << "ptInv, nCorr = " << ptInv << "  " << nCorr << std::endl;

    // Repeat series of loops if found peak is at edge
    int		iCorrDir	= 0;
    bool	edge		= true;
    while (edge)
    {
        m_hMaxPt = 0;
        int hMaxPt1 = 0;
        // Fill a histogram with corrected phi, 
	// finding the peak (of 2 bins added)
        // Repeat for a series of different corrections, 
	// looking for the largest peak
        for (int iCorr = nCorr1; iCorr != nCorr2; ++iCorr)
	{
            // Clear histogram
            for (int i = 0; i != m_maxBin; i++) hist[i] = 0;

            // Fill histogram
	    double wbin	= m_scaleFactor*m_wbin0;
            for (std::vector<TRT_RoadData*>::const_iterator dit = m_roadData->begin();
		 dit != m_roadData->end();
		 ++dit)
	    {
		TRT_RoadData& roadData = **dit;
		if (roadData.strawStatus() == TRT_RoadData::pendingStraw) continue;
// 		if (electronMode != 3 || roadData.trThreshold() > 0)
// 		{
		    double phiCorr	= iCorr*m_dCorr*roadData.rhoRoad();
		    double phi		= roadData.phiRoad() - phiCorr;
		    int bin		= static_cast<int>(phi/wbin + 0.5) + m_maxBin/2;
		    if (bin >= 0 && bin < m_maxBin)
		    {
			if (hist[bin] != m_maxHit) hit[hist[bin]++][bin] = dit;
		    }
// 		}
	    }

            // Create doublet histogram
            for (int i = 0; i != m_maxBin-1; ++i) histD[i] = hist[i] + hist[i+1];

            // Find the peak in the doublet histogram
	    std::vector<unsigned>::iterator it = std::max_element(histD.begin(), histD.end());
            unsigned hMax = *it;
            int nMax = it - histD.begin();
            int hMax1 = std::max(hist[nMax],hist[nMax+1]);

            // Save data for largest peak
            if (hMax > m_hMaxPt || (hMax == m_hMaxPt && hMax1 > hMaxPt1)) {
                m_hMaxPt = hMax;
                hMaxPt1 = hMax1;
                m_iCorrMax = iCorr;
                int k = 0;
                for (int i = nMax; i != nMax+2; ++i)
		{
                    for (unsigned j = 0; j != hist[i]; ++j)
		    {
                        m_hitPt[k++] = hit[j][i];
                    }
                }
            }

//  	    std::cout << "iCorr, nMax, hMax " << iCorr << "  " 
//  		      << nMax << "  " << hMax << std::endl;
//  	    std::cout << "hist elements" << std::endl;
//  	    for (int j = 0; j < m_maxBin; j+=20)
//  	    {
//  		for (int i = 0; i != 20; i++) std::cout << hist[i+j] << "  ";
//  		std::cout << std::endl;
//  	    }
//  	    std::cout << std::endl;
        }
	
        // Check if peak at edge, but avoid oscillation
	if (m_hMaxPt < m_minPeak) return;
        if (m_iCorrMax == nCorr1 && iCorrDir != 1)
	{
            iCorrDir = -1;
            nCorr2 = nCorr1 + 1;
            nCorr1 = nCorr2 - nCorr;
        } 
        else if (m_iCorrMax == nCorr2-1 && iCorrDir != -1)
	{
            iCorrDir = 1;
            nCorr1 = nCorr2 - 1;
            nCorr2 = nCorr1 + nCorr;
        }
        else
	{
            edge = false;
        }
    }
//     std::cout << std::endl;
//     std::cout << "Best iCorr, hMax " << m_iCorrMax << "  " << m_hMaxPt << std::endl;
}

void
TRT_Histogrammer::outlierRemoval (std::list<TRT_RoadData*>& driftHits)
{
    // allow around 10% outliers
    // note they are not permanently removed - they will come back as straw hits
    int maxOutliers	= driftHits.size()/10 + 1;
    for (int remove = 0; remove != maxOutliers; ++remove)
    {	
	// find "worst" local residual
	TRT_RoadData*      outlier = 0;
	TRT_RoadData*      first   = 0;
	TRT_RoadData*      second  = 0;
	TRT_RoadData*      third   = 0;
	double             worst   = 0.;
	for (std::list<TRT_RoadData*>::const_iterator i = driftHits.begin();
	     i != driftHits.end();
	     ++i)
	{	
	    if (! second)
	    {
		if (! first)
		{
		    first   = *i;
		    outlier = first;
		    worst   = fabs(residual(*first));
		    continue;
		}
		second = *i;
		continue;
	    }
	    third = *i;
	    double resid = fabs(residual(*second) - 0.5*(residual(*first)+residual(*third)));
	    resid /= sqrt(1.5);
	
	    //std::cout << " resid " << resid << " " << residual(*second) << std::endl;
	
	    if (resid > worst)
	    {
		outlier     = second;
		worst       = resid;
	    }
	    first   = second;
	    second  = third;
	}
	
	if (! second)	continue;
	double resid = fabs(residual(*second));
	//std::cout << " resid " << resid << " " << residual(*second) << std::endl;
	if (resid > worst)
	{
	    outlier = second;
	    worst   = resid;
	}

	if (! outlier) return;
	
	// reject worst (need to improve criterion)
	// TODO:: replace this with configured cut * error from RIO_OnTrack
	if (worst < 3.0 * 0.14) return;

	// flag status and remove from list
	//std::cout << " outlier " << worst << "  " << residual(*outlier) << std::endl;
	outlier->strawStatus(TRT_RoadData::notConfirmed);
	std::list<TRT_RoadData*>::iterator iter = find(driftHits.begin(), driftHits.end(), outlier);
	if (iter != driftHits.end()) driftHits.erase(iter);
    }
}

