/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "AODReader.h"

#include "GaudiKernel/IToolSvc.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/EMShower.h"

#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

AODReader::AODReader (const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_nt(0)
{

}

AODReader::~AODReader () { }

StatusCode AODReader::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;


  msg(MSG::INFO) << "creating n tuple" << endmsg;
  NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE");
  
  //Checking if the n tuple is already booked
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE/AOD");

  if ( !nt )    {    // Normally, this should always be true, but who knows...
    nt = ntupleSvc()->book ("/NTUPLES/FILE/AOD", CLID_ColumnWiseTuple, "Readed info from aod container");

    if ( nt )    {
      msg(MSG::INFO) << "booked ntuple " << endmsg;

      StatusCode sc = StatusCode::SUCCESS;

      sc = nt->addItem("eta"		, this->m_eta);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for eta" << endmsg;
      sc = nt->addItem("energy"		, this->m_energy);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for erec" << endmsg;
      sc = nt->addItem("pt"		, this->m_pt);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for pt" << endmsg;
      sc = nt->addItem("e237"		, this->m_e237);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for e237" << endmsg;
      sc = nt->addItem("e277"		, this->m_e277);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for e277" << endmsg;
      sc = nt->addItem("weta1"		, this->m_weta1);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for weta1" << endmsg;
      sc = nt->addItem("weta2"		, this->m_weta2);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for weta2" << endmsg;
      sc = nt->addItem("e2tsts1"	, this->m_e2tsts1);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for e2tsts1" << endmsg;
      sc = nt->addItem("emins1"		, this->m_emins1);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for emins1" << endmsg;
      sc = nt->addItem("fracs1"		, this->m_fracs1);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for fracs1" << endmsg;
      sc = nt->addItem("wtots1"		, this->m_wtots1);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for wtots1" << endmsg;
      sc = nt->addItem("f1"		, this->m_f1);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for f1" << endmsg;
      sc = nt->addItem("f1core"		, this->m_f1core);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for f1core" << endmsg;
      sc = nt->addItem("ethad1"		, this->m_ethad1);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for ethad1" << endmsg;
      sc = nt->addItem("et"		, this->m_et);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for et" << endmsg;

      sc = nt->addItem("truth_energy"	, this->m_truth_energy);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for egen" << endmsg;
      sc = nt->addItem("truth_px"	, this->m_truth_px);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for tr_px" << endmsg;
      sc = nt->addItem("truth_py"	, this->m_truth_py);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for tr_py" << endmsg;
      sc = nt->addItem("truth_pz"	, this->m_truth_pz);
      if( sc.isFailure() ) msg(MSG::ERROR) << "Can not create ntuple entry for tr_pz" << endmsg;
    }

  } else {
    msg(MSG::ERROR) << "didn't manage to book ntuple" << endmsg;
    return StatusCode::FAILURE;
  }


  m_nt = nt;

  return StatusCode::SUCCESS;
}

StatusCode AODReader::finalize()
{
  msg(MSG::INFO) << "Finalised " << name() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode AODReader::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  /// read the AOD electron container from persistecy storage
  const ElectronContainer* elecTES = 0;
  sc=evtStore()->retrieve( elecTES, "ElectronAODCollection");
  if( sc.isFailure()  ||  !elecTES ) {
    msg(MSG::WARNING) << "No AOD electron container found in TDS" << endmsg;
    return sc;
  }
  msg(MSG::INFO) << "ElectronContainer successfully retrieved" << endmsg;

  const McEventCollection * mcEvtColl = 0;

  if (( mcEvtColl = evtStore()->retrieve<const McEventCollection>() )) {
	  msg(MSG::INFO) << "TruthParticleContainer successfully retrieved" << endmsg;
  } else {
	  msg(MSG::WARNING) << "Could not retrieve TruthParticleContainer" << endmsg;
  }
  
  ElectronContainer::const_iterator elecItr       = elecTES->begin();
  ElectronContainer::const_iterator elecItrE      = elecTES->end();
  
  HepMC::GenParticle * trPart = 0;
  if (mcEvtColl) {
	  McEventCollection::const_iterator mcTrPart = mcEvtColl->begin();
	  if (mcTrPart != mcEvtColl->end()) {
		  trPart = (*mcTrPart)->barcode_to_particle(10001);
		  if (!trPart) {
			  msg(MSG::WARNING) << "Not a single particle event. Truth information won't be available" << endmsg;
		  }
	  } else {
		  msg(MSG::WARNING) << "Empty TruthParticleContainer. Truth information won't be available" << endmsg;
	  }
  }

  const Analysis::Electron * primElec = 0;
  
  
  /// loop over the AOD electron container,
  /// take the first one (only one?) with the correct author
  for (; elecItr != elecItrE; ++elecItr) {
    if (((*elecItr)->author() & (8 + 1))  > 0) {
        primElec = (*elecItr);

		const EMShower * emshower = primElec->detail<EMShower> ("egDetailAOD");
		if (emshower) { //exist for non-forward electrons
			m_e237 = emshower->e237();
			m_e277 = emshower->e277();
			m_weta1 = emshower->weta1();
			m_weta2 = emshower->weta2();
			m_e2tsts1 = emshower->e2tsts1();
			m_emins1 = emshower->emins1();
			m_fracs1 = emshower->fracs1();
			m_wtots1 = emshower->wtots1();
			m_f1 = emshower->f1();
			m_f1core = emshower->f1core();
			m_ethad1 = emshower->ethad1();
		} else {
			m_e237 = 0;
			m_e277 = 0;
			m_weta1 = 0;
			m_weta2 = 0;
			m_e2tsts1 = 0;
			m_emins1 = 0;
			m_fracs1 = 0;
			m_wtots1 = 0;
			m_f1 = 0;
			m_f1core = 0;
			m_ethad1 = 0;
			m_et = 0;
		}
		if (primElec->cluster()) { // don't know if it's possible for this not to exist, but just in case
			m_et = primElec->cluster()->et();
		} else {
			m_et = 0;
		}
		m_energy = primElec->e();
		m_eta = primElec->eta();
		m_pt = primElec->pt();
		if (trPart) {
			m_truth_energy = trPart->momentum().e();
			m_truth_px = trPart->momentum().px();
			m_truth_py = trPart->momentum().py();
			m_truth_pz = trPart->momentum().pz();
		} else {
			m_truth_energy = 0;
			m_truth_px = 0;
			m_truth_py = 0;
			m_truth_pz = 0;
		}
		// writing a record
		ntupleSvc()->writeRecord(m_nt);
		msg(MSG::VERBOSE) << "n tuple entry writed" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
