/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloCalibClusterMomentsMaker.cxx,v 1.16 2009-05-18 16:16:49 pospelov Exp $
//
// Description: see CaloCalibClusterMomentsMaker.h
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
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker.h"

//---------------
// C++ Headers --
//---------------
#include <iterator>
#include <sstream>
#include <set>

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDmDetDescr/CaloDmDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

#include "StoreGate/ReadHandle.h" 
#include "CLHEP/Units/SystemOfUnits.h"

#include <CLHEP/Geometry/Vector3D.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <cmath>


using CLHEP::HepLorentzVector;
using CLHEP::MeV;
using CLHEP::cm;


//###############################################################################

CaloCalibClusterMomentsMaker::CaloCalibClusterMomentsMaker(const std::string& type, 
							   const std::string& name,
							   const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_calo_id(nullptr),
    m_caloDM_ID(nullptr),
    m_caloDmDescrManager(nullptr),
    m_energyMin(200*MeV),
    m_energyMinCalib(20*MeV),
    m_apars_alpha(0.5),
    m_apars_r0(0.2),
    //m_showerScale(25*cm),
    m_MatchDmType(kMatchDmMedium)
{
  declareInterface<CaloClusterCollectionProcessor> (this);
  // Name(s) of Moments to calculate
  declareProperty("MomentsNames",m_momentsNames);
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_TOT"),xAOD::CaloCluster::ENG_CALIB_TOT));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_OUT_L"),xAOD::CaloCluster::ENG_CALIB_OUT_L));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_OUT_M"),xAOD::CaloCluster::ENG_CALIB_OUT_M));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_OUT_T"),xAOD::CaloCluster::ENG_CALIB_OUT_T));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_L"),xAOD::CaloCluster::ENG_CALIB_DEAD_L));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_M"),xAOD::CaloCluster::ENG_CALIB_DEAD_M));
  m_validNames.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_T"),xAOD::CaloCluster::ENG_CALIB_DEAD_T));
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

  // Name(s) of Moments which can be stored on the AOD - all others go to ESD
  m_momentsNamesAOD.emplace_back("ENG_CALIB_TOT");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_OUT_L");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_OUT_M");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_OUT_T");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_L");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_M");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_T");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_EMB0");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_EME0");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_TILEG3");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_TOT");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_EMB0");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_TILE0");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_TILEG3");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_EME0");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_HEC0");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_FCAL");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_LEAKAGE");
  m_momentsNamesAOD.emplace_back("ENG_CALIB_DEAD_UNCLASS");

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

  declareProperty("MatchDmType",m_MatchDmType);
}


CaloCalibClusterMomentsMaker::~CaloCalibClusterMomentsMaker() {
  for( int im=0;im<3;im++) 
    delete[] m_i_phi_eta[im];
}


//###############################################################################

StatusCode CaloCalibClusterMomentsMaker::initialize()
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
    case xAOD::CaloCluster::ENG_CALIB_DEAD_L:
      m_doDeadL = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_DEAD_M:
      m_doDeadM = true;
      break;
    case xAOD::CaloCluster::ENG_CALIB_DEAD_T:
      m_doDeadT = true;
      break;
    default:
      break;
    }
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

  // dead material identifier description manager  
  m_caloDmDescrManager = CaloDmDescrManager::instance(); 

  ATH_CHECK( detStore()->retrieve(m_calo_id, "CaloCell_ID") ); 
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

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode
CaloCalibClusterMomentsMaker::execute(const EventContext& ctx,
                                      xAOD::CaloClusterContainer *theClusColl) const
{  
  ATH_MSG_DEBUG( "Executing " << name()  );

  const CaloDetDescrManager* calo_dd_man = nullptr;
  ATH_CHECK( detStore()->retrieve (calo_dd_man, "CaloMgr") );

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
	ATH_MSG_ERROR( "SG does not contain calibration hit container " << key.key()  );
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
	ATH_MSG_ERROR( "SG does not contain DM calibration hit container " << key.key()  );
      }
      foundAllContainers = false;
    }
    else {
      v_dmcchc.push_back(cchc.cptr());
    }
  }

  if ( !m_foundAllContainers && foundAllContainers ) 
    m_foundAllContainers = true;

  if ( foundAllContainers ) {
    std::vector<ClusWeight *> cellVector[CaloCell_ID::NSUBCALO];
  
    for(int ic=0;ic<CaloCell_ID::NSUBCALO; ic++) {
      unsigned int maxHashSize(0);
      IdentifierHash myHashMin,myHashMax;
      m_calo_id->calo_cell_hash_range (ic,myHashMin,myHashMax);
      maxHashSize = myHashMax-myHashMin;
      cellVector[ic].resize(maxHashSize,nullptr);
    }

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
	IdentifierHash myHashId;
	int otherSubDet(-1);
	myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);
	ClusWeight * myClus = new ClusWeight();
	myClus->iClus = iClus;
	myClus->weight = cellIter.weight();
	myClus->next = nullptr;
	ClusWeight * theList = cellVector[otherSubDet][(unsigned int)myHashId];
	if ( theList ) {
	  while ( theList->next ) 
	    theList = theList->next;
	  theList->next = myClus;
	}
	else {
	  cellVector[otherSubDet][(unsigned int)myHashId] = myClus;
	}
      }
    }

    // calculate calib hit moments
    std::vector<double> engCalibTot(theClusColl->size(),0);
    std::vector<double> engCalibOut[3];
    std::vector<double> engCalibDead[3];
    double eTot(0),eOut[3],eDead[3];

    // calibration energy in samplings [iClus][CaloSample]
    std::vector<std::vector<double> > engCalibTotSmp;
    engCalibTotSmp.resize(theClusColl->size());
    for(unsigned int i_cls=0; i_cls<theClusColl->size(); i_cls++){
      engCalibTotSmp[i_cls].resize(CaloSampling::Unknown, 0.0);
    }
    // dead material energy in different areas [iClus][nDmArea]
    std::vector<std::vector<double> > engCalibDeadInArea;
    engCalibDeadInArea.resize(theClusColl->size());
    for(unsigned int i_cls=0; i_cls<theClusColl->size(); i_cls++){
      engCalibDeadInArea[i_cls].resize(CaloDmDescrArea::DMA_MAX,0.0);
    }
    // lists of clusters populating given area [eta*phi][iClus list]
    std::vector<std::vector <int > > clusListL;
    std::vector<std::vector <int > > clusListM;
    std::vector<std::vector <int > > clusListT;
    if ( m_doOutOfClusterL || m_doDeadL || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmLoose) )
      clusListL.resize((2*m_n_phi_out+1)*(2*m_n_eta_out+1));
    if ( m_doOutOfClusterM || m_doDeadM || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmMedium) )
      clusListM.resize((2*m_n_phi_out+1)*(2*m_n_eta_out+1));
    if ( m_doOutOfClusterT || m_doDeadT || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmTight) )
      clusListT.resize((2*m_n_phi_out+1)*(2*m_n_eta_out+1));

    // calculate total calib energy of clusters
    std::vector<const CaloCalibrationHitContainer * >::const_iterator it;
    for (it=v_cchc.begin();it!=v_cchc.end();it++) {
      CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
      CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
      //loop over cells in calibration container
      for(;chIter!=chIterE;chIter++)  {
	Identifier myId = (*chIter)->cellID();
        CaloSampling::CaloSample nsmp = CaloSampling::CaloSample(m_calo_id->calo_sample(myId));
	int otherSubDet;
	IdentifierHash myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);
	if ( myHashId != CaloCell_ID::NOT_VALID ) {
	  ClusWeight * theList = cellVector[otherSubDet][(unsigned int)myHashId];
	  while ( theList ) {
	    engCalibTot[theList->iClus] += theList->weight * (*chIter)->energyTotal();
	    eTot += theList->weight * (*chIter)->energyTotal();
            if( nsmp < CaloSampling::Unknown ) {
              engCalibTotSmp[theList->iClus][nsmp] += theList->weight * (*chIter)->energyTotal();
            }
	    theList = theList->next;
	  }
	}
      }
    }

    for(unsigned int ii=0;ii<3;ii++) {
      if ( (ii == 0 && (m_doOutOfClusterL || m_doDeadL || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmLoose) ) ) || 
	   (ii == 1 && (m_doOutOfClusterM || m_doDeadM || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmMedium) ) ) || 
	   (ii == 2 && (m_doOutOfClusterT || m_doDeadT || (m_doDeadEnergySharing && m_MatchDmType==kMatchDmTight)) ) ) {
	engCalibOut[ii].resize(theClusColl->size(),0);
	engCalibDead[ii].resize(theClusColl->size(),0);
	eOut[ii] = 0;
	eDead[ii] = 0;
	clusIter = theClusColl->begin();
	iClus = 0;
	for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
           xAOD::CaloCluster * theCluster = (*clusIter);
	  
	  if ( engCalibTot[iClus] > 0 ) {
	    int iEtaSign = 1;
	    if ( theCluster->eta() < 0 )
	      iEtaSign = -1;
	    int jeta = (int)(floor(m_n_eta_out*(theCluster->eta()/(m_out_eta_max))));
	    int jphi = (int)(floor(m_n_phi_out*(theCluster->phi())/(m_out_phi_max)));
	    if ( jeta >= -m_n_eta_out && jeta < m_n_eta_out ) {
	      if ( jphi < -m_n_phi_out ) 
		jphi +=  2*m_n_phi_out;
	      if ( jphi >=  m_n_phi_out ) 
		jphi -=  2*m_n_phi_out;
	      unsigned int iBin;
	      unsigned int iEtaBin(jeta);
	      if ( jeta < 0 )
		iEtaBin = abs(jeta)-1;
	      for (iBin=0;iBin<m_i_phi_eta[ii][iEtaBin].size();iBin++) {
		int jp = m_i_phi_eta[ii][iEtaBin][iBin].iPhi+jphi;
		if ( jp < -m_n_phi_out ) 
		  jp +=  2*m_n_phi_out;
		if ( jp >=  m_n_phi_out ) 
		  jp -=  2*m_n_phi_out;
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
      
    if ( m_doOutOfClusterL  || 
	 m_doOutOfClusterM  || 
	 m_doOutOfClusterT  ) {
      // calculate out-of-cluster energy of clusters
      for (it=v_cchc.begin();it!=v_cchc.end();it++) {
	CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
	CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
	//loop over cells in calibration container
	for(;chIter!=chIterE;chIter++)  {
	  Identifier myId = (*chIter)->cellID();
	  int otherSubDet;
	  IdentifierHash myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);
	  if ( myHashId != CaloCell_ID::NOT_VALID ) {
	    if ( !cellVector[otherSubDet][(unsigned int)myHashId] ) {
	      // hit is not inside any cluster
	      const CaloDetDescrElement* myCDDE = 
                calo_dd_man->get_element(myId);
	      if ( myCDDE ) {
		int jeO = (int)floor(m_n_eta_out*(myCDDE->eta()/m_out_eta_max));
		if ( jeO >= -m_n_eta_out && jeO < m_n_eta_out ) {
		  int jpO = (int)floor(m_n_phi_out*(myCDDE->phi()/m_out_phi_max));
		  if ( jpO < -m_n_phi_out ) 
		    jpO +=  2*m_n_phi_out;
		  if ( jpO >=  m_n_phi_out ) 
		    jpO -=  2*m_n_phi_out;
		  // HepVector3D hitPos(myCDDE->x(),myCDDE->y(),myCDDE->z());
		  for (unsigned int ii=0;ii<3;ii++) {
		    std::vector<std::vector <int > > *pClusList=nullptr;
  
		    if ( ii == 0 && m_doOutOfClusterL ) 
		      pClusList = &clusListL;
		    else if ( ii == 1 && m_doOutOfClusterM ) 
		      pClusList = &clusListM;
		    else if ( ii == 2 && m_doOutOfClusterT ) 
		      pClusList = &clusListT;
		    if ( pClusList) {
		      // loop over list of potential neighboring clusters
		      double hitClusNorm(0.);
		      // loop over clusters which match given OOC hit
		      for(unsigned int i_cls=0; i_cls<(*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size(); i_cls++){
			int iClus = (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out][i_cls];
			hitClusNorm += engCalibTot[iClus];
		      }
		      if ( hitClusNorm > 0 ) {
                        const double inv_hitClusNorm = 1. / hitClusNorm;
			for(unsigned int i_cls=0; i_cls<(*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size(); i_cls++){
			  int iClus = (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out][i_cls];
			  double w = engCalibTot[iClus] * inv_hitClusNorm;
			  engCalibOut[ii][iClus] += w*(*chIter)->energyTotal();
			  eOut[ii] += w*(*chIter)->energyTotal();
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    if ( m_doDeadL  || 
	 m_doDeadM  || 
	 m_doDeadT  ) {
      // calculate dead-material energy of clusters
      for (it=v_dmcchc.begin();it!=v_dmcchc.end();it++) {
	CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
	CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
	//loop over cells in DM calibration container
	for(;chIter!=chIterE;chIter++)  {
	  Identifier myId = (*chIter)->cellID();
	  if (m_calo_id->is_lar_dm(myId) || m_calo_id->is_tile_dm(myId)) {
	    CaloDmDescrElement* myCDDE(nullptr);
	    myCDDE = m_caloDmDescrManager->get_element(myId);
	    if ( myCDDE ) {
	      int jeO = (int)floor(m_n_eta_out*(myCDDE->eta()/m_out_eta_max));
	      if ( jeO >= -m_n_eta_out && jeO < m_n_eta_out ) {
		int jpO = (int)floor(m_n_phi_out*(myCDDE->phi()/m_out_phi_max));
		if ( jpO < -m_n_phi_out ) 
		  jpO +=  2*m_n_phi_out;
		if ( jpO >=  m_n_phi_out ) 
		  jpO -=  2*m_n_phi_out;
		for (unsigned int ii=0;ii<3;ii++) {
		  std::vector<std::vector <int > > *pClusList=nullptr;
		  if ( ii == 0 && m_doDeadL ) 
		    pClusList = &clusListL;
		  else if ( ii == 1 && m_doDeadM ) 
		    pClusList = &clusListM;
		  else if ( ii == 2 && m_doDeadT ) 
		    pClusList = &clusListT;
		  if ( pClusList) {
		    // loop over list of potential neighboring clusters
		    std::vector<double > hitClusEffEnergy((*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size());
		    double hitClusNorm(0.);
		    // loop over clusters which match given DM hit
		    for(unsigned int i_cls=0; i_cls<(*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size(); i_cls++){
		      int iClus = (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out][i_cls];
                      xAOD::CaloCluster * theCluster = theClusColl->at(iClus);
		      
		      double dPhi = theCluster->phi()-myCDDE->phi();
		      if ( dPhi < -M_PI ) dPhi += 2*M_PI;
		      else if ( dPhi > M_PI ) dPhi -= 2*M_PI;
		      double dEta = theCluster->eta()-myCDDE->eta();
		      double dist = sqrt(dPhi*dPhi+dEta*dEta);
		      double w = 0;
		      w = engCalibTot[iClus]*exp(-dist/m_apars_r0);
		      hitClusNorm += w;
		      hitClusEffEnergy[i_cls]=w;
		    }
		    if ( hitClusNorm > 0 ) {
                      const double inv_hitClusNorm = 1. / hitClusNorm;
		      for(unsigned int i_cls=0; i_cls<(*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size(); i_cls++){
			int iClus = (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out][i_cls];
			double w = hitClusEffEnergy[i_cls] * inv_hitClusNorm;
			engCalibDead[ii][iClus] += w*(*chIter)->energyTotal();
			eDead[ii] += w*(*chIter)->energyTotal();
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    // ------------------------------------------------------------------------
    //
    // calculate dead-material energy of clusters (way2)
    // + energy is shared among clusters within certain area
    // + distance to clusters and energy in specific samplings are used as sharing criteria
    // + calculations are done separately for different dead material areas
    std::vector<std::vector <int > > *pClusList=nullptr;
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
            CaloDmDescrElement* myCDDE(nullptr);
            myCDDE = m_caloDmDescrManager->get_element(myId);
            if ( myCDDE ) {

              int jeO = (int)floor(m_n_eta_out*(myCDDE->eta()/m_out_eta_max));
              if ( jeO >= -m_n_eta_out && jeO < m_n_eta_out ) {
                int jpO = (int)floor(m_n_phi_out*(myCDDE->phi()/m_out_phi_max));
                if ( jpO < -m_n_phi_out ) 
                  jpO +=  2*m_n_phi_out;
                if ( jpO >=  m_n_phi_out ) 
                  jpO -=  2*m_n_phi_out;

              /* ****************************************
                share energy of DM hit among different clusters
              ***************************************** */
                int nDmArea = m_caloDmDescrManager->get_dm_area(myId);
                const CaloDmRegion *dmRegion = m_caloDmDescrManager->get_dm_region(myId);

                std::vector<int> hitClusIndex;
                std::vector<double > hitClusEffEnergy;
                double hitClusNorm = 0.0;
                // loop over clusters which match given DM hit
                for(unsigned int i_cls=0; i_cls< (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out].size(); i_cls++){
                  int iClus = (*pClusList)[(jpO+m_n_phi_out)*(2*m_n_eta_out+1)+jeO+m_n_eta_out][i_cls];
                  xAOD::CaloCluster * theCluster = theClusColl->at(iClus);

                  if(engCalibTot[iClus] > m_energyMinCalib  && theCluster->e()>m_energyMin) {
                    double sum_smp_energy = 0.0;
                    // loop over calo sampling numbers registered for given DM area
                    for(unsigned int i_smp=0; i_smp<dmRegion->m_CaloSampleNeighbours.size(); i_smp++) {
                       xAOD::CaloCluster::CaloSample nsmp = (xAOD::CaloCluster::CaloSample) dmRegion->m_CaloSampleNeighbours[i_smp];
                      if( (dmRegion->m_CaloSampleEtaMin[i_smp]-0.5) <= theCluster->eta()  &&
                           theCluster->eta() <= (dmRegion->m_CaloSampleEtaMax[i_smp]+0.5) ){
                        sum_smp_energy += theCluster->eSample(nsmp);
                      }
                    }
                    if(sum_smp_energy > 0.0) {
//                       HepLorentzVector hlv_cls(1,0,0,1);
//                       hlv_cls.setREtaPhi(1./cosh(theCluster->eta()),theCluster->eta(),theCluster->phi());
//                       HepLorentzVector hlv_hit(1,0,0,1);
//                       hlv_hit.setREtaPhi(1./cosh(myCDDE->eta()),myCDDE->eta(),myCDDE->phi());
//                       double x_angle = hlv_cls.angle(hlv_hit.vect());
//                       //double x_apars_r0 = m_apars_r0*angle_mollier_factor(theCluster->eta());
//                       double x_apars_r0 = m_apars_r0;
                      double phi_diff=myCDDE->phi()-theCluster->phi();
                      if(phi_diff <= -M_PI){
                        phi_diff += 2.*M_PI;
                      } else if (phi_diff > M_PI){
                        phi_diff -= 2.*M_PI;
                      }
                      float distance=sqrt(pow((myCDDE->eta()-theCluster->eta()),2)+pow(phi_diff,2));
                      double effEner = pow(sum_smp_energy,m_apars_alpha)*exp(-distance/m_apars_r0);
                      hitClusIndex.push_back(iClus);
                      hitClusEffEnergy.push_back(effEner);
                      hitClusNorm += effEner;
                    }
                  } // good energetic cluster
                } // loop over clusters in the list

                // now we have to calculate weight for assignment hit energy to cluster
                if(hitClusNorm > 0.0) {
                  const double inv_hitClusNorm = 1. / hitClusNorm;
                  for(unsigned int i_cls=0; i_cls<hitClusIndex.size(); i_cls++){
                    int iClus = hitClusIndex[i_cls];
                    double dm_weight = hitClusEffEnergy[i_cls] * inv_hitClusNorm;
                    if(dm_weight > 1.0 || dm_weight < 0.0 ){
                      std::cout << "CaloCalibClusterMomentsMaker::execute() ->Error! Strange weight " <<  dm_weight<< std::endl;
                      std::cout << hitClusEffEnergy[i_cls] << " " << hitClusNorm << std::endl;
                    }
                    engCalibDeadInArea[iClus][nDmArea] += (*chIter)->energyTotal()*dm_weight;
                    engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_ALL] += (*chIter)->energyTotal()*dm_weight;
                  }
                } // non zero hitClusNorm

              } // loop over united ieta*iphi
            } // myCDDE
          } // tile and lar
        } // eol over DM calibration hits
      } //eol over containers

    } // doDeadEnergySharing

    // assign moments
    for( clusIter = theClusColl->begin(),iClus=0;
	 clusIter!=clusIterEnd;clusIter++,iClus++) {
       xAOD::CaloCluster * theCluster = *clusIter;

      // total DM energy assigned to cluster
      double eng_calib_dead_tot = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_ALL]
            + engCalibTotSmp[iClus][CaloSampling::PreSamplerB]
            + engCalibTotSmp[iClus][CaloSampling::PreSamplerE]
            + engCalibTotSmp[iClus][CaloSampling::TileGap3];
      // DM energy before barrel presampler, inside it, and between presampler and strips
      double eng_calib_dead_emb0 = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_EMB0]
            + engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_EMB1]
            + engCalibTotSmp[iClus][CaloSampling::PreSamplerB];
      // DM energy between barrel and tile
      double eng_calib_dead_tile0 = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_EMB3_TILE0];
      // DM energy before scintillator and inside scintillator
      double eng_calib_dead_tileg3 = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_SCN]
            + engCalibTotSmp[iClus][CaloSampling::TileGap3];
      // DM energy beforee endcap presampler, inside it and between presampler and strips
      double eng_calib_dead_eme0 = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_EME0]
            + engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_EME12]
            + engCalibTotSmp[iClus][CaloSampling::PreSamplerE];
      // DM energy between emec and hec
      double eng_calib_dead_hec0 = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_EME3_HEC0];
      // DM energy before FCAL and between HEC and FCAL
      double eng_calib_dead_fcal = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_FCAL0]
            + engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_HEC_FCAL];
      // DM leakage behind the calorimeter
      double eng_calib_dead_leakage = engCalibDeadInArea[iClus][CaloDmDescrArea::DMA_LEAK];
      // the rest of DM energy which remains unclassified
      double eng_calib_dead_unclass = eng_calib_dead_tot - eng_calib_dead_emb0 - eng_calib_dead_tile0 
            - eng_calib_dead_tileg3 - eng_calib_dead_eme0 - eng_calib_dead_hec0 - eng_calib_dead_fcal
            - eng_calib_dead_leakage;

      if ( !m_momentsNames.empty() ) {
	std::vector<double> myMoments(m_validMoments.size(),0);
	
	// assign moments 
	moment_name_set::const_iterator vMomentsIter = m_validMoments.begin(); 
	moment_name_set::const_iterator vMomentsIterEnd = m_validMoments.end();
	
	int iMoment=0;
	for(; vMomentsIter!=vMomentsIterEnd; vMomentsIter++,iMoment++) {
	  // now calculate the actual moments
	  switch (vMomentsIter->second) { 
	  case xAOD::CaloCluster::ENG_CALIB_TOT:
	    myMoments[iMoment] = engCalibTot[iClus];
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
	  case xAOD::CaloCluster::ENG_CALIB_DEAD_L:
	    myMoments[iMoment] = engCalibDead[0][iClus];
	    break;
	  case xAOD::CaloCluster::ENG_CALIB_DEAD_M:
	    myMoments[iMoment] = engCalibDead[1][iClus];
	    break;
	  case xAOD::CaloCluster::ENG_CALIB_DEAD_T:
	    myMoments[iMoment] = engCalibDead[2][iClus];
	    break;
          case xAOD::CaloCluster::ENG_CALIB_EMB0:
            myMoments[iMoment] = engCalibTotSmp[iClus][CaloSampling::PreSamplerB];
            break;
          case xAOD::CaloCluster::ENG_CALIB_EME0:
            myMoments[iMoment] = engCalibTotSmp[iClus][CaloSampling::PreSamplerE];
            break;
          case xAOD::CaloCluster::ENG_CALIB_TILEG3:
            myMoments[iMoment] = engCalibTotSmp[iClus][CaloSampling::TileGap3];
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
	  default:
	    // nothing to be done for other moments
	    break;
	  }

          theCluster->insertMoment(vMomentsIter->second, myMoments[iMoment]);
	}
      }
    }

    for(unsigned int ic=0;ic<CaloCell_ID::NSUBCALO; ic++) {
      for (unsigned int ii = 0;ii<cellVector[ic].size();ii++ ) {
	ClusWeight * theList = cellVector[ic][ii];
	ClusWeight * prev = nullptr;
	while ( theList) {
	  while ( theList->next ) {
	    prev = theList;
	    theList = theList->next;
	  }
	  delete theList;
	  if ( prev ) 
	    prev->next = nullptr;
	  else 
	    cellVector[ic][ii] = nullptr;
	  theList = cellVector[ic][ii];
	  prev = nullptr;
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}



/* ****************************************************************************

**************************************************************************** */
double CaloCalibClusterMomentsMaker::angle_mollier_factor(double x) const
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

