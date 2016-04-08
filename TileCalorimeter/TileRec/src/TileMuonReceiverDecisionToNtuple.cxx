/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuonReceiverDecisionToNtuple.cxx
//  Author   : Joao Gentil Saraiva <jmendes@lxplus.cern.ch>
//  Created  : April 2014
//
//  DESCRIPTION:
//    Produce a simple output of a container into a ntuple
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Gaudi Includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Atlas include
#include "AthenaKernel/errorcheck.h"
#include "EventContainers/SelectAllObject.h"

// TileCal include
//
#include "TileEvent/TileContainer.h"

// header
//
#include "TileRec/TileMuonReceiverDecisionToNtuple.h"

// set a limit to the dimension of nt vectors
//
//const int max_inputs=128; // this is the maximum possible number of inputs for the 
                          // current configuration of tile-d trigger i.e. 1 input per EB module (2x64)

// Constructor 
TileMuonReceiverDecisionToNtuple::TileMuonReceiverDecisionToNtuple(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_TileMuRcvContainer("TileMuRcvCnt")
{
    m_ntupleLoc= "/FILE1";
    m_ntupleID = "TileMuRcvNt"; // usualy it is a number hxxx, but here we use a name

    declareProperty("TileMuonReceiverContainer" , m_TileMuRcvContainer); 
    declareProperty("NTupleLoc"                 , m_ntupleLoc);
    declareProperty("NTupleID"                  , m_ntupleID);
    declareProperty("NDecisionBits"             , m_nBits=4);
    declareProperty("NMaxTrigInputs"            , m_nMaxTrigInputs=128);
}


// Destructor
TileMuonReceiverDecisionToNtuple::~TileMuonReceiverDecisionToNtuple()
{
}



// Initialize
//
StatusCode TileMuonReceiverDecisionToNtuple::initialize() 
{ 
  ATH_MSG_INFO( "TileMuonReceiverDecisionToNtuple::initialize begins" );

  m_ntupleLoc="/NTUPLES" + m_ntupleLoc;

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleSvc(), m_ntupleLoc);

  if(!DirPtr) DirPtr=ntupleSvc()->createDirectory(m_ntupleLoc);
  if(!DirPtr) {
     ATH_MSG_ERROR( "Invalid Ntuple Directory: " );
     return StatusCode::FAILURE;
   }

   m_ntuplePtr=ntupleSvc()->book(DirPtr.ptr(), m_ntupleID,
                                     CLID_ColumnWiseTuple, "Tile Muon Receiver Ntuple");
  // if(!m_ntuplePtr) {

  //   std::string ntupleCompleteID=m_ntupleLoc+"/"+m_ntupleID;

  //   NTuplePtr nt(ntupleSvc(),ntupleCompleteID);
  //   if (!nt) {
  //     ATH_MSG_ERROR( "Failed to book or to retrieve ntuple "
  //        << ntupleCompleteID );
  //     return StatusCode::FAILURE;
  //   } else {
  //     ATH_MSG_INFO( "Reaccessing ntuple " << ntupleCompleteID );
  //     m_ntuplePtr = nt;
  //   }
  // }

  //
  // The container has an object for each module in a extended barrel partition. Are expected 128 entries per event.
  //
  // For each entry i will have : 
  //                              - thresholds (?) 4 (Global will be equal for all entries) | one vector for each event -> 128 id's
  //                              - decision ... NTuple::Matrix ?
  //                              - d6 energy, d6 time                   
  //                                d5+d6 energy, d5+d6 time                                | one vector combines each qty for each id -> NTuple::Array for ech qty
  //                              - id mod and part                                         | id is a convoluted number combining part and mod eg 356 part:3 mod:56 -> NTuple::Array for ech qty
  //

  CHECK (  m_ntuplePtr->addItem("TileMuRcv/nTrigInputs"        , m_nTrigInput,0,m_nMaxTrigInputs) );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/trigThr_Hi_D5andD6" , m_thershold_hi_d5andd6)          );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/trigThr_Lo_D5andD6" , m_thershold_lo_d5andd6)          );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/trigThr_Hi_D6"      , m_thershold_hi_d6)               );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/trigThr_Lo_D6"      , m_thershold_lo_d6)               );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/t_D5andD6"          , m_nTrigInput, m_time_D5andD6)    );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/t_D6"               , m_nTrigInput, m_time_D6)         );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/e_D5andD6"          , m_nTrigInput, m_energy_D5andD6)  );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/e_D6"               , m_nTrigInput, m_energy_D6)       );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/mod"                , m_nTrigInput, m_module)          );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/part"               , m_nTrigInput, m_partition)       );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/d_tile2tgcSL_bit0"  , m_nTrigInput, m_trigbit0)        );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/d_tile2tgcSL_bit1"  , m_nTrigInput, m_trigbit1)        );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/d_tile2tgcSL_bit2"  , m_nTrigInput, m_trigbit2)        );
  CHECK (  m_ntuplePtr->addItem("TileMuRcv/d_tile2tgcSL_bit3"  , m_nTrigInput, m_trigbit3)        );


  // ATH_MSG_INFO( "Initialization completed" );
  return StatusCode::SUCCESS;
}

// execute
StatusCode TileMuonReceiverDecisionToNtuple::execute()
{
  // step1: read container and declare temporary vectors
  //
  const TileMuonReceiverContainer *TileMuRcvCnt ; 
  CHECK( evtStore()->retrieve(TileMuRcvCnt, m_TileMuRcvContainer) );

  // step2: to fill items in ntuple
  //
  TileMuonReceiverContainer::const_iterator it  = TileMuRcvCnt->begin();  
  TileMuonReceiverContainer::const_iterator end = TileMuRcvCnt->end();

  ATH_MSG_INFO("Size of the container " << TileMuRcvCnt->size() );	

  if (it!=end){
    // the first entry in the container are the thresholds and was wrote like that to avoid
    // replicating this information in every element of the collection so to avoid other errors
    //
    const std::vector<float> & thresh = (*it)->GetThresholds(); 
    ++it;

    if (thresh.size()==4){
      ATH_MSG_VERBOSE( " thresholds: "<< thresh[0] <<" "<< thresh[1] <<" "<< thresh[2] <<" "<< thresh[3] );
      m_thershold_hi_d5andd6 = thresh[0];
      m_thershold_lo_d5andd6 = thresh[1];
      m_thershold_hi_d6      = thresh[2];
      m_thershold_lo_d6      = thresh[3];
    } else {
      ATH_MSG_WARNING( " unexpected thresholds vector size: "<< thresh.size());
      m_thershold_hi_d5andd6 = -1000.;
      m_thershold_lo_d5andd6 = -1000.;
      m_thershold_hi_d6      = -1000.;
      m_thershold_lo_d6      = -1000.;
    }
  } else {
    ATH_MSG_DEBUG( " no thresholds available" );
    m_thershold_hi_d5andd6 = -1000.;
    m_thershold_lo_d5andd6 = -1000.;
    m_thershold_hi_d6      = -1000.;
    m_thershold_lo_d6      = -1000.;
  }
  
  m_nTrigInput=0;

  for(; it != end; ++it) {

    if (m_nTrigInput >= m_nMaxTrigInputs) {
      ATH_MSG_WARNING(" Number of input objects exceeds max size = " << m_nMaxTrigInputs);
      break;
    }
    
    const TileMuonReceiverObj * obj = (*it);
    
    // going over the collection of inputs to the L1Muon
    //
    // (1) ID

    int id = obj->GetID();
    int part = id/100;
    int module = id - 100*part;
    m_partition[m_nTrigInput] = part;
    m_module[m_nTrigInput] = module;
    if (id>0) {
      ATH_MSG_DEBUG( " index: " << m_nTrigInput << " id: " << id << " - " << ((part<4)?"EBA":"EBC") << " " << module);
    } else {
      ATH_MSG_WARNING( " index: " << m_nTrigInput << " unexpected id: " << id);
    }
    
    // (2) Energy

    const std::vector<float> & ene = obj->GetEne(); 

    if (ene.size()==2){
      ATH_MSG_VERBOSE( " energy: " << ene[0] << " " << ene[1]);
      m_energy_D5andD6[m_nTrigInput] = ene[0];
      m_energy_D6[m_nTrigInput]      = ene[1];
    } else {
      ATH_MSG_WARNING( " unexpected energy vector size: " << ene.size());
      m_energy_D5andD6[m_nTrigInput] = -1000.;
      m_energy_D6[m_nTrigInput]      = -1000.;
    }
    
    // (3) Time d6:t1+t2/2 d5+d6:t1+t2+t3+t4/4

    const std::vector<float> & time = obj->GetTime(); 

    if (time.size()==2){
       ATH_MSG_VERBOSE(" time: " << time[0] << " " << time[1] );
       m_time_D5andD6[m_nTrigInput] = time[0];
       m_time_D6[m_nTrigInput]      = time[1];
    } else {
       ATH_MSG_WARNING(" unexpected time vector size: " << time.size());
       m_time_D5andD6[m_nTrigInput] = -1000.;
       m_time_D6[m_nTrigInput]      = -1000.;
    }
    
    // (4) decision

    const std::vector<bool> & decision = obj->GetDecision(); 

    if (decision.size()==4){
       ATH_MSG_DEBUG( " decision: "<< decision[0] <<" "<< decision[1] <<" "<< decision[2] <<" "<< decision[3]) ;
       m_trigbit0[m_nTrigInput] = decision[0]; // d5+d6 hi
       m_trigbit1[m_nTrigInput] = decision[1]; // d5+d6 lo
       m_trigbit2[m_nTrigInput] = decision[2]; // d6 hi
       m_trigbit3[m_nTrigInput] = decision[3]; // d6 lo     is this right? Ie d6 lo is 0?
    } else {
       ATH_MSG_WARNING( " unexpected decision vector size: "<< decision.size()) ;
       m_trigbit0[m_nTrigInput] = false;
       m_trigbit1[m_nTrigInput] = false;
       m_trigbit2[m_nTrigInput] = false;
       m_trigbit3[m_nTrigInput] = false;
    }
    
    m_nTrigInput++;
  }

  ATH_MSG_INFO(" Number of inputs " << m_nTrigInput); 

  // step3: commit ntuple
  //
  CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );

  // Execution completed.
  //
  ATH_MSG_DEBUG( "execute() completed successfully" );
  return StatusCode::SUCCESS;
}

// finalize
StatusCode TileMuonReceiverDecisionToNtuple::finalize()
{
  ATH_MSG_INFO( "finalize() successfully" );
  return StatusCode::SUCCESS;
}
