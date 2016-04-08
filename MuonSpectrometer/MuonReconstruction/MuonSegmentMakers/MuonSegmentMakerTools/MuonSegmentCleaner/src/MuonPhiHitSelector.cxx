/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentCleaner/MuonPhiHitSelector.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <Eigen/LU>

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h" 
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

#include "MuonPrepRawData/MuonCluster.h"

#include "TrkSurfaces/PlaneSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
//#include "TrkParameters/Perigee.h"

#include "CxxUtils/sincos.h"

MuonPhiHitSelector::MuonPhiHitSelector(const std::string& type,const std::string& name,const IInterface* parent):AthAlgTool(type,name,parent),m_competingRIOsOnTrackTool ("Muon::MuonCompetingClustersOnTrackCreator/MuonCompetingClustersOnTrackCreator"),m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),m_phi(0)
				      //,m_cosmics(false),m_debug(false),m_summary(false) 
{
  declareInterface<IMuonHitSelector>(this);

  m_cosmics = false;
  declareProperty("DoCosmics",m_cosmics);

  m_debug = false; 
  declareProperty("DoDebug",m_debug);

  m_summary = false;  
  declareProperty("DoSummary",m_summary);

  // Create and write out the phi clusters or hits

  m_makeClusters = false;  
  declareProperty("MakeClusters",m_makeClusters);
  m_competingRios = false; ;  
  declareProperty("CompetingRios",m_competingRios);
}

MuonPhiHitSelector::~MuonPhiHitSelector()
{
}

StatusCode MuonPhiHitSelector::initialize()
{
  ATH_MSG_VERBOSE(" MuonPhiHitSelector::Initializing ");
 
  ATH_CHECK( m_competingRIOsOnTrackTool.retrieve() );

  ATH_CHECK( m_clusterCreator.retrieve() );

  StatusCode sc = detStore()->retrieve( m_detMgr );
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR(" Cannot retrieve MuonDetDescrMgr ");
  } else {
    m_mdtIdHelper = m_detMgr->mdtIdHelper();
    m_cscIdHelper = m_detMgr->cscIdHelper();
    m_rpcIdHelper = m_detMgr->rpcIdHelper();
    m_tgcIdHelper = m_detMgr->tgcIdHelper();
    ATH_MSG_INFO(" Retrieved IdHelpers: (mdt, csc, rpc and tgc) ");
  }

  ATH_MSG_VERBOSE("End of Initializing");

  return StatusCode::SUCCESS;
}

StatusCode MuonPhiHitSelector::finalize()
{
  return StatusCode::SUCCESS;
}

std::vector<const Trk::MeasurementBase*>* MuonPhiHitSelector::select_rio( const double pmom, const std::vector<const Trk::RIO_OnTrack* >& associatedHits, const std::vector<const Trk::PrepRawData*>& unassociatedHits) const
{
  // Make Rios on Track 

  int time_start = std::clock()/1000;  

  std::vector<const Trk::MeasurementBase*>* selectedHits = new std::vector<const Trk::MeasurementBase*>() ;
  std::vector<const Trk::MeasurementBase*>* selectedClusters = new std::vector<const Trk::MeasurementBase*>() ;

  ATH_MSG_VERBOSE(" Executing MuonPhiHitSelectorTool select_rio ");

  m_phi =0.; 
  int nhits = associatedHits.size() + unassociatedHits.size();

  if (m_debug) std::cout << " Executing MuonPhiHitSelectorTool nhits select_rio " << nhits << std::endl;  

  std::vector<double> phiHitx(nhits);
  std::vector<double> phiHity(nhits);
  std::vector<double> phiHitz(nhits);
  std::vector<double> phiError(nhits);
  std::vector<Identifier> phiId(nhits);
  std::vector<double> phiPull(nhits);
  std::vector<int> phiSelect(nhits);
  std::vector<int> phiMult(nhits);
  std::vector<int> quality(nhits);
  std::vector<const Trk::PrepRawData*> phiPrep(nhits);

  std::map<Identifier,int> phiMapId;
  int nphi = 0;
  std::vector<const Trk::RIO_OnTrack* >::const_iterator it = associatedHits.begin();
  std::vector<const Trk::RIO_OnTrack* >::const_iterator it_end = associatedHits.end();

  for(; it != it_end ; ++it )  {
    const Trk::PrepRawData* prd = (*it)->prepRawData();
    Identifier id = prd->identify(); 
    phiId[nphi] = id;  
    Amg::Vector3D gHitPos = (*it)->globalPosition();
    // RPC code 
    if (m_rpcIdHelper->is_rpc(id) ) {
      //      const Muon::RpcClusterOnTrack* crot = dynamic_cast<const Muon::RpcClusterOnTrack*>(*it);
      //      if( !crot ) { 
      //	ATH_MSG_WARNING("This is not a RpcClusterOnTrack!!! ");
      //	continue;
      //      }
      //      gHitPos = crot->globalPosition();
      phiSelect[nphi] = 1;
    }

    // TGC code 
    else if (m_tgcIdHelper->is_tgc(id)) {
      //      const Muon::TgcClusterOnTrack* crot = dynamic_cast<const Muon::TgcClusterOnTrack*>(*it);
      //      if( !crot ){
      //	ATH_MSG_WARNING("This is not a TgcClusterOnTrack!!! ");
      //	continue;
      //      }
      //      gHitPos = crot->globalPosition();
      phiSelect[nphi] = 2;
    }
    // CSC code
    else if (m_cscIdHelper->is_csc(id)) {
      //      const Muon::CscClusterOnTrack* crot = dynamic_cast<const Muon::CscClusterOnTrack*>(*it);
      //      if( !crot ){
      //	ATH_MSG_WARNING("This is not a CscClusterOnTrack!!! ");
      //	continue;
      //      }
      //      gHitPos = crot->globalPosition();
      phiSelect[nphi] = 3;
    }
    phiHitx[nphi] = gHitPos.x(); 
    phiHity[nphi] = gHitPos.y(); 
    phiHitz[nphi] = gHitPos.z();  
    
    //double error = (*it)->localErrorMatrix().covariance().fast(1,1);
    const Amg::MatrixX& cov = (*it)->localCovariance();
    double error = cov(0,0);
    
    // for the TGCs diagonalize the error matrix and use the smallest component
    if( cov.cols() > 1 ){
      AmgMatrix(2,2) Er;
      Er(0,0) = cov(0,0);
      Er(0,1) = cov(0,1);
      Er(1,1) = cov(1,1);
      Er(1,0) = Er(0,1);   
     
      double chi = Er(0,0) != Er(1,1) ? atan(-2*Er(0,1)/(Er(0,0)-Er(1,1)))/2. : 0.;

      CxxUtils::sincos scchi(chi);

      AmgMatrix(2,2) Rot;
      Rot(0,0) = scchi.cs;
      Rot(1,1) = Rot(0,0);
      Rot(0,1) = scchi.sn;
      Rot(1,0) = -Rot(0,1);
      AmgMatrix(2,2) D = Rot.transpose()*Er*Rot;
      if (m_debug) std::cout << " Diagonalized error matrix " << D << std::endl; 
      error = D(0,0) < D(1,1) ? D(0,0) : D(1,1);
    }
    phiError[nphi] = sqrt(error);
    quality[nphi] = 1000;
    phiMapId[id] = 1;
    phiPrep[nphi] = prd;
    double phipos = atan2(phiHity[nphi],phiHitx[nphi]);
    if (m_debug) std::cout << " phi Segment Hit " << nphi << " det " << phiSelect[nphi] << " phi " << phipos << std::endl;
    nphi++;
  }
  int nphiseg = nphi;

  std::vector<const Trk::PrepRawData* >::const_iterator itu = unassociatedHits.begin();
  std::vector<const Trk::PrepRawData* >::const_iterator itu_end = unassociatedHits.end();

  for(; itu != itu_end ; ++itu )  {
    Identifier id = (*itu)->identify();
    phiId[nphi] = id;
    // Skip phi hits already on segment
    if (phiMapId.count(id) > 0) continue;
    const Muon::MuonCluster* clus = dynamic_cast<const Muon::MuonCluster*>(*itu);
    if( !clus ) continue;
    if (m_rpcIdHelper->is_rpc(id))  phiSelect[nphi] = 1;
    else if (m_tgcIdHelper->is_tgc(id))  phiSelect[nphi] = 2;
    else if (m_cscIdHelper->is_csc(id))  phiSelect[nphi] = 3;
    Amg::Vector3D gHitPos = clus->globalPosition();
    phiHitx[nphi] = gHitPos.x(); 
    phiHity[nphi] = gHitPos.y(); 
    phiHitz[nphi] = gHitPos.z();  
    phiError[nphi] = (*itu)->localCovariance()(Trk::locX);
    quality[nphi] = 10;
    phiPrep[nphi] = *itu;
    double phipos = atan2(phiHity[nphi],phiHitx[nphi]);
    if (m_debug) std::cout << " phi Pattern Hit " << nphi << " phi " << phipos << std::endl;
    nphi++;
  }

  double chi2,r0;
  int nfit;
  std::vector<double> errorM(4);

  fitRecPhi( pmom, phiId, phiHitx, phiHity, phiHitz, phiError, quality, nphi, phiPull, phiMult, phiSelect, chi2, r0, m_phi, errorM, nfit);
  
  // Define global track parameters (not used 27-8 JS)

  //   double aver_r = 0.;
  //   double aver_z = 0.;
  //   for(int i = 0; i < nphi  ; ++i )  {
  //     if (phiSelect[i]>0) {
  //       aver_r += sqrt(phiHitx[i]*phiHitx[i]+phiHity[i]*phiHity[i] );
  //       aver_z += phiHitz[i];
  //     }
  //   } 
  //   double avtheta = atan2(aver_r,aver_z);
  //   double sincosm_phi[2];
  //   sincos(m_phi,&sincosm_phi[0],&sincosm_phi[1]);
  //   double sincos_avtheta[2];
  //   sincos(avtheta,&sincos_avtheta[0],&sincos_avtheta[1]);
  
  //   const Trk::GlobalMomentum globPatDir = Trk::GlobalMomentum ( pmom*sincosm_phi[1]*sincos_avtheta[0], pmom*sincosm_phi[0]*sincos_avtheta[0], pmom*sincos_avtheta[1] );
  //   const Trk::GlobalPosition globPatPos = Trk::GlobalPosition (r0*sincosm_phi[0],-r0*sincosm_phi[1],0.);

  //  const Trk::Perigee perigee = Trk::Perigee (globPatPos, globPatDir, 1., Trk::GlobalPosition(0.,0.,0.)); 

  for(int i = 0; i < nphi  ; ++i )  {
    if (phiSelect[i]>0) { 
      //      std::list <const Trk::PrepRawData*> prdList;
      //      prdList.push_back(phiPrep[i]);
      //      const Trk::CompetingRIOsOnTrack*  rio = m_competingRIOsOnTrackTool->createBroadCluster(prdList,1.);
      //      if (m_debug) std::cout << " Make ONE competing rio/cluster per PrepData: number of rios " << prdList.size() << std::endl;
      //       if (rio) selectedHits->push_back(rio);
      if (phiSelect[i] == 1) {
	const Muon::RpcPrepData* prd = dynamic_cast <const Muon::RpcPrepData*> (phiPrep[i]);
	const Amg::Vector3D globalpos(phiHitx[i],phiHity[i],phiHitz[i]);
	const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
	if (rio) selectedHits->push_back(rio);
      } 
      else if (phiSelect[i] == 2) {
	const Muon::TgcPrepData* prd = dynamic_cast <const Muon::TgcPrepData*> (phiPrep[i]);
	const Amg::Vector3D globalpos(phiHitx[i],phiHity[i],phiHitz[i]);
	const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
	if (rio) selectedHits->push_back(rio);
      } 
      else if (phiSelect[i] == 3) {
	const Muon::CscPrepData* prd = dynamic_cast <const Muon::CscPrepData*> (phiPrep[i]);
	const Amg::Vector3D globalpos(phiHitx[i],phiHity[i],phiHitz[i]);
	const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
	if (rio) selectedHits->push_back(rio);
      }

      if (m_debug) std::cout << " Make ONE rio per PrepData " << std::endl;
    }
  } 
  if (m_debug||m_summary) { 
    std::cout << " Fit hit results phi " << m_phi << " chi2 " << chi2 <<  " segment hits  " << nphiseg << " pattern hits " << nphi-nphiseg << " nfit " << nfit << " rio size " << selectedHits->size() << std::endl;
  } 

  std::vector<double> clusterX(nphi);
  std::vector<double> clusterY(nphi);
  std::vector<double> clusterZ(nphi);
  std::vector<double> clusterError(nphi);
  std::vector<Identifier> clusterId(nphi);
  std::vector<int> clusterHits(nphi);
  std::vector<double> clusterPull(nphi);
  std::vector<int> clusterSelect(nphi);
  // Link from hit to cluster
  std::vector<int> clusterInt(nphi);

  int ncl,imax; 
  double chi2cl,r0cl, phicl; 
  std::vector<double> errorMcl(4);
  clusterPhi(phiId, phiHitx, phiHity, phiHitz, phiError, phiPull, phiSelect, nphi, clusterX, clusterY, clusterZ, clusterError, clusterId, clusterHits, clusterSelect, clusterInt,  ncl);


  for(int ic = 0; ic < ncl  ; ++ic )  {
    std::list  <const Trk::PrepRawData*> prdList;
    int iic = -1;
    double avError = 0.;
    int ip = -1;
    int np = 0;
    for(int i = 0; i < nphi  ; ++i )  {
      if (clusterInt[i] == ic) {
        ip = i; 
        prdList.push_back(phiPrep[i]);
        avError += 1./(phiError[i]*phiError[i]);
        if (clusterId[ic] == phiId[i]) iic = i; 
        np++; 
      }
    }
    if (iic > -1) {
      if (m_debug) std::cout << " Phi cluster found np " << np << " ip " << ip << std::endl;
      if (np ==1) {
	// Only one PrepData: create RIO on Track
	const Amg::Vector3D globalpos(clusterX[ic],clusterY[ic],clusterZ[ic]);
	if (phiSelect[ip] == 1) {
	  if (m_debug) std::cout << " Phi RPC rio " << std::endl;
	  const Muon::RpcPrepData* prd = dynamic_cast <const Muon::RpcPrepData*> (phiPrep[ip]);
	  const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
	  if (rio) selectedClusters->push_back(rio);
        }
        else if (phiSelect[ip] == 2) {
	  if (m_debug) std::cout << " Phi TGC rio " << std::endl;
	  const Muon::TgcPrepData* prd = dynamic_cast <const Muon::TgcPrepData*> (phiPrep[ip]);
	  const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
	  if (rio) selectedClusters->push_back(rio);
        }
        else if (phiSelect[ip] == 3) {
	  if (m_debug) std::cout << " Phi CSC rio " << std::endl;
	  const Muon::CscPrepData* prd = dynamic_cast <const Muon::CscPrepData*> (phiPrep[ip]);
	  const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
	  if (rio) selectedClusters->push_back(rio);
        }
      } else {

        if (m_competingRios) {
	  // More PrepData's: create Competing RIOs on Track
	  avError = sqrt(1./avError);
	  double scaleFactor = clusterError[ic]/avError;
	  const Trk::CompetingRIOsOnTrack*  rio = m_competingRIOsOnTrackTool->createBroadCluster(prdList,scaleFactor);
	  if (rio) selectedClusters->push_back(rio);
	  if (m_debug) std::cout << " Make competing rio/cluster " << " scale factor " << scaleFactor << " number of rios " << prdList.size() << std::endl;
        } else {
	  // Make one Rio for central cluster
          ip = iic;
          const Amg::Vector3D globalpos(clusterX[ic],clusterY[ic],clusterZ[ic]);
          if (phiSelect[ip] == 1) {
            if (m_debug) std::cout << " Phi RPC rio central cluster" << std::endl;
            const Muon::RpcPrepData* prd = dynamic_cast <const Muon::RpcPrepData*> (phiPrep[ip]);
            const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
            if (rio) selectedClusters->push_back(rio);
	  }
	  else if (phiSelect[ip] == 2) {
            if (m_debug) std::cout << " Phi TGC rio central cluster" << std::endl;
            const Muon::TgcPrepData* prd = dynamic_cast <const Muon::TgcPrepData*> (phiPrep[ip]);
            const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
            if (rio) selectedClusters->push_back(rio);
	  }
	  else if (phiSelect[ip] == 3) {
            if (m_debug) std::cout << " Phi CSC rio central cluster" << std::endl;
            const Muon::CscPrepData* prd = dynamic_cast <const Muon::CscPrepData*> (phiPrep[ip]);
            const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd,globalpos);
            if (rio) selectedClusters->push_back(rio);
	  }
        }
      }
    }else {
      if (m_debug) std::cout << " Phi cluster NOT found " << std::endl;
    }
  }      

  fitPhiSL(pmom, clusterId, clusterX, clusterY, clusterZ, clusterError, clusterSelect, ncl, clusterPull, imax, chi2cl, r0cl, phicl, errorMcl , false );


  if (m_debug||m_summary) { 
    std::cout << " PhiHitSelector Time spent "  << std::clock()/1000-time_start << " nhits " << nhits << " segment hits " << associatedHits.size() << " nfit " << nfit << " nclusters " << ncl << std::endl;
    std::cout << " Fit cluster results phi " << phicl << " chi2 " << chi2cl <<  " number of clusters  " << ncl << " size cluster Hits " << selectedClusters->size() << std::endl;
  }
  if (m_makeClusters) {
    m_phi = phicl;

    std::vector<const Trk::MeasurementBase*>::iterator mit = selectedHits->begin();
    for (;mit!=selectedHits->end();++mit){
      delete *mit;
    }
    delete selectedHits;
    return selectedClusters;
  }
  
  std::vector<const Trk::MeasurementBase*>::iterator mit = selectedClusters->begin();
  for (;mit!=selectedClusters->end();++mit){
    delete *mit;
  }
  delete selectedClusters;
  return selectedHits;
}


void MuonPhiHitSelector::clusterPhi( const std::vector<Identifier> & id,  const std::vector<double> & hitx,  const std::vector<double> & hity,  const std::vector<double> & hitz, const std::vector<double> & error, const std::vector<double> & pull, std::vector<int> & select, const int n, std::vector<double> & clusterX , std::vector<double> & clusterY ,std::vector<double> & clusterZ , std::vector<double> & clusterError , std::vector<Identifier> & clusterId, std::vector<int> & clusterHits, std::vector<int> & clusterSelect, std::vector<int> & clusterInt, int & ncl ) const {

  //
  // Use hits (select > 0) and pulls to make clusters
  // 
  //
  // Inputs 
  //        id             = identifiers hits 
  //        hitx hity hitz = position in space 
  //        error          = associated error (in x-y plane)
  //        pull (from fit)= residual (hit -fit) /error
  //        select         = > 0 for selected hits
  //        n              = total number of hits 

  // Outputs 
  //        clusterX Y Z   = cluster positions
  //        clusterError   =          errors
  //        clusterId      = cluster identifier (smallest pull)
  //        clusterHits    = number of hits per cluster 
  //        ncl            = number of clusters 
  //         chi2          = total chi2
  //         r0            = perigee parameter of fit (0,0)
  //         phi           = azimuthal angle of fit at perigee


  if (m_debug) std::cout << " Start phi clustering " << std::endl;

  ncl = 0;
  if (n ==0) return;

  std::vector<int> scode(n);
    
  for(int i = 0; i < n  ; ++i )  {
    Identifier idi = id[i];
    int code = 0;
    if (m_rpcIdHelper->is_rpc( idi )) {
      int doubZ = m_rpcIdHelper->doubletZ(idi);
      int doubPhi = m_rpcIdHelper->doubletPhi(idi);
      code = 100000000*(m_rpcIdHelper->stationName(idi))+1000000*(m_rpcIdHelper->stationPhi(idi))+ 10000* ((m_rpcIdHelper->stationEta(idi))+1000);
      code += 1000*(doubZ-1) + 100*(doubPhi-1);
      code += 2*((m_rpcIdHelper->doubletR(idi))-1) + 16*((m_rpcIdHelper->gasGap(idi))-1) ;
    }
    else if (m_tgcIdHelper->is_tgc( idi )) {
      code = 1000000*(m_tgcIdHelper->stationName(idi))+10000*(m_tgcIdHelper->stationPhi(idi))+ 100* ((m_tgcIdHelper->stationEta(idi))+10);
      code = code + m_tgcIdHelper->gasGap(idi);
    }
    else if (m_cscIdHelper->is_csc( idi )) {
      code = 1000000*(m_cscIdHelper->stationName(idi))+10000*(m_cscIdHelper->stationPhi(idi))+ 100* ((m_cscIdHelper->stationEta(idi))+10);
      code = code + m_cscIdHelper->wireLayer(idi);
    }
    scode[i] = code;
  }   

  //    std::vector<int> clusterInt(n);

  for(int i = 0; i < n  ; ++i )  {
    clusterInt[i] = -1;
  }

  int icl = -1;
  for (int i = 0; i < n ; ++i ) {
    if ( error[i] != 0  && select[i] > 0 && clusterInt[i] == -1 ) {
      icl++;
      clusterInt[i] = icl;
      for (int j = i+1; j < n ; ++j ) {
	if (clusterInt[j] == -1 ) {
	  if ( error[j] != 0 && select[j] > 0 ) {
	    if (scode[i] == scode [j]) clusterInt[j] = icl;
	  }
	}
      }
    }
  }
 
  std::vector<double> clusterCommon2Error(icl+1);
  std::vector<int> clusterCode(icl+1);
  ncl = icl+1;
  for (int ic = 0; ic < icl+1  ; ++ic ) {
    clusterX[ic] = 0.;
    clusterY[ic] = 0.;
    clusterZ[ic] = 0.;
    clusterError[ic] = 0.;
    clusterCommon2Error[ic] = 0.;
    clusterHits[ic] = 0;
    clusterCode[ic] = 0;
    clusterSelect[ic] = 0;
    double pullMax = 10.;
    for (int i = 0; i < n  ; ++i ) {
      if (select[i] > 0) {
	if (ic == clusterInt[i] ) {
	  clusterSelect[ic] = select[i];
	  double w = 1./(error[i]*error[i]);
	  clusterX[ic]+= hitx[i]*w; 
	  clusterY[ic]+= hity[i]*w; 
	  clusterZ[ic]+= hitz[i]*w; 
	  clusterError[ic]+= w; 
	  if ( fabs(pull[i]) < fabs(pullMax)) {
	    pullMax = pull[i];
	    clusterId[ic] = id[i];
	    clusterCode[ic] = scode[i];
	    clusterSelect[ic] = select[i];
	  } 
	  clusterHits[ic]++; 
	  if (clusterHits[ic] == 1) clusterCommon2Error[ic]= 0.;
	  //            error[i]*error[i] - error0[i]*error0[i] ; 
	}
      }
    } 
    clusterX[ic] = clusterX[ic]/clusterError[ic]; 
    clusterY[ic] = clusterY[ic]/clusterError[ic]; 
    clusterZ[ic] = clusterZ[ic]/clusterError[ic]; 
// Don't assume improvement on errors due to clustering 
    clusterError[ic]= sqrt(clusterHits[ic])/sqrt(clusterError[ic]); 
    if (m_debug) {
      std::cout << " cluster phi " << ic << " x " << clusterX[ic] << " y " << clusterY[ic] << " z " << clusterZ[ic] << " error " << clusterError[ic] << " hits " << clusterHits[ic] <<  " select " << clusterSelect[ic] << " Code " << clusterCode[ic] << std::endl;  
    }
  }
  
}
void MuonPhiHitSelector::fitRecPhi( const double pmom, const std::vector<Identifier> & phiId,  const std::vector<double> & phiHitx,  const std::vector<double> & phiHity,  const std::vector<double> & phiHitz, const std::vector<double> & phiError, std::vector<int> & quality, const int nphi, std::vector<double> & phiPull, std::vector<int> & phiMult, std::vector<int> & phiSelect, double & chi2, double & r0, double & phi,  std::vector<double> & errorM, int & nfit) const {

  //
  // Use reconstructed hits to perform fit for phi
  //

  if (m_debug) std::cout << " Start phi fit reconstruction " << std::endl;

  chi2 =0.;
  r0 = 0.;
  nfit = 0;
  phi =0.;

  int ncsc = 0;
  int ntgc = 0;
  int nrpc = 0;


  if (nphi ==0) return;

  std::vector<double> error0(nphi);
  std::vector<double> error(nphi);
  std::vector<double> errorf(nphi);
  std::vector<int> scode(nphi);
  std::vector<int> srcode(nphi);
  std::vector<int> phiSelectKeep(nphi);
  std::map<int,int> clusters;
  std::map<int,int> clustersr;
  std::map<int,int> clusterspat;
    
  for(int i = 0; i < nphi  ; ++i )  {

    Identifier idi = phiId[i];
    int code = 0;
    int rcode = 0;
    if (m_rpcIdHelper->is_rpc( idi )) {
      code = 1000000*(m_rpcIdHelper->stationName(idi))+10000*(m_rpcIdHelper->stationPhi(idi))+ 100* ((m_rpcIdHelper->stationEta(idi))+10);
      code = code + 2*((m_rpcIdHelper->doubletR(idi))-1)+16*((m_rpcIdHelper->gasGap(idi))-1);
      rcode = 1000000*(m_rpcIdHelper->stationName(idi))+10000*(m_rpcIdHelper->stationPhi(idi))+ 0* ((m_rpcIdHelper->stationEta(idi))+10);
      rcode = rcode + 2*((m_rpcIdHelper->doubletR(idi))-1)+16*((m_rpcIdHelper->gasGap(idi))-1);
    }
    else if (m_tgcIdHelper->is_tgc( idi )) {
      code = 1000000*(m_tgcIdHelper->stationName(idi))+10000*(m_tgcIdHelper->stationPhi(idi))+ 100* ((m_tgcIdHelper->stationEta(idi))+10);
      code = code + m_tgcIdHelper->gasGap(idi);
      rcode = 1000000*(m_tgcIdHelper->stationName(idi))+10000*(m_tgcIdHelper->stationPhi(idi))+ 0* ((m_tgcIdHelper->stationEta(idi))+10);
      rcode = rcode + m_tgcIdHelper->gasGap(idi);
    }
    else if (m_cscIdHelper->is_csc( idi )) {
      code = 1000000*(m_cscIdHelper->stationName(idi))+10000*(m_cscIdHelper->stationPhi(idi))+ 100* ((m_cscIdHelper->stationEta(idi))+10);
      code = code + m_cscIdHelper->wireLayer(idi);
      rcode = 1000000*(m_cscIdHelper->stationName(idi))+10000*(m_cscIdHelper->stationPhi(idi))+ 0* ((m_cscIdHelper->stationEta(idi))+10);
      rcode = rcode + m_cscIdHelper->wireLayer(idi);
    }

    scode[i] = code;
    srcode[i] = rcode;
    int idet = 0;
    if (m_rpcIdHelper->is_rpc(idi)) idet = 1;
    else if (m_tgcIdHelper->is_tgc(idi)) idet = 2;
    else if (m_cscIdHelper->is_csc(idi)) idet = 3;
    phiSelect[i] = idet;
    phiSelectKeep[i] = idet;
  }
  // Hits on segments
  for(int i = 0; i < nphi  ; ++i )  {
    if ( phiError[i] != 0 && quality[i] > 100 ) {
      clusters[scode[i]]++;
      clustersr[srcode[i]]++;
    }
  }
  // Drop hits on patterns that are in same station and layer as segment hit
  //  Avoid adding again (solved) ambiguous hits
 
  for(int i = 0; i < nphi  ; ++i )  {
    if (phiError[i] != 0 && quality[i]>0 && quality[i] < 100) {
      if (clustersr.count(srcode[i]) > 0) {
	quality[i] = 0;
      } else {
	clusterspat[scode[i]]++;
      } 
    }
  }

  // Assign errors according to multiplicities
    if(m_debug) std::cout << " phi hits " << nphi << " segment clusters " << clusters.size() << " pattern clusters " << clusterspat.size() << std::endl;

  for(int i = 0; i < nphi  ; ++i )  {
    error0[i] = 0;
    Identifier id = phiId[i];
    phiMult[i] = 0;
    if ( phiError[i] != 0 && quality[i] > 0 ) {
      int n = 0;
      if (quality[i] > 100 ) {
	n = clusters[scode[i]];
// Treat phi hits from segment with high multiplicity > 10 as lying on patterm
        if (n > 10) quality[i] = 10; 
      } else if (quality[i] < 100 ) {
	n = clusterspat[scode[i]];
// Drop phi hits patterns with high multiplicity  
	if (clusters.count(scode[i])==1 || n > 10) {
	  n = 0;
	  // drop phi hits from pattern if already segment hits in same layer
	  quality[i] = 0;
	  phiSelect[i] = 0;
	  phiSelectKeep[i] = 0;
	  continue;
	}
      } 
      phiMult[i] = n;
      double fact = 1.;
      if (m_rpcIdHelper->is_rpc(id)) fact = 1.2;
      else if (m_tgcIdHelper->is_tgc(id)) n = 1;
      else if (m_cscIdHelper->is_csc(id)) n = 1;
           
      error0[i]=phiError[i]*sqrt(n)*fact;  
      error[i]=phiError[i]*sqrt(n)*fact;  
      double phiHit = atan2 ( phiHity[i], phiHitx[i] );
      if (m_debug) {
	//	  std::string st = id.stationNumberToFixedStationString(id.stationName());
	std::cout << i << " Station " << int(scode[i]/1000000) << " Hit x " << phiHitx[i] << " Hit y " << phiHity[i] << " Hit z " << phiHitz[i] << " error " << phiError[i] << " phi Hit " << phiHit << std::endl; 
	std::cout << " station " << phiSelect[i] << std::endl;
	std::cout << " code " << scode[i] << " multiplicity " << n << " error " << error0[i] << " quality " << quality[i] << std::endl;
	if ( error0[i] < 1. ) std::cout << " TOO small error " << std::endl;
      }
    }
  }

  // Count layers hit 

  std::map<int,int> layersHit;
  for(int i = 0; i < nphi  ; ++i )  {
    if ( phiError[i] != 0 && quality[i] > 0 ) {
      layersHit[srcode[i]]++;
    }
  }
  int allLayerHits = layersHit.size(); 
  int allLayerRecoHits = 0; 
  double pfit = 20000.;

  for (int iqua = 0; iqua < 3 ; ++iqua )  {

    double quacut = 10;
    if (iqua == 1 ) quacut = 0; 
    else if (iqua == 2 ) {
      quacut = 10;
      pfit = pmom;
    } 

    if (m_debug) std::cout << " Quality loop " << iqua << " quality cut " << quacut << std::endl;
    int nsel = 0;
    int nselseg = 0;   
    for(int i = 0; i < nphi  ; ++i )  {

      if (iqua == 1) phiSelect[i] = phiSelectKeep[i];

      if ( phiError[i] != 0 && quality[i] > quacut ) {    
	nsel++;
        if (quality[i]>100) nselseg++;  
	if (quality[i]== 10 && iqua == 1) quality[i] = 11;
      } else {
	phiSelect[i] = 0;
      }
      if (m_debug) std::cout << " index i " << i << " phiSelect " <<  phiSelect[i] << " Quality " << quality[i] << " error " << error[i]  << std::endl;
    }

    int imax = -1;
    if (iqua ==1 && nselseg> 0) {
      // Test and drop pattern Hits if far off 
      double errorScaleFactor = 25.;
      std::vector<int> phiPatSelect(nphi,0);
      for(int i = 0; i < nphi  ; ++i )  {
        phiPatSelect[i] = 0;
        if( phiSelect[i] > 0 && quality[i] > 0 && quality[i] < 100 ) {
          phiPatSelect[i] = 1;
          error[i] = errorScaleFactor*error[i];
        }
        if (m_debug) std::cout << " select " << phiSelect[i] << " quality " << quality[i] << " error " << error[i] << std::endl;
      }
      if (m_debug) std::cout << " performing outlier removal for pattern hits " << std::endl; 
//      fitPhiSL(pfit, phiId,  phiHitx,  phiHity,  phiHitz, error, phiSelect, nphi, phiPull, imax, chi2, r0, phi, errorM , true);
      fitPhiSL(pfit, phiId,  phiHitx,  phiHity,  phiHitz, error, phiSelect, nphi, phiPull, imax, chi2, r0, phi, errorM , false);
      for(int i = 0; i < nphi  ; ++i )  {
        if(phiPatSelect[i] == 1) {
          error[i] = error[i]/errorScaleFactor;
	  double rescaledPull = phiPull[i]*errorScaleFactor;
	  // 3 sigma cut  
          if (fabs(rescaledPull) < 3.) {
            phiSelect[i] = phiSelectKeep[i];
          } else {
            phiSelect[i] = 0;
            phiSelectKeep[i] = 0;
       	    if (m_debug) std::cout << " Drop Pattern Hits with Quality == 1 " << i << " quality " << quality[i] 
				   << " Pull " << rescaledPull << " phiSelect " << phiSelect[i] << std::endl;
	  }
        }
      }
    }

    const double pfitc = pfit;
    imax = -1;
    
    if (iqua == 2) {
      // low momentum fit with scaled error (factor 10) for dropped segment hits 
      std::vector<int> phiReSelect(nphi);
      for(int i = 0; i < nphi  ; ++i )  {
        if (m_debug) std::cout << " select " << phiSelect[i] << " quality " << quality[i] << std::endl;
        phiReSelect[i] = 0;
        if(phiSelect[i] == 0 && quality[i] > 99) {
          phiReSelect[i] = 1;
          phiSelect[i] = phiSelectKeep[i];
          error[i] = 10.*error[i];
        }
      }
      fitPhiSL(pfitc, phiId,  phiHitx,  phiHity,  phiHitz, error, phiSelect, nphi, phiPull, imax, chi2, r0, phi, errorM, false);
      for(int i = 0; i < nphi  ; ++i )  {
        if(phiReSelect[i] == 1) {
          error[i] = error[i]/10.;
	  // 10 sigma cut (error rescale = 10) 
          if (fabs(phiPull[i]) < 1) {
            phiSelect[i] = phiSelectKeep[i];
          } else {
            phiSelect[i] = 0;
          } 
          if (m_debug) std::cout << " Low momentum Quality == 2 add hit  nr " << i << " quality " << quality[i] << " Pull " << phiPull[i] << " phiSelect " << phiSelect[i] << std::endl;
        }
      }
    }
    if (iqua == 1 && m_debug) std::cout << " Quality loop " << std::endl;
    nsel = 0;
    for(int i = 0; i < nphi  ; ++i )  {
      errorf[i] = error[i];
      if (iqua ==1) phiSelect[i] = phiSelectKeep[i];

      if ( phiError[i] != 0 && quality[i] > quacut ) {
	nsel++;
	if (quality[i]== 10 && iqua == 1) quality[i] = 11;
      } else {
	phiSelect[i] = 0;
      }
    } 

    if(m_debug) std::cout << " Selected PHI hits in fit " << nsel << " iqua " << iqua << std::endl; 
    if (nsel == 0) continue;

    int niter = -1;
    // do hit dropping in maximal 10 iterations by putting quality to 0

    for (int iter = 0; iter < 100 ; ++iter ) {
      niter++;
      double power = (iter - 10)/20.;
      if (power< 0.) power = 0.;
      chi2 =0.;
      nfit = 0;
      if (iter > 10) {
// Shower treatment inflate errors with multiplicity
        for(int i = 0; i < nphi  ; ++i )  {
         errorf[i] = error[i]*pow(phiMult[i],power);
        }
      } 
      fitPhiSL(pfitc, phiId,  phiHitx,  phiHity,  phiHitz, errorf, phiSelect, nphi, phiPull, imax, chi2, r0, phi, errorM, false );

      ncsc = 0;
      ntgc = 0;
      nrpc = 0;

      // Count layers hit in Reconstruction 

      std::map<int,int> layersRecoHit;
 
      for(int i = 0; i < nphi  ; ++i )  {
	Identifier id = phiId[i];
	if ( error[i] == 0 || quality[i] < quacut) phiSelect[i] = 0;
	if ( error[i] != 0 && quality[i] > quacut) {
	  layersRecoHit[srcode[i]]++;
	  if (m_debug) {
	    if (m_rpcIdHelper->is_rpc(id)) nrpc++;
	    else if (m_tgcIdHelper->is_tgc(id)) ntgc++;
	    else if (m_cscIdHelper->is_csc(id)) ncsc++;
	  }
	  nfit++;
	}
      }
      allLayerRecoHits = layersRecoHit.size(); 
      double frac =  allLayerRecoHits/ ( allLayerHits + 0.0001); 

      if (nfit == 1) break;

      if (imax < 0 || imax > nphi ) {
	if (m_debug) std::cout << " Fitphi imax " << imax << std::endl;
	break;
      }

      if (chi2 < 5*(nfit+1) || fabs(phiPull[imax]) < 3.0 ) {
	// &&  chi2max < 25 ) {
 
	if (m_debug) std::cout << " Final phi " << phi << " frac " << frac << " chi2 " << chi2 << std::endl; 
	break;
      } 

      //      quality[imax] = 0;
      phiSelect[imax] = 0;

      if (m_debug) { 
	std::cout << " = Start hit dropping " << imax << " pullmax " << phiPull[imax] << " phi " << phi << " chi2 " << chi2 << std::endl; 
      }
    }

    //      clusterPhi(phiId, phiHitx, phiHity, phiHitz, error, phiPull, phiSelect, nphi, clusterX, clusterY, clusterZ, clusterError, clusterId, clusterHits, clusterSelect, ncl);
    //      double r0cl; 
    //      std::vector<double> errorMcl(4);
    //      double phicl;
    //      fitPhiSL(pmom, clusterId, clusterX, clusterY, clusterZ, clusterError, clusterSelect, ncl, clusterPull, imax, chi2cl, r0cl, phicl, errorMcl, false);

    if (m_debug) { 
      std::cout << " Fit results phi " << phi << " chi2 " << chi2 <<  " ndof " << nfit << std::endl;
      std::cout << " Reco RPC " << nrpc << " TGC " << ntgc << " CSC " << ncsc << std::endl;
    }

    
    int nacc = 0;
    int nshowerdrop = 0;
    for(int i = 0; i < nphi  ; ++i )  {
      double power = (niter - 10)/20.;
      if (power< 0.) power = 0.;
      double pull = phiPull[i]*pow(phiMult[i],power);
      if (niter > 10 && fabs(pull) > 3.0 && phiSelect[i] > 0 ) {
        phiSelect[i] = 0;
        quality[i] = 0;
        nshowerdrop++;
        if (m_debug) std::cout << " Drop shower hit i " << i << " with pull " << pull << " iterations " << niter  << " power " << power << std::endl;  
      }
      if( phiSelect[i] == 0) {
	//        quality[i] = 0;
      } else {
        nacc++;
      }
    }
    if(m_debug) std::cout << " phi hits " << nphi << " selected for fit " << nfit << " iqua " << iqua << " iterations " << niter << " accepted hits " << nacc << " nshower drop " << nshowerdrop << std::endl; 
  }
}

void MuonPhiHitSelector::fitPhiSL(const double pmom, const std::vector<Identifier> & /*id*/ ,  const std::vector<double> & hitx,  const std::vector<double> & hity,  const std::vector<double> & hitz, const std::vector<double> & error, std::vector<int> & select, const int n, std::vector<double> & pull, int & imax ,  double & chi2, double & r0, double & phi , std::vector<double> & errorM, bool fast) const {

  // Perform straight line fit to hits: good hits have select > 0  
  // in the fit scattering centres are added for nfit-1 angles
  //            WITH  beamspot constraint 
  //                degrees of freedom = 2*nfit

  // Fit is based on matrix inversions formulae

  // Inputs pmom           = estimate of momentum
  //        id             = identifiers hits 
  //        hitx hity hitz = position in space 
  //        error          = associated error (in x-y plane)
  //        select         = > 0 for selected hits
  //        n              = total number of hits 


  // Outputs pull          = residual (hit -fit) /error
  //         imax          = index for hit with maximum pull
  //         chi2          = total chi2
  //         r0            = perigee parameter of fit (0,0)
  //         phi           = azimuthal angle of fit at perigee

  double pest = pmom;
  if (pest > 20000.) pest = 20000.;

  r0 = 0.;
  phi = 0.;
  chi2 = 0.;
  imax = 0;

  // Calculate mean position 
  double xm = 0.;
  double ym = 0.;
  double dtot = 0.;
  double em = 0.;
//   int nrpc = 0;
//   int ntgc = 0;
//   int ncsc = 0;
  for(int i = 0; i < n  ; ++i )  {
    if ( error[i] != 0 && select[i] > 0 ) {
//       if (m_rpcIdHelper->is_rpc(id[i])) nrpc++;
//       else if (m_tgcIdHelper->is_tgc(id[i])) ntgc++;
//       else if (m_cscIdHelper->is_csc(id[i])) ncsc++;
      double inver2 = 1./(error[i]*error[i]); 
      xm +=  hitx[i]*inver2;
      ym +=  hity[i]*inver2;
      dtot +=  sqrt (hitx[i]*hitx[i] + hity[i]*hity[i] + hitz[i]*hitz[i] )*inver2 ;
      em +=  inver2;
    }
  }

  if (em  == 0) return;  

  dtot = dtot/em;

  //    Beamspot error 10 mm for cosmics 10000 

  double ebs = 0.1;
  if (m_cosmics) ebs = 10000.;

  if (m_debug) std::cout << " pmom " << pmom << " error beam " << ebs << std::endl;
  double ebs2 = ebs*ebs;
  double invebs2 = 1./ebs2;
  double xmc = xm / ( em + invebs2); 
  double ymc = ym / ( em + invebs2); 
  xm = xm/em;
  ym = ym/em;

  // Constraint on beam spot 

  double len2 = xmc*xmc+ymc*ymc;
  double xcc =  len2*xmc*invebs2;
  double ycc =  len2*ymc*invebs2;

  for(int i = 0; i < n  ; ++i )  {
    if ( error[i] != 0 && select[i] > 0) {
      double inver2 = 1./(error[i]*error[i]);
      double xdiff = hitx[i]-xmc;
      double ydiff = hity[i]-ymc;
      double xdiff2 = xdiff*xdiff;
      double ydiff2 = ydiff*ydiff;
      len2 =xdiff2+ydiff2;
      double sign = 1.;
// Non Cosmics assume IP at 0 0  
      if (xdiff*hitx[i]+ydiff*hity[i]<0&&!m_cosmics) sign =-1;
// Cosmics assume down going 
      if (ydiff<0&&m_cosmics) sign =-1;
      xcc +=  len2*sign*xdiff*inver2;
      ycc +=  len2*sign*ydiff*inver2;
    }
  }
    
  if (em>0) phi = atan2(ycc,xcc);
  CxxUtils::sincos scphi(phi);

  r0 = xmc*scphi.sn - ymc*scphi.cs; 
  double x0 = r0*scphi.sn;
  double y0 = -r0*scphi.cs; 

  if(m_debug) std::cout << " Constraint r0 " << r0 << " xpos " << xmc << " ypos " << ymc << " phi " << phi << std::endl; 
  // assume 0,0
  std::vector<double> d(n);       
  std::vector<double> dist(n);       
  std::map < double, int > distanceSort;       
  double pullmax = 0.;
  for(int i = 0; i < n  ; ++i )  {
    if ( error[i] != 0 && select[i] > 0) {
      double xdiff = hitx[i]-x0;
      double ydiff = hity[i]-y0;
      double xdiff2 = xdiff*xdiff;
      double ydiff2 = ydiff*ydiff;
      d[i] = sqrt(xdiff2 + ydiff2);
      dist[i] = sqrt(xdiff2 + ydiff2 + hitz[i]*hitz[i]);
      distanceSort[dist[i]] = i;
      pull[i] = hitx[i]*scphi.sn - hity[i]*scphi.cs - r0;
      if (fabs(pull[i])> fabs(pullmax)) {
       pullmax = pull[i];
       imax = i;
      } 
    }
  }
   
  if (fast) return;
 
  std::map< double, int >::iterator it = distanceSort.begin();
  std::map< double, int >::iterator it_end = distanceSort.end();
    
  int nfit = 0; 
  std::vector<double> xf(2*n);       
  std::vector<double> lf(2*n);       
  std::vector<double> yf(2*n);       
  std::vector<double> ef(2*n);       
  std::vector<int> indexf(n);       
  //
  //      measurements yf error ef at distance xf (0:nfit)
  //      scattering centra angle zero yf error ef at distance xf(nfit+1..2nfit-1)     
  //      beamspot at yf(2 nfit) = 0 error ebs2 at distance xf(2 nfit)
    
  for(; it != it_end; ++it){
    int index = it->second;
    xf[nfit] = d[index];
    lf[nfit] = dist[index];
    yf[nfit] = (hitx[index]-xmc)*scphi.sn - (hity[index]-ymc)*scphi.cs;
    ef[nfit] = error[index];
    indexf[nfit] = index; 
    nfit++;
  }   
    
  // NB start at 1 to add scattering centra
    
  double erang = 0.030*5000./(pest+1000.);
  for(int i =1; i < nfit  ; ++i )  {
    xf[nfit+i-1] = xf[i-1];
    yf[nfit+i-1] = 0.;
    double scale = 1.;
    if (select[i] == 1) scale = 1.; 
    else if (select[i] == 3) scale = 0.5; 
    else if (select[i] == 2) scale = 2.5; 
    ef[nfit+i-1] = scale*erang;
  }
  // Beamspot
  yf[2*nfit-1] = 0.;
  xf[2*nfit-1] = 0.;
  ef[2*nfit-1] = ebs;
    
  Amg::MatrixX v(nfit+1,1);
  v.setIdentity();

  if (m_debug) std::cout << "  fitPhiSL " << " nfit " << nfit << std::endl;

  for(int i = 0; i < nfit+1 ; ++i )  {
    v(i,0) = 0.;
    for(int j = 0; j < nfit ; ++j )  {
      double inver2 = 1./(ef[j]*ef[j]);
      //        std::cout << " xf[j] " << xf [j] << " yf [j] " << yf[j] << " ef[j] " << ef[j] << std::endl;
      if (i == 0) v(i,0) += yf[j]*inver2;
      else if (i == 1) v(i,0) += yf[j]*xf[j]*inver2;
      else if (i > 1 && j > i-2 ) {
	//         std::cout << " i " << i << " xf[i-2] " << xf [i-2] << std::endl;
	v(i,0) += yf[j]*(lf[j]-lf[i-2])*inver2;
      } 
      //       std::cout << " v[i][0] " << v[i][0] << std::endl;
    }  
  }  

  // Track Model Matrix

  Amg::MatrixX model(nfit+1,2*nfit);
  model.setIdentity();
  // Measurements related to position and slope 

  for(int i = 0; i <nfit+1 ; ++i )  {
    for(int j = 0; j <nfit ; ++j )  {
      model(i,j) = 0.;
      if ( i == 0 ) model(i,j) = 1.;
      else if ( i == 1 ) model(i,j) = xf[j];
      // scattering angle
      else if ( i > 1 && j > i - 2 ) model(i,j) = lf[j]-lf[i-2];
    }
  }

  // Constraints on scattering angles and beamspot 

  for(int i = 0; i <nfit+1 ; ++i )  {
    for(int j = nfit; j < 2*nfit ; ++j )  {
      model(i,j) = 0.;
      // scattering angle  
      if ( i == j- nfit + 2) model(i,j) = 1.;
      // Beam spot 
      if ( i == 0 && j == 2*nfit-1) model(i,j) = 1.;
    }
    //      if (m_debug) {
    //        for(int j = 0 ; j < 2*nfit ; ++j )  {
    //         std::cout << " i " << i << " j " << j << " Matrix model i j " << model[i][j] << std::endl; 
    //        } 
    //      } 
  }
    
  // Covariance Inverse of Track parameters
    
  Amg::MatrixX covT(nfit+1,nfit+1);
  for(int i = 0; i <nfit+1 ; ++i )  {
    for(int j = 0; j <nfit+1 ; ++j )  {
      covT(i,j) = 0.;
      for(int k = 0; k <2*nfit ; ++k )  {
	double er2 = ef[k]*ef[k];
	covT(i,j) += model(i,k)*model(j,k)/er2;
      }
    }
  }
  
// Invert covariance matrix and replace it (should be CovT)
  Amg::MatrixX covTI = covT.inverse();

  Amg::MatrixX t(nfit+1,1);
  // Solution for Track parameters
  t = covTI*v;
   
  if (m_debug && fabs(t(1,0))> 0.2 ) {
    std::cout << " Don't trust fit result " << t(1,0) << " Keep Old result "  << std::endl;
  }
  if (fabs(t(1,0))> 0.2) return;
   
  //  if (m_debug) { 
    //      for(int i = 0; i < nfit+1 ; ++i )  {
    //        std::cout << " result i " << i << " parameter t " << t(i,0)  << std::endl; 
    //      }
  //}
    
  // calculate residuals and chi2
  std::vector <double> resi(2*nfit); 
  std::vector <double> pulli(2*nfit); 
  std::vector <double> errf(2*nfit); 
  std::vector <double> pullf(2*nfit); 
  std::vector <double> resiOut(2*nfit); 
  std::vector <double> pullOut(2*nfit); 
  pullmax = 0.;
  int jmax = 0;

  errorM[0] = covTI(0,0);
  errorM[1] = covTI(0,1);
  errorM[2] = covTI(1,1);
  errorM[3] = 0.;  
  if (nfit>2) {
     double invlt = 1./(lf[nfit-1]-lf[1]);
     for(int i = 1; i < nfit-1 ; ++i )  {
       double w = (lf[nfit-1]-lf[i])*invlt;
       errorM[3] += covTI(i+1,i+1)*w*w;
     }
  }

  if(m_debug) {
    if (nfit>=3) {  std::cout << " Error angle " << covTI(3,3) << std::endl;} // covTI has dim nfit+1
    std::cout << " errorM[3] " << errorM[3] << std::endl;
  }

  for(int i = 0; i < 2*nfit ; ++i )  {

    // Calculate         prediction at each measurement i
    //           propagate error of track parameters to measurement i
    double error2 = 0.;
    double ypred = 0.;
    for(int j = 0; j < nfit+1 ; ++j )  {
      if(m_debug && i == 0) std::cout << " Parameter j " << j << " t(j,0) " << t(j,0) << std::endl;
      if(m_debug && model(j,i) != 0) std::cout << " i " << i << " model ij " << model(j,i) << std::endl;
      ypred += model(j,i)*t(j,0);
      for(int k = 0; k < nfit+1 ; ++k )  {
	error2 += model(j,i)*covTI(j,k)*model(k,i);
      }    
    }
    double ef_i2 = ef[i]*ef[i];
    double inv_ef_i2 = 1./ef_i2;

    resi[i] = ypred - yf[i];
    pulli[i] = resi[i]/ef[i];

    // errf propagated error and pullf
    errf[i] = sqrt(error2);
    pullf[i] = resi[i]/errf[i];

    // calculate residual without hit and error without hit
    //    Think of Kalmanm method to exclude hit and error
    double err2invOut = 1./error2 - inv_ef_i2;
    if (err2invOut > 0) {
      //        double errinvOut = sqrt(err2invOut);
      resiOut[i] = (ypred/error2 - yf[i]*inv_ef_i2)/err2invOut - yf[i];
      //        pullOut[i] = resiOut[i]*errinvOut;
      pullOut[i] = resiOut[i]/sqrt(1./err2invOut+ef_i2);
    }

    if (fabs(pullOut[i]) > fabs(pullmax) && i < nfit ) {
      imax = indexf[i];
      jmax = i;
      pullmax = pullOut[i];
    } 
    chi2 += resi[i]*resi[i]*inv_ef_i2;

    if (i < nfit ) {
      pull[indexf[i]] = pullOut[i];
    }
    if (m_debug&& i < nfit) std::cout << " i " << i << " index " << indexf[i] << " det " << select[indexf[i]] << " ypred " << ypred << " mst " << yf[i] << " residual " <<  resi[i] << " error " << ef[i] << " dist " << dist[i] << " hitz " << hitz[i] << " Pull " << pulli[i] << " Pullf " << pullf[i] << " resi out " << resiOut[i] << " pull out " << pullOut[i] << std::endl;   
    if (m_debug&& i > nfit) std::cout << " i " << i <<  " ypred " << ypred << " mst " << yf[i] << " residual " <<  resi[i] << " error " << ef[i] << std::endl;   
  }   
  r0 = r0 + t(0,0);
  phi = phi + t(1,0);

  if (m_debug ) std::cout << " delta phi " << t(1,0) << std::endl;
  if (m_debug && fabs(t(1,0))> 0.1 ) std::cout << " ALARM delta phi " << t(1,0) << std::endl;
 
  if(m_debug) std:: cout<< " Track parameters r0 " << r0 << " phi " << phi  <<   " chi2 " << chi2 << " jmax " << jmax << " imax " << imax << " pullmax " << pullmax << std::endl;     
      
}
  
