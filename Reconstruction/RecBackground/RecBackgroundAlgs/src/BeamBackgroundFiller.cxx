/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundFiller.cxx 765686 2016-08-01 01:06:35Z ssnyder $
#include "RecBackgroundAlgs/BeamBackgroundFiller.h"
#include "AthenaKernel/errorcheck.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODJet/JetConstituentVector.h"
#include "CaloGeoHelpers/CaloSampling.h"

//------------------------------------------------------------------------------
BeamBackgroundFiller::BeamBackgroundFiller(const std::string& name,
                                 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_numSegment(0),
  m_numSegmentEarly(0),
  m_numSegmentACNoTime(0),
  m_numSegmentAC(0),
  m_numMatched(0),
  m_numNoTimeLoose(0),
  m_numNoTimeMedium(0),
  m_numNoTimeTight(0),
  m_numOneSidedLoose(0),
  m_numOneSidedMedium(0),
  m_numOneSidedTight(0),
  m_numTwoSidedNoTime(0),
  m_numTwoSided(0),
  m_numClusterShape(0),
  m_numJet(0),
  m_direction(0),
  m_helperTool("Muon::MuonEDMHelperTool"),
  m_idHelperTool("Muon::MuonIdHelperTool"),
  m_idToFixedIdTool("MuonCalib::IdToFixedIdTool")
{

  declareProperty("doMuonBoyCSCTiming", m_doMuonBoyCSCTiming = true);

  declareProperty("cutThetaCsc" , m_cutThetaCsc  = 5. );
  declareProperty("cutThetaMdtI", m_cutThetaMdtI = 10.);
  declareProperty("cutPhi" , m_cutPhiSeg  = 4.);
  declareProperty("cutPhiCsc" , m_cutPhiCsc  = 4.);
  declareProperty("cutPhiMdtI", m_cutPhiMdtI = 4.);
  declareProperty("cutRadiusCsc" , m_cutRadiusCsc  = 300.);
  declareProperty("cutRadiusMdtI", m_cutRadiusMdtI = 800.);

  declareProperty("cutEnergy", m_cutEnergy = 10000.);

  // CSC :  881 < R < 2081
  // LAr barrel :  1500 < R < 1970
  // TileCal :  2280 < R < 4250
  declareProperty("cutRadiusLow" , m_cutRadiusLow  = 881. );
  declareProperty("cutRadiusHigh", m_cutRadiusHigh = 4250.);

  declareProperty("cutMuonTime", m_cutMuonTime = 25.);
  declareProperty("cutClusTime", m_cutClusTime = 2.5);

  declareProperty("cutTimeDiffAC", m_cutTimeDiffAC = 25.);

  declareProperty("cutDrdz", m_cutDrdz = .15);

}


//------------------------------------------------------------------------------
BeamBackgroundFiller::~BeamBackgroundFiller()
{ 
}


//------------------------------------------------------------------------------
StatusCode BeamBackgroundFiller::initialize() {
  CHECK( m_helperTool.retrieve() );
  CHECK( m_idHelperTool.retrieve() );
  CHECK( m_idToFixedIdTool.retrieve() );

  ATH_CHECK(m_cscSegmentContainerReadHandleKey.initialize());
  ATH_CHECK(m_mdtSegmentContainerReadHandleKey.initialize());
  ATH_CHECK(m_caloClusterContainerReadHandleKey.initialize());
  ATH_CHECK(m_jetContainerReadHandleKey.initialize());

  ATH_CHECK(m_beamBackgroundDataWriteHandleKey.initialize());
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode BeamBackgroundFiller::execute() {

  // find muon segments from beam background muon candidates and match them with calorimeter clusters
  FillMatchMatrix();

  // apply Beam Background Identifiaction Methods
  SegmentMethod();
  OneSidedMethod();
  TwoSidedMethod();
  ClusterShapeMethod();

  // identify fake jets
  FindFakeJets();

  // fill the results into BeamBackgroundData
  SG::WriteHandle<BeamBackgroundData> beamBackgroundDataWriteHandle(m_beamBackgroundDataWriteHandleKey);
  ATH_CHECK(beamBackgroundDataWriteHandle.record(std::make_unique<BeamBackgroundData>()));
  FillBeamBackgroundData(beamBackgroundDataWriteHandle);


  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
/**
 * This function selects the muon segments with the direction parallel to the beam pipe and
 * calorimeter clusters above certain energy threshold.
 * Matching matrix is created to store the results of beam background identification
 * for each cluster and segment
 */
void BeamBackgroundFiller::FillMatchMatrix()
{
  m_numMatched = 0;

  m_indexSeg.clear();
  m_resultSeg.clear();
  m_indexClus.clear();
  m_matchMatrix.clear();
  m_resultClus.clear();

  // select only the CSC segments with the global direction parallel to the beam pipe
  SG::ReadHandle<Trk::SegmentCollection> cscSegmentReadHandle(m_cscSegmentContainerReadHandleKey);
  
  if (!cscSegmentReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to Trk::SegmentCollection with name: " << m_cscSegmentContainerReadHandleKey);
  else ATH_MSG_DEBUG(m_cscSegmentContainerReadHandleKey << " retrieved from StoreGate");
  
  unsigned int cscSegmentCounter = 0;
  for (auto thisCSCSegment : *cscSegmentReadHandle){
    cscSegmentCounter++;
    const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*>(thisCSCSegment);

    if (!seg) std::abort();

    Identifier id = m_helperTool->chamberId(*seg);
    if ( !id.is_valid() ) continue;
    if ( !m_idHelperTool->isMuon(id) ) continue;

    if ( !m_idHelperTool->isCsc(id) ) continue;

    const Amg::Vector3D& globalPos = seg->globalPosition();
    const Amg::Vector3D& globalDir = seg->globalDirection();
    double thetaPos = globalPos.theta();
    double thetaDir = globalDir.theta();

    double d2r = TMath::Pi()/180.;
    if( TMath::Cos(2.*(thetaPos-thetaDir)) > TMath::Cos(2.*m_cutThetaCsc*d2r) ) continue;
    
    ElementLink<Trk::SegmentCollection> segLink;
    segLink.toIndexedElement(*cscSegmentReadHandle, cscSegmentCounter-1);
    m_indexSeg.push_back(segLink);
  }
  
  // select only the MDT segments with the global direction parallel to the beam pipe
  SG::ReadHandle<Trk::SegmentCollection> mdtSegmentReadHandle(m_mdtSegmentContainerReadHandleKey);
  
  if (!mdtSegmentReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to Trk::SegmentCollection with name: " << m_mdtSegmentContainerReadHandleKey);
  else ATH_MSG_DEBUG(m_mdtSegmentContainerReadHandleKey << " retrieved from StoreGate");

  unsigned int mdtSegmentCounter = 0;
  for (auto thisMDTSegment : *mdtSegmentReadHandle){
    mdtSegmentCounter++;
    const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*>(thisMDTSegment);
    if (!seg) std::abort();

    Identifier id = m_helperTool->chamberId(*seg);
    if ( !id.is_valid() ) continue;  
    if ( !m_idHelperTool->isMuon(id) ) continue;

    Muon::MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(id);

    if(!( chIndex == Muon::MuonStationIndex::EIL || chIndex == Muon::MuonStationIndex::EIS )) continue;

    const Amg::Vector3D& globalPos = seg->globalPosition();
    const Amg::Vector3D& globalDir = seg->globalDirection();
    double thetaPos = globalPos.theta();
    double thetaDir = globalDir.theta();

    double d2r = M_PI/180.;
    if( TMath::Cos(2.*(thetaPos-thetaDir)) > TMath::Cos(2.*m_cutThetaMdtI*d2r) ) continue;
    
    ElementLink<Trk::SegmentCollection> segLink;
    segLink.toIndexedElement(*mdtSegmentReadHandle, mdtSegmentCounter-1);
    m_indexSeg.push_back(segLink);

  }
  
  m_resultSeg.assign(m_indexSeg.size(), int(0));


  // find matching clusters
  SG::ReadHandle<xAOD::CaloClusterContainer> caloClusterContainerReadHandle(m_caloClusterContainerReadHandleKey);

  if (!caloClusterContainerReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to CaloClusterContainer with name: " << m_caloClusterContainerReadHandleKey);
  else ATH_MSG_DEBUG(m_caloClusterContainerReadHandleKey << " retrieved from StoreGate");

  unsigned int caloClusterCounter = 0;
  for (auto thisCaloCluster : *caloClusterContainerReadHandle){
    caloClusterCounter++;
    
    double eEmClus =
      thisCaloCluster->eSample(CaloSampling::CaloSample::PreSamplerB) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::EMB1) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::EMB2) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::EMB3) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::PreSamplerE) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::EME1) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::EME2) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::EME3) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::FCAL0);
    double eHadClus =
      thisCaloCluster->eSample(CaloSampling::CaloSample::HEC0) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::HEC1) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::HEC2) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::HEC3) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileBar0) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileBar1) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileBar2) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileGap1) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileGap2) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileGap3) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileExt0) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileExt1) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::TileExt2) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::FCAL1) +
      thisCaloCluster->eSample(CaloSampling::CaloSample::FCAL2);
    double eClus = eEmClus + eHadClus;

    // ignore low energy clusters
    if( eClus < m_cutEnergy ) continue;

    double rClus(0.);
    if (!thisCaloCluster->retrieveMoment(xAOD::CaloCluster_v1::CENTER_MAG,rClus))
      rClus = 0;
    rClus = rClus/cosh(thisCaloCluster->eta());
    
    double phiClus = thisCaloCluster->phi();

    // remove clusters at low radius (outside the CSC acceptance)
    if( rClus < m_cutRadiusLow ) continue;
    if( rClus > m_cutRadiusHigh ) continue;


    std::vector<int> matchedSegmentsPerCluster;
    matchedSegmentsPerCluster.assign(m_indexSeg.size(), int(0));
    bool matched = false;
    
    for(unsigned int j=0; j<m_indexSeg.size(); j++) {
      const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*> (*m_indexSeg[j]);
      if (!seg) std::abort();

      Identifier id = m_helperTool->chamberId(*seg);
      bool isCsc = m_idHelperTool->isCsc(id);

      const Amg::Vector3D& globalPos = seg->globalPosition();
      double phiSeg = globalPos.phi();
      double rSeg = globalPos.perp();

      // match in phi
      double d2r = M_PI/180.;
      if( TMath::Cos(phiClus-phiSeg) < TMath::Cos(m_cutPhiCsc*d2r) && isCsc ) continue;
      if( TMath::Cos(phiClus-phiSeg) < TMath::Cos(m_cutPhiMdtI*d2r) && !isCsc ) continue;

      // match in radius
      if( TMath::Abs(rClus-rSeg) > m_cutRadiusCsc && isCsc ) continue;
      if( TMath::Abs(rClus-rSeg) > m_cutRadiusMdtI && !isCsc ) continue;

      matchedSegmentsPerCluster[j] = 1;
      matched = true;
      m_resultSeg[j] = m_resultSeg[j] | BeamBackgroundData::Matched;
    }

    if(!matched) continue;

    ElementLink<xAOD::CaloClusterContainer> clusLink;
    clusLink.toIndexedElement(*caloClusterContainerReadHandle, caloClusterCounter-1);
    m_indexClus.push_back(clusLink);
    m_matchMatrix.push_back(matchedSegmentsPerCluster);
    m_numMatched++;
    
  }

  m_resultClus.assign(m_indexClus.size(), int(1));
}


//------------------------------------------------------------------------------
/**
 * This function looks at the segments found by the FillMatchMatrix function.
 * The following results are set:
 * - number of segments with the direciton parallel to the beam pipe
 *   (all the segments in the matching matrix)
 * - number of such segments with early time
 * - number of segment pairs on side A and C
 * - number of segment pairs on side A and C with the corresponding time difference
 */
void BeamBackgroundFiller::SegmentMethod()
{
  m_numSegment = 0;
  m_numSegmentEarly = 0;
  m_numSegmentACNoTime = 0;
  m_numSegmentAC = 0;


  for(unsigned int segIndex=0; segIndex<m_indexSeg.size(); segIndex++) {

    const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*> (*m_indexSeg[segIndex]);
    if (!seg) std::abort();

    const Amg::Vector3D&  globalPos = seg->globalPosition();
    double zSeg = globalPos.z();

    double tSeg = seg->time();

    m_numSegment++;
    m_resultSeg[segIndex] = m_resultSeg[segIndex] | BeamBackgroundData::Segment;

    // muon segment: in-time (1), early (2), ambiguous (0)
    int timeStatus = 0;
    double inTime = -(-TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
    double early  = -( TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
    if( TMath::Abs(tSeg - inTime) < m_cutMuonTime ) timeStatus = 1;
    if( TMath::Abs(tSeg - early ) < m_cutMuonTime ) timeStatus = 2;
/*
    // also take the CSC segments that fall outside of the time readout window on the early side
    // CscTimeStatus::CscTimeEarly could be used here
    Identifier id = m_helperTool->chamberId(*seg);
    MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( id ) ;
    int stationName = fid.stationName();
    if( tSeg < early && (stationName==33 || stationName==34) ) timeStatus = 2;
*/
    if( timeStatus == 2 ) {
      m_numSegmentEarly++;
      m_resultSeg[segIndex] = m_resultSeg[segIndex] | BeamBackgroundData::SegmentEarly;
    }


    // take only the segments on side A (z > 0)
    if( zSeg < 0. ) continue;

    unsigned int segIndexA = segIndex;

    double tSegA = tSeg;
    double timeStatusA = timeStatus;

    double phiSegA = globalPos.phi();


    for(unsigned int segIndexC=0; segIndexC<m_indexSeg.size(); segIndexC++) {

      const Muon::MuonSegment* segC = dynamic_cast<const Muon::MuonSegment*> (*m_indexSeg[segIndexC]);
      if (!segC) std::abort();

      const Amg::Vector3D&  globalPos = segC->globalPosition();
      double zSegC = globalPos.z();

      double tSegC = seg->time();

      // take only the segments on side C (z < 0)
      if( zSegC > 0. ) continue;

      // muon segment: in-time (1), early (2), ambiguous (0)
      int timeStatusC = 0;
      double inTime = -(-TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
      double early  = -( TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
      if( TMath::Abs(tSegC - inTime) < m_cutMuonTime ) timeStatusC = 1;
      if( TMath::Abs(tSegC - early ) < m_cutMuonTime ) timeStatusC = 2;
/*
      // also take the CSC segments that fall outside of the time readout window on the early side
      // CscTimeStatus::CscTimeEarly could be used here
      Identifier id = m_helperTool->chamberId(*seg);
      MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( id ) ;
      int stationName = fid.stationName();
      if( tSegC < early && (stationName==33 || stationName==34) ) timeStatus = 2;
*/
      double phiSegC = globalPos.phi();

      // match in phi
      double d2r = M_PI/180.;
      if( TMath::Cos(phiSegA-phiSegC) < TMath::Cos(m_cutPhiSeg*d2r) ) continue;


      m_numSegmentACNoTime++;
      m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | BeamBackgroundData::SegmentACNoTime;
      m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | BeamBackgroundData::SegmentACNoTime;


      if( timeStatusA == 0 || timeStatusC == 0 ) continue;

      // check the time difference
      if( TMath::Abs(tSegA - tSegC) > m_cutTimeDiffAC ) {
        m_numSegmentAC++;
        m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | BeamBackgroundData::SegmentAC;
        m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | BeamBackgroundData::SegmentAC;
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * This function is the implementation of the "No-Time Method" and the "One-Sided Method".
 * The "One-Sided Method" compares the time of the calorimeter cluster with the expected time.
 * The expected time is calculated based on the direction of the beam background muon
 * which is reconstructed from the position and time of the muon segment.
 * The "No-Time Method" does not use the time information of the muon segment
 * thus the direction of the beam background muon is not known.
 * Therefore, the cluster time is compared to two expected time values
 * (corresponding to both A->C and C->A directions).
 */
void BeamBackgroundFiller::OneSidedMethod()
{
  m_numNoTimeLoose = 0;
  m_numNoTimeMedium = 0;
  m_numNoTimeTight = 0;

  m_numOneSidedLoose = 0;
  m_numOneSidedMedium = 0;
  m_numOneSidedTight = 0;


  for(unsigned int clusIndex=0; clusIndex<m_indexClus.size(); clusIndex++) {

    const xAOD::CaloCluster* clus = *m_indexClus[clusIndex];

    double rClus(0.);
    if (!clus->retrieveMoment(xAOD::CaloCluster_v1::CENTER_MAG,rClus))
      rClus = 0;
    rClus = rClus/cosh(clus->eta());
    double zClus = rClus * sinh(clus->eta());
    double tClus = clus->time();

    // calculate expected cluster time
    double expectedClusterTimeAC = -( zClus + TMath::Sqrt(rClus*rClus + zClus*zClus))*(1e-3/3e8/1e-9);
    double expectedClusterTimeCA = -(-zClus + TMath::Sqrt(rClus*rClus + zClus*zClus))*(1e-3/3e8/1e-9);

    for(unsigned int segIndex=0; segIndex<m_indexSeg.size(); segIndex++) {

      if( !(m_matchMatrix[clusIndex][segIndex]&1) ) continue;

      const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*> (*m_indexSeg[segIndex]);
      if (!seg) std::abort();

      const Amg::Vector3D&  globalPos = seg->globalPosition();
      double zSeg = globalPos.z();

      double tSeg = seg->time();

      // muon segment: in-time (1), early (2), ambiguous (0)
      int timeStatus = 0;
      double inTime = -(-TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
      double early  = -( TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
      if( TMath::Abs(tSeg - inTime) < m_cutMuonTime ) timeStatus = 1;
      if( TMath::Abs(tSeg - early ) < m_cutMuonTime ) timeStatus = 2;
/*
      // also take the CSC segments that fall outside of the time readout window on the early side
      // CscTimeStatus::CscTimeEarly could be used here
      Identifier id = m_helperTool->chamberId(*seg);
      MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( id ) ;
      int stationName = fid.stationName();
      if( tSeg < early && (stationName==33 || stationName==34) ) timeStatus = 2;
*/
      // reconstruct beam background direction: A->C (1), C->A (-1)
      int direction = 0;
      if( (zSeg>0 && timeStatus==2) || (zSeg<0 && timeStatus==1) ) direction = 1;
      if( (zSeg>0 && timeStatus==1) || (zSeg<0 && timeStatus==2) ) direction = -1;


      // check the cluster time without the beam background direction information
      if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime ||
          TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime ) {
        m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::NoTimeLoose;
      }
      if( (TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > m_cutClusTime) ||
          (TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > m_cutClusTime) ) {
        m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::NoTimeMedium;
      }
      if( (TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > 2.*m_cutClusTime) ||
          (TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > 2.*m_cutClusTime) ) {
        m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::NoTimeTight;
      }

      // check the cluster time with the beam background direction information
      if( direction == 1 ) {
        if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedLoose;
        }
        if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedMedium;
        }
        if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > 2.*m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedTight;
        }
      } else
      if( direction == -1 ) {
        if( TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedLoose;
        }
        if( TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedMedium;
        }
        if( TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > 2.*m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedTight;
        }
      }

      m_resultClus[clusIndex] = m_resultClus[clusIndex] | m_matchMatrix[clusIndex][segIndex];
      m_resultSeg[segIndex] = m_resultSeg[segIndex] | m_matchMatrix[clusIndex][segIndex];
    }

    if(m_resultClus[clusIndex] & BeamBackgroundData::NoTimeLoose ) m_numNoTimeLoose++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::NoTimeMedium) m_numNoTimeMedium++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::NoTimeTight ) m_numNoTimeTight++;

    if(m_resultClus[clusIndex] & BeamBackgroundData::OneSidedLoose ) m_numOneSidedLoose++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::OneSidedMedium) m_numOneSidedMedium++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::OneSidedTight ) m_numOneSidedTight++;
  }
}


//------------------------------------------------------------------------------
/**
 * This function is the implementation of the "Two-Sided No-Time Method" and
 * the "Two-Sided Method" that looks at the clusters matched with
 * at least one muon segment on side A and one muon segment on side C.
 * In case of the "Two-Sided Method",
 * corresponding time difference of the muon segments is required
 * and the direction of the beam background muon is also stored.
 */
void BeamBackgroundFiller::TwoSidedMethod()
{
  m_numTwoSidedNoTime = 0;
  m_numTwoSided = 0;
  m_direction = 0;

  for(unsigned int clusIndex=0; clusIndex<m_indexClus.size(); clusIndex++) {

    for(unsigned int segIndexA=0; segIndexA<m_indexSeg.size(); segIndexA++) {

      if( !(m_matchMatrix[clusIndex][segIndexA]&1) ) continue;

      const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*> (*m_indexSeg[segIndexA]);
      if (!seg) std::abort();

      const Amg::Vector3D&  globalPos = seg->globalPosition();
      double zSegA = globalPos.z();

      double tSegA = seg->time();


      // muon segment: in-time (1), early (2), ambiguous (0)
      int timeStatusA = 0;
      double inTime = -(-TMath::Abs(zSegA) + globalPos.mag())*(1e-3/3e8/1e-9);
      double early  = -( TMath::Abs(zSegA) + globalPos.mag())*(1e-3/3e8/1e-9);
      if( TMath::Abs(tSegA - inTime) < m_cutMuonTime ) timeStatusA = 1;
      if( TMath::Abs(tSegA - early ) < m_cutMuonTime ) timeStatusA = 2;
/*
      // also take the CSC segments that fall outside of the time readout window on the early side
      // CscTimeStatus::CscTimeEarly could be used here
      Identifier id = m_helperTool->chamberId(*seg);
      MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( id ) ;
      int stationName = fid.stationName();
      if( tSegA < early && (stationName==33 || stationName==34) ) timeStatusA = 2;
*/
      // take only the segments on side A (z > 0)
      if( zSegA < 0. ) continue;

      for(unsigned int segIndexC=0; segIndexC<m_indexSeg.size(); segIndexC++) {

        if( !(m_matchMatrix[clusIndex][segIndexC]&1) ) continue;

        const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*> (*m_indexSeg[segIndexC]);
        if (!seg) std::abort();

        const Amg::Vector3D&  globalPos = seg->globalPosition();
        double zSegC = globalPos.z();

        double tSegC = seg->time();


        // muon segment: in-time (1), early (2), ambiguous (0)
        int timeStatusC = 0;
        double inTime = -(-TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
        double early  = -( TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
        if( TMath::Abs(tSegC - inTime) < m_cutMuonTime ) timeStatusC = 1;
        if( TMath::Abs(tSegC - early ) < m_cutMuonTime ) timeStatusC = 2;
/*
        // also take the CSC segments that fall outside of the time readout window on the early side
        // CscTimeStatus::CscTimeEarly could be used here
        Identifier id = m_helperTool->chamberId(*seg);
        MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( id ) ;
        int stationName = fid.stationName();
        if( tSegC < early && (stationName==33 || stationName==34) ) timeStatusC = 2;
*/
        // take only the segments on side C (z < 0)
        if( zSegC > 0. ) continue;


        m_matchMatrix[clusIndex][segIndexA] = m_matchMatrix[clusIndex][segIndexA] | BeamBackgroundData::TwoSidedNoTime;
        m_matchMatrix[clusIndex][segIndexC] = m_matchMatrix[clusIndex][segIndexC] | BeamBackgroundData::TwoSidedNoTime;
        m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | m_matchMatrix[clusIndex][segIndexA];
        m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | m_matchMatrix[clusIndex][segIndexC];

  
        if( timeStatusA == 0 || timeStatusC == 0 ) continue;

        // check the time difference
        if( TMath::Abs(tSegA - tSegC) > m_cutTimeDiffAC ) {
          m_matchMatrix[clusIndex][segIndexA] = m_matchMatrix[clusIndex][segIndexA] | BeamBackgroundData::TwoSided;
          m_matchMatrix[clusIndex][segIndexC] = m_matchMatrix[clusIndex][segIndexC] | BeamBackgroundData::TwoSided;
          m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | m_matchMatrix[clusIndex][segIndexA];
          m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | m_matchMatrix[clusIndex][segIndexC];

          // direction of beam background
          if( timeStatusA == 2 ) m_direction++;  // A->C
          if( timeStatusC == 2 ) m_direction--;  // C->A
        }

      }

      m_resultClus[clusIndex] = m_resultClus[clusIndex] | m_matchMatrix[clusIndex][segIndexA];
    }

    if(m_resultClus[clusIndex] & BeamBackgroundData::TwoSidedNoTime) m_numTwoSidedNoTime++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::TwoSided) m_numTwoSided++;
  }
}


//------------------------------------------------------------------------------
/**
 * This function is the implementation of the "Cluster-Shape Method".
 * The shape of the cluster is described by the variable dr/dz
 * which is the ratio of the standard deviation of the radial position of the contianed cells
 * and the standard deviation of the z-position of the contained cells.
 * Only the clusters matched with muon segments are checked.
 */
void BeamBackgroundFiller::ClusterShapeMethod()
{
  m_numClusterShape = 0;

  m_drdzClus.clear();

  for(unsigned int clusIndex=0; clusIndex<m_indexClus.size(); clusIndex++) {

    const xAOD::CaloCluster* clus = *m_indexClus[clusIndex];

    double rClus(0.);
    if (!clus->retrieveMoment(xAOD::CaloCluster_v1::CENTER_MAG,rClus))
      rClus = 0;
    rClus = rClus/cosh(clus->eta());
    double zClus = rClus * sinh(clus->eta());


    // calculate dr/dz
    double dr = 0.;
    double dz = 0.;
    double drdz = -1.;
    int nCell = 0;

    if( clus->getCellLinks()!=0 ) {
      xAOD::CaloCluster::const_cell_iterator firstCell = clus->cell_begin();
      xAOD::CaloCluster::const_cell_iterator lastCell  = clus->cell_end();

      for(; firstCell != lastCell; ++firstCell) {
        const CaloCell* cell = *firstCell;

        if( cell->time() == 0. ) continue;
        if( cell->energy() < 100. ) continue;
        nCell++;


        //double rCell = sqrt(cell->x()*cell->x() + cell->y()*cell->y());
        //double zCell = cell->z();
	const CaloDetDescrElement* dde=cell->caloDDE();
	const double rCell=dde->r();
	const double zCell=dde->z();
        dr = dr + (rCell-rClus)*(rCell-rClus);
        dz = dz + (zCell-zClus)*(zCell-zClus);
      }
    }

    if(nCell) {
      dr = sqrt(dr / nCell);
      dz = sqrt(dz / nCell);
      if( dz > 0. ) drdz = dr / dz;
    }

    m_drdzClus.push_back(drdz);


    // check dr/dz
    if( drdz < 0. ) continue;
    if( drdz < m_cutDrdz ) {
      for(unsigned int segIndex=0; segIndex<m_indexSeg.size(); segIndex++) {
        if( !(m_matchMatrix[clusIndex][segIndex]&1) ) continue;
        m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::ClusterShape;
        m_resultSeg[segIndex] = m_resultSeg[segIndex] | m_matchMatrix[clusIndex][segIndex];
      }
      m_resultClus[clusIndex] = m_resultClus[clusIndex] | BeamBackgroundData::ClusterShape;
      m_numClusterShape++;
    } 
  }
}


//------------------------------------------------------------------------------
/**
 * This function checks whether the matched clusters are contained in any jets.
 * If yes, the jet is marked as fake jet and the corresponding result
 * of the Beam Background Identification Method is stored
 * (using the OR condition if more than one cluster is found within one jet)
 */
void BeamBackgroundFiller::FindFakeJets()
{
  m_numJet = 0;

  m_indexJet.clear();
  m_resultJet.clear();

  // find the jet that contains this cluster
  SG::ReadHandle<xAOD::JetContainer> jetContainerReadHandle(m_jetContainerReadHandleKey);

  if(!jetContainerReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to JetContainer with name: " << m_jetContainerReadHandleKey);
  else ATH_MSG_DEBUG(m_jetContainerReadHandleKey << " retrieved from StoreGate");

  unsigned int jetCounter = 0;
  for (auto thisJet : *jetContainerReadHandle){
    bool isFakeJet = false;
    int resultJet = 0;

    xAOD::JetConstituentVector vec = thisJet->getConstituents();
    xAOD::JetConstituentVector::iterator constIt = vec.begin();
    xAOD::JetConstituentVector::iterator constItE = vec.end();

    for(; constIt != constItE; ++constIt) {
      if(constIt->type()!=xAOD::Type::CaloCluster)
	continue;
      const xAOD::CaloCluster* jetConst = dynamic_cast<const xAOD::CaloCluster*>(constIt->rawConstituent());

      for(unsigned int clusIndex=0; clusIndex<m_indexClus.size(); clusIndex++) {
        const xAOD::CaloCluster* clus = *m_indexClus[clusIndex];

        if( jetConst == clus ) {
          isFakeJet = true;
          resultJet = resultJet | m_resultClus[clusIndex];
        }
      }
    }

    if( isFakeJet ) {
      ElementLink<xAOD::JetContainer> jetLink;
      jetLink.toIndexedElement(*jetContainerReadHandle, jetCounter);
      m_indexJet.push_back(jetLink);
      m_resultJet.push_back(resultJet);
      m_numJet++;
    }
    jetCounter++;
  }
}


//------------------------------------------------------------------------------
/**
 * This function stores all the results in BeamBackgroundData
 */
void BeamBackgroundFiller::FillBeamBackgroundData(SG::WriteHandle<BeamBackgroundData>& beamBackgroundDataWriteHandle)
{

  beamBackgroundDataWriteHandle->SetNumSegment(m_numSegment);
  beamBackgroundDataWriteHandle->SetNumSegmentEarly(m_numSegmentEarly);
  beamBackgroundDataWriteHandle->SetNumSegmentACNoTime(m_numSegmentACNoTime);
  beamBackgroundDataWriteHandle->SetNumSegmentAC(m_numSegmentAC);
  beamBackgroundDataWriteHandle->SetNumMatched(m_numMatched);
  beamBackgroundDataWriteHandle->SetNumNoTimeLoose(m_numNoTimeLoose);
  beamBackgroundDataWriteHandle->SetNumNoTimeMedium(m_numNoTimeMedium);
  beamBackgroundDataWriteHandle->SetNumNoTimeTight(m_numNoTimeTight);
  beamBackgroundDataWriteHandle->SetNumOneSidedLoose(m_numOneSidedLoose);
  beamBackgroundDataWriteHandle->SetNumOneSidedMedium(m_numOneSidedMedium);
  beamBackgroundDataWriteHandle->SetNumOneSidedTight(m_numOneSidedTight);
  beamBackgroundDataWriteHandle->SetNumTwoSidedNoTime(m_numTwoSidedNoTime);
  beamBackgroundDataWriteHandle->SetNumTwoSided(m_numTwoSided);
  beamBackgroundDataWriteHandle->SetNumClusterShape(m_numClusterShape);
  beamBackgroundDataWriteHandle->SetNumJet(m_numJet);

  int decision = 0;
  for(unsigned int i=0; i<m_indexSeg.size(); i++) {
    decision = decision | m_resultSeg[i];
  }
  for(unsigned int i=0; i<m_indexClus.size(); i++) {
    decision = decision | m_resultClus[i];
  }
  beamBackgroundDataWriteHandle->SetDecision(decision);

  beamBackgroundDataWriteHandle->SetDirection(m_direction);

  beamBackgroundDataWriteHandle->FillIndexSeg(m_indexSeg);
  beamBackgroundDataWriteHandle->FillResultSeg(&m_resultSeg);
  beamBackgroundDataWriteHandle->FillIndexClus(m_indexClus);
  beamBackgroundDataWriteHandle->FillMatchMatrix(&m_matchMatrix);

  beamBackgroundDataWriteHandle->FillResultClus(&m_resultClus);
  beamBackgroundDataWriteHandle->FillIndexJet(m_indexJet);
  beamBackgroundDataWriteHandle->FillDrdzClus(&m_drdzClus);

  beamBackgroundDataWriteHandle->FillIndexJet(m_indexJet);
  beamBackgroundDataWriteHandle->FillResultJet(&m_resultJet);


  ATH_MSG_DEBUG( "parallel segments " <<
		 m_numSegment << " " <<
		 m_numSegmentEarly << " " <<
		 m_numSegmentACNoTime << " " <<
		 m_numSegmentAC);

  ATH_MSG_DEBUG( "matched clusters " <<
		 m_numMatched << " " <<
		 m_numNoTimeLoose << " " <<
		 m_numNoTimeMedium << " " <<
		 m_numNoTimeTight << " " <<
		 m_numOneSidedLoose << " " <<
		 m_numOneSidedMedium << " " <<
		 m_numOneSidedTight << " " <<
		 m_numTwoSidedNoTime << " " <<
		 m_numTwoSided << " " <<
		 m_numClusterShape);
}


//------------------------------------------------------------------------------
StatusCode BeamBackgroundFiller::finalize() {
  return StatusCode::SUCCESS;
}
