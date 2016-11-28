/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigSORFromPtreeHelper.cxx
 * @author Ricardo Abreu
 *
 * @brief Helper class to retrieve the Start Of Run parameters from the
 * prepareForRun ptree and put them into the detector store with whole-run
 * validity. This class replaces it's old analogous TrigSORFromISHelper.
 */

#include "TrigSORFromPtreeHelper.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IAddressProvider.h"
#include "owl/time.h"
#include <eformat/DetectorMask.h>

#define ST_WHERE CLNAME << "::" << __func__ << "(): "

using namespace boost::property_tree;
using std::string;
using SORHelper = TrigSORFromPtreeHelper;

namespace
{
  //////////////////////////////////////////////////////////////////////////////
  const string CLNAME{"TrigSORFromPtreeHelper"};
  const string SORPATH{"/TDAQ/RunCtrl/SOR_Params"};

  //////////////////////////////////////////////////////////////////////////////
  void printSOR(MsgStream& log, const SORHelper::SOR * sor)
  {
    log << MSG::DEBUG << ST_WHERE
        << "Dump SOR CondAttrListCollection: size=" << sor->size() << endmsg;
    sor->dump();
  }

  //////////////////////////////////////////////////////////////////////////////
  void setSpec(coral::AttributeListSpecification * attrSpec)
  {
    attrSpec->extend("RunNumber", "unsigned int");
    attrSpec->extend("SORTime", "unsigned long long");
    attrSpec->extend("RunType", "string");
    attrSpec->extend("DetectorMaskFst", "unsigned long long");
    attrSpec->extend("DetectorMaskSnd", "unsigned long long");
    attrSpec->extend("RecordingEnabled", "bool");
  }
}

////////////////////////////////////////////////////////////////////////////////
const SORHelper::SOR * SORHelper::fillSOR(const ptree & rparams) const
{
  m_log << MSG::DEBUG << ST_WHERE << "Setup SOR in DetectorStore" << endmsg;
  SG dstore("DetectorStore", CLNAME);
  if((dstore.retrieve()).isFailure())
    m_log << MSG::ERROR << ST_WHERE << "could not find DetectorStore" << endmsg;

  // get handle to the IOVDbSvc
  ServiceHandle<IIOVDbSvc> iovdbsvc("IOVDbSvc", CLNAME);
  if ((iovdbsvc.retrieve()).isFailure()) {
    m_log << MSG::ERROR << ST_WHERE 
          << "could not find IOVDbSvc. Time dependent conditions data may be not properly handled." << endmsg;
  } else {
    IOVTime currentIOVTime(rparams.get<unsigned int>("run_number"), 
			   IOVTime::MINEVENT,
			   OWLTime{(rparams.get_child("timeSOR").data()).c_str()}.total_mksec_utc() * 1000);

    // Signal BeginRun directly to IOVDbSvc to set complete IOV start time
    if (StatusCode::SUCCESS != iovdbsvc->signalBeginRun(currentIOVTime)) {
      m_log << MSG::ERROR << ST_WHERE 
            << "Unable to signal begin run IOVTime to IOVDbSvc. IOVTime = " << currentIOVTime << endmsg;
    } else {
      m_log << MSG::DEBUG << ST_WHERE 
            << "Set start of run time to IOVTime = " << currentIOVTime << endmsg;
    }
  }

  auto sor = getSOR(dstore);
  if(!sor || fillSor(rparams, sor).isFailure() ||
      updateProxy(dstore, sor).isFailure())
  {
    m_log << MSG::ERROR << ST_WHERE << "could not properly setup SOR" << endmsg;
    return nullptr;
  }

  m_log << MSG::DEBUG << ST_WHERE << "successfully setup SOR" << endmsg;
  printSOR(m_log, sor);

  return sor;
}

////////////////////////////////////////////////////////////////////////////////
SORHelper::SOR * SORHelper::getSOR(const SG & dstore) const
{
  auto sc = StatusCode{};
  auto sor = new SOR(true);
  if(dstore->transientContains<SOR>(SORPATH))
  {
    const SOR * oldsor = dstore->retrieve<const SOR>(SORPATH);
    m_log << MSG::INFO << ST_WHERE
          << "overwriting SOR contents (a dump of the old one follows)"<<endmsg;
    printSOR(m_log, oldsor);
    sc = dstore->overwrite(sor, SORPATH, true);
  }
  else
  {
    m_log << MSG::INFO << ST_WHERE << "recording new SOR" << endmsg;
    sc = dstore->record(sor, SORPATH, true);
  }

  if(sc.isFailure())
  {
    m_log << MSG::ERROR << ST_WHERE
          << "could not record SOR in DetectorStore\n" << dstore->dump()
          << endmsg;
    delete sor;
    sor = nullptr;
  }

  return sor;
}


////////////////////////////////////////////////////////////////////////////////
coral::AttributeList SORHelper::getAttrList(const ptree& rparams) const
{
  // First create attribute specification
  // ugly new needed:
  // dtor is protected, have to use ptr and release it explicitly... go figure
  auto attrSpec = new coral::AttributeListSpecification{};
  setSpec(attrSpec);

  // now create the attribute list and fill it in
  coral::AttributeList attrList(*attrSpec);

  attrList["RunNumber"].data<unsigned int>() =
      rparams.get<unsigned int>("run_number");
  attrList["RunType"].data<string>() =
      rparams.get<string>("run_type");
  attrList["RecordingEnabled"].data<bool>() =
      rparams.get<bool>("recording_enabled");

  const auto& t = rparams.get_child("timeSOR").data();
  attrList["SORTime"].data<unsigned long long>() =
      OWLTime{t.c_str()}.total_mksec_utc() * 1000;

  auto dm = getDetMask(rparams);
  attrList["DetectorMaskFst"].data<unsigned long long>() = dm.first;
  attrList["DetectorMaskSnd"].data<unsigned long long>() = dm.second;

  // coral wants us to have to do this explicitly...
  attrSpec->release(); // we don't delete because the dtor is private
                       // we have to hope the CORAL framework will

  return attrList;
}

////////////////////////////////////////////////////////////////////////////////
std::pair<uint64_t, uint64_t>
SORHelper::getDetMask(const ptree& rparams) const
{
  const auto& dmstr = rparams.get_child("det_mask").data();
  return eformat::helper::DetectorMask(dmstr).serialize();
}

////////////////////////////////////////////////////////////////////////////////
StatusCode SORHelper::fillSor(const ptree & rparams, SOR * sor) const
{
  // obtain SOR contents from ptree
  auto attrList = getAttrList(rparams);

  // Validity stuff
  IOVTime iovTimeStart(attrList["RunNumber"].data<unsigned int>() ,0);
  IOVTime iovTimeStop(attrList["RunNumber"].data<unsigned int>()+1,0);
  IOVRange iovRange(iovTimeStart, iovTimeStop);
  auto channum = SOR::ChanNum{0};

  sor->add(channum, attrList);
  sor->add(channum,iovRange);
  sor->resetMinRange();
  sor->addNewStart(iovTimeStart);
  sor->addNewStop(iovTimeStop);

  return setIOVRange(iovRange);
}

////////////////////////////////////////////////////////////////////////////////
StatusCode SORHelper::setIOVRange(IOVRange & iovRange) const
{
  // set IOVRange on the IOVSvc
  ServiceHandle<IIOVSvc> iovsvc("IOVSvc", CLNAME);
  if ((iovsvc.retrieve()).isFailure())
  {
    m_log << MSG::ERROR << ST_WHERE << "could not find IOVSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  auto clid = ClassID_traits<SOR>::ID();
  if ((iovsvc->setRange(clid, SORPATH, iovRange, "StoreGateSvc")).isFailure())
  {
    m_log << MSG::ERROR << ST_WHERE
          << "could not set IOVRange for SOR folder in IOVSvc." << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
StatusCode
SORHelper::updateProxy(const SG & dstore, SOR * sor) const
{
  // check the SOR_Params proxy and add if necessary an IAddressProvider
  //(typically for MC)
  auto proxy = dstore->proxy(sor);
  if (!proxy) {
    m_log << MSG::ERROR << ST_WHERE
          << "could not find proxy for SOR_Params folder." << endmsg;
    return StatusCode::FAILURE;
  }

  auto transientAddr = proxy->transientAddress();
  if (!transientAddr) {
    m_log << MSG::ERROR << ST_WHERE
          << "could not find transient address for SOR_Params folder" << endmsg;
    return StatusCode::FAILURE;
  }

  // check if the transient address has an IAddressProvider, if not set
  //IOVDbSvc as provider
  if (!transientAddr->provider()) {
    // get handle to the IOVDbSvc
    ServiceHandle<IIOVDbSvc> iovdbsvc("IOVDbSvc", CLNAME);
    if ((iovdbsvc.retrieve()).isFailure()) {
      m_log << MSG::ERROR << ST_WHERE << "could not find IOVDbSvc." << endmsg;
      return StatusCode::FAILURE;
    }

    IAddressProvider* provider = dynamic_cast<IAddressProvider*>(&*iovdbsvc);
    if (!provider) {
      m_log << MSG::ERROR << ST_WHERE
            << "could not cast to IAddressProvider interface and set the "
               "provider for SOR_Params." << endmsg;
      return StatusCode::FAILURE;
    }
    transientAddr->setProvider(provider, transientAddr->storeID());
  }

  return StatusCode::SUCCESS;
}

