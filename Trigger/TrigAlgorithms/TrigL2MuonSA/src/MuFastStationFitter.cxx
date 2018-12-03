/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastStationFitter.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"

#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include <math.h>
#include <iomanip>

#include "TMath.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

using namespace TrigL2MuonSA;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastStationFitter("IID_MuFastStationFitter", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastStationFitter::interfaceID() { return IID_MuFastStationFitter; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastStationFitter::MuFastStationFitter(const std::string& type, 
                                                       const std::string& name,
                                                       const IInterface*  parent): 
  AthAlgTool(type,name,parent),
  m_backExtrapolator("TrigMuonBackExtrapolator"),
  m_alphaBetaEstimate("TrigL2MuonSA::AlphaBetaEstimate"),
  m_ptFromAlphaBeta("TrigL2MuonSA::PtFromAlphaBeta")
{
   declareInterface<TrigL2MuonSA::MuFastStationFitter>(this);

   declareProperty("BackExtrapolator", m_backExtrapolator, "public tool for back extrapolating the muon tracks to the IV");
   declareProperty("PtFromAlphaBeta", m_ptFromAlphaBeta);

   declareProperty("ENDCAPINN_MDT_CHI2_LIMIT", m_endcapinn_mdt_chi2_limit = 20);
   declareProperty("ENDCAPMID_MDT_CHI2_LIMIT", m_endcapmid_mdt_chi2_limit = 20);
   declareProperty("ENDCAPOUT_MDT_CHI2_LIMIT", m_endcapout_mdt_chi2_limit = 20);
   declareProperty("ENDCAPEE_MDT_CHI2_LIMIT",  m_endcapee_mdt_chi2_limit  = 20);

   declareProperty("RWIDTH_EndcapINN_FIRST",  m_rwidth_Endcapinn_first    = 150.);
   declareProperty("RWIDTH_EndcapINN_SECOND", m_rwidth_Endcapinn_second   = 80. );
   declareProperty("RWIDTH_EndcapMID_FIRST",  m_rwidth_Endcapmid_first    = 150.);
   declareProperty("RWIDTH_EndcapMID_SECOND", m_rwidth_Endcapmid_second   = 100.);
   declareProperty("RWIDTH_EndcapOUT_FIRST",  m_rwidth_Endcapout_first    = 120.);
   declareProperty("RWIDTH_EndcapOUT_SECOND", m_rwidth_Endcapout_second   = 60. );
   declareProperty("RWIDTH_EndcapEE_FIRST",   m_rwidth_Endcapee_first     = 150.);
   declareProperty("RWIDTH_EndcapEE_SECOND",  m_rwidth_Endcapee_second    = 100.);

   declareProperty("MDT_DRFITSPACE_UPLIMIT",   m_mdt_driftspace_uplimit   = 14.8);
   declareProperty("MDT_DRFITSPACE_DOWNLIMIT", m_mdt_driftspace_downlimit = 0.1);
   declareProperty("MDT_DRFITTIME_LIMIT",      m_mdt_drifttime_limit      = 1700.);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastStationFitter::~MuFastStationFitter() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::initialize()
{
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
      return sc;
   }
   
  // BackExtrapolator services
   sc = m_backExtrapolator.retrieve();
   if ( !sc.isSuccess() ) {
     ATH_MSG_ERROR("Could not retrieve " << m_backExtrapolator);
    return sc;
  } 

   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
StatusCode TrigL2MuonSA::MuFastStationFitter::setMCFlag(BooleanProperty use_mcLUT)
{
  m_use_mcLUT = use_mcLUT;
  StatusCode sc = StatusCode::SUCCESS;

  if (m_use_mcLUT) {
     sc = serviceLocator()->service("PtEndcapLUTSvc_MC", m_ptEndcapLUTSvc);
  } else {
     sc = serviceLocator()->service("PtEndcapLUTSvc",    m_ptEndcapLUTSvc);
  }
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not find PtEndcapLUTSvc");
     return sc;
  }

  // Calculation of alpha and beta
  sc = m_alphaBetaEstimate.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_alphaBetaEstimate);
    return sc;
  }
  ATH_MSG_DEBUG("Retrieved service " << m_alphaBetaEstimate);

  m_alphaBetaEstimate->setMCFlag(m_use_mcLUT, m_ptEndcapLUTSvc);

  // conversion: alpha, beta -> pT
  sc = m_ptFromAlphaBeta.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_ptFromAlphaBeta);
    return sc;
  }
  ATH_MSG_DEBUG("Retrieved service " << m_ptFromAlphaBeta);

  m_ptFromAlphaBeta->setMCFlag(m_use_mcLUT, m_ptEndcapLUTSvc);

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::findSuperPoints(const LVL1::RecMuonRoI*    p_roi,
                                                              TrigL2MuonSA::RpcFitResult& rpcFitResult,
                                                              std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{

  StatusCode sc = StatusCode::SUCCESS;

  // 
  std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
  for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) { // loop for track candidates

    if (rpcFitResult.isSuccess) {
      //      itTrack->phiMSDir = rpcFitResult.phiDir;
      itTrack->phiMSDir = (cos(rpcFitResult.phi)!=0)? tan(rpcFitResult.phi): 0;
    } else {
      itTrack->phiMSDir = (cos(p_roi->phi())!=0)? tan(p_roi->phi()): 0;
      itTrack->isRpcFailure = true;
    }
    
    sc = superPointFitter(*itTrack);
    if (sc != StatusCode::SUCCESS) return sc;
  }
  // 

  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::findSuperPoints(const LVL1::RecMuonRoI*   p_roi,
                                                              TrigL2MuonSA::TgcFitResult& tgcFitResult,
                                                              std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{

  StatusCode sc = StatusCode::SUCCESS;
  
  std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
  for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) { // loop for track candidates

    if (tgcFitResult.isSuccess) {
      itTrack->phiMSDir = tgcFitResult.phiDir;
    } else {
      itTrack->phiMSDir = (cos(p_roi->phi())!=0)? tan(p_roi->phi()): 0;
      itTrack->isTgcFailure = true;
    }

    sc = superPointFitter(*itTrack);
    if (sc != StatusCode::SUCCESS) return sc;
  }
  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::findSuperPoints(const LVL1::RecMuonRoI*   p_roi,
                                                              const TrigL2MuonSA::MuonRoad& muonRoad,
                                                              TrigL2MuonSA::TgcFitResult& tgcFitResult,
                                                              std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{

  StatusCode sc = StatusCode::SUCCESS;
  
  std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
  for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) { // loop for track candidates

    if (tgcFitResult.isSuccess) {
      itTrack->phiMSDir = tgcFitResult.phiDir;
    } else {
      itTrack->phiMSDir = (cos(p_roi->phi())!=0)? tan(p_roi->phi()): 0;
      itTrack->isTgcFailure = true;
    }

    sc = superPointFitter(*itTrack, muonRoad);

    if (sc != StatusCode::SUCCESS) return sc;

    makeReferenceLine(*itTrack, muonRoad); 
    sc = m_alphaBetaEstimate->setAlphaBeta(p_roi, tgcFitResult, *itTrack, muonRoad);
    if (!sc.isSuccess()) {
      ATH_MSG_WARNING("Endcap alpha and beta estimation failed");
      return sc;
    }

    sc = m_ptFromAlphaBeta->setPt(*itTrack,tgcFitResult);
    if (!sc.isSuccess()) {
      ATH_MSG_WARNING("Endcap pT estimation failed");
      return sc;
    }

    double exInnerA = fromAlphaPtToInn(tgcFitResult,*itTrack);
    double bw = muonRoad.bw[3][0];
    double aw = muonRoad.aw[3][0];
    if(exInnerA !=0 ) updateInnSP(*itTrack, exInnerA, aw,bw);

  }
  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::superPointFitter(TrigL2MuonSA::TrackPattern& trackPattern)
{
   int   count;
   int   FitFlag;
   float Xor, Yor, sigma, rm=0., phim=0;
   float Ymid, Xmid, Amid;

   const float ZERO_LIMIT         = 1e-6;
   const unsigned int MAX_STATION = 10; // no BMG(Backup=10)
   const float SIGMA              = 0.0080;
   const float DRIFTSPACE_LIMIT   = 16.;
   const int   MIN_MDT_FOR_FIT    = 3;

   TrigL2MuonSA::MdtHits*    mdtSegment;
   TrigL2MuonSA::SuperPoint* superPoint;
   TrigL2MuonSA::PBFitResult pbFitResult;

   for (unsigned int chamber=0; chamber<MAX_STATION; chamber++) { // loop for station
     if (chamber==9) continue;//skip BME chamber
     
     count = 0;
     Xor = 0.;
     Yor = 0.;
     Amid = 0.;
     Xmid = 0.;
     Ymid = 0.;
     
     mdtSegment = &(trackPattern.mdtSegments[chamber]);
     superPoint = &(trackPattern.superPoints[chamber]);
     
     if (mdtSegment->size()==0) continue;

     TrigL2MuonSA::MdtHits::iterator itMdtHit;
     for (itMdtHit=mdtSegment->begin(); itMdtHit!=mdtSegment->end(); itMdtHit++) { // loop for MDT hit
       
       
       if (count >= NMEAMX) continue;
       if (itMdtHit->isOutlier) continue;
       
       superPoint->Ndigi++;
       
       if (!count) {
         rm   = itMdtHit->cYmid;
         Amid = itMdtHit->cAmid;
         Xmid = itMdtHit->cXmid;
         Ymid = itMdtHit->cYmid;
       }
       if (!Xor) {
         Xor = itMdtHit->R;
         Yor = itMdtHit->Z;
       }
       
       phim  = itMdtHit->cPhip;
       sigma = (fabs(itMdtHit->DriftSigma) > ZERO_LIMIT)? itMdtHit->DriftSigma: SIGMA;

       int station = 0;
       if (chamber == 0 || chamber == 3 ) station = 0;
       if (chamber == 1 || chamber == 4 ) station = 1;
       if (chamber == 2 || chamber == 5 ) station = 2;
       if (chamber == 6 ) station = 3;
       if ( fabs(itMdtHit->DriftSpace) > ZERO_LIMIT &&
            fabs(itMdtHit->DriftSpace) < DRIFTSPACE_LIMIT &&
            fabs(itMdtHit->DriftTime) > ZERO_LIMIT ) {

         pbFitResult.XILIN[count] = itMdtHit->R - Xor;
         pbFitResult.YILIN[count] = itMdtHit->Z - Yor;
         pbFitResult.IGLIN[count] = 2;
         pbFitResult.RILIN[count] = (fabs(itMdtHit->DriftSpace) > ZERO_LIMIT)?
           itMdtHit->DriftSpace: SetDriftSpace(itMdtHit->DriftTime, itMdtHit->R, itMdtHit->Z, phim, trackPattern.phiMSDir);
         pbFitResult.WILIN[count] = 1/(sigma*sigma);
         pbFitResult.JLINE[count] = count;
         pbFitResult.IDMEA[count] = station*10 + itMdtHit->Layer;
 
         pbFitResult.DISTJ[count] = 0.;
         pbFitResult.RESI[count] = 0.;
 
         count++;
         pbFitResult.NPOI = count;

      } else {
         superPoint->Ndigi--;
      }
       
     } // end loop for MDT hits

     ATH_MSG_DEBUG("... MDT hit used in fit #=" << pbFitResult.NPOI);
     for(int i=0;i<pbFitResult.NPOI;i++) {
       ATH_MSG_DEBUG("i/XILIN[i]/YILIN[i]/RILIN[i]/WILIN[i] = "
		     << i << "/" << pbFitResult.XILIN[i] << "/" << pbFitResult.YILIN[i]
		     << "/" << pbFitResult.RILIN[i] << "/" << pbFitResult.WILIN[i]);
     }
     
     if (count >= MIN_MDT_FOR_FIT) {

       FitFlag = Evlfit(1, pbFitResult);
       ATH_MSG_DEBUG("FitFlag = " << FitFlag);
       
       float ac = Amid;
       float bc = (Ymid - Xor) -ac*(Xmid - Yor);
       float X = ( (pbFitResult.ALIN*bc)+pbFitResult.BLIN )/( 1-ac*pbFitResult.ALIN );
       
       superPoint->Npoint = pbFitResult.NPOI;
       
       if (trackPattern.s_address == -1) { // Endcap

         if (pbFitResult.ALIN!=0. ) {
           superPoint->Z = rm;
           superPoint->R = (rm-Yor)/pbFitResult.ALIN - pbFitResult.BLIN/pbFitResult.ALIN + Xor;
           superPoint->Alin = 1./pbFitResult.ALIN;
           superPoint->Blin = -pbFitResult.BLIN/pbFitResult.ALIN;

           if (chamber==0){//barrel inner
             superPoint->R      = ac*X + bc + Xor;
             superPoint->Z      = X + Yor;
             superPoint->Alin   = pbFitResult.ALIN;
             superPoint->Blin   = pbFitResult.BLIN;
           }
         }
 
       } else { // Barrel
 
         superPoint->R      = ac*X + bc + Xor;
         superPoint->Z      = X + Yor;
         superPoint->Alin   = pbFitResult.ALIN;
         superPoint->Blin   = pbFitResult.BLIN; 
         if ( chamber == 3 ){ //// Endcap Inner
           superPoint->Z = rm;
           superPoint->R = (rm-Yor)/pbFitResult.ALIN - pbFitResult.BLIN/pbFitResult.ALIN + Xor;
           superPoint->Alin = 1./pbFitResult.ALIN;
           superPoint->Blin = -pbFitResult.BLIN/pbFitResult.ALIN;
         }
       }
       
       superPoint->Phim   = phim;
       superPoint->Xor    = Xor;
       superPoint->Yor    = Yor;
       superPoint->Chi2   = pbFitResult.CHI2;
       superPoint->PChi2  = pbFitResult.PCHI2;
       for(int i=0;i<pbFitResult.NPOI;i++) superPoint->Residual[i] =  pbFitResult.RESI[i];
       
     }
     
     ATH_MSG_DEBUG("... Superpoint chamber/s_address/count/R/Z/Alin/Blin/Phim/Xor/Yor/Chi2/PChi2="
		   << chamber << "/" << trackPattern.s_address << "/" << count << "/"
		   << superPoint->R << "/" << superPoint->Z << "/" << superPoint->Alin << "/"
		   << superPoint->Blin << "/" << superPoint->Phim << "/" << superPoint->Xor << "/"
		   << superPoint->Yor << "/" << superPoint->Chi2 << "/" << superPoint->PChi2);

   } // end loop for stations
   
   //
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
StatusCode TrigL2MuonSA::MuFastStationFitter::superPointFitter(TrigL2MuonSA::TrackPattern& trackPattern,
                                                               const TrigL2MuonSA::MuonRoad&    m_muonRoad)
{
  const unsigned int MAX_STATION = 10;
  TrigL2MuonSA::MdtHits*    mdtSegment;
  TrigL2MuonSA::SuperPoint* superPoint;
  TrigL2MuonSA::PBFitResult pbFitResult;
 
  for (unsigned int chamber=0; chamber<MAX_STATION; chamber++) { // loop for station
    ATH_MSG_DEBUG(" superpoint fit station "<<chamber);

    if(chamber== 1 || chamber == 2 || chamber ==7 || chamber == 9) continue; // only loop for endcap Inner/Middle/Outer/EE/barrel inn

    mdtSegment = &(trackPattern.mdtSegments[chamber]);
    superPoint = &(trackPattern.superPoints[chamber]);
    if (mdtSegment->size()==0) continue;
    
    TrigL2MuonSA::MdtHits::iterator itMdtHit;
    if (chamber==0 || chamber == 6 || chamber==8){
      int   count=0;
      int   FitFlag;
      float Xor   = 0.;
      float Yor   = 0.;
      float sigma = 0.;
      float rm    = 0.;
      float phim  = 0.;
      float Ymid  = 0.;
      float Xmid  = 0.;
      float Amid  = 0.;
      const float ZERO_LIMIT         = 1e-6;
      const float SIGMA              = 0.0080;
      const float DRIFTSPACE_LIMIT   = 16.;
      const int   MIN_MDT_FOR_FIT    = 3;

     for (itMdtHit=mdtSegment->begin(); itMdtHit!=mdtSegment->end(); itMdtHit++) { // loop for MDT hit

       if (count >= NMEAMX) continue;
       if (itMdtHit->isOutlier) continue;

       superPoint->Ndigi++;
       if (!count) {
         rm   = itMdtHit->cYmid;
         Amid = itMdtHit->cAmid;
         Xmid = itMdtHit->cXmid;
         Ymid = itMdtHit->cYmid;
       }
       if (!Xor) {
         Xor = itMdtHit->R;
         Yor = itMdtHit->Z;
       }
       
       phim  = itMdtHit->cPhip;
       sigma = (fabs(itMdtHit->DriftSigma) > ZERO_LIMIT)? itMdtHit->DriftSigma: SIGMA;

       int station = 0;
       if (chamber == 6 ) station = 3;
       if (chamber == 0 ) station = 0;
       if ( fabs(itMdtHit->DriftSpace) > ZERO_LIMIT &&
            fabs(itMdtHit->DriftSpace) < DRIFTSPACE_LIMIT &&
            fabs(itMdtHit->DriftTime) > ZERO_LIMIT ) {
         
         pbFitResult.XILIN[count] = itMdtHit->R - Xor;
         pbFitResult.YILIN[count] = itMdtHit->Z - Yor;
         pbFitResult.IGLIN[count] = 2;
         pbFitResult.RILIN[count] = (fabs(itMdtHit->DriftSpace) > ZERO_LIMIT)?
         	itMdtHit->DriftSpace: SetDriftSpace(itMdtHit->DriftTime, itMdtHit->R, itMdtHit->Z, phim, trackPattern.phiMSDir);
         pbFitResult.WILIN[count] = 1/(sigma*sigma);
         pbFitResult.JLINE[count] = count;
         pbFitResult.IDMEA[count] = station*10 + itMdtHit->Layer;
         pbFitResult.DISTJ[count] = 0.;
         pbFitResult.RESI[count] = 0.;
         count++;
         pbFitResult.NPOI = count;
        } else {
          superPoint->Ndigi--;
        }
      } // end loop for MDT hits
     
     ATH_MSG_DEBUG("... MDT hit used in fit #=" << pbFitResult.NPOI);
      for(int i=0;i<pbFitResult.NPOI;i++) {
        ATH_MSG_DEBUG("i/XILIN[i]/YILIN[i]/RILIN[i]/WILIN[i] = "
		      << i << "/" << pbFitResult.XILIN[i] << "/" << pbFitResult.YILIN[i]
		      << "/" << pbFitResult.RILIN[i] << "/" << pbFitResult.WILIN[i]);
      }
      if (count >= MIN_MDT_FOR_FIT) {
        FitFlag = Evlfit(1, pbFitResult);
        ATH_MSG_DEBUG("FitFlag = " << FitFlag);
       
        float ac = Amid;
        float bc = (Ymid - Xor) -ac*(Xmid - Yor);
        float X = ( (pbFitResult.ALIN*bc)+pbFitResult.BLIN )/( 1-ac*pbFitResult.ALIN );
       
        superPoint->Npoint = pbFitResult.NPOI;
        if (trackPattern.s_address == -1) { // Endcap
          if (pbFitResult.ALIN!=0. ) {
            superPoint->Z = rm;
            superPoint->R = (rm-Yor)/pbFitResult.ALIN - pbFitResult.BLIN/pbFitResult.ALIN + Xor;
            superPoint->Alin = 1./pbFitResult.ALIN;
            superPoint->Blin = -pbFitResult.BLIN/pbFitResult.ALIN;
            if (chamber==0 || chamber==8){//endcap barrel inner or BEE
              superPoint->R      = ac*X + bc + Xor;
              superPoint->Z      = X + Yor;
              superPoint->Alin   = pbFitResult.ALIN;
              superPoint->Blin   = pbFitResult.BLIN;
            }
           }      
         }
           superPoint->Phim   = phim;
           superPoint->Xor    = Xor;
           superPoint->Yor    = Yor;
           superPoint->Chi2   = pbFitResult.CHI2;
           superPoint->PChi2  = pbFitResult.PCHI2;
           for(int i=0;i<pbFitResult.NPOI;i++) superPoint->Residual[i] =  pbFitResult.RESI[i];

       }
      ATH_MSG_DEBUG("...Special Superpoint chamber/s_address/count/R/Z/Alin/Blin/Phim/Xor/Yor/Chi2/PChi2="
		    << chamber << "/" << trackPattern.s_address << "/" << count << "/"
		    << superPoint->R << "/" << superPoint->Z << "/" << superPoint->Alin << "/"
		    << superPoint->Blin << "/" << superPoint->Phim << "/" << superPoint->Xor << "/"
		    << superPoint->Yor << "/" << superPoint->Chi2 << "/" << superPoint->PChi2);
       continue;
    }

    double m_aw = m_muonRoad.aw[chamber][0];
    double m_bw = m_muonRoad.bw[chamber][0];
    double nrWidth = 0.; 
    unsigned int  sumN = 0;
    //chamber=3/4/5 => Endcap Inner/Middle/Outer
    if(chamber==3) {nrWidth = m_rwidth_Endcapinn_first;}
    if(chamber==4) {nrWidth = m_rwidth_Endcapmid_first;}
    if(chamber==5) {nrWidth = m_rwidth_Endcapout_first;}
    
    for(itMdtHit=mdtSegment->begin(); itMdtHit!=mdtSegment->end(); itMdtHit++){
      if (fabs(itMdtHit->DriftSpace) < m_mdt_driftspace_downlimit ||
          fabs(itMdtHit->DriftSpace) > m_mdt_driftspace_uplimit){
        itMdtHit->isOutlier = 2;
        continue;
      }
            
      if(itMdtHit->isOutlier > 1)continue;
      double Z = itMdtHit->Z;
      double R = itMdtHit->R;
      double nbw = m_aw*Z + m_bw;
      if (R>(nbw-nrWidth) && R<(nbw+nrWidth)){
        itMdtHit->isOutlier = 0;
        sumN++;
      }  else {
        itMdtHit->isOutlier = 2;
         continue;
      }
    }
    if(sumN==0) continue;
    
    stationSPFit(mdtSegment, superPoint,pbFitResult, trackPattern.s_address,chamber,m_aw, trackPattern.phiMSDir);
    
  } // end loop for stations
  
  return StatusCode::SUCCESS; 
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
void TrigL2MuonSA::MuFastStationFitter::stationSPFit(TrigL2MuonSA::MdtHits*    mdtSegment, 
                                                     TrigL2MuonSA::SuperPoint* superPoint,
                                                     TrigL2MuonSA::PBFitResult& pbFitResult,int s_address, int i_station,double aw, float phiDir){

  TrigL2MuonSA::MdtHits::iterator itMdtHit;

  unsigned int i_layer_max = 0;
  int   count;
  //int   FitFlag;
  float Xor, Yor, sigma, rm=0., phim=0;
  float Ymid, Xmid, Amid;
 
  const float ZERO_LIMIT         = 1e-6;
  const unsigned int MAX_STATION = 8;
  const float SIGMA              = 0.0080;
  const float DRIFTSPACE_LIMIT   = 16.;
  const int   MIN_MDT_FOR_FIT    = 3;

  const unsigned int MAX_LAYER = 12;
  //const unsigned int MAX_CAND  = 6;
  unsigned int MdtLayerHits[MAX_STATION][MAX_LAYER];
  std:: vector<unsigned int> MdtLayerHits_index[MAX_STATION][MAX_LAYER];

  for (unsigned int i_st=0; i_st<MAX_STATION; i_st++) {
    for (unsigned int i_ly=0; i_ly<MAX_LAYER; i_ly++) {
      MdtLayerHits[i_st][i_ly] = 0;
      MdtLayerHits_index[i_st][i_ly].clear();
    }
  }

  float Chbest = 1.e25;
  double avZ[8];
  double avR[8];
  double sumZ[8];
  double sumR[8];
  unsigned int sumN[8];
  double nsWidth=0.;

  for (unsigned int i_st=0; i_st<8; i_st++) {
    avZ[i_st]  = 0.;
    avR[i_st]  = 0.;
    sumZ[i_st] = 0.;
    sumR[i_st] = 0.;
    sumN[i_st] = 0.;
  }

  for ( int i_hit=0; i_hit<(int)mdtSegment->size(); i_hit++){
    //unsigned int i_station =mdtSegment->at(i_hit).Chamber;

    if (mdtSegment->at(i_hit).isOutlier>1) continue;

    double Z = mdtSegment->at(i_hit).Z;
    double R = mdtSegment->at(i_hit).R;

    sumZ[i_station] = sumZ[i_station] + Z;
    sumR[i_station] = sumR[i_station] + R;
    sumN[i_station]++;

    if (sumN[i_station]!=0) {
      avZ[i_station] = sumZ[i_station]/sumN[i_station];
      avR[i_station] = sumR[i_station]/sumN[i_station];
    }
  }

  if (sumN[i_station]==0) return;

  for (unsigned int i_hit=0; i_hit<mdtSegment->size(); i_hit++) {

    if (mdtSegment->at(i_hit).isOutlier>1) continue;

    double Z = mdtSegment->at(i_hit).Z;
    double R = mdtSegment->at(i_hit).R;

    if (i_station==3) nsWidth = m_rwidth_Endcapinn_second;
    if (i_station==4) nsWidth = m_rwidth_Endcapmid_second;
    if (i_station==5) nsWidth = m_rwidth_Endcapout_second;

    double nbw = aw*Z+(avR[i_station]-aw*avZ[i_station]);

    if ( R>(nbw-nsWidth) && R<(nbw+nsWidth) ) {
       mdtSegment->at(i_hit).isOutlier = 0;
    } else {
       mdtSegment->at(i_hit).isOutlier = 2;
       continue;
    }
  }

  for ( int i_hit=0; i_hit<(int)mdtSegment->size(); i_hit++) {

    unsigned int i_layer =mdtSegment->at(i_hit).Layer;
    if (mdtSegment->at(i_hit).isOutlier>1) continue;
    if (i_layer > i_layer_max) i_layer_max = i_layer;

    MdtLayerHits[i_station][i_layer]++;
    MdtLayerHits_index[i_station][i_layer].push_back(i_hit);
  }

  unsigned int real_layer_num = 0;
  std::vector<unsigned int> Ly_1st;
  Ly_1st.clear();
  std::vector<float> Line_A;
  std::vector<float> Line_B;
  std::vector<float> Line_Chi2;
  std::vector<unsigned int> Line_count;
  std::vector<float>  Line_Xor;
  std::vector<float>Line_Yor ;
  std::vector<float>Line_Amid;
  std::vector<float>Line_Xmid;
  std::vector<float>Line_Ymid;
  std::vector<float>  Line_sum_Z;
  std::vector<float>  Line_sum_R;
  std::vector<float>  Line_rm;
  std::vector<float>  Line_phim;
  std::vector<float>  Maxlayers_A;
  std::vector<float>  Maxlayers_B;
  std::vector<float>  Maxlayers_Chi2;
  std::vector<float>  Maxlayers_RESI;
  float Maxlayers_Phim  = 0;
  float Maxlayers_R     = 0;
  float Maxlayers_Z     = 0;
  float Maxlayers_Xor   = 0;
  float Maxlayers_Yor   = 0;
  float Maxlayers_PChi2 = 0;
  int   Maxlayers_N     = 0;
  Maxlayers_A.clear(); 
  Maxlayers_B.clear();
  Maxlayers_Chi2.clear();
  Maxlayers_RESI.clear();

  for (unsigned int i_layer=0; i_layer<=i_layer_max; i_layer++) {

    if (MdtLayerHits[i_station][i_layer]==0) continue;

    Ly_1st.push_back(i_layer);
    real_layer_num++;
  }

  const int real_layer= Ly_1st.size();
  std::vector<std::vector<unsigned int> > Ly_flg;

  for (int pr=real_layer; pr>=3; pr--) {

    Ly_flg.clear();
    Line_A.clear(); 
    Line_B.clear();
    Line_Chi2.clear();
    Line_count.clear();
    Line_Xor.clear();
    Line_Yor .clear();
    Line_Amid.clear();
    Line_Xmid.clear();
    Line_Ymid.clear();
    Line_sum_Z.clear();
    Line_sum_R.clear();
    Line_rm.clear();
    Line_phim.clear();

    int total_cp = 0;
    findLayerCombination(Ly_1st, real_layer, pr,Ly_flg, total_cp);

    for (int i=0;i<(int)Ly_flg.size(); i++) {

      std::vector<std::vector<int> >tID;
      tID.clear();
      std::vector<std::vector<int> >tIndex;
      tIndex.clear();

      int tube_ID[NMEAMX][2];
      int tubeindex[NMEAMX][2];

      for (int ti=0; ti<8; ti++) {
        for (int tj=0; tj<2; tj++) {
          tube_ID[ti][tj]   = 0;
          tubeindex[ti][tj] = 0;
        }
      }

      for (int j=0; j<(int)Ly_flg[i].size(); j++) {

        int i_layer = Ly_flg[i][j];
        std::vector<int> tid;
        tid.clear();
        std::vector<int> tindex;
        tindex.clear();
        if (MdtLayerHits[i_station][i_layer]==0) continue;

        float tube_1st = 999999.;
        float tube_2nd = 999999.;
        int layer_1st= 9999;
        int layer_2nd = 9999;

        for (unsigned int i_hit=0; i_hit< MdtLayerHits[i_station][i_layer]; i_hit++) {

          unsigned int i_index = MdtLayerHits_index[i_station][i_layer].at(i_hit);

          if (mdtSegment->at(i_index).isOutlier>1) continue;

          float nbw3 = (mdtSegment->at(i_index).Z)*(aw) + (avR[i_station]-(aw)*avZ[i_station]) ;
          float dis_tube = fabs(fabs(nbw3-mdtSegment->at(i_index).R)- mdtSegment->at(i_index).DriftSpace);

          if (dis_tube<tube_1st) {
            tube_2nd  = tube_1st;
            layer_2nd = layer_1st;
            tube_1st  = dis_tube;
            layer_1st = i_index;
          } else if (dis_tube<tube_2nd) {
            tube_2nd  = dis_tube;
            layer_2nd = i_index;
          }
        }

        if ( layer_1st != 9999 ) {
          mdtSegment->at(layer_1st).isOutlier = 0;
          tid.push_back(1);
          tindex.push_back(layer_1st);
        }

        if ( layer_2nd != 9999 ) {
          mdtSegment->at(layer_2nd).isOutlier = 1;
          tid.push_back(1);
          tindex.push_back(layer_2nd);
        }

        tID.push_back(tid);
        tIndex.push_back(tindex);
      }

      for (int ti=0; ti<(int)tID.size();ti++) {
        for (int tj=0; tj<(int)tID[ti].size();tj++) {
          tube_ID[ti][tj]   = tID[ti][tj];
          tubeindex[ti][tj] = tIndex[ti][tj];
        }
      }

      std::vector<int> isg;
      std::vector<int> hitarray;
      int sumid;
      int ntry = (int)floor(pow(2.,pr))-1;

      for (int ntryi=0; ntryi<=ntry; ntryi++) {

        isg.clear();
        hitarray.clear();
        sumid = 1;

        for (int ntryj=1; ntryj<=pr; ntryj++) {
          int yhit = 0;
          int Isg = (ntryi&(int)pow(2.,ntryj-1))? 1 : 0;
          isg.push_back(Isg);
          if (tube_ID[ntryj-1][Isg] != 0)  yhit = 1;
          sumid = sumid * yhit;
        }

        if (sumid==1) {
          for (int tt=0;tt<(int)isg.size(); tt++) {
            int tindex = tubeindex[tt][isg[tt]];
            hitarray.push_back(tindex);
          }
        }

        count = 0;
        Xor = 0.;
        Yor = 0.;
        Amid = 0.;
        Xmid = 0.;
        Ymid = 0.;

        float sum_Z_used = 0.;
        float sum_R_used = 0.;

        if (hitarray.size()==0) continue;

        for (itMdtHit=mdtSegment->begin(); itMdtHit!=mdtSegment->end(); itMdtHit++) { // loop for MDT hit

          int hit_index = std::distance(mdtSegment->begin(),itMdtHit);

          if(mdtSegment->at(hit_index).isOutlier>1) continue;

	  if (count >= NMEAMX) continue;

          int fd=0;

          for (int j=0; j<(int)hitarray.size(); j++) {

            if (hitarray[j]==hit_index) {
              fd=1;
              break;
            }
          }

          if (fd==0) continue;

          superPoint->Ndigi++;

          if (!count) {
            rm   = itMdtHit->cYmid;
            Amid = itMdtHit->cAmid;
            Xmid = itMdtHit->cXmid;
            Ymid = itMdtHit->cYmid;
          }

          if (!Xor) {
            Xor = itMdtHit->R;
            Yor = itMdtHit->Z;
          }

          phim  = itMdtHit->cPhip;
          sigma = (fabs(itMdtHit->DriftSigma) > ZERO_LIMIT)? itMdtHit->DriftSigma: SIGMA;

          if ( fabs(itMdtHit->DriftSpace) > ZERO_LIMIT &&
               fabs(itMdtHit->DriftSpace) < DRIFTSPACE_LIMIT &&
               fabs(itMdtHit->DriftTime) > ZERO_LIMIT ) {

             pbFitResult.XILIN[count] = itMdtHit->R - Xor;
             pbFitResult.YILIN[count] = itMdtHit->Z - Yor;
             pbFitResult.IGLIN[count] = 2;
             pbFitResult.RILIN[count] = (fabs(itMdtHit->DriftSpace) > ZERO_LIMIT)?
               itMdtHit->DriftSpace: SetDriftSpace(itMdtHit->DriftTime, itMdtHit->R, itMdtHit->Z, phim, phiDir);//itMdtHit->DriftSpace ;//
             pbFitResult.WILIN[count] = 1/(sigma*sigma);
             pbFitResult.JLINE[count] = count;

             int i_st = 0;
             if (i_station==3) i_st = 0;
             if (i_station==4) i_st = 1;
             if (i_station==5) i_st = 2;
             pbFitResult.IDMEA[count] = i_st*10 + itMdtHit->Layer;
             pbFitResult.DISTJ[count] = 0.;
             pbFitResult.RESI[count] = 0.;

             count++;
             pbFitResult.NPOI = count;

             sum_Z_used = sum_Z_used + itMdtHit->Z;
             sum_R_used = sum_R_used + itMdtHit->R;
          } else {
             superPoint->Ndigi--;
          }
        } // end loop for MDT hits

        ATH_MSG_DEBUG("... MDT hit used in fit #=" << pbFitResult.NPOI);
        for (int i=0;i<pbFitResult.NPOI;i++) {
          ATH_MSG_DEBUG("i/XILIN[i]/YILIN[i]/RILIN[i]/WILIN[i] = "
			<< i << "/" << pbFitResult.XILIN[i] << "/" << pbFitResult.YILIN[i]
			<< "/" << pbFitResult.RILIN[i] << "/" << pbFitResult.WILIN[i]);
        }

        if (count >= MIN_MDT_FOR_FIT) {
          Circles(pbFitResult.NPOI,pbFitResult.XILIN,pbFitResult.YILIN,pbFitResult.RILIN,pbFitResult.WILIN,
                  pbFitResult.IGLIN,&pbFitResult.ALIN,&pbFitResult.BLIN,pbFitResult.DABLIN,&pbFitResult.CHI2,
                  &pbFitResult.PCHI2, pbFitResult.SlopeCand, pbFitResult.InterceptCand, pbFitResult.Chi2Cand);

         //FitFlag = Evlfit(1, pbFitResult);

          for (int cand=0; cand<6; cand++) {

            if (pbFitResult.SlopeCand[cand]!=0.) {
              Line_A.push_back(1/pbFitResult.SlopeCand[cand]);
              Line_B.push_back(-pbFitResult.InterceptCand[cand]/pbFitResult.SlopeCand[cand]-Yor/pbFitResult.SlopeCand[cand]+Xor); 
              Line_Chi2.push_back(pbFitResult.Chi2Cand[cand]);
              Line_count.push_back(pbFitResult.NPOI);
              Line_Xor.push_back(Xor);
              Line_Yor .push_back(Yor);
              Line_Amid.push_back(Amid);
              Line_Xmid.push_back(Xmid);
              Line_Ymid.push_back(Ymid);
              Line_sum_Z.push_back(sum_Z_used/count);
              Line_sum_R.push_back(sum_R_used/count);
              Line_rm.push_back(rm);
              Line_phim.push_back(phim);
            }
          }
        }
      }
    }//end one of cp

    if (Line_Chi2.size()==0) continue;

    std::multimap<float, int>chi_map;
    chi_map.clear();
    std::vector<float> t_A;
    std::vector<float> t_B;
    std::vector<float> t_Chi2;
    std::vector<float> t_count;
    std::vector<float> t_Xor;
    std::vector<float> t_Yor;
    std::vector<float> t_Amid;
    std::vector<float> t_Xmid;
    std::vector<float> t_Ymid;
    std::vector<float> t_sum_Z;
    std::vector<float> t_sum_R;
    std::vector<float> t_rm;
    std::vector<float> t_phim;

    t_A.clear();
    t_B.clear();
    t_Chi2.clear();
    t_count.clear();
    t_Xor.clear();
    t_Yor.clear();
    t_Amid.clear();
    t_Xmid.clear();
    t_Ymid.clear();
    t_sum_Z.clear();
    t_sum_R.clear();
    t_rm.clear();
    t_phim.clear();

    for (int ir=0; ir<(int)Line_Chi2.size(); ir++) chi_map.insert(std::make_pair(Line_Chi2.at(ir), ir));

    for (std::multimap<float, int>::iterator jt = chi_map.begin(); jt != chi_map.end(); ++jt) {
      t_A.push_back(Line_A.at(jt->second));
      t_B.push_back(Line_B.at(jt->second));
      t_Chi2.push_back(Line_Chi2.at(jt->second));
      t_count.push_back(Line_count.at(jt->second));
      t_Xor.push_back(Line_Xor.at(jt->second));
      t_Yor.push_back(Line_Yor.at(jt->second));
      t_Amid.push_back(Line_Amid.at(jt->second));
      t_Xmid.push_back(Line_Xmid.at(jt->second));
      t_Ymid.push_back(Line_Ymid.at(jt->second));
      t_sum_Z.push_back(Line_sum_Z.at(jt->second));
      t_sum_R.push_back(Line_sum_R.at(jt->second));
      t_rm.push_back(Line_rm.at(jt->second));
      t_phim.push_back(Line_phim.at(jt->second));
      if(pr==real_layer){//save max layers information
        Maxlayers_A.push_back(Line_A.at(jt->second));
        Maxlayers_B.push_back(Line_B.at(jt->second));
        Maxlayers_Chi2.push_back(Line_Chi2.at(jt->second));
      }
    }

    superPoint->Npoint = t_count[0];//pbFitResult.NPOI;
    if(i_station==4 && pr==real_layer){
      Maxlayers_Z     = t_sum_Z[0];
      Maxlayers_R     = t_A[0]*t_sum_Z[0]+t_B[0];
      Maxlayers_Phim  = t_phim[0];
      Maxlayers_Xor   = t_Xor[0];
      Maxlayers_Yor   = t_Yor[0];
      Maxlayers_PChi2 = pbFitResult.PCHI2;
      Maxlayers_N     = t_count[0];  
    }

    if (s_address == -1) { // Endcap

      if (t_A[0]!=0. ) {
        superPoint->Z = t_sum_Z[0];
        superPoint->R = t_A[0]*t_sum_Z[0]+t_B[0];
        superPoint->Alin =t_A[0]; 
        superPoint->Blin =t_B[0]; 
      }

      superPoint->Phim   = t_phim[0];
      superPoint->Xor    = t_Xor[0];
      superPoint->Yor    = t_Yor[0];
      superPoint->Chi2   = t_Chi2[0];
      superPoint->PChi2  = pbFitResult.PCHI2;

      for (int i=0;i<pbFitResult.NPOI;i++) superPoint->Residual[i] =  pbFitResult.RESI[i];

      for (int cand=0; cand<6; cand++) {
        if (t_A[cand]!=0. ) {
          superPoint->SlopeCand[cand]     = t_A[cand];
          superPoint->InterceptCand[cand] = t_B[cand];
          superPoint->Chi2Cand[cand]      = t_Chi2[cand];
        }
      }
    }

    Chbest=t_Chi2[0];

    if (real_layer>3) {
      if ((i_station == 3 || i_station == 5) && pr==4 && Chbest > m_endcapmid_mdt_chi2_limit) {

        superPoint->Z =0.; 
        superPoint->R =0.; 
        superPoint->Alin=0.;
        superPoint->Blin=0.;

        for (int cand=0; cand<6; cand++) {
          superPoint->SlopeCand[cand]     = 0.;
          superPoint->InterceptCand[cand] = 0.;
          superPoint->Chi2Cand[cand]      = 0.;
        }
        return;
      }
    }
    if (Chbest<m_endcapmid_mdt_chi2_limit){ 

      ATH_MSG_DEBUG("... Superpoint chamber/s_address/count/R/Z/Alin/Blin/Phim/Xor/Yor/Chi2/PChi2="
		    << i_station << "/" << s_address << "/" << count << "/"
		    << superPoint->R << "/" << superPoint->Z << "/" << superPoint->Alin << "/"
		    << superPoint->Blin << "/" << superPoint->Phim << "/" << superPoint->Xor << "/"
		    << superPoint->Yor << "/" << superPoint->Chi2 << "/" << superPoint->PChi2);

       break;//jump out all cp
    }else{
    	if(i_station==4 && Maxlayers_A.size()>0){
        superPoint->Npoint = Maxlayers_N; 
        superPoint->Z = Maxlayers_Z;
        superPoint->R = Maxlayers_R;
        superPoint->Alin =Maxlayers_A[0]; 
        superPoint->Blin =Maxlayers_B[0]; 
        superPoint->Phim   = Maxlayers_Phim;
        superPoint->Xor    = Maxlayers_Xor;
        superPoint->Yor    = Maxlayers_Yor;
        superPoint->Chi2   = Maxlayers_Chi2[0];
        superPoint->PChi2  = Maxlayers_PChi2;
        for (int cand=0; cand<6; cand++) {
          if (Maxlayers_A[cand]!=0. ) {
            superPoint->SlopeCand[cand]     = Maxlayers_A[cand];
            superPoint->InterceptCand[cand] = Maxlayers_B[cand];
            superPoint->Chi2Cand[cand]      = Maxlayers_Chi2[cand];
          }
        }
      }
    }
  }//end all cp

  return;
}

// --------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void TrigL2MuonSA::MuFastStationFitter::makeReferenceLine(TrigL2MuonSA::TrackPattern&   trackPattern,
                                                          const TrigL2MuonSA::MuonRoad& muonRoad){

  TrigL2MuonSA::SuperPoint* superPoint;
  const unsigned int MAX_STATION = 8;
  float aw[8];
  //float bw[8]; 
  float spZ[8];
  float spR[8]; 
  float A_cand[8][6];
  float B_cand[8][6];
  float Chi2_cand[8][6]; 
  float spA[8];
  float spB[8];
  float spChi2[8];
  const int middle = 4;
  const int outer  = 5;

  for (unsigned int i_station=4; i_station<MAX_STATION; i_station++) {

    aw[i_station]=0.;
    //bw[i_station]=0.;  
    spZ[i_station]=0.;
    spR[i_station]=0.; 
    spA[i_station]=0.;
    spB[i_station]=0.;
    spChi2[i_station]=0.;

    for (unsigned int ci=0; ci<NCAND; ci++) {
      A_cand[i_station][ci]    =0.;  
      B_cand[i_station][ci]    =0.; 
      Chi2_cand[i_station][ci] =0.;   
    }

    if (i_station<4 || i_station>5) continue; // only loop for endcap Inner/Middle/Outer

    superPoint = &(trackPattern.superPoints[i_station]);
    aw[i_station] = muonRoad.aw[i_station][0];
    //bw[i_station] = muonRoad.bw[i_station][0];
    spZ[i_station]  = superPoint->Z;
    spR[i_station]  = superPoint->R;
    spA[i_station]  = superPoint->Alin;
    spB[i_station]  = superPoint->Blin;

    for (unsigned int cand=0; cand<NCAND; cand++) {
      A_cand[i_station][cand]    = superPoint->SlopeCand[cand];
      B_cand[i_station][cand]    = superPoint->InterceptCand[cand];
      Chi2_cand[i_station][cand] = superPoint->Chi2Cand[cand];
    }
  }

  //A[station][candidate]
  float test_diff  = 1.e25;
  float best_diff  = 1.e25;
  float rmatched   = 0.;
  float match_midA = 0.;
  float match_outA = 0.;

  if (A_cand[middle][0] == 0. && A_cand[middle][1] == 0.) {
    spZ[middle]    = 0.;
    spR[middle]    = 0.;
    spChi2[middle] = 0.;
  }

  if (A_cand[outer][0] == 0. && A_cand[outer][1] == 0.) {
    spZ[outer]    = 0.;
    spR[outer]    = 0.;
    spChi2[outer] = 0.;
  }

  if (A_cand[middle][0] != 0. && A_cand[outer][0] == 0.) {

    best_diff = 1.e25;
    test_diff = 1.e25;

    for (int m=0; m<6; m++) {

      test_diff = fabs(A_cand[middle][m] - aw[middle]);

      if (test_diff<best_diff) {
        best_diff      = test_diff;
        rmatched       = A_cand[middle][m];
        spB[middle]    = B_cand[middle][m];
        spChi2[middle] = Chi2_cand[middle][m];
       spR[middle]    = rmatched * spZ[middle] + spB[middle];

      }
    }
  }

  if(A_cand[outer][1]  != 0. && A_cand[outer][0]  != 0. && spA[outer] !=0.  && spZ[outer]  !=0. && spR[outer] !=0. &&
     A_cand[middle][1] != 0. && A_cand[middle][0] != 0. && spA[middle]!=0.  && spZ[middle] !=0. && spR[middle] !=0.){
    
    float sp_line = 0.;
    if((spZ[outer]-spZ[middle]) !=0.) sp_line = (spR[outer]-spR[middle])/(spZ[outer]-spZ[middle]);

    for (int t=0; t<2; ++t) {
      best_diff = 1.e25;
      test_diff = 1.e25;
      if (sp_line!=0.) {
        for (int i=0; i<6; ++i) {
          if (t==0) test_diff = fabs(A_cand[middle][i] - sp_line);
          else if (t==1) test_diff = fabs(A_cand[outer][i] - sp_line);
          if (test_diff<best_diff) {
            best_diff = test_diff;
            if (t==0) {
            match_midA      = A_cand[middle][i];
            spB[middle]     = B_cand[middle][i];
            spChi2[middle]  = Chi2_cand[middle][i];
            spR[middle]     = match_midA * spZ[middle] + spB[middle];
            } else if(t==1) {
            match_outA     = A_cand[outer][i];
            spB[outer]     = B_cand[outer][i];
            spChi2[outer]  = Chi2_cand[outer][i];
            spR[outer]     = match_outA * spZ[outer] + spB[outer];
            }
          }
        }
      }
    }
  }

  if (spA[middle]!=0.) {
    if (match_midA != 0.) {
      spA[middle] = match_midA;
    } else if (rmatched != 0.) {
      spA[middle] = rmatched;
    }

    if (match_outA != 0.) spA[outer] = match_outA;

  }

  for (unsigned int i_station=4; i_station<MAX_STATION; i_station++) {
    if (i_station<4 || i_station>5) continue; // only loop for endcap Inner/Middle/Outer
    superPoint = &(trackPattern.superPoints[i_station]);
    if(spA[i_station]!=0.){
      superPoint->Alin =spA[i_station];
      superPoint->Blin =spB[i_station];
      superPoint->Chi2 =spChi2[i_station];
    }
  }
  return;
}

//--------------------------------------------------------------------------------------

double TrigL2MuonSA::MuFastStationFitter::fromAlphaPtToInn(TrigL2MuonSA::TgcFitResult& tgcFitResult,
                                                           TrigL2MuonSA::TrackPattern& trackPattern)
{

  TrigL2MuonSA::SuperPoint* superPoint;
  float MiddleSlope     = 0;//float MiddleIntercept = 0;
  float OuterSlope      = 0;//float OuterIntercept  = 0;
  //float MiddleR         = 0; float MiddleZ         = 0;
  //float OuterR          = 0;float OuterZ          = 0;

  for (int i_station=4; i_station<6; i_station++) {

    int chamberID = -1;

    if ( i_station == 4 ) chamberID = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
    if ( i_station == 5 ) chamberID = xAOD::L2MuonParameters::Chamber::EndcapOuter;

    superPoint = &(trackPattern.superPoints[chamberID]);

    if ( superPoint->Npoint > 2 && superPoint->R > 0.) {
      if ( i_station==4 ) { 
        MiddleSlope = superPoint->Alin;
        //MiddleIntercept = superPoint->R - MiddleSlope*superPoint->Z;
        //MiddleR = superPoint->R;
        //MiddleZ = superPoint->Z;
      } if ( i_station==5 ) {
        OuterSlope  = superPoint->Alin;
        //OuterIntercept = superPoint->R - OuterSlope*superPoint->Z;
        //OuterR = superPoint->R;
        //OuterZ = superPoint->Z;
      }
    }
  }

  double mdtpT    = fabs(tgcFitResult.tgcPT);
  double alpha_pt = fabs(trackPattern.ptEndcapAlpha);

  if (MiddleSlope!=0. && OuterSlope!=0.) {
    mdtpT = alpha_pt;
  } else if (fabs(tgcFitResult.tgcPT)>=8.0 && MiddleSlope != 0.) {
    mdtpT = alpha_pt;
  }

  mdtpT = (fabs(tgcFitResult.tgcPT)>1e-5)? mdtpT*(tgcFitResult.tgcPT/fabs(tgcFitResult.tgcPT)) : 0;
  double etaMiddle = (tgcFitResult.tgcMid1[3])? tgcFitResult.tgcMid1[0] : tgcFitResult.tgcMid2[0];
  double phiMiddle = (tgcFitResult.tgcMid1[3])? tgcFitResult.tgcMid1[1] : tgcFitResult.tgcMid2[1];
  double eta;
  double sigma_eta;
  double extrInnerEta = 0;
  double naw = 0;
  xAOD::L2StandAloneMuon* muonSA = new xAOD::L2StandAloneMuon();
  muonSA->makePrivateStore();
  muonSA->setSAddress(-1);
  muonSA->setPt(mdtpT);
  muonSA->setEtaMS(etaMiddle);
  muonSA->setPhiMS(phiMiddle);
  muonSA->setRMS(0.);
  muonSA->setZMS(0.);
  double phi;
  double sigma_phi;
  double theta = 0.;
  StatusCode sc = m_backExtrapolator->give_eta_phi_at_vertex(muonSA, eta,sigma_eta,phi,sigma_phi,0.);

  if (sc.isSuccess() ){
    extrInnerEta = eta;
  } else {
    extrInnerEta = etaMiddle;
  }

  delete muonSA;

  if (extrInnerEta != 0.) {
    theta = atan(exp(-fabs(extrInnerEta)))*2.;
    naw = tan(theta)*(fabs(extrInnerEta)/extrInnerEta);
  }

  return naw;

}
// --------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void TrigL2MuonSA::MuFastStationFitter::updateInnSP(TrigL2MuonSA::TrackPattern& trackPattern,
                                                    double &aw,
                                                    double &tgc_aw,
                                                    double &bw)
{

  double nrWidth = m_rwidth_Endcapinn_first;
  //double nsWidth = 80.;
  //double bw = 0.;
  unsigned int  sumN[8];

  for (unsigned int i_st=0; i_st<8;i_st++) {
    sumN[i_st] = 0;
  }

  TrigL2MuonSA::MdtHits* mdtSegment;
  TrigL2MuonSA::SuperPoint* superPoint;
  TrigL2MuonSA::PBFitResult pbFitResult;
  const int i_station = 3;//endcap inner
  int chamberID = i_station;

  mdtSegment = &(trackPattern.mdtSegments[chamberID]);
  superPoint = &(trackPattern.superPoints[chamberID]);

  if (mdtSegment->size()==0) return;

  TrigL2MuonSA::MdtHits::iterator itMdtHit;
  for(itMdtHit=mdtSegment->begin(); itMdtHit!=mdtSegment->end(); itMdtHit++){
    if (fabs(itMdtHit->DriftSpace) < m_mdt_driftspace_downlimit ||
        fabs(itMdtHit->DriftSpace) > m_mdt_driftspace_uplimit){
      itMdtHit->isOutlier = 2;
      continue;
    }
 
    if (itMdtHit->isOutlier > 1) continue;

    double Z = itMdtHit->Z;
    double R = itMdtHit->R;
    double nbw = tgc_aw*Z + bw;

    if (R>(nbw-nrWidth) && R<(nbw+nrWidth)) {
      itMdtHit->isOutlier = 0;
      sumN[i_station]++;
    } else {
      itMdtHit->isOutlier = 2;
      continue;
    }
  }

  if (sumN[i_station]==0) return;

  stationSPFit(mdtSegment, superPoint,pbFitResult, trackPattern.s_address,i_station, aw, trackPattern.phiMSDir);

  float df=1.e25;

  for (int cand=0; cand<NCAND; cand++) {
    float ds=fabs(superPoint->SlopeCand[cand]-aw);
    if (ds<df) {
      df=ds;
      superPoint->Alin = superPoint->SlopeCand[cand];
      superPoint->Blin = superPoint->InterceptCand[cand];
      superPoint->Chi2 = superPoint->Chi2Cand[cand];
    }
  }

  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastStationFitter::findLayerCombination(std::vector<unsigned int> &a,
                                                             int n,
                                                             int r,
                                                             std::vector<std::vector<unsigned int> > &c,
                                                             int &nr)
{
  std::vector<unsigned int> b;

  for (int i=0;i<r;i++) b.push_back(0);

  findSubLayerCombination(a,n,r,b,0,r,c,nr);

  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastStationFitter::findSubLayerCombination(std::vector<unsigned int> &a,
                                                                int n, 
                                                                int r,
                                                                std::vector<unsigned int> &b,
                                                                int index,
                                                                int num,
                                                                std::vector<std::vector<unsigned int> > &c,
                                                                int &nr)
{
  for (int i=index; i<n-num+1; i++) {

    b[r-num] = a[i];
    std::vector<unsigned int> t;
    t.clear();

    if (num==1) {

      for (int j = 0;j<r; j++) t.push_back(b[j]);

      c.push_back(t);
      nr++;

    } else {

      findSubLayerCombination(a,n,r,b,i+1,num-1,c,nr);
    }
  }

  return;
}
//-------------------------------------------------------------------------------

// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastStationFitter::finalize()
{
  ATH_MSG_DEBUG("Finalizing MuFastStationFitter - package version " << PACKAGE_VERSION);
   
   StatusCode sc = AthAlgTool::finalize(); 

   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

float TrigL2MuonSA::MuFastStationFitter::SetDriftSpace(float tdr, float rad, float zeta, float phim, float phiDir) {
  
  const float CSPEED = 2.99979e+10;    //     light speed (cm/s)
  const float MDT_RED = 0.7;
  const float DRIFTVE = 0.00209;        //     1.46/700 drif velocity (cm/ns)

  float flyspa,sy,x,y,phip,phis,prt,tof;
  
  sy = 1.;

  if(phim>=M_PI) {phim = phim - 2*M_PI; sy = -1.;}

  y = sy*fabs(phiDir*rad*sqrt(1./(1.+phiDir*phiDir)));
  x = y/phiDir;
  phip = atan2(y,x);
  phis = tan(fabs(phim-phip))*rad;

  if(phim>=phip) prt = - phis/(CSPEED*MDT_RED);
  else prt = + phis/(CSPEED*MDT_RED);

  flyspa = sqrt(rad*rad+zeta*zeta+phis*phis);
  tof = flyspa/CSPEED;

  return (tdr - tof*1.e+9 - prt*1.e+9)*DRIFTVE;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

/*==============================================================================

      Call CIRCLES to fit a line, then rejects the worst point(s)

      IFIT = 0 ok,     = 1 failure
      IFLA = 0 normal, = 1 do not try to exclude points

==============================================================================*/

int TrigL2MuonSA::MuFastStationFitter::Evlfit(int Ifla, TrigL2MuonSA::PBFitResult& pbFitResult) {

  int i,j,k,Ifit,Ntry,IGcur,Jbad;
  float Xnor,rlin,Xbad,test;
  
  pbFitResult.NDOF = -2;
  Ntry = 0;
  Ifit = 0;
  
  const double ZERO_LIMIT = 1e-6;
  
  for(j=0;j<pbFitResult.NPOI;j++) if(pbFitResult.IGLIN[j]>=1) pbFitResult.NDOF++;
  
  while(pbFitResult.NDOF>=1) {

    //      printf("pbFitResult.NDOF = %2d\n",pbFitResult.NDOF);
    Ntry++;
    Circles(pbFitResult.NPOI,pbFitResult.XILIN,pbFitResult.YILIN,pbFitResult.RILIN,pbFitResult.WILIN,
            pbFitResult.IGLIN,&pbFitResult.ALIN,&pbFitResult.BLIN,pbFitResult.DABLIN,&pbFitResult.CHI2,
            &pbFitResult.PCHI2);

    if(pbFitResult.CHI2<=ZERO_LIMIT) break;

    Xnor = 1. / sqrt(1. + pbFitResult.ALIN * pbFitResult.ALIN);

    for(i=0;i<pbFitResult.NPOI;i++) pbFitResult.RESI[i] = 0.;

    for(j=0;j<pbFitResult.NPOI;j++) {

      pbFitResult.DISTJ[j] = (pbFitResult.ALIN * pbFitResult.XILIN[j] + pbFitResult.BLIN - pbFitResult.YILIN[j]) * Xnor;
      IGcur = abs(pbFitResult.IGLIN[j])%100;

      if (IGcur==1) {
        pbFitResult.RESI[j] = pbFitResult.DISTJ[j];
      } else if (IGcur==2) {
        rlin = (pbFitResult.DISTJ[j]>=0.) ? pbFitResult.RILIN[j] : -pbFitResult.RILIN[j];
        pbFitResult.RESI[j] = pbFitResult.DISTJ[j] - rlin;
      } else if (IGcur==3) {
        pbFitResult.RESI[j] = pbFitResult.DISTJ[j] - pbFitResult.RILIN[j];
      }
    } 

    if(pbFitResult.PCHI2>=0.01||Ifla==1) return Ifit;
     
    if (pbFitResult.NDOF<=1||Ntry>=6) {

      Ifit  = 1;
      pbFitResult.NDOF  = 0;
      pbFitResult.ALIN  = 0.;
      pbFitResult.BLIN  = 0.;
      pbFitResult.CHI2  = -1.;
      pbFitResult.PCHI2 = -.5;

      for (i=0;i<2;i++) {
        for(k=0;k<2;k++) {
          pbFitResult.DABLIN[i][k] = 0.;
        }
      }

      for(j=0;j<pbFitResult.NPOI;j++) pbFitResult.IGLIN[j] = - abs(pbFitResult.IGLIN[j])%100;

      return Ifit;
    }
     
    //    Exclude the worst point       
    Jbad = 0;
    Xbad = -1.;
    for (j=0;j<pbFitResult.NPOI;j++) {
      if (pbFitResult.IGLIN[j]>=1) {
        test = pbFitResult.RESI[j] * pbFitResult.RESI[j] * pbFitResult.WILIN[j];
        if (test>=Xbad) {
          Xbad = test;
          Jbad = j;
        }
      }
    }
     
    //    Try again
     
    pbFitResult.IGLIN[Jbad] = - pbFitResult.IGLIN[Jbad] - 100;
    pbFitResult.NDOF        = pbFitResult.NDOF - 1;
     
  }

  return Ifit;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

/*==============================================================================

      Computes the best line tangent to many circles or points

      Version 1 - P.B. 960715 - First attempt
      Version 2 - P.B. 960910 - New algorithm for choice of the signs

      y = A x + b

      Input :
      =======

      NMEAS   : number of meas
      IG(i)   : for each meas, a flag.

      ---- if IG = 1 ---
      Xi, Yi  : x, y of the points (i=1...NMEAS) (e.g. wire of a mwp)
      RRi     : meaningless

      ---- if IG = 2 ---
      Xi, Yi  : x, y of the centers (i=1...NMEAS) (i.e. wire of a mdt)
      RRi     : distance line-center (i.e. measured drift distance)

      ---- if IG = 3 ---
      Xi, Yi  : as above, for IG=2
      RRi     : as above, but with +ve or -ve sign
 
      ---- if IG.le.0 ---> point NOT to be used
      Xi, Yi  : meaningless
      RRi     : meaningless

      Wi      : weight for the i-th meas (= 1/sigma**2)

      Output :
      ========

      A,B,DAB : line coefficients (y = Ax + b) + error matrix (2x2)
      CHI2    : Chi square = Sum Wi * (RRi - dist) ** 2 (error if .lt.0)
      PCHI2   : P(chisquare,d.o.f.)

==============================================================================*/

void TrigL2MuonSA::MuFastStationFitter::Circles (int Nmeas,float *XI,float *YI,float *RI,float *WI,int *IG,
                                                 float *A,float *B,float DAB[2][2],float *Chi2,float *Pchi2) {

  float RRi[NMEAMX],WIlim,CHbest,Abest,Bbest;
  float A0,B0,SAA,SBB,SAB,Square,Aj,Bj;
  int i,j,jj,Ntrue,Ngood,Nbest,Ntry,NOgo,Isig,Iflg,Ksign[4];
  int Ntrue2,Igg[NMEAMX];

//    Find the four "besrit" points (equispaced, wi.gt.1/2*wimax)

  *Chi2 = -1.;
  if (Nmeas<=2||Nmeas>=NMEAMX+1) return;
  Ntrue = 0;
  Ngood = 0;
  Nbest = 3;
  WIlim = 0.;
    
  Abest = 0.;
  Bbest = 0.;
    
  for(i=0;i<4;i++) Ksign[i] = -1;

  for (j=0;j<Nmeas;j++) {
    if (IG[j]>=1) {
      Ntrue++;
      WIlim = (WIlim>=WI[j])? WIlim : WI[j];
    }
  }

  if(Ntrue<=2) return;

  WIlim = 0.1 * WIlim;

  for(j=0;j<Nmeas;j++) if(IG[j]>=1&&WI[j]>=WIlim) Ngood++;
      
  for (j=0;j<Nmeas;j++) {
    if (IG[j]>=1&&(WI[j]>=WIlim||Ngood<=3)) {
      if (Ksign[0]==-1.) {
        Ksign[0] = j;
      } else if (Ksign[1]==-1.) {
        Ksign[1] = j;
      } else if(Ksign[2]==-1.) {
        Ksign[2] = j;
      } else if(Ksign[3]==-1.) {
        Ksign[3] = j;
        Nbest    = 4;
      } else {
        Ksign[2] = Ksign[3];
        Ksign[3] = j;
      }
    }
  }

  //    First attempt, try with a line through the centers

  Xline(XI,YI,WI,IG,Nmeas,&A0,&B0,&SAA,&SBB,&SAB,&Square);

  //    Then try 16 times trough the best four points

  for (i=0;i<NMEAMX;i++) Igg[i] = -1;

  CHbest = 1.e25;
  Ntry   = (int)floor(pow(2.,Nbest)) - 1;         // 2**4 - 1 = 15

  for (j=0;j<=Ntry;j++) {
    NOgo = 0;
    for (jj=1;jj<=Nbest;jj++) {
      Isig = (j&(int)pow(2.,jj-1))? 1 : 0;
      //          Isig = ibits(&j,&jj1,&one);            // alternatively 0, 1
      Iflg = IG[Ksign[jj-1]];
      Igg[Ksign[jj-1]] = Iflg;
      RRi[Ksign[jj-1]] = RI[Ksign[jj-1]];
      if (Iflg==2) {
        Igg[Ksign[jj-1]] = 3;
        if (Isig==1) RRi[Ksign[jj-1]] = - RI[Ksign[jj-1]];
      } else if (Isig==1) {
        NOgo = 1;
      }
    }

    if (NOgo==0) {
      Aj = A0;
      Bj = B0;
      Circfit(Nmeas,XI,YI,RRi,WI,Igg,&Aj,&Bj,DAB,Chi2);

      if (*Chi2>=0.0&&*Chi2<=CHbest) {
        Abest  = Aj;
        Bbest  = Bj;
        CHbest = *Chi2;
      }
    }
  }
  //    ... and finally with all the points

  *A = Abest;
  *B = Bbest;
  Circfit(Nmeas,XI,YI,RI,WI,IG,A,B,DAB,Chi2);

  if (*Chi2>=0.0) {
    Ntrue2 = Ntrue - 2;
    *Pchi2 = TMath::Prob(*Chi2, Ntrue2);
  }

  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

/*==============================================================================

      Auxiliary to circles (see) - It makes the actual computations

      NB - A and B MUST contain on input a first approximation of
           their values
  
==============================================================================*/

void TrigL2MuonSA::MuFastStationFitter::Circfit (int Nmeas,float *XI,float *YI,float *RI,float *WI,int *IG,
                                                 float *A,float *B,float DAB[2][2],float *Chi2) {

  float XX[NMEAMX],YY[NMEAMX],Test,Toll,Xnor,Aold,Bold,Epsi;
  float SAA,SAB,SBB,Square;
  int j,Niter;

  Toll    = .1;
  Niter   = 0;
  //      *A      = 0.;
  //      *B      = 0.;
  //      SAA     = 0.;
  //      SAB     = 0.;
  //      SBB     = 0.;
  Square  = 0.;

  //    Many iterations ...

  do {
    Niter++;
    Xnor  = 1. / sqrt(1. + *A * *A);
    Aold  = *A;
    Bold  = *B;
      for(j=0;j<Nmeas;j++) {
        XX[j] = 0.;
        YY[j] = 0.;
        if(IG[j]==1) {
          XX[j] = XI[j];
                YY[j] = YI[j];
            } else if(IG[j]==2) {
                if(*A * XI[j] + *B - YI[j]>=0.) Epsi = 1.0;    // mod 961017
                else Epsi = -1.0;
                XX[j] = XI[j] - Epsi * Xnor * fabs(RI[j]) * *A;
                YY[j] = YI[j] + Epsi * Xnor * fabs(RI[j]);
            } else if(IG[j]==3) {
                XX[j] = XI[j] - Xnor * RI[j] * *A;
                YY[j] = YI[j] + Xnor * RI[j];
            }
        }

        Xline(XX,YY,WI,IG,Nmeas,A,B,&SAA,&SBB,&SAB,&Square);
        if(Square<=0.) break;
        Test = ((Aold-*A)*(Aold-*A))/ SAA + ((Bold-*B)*(Bold-*B))/ SBB;

    } while(Test>=Toll&&Niter<=20);


    DAB[0][0] = SAA;
    DAB[0][1] = SAB;
    DAB[1][0] = SAB;
    DAB[1][1] = SBB;
    *Chi2     = Square;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

/*==============================================================================
      A simple linear fit : y = A x + B     (see PDG 94, 17.20-25)

      W  = weights ( = 1./err**2)
      IG = flags   (if .le.0, don't use)
==============================================================================*/

void TrigL2MuonSA::MuFastStationFitter::Xline (float *X,float *Y,float *W,int *IG,int NP,
     float *A,float *B,float *SAA,float *SBB,float *SAB,float *Square) {
      
    int j;
    float S1,SX,SY,SXX,SXY,SYY,Deter,DY;

    *Square = -7.;
    S1      = 0.;
    SX      = 0.;
    SY      = 0.;
    SXX     = 0.;
    SXY     = 0.;
    SYY     = 0.;
  
    for(j=0;j<NP;j++) {
        if(IG[j]>=1) {
            S1  = S1  + W[j];
            SX  = SX  + W[j] * X[j];
            SY  = SY  + W[j] * Y[j];
            SXX = SXX + W[j] * X[j] * X[j];
            SXY = SXY + W[j] * X[j] * Y[j];
            SYY = SYY + W[j] * Y[j] * Y[j];
        }
    }
  
    Deter  = S1 * SXX - SX * SX;
  
    if (Deter!=0.) {
        *A      = (S1 * SXY - SX * SY)  / Deter;
        *B      = (SY * SXX - SX * SXY) / Deter;
        *SAA    =   S1  / Deter;
        *SBB    =   SXX / Deter;
        *SAB    = - SX  / Deter;
    
        *Square = 0.;
        for(j=0;j<NP;j++) {
            if(IG[j]>=1) {
                DY =(Y[j] - *A * X[j] - *B)/sqrt(1 + *A * *A);
                //printf("Punto n.=%d , DY = %12.6f\n",j,DY);
                *Square = *Square + W[j] * DY * DY;
            }
        }
    }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
 void TrigL2MuonSA::MuFastStationFitter::Circles (int Nmeas,float *XI,float *YI,float *RI,float *WI,int *IG,
                                                  float *A,float *B,float DAB[2][2],float *Chi2,float *Pchi2,
                                                  float *SlopeCand, float *InterceptCand, float *Chi2Cand) 
{
  float RRi[NMEAMX],WIlim,CHbest,Abest,Bbest;
  float A0,B0,SAA,SBB,SAB,Square,Aj,Bj;
  int i,j,jj,Ntrue,Ngood,Nbest,Ntry,NOgo,Isig,Iflg,Ksign[4];
  int Ntrue2,Igg[NMEAMX];

  std::vector<float> st_chi2;
  std::vector<float> st_A;
  std::vector<float> st_B;

  for (int ii=0; ii<NCAND; ii++) {
    SlopeCand[ii]     = 0.;//99999;
    InterceptCand[ii] = 0.;//99999;
    Chi2Cand[ii]      = 0.;
  }

//    Find the four "besrit" points (equispaced, wi.gt.1/2*wimax)

  *Chi2 = -1.;
  if (Nmeas<=2||Nmeas>=NMEAMX+1) return;
  Ntrue = 0;
  Ngood = 0;
  Nbest = 3;
  WIlim = 0.;
   
  Abest = 0.;
  Bbest = 0.;
    
  for (i=0;i<4;i++) Ksign[i] = -1;

  for (j=0;j<Nmeas;j++) {
    if (IG[j]>=1) {
       Ntrue++;
       WIlim = (WIlim>=WI[j])? WIlim : WI[j];
    }
  }

  if (Ntrue<=2) return;

  WIlim = 0.1 * WIlim;

  for(j=0;j<Nmeas;j++) if(IG[j]>=1&&WI[j]>=WIlim) Ngood++;
      
  for (j=0;j<Nmeas;j++) {
    if (IG[j]>=1&&(WI[j]>=WIlim||Ngood<=3)) {

      if (Ksign[0]==-1.) {
        Ksign[0] = j;
      } else if (Ksign[1]==-1.) {
        Ksign[1] = j;
      } else if (Ksign[2]==-1.) {
        Ksign[2] = j;
      } else if(Ksign[3]==-1.) {
        Ksign[3] = j;
        Nbest    = 4;
      } else {
        Ksign[2] = Ksign[3];
        Ksign[3] = j;
      }
    }
  }

  //    First attempt, try with a line through the centers

  Xline(XI,YI,WI,IG,Nmeas,&A0,&B0,&SAA,&SBB,&SAB,&Square);

//    Then try 16 times trough the best four points
  st_A.clear(); st_B.clear(); st_chi2.clear();

  for (i=0;i<NMEAMX;i++) Igg[i] = -1;

  CHbest = 1.e25;
  Ntry   = (int)floor(pow(2.,Nbest)) - 1;         // 2**4 - 1 = 15

  for (j=0;j<=Ntry;j++) {

    NOgo = 0;

    for (jj=1;jj<=Nbest;jj++) {
      Isig = (j&(int)pow(2.,jj-1))? 1 : 0;
      //          Isig = ibits(&j,&jj1,&one);            // alternatively 0, 1
      Iflg = IG[Ksign[jj-1]];
      Igg[Ksign[jj-1]] = Iflg;
      RRi[Ksign[jj-1]] = RI[Ksign[jj-1]];

      if (Iflg==2) {

        Igg[Ksign[jj-1]] = 3;

        if (Isig==1) RRi[Ksign[jj-1]] = - RI[Ksign[jj-1]];

      } else if (Isig==1) {

        NOgo = 1;

      }
    }

    if (NOgo==0) {

      Aj = A0;
      Bj = B0;
      Circfit(Nmeas,XI,YI,RRi,WI,Igg,&Aj,&Bj,DAB,Chi2);
      Circfit(Nmeas,XI,YI,RI,WI,IG,&Aj,&Bj,DAB,Chi2);
      st_A.push_back(Aj); st_B.push_back(Bj); st_chi2.push_back(*Chi2);

      if (*Chi2>=0.0&&*Chi2<=CHbest) {
        Abest  = Aj;
        Bbest  = Bj;
        CHbest = *Chi2;
      }
    }
  }

  std::multimap<float, int>chi_map;
  chi_map.clear();
  std::vector<float> t_A;
  std::vector<float> t_B;
  std::vector<float> t_chi2;
  t_A.clear();
  t_B.clear();
  t_chi2.clear();

  for (int ir=0; ir<(int)st_chi2.size(); ir++) chi_map.insert(std::make_pair(st_chi2.at(ir), ir));

  for (std::multimap<float, int>::iterator jt = chi_map.begin(); jt != chi_map.end(); ++jt) {
    t_A.push_back(st_A.at(jt->second));
    t_B.push_back(st_B.at(jt->second));
    t_chi2.push_back(st_chi2.at(jt->second));
  }

  for (int nv=0; nv<6; nv++) {
    SlopeCand[nv]     = t_A[nv];
    InterceptCand[nv] = t_B[nv];
    Chi2Cand[nv]      = t_chi2[nv];
  }

   //    ... and finally with all the points
  *A = Abest;
  *B = Bbest;
  Circfit(Nmeas,XI,YI,RI,WI,IG,A,B,DAB,Chi2);

  if (*Chi2>=0.0) {
    Ntrue2 = Ntrue - 2;
    *Pchi2 = TMath::Prob(*Chi2, Ntrue2);
  }

  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

