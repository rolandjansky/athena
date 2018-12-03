/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArPedestals2Ntuple.h"
#include "LArRawConditions/LArPedestalComplete.h"
#include "CaloIdentifier/CaloGain.h"


LArPedestals2Ntuple::LArPedestals2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): LArCond2NtupleBase(name, pSvcLocator)
{ 
  declareProperty("ContainerKey",m_contKey);
  m_ntTitle="Pedestals";
  m_ntpath="/NTUPLES/FILE1/PEDESTALS";
    
}

LArPedestals2Ntuple::~LArPedestals2Ntuple() 
{}

StatusCode LArPedestals2Ntuple::stop() {
  StatusCode sc;
  const ILArPedestal* larPedestal;
  sc=m_detStore->retrieve(larPedestal,m_contKey);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Unable to retrieve ILArPedestal with key " 
	      << m_contKey << " from DetectorStore" );
    return StatusCode::FAILURE;
  } 

  NTuple::Item<long> cellIndex,gain;
  NTuple::Item<double> ped;
  NTuple::Item<double> rms;


  sc=m_nt->addItem("icell",cellIndex,0,200000);
  if (sc!=StatusCode::SUCCESS)
    {ATH_MSG_ERROR( "addItem 'Cell Index' failed" );
    return StatusCode::FAILURE;
  }

  sc=m_nt->addItem("gain",gain,0,3);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'gain' failed" );
    return StatusCode::FAILURE;
  }


  sc=m_nt->addItem("ped",ped,-1000.,5000.);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'ped' failed" );
    return StatusCode::FAILURE;
  }

  sc=m_nt->addItem("rms",rms,0.,1e12);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'rms' failed" );
    return StatusCode::FAILURE;
  }


 unsigned cellCounter=0;
 for(long igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++) {
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     if (larPedestal->pedestal(hwid,igain)>ILArPedestal::ERRORCODE) {
       fillFromIdentifier(hwid);       
       cellIndex = cellCounter;
                     
       ped=larPedestal->pedestal(hwid,igain);
       rms=larPedestal->pedestalRMS(hwid,igain);
       gain=igain;

       sc=ntupleSvc()->writeRecord(m_nt);
       if (sc!=StatusCode::SUCCESS) {
	 ATH_MSG_ERROR( "writeRecord failed" );
	 return StatusCode::FAILURE;
       }
     }// end if Pedestal exists for this channel
     cellCounter++;
   }//end loop over gains
 }//end loop over online ID

 ATH_MSG_INFO( "LArPedestals2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
   
