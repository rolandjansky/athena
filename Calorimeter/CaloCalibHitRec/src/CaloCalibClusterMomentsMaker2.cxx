/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloCalibClusterMomentsMaker2.cxx,v 1.16 2009-05-18 16:16:49 pospelov Exp $
//
// Description: see CaloCalibClusterMomentsMaker2.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker2.h"

//---------------
// C++ Headers --
//---------------
#include <iterator>
#include <sstream>
#include <set>

#include "CaloEvent/CaloCell.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "StoreGate/ReadHandle.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <math.h>
#include <CLHEP/Vector/LorentzVector.h>


using CLHEP::HepLorentzVector;
using CLHEP::MeV;
using CLHEP::cm;


//###############################################################################

CaloCalibClusterMomentsMaker2::CaloCalibClusterMomentsMaker2(const std::string& type, 
							   const std::string& name,
							   const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_calo_dd_man(0),
    m_calo_id(0),
    m_caloDM_ID(0),
    m_caloDmDescrManager(0),
    m_useParticleID(true),
    m_energyMin(200*MeV),
    m_energyMinCalib(20*MeV),
    m_apars_alpha(0.5),
    m_apars_r0(0.2),
    m_MatchDmType(kMatchDmLoose)
{
  declareInterface<CaloClusterCollectionProcessor> (this);
  // Name(s) of Moments to calculate
  declareProperty("MomentsNames",m_momentsNames);
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_TOT"),xAOD::CaloCluster::ENG_CALIB_TOT));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_OUT_L"),xAOD::CaloCluster::ENG_CALIB_OUT_L));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_OUT_M"),xAOD::CaloCluster::ENG_CALIB_OUT_M));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_OUT_T"),xAOD::CaloCluster::ENG_CALIB_OUT_T));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_EMB0"),xAOD::CaloCluster::ENG_CALIB_EMB0));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_EME0"),xAOD::CaloCluster::ENG_CALIB_EME0));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_TILEG3"),xAOD::CaloCluster::ENG_CALIB_TILEG3));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TOT"),xAOD::CaloCluster::ENG_CALIB_DEAD_TOT));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_EMB0"),xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TILE0"),xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TILEG3"),xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_EME0"),xAOD::CaloCluster::ENG_CALIB_DEAD_EME0));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_HEC0"),xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_FCAL"),xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_LEAKAGE"),xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_UNCLASS"),xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_FRAC_EM"),xAOD::CaloCluster::ENG_CALIB_FRAC_EM));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_FRAC_HAD"),xAOD::CaloCluster::ENG_CALIB_FRAC_HAD));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_FRAC_REST"),xAOD::CaloCluster::ENG_CALIB_FRAC_REST));

  // Name(s) of Moments which can be stored on the AOD - all others go to ESD
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_TOT"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_OUT_L"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_OUT_M"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_OUT_T"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_EMB0"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_EME0"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_TILEG3"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_TOT"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_EMB0"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_TILE0"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_TILEG3"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_EME0"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_HEC0"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_FCAL"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_LEAKAGE"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_DEAD_UNCLASS"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_FRAC_EM"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_FRAC_HAD"));
  m_momentsNamesAOD.push_back(std::string("ENG_CALIB_FRAC_REST"));

  declareProperty("AODMomentsNames",m_momentsNamesAOD);
  declareProperty("CalibrationHitContainerNames",m_CalibrationHitContainerNames);
  declareProperty("DMCalibrationHitContainerNames",m_DMCalibrationHitContainerNames);
  m_n_phi_out = 127; // not more than 127 since we store indices (-127,...-1,0,...,126) and have 8 bits only
  m_n_eta_out = 127;
  m_out_phi_max =  M_PI;
  m_out_eta_max =  6;

  m_rmaxOut[0] = 1.0;
  m_rmaxOut[1] = 0.5; 
  m_rmaxOut[2] = 0.3;

  for( int im=0;im<3;im++) {
    m_i_phi_eta[im] = new std::vector<CalibHitIPhiIEtaRange> [m_n_eta_out];
  }
  m_doDeadEnergySharing = false;
  m_foundAllContainers = false;
  m_doOutOfClusterL = false;
  m_doOutOfClusterM = false;
  m_doOutOfClusterT = false;
  m_doDeadL = false;
  m_doDeadM = false;
  m_doDeadT = false;
  m_doCalibFrac = false;

  declareProperty("MatchDmType",m_MatchDmType);

  declareProperty( "UseParticleID",m_useParticleID);
}


CaloCalibClusterMomentsMaker2::~CaloCalibClusterMomentsMaker2() {
  for( int im=0;im<3;im++) 
    delete[] m_i_phi_eta[im];
}


//###############################################################################

StatusCode CaloCalibClusterMomentsMaker2::initialize()
{
  ATH_MSG_INFO( "Initializing " << name()  );

  std::vector<std::string>::const_iterator mNameIter = m_momentsNames.begin(); 
  std::vector<std::string>::const_iterator mNameIterEnd = m_momentsNames.end(); 
  for(; mNameIter!=mNameIterEnd; mNameIter++) {
    moment_name_vector::const_iterator mVNameIter = m_validNames.begin(); 
    moment_name_vector::const_iterator mVNameIterEnd = m_validNames.end(); 
    bool isValid(false);
    for(; mVNameIter!=mVNameIterEnd; mVNameIter++) {
      if ( *mNameIter == mVNameIter->first ) {
	m_validMoments.insert(*mVNameIter);
	isValid = true;
	break;
      }
    }
    if ( !isValid) {
      msg() << MSG::ERROR << "Moment " << *mNameIter
	  << " is not a valid Moment name and will be ignored! "
	  << "Valid names are:";
      for (unsigned int i=0;i<m_validNames.size();i++) 
	msg() << (i==0?" ":", ") << m_validNames[i].first;
      msg() << endmsg;
    }
  }

  // to switch on clever DeadMaterial assignment procedure
  // and check if tight, medium and/or loose versions for out-of-cluster and 
  // simple dead-material assignment are wanted
  for(moment_name_set::const_iterator it=m_validMoments.begin(); it!= m_validMoments.end(); it++){
    switch ((*it).second) {
    case xAOD::CaloCluster::ENG_CALIB_DEAD_TOT:
      m_doDeadEnergySharing = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_OUT_L:
      m_doOutOfClusterL = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_OUT_M:
      m_doOutOfClusterM = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_OUT_T:
      m_doOutOfClusterT = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_FRAC_EM:
      m_doCalibFrac = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_FRAC_HAD:
      m_doCalibFrac = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_FRAC_REST:
      m_doCalibFrac = true;
      break;
    default:
      break;
    }
  }
  
  if(m_doCalibFrac && !m_useParticleID) {
    ATH_MSG_INFO( "Usage of ParticleID was switched off (UseParticleID==False), no ENG_CALIB_FRAC_* moments will be available"  );
    m_doCalibFrac = false;
  }

  mNameIter = m_momentsNamesAOD.begin(); 
  mNameIterEnd = m_momentsNamesAOD.end(); 
  for(; mNameIter!=mNameIterEnd; mNameIter++) {
    moment_name_set::const_iterator vMomentsIter = m_validMoments.begin();
    moment_name_set::const_iterator vMomentsIterEnd = m_validMoments.end();
    for( ; vMomentsIter != vMomentsIterEnd; vMomentsIter++ ) {
      if ( vMomentsIter->first == *mNameIter ) {
	m_momentsAOD.insert(vMomentsIter->second);
	break;
      }
    }
  }
  if( m_momentsAOD.find(xAOD::CaloCluster::ENG_CALIB_DEAD_TOT) != m_momentsAOD.end()) {
    m_doDeadEnergySharing = true;
  }

  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance();

  // dead material identifier description manager
  m_caloDmDescrManager = CaloDmDescrManager::instance(); 

  m_calo_id = m_calo_dd_man->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_caloDM_ID) );

  // initialize distance tables
  for(int jeta = 0;jeta<m_n_eta_out;jeta++) {
    double eta0 = (jeta+0.5) * (m_out_eta_max)/m_n_eta_out; 
    HepLorentzVector middle(1,0,0,1);
    middle.setREtaPhi(1./cosh(eta0),eta0,0);
    double x_rmaxOut[3];
    for (int im=0;im<3;im++) {
      x_rmaxOut[im] = m_rmaxOut[im]*angle_mollier_factor(eta0);
    }
    for (int jp=-m_n_phi_out;jp<m_n_phi_out;jp++) {
      double phi = (jp+0.5) * m_out_phi_max/m_n_phi_out; 
      int ietaMin[3] = {m_n_eta_out,m_n_eta_out,m_n_eta_out};
      int ietaMax[3] = {-m_n_eta_out,-m_n_eta_out,-m_n_eta_out};
      for(int je = -m_n_eta_out;je<m_n_eta_out;je++) {
        double eta = (je+0.5) * m_out_eta_max/m_n_eta_out; 
        HepLorentzVector cpoint(1,0,0,1);
        cpoint.setREtaPhi(1./cosh(eta),eta,phi);
        double r = middle.angle(cpoint.vect());
        for (int im=0;im<3;im++) {
          if ( r < x_rmaxOut[im] ) {
            if ( je < ietaMin[im] ) 
              ietaMin[im] = je;
            if ( je > ietaMax[im] ) 
              ietaMax[im] = je;
          }
        }
      }
      for (int im=0;im<3;im++) {
        if ( ietaMin[im] <= ietaMax[im] ) {
          CalibHitIPhiIEtaRange theRange;
          theRange.iPhi = (char)jp;
          theRange.iEtaMin = (char)ietaMin[im];
          theRange.iEtaMax = (char)ietaMax[im];
          m_i_phi_eta[im][jeta].push_back(theRange); 
        }
      }
    }
  }

  ATH_CHECK( m_CalibrationHitContainerNames.initialize() );
  ATH_CHECK( m_DMCalibrationHitContainerNames.initialize() );

  ATH_CHECK(m_truthParticleContainerKey.initialize());

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode
CaloCalibClusterMomentsMaker2::execute(const EventContext& ctx,
                                       xAOD::CaloClusterContainer *theClusColl) const
{

  bool foundAllContainers (true);
  std::vector<const CaloCalibrationHitContainer *> v_cchc;
  for (const SG::ReadHandleKey<CaloCalibrationHitContainer>& key :
         m_CalibrationHitContainerNames)
  {
    SG::ReadHandle<CaloCalibrationHitContainer> cchc (key, ctx);
    if ( !cchc.isValid() ) {
      if (m_foundAllContainers) {
	// print ERROR message only if there was at least one event with 
	// all containers 
	msg(MSG::ERROR) << "SG does not contain calibration hit container " << key.key() << endmsg;
      }
      foundAllContainers = false;
    }
    else {
      v_cchc.push_back(cchc.cptr());
    }
  }

  std::vector<const CaloCalibrationHitContainer *> v_dmcchc;
  for (const SG::ReadHandleKey<CaloCalibrationHitContainer>& key :
         m_DMCalibrationHitContainerNames)
  {
    SG::ReadHandle<CaloCalibrationHitContainer> cchc (key, ctx);
    if ( !cchc.isValid() ) {
      if (m_foundAllContainers) {
	// print ERROR message only if there was at least one event with 
	// all containers 
	msg(MSG::ERROR) << "SG does not contain DM calibration hit container " << key.key() << endmsg;
      }
      foundAllContainers = false;
    }
    else {
      v_dmcchc.push_back(cchc.cptr());
    }
  }

  if ( !m_foundAllContainers && foundAllContainers ) 
    m_foundAllContainers = true;

  if ( !foundAllContainers ) return StatusCode::SUCCESS;

  // will contain detailed info about cluster calibration eneries
  ClusInfo_t clusInfoVec (theClusColl->size());

  CellInfoSet_t cellInfo;

  /* ********************************************
  filling the map with info which cell belongs to which cluster and what 
  is the cell weight in the cluster
  ******************************************** */
  xAOD::CaloClusterContainer::iterator clusIter = theClusColl->begin();
  xAOD::CaloClusterContainer::iterator clusIterEnd = theClusColl->end();
  int iClus = 0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
     const xAOD::CaloCluster * theCluster = (*clusIter);

    // loop over all cell members and fill cell vector for used cells
    xAOD::CaloCluster::const_cell_iterator cellIter    = theCluster->cell_begin();
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = theCluster->cell_end();
    for(; cellIter != cellIterEnd; cellIter++ ){
      const CaloCell* pCell = (*cellIter);
      Identifier myId = pCell->ID();

      MyCellInfo info (iClus, cellIter.weight() );
      CellInfoSet_t::iterator bookmark = cellInfo.lower_bound( myId );
      if(bookmark == cellInfo.end() || bookmark->first != myId) {
        // We haven't had a infohit in this cell before.  Add it to our set.
        if (bookmark != cellInfo.begin()) --bookmark;
        cellInfo.emplace_hint (bookmark, myId, std::move(info));
      }else{
        // Update the existing hit.
        bookmark->second.Add( info );
      }
    } // cellIter
  } // iClus


  /* ********************************************
  calculate total calib energy inside clusters
  ******************************************** */
  std::vector<const CaloCalibrationHitContainer * >::const_iterator it;
  unsigned int nHitsTotal = 0;
  unsigned int nHitsWithoutParticleID = 0;
  for (it=v_cchc.begin();it!=v_cchc.end();it++) {
    CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
    CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
    //loop over cells in calibration container
    for(;chIter!=chIterE;chIter++)  {
      Identifier myId = (*chIter)->cellID();

      CellInfoSet_t::iterator pos = cellInfo.find( myId );
      if(pos != cellInfo.end() ) {
        // i.e. given hit id belongs to one or more clusters
        CaloSampling::CaloSample nsmp = CaloSampling::CaloSample(m_calo_id->calo_sample(myId));
        for ( const std::pair<int, double>& p : pos->second.m_ClusWeights) {
          int iClus = p.first;
          double weight = p.second;
          if(m_useParticleID){
            clusInfoVec[iClus].Add(weight * (*chIter)->energyTotal(), nsmp, (*chIter)->particleID());
          }else{
            clusInfoVec[iClus].Add(weight * (*chIter)->energyTotal(), nsmp);
          }
        }
      }
      if( m_useParticleID && (*chIter)->particleID() == 0) {
        nHitsWithoutParticleID++;
      }
      nHitsTotal++;
    }
  }

  // if all calibration hits have ParticleID(i.e. barcode)==0 when simulation was done without ParticleID
  bool doCalibFrac = m_doCalibFrac;
  bool useParticleID = m_useParticleID;
  if(m_useParticleID && (nHitsTotal == nHitsWithoutParticleID) ) {
    msg(MSG::INFO) << "Calibration hits do not have ParticleID, i.e. barcode of particle caused hits is always 0. Continuing without ParticleID machinery."<< endmsg;
    useParticleID = false;
  }

  // reading particle information for later calcution of calibration enegry fraction caused
  // by particles of different types
  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainerReadHandle(m_truthParticleContainerKey);

  if (doCalibFrac && !truthParticleContainerReadHandle.isValid()){
    ATH_MSG_WARNING("Invalid read handle to TruthParticleContainer with key: " << m_truthParticleContainerKey.key());
    doCalibFrac = false;
  }

  std::vector<double> engCalibOut[3];

  /* ****************************************************************
  lists of clusters populating given area [eta*phi][iClus list]
  **************************************************************** */
  std::vector<std::vector <int > > clusListL;
  std::vector<std::vector <int > > clusListM;
  std::vector<std::vector <int > > clusListT;
  if ( m_doOutOfClusterL || m_doDeadL || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmLoose) )
    clusListL.resize((2*m_n_phi_out+1)*(2*m_n_eta_out+1));
  if ( m_doOutOfClusterM || m_doDeadM || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmMedium) )
    clusListM.resize((2*m_n_phi_out+1)*(2*m_n_eta_out+1));
  if ( m_doOutOfClusterT || m_doDeadT || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmTight) )
    clusListT.resize((2*m_n_phi_out+1)*(2*m_n_eta_out+1));

  for(unsigned int ii=0;ii<3;ii++) {
    if ( (ii == 0 && (m_doOutOfClusterL || m_doDeadL || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmLoose) ) ) || 
         (ii == 1 && (m_doOutOfClusterM || m_doDeadM || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmMedium) ) ) || 
         (ii == 2 && (m_doOutOfClusterT || m_doDeadT || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmTight)) ) ) {
      engCalibOut[ii].resize(theClusColl->size(),0);
      clusIter = theClusColl->begin();
      iClus = 0;
      for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
        const xAOD::CaloCluster * theCluster = (*clusIter);

        MyClusInfo& clusInfo = clusInfoVec[iClus];

        if ( clusInfo.engCalibIn.engTot > 0 ) {
          int iEtaSign = 1;
          if ( theCluster->eta() < 0 ) iEtaSign = -1;
          int jeta = (int)(floor(m_n_eta_out*(theCluster->eta()/(m_out_eta_max))));
          int jphi = (int)(floor(m_n_phi_out*(theCluster->phi())/(m_out_phi_max)));
          if ( jeta >= -m_n_eta_out && jeta < m_n_eta_out ) {
            if ( jphi < -m_n_phi_out ) jphi +=  2*m_n_phi_out;
            if ( jphi >=  m_n_phi_out ) jphi -=  2*m_n_phi_out;
            unsigned int iBin;
            unsigned int iEtaBin(jeta);
            if ( jeta < 0 ) iEtaBin = abs(jeta)-1;
            for (iBin=0;iBin<m_i_phi_eta[ii][iEtaBin].size();iBin++) {
              int jp = m_i_phi_eta[ii][iEtaBin][iBin].iPhi+jphi;
              if ( jp < -m_n_phi_out )  jp +=  2*m_n_phi_out;
              if ( jp >=  m_n_phi_out )  jp -=  2*m_n_phi_out;
              int jEtaMin = iEtaSign<0?-m_i_phi_eta[ii][iEtaBin][iBin].iEtaMax-1:m_i_phi_eta[ii][iEtaBin][iBin].iEtaMin;
              int jEtaMax = iEtaSign<0?-m_i_phi_eta[ii][iEtaBin][iBin].iEtaMin-1:m_i_phi_eta[ii][iEtaBin][iBin].iEtaMax;
              for( int je = jEtaMin;je<=jEtaMax;je++ ) {
              if(ii == 0 ) clusListL[(jp+m_n_phi_out)*(2*m_n_eta_out+1)+je+m_n_eta_out].push_back(iClus);
              else if(ii == 1 ) clusListM[(jp+m_n_phi_out)*(2*m_n_eta_out+1)+je+m_n_eta_out].push_back(iClus);
              else if(ii == 2 ) clusListT[(jp+m_n_phi_out)*(2*m_n_eta_out+1)+je+m_n_eta_out].push_back(iClus);
              }
            }
          }
        }
      }
    }
  }

  /* ****************************************************************
  calculate out-of-cluster energy of clusters
  **************************************************************** */
  if ( m_doOutOfClusterL  ||  m_doOutOfClusterM  ||  m_doOutOfClusterT  ) {
    for (it=v_cchc.begin();it!=v_cchc.end();it++) {
      CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
      CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
      //loop over cells in calibration container
      for(;chIter!=chIterE;chIter++)  {
        Identifier myId = (*chIter)->cellID();

          CellInfoSet_t::iterator pos = cellInfo.find( myId );
          if(pos == cellInfo.end() ) {
            // hit is not inside any cluster
            const CaloDetDescrElement* myCDDE = 
              m_calo_dd_man->get_element(myId);
            int pid(0);
            if(useParticleID) pid = (*chIter)->particleID();
            if ( myCDDE ) {
              int jeO = (int)floor(m_n_eta_out*(myCDDE->eta()/m_out_eta_max));
              if ( jeO >= -m_n_eta_out && jeO < m_n_eta_out ) {
                int jpO = (int)floor(m_n_phi_out*(myCDDE->phi()/m_out_phi_max));
                if ( jpO < -m_n_phi_out ) jpO +=  2*m_n_phi_out;
                if ( jpO >=  m_n_phi_out ) jpO -=  2*m_n_phi_out;
                for (unsigned int ii=0;ii<3;ii++) {
                  std::vector<std::vector <int > > *pClusList=0;
                  if ( ii == 0 && m_doOutOfClusterL ) 
                    pClusList = &clusListL;
                  else if ( ii == 1 && m_doOutOfClusterM ) 
                    pClusList = &clusListM;
                  else if ( ii == 2 && m_doOutOfClusterT ) 
                    pClusList = &clusListT;
                  if ( pClusList) {
                    // loop over list of potential neighboring clusters
                    double hitClusNorm(0.);
                    std::vector<int> hitClusIndex;
                    std::vector<double > hitClusEffEnergy;
                    // loop over clusters which match given OOC hit
                    for(unsigned int i_cls=0; i_cls<(*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size(); i_cls++){
                      int iClus = (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out][i_cls];
                      MyClusInfo& clusInfo = clusInfoVec[iClus];
                      // getting access to calibration energy inside cluster caused by same particleID (barcode)
                      // as given OOC hit
                      std::map<int, MyClusInfo::ClusCalibEnergy>::iterator pos = clusInfo.engCalibParticle.find(pid);
                      if(pos!=clusInfo.engCalibParticle.end()) {
                        // given cluster have some energy inside caused by same particle as given OOC hitClusEffEnergy
                        // so the hit will be assigned to this cluster with some weight
                        hitClusNorm += pos->second.engTot;
                        hitClusEffEnergy.push_back(pos->second.engTot);
                        hitClusIndex.push_back(iClus);
                      }
                    }
                    if ( hitClusNorm > 0 ) {
                      const double inv_hitClusNorm = 1. / hitClusNorm;
                      for(unsigned int i_cls=0; i_cls<hitClusIndex.size(); i_cls++){
                        int iClus = hitClusIndex[i_cls];
                        double w = hitClusEffEnergy[i_cls] * inv_hitClusNorm;
                        engCalibOut[ii][iClus] += w*(*chIter)->energyTotal();
                    }
                  }
                }
              }
            }
          }
        } // hits not in cluster
      }
    }
  }

  // ------------------------------------------------------------------------
  // calculate dead-material energy of clusters (way2)
  // + energy is shared among clusters within certain area
  // + distance to clusters and energy in specific samplings are used as sharing criteria
  // + calculations are done separately for different dead material areas
  std::vector<std::vector <int > > *pClusList=0;
  if ( m_MatchDmType == kMatchDmLoose ) {
    pClusList = &clusListL;
  } else if ( m_MatchDmType == kMatchDmMedium ) {
    pClusList = &clusListM;
  } else if ( m_MatchDmType == kMatchDmTight ) {
    pClusList = &clusListT;
  }
  if( m_doDeadEnergySharing && pClusList) {

    for (it=v_dmcchc.begin();it!=v_dmcchc.end();it++) {
      CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
      CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
      //loop over cells in DM calibration container
      for(;chIter!=chIterE;chIter++)  {
        Identifier myId = (*chIter)->cellID();
        if (m_calo_id->is_lar_dm(myId) || m_calo_id->is_tile_dm(myId)) {
          CaloDmDescrElement* myCDDE(0);
          myCDDE = m_caloDmDescrManager->get_element(myId);
          if ( myCDDE ) {
            int pid(0);
            if(useParticleID) pid = (*chIter)->particleID();

            int jeO = (int)floor(m_n_eta_out*(myCDDE->eta()/m_out_eta_max));
            if ( jeO >= -m_n_eta_out && jeO < m_n_eta_out ) {
              int jpO = (int)floor(m_n_phi_out*(myCDDE->phi()/m_out_phi_max));
              if ( jpO < -m_n_phi_out ) jpO +=  2*m_n_phi_out;
              if ( jpO >=  m_n_phi_out ) jpO -=  2*m_n_phi_out;

              /* ****************************************
              share energy of DM hit among different clusters
              ***************************************** */
              int nDmArea = m_caloDmDescrManager->get_dm_area(myId);
              const CaloDmRegion *dmRegion = m_caloDmDescrManager->get_dm_region(myId);
	      int hitClusVecIndex = 0;
              std::vector<int> hitClusIndex(theClusColl->size());
              std::vector<double > hitClusEffEnergy(theClusColl->size());
              double hitClusNorm = 0.0;
              // loop over clusters which match given DM hit
              for(unsigned int i_cls=0; i_cls< (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size(); i_cls++){
		int iClus = (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out][i_cls];
                xAOD::CaloCluster * theCluster = theClusColl->at(iClus);
                
                MyClusInfo& clusInfo = clusInfoVec[iClus];
                // getting access to calibration energy inside cluster caused by same particleID (barcode)
                // as given OOC hit
                std::map<int, MyClusInfo::ClusCalibEnergy>::iterator pos = clusInfo.engCalibParticle.find(pid);
                if(pos!=clusInfo.engCalibParticle.end()) {
                  double engClusPidCalib = pos->second.engTot;

                  if(engClusPidCalib > m_energyMinCalib  && theCluster->e()>m_energyMin) {
                    double sum_smp_energy = 0.0;
                    // loop over calo sampling numbers registered for given DM area
                    for(unsigned int i_smp=0; i_smp<dmRegion->m_CaloSampleNeighbours.size(); i_smp++) {
                      CaloSampling::CaloSample nsmp = (CaloSampling::CaloSample) dmRegion->m_CaloSampleNeighbours[i_smp];
		      if( (dmRegion->m_CaloSampleEtaMin[i_smp]-0.5) <= theCluster->eta()  &&
                          theCluster->eta() <= (dmRegion->m_CaloSampleEtaMax[i_smp]+0.5) ){
                        //sum_smp_energy += theCluster->eSample(nsmp);
                        sum_smp_energy += pos->second.engSmp[nsmp];
                      }
                    }
                    if(sum_smp_energy > 0.0) {
                      double phi_diff=myCDDE->phi()-theCluster->phi();
                      if(phi_diff <= -M_PI){
                        phi_diff += 2.*M_PI;
                      } else if (phi_diff > M_PI){
                        phi_diff -= 2.*M_PI;
                      }
                      double eta_diff = (myCDDE->eta()-theCluster->eta());
		      float distance=sqrt(eta_diff * eta_diff + phi_diff * phi_diff);

                      double effEner = pow(sum_smp_energy,m_apars_alpha)*exp(-distance/m_apars_r0);
					  
                      hitClusIndex [hitClusVecIndex] = iClus;
                      hitClusEffEnergy [hitClusVecIndex++]= effEner;
                      hitClusNorm += effEner;
                    }
                  } // good energetic cluster
		} // we ve found a pid
              } // loop over clusters in the list
              hitClusIndex.resize(hitClusVecIndex);
              hitClusEffEnergy.resize(hitClusVecIndex);


              // now we have to calculate weight for assignment hit energy to cluster
              if(hitClusNorm >0.0) {
                const double inv_hitClusNorm = 1. / hitClusNorm;
                for(unsigned int i_cls=0; i_cls<hitClusIndex.size(); i_cls++){
                  int iClus = hitClusIndex[i_cls];
                  double dm_weight = hitClusEffEnergy[i_cls] * inv_hitClusNorm;
                  if(dm_weight > 1.0 || dm_weight < 0.0 ){
                    std::cout << "CaloCalibClusterMomentsMaker2::execute() ->Error! Strange weight " <<  dm_weight<< std::endl;
                    std::cout << hitClusEffEnergy[i_cls] << " " << hitClusNorm << std::endl;
                  }
                  clusInfoVec[iClus].engCalibDeadInArea[nDmArea] += (*chIter)->energyTotal()*dm_weight;
                  clusInfoVec[iClus].engCalibDeadInArea[CaloDmDescrArea::DMA_ALL] += (*chIter)->energyTotal()*dm_weight;
                }
              } // hit clus norm

            } // loop over united ieta*iphi
          } // myCDDE
        } // tile and lar
      } // eol over DM calibration hits
    } //eol over containers

  } // doDeadEnergySharing

  // fraction of calibration energies caused by different particles
  std::vector<double> engCalibFrac;
  engCalibFrac.resize(kCalibFracMax, 0.0);

  std::map<unsigned int,int> truthBarcodeToPdgCodeMap;
  
  //loop on truth particle container is slow, so put needed information in a map for faster key lookup in later loops
  for ( auto thisTruthParticle : *truthParticleContainerReadHandle){	  

    if (!thisTruthParticle){
      ATH_MSG_WARNING("Got invalid pointer to TruthParticle");
      continue;
    }
    
    truthBarcodeToPdgCodeMap[thisTruthParticle->barcode()] = thisTruthParticle->pdgId();    
  }//truth particle loop
  
  // assign moments
  for( clusIter = theClusColl->begin(),iClus=0;  clusIter!=clusIterEnd;clusIter++,iClus++) {
    xAOD::CaloCluster * theCluster = *clusIter;
    MyClusInfo& clusInfo = clusInfoVec[iClus];

    // total DM energy assigned to cluster
    double eng_calib_dead_tot = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_ALL]
          + clusInfo.engCalibIn.engSmp[CaloSampling::PreSamplerB]
          + clusInfo.engCalibIn.engSmp[CaloSampling::PreSamplerE]
          + clusInfo.engCalibIn.engSmp[CaloSampling::TileGap3];
    // DM energy before barrel presampler, inside it, and between presampler and strips
    double eng_calib_dead_emb0 = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_EMB0]
          + clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_EMB1]
          + clusInfo.engCalibIn.engSmp[CaloSampling::PreSamplerB];
    // DM energy between barrel and tile
    double eng_calib_dead_tile0 = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_EMB3_TILE0];
    // DM energy before scintillator and inside scintillator
    double eng_calib_dead_tileg3 = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_SCN]
          + clusInfo.engCalibIn.engSmp[CaloSampling::TileGap3];
    // DM energy beforee endcap presampler, inside it and between presampler and strips
    double eng_calib_dead_eme0 = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_EME0]
          + clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_EME12]
          + clusInfo.engCalibIn.engSmp[CaloSampling::PreSamplerE];
    // DM energy between emec and hec
    double eng_calib_dead_hec0 = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_EME3_HEC0];
    // DM energy before FCAL and between HEC and FCAL
    double eng_calib_dead_fcal = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_FCAL0]
          + clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_HEC_FCAL];
    // DM leakage behind the calorimeter
    double eng_calib_dead_leakage = clusInfo.engCalibDeadInArea[CaloDmDescrArea::DMA_LEAK];
    // the rest of DM energy which remains unclassified
    double eng_calib_dead_unclass = eng_calib_dead_tot - eng_calib_dead_emb0 - eng_calib_dead_tile0 
          - eng_calib_dead_tileg3 - eng_calib_dead_eme0 - eng_calib_dead_hec0 - eng_calib_dead_fcal
          - eng_calib_dead_leakage;

    if(doCalibFrac){
      get_calib_frac(truthBarcodeToPdgCodeMap, clusInfo, engCalibFrac);
    }

    if ( m_momentsNames.size() > 0 ) {
      std::vector<double> myMoments(m_validMoments.size(),0);
      // assign moments 
      moment_name_set::const_iterator vMomentsIter = m_validMoments.begin(); 
      moment_name_set::const_iterator vMomentsIterEnd = m_validMoments.end();

      int iMoment=0;
      for(; vMomentsIter!=vMomentsIterEnd; vMomentsIter++,iMoment++) {
        // now calculate the actual moments
        switch (vMomentsIter->second) { 
        case xAOD::CaloCluster::ENG_CALIB_TOT:
          myMoments[iMoment] = clusInfo.engCalibIn.engTot;
          break;
        case xAOD::CaloCluster::ENG_CALIB_OUT_L:
          myMoments[iMoment] = engCalibOut[0][iClus];
          break;
        case xAOD::CaloCluster::ENG_CALIB_OUT_M:
          myMoments[iMoment] = engCalibOut[1][iClus];
          break;
        case xAOD::CaloCluster::ENG_CALIB_OUT_T:
          myMoments[iMoment] = engCalibOut[2][iClus];
          break;
        case xAOD::CaloCluster::ENG_CALIB_EMB0:
          myMoments[iMoment] = clusInfo.engCalibIn.engSmp[CaloSampling::PreSamplerB];
          break;
        case xAOD::CaloCluster::ENG_CALIB_EME0:
          myMoments[iMoment] = clusInfo.engCalibIn.engSmp[CaloSampling::PreSamplerE];
          break;
        case xAOD::CaloCluster::ENG_CALIB_TILEG3:
          myMoments[iMoment] = clusInfo.engCalibIn.engSmp[CaloSampling::TileGap3];
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_TOT:
          myMoments[iMoment] = eng_calib_dead_tot;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0:
          myMoments[iMoment] = eng_calib_dead_emb0;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0:
          myMoments[iMoment] = eng_calib_dead_tile0;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3:
          myMoments[iMoment] = eng_calib_dead_tileg3;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_EME0:
          myMoments[iMoment] = eng_calib_dead_eme0;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0:
          myMoments[iMoment] = eng_calib_dead_hec0;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL:
          myMoments[iMoment] = eng_calib_dead_fcal;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE:
          myMoments[iMoment] = eng_calib_dead_leakage;
          break;
        case xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS:
          myMoments[iMoment] = eng_calib_dead_unclass;
          break;
        case xAOD::CaloCluster::ENG_CALIB_FRAC_EM:
          myMoments[iMoment] = engCalibFrac[kCalibFracEM];
          break;
        case xAOD::CaloCluster::ENG_CALIB_FRAC_HAD:
          myMoments[iMoment] = engCalibFrac[kCalibFracHAD];
          break;
        case xAOD::CaloCluster::ENG_CALIB_FRAC_REST:
          myMoments[iMoment] = engCalibFrac[kCalibFracREST];
          break;
        default:
          // nothing to be done for other moments
          break;
        }

        theCluster->insertMoment(vMomentsIter->second, myMoments[iMoment]);
      }
    }
  }

  return StatusCode::SUCCESS;
}



/* ****************************************************************************

**************************************************************************** */
double CaloCalibClusterMomentsMaker2::angle_mollier_factor(double x) const
{
  double eta = fabs(x);
  double ff;
  if(eta<1.6){
    ff = atan(5.0*1.7/(200.0*cosh(eta)));
  }else if(eta<3.2){
    ff = atan(5.0*1.6/(420./tanh(eta)));
  }else{
    ff = atan(5.0*0.95/(505./tanh(eta)));
  }
  return ff*(1./atan(5.0*1.7/200.0));
}



/* ****************************************************************************
Calculation of energy fraction caused by particles of different types
**************************************************************************** */
void CaloCalibClusterMomentsMaker2::get_calib_frac(const std::map<unsigned int,int>& truthBarcodeToPdgCodeMap,
                                                   const MyClusInfo& clusInfo, std::vector<double> &engFrac) const
{
  static unsigned int nWarnings = 0;
  engFrac.resize(kCalibFracMax, 0.0);
  if(clusInfo.engCalibIn.engTot <= 0.0) return;
  // each MyClusInfo has a map of particle's barcode and particle calibration deposits in given cluster
  for(std::map<int, MyClusInfo::ClusCalibEnergy >::const_iterator it = clusInfo.engCalibParticle.begin(); it != clusInfo.engCalibParticle.end(); it++){
    unsigned int barcode = it->first;
    int pdg_id = 0;
    try { pdg_id = truthBarcodeToPdgCodeMap.at(barcode); }
    catch (const std::out_of_range& e){
      if (nWarnings < 10 ){
	ATH_MSG_WARNING("truthBarcodeToPdgCodeMap cannot find an entry with barcode " << barcode);
	nWarnings++;
      }
      continue;
    }

    if( abs(pdg_id) == 211) {
      engFrac[kCalibFracHAD] += it->second.engTot;
    }else if( pdg_id == 111 || pdg_id == 22 || abs(pdg_id)==11) {
      engFrac[kCalibFracEM] += it->second.engTot;
    }else{
      engFrac[kCalibFracREST] += it->second.engTot;
    }
  }
  for(unsigned int i=0; i<engFrac.size(); i++) engFrac[i] = engFrac[i]/clusInfo.engCalibIn.engTot;
}

