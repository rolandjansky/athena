/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloLevel2Mon.cxx
// PACKAGE:     TrigT1CaloMonitoring  
//
// AUTHOR:      Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//           
// DESCRIPTION: Monitoring of transmission L1Calo --> Level2
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/CPMRoI.h"

#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/JetEnergyResult.h"
#include "TrigT1Result/JetEnergyRoI.h"
#include "TrigT1Result/EMTauResult.h"
#include "TrigT1Result/EMTauRoI.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "L1CaloLevel2Mon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
// *********************************************************************
// Public Methods
// *********************************************************************

/*---------------------------------------------------------*/
L1CaloLevel2Mon::L1CaloLevel2Mon( const std::string & type,
                const std::string & name, const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_errorTool("LVL1::TrigT1CaloMonErrorToolV1/TrigT1CaloMonErrorToolV1"),
    m_histTool("LVL1::TrigT1CaloLWHistogramToolV1/TrigT1CaloLWHistogramToolV1"),
    m_histBooked(false),
    m_h_l2_1d_L1NeL2Summary(0),
    m_h_l2_1d_L1EqL2Summary(0),
    m_h_l2_2d_JEMRoIErrors(0),
    m_h_l2_2d_CPMRoIErrorsEM(0),
    m_h_l2_2d_CPMRoIErrorsEMTau(0),
    m_h_l2_2d_MismatchEvents(0)
/*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "JEMRoILocation",
    m_L1Calo_JEMRoILocation = LVL1::TrigT1CaloDefs::JEMRoILocation);
  declareProperty( "CMMRoILocation",
    m_L1Calo_CMMRoILocation = LVL1::TrigT1CaloDefs::CMMRoILocation);
  declareProperty( "CPMRoILocation",
    m_L1Calo_CPMRoILocation = LVL1::TrigT1CaloDefs::CPMRoILocation);
  declareProperty( "RoIBResultLocation",
    m_L2_RoIBResultLocation = "RoIBResult");

  declareProperty( "PathInRootFile",
    m_PathInRootFile = "LVL1_Interfaces/Level2");
}


/*---------------------------------------------------------*/
L1CaloLevel2Mon::~L1CaloLevel2Mon()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode L1CaloLevel2Mon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloLevel2Mon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in L1CaloLevel2Mon::bookHistograms" << endmsg;
  
  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if( newLumiBlockFlag() ) { }
  
  if( newRunFlag() ) {	

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup L1CaloLevel2( this, m_PathInRootFile, run, attr );
    MonGroup L1CaloLevel2Events( this, m_PathInRootFile, run, attr,
                                                          "", "eventSample" );

    //--------------- transmission checks L1Calo <-> Level2 ------------------

    m_histTool->setMonGroup(&L1CaloLevel2);

    m_h_l2_1d_L1NeL2Summary = m_histTool->book1F("l2_1d_L1NeL2Summary",
      "Transmission Errors between L1Calo and Level2",
      NumberOfRoiTypes, 0, NumberOfRoiTypes);
    setLabels(m_h_l2_1d_L1NeL2Summary);
    m_h_l2_1d_L1EqL2Summary = m_histTool->book1F("l2_1d_L1EqL2Summary",
      "Transmission Matches between L1Calo and Level2",
      NumberOfRoiTypes, 0, NumberOfRoiTypes);
    setLabels(m_h_l2_1d_L1EqL2Summary);

    m_h_l2_2d_JEMRoIErrors = m_histTool->bookJEMModuleVsCrate(
      "l2_2d_JEMRoIErrors", "Transmission Errors between JEMs and Level2");
    m_h_l2_2d_CPMRoIErrorsEM = m_histTool->bookCPMModuleVsCrate(
      "l2_2d_CPMRoIErrorsEM", "Transmission Errors between CPMs and Level2 EM");
    m_h_l2_2d_CPMRoIErrorsEMTau = m_histTool->bookCPMModuleVsCrate(
      "l2_2d_CPMRoIErrorsEMTau",
      "Transmission Errors between CPMs and Level2 EMTau");

    m_histTool->setMonGroup(&L1CaloLevel2Events);

    m_h_l2_2d_MismatchEvents = m_histTool->bookEventNumbers(
      "l2_2d_MismatchEvents",
      "Transmission Errors between L1Calo and Level2 Event Numbers",
      NumberOfRoiTypes, 0, NumberOfRoiTypes);
    setLabels(m_h_l2_2d_MismatchEvents, false);

    m_histTool->unsetMonGroup();
    m_histBooked = true;
       
  }

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode L1CaloLevel2Mon::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "in L1CaloLevel2Mon::fillHistograms" << endmsg;

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt or with ROB errors

  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }
 
  // Get RoIB RDO object back from SG:
  const ROIB::RoIBResult* roib_rdo = 0;
  StatusCode sc = evtStore()->retrieve(roib_rdo, m_L2_RoIBResultLocation);
  if ( sc.isFailure() || !roib_rdo ) {
    msg(MSG::WARNING) << "ERROR retrieving RoIB RDO from store!" << endmsg;
    return sc;
  }

  //--------------------------------------------------------------------------
  //------------------------ JetRoI ------------------------------------------
  //--------------------------------------------------------------------------

  //------------ retrieve JEMRoIs from Storegate -------------------
  const JemRoiCollection* L1Calo_JEMRoI = 0;
  sc = evtStore()->retrieve(L1Calo_JEMRoI, m_L1Calo_JEMRoILocation);
  if (sc == StatusCode::FAILURE || !L1Calo_JEMRoI) {
    msg(MSG::INFO) << "No JEMRoI found in TES at " << m_L1Calo_JEMRoILocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }
 
  //------------- Get L2 RoIs from RoIBResult ----------------------
  JemRoiCollection* L2_JEMRoI = new JemRoiCollection();
  int L2_JetEtHits        = 0;
  int L2_SumEtHits        = 0;
  int L2_MissingEtHits    = 0;
  int L2_MissingEtSigHits = 0;
  int L2_Ex = 0;
  int L2_Ey = 0;
  int L2_Et = 0;

  const JetEnergyResultCollection& jerColl(roib_rdo->jetEnergyResult());
  JetEnergyResultCollection::const_iterator it1    = jerColl.begin();
  JetEnergyResultCollection::const_iterator it1End = jerColl.end();
  for (; it1 != it1End; ++it1) {
    const JetEnergyRoICollection& roiColl(it1->roIVec());
    JetEnergyRoICollection::const_iterator it2    = roiColl.begin();
    JetEnergyRoICollection::const_iterator it2End = roiColl.end();
    for (; it2 != it2End; ++it2) {
      const unsigned int roIType = it2->roIType();
      //------ Fill Level-2 JEMRoIs in Vector ----------
      if (roIType == LVL1::TrigT1CaloDefs::JetRoIWordType) {
	LVL1::JEMRoI* roi = new LVL1::JEMRoI(it2->roIWord());
	L2_JEMRoI->push_back(roi);	       
	if (debug) {
          const int nthresh = (roi->forward()) ? 4 : 8;
	  msg(MSG::DEBUG) << "L2:     JEP crate " << roi->crate()
	                  << " module "           << roi->jem()
		          << " frame "            << roi->frame()
	      	          << " location "         << roi->location()
		          << " RoI "
		          << m_histTool->thresholdString(roi->hits(), nthresh)
		          << endmsg;
        }
      } else if (roIType == LVL1::TrigT1CaloDefs::JetEtRoIWordType) {
        L2_JetEtHits = it2->jetEt();
      //--------- retrieve CMM Energies -----------
      } else if (roIType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0) {
        L2_Ex = it2->energyX();
	L2_MissingEtSigHits = (it2->roIWord() >> 16) & 0xff;
      } else if (roIType == LVL1::TrigT1CaloDefs::EnergyRoIWordType1) {
	L2_Ey = it2->energyY();
	L2_SumEtHits = (it2->roIWord() >> 16) & 0xff;
      } else if (roIType == LVL1::TrigT1CaloDefs::EnergyRoIWordType2) {
	L2_Et = it2->energySum();
	std::vector<unsigned int> hits(it2->etMissThresholds());
	const int size = hits.size();
	L2_MissingEtHits = 0;
	for (int bit = 0; bit < size; ++bit) {
	  L2_MissingEtHits |= (hits[bit] << bit);
        }
      }
    }
  }

  //------------------ Step over all cells and compare -----------------------
  JemRoiCollection::const_iterator it_L1Calo_JEMRoI;
  JemRoiCollection::const_iterator it_L1Calo_JEMRoIEnd = L1Calo_JEMRoI->end();
  JemRoiCollection::const_iterator it_L2_JEMRoI;
  JemRoiCollection::const_iterator it_L2_JEMRoIEnd = L2_JEMRoI->end();
 
  if (debug) {
    it_L1Calo_JEMRoI = L1Calo_JEMRoI->begin();
    for (; it_L1Calo_JEMRoI != it_L1Calo_JEMRoIEnd; ++it_L1Calo_JEMRoI) {
      const int nthresh = ((*it_L1Calo_JEMRoI)->forward()) ? 4 : 8;
      msg(MSG::DEBUG) << "L1Calo: JEP crate " << (*it_L1Calo_JEMRoI)->crate()
                      << " module "           << (*it_L1Calo_JEMRoI)->jem()
		      << " frame "            << (*it_L1Calo_JEMRoI)->frame()
		      << " location "         << (*it_L1Calo_JEMRoI)->location()
	              << " RoI "
		      << m_histTool->thresholdString(
		                           (*it_L1Calo_JEMRoI)->hits(), nthresh)
	              << endmsg;
    }
  }

  // L1s different from or not in L2

  std::vector<int> foundL2(L2_JEMRoI->size());
  std::vector<int>::iterator it_L2_found;
  it_L1Calo_JEMRoI = L1Calo_JEMRoI->begin();    
  for (; it_L1Calo_JEMRoI != it_L1Calo_JEMRoIEnd; ++it_L1Calo_JEMRoI) {
    const int crate    = (*it_L1Calo_JEMRoI)->crate();
    const int jem      = (*it_L1Calo_JEMRoI)->jem();
    const int frame    = (*it_L1Calo_JEMRoI)->frame();
    const int location = (*it_L1Calo_JEMRoI)->location();
    const int hits     = (*it_L1Calo_JEMRoI)->hits();
    const int nthresh  = ((*it_L1Calo_JEMRoI)->forward()) ? 4 : 8;

    int hitsL2 = 0;
    it_L2_JEMRoI = L2_JEMRoI->begin();
    it_L2_found  = foundL2.begin();
    for (; it_L2_JEMRoI != it_L2_JEMRoIEnd; ++it_L2_JEMRoI, ++it_L2_found) {
      if ((*it_L2_found) == 0) {
        if ((crate    == (*it_L2_JEMRoI)->crate()) &&
  	    (jem      == (*it_L2_JEMRoI)->jem())   &&
            (frame    == (*it_L2_JEMRoI)->frame()) &&
            (location == (*it_L2_JEMRoI)->location())) {
	  hitsL2 = (*it_L2_JEMRoI)->hits();
	  (*it_L2_found) = 1;
	  break;
        }
      }
    }

    if (hits != hitsL2) {
      if (debug) {
        msg(MSG::DEBUG) << "JEP crate " << crate << " module "   << jem
   		        << " frame "    << frame << " location " << location
		        << " L1Calo: RoI "
			<< m_histTool->thresholdString(hits, nthresh)
		        << " L2: RoI "
			<< m_histTool->thresholdString(hitsL2, nthresh)
		        << endmsg;
      }
      m_h_l2_1d_L1NeL2Summary->Fill(JetType);
      m_h_l2_2d_JEMRoIErrors->Fill(jem, crate);
      m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, JetType);
    } else if (hits) m_h_l2_1d_L1EqL2Summary->Fill(JetType);
  }
      
  // L2s not in L1

  it_L2_JEMRoI = L2_JEMRoI->begin();
  it_L2_found  = foundL2.begin();
  for (; it_L2_JEMRoI != it_L2_JEMRoIEnd; ++it_L2_JEMRoI, ++it_L2_found) {
    if (((*it_L2_found) == 0) && (*it_L2_JEMRoI)->hits()) {
      if (debug) {
	const int hitsL1  = 0;
        const int nthresh = ((*it_L2_JEMRoI)->forward()) ? 4 : 8;
        msg(MSG::DEBUG) << "JEP crate " << (*it_L2_JEMRoI)->crate()
	                << " module "   << (*it_L2_JEMRoI)->jem()
			<< " frame "    << (*it_L2_JEMRoI)->frame()
			<< " location " << (*it_L2_JEMRoI)->location()
			<< " L1Calo: RoI "
			<< m_histTool->thresholdString(hitsL1, nthresh)
	                << " L2: RoI "
			<< m_histTool->thresholdString(
			                       (*it_L2_JEMRoI)->hits(), nthresh)
	                << endmsg;
      }
      m_h_l2_1d_L1NeL2Summary->Fill(JetType);
      m_h_l2_2d_JEMRoIErrors->Fill((*it_L2_JEMRoI)->jem(),
                                               (*it_L2_JEMRoI)->crate());
      m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, JetType);
    }
  }
  delete L2_JEMRoI;

  // retrieve CMMRoI information from Storegate
  const LVL1::CMMRoI* L1Calo_CR = 0;
  sc = evtStore()->retrieve (L1Calo_CR, m_L1Calo_CMMRoILocation);
  if (sc == StatusCode::FAILURE || !L1Calo_CR) {
    msg(MSG::INFO) << "No L1Calo CMM RoI found in TES at "
                   << m_L1Calo_CMMRoILocation << endmsg;
    return StatusCode::SUCCESS;    
  }

  int L1Calo_JetEtHits        = L1Calo_CR->jetEtHits();
  int L1Calo_SumEtHits        = L1Calo_CR->sumEtHits();
  int L1Calo_MissingEtHits    = L1Calo_CR->missingEtHits();
  int L1Calo_MissingEtSigHits = L1Calo_CR->missingEtSigHits();
  // include overflow bit in ex, ey, et
  int L1Calo_Ex = (L1Calo_CR->ex() | ((L1Calo_CR->exError() & 0x1) << 15));
  int L1Calo_Ey = (L1Calo_CR->ey() | ((L1Calo_CR->eyError() & 0x1) << 15));
  int L1Calo_Et = (L1Calo_CR->et() | ((L1Calo_CR->etError() & 0x1) << 15));
      
  if (L1Calo_JetEtHits != L2_JetEtHits) {
    m_h_l2_1d_L1NeL2Summary->Fill(JetEtType);
    m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, JetEtType);
  } else if (L1Calo_JetEtHits) m_h_l2_1d_L1EqL2Summary->Fill(JetEtType);

  if (L1Calo_SumEtHits != L2_SumEtHits) {
    m_h_l2_1d_L1NeL2Summary->Fill(SumEtType);
    m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, SumEtType);
  } else if (L1Calo_SumEtHits) m_h_l2_1d_L1EqL2Summary->Fill(SumEtType);

  if (L1Calo_MissingEtHits != L2_MissingEtHits) {
    m_h_l2_1d_L1NeL2Summary->Fill(MissingEtType);
    m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, MissingEtType);
  } else if (L1Calo_MissingEtHits) {
    m_h_l2_1d_L1EqL2Summary->Fill(MissingEtType);
  }

  if (L1Calo_MissingEtSigHits != L2_MissingEtSigHits) {
    m_h_l2_1d_L1NeL2Summary->Fill(MissingEtSigType);
    m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, MissingEtSigType);
  } else if (L1Calo_MissingEtSigHits) {
    m_h_l2_1d_L1EqL2Summary->Fill(MissingEtSigType);
  }

  if ((L1Calo_Ex != L2_Ex) || (L1Calo_Ey != L2_Ey) || (L1Calo_Et != L2_Et)) {
    m_h_l2_1d_L1NeL2Summary->Fill(EnergyType);
    m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, EnergyType);
  } else if (L1Calo_Ex || L1Calo_Ey || L1Calo_Et) {
    m_h_l2_1d_L1EqL2Summary->Fill(EnergyType);
  }

  //--------------------------------------------------------------------------
  //------------------------ CP RoI ------------------------------------------
  //--------------------------------------------------------------------------

  //------------ retrieve CPMRoIs from Storegate -------------------
  const CpmRoiCollection* L1Calo_CPMRoI = 0;
  sc = evtStore()->retrieve(L1Calo_CPMRoI, m_L1Calo_CPMRoILocation);
  if (sc == StatusCode::FAILURE || !L1Calo_CPMRoI) {
    msg(MSG::INFO) << "No CPMRoI found in TES at " << m_L1Calo_CPMRoILocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }

  //------------- Get L2 RoIs from RoIBResult ----------------------
  CpmRoiCollection* L2_CPMRoI = new CpmRoiCollection();
  const int nthresh = 16;
    
  const EMTauResultCollection& etrColl(roib_rdo->eMTauResult());
  EMTauResultCollection::const_iterator et1    = etrColl.begin();
  EMTauResultCollection::const_iterator et1End = etrColl.end();
  for (; et1 != et1End; ++et1) {
    const EMTauRoICollection& roiColl(et1->roIVec());
    EMTauRoICollection::const_iterator et2    = roiColl.begin();
    EMTauRoICollection::const_iterator et2End = roiColl.end();
    for (; et2 != et2End; ++et2) {
      //------ Fill Level-2 CPMRoIs in Vector ----------
      if (et2->roIType() == LVL1::TrigT1CaloDefs::CpRoIWordType) {
	LVL1::CPMRoI* roi = new LVL1::CPMRoI(et2->roIWord());
	L2_CPMRoI->push_back(roi);
	if (debug) {
          msg(MSG::DEBUG) << "L2:     CP crate " << roi->crate()
	                  << " module "          << roi->cpm()
			  << " chip "            << roi->chip()
			  << " location "        << roi->location()
		          << " RoI "
			  << m_histTool->thresholdString(roi->hits(), nthresh)
			  << endmsg;
        }
      }
    }
  }

  //------------------ Step over all cells and compare -----------------------
  CpmRoiCollection::const_iterator it_L1Calo_CPMRoI;
  CpmRoiCollection::const_iterator it_L1Calo_CPMRoIEnd = L1Calo_CPMRoI->end();
  CpmRoiCollection::const_iterator it_L2_CPMRoI;
  CpmRoiCollection::const_iterator it_L2_CPMRoIEnd = L2_CPMRoI->end();

  if (debug) {
    it_L1Calo_CPMRoI = L1Calo_CPMRoI->begin();
    for (; it_L1Calo_CPMRoI != it_L1Calo_CPMRoIEnd; ++it_L1Calo_CPMRoI) {
      msg(MSG::DEBUG) << "L1Calo: CP crate " << (*it_L1Calo_CPMRoI)->crate()
                      << " module "          << (*it_L1Calo_CPMRoI)->cpm()
		      << " chip "            << (*it_L1Calo_CPMRoI)->chip()
		      << " location "        << (*it_L1Calo_CPMRoI)->location()
	              << " RoI "
		      << m_histTool->thresholdString(
		                         (*it_L1Calo_CPMRoI)->hits(), nthresh)
	              << endmsg;
    }
  }

  // L1s different from or not in L2

  foundL2.clear();
  foundL2.resize(L2_CPMRoI->size());
  it_L1Calo_CPMRoI = L1Calo_CPMRoI->begin();
  for (; it_L1Calo_CPMRoI != it_L1Calo_CPMRoIEnd; ++it_L1Calo_CPMRoI) {
    const int crate    = (*it_L1Calo_CPMRoI)->crate();
    const int cpm      = (*it_L1Calo_CPMRoI)->cpm();
    const int chip     = (*it_L1Calo_CPMRoI)->chip();
    const int location = (*it_L1Calo_CPMRoI)->location();
    const int hits     = (*it_L1Calo_CPMRoI)->hits();
    const int hitsEm   = hits & 0xff;
    const int hitsTau  = (hits >> 8) & 0xff;

    int hitsL2 = 0;
    it_L2_CPMRoI = L2_CPMRoI->begin();
    it_L2_found  = foundL2.begin();
    for (; it_L2_CPMRoI != it_L2_CPMRoIEnd; ++it_L2_CPMRoI, ++it_L2_found) {
      if ((*it_L2_found) == 0) {
        if ((crate    == (*it_L2_CPMRoI)->crate()) &&
	    (cpm      == (*it_L2_CPMRoI)->cpm())   &&
	    (chip     == (*it_L2_CPMRoI)->chip())  &&
	    (location == (*it_L2_CPMRoI)->location())) {
          hitsL2 = (*it_L2_CPMRoI)->hits();
	  (*it_L2_found) = 1;
	  break;
        }
      }
    }

    if (hits != hitsL2) {
      if (debug) {
        msg(MSG::DEBUG) << "CP crate " << crate << " module " << cpm
	                << " chip "    << chip  << " location " << location
			<< " L1Calo: RoI "
			<< m_histTool->thresholdString(hits, nthresh)
			<< " L2: RoI "
			<< m_histTool->thresholdString(hitsL2, nthresh)
			<< endmsg;
      }
      const int hitsEmL2  = hitsL2 & 0xff;
      const int hitsTauL2 = (hitsL2 >> 8) & 0xff;
      if (hitsEm != hitsEmL2) {
        m_h_l2_1d_L1NeL2Summary->Fill(EMType);
	m_h_l2_2d_CPMRoIErrorsEM->Fill(cpm, crate);
	m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, EMType);
      }
      if (hitsTau != hitsTauL2) {
        m_h_l2_1d_L1NeL2Summary->Fill(EMTauType);
	m_h_l2_2d_CPMRoIErrorsEMTau->Fill(cpm, crate);
	m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, EMTauType);
      }
    } else {
      if (hitsEm)  m_h_l2_1d_L1EqL2Summary->Fill(EMType);
      if (hitsTau) m_h_l2_1d_L1EqL2Summary->Fill(EMTauType);
    }
  }

  // L2s not in L1

  it_L2_CPMRoI = L2_CPMRoI->begin();
  it_L2_found  = foundL2.begin();
  for (; it_L2_CPMRoI != it_L2_CPMRoIEnd; ++it_L2_CPMRoI, ++it_L2_found) {
    if (((*it_L2_found) == 0) && (*it_L2_CPMRoI)->hits()) {
      const int crate   = (*it_L2_CPMRoI)->crate();
      const int cpm     = (*it_L2_CPMRoI)->cpm();
      const int hits    = (*it_L2_CPMRoI)->hits();
      const int hitsEm  = hits & 0xff;
      const int hitsTau = (hits >> 8) & 0xff;
      if (debug) {
        const int hitsL1 = 0;
	msg(MSG::DEBUG) << "CP crate "  << crate
	                << " module "   << cpm
			<< " chip "     << (*it_L1Calo_CPMRoI)->chip()
			<< " location " << (*it_L1Calo_CPMRoI)->location()
			<< " L1Calo: RoI "
			<< m_histTool->thresholdString(hitsL1, nthresh)
			<< " L2: RoI "
			<< m_histTool->thresholdString(hits, nthresh)
		        << endmsg;
      }
      if (hitsEm) {
        m_h_l2_1d_L1NeL2Summary->Fill(EMType);
	m_h_l2_2d_CPMRoIErrorsEM->Fill(cpm, crate);
	m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, EMType);
      }
      if (hitsTau) {
        m_h_l2_1d_L1NeL2Summary->Fill(EMTauType);
	m_h_l2_2d_CPMRoIErrorsEMTau->Fill(cpm, crate);
	m_histTool->fillEventNumber(m_h_l2_2d_MismatchEvents, EMTauType);
      }
    }
  }
  delete L2_CPMRoI;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloLevel2Mon::procHistograms()
/*---------------------------------------------------------*/
{
  //if( endOfLumiBlockFlag() ) { }
	
  //if( endOfRunFlag ) { }
  
  return StatusCode::SUCCESS;
}

void L1CaloLevel2Mon::setLabels(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+EMType,           "EMHits");
  axis->SetBinLabel(1+EMTauType,        "EMTauHits");
  axis->SetBinLabel(1+JetType,          "JetHits");
  axis->SetBinLabel(1+JetEtType,        "JetEtHits");
  axis->SetBinLabel(1+EnergyType,       "E_{x},E_{y},E_{t}");
  axis->SetBinLabel(1+MissingEtType,    "MissingEtHits");
  axis->SetBinLabel(1+SumEtType,        "SumEtHits");
  axis->SetBinLabel(1+MissingEtSigType, "MissEtSigHits");
}
// ============================================================================
}  // end namespace
// ============================================================================
