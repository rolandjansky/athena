/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ANNSelectionTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuGirlSelection/ANNSelectionTool.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "MuGirlSelection/test_barrel.h"
#include "MuGirlSelection/test_endcap.h"
#include "Particle/TrackParticle.h"

//================ Constructor =================================================

MuGirlNS::ANNSelectionTool::ANNSelectionTool(const std::string& t,
        const std::string& n,
        const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_pMuGirlCandidateTool("MuGirlNS::CandidateTool")
{
    declareInterface<IANNSelectionTool>(this);
    declareProperty("CandidateTool", m_pMuGirlCandidateTool);
}

//================ Destructor =================================================

MuGirlNS::ANNSelectionTool::~ANNSelectionTool()
{}

//=============================================================================
int MuGirlNS::ANNSelectionTool::calculateRegion(double /*pt*/, double eta)
{
    //double pt = info->pTrackParticle->measuredPerigee()->pT();
    //double eta = fabs(info->pTrackParticle->measuredPerigee()->eta());
    int iregion = -1;
    //  PERFORM NEURAL NETWORK SELECTION ONLY IF PT<10 GEV
    if (1)
    {
        if (eta<0.4 || (eta>=0.65 && eta<0.9))
            iregion = 0;  // BARREL GOOD
        if (eta>=0.4 && eta<0.65)
            iregion = 1;  // BARREL BAD
        if (eta>=.9 && eta<1.15)
            iregion = 2;  // REGIONS OVERLAP
        if (eta>=1.15 && eta<1.4)
            iregion = 3;  // MAGNETS OVERLAP
        if (eta>=1.4 && eta<2.0)
            iregion = 4;  // ENDCAP
        if (eta>=2.0)
            iregion = 5;  // FORWARD
    }
    if (0)
    {
        if (eta<1.1)
            iregion = 6;  // BARREL GOOD
        if (eta>0.9)
            iregion = 7;  // BARREL BAD
    }
    return iregion;
}


//================ Initialisation =================================================

StatusCode MuGirlNS::ANNSelectionTool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endmsg;
    StatusCode sc = AthAlgTool::initialize();
    if (sc.isFailure())
        return sc;

    if (retrieve(m_pMuGirlCandidateTool).isFailure())
        return StatusCode::RECOVERABLE;

    msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode MuGirlNS::ANNSelectionTool::finalize()
{
    msg(MSG::INFO) << "Finalizing " << name() << " - package version " << PACKAGE_VERSION << endmsg;
    StatusCode sc = AthAlgTool::finalize();
    return sc;
}

//=============================================================================
double MuGirlNS::ANNSelectionTool::evaluateAnnBarrelGoodLowPt(CandidateSummary* info)
{
    AnnResults_tgcfix_barrel_good ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    return ANN.value(0,
                     info->numMdtHits,
                     info->numRpcPhiHits,
                     info->numRpcEtaHits,
                     info->numMdtSegs,
                     info->numRpcSegs,
                     info->numMdtHitsInSeg,
                     info->numRpcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}

double MuGirlNS::ANNSelectionTool::evaluateAnnBarrelGoodHighPt(CandidateSummary* info)
{   
    //AnnResults_barrelg_gt5 ANN;
    AnnResults_tgcfix_barrel_good ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    return ANN.value(0,
                     info->numMdtHits,
                     info->numRpcPhiHits,
                     info->numRpcEtaHits,
                     info->numMdtSegs,
                     info->numRpcSegs,
                     info->numMdtHitsInSeg,
                     info->numRpcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}


double MuGirlNS::ANNSelectionTool::evaluateAnnBarrelBadLowPt(CandidateSummary* info)
{
    AnnResults_tgcfix_barrel_bad ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numRpcPhiHits,
                     info->numRpcEtaHits,
                     info->numMdtSegs,
                     info->numRpcSegs,
                     info->numMdtHitsInSeg,
                     info->numRpcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}

double MuGirlNS::ANNSelectionTool::evaluateAnnBarrelBadHighPt(CandidateSummary* info)
{   
    //AnnResults_barrelb_gt5 ANN;
    AnnResults_tgcfix_barrel_bad ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numRpcPhiHits,
                     info->numRpcEtaHits,
                     info->numMdtSegs,
                     info->numRpcSegs,
                     info->numMdtHitsInSeg,
                     info->numRpcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}


double MuGirlNS::ANNSelectionTool::evaluateAnnEndCapLowPt(CandidateSummary* info)
{
    //AnnResults_tgcfix_endcap_nocsc ANN;
    AnnResults_endcap_lt5 ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcPhiHits,
                     info->numTgcEtaHits,
                     info->numMdtSegs,
                     info->numTgcSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}

double MuGirlNS::ANNSelectionTool::evaluateAnnEndCapHighPt(CandidateSummary* info)
{
    AnnResults_endcap_gt5 ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcPhiHits,
                     info->numTgcEtaHits,
                     info->numMdtSegs,
                     info->numTgcSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}


double MuGirlNS::ANNSelectionTool::evaluateAnnForwardLowPt(CandidateSummary* info)
{
    //AnnResults_tgcfix_forward_nocsc ANN;
    AnnResults_forward_lt5 ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcPhiHits,
                     info->numTgcEtaHits,
                     info->numMdtSegs,
                     info->numTgcSegs,
                     //info->numCscSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}

double MuGirlNS::ANNSelectionTool::evaluateAnnForwardHighPt(CandidateSummary* info)
{
    AnnResults_forward_gt5 ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcPhiHits,
                     info->numTgcEtaHits,
                     info->numMdtSegs,
                     info->numTgcSegs,
                     //info->numCscSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}


double MuGirlNS::ANNSelectionTool::evaluateAnnRegionOverlapLowPt(CandidateSummary* info)
{
    //AnnRegionOverlapLowPt ANN;
    //AnnResultsOverlapReg_2 ANN;
    AnnResults_tgcfix_barrel_endcap ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcRpcPhiHits,
                     info->numTgcRpcEtaHits,
                     info->numMdtSegs,
                     info->numTgcRpcSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcRpcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}

double MuGirlNS::ANNSelectionTool::evaluateAnnRegionOverlapHighPt(CandidateSummary* info)
{
    //AnnRegionOverlapLowPt ANN;
    //AnnResultsOverlapReg_2 ANN;
    //AnnResults_overlapreg_gt5 ANN;
    AnnResults_tgcfix_barrel_endcap ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcRpcPhiHits,
                     info->numTgcRpcEtaHits,
                     info->numMdtSegs,
                     info->numTgcRpcSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcRpcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}


double MuGirlNS::ANNSelectionTool::evaluateAnnMagnetOverlapLowPt(CandidateSummary* info)
{
    AnnResults_tgcfix_magnet ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcPhiHits,
                     info->numTgcEtaHits,
                     info->numMdtSegs,
                     info->numTgcSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}

double MuGirlNS::ANNSelectionTool::evaluateAnnMagnetOverlapHighPt(CandidateSummary* info)
{
    //AnnResults_overlapmag_gt5 ANN;
    AnnResults_tgcfix_magnet ANN;
    double angle = info->innAngle;
    double delta = info->innDelta;
    if (angle == 999.0)
    {
        angle = info->midAngle;
        delta = info->midDelta;
    }
    double LHR = info->lhr;
    if (LHR<0) LHR = 0;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcPhiHits,
                     info->numTgcEtaHits,
                     info->numMdtSegs,
                     info->numTgcSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcHitsInSeg,
                     angle,
                     delta,
                     LHR);
}


double MuGirlNS::ANNSelectionTool::evaluateAnnTest_barrel(CandidateSummary* info)
{
    test_barrel ANN;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numRpcPhiHits,
                     info->numRpcEtaHits,
                     info->numMdtSegs,
                     info->numRpcSegs,
                     info->numMdtHitsInSeg,
                     info->numRpcHitsInSeg);
}

double MuGirlNS::ANNSelectionTool::evaluateAnnTest_endcap(CandidateSummary* info)
{
    test_endcap ANN;
    return ANN.value(0,
                     info->numMdtHits,
                     info->numTgcPhiHits,
                     info->numTgcEtaHits,
                     info->numMdtSegs,
                     info->numTgcSegs,
                     info->numMdtHitsInSeg,
                     info->numTgcHitsInSeg,
                     info->pTrackParticle->perigeeParameters().eta());
}
//============================================================================================
void MuGirlNS::ANNSelectionTool::evaluateANN(CandidateSummary* info,double v[8], double NN[2])
{
   double pt = 0;
   double eta = 0;
   if (info->startFromEF && info->pMuonEFTrack !=NULL) {
   pt = (info->pMuonEFTrack)->pt();
   eta = fabs((info->pMuonEFTrack)->eta());
   }
   else if (info->startFromMF && info->pMuonFeature !=NULL)
   {
      pt = fabs((info->pMuonFeature)->pt())*1000;
      eta = fabs((info->pMuonFeature)->eta());
   }
   else 
   if (info->pTrackParticle != NULL)
   {
    pt = info->pTrackParticle->perigeeParameters().pT();
    eta = fabs(info->pTrackParticle->perigeeParameters().eta());
   } 
   int region = calculateRegion(pt,eta);
   ANNFunction ANNFunctions[8]={NULL};
   if (pt<5000)
   {
      ANNFunctions[0]=&MuGirlNS::ANNSelectionTool::evaluateAnnBarrelGoodLowPt;
      ANNFunctions[1]=&MuGirlNS::ANNSelectionTool::evaluateAnnBarrelBadLowPt;
      ANNFunctions[2]=&MuGirlNS::ANNSelectionTool::evaluateAnnRegionOverlapLowPt;
      ANNFunctions[3]=&MuGirlNS::ANNSelectionTool::evaluateAnnMagnetOverlapLowPt;
      ANNFunctions[4]=&MuGirlNS::ANNSelectionTool::evaluateAnnEndCapLowPt;
      ANNFunctions[5]=&MuGirlNS::ANNSelectionTool::evaluateAnnForwardLowPt;
   }
   else
   {
      ANNFunctions[0]=&MuGirlNS::ANNSelectionTool::evaluateAnnBarrelGoodHighPt;
      ANNFunctions[1]=&MuGirlNS::ANNSelectionTool::evaluateAnnBarrelBadHighPt;
      ANNFunctions[2]=&MuGirlNS::ANNSelectionTool::evaluateAnnRegionOverlapHighPt;
      ANNFunctions[3]=&MuGirlNS::ANNSelectionTool::evaluateAnnMagnetOverlapHighPt;
      ANNFunctions[4]=&MuGirlNS::ANNSelectionTool::evaluateAnnEndCapHighPt;
      ANNFunctions[5]=&MuGirlNS::ANNSelectionTool::evaluateAnnForwardHighPt;
   }
      ANNFunctions[6]=&MuGirlNS::ANNSelectionTool::evaluateAnnTest_barrel;
      ANNFunctions[7]=&MuGirlNS::ANNSelectionTool::evaluateAnnTest_endcap;
    NN[0] = NN[1] = 0.;
    for (int i=0;i<6;i++)
        v[i] = 0;

    if (region >-1)
    {
        v[region] = (*this.*ANNFunctions[region])(info);
        if (region < 2)
            NN[0] = v[region];
        else if (region > 1)
            NN[1] = v[region];
    }
}
