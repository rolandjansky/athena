/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/FakeLArOFCs.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArIdentifier/LArOnlineID.h"


FakeLArOFCs::FakeLArOFCs(const std::string & name, ISvcLocator * pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("MaxSample",m_maxSample=2);
  declareProperty("NSample",m_NSample=5);
  declareProperty("OFCKey",m_keyOFC="LArOFC");
}

FakeLArOFCs::~FakeLArOFCs() {}

                     
StatusCode FakeLArOFCs::initialize() {
  const LArOnlineID* onlineHelper = nullptr;
  ATH_CHECK( detStore()->retrieve(onlineHelper, "LArOnlineID") );

  std::vector<std::vector<float> > aVec(25);
  for (int i=0;i<m_NSample;i++)
    if (i==m_maxSample)
      aVec[0].push_back(1);
    else
      aVec[0].push_back(0);

  std::vector< std::vector<float> > bVec(25); //Stays zero-size;
  LArOFCComplete* larOFCComplete=new LArOFCComplete();
  for(int igain=0; igain<3; igain++) {
    for (HWIdentifier ch_id : onlineHelper->channel_range()) {
      larOFCComplete->set(ch_id,(int)igain,aVec,bVec);
    }
  }//End loop over gains.
  ATH_CHECK( detStore()->record(larOFCComplete,m_keyOFC) );
  const ILArOFC* iLArOFC=larOFCComplete;
  ATH_CHECK (detStore()->symLink(larOFCComplete,iLArOFC) );
  return StatusCode::SUCCESS;
}

StatusCode FakeLArOFCs::execute()
{return StatusCode::SUCCESS;}
  
StatusCode FakeLArOFCs::finalize()
{return StatusCode::SUCCESS;}
