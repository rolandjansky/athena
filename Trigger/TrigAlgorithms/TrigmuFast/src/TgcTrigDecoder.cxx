/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/TgcTrigDecoder.h"
#include "TrigmuFast/MuServiceInterface.h"

#include "TGCgeometry/TGCgeometrySvc.h"

#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"

#include "MuonCablingTools/ShowRequest.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

const double STRIP_WIDTH    = 26.3; // mm
const double STRIP_WEIGHT   = 12.0 / (STRIP_WIDTH * STRIP_WIDTH);
const double WIRE_WIDTH     = 18.0; // mm
const double WIRE_WEIGHT    = 12.0 / (WIRE_WIDTH * WIRE_WIDTH);

using namespace std;

const string TgcTrigDecoder::s_name = "Level-1 Tgc Decoding";

TgcTrigDecoder::TgcTrigDecoder(MuMessageInterface& msg,
                               __osstream* display,
                               const TgcRdo* tgc_rdo,
                               const LVL1::RecMuonRoI* roi,
                               TriggerData& out,
                               RawVec& tgc_raw_data,
                               LUTDigitVec& tgc_lut_digits,
                               const TGCgeometrySvc* pTgcLUTSvc,
                               MuServiceInterface* svc) :
        TgcTrigStd(msg, display, tgc_rdo, roi, out, tgc_raw_data, tgc_lut_digits, pTgcLUTSvc, svc)
{}

TgcTrigDecoder::~TgcTrigDecoder(void)
{}

bool TgcTrigDecoder::run(MuonFeatureDetails* det)
{
    // initialization of the trigger data
    memset(&m_tgc_data, 0, sizeof(m_tgc_data));
    m_tgc_data.isEndcap   = true;
    m_tgc_data.type       = (m_pMuonRoI->getThresholdNumber() < 4) ? 1 : 2;
    m_tgc_data.threshold  = m_pMuonRoI->getThresholdNumber();
    m_tgc_data.MDT_Sector_for_trigger = 99;
    m_tgc_data.MDT_Sector_for_overlap = 99;

    m_tgc_data.beta = 1.;
    if(det!=0) det->setSpeed_over_c(m_tgc_data.beta);

    // initialization of the TGC digits vectors
    m_tgc_lut_digits.clear();
    m_tgc_raw_data.clear();
    m_tgcStripMidPoints.clear();
    m_tgcWireMidPoints.clear();
    m_tgcStripInnPoints.clear();
    m_tgcWireInnPoints.clear();
    
    if (m_tgc_rdo==0) return false;

    // Create digits from TGC RDO.
    if (!decodeTgcRDO())
    {
        std::string err = "Decoding of Tgc digits failed";
        m_msg->lock();
        m_msg->displayError(err.c_str());
        m_msg->unlock();
        return false;
    }

    std::string msgstr = "Using TGC lookup table";
    m_msg->lock();
    m_msg->displayDebug(msgstr.c_str());
    m_msg->unlock();
    // Split digits to Strip/Wire points.
    if (!prepareTgcPoints())
    {
        std::string err = "Preparation of Tgc points failed";
        m_msg->lock();
        m_msg->displayError(err.c_str());
        m_msg->unlock();
	fillTgcPointsIntoESD(det);
        return false;
    }

    // Create a fit object.
    TgcFit fit(m_msg, m_pMuonRoI, m_svc->tgcFitParameters().CHI2_TEST);
    // Fit lines to TGC middle station.
    TgcFit::Status status = fit.runTgcMiddle(m_tgcStripMidPoints, m_tgcWireMidPoints, m_tgc_data);
    if (status == TgcFit::FIT_NONE)
    {
        std::string algoName = m_svc->name();
	if( (algoName.find("_Muon")     != std::string::npos && 
	     algoName.find("_MuonEcut") == std::string::npos) ||
	    algoName.find("_900GeV") != std::string::npos ) {
	   std::string err = "Fit to TGC middle station points failed";
	   char buf[128];
	   sprintf(buf,"RoI Eta/Phi/Threshold=%6.2f/%6.2f/%i",m_pMuonRoI->eta(),m_pMuonRoI->phi(),m_pMuonRoI->getThresholdNumber());
	   m_msg->lock();
	   m_msg->displayWarning(err.c_str());
	   m_msg->displayWarning(buf);
	   m_msg->unlock();
	}
	fillTgcPointsIntoESD(det);
        return false;
	
    } else if (status == TgcFit::FIT_POINT)
    {
        std::string dbg = "Fit to TGC middle station returns only a point";
        m_msg->lock();
        m_msg->displayDebug(dbg.c_str());
        m_msg->unlock();

        // set trigger side with the RoI position for the cosmic run
        int side = (m_tgc_data.tgcMid1[3]<0.)? 0 : 1;
        float phiMiddle = m_tgc_data.tgcMid1[1];
        int endcap_tr,endcap_ov;
        m_svc->mdtgeo()->getEsects(1, side, phiMiddle, endcap_tr, endcap_ov);
        m_tgc_data.PhysicsSector = endcap_tr/2 + 1;
        m_tgc_data.LargeSmall    = endcap_tr%2;
        m_tgc_data.side = side;
        m_tgc_data.MDT_Sector_for_trigger = endcap_tr;
        m_tgc_data.MDT_Sector_for_overlap = endcap_ov;
	
	if(det!=0) det->setTgcMid1(m_tgc_data.tgcMid1[0],m_tgc_data.tgcMid1[1],
	                           m_tgc_data.tgcMid1[2],m_tgc_data.tgcMid1[3]);
	
	if(!m_svc->isCosmic()) {
	    fillTgcPointsIntoESD(det);
	    return false;
	}
    }



    // set the PhysicsSector and type according to the projection on the MDT
    // middle station
    int side = (m_tgc_data.tgcMid1[3]<0.)? 0 : 1;
    float phiMiddle = m_tgc_data.tgcMid1[1];
    // 2012.11.26 kn: to avoid failure in mdtgeo->getEsects()
    //  which results in wrong PhysicsSector -> nan in Z pos etc later.
    if (phiMiddle  > 3.14 ) phiMiddle =  3.14;
    if (phiMiddle < -3.14 ) phiMiddle = -3.14;
    int endcap_tr,endcap_ov;
    m_svc->mdtgeo()->getEsects(1, side, phiMiddle, endcap_tr, endcap_ov);
    m_tgc_data.PhysicsSector = endcap_tr/2 + 1;
    m_tgc_data.LargeSmall    = endcap_tr%2;
    m_tgc_data.side = side;
    m_tgc_data.MDT_Sector_for_trigger = endcap_tr;
    m_tgc_data.MDT_Sector_for_overlap = endcap_ov;
    
    
    if(det!=0) {
        det->setTgcMid1(m_tgc_data.tgcMid1[0],m_tgc_data.tgcMid1[1],
	                m_tgc_data.tgcMid1[2],m_tgc_data.tgcMid1[3]);
        det->setTgcMid2(m_tgc_data.tgcMid2[0],m_tgc_data.tgcMid2[1],
	                m_tgc_data.tgcMid2[2],m_tgc_data.tgcMid2[3]);
        det->setTgcMidF(m_tgc_data.tgcMidRhoChi2,m_tgc_data.tgcMidRhoNin,
	                m_tgc_data.tgcMidPhiChi2,m_tgc_data.tgcMidPhiNin);
    }
    
    status = fit.runTgcInner(m_tgcStripInnPoints, m_tgcWireInnPoints, m_tgc_data);
    if (status == TgcFit::FIT_NONE)
    {
        std::string dbg = "Fit to TGC inner station points failed";
        m_msg->lock();
        m_msg->displayDebug(dbg.c_str());
        m_msg->unlock();
        //return false;
    } else {
       if(det!=0) {
           det->setTgcInn(m_tgc_data.tgcInn[0],m_tgc_data.tgcInn[1],
	                  m_tgc_data.tgcInn[2],m_tgc_data.tgcInn[3]);
           det->setTgcInnF(m_tgc_data.tgcInnRhoStd,m_tgc_data.tgcInnRhoNin,
	                   m_tgc_data.tgcInnPhiStd,m_tgc_data.tgcInnPhiNin);
       }
    }

    postprocessTgcPoints();
    
    fillTgcPointsIntoESD(det);
    return true;
}

bool TgcTrigDecoder::decodeTgcRDO()
{
    // Loop over RawData, converter RawData to digit
    // retrieve/create digit collection, and insert digit into collection
    for (TgcRdo::const_iterator itD = m_tgc_rdo->begin(); itD != m_tgc_rdo->end(); ++itD)
    {
        TgcRawData* pRawData = *itD;

        // check Hit or Coincidence
        if (pRawData->isCoincidence())
        {
            tgcLUTDigit lutDigit;
            lutDigit.eta           = 0.0;
            lutDigit.phi           = 0.0;
            lutDigit.r             = 0.0;
            lutDigit.z             = 0.0;
            lutDigit.width         = 0.0;
            lutDigit.sta           = 0;
            lutDigit.isStrip       = 0.0;
            lutDigit.rawData       = pRawData;
            lutDigit.inRoad        = false;
            lutDigit.isCoincidence = true;
            lutDigit.inSegment     = false;
            m_tgc_lut_digits.push_back(lutDigit);
            continue;
        }

        // select current Hits
        //if (pRawData->bcTag() != 0 && pRawData->bcTag() != 2 ) continue;

        tgcRawData rawData;
        rawData.bcTag  = pRawData->bcTag();
        rawData.sswId  = pRawData->sswId();
        rawData.slbId  = pRawData->slbId();
        rawData.bitpos = pRawData->bitpos();
        for (RawVec::const_iterator itR = m_tgc_raw_data.begin(); itR != m_tgc_raw_data.end(); ++itR)
        {
            if(rawData.sswId == itR->sswId && rawData.slbId == itR->slbId && rawData.bitpos == itR->bitpos)
            {
                rawData.bitpos = 999;
                break;
            }
        }
        if (rawData.bitpos == 999)
            continue;
        m_tgc_raw_data.push_back(rawData);

        const TGCgeometrySvc::Entry& e =
            m_pTgcLUTSvc->getEntry(pRawData->subDetectorId(),
                                   pRawData->rodId(),
                                   pRawData->sswId(),
                                   pRawData->slbId(),
                                   pRawData->bitpos());
        tgcLUTDigit lutDigit;
        lutDigit.eta           = e.eta;
        lutDigit.phi           = e.phi;
        lutDigit.r             = e.r;
        lutDigit.z             = e.z;
        lutDigit.width         = e.width;
        lutDigit.sta           = e.sta;
        lutDigit.isStrip       = e.isStrip;
        lutDigit.rawData       = pRawData;
        lutDigit.inRoad        = false;
        lutDigit.isCoincidence = false;
        lutDigit.inSegment     = false;

        std::ostringstream oss;
        oss << "lutDigit r=" << e.r << " z=" << e.z << " phi=" << e.phi <<" station=" << e.sta << " strip=" << e.isStrip;
        m_msg->lock();
        m_msg->displayDebug(oss.str());
        m_msg->unlock();
        m_tgc_lut_digits.push_back(lutDigit);
    }

    return true;
}

bool TgcTrigDecoder::prepareTgcPoints()
{
    const TGCRoadParameters& road_parameters = m_svc->tgcRoadParameters();

    // Select the eta cut based on ROI Pt.
    double mid_eta_test = m_pMuonRoI->getThresholdNumber() < 4 ? road_parameters.ETA_LOW_MID_DELTA : road_parameters.ETA_HIGH_MID_DELTA;
    double inn_eta_test = m_pMuonRoI->getThresholdNumber() < 4 ? road_parameters.ETA_LOW_INN_DELTA : road_parameters.ETA_HIGH_INN_DELTA;

    // loop over TGC digits.
    unsigned int iDigit;
    for (iDigit = 0; iDigit < m_tgc_lut_digits.size(); iDigit++)
    {
        // Get the digit point.
        tgcLUTDigit& digit = m_tgc_lut_digits[iDigit];
        if (digit.isCoincidence)
            continue;

        if (digit.width == 0) continue;
	
	double w = 12.0 / digit.width / digit.width;
        if (digit.isStrip)
        {
            w *= digit.r * digit.r;
	    double roiPhi   = m_pMuonRoI->phi();
	    double digitPhi = digit.phi;
	    double dphi = fabs(digitPhi - roiPhi);
	    if( dphi > M_PI ) dphi = M_PI*2 - dphi;
            // For strips, apply phi cut
            if (m_svc->isCosmic() || (digit.sta < 3 && dphi < road_parameters.PHI_MID_DELTA))
            {
	        double phi = digitPhi;
	        if( digitPhi*roiPhi < 0 && fabs(digitPhi-roiPhi) > M_PI ) {
		   if( roiPhi > 0 ) phi += M_PI*2;
		   else             phi -= M_PI*2;
		   /*
		   char buf[128];
		   m_msg->lock();
		   sprintf(buf,"phi corrected: digitPhi/roiPhi/phi=%f/%f/%f",digitPhi,roiPhi,phi);
		   m_msg->displayInfo(buf);
		   m_msg->unlock();
		   */
		}
                m_tgcStripMidPoints.push_back(TgcFit::Point(iDigit + 1, digit.sta, digit.z, phi, w));
                digit.inRoad = true;
            }
            else if (m_svc->isCosmic() || (digit.sta == 3 && dphi < road_parameters.PHI_INN_DELTA))
            {
	        double phi = digitPhi;
	        if( digitPhi*roiPhi < 0 && fabs(digitPhi-roiPhi) > M_PI ) {
		   if( roiPhi > 0 ) phi += M_PI*2;
		   else             phi -= M_PI*2;
		   /*
		   char buf[128];
		   m_msg->lock();
		   sprintf(buf,"phi corrected: digitPhi/roiPhi/phi=%f/%f/%f",digitPhi,roiPhi,phi);
		   m_msg->displayInfo(buf);
		   m_msg->unlock();
		   */
		}
                m_tgcStripInnPoints.push_back(TgcFit::Point(iDigit + 1, digit.sta, digit.z, phi, w));
                digit.inRoad = true;
            }
        }
        else
        {
            // For wires, apply eta cut.
            if (m_svc->isCosmic() || (digit.sta < 3 && fabs(digit.eta - m_pMuonRoI->eta()) < mid_eta_test))
            {
                m_tgcWireMidPoints.push_back(TgcFit::Point(iDigit + 1, digit.sta, digit.z, digit.r, w));
                digit.inRoad = true;
            }
            else if (m_svc->isCosmic() || (digit.sta == 3 && fabs(digit.eta - m_pMuonRoI->eta()) < inn_eta_test))
            {
                m_tgcWireInnPoints.push_back(TgcFit::Point(iDigit + 1, digit.sta, digit.z, digit.r, w));
                digit.inRoad = true;
            }
        }
    }

    return true;
}

void TgcTrigDecoder::postprocessTgcPoints()
{
    TgcFit::PointArray::const_iterator itPt;
    for (itPt = m_tgcStripMidPoints.begin(); itPt != m_tgcStripMidPoints.end(); itPt++)
    {
        if (!itPt->bOutlier)
            m_tgc_lut_digits[itPt->nIdx-1].inSegment = true;
    }
    for (itPt = m_tgcStripInnPoints.begin(); itPt != m_tgcStripInnPoints.end(); itPt++)
    {
        if (!itPt->bOutlier)
            m_tgc_lut_digits[itPt->nIdx-1].inSegment = true;
    }
    for (itPt = m_tgcWireMidPoints.begin(); itPt != m_tgcWireMidPoints.end(); itPt++)
    {
        if (!itPt->bOutlier)
            m_tgc_lut_digits[itPt->nIdx-1].inSegment = true;
    }
    for (itPt = m_tgcWireInnPoints.begin(); itPt != m_tgcWireInnPoints.end(); itPt++)
    {
        if (!itPt->bOutlier)
            m_tgc_lut_digits[itPt->nIdx-1].inSegment = true;
    }
}

void TgcTrigDecoder::fillTgcPointsIntoESD(MuonFeatureDetails* det) {
    if(det == 0) return;
    TgcFit::PointArray::const_iterator itPt;
    for (itPt = m_tgcStripMidPoints.begin(); itPt != m_tgcStripMidPoints.end(); itPt++)
    {
        const tgcLUTDigit& digit = m_tgc_lut_digits[itPt->nIdx-1];
        det->setTgcMidPhihit(digit.phi,digit.r,digit.z,digit.width,digit.inSegment);
    }
    for (itPt = m_tgcStripInnPoints.begin(); itPt != m_tgcStripInnPoints.end(); itPt++)
    {
        const tgcLUTDigit& digit = m_tgc_lut_digits[itPt->nIdx-1];
        det->setTgcInnPhiHit(digit.phi,digit.r,digit.z,digit.width,digit.inSegment);
    }
    for (itPt = m_tgcWireMidPoints.begin(); itPt != m_tgcWireMidPoints.end(); itPt++)
    {
        const tgcLUTDigit& digit = m_tgc_lut_digits[itPt->nIdx-1];
        det->setTgcMidRhoHit(digit.phi,digit.r,digit.z,digit.width,digit.inSegment);
    }
    for (itPt = m_tgcWireInnPoints.begin(); itPt != m_tgcWireInnPoints.end(); itPt++)
    {
        const tgcLUTDigit& digit = m_tgc_lut_digits[itPt->nIdx-1];
        det->setTgcInnRhoHit(digit.phi,digit.r,digit.z,digit.width,digit.inSegment);
    }

}
