/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawChannelToNtuple.cxx
//  Author   : Zhifang
//  Created  : Nov, 2002
//
//  DESCRIPTION:
//     Implement the algorithm
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

//Gaudi Includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//Atlas include
#include "AthenaKernel/errorcheck.h"
#include "EventContainers/SelectAllObject.h"

// Calo include
#include "CaloIdentifier/TileID.h"

//TileCalo include
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRec/TileRawChannelToNtuple.h"

const int max_chan=12288;

// Constructor & deconstructor
TileRawChannelToNtuple::TileRawChannelToNtuple(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_tileID(0)
  , m_tileHWID(0)
{
  m_rawChannelContainer="TileRawChannelCnt";
  m_ntupleLoc="/FILE1/TileRec";
  m_ntupleID="h70";

  declareProperty("TileRawChannelContainer", m_rawChannelContainer);    
  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
}

TileRawChannelToNtuple::~TileRawChannelToNtuple()
{
}

// Alg standard interface function
StatusCode TileRawChannelToNtuple::initialize() 
{ 

  ATH_MSG_INFO( "Initialization started" );

  // retrieve TileID helper from det store

  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );

  m_ntupleLoc="/NTUPLES" + m_ntupleLoc;

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleService(), m_ntupleLoc);
  if(!DirPtr) DirPtr = ntupleService()->createDirectory(m_ntupleLoc);
  if(!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " );
    return StatusCode::FAILURE;
  }
  m_ntuplePtr=ntupleService()->book(DirPtr.ptr(), m_ntupleID, 
                                    CLID_ColumnWiseTuple, "TileRC-Ntuple");
  if(!m_ntuplePtr) {
    ATH_MSG_ERROR( "Failed to book ntuple: TileRCNtuple" );
    return StatusCode::FAILURE;
  }

  CHECK( m_ntuplePtr->addItem("TileRC/nchan",m_nchan,0,max_chan) );
  CHECK( m_ntuplePtr->addItem("TileRC/totalE",m_tolE) );

  CHECK( m_ntuplePtr->addItem("TileRC/energy",m_nchan,m_energy) );
  CHECK( m_ntuplePtr->addItem("TileRC/time",m_nchan,m_time) );
  CHECK( m_ntuplePtr->addItem("TileRC/quality",m_nchan,m_quality) );

  CHECK( m_ntuplePtr->addItem("TileRC/detector",m_nchan,m_detector,0,3) );
  CHECK( m_ntuplePtr->addItem("TileRC/side",m_nchan,m_side,-1,1) );
  CHECK( m_ntuplePtr->addItem("TileRC/sample",m_nchan,m_sample,-1,3) );
  CHECK( m_ntuplePtr->addItem("TileRC/eta",m_nchan,m_eta,-1,15) );
  CHECK( m_ntuplePtr->addItem("TileRC/phi",m_nchan,m_phi,0,63) );
  CHECK( m_ntuplePtr->addItem("TileRC/pmt",m_nchan,m_pmt,-1,1) );
  CHECK( m_ntuplePtr->addItem("TileRC/channel",m_nchan,m_channel,0,47) );
  CHECK( m_ntuplePtr->addItem("TileRC/gain",m_nchan,m_gain,0,2) );

  ATH_MSG_INFO( "Initialization completed" );
  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelToNtuple::execute()
{

  // step1: read RCs from TDS
  const TileRawChannelContainer* RawChannelCnt;
  CHECK( evtStore()->retrieve(RawChannelCnt, m_rawChannelContainer) );

  // step2: to fill items in ntuple
  SelectAllObject<TileRawChannelContainer> selRCs(RawChannelCnt);
  SelectAllObject<TileRawChannelContainer>::const_iterator it=selRCs.begin(); 
  SelectAllObject<TileRawChannelContainer>::const_iterator end=selRCs.end(); 
  m_nchan=0;
  m_tolE=0.0;
  for(; it != end; ++it) {
    m_tolE+=(*it)->amplitude();
    m_energy[m_nchan]=(*it)->amplitude();
    m_time[m_nchan]=(*it)->time();
    m_quality[m_nchan]=(*it)->quality();

    HWIdentifier hwid=(*it)->adc_HWID();

    Identifier id=(*it)->adc_ID();
    if (id.is_valid() ) {

      m_detector[m_nchan]=m_tileID->section(id);
      m_side[m_nchan]=m_tileID->side(id);
      m_sample[m_nchan]=m_tileID->sample(id);
      m_eta[m_nchan]=m_tileID->tower(id);
      m_phi[m_nchan]=m_tileID->module(id);
      m_pmt[m_nchan]=m_tileID->pmt(id);
      m_channel[m_nchan]=m_tileHWID->channel(hwid);
      m_gain[m_nchan]=m_tileHWID->adc(hwid);
      
    } else { 

      m_detector[m_nchan]=(m_tileHWID->ros(hwid)+1)/2;
      m_side[m_nchan]=(m_tileHWID->ros(hwid)%2)*2-1;
      m_sample[m_nchan]=-1;
      m_eta[m_nchan]=-1;
      m_phi[m_nchan]=m_tileHWID->drawer(hwid);
      m_pmt[m_nchan]=-1;
      m_channel[m_nchan]=m_tileHWID->channel(hwid);
      m_gain[m_nchan]=m_tileHWID->adc(hwid);
    }
      
    m_nchan++;

    if (m_nchan >= max_chan) break;
  }      
 
  // step3: commit ntuple
  CHECK( ntupleService()->writeRecord(m_ntuplePtr) );

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );
  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelToNtuple::finalize()
{
  ATH_MSG_INFO( "finalize() successfully" );
  return StatusCode::SUCCESS;
}

