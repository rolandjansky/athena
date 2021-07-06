/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArAutoCorrAlgToDB.cxx
 PACKAGE:  LArCalorimeter/LArCalibUtils
 
 AUTHORS:  P. Strizenec, based on Tool by G. Unal

 PURPOSE:  write LArAutoCorr in Database from LArAutoCorrTotal conditions object
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArAutoCorrAlgToDB.h"
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "CaloIdentifier/CaloGain.h"


LArAutoCorrAlgToDB::LArAutoCorrAlgToDB(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_onlineHelper(0)
{
   declareProperty("GroupingType",    m_groupingType="ExtendedFeedThrough");
   declareProperty("OutAutoCorrKey",  m_acContName="LArPhysAutoCorr");
   declareProperty("isSC",m_isSC=false);
}


LArAutoCorrAlgToDB::~LArAutoCorrAlgToDB()
{}

//----------------------------------------------------------------------------
StatusCode LArAutoCorrAlgToDB::initialize()
{
  StatusCode sc;
  if ( m_isSC ) {
    const LArOnline_SuperCellID* ll;
    sc = detStore()->retrieve(ll, "LArOnline_SuperCellID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
      return StatusCode::FAILURE;
    }
    else {
      m_onlineHelper = (const LArOnlineID_Base*)ll;
      ATH_MSG_DEBUG("Found the LArOnlineID helper");
    }
    m_nGains=1;
  } else { // m_isSC
    const LArOnlineID* ll;
    sc = detStore()->retrieve(ll, "LArOnlineID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
      return StatusCode::FAILURE;
    }
    else {
      m_onlineHelper = (const LArOnlineID_Base*)ll;
      ATH_MSG_DEBUG(" Found the LArOnlineID helper. ");
    }
    m_nGains=(unsigned) CaloGain::LARNGAIN;
  }
  
  ATH_CHECK( m_autocorrKey.initialize() );
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode LArAutoCorrAlgToDB::stop() {

  ATH_MSG_INFO ( ">>> stop()" );

  // ReadHandle setup
  SG::ReadCondHandle<LArAutoCorrTotal> acHdl(m_autocorrKey);
  const LArAutoCorrTotal *acTotal = *acHdl;
  if(!acTotal) {
     ATH_MSG_ERROR("Could not read LArAutoCorrTotal with key " <<m_autocorrKey.key()<<" from ConditionsStore");
     return StatusCode::FAILURE;
  }  
 
  auto larAutoCorrComplete = std::make_unique<LArAutoCorrComplete>();
  // Initialize LArAutoCorrComplete 
  ATH_CHECK( larAutoCorrComplete->setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( larAutoCorrComplete->initialize() );

  //Loop over gains
  for (unsigned igain=0;igain<m_nGains;igain++) {
    CaloGain::CaloGain gain=(CaloGain::CaloGain)igain;
    //Loop over cells
    std::vector<HWIdentifier>::const_iterator it   =m_onlineHelper->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e =m_onlineHelper->channel_end();
    unsigned nSkipped=0;
    unsigned nDone=0;
    for(;it!=it_e;++it) {

      HWIdentifier chid = (*it);
     
      const std::vector<double>  AutoCorr = acTotal->autoCorrTotal(chid,igain,-1);
      // Not to process channels not existing in input elec. autocorr. DB
      if(AutoCorr.size() < 2 || (AutoCorr[0]==0. && AutoCorr[1]==0.)) {
	++nSkipped;
	continue;
      }
      const std::vector<double> rmsSampl =  acTotal->samplRMS(chid,igain,-1);
      unsigned int nsamples_AC = (1+((int)(sqrt(1+8*AutoCorr.size()))))/2;

      std::vector<float> cov;
      unsigned int ntot = nsamples_AC*(nsamples_AC+1) / 2;
      cov.resize(ntot,0.);

      unsigned int k=0;
      for (unsigned i=0;i<nsamples_AC;i++) {  
         for (unsigned j=i;j<nsamples_AC;j++,k++) {
          double AC;
          if (i==j) {
             AC=1.;
          }
          else {
             int i1=std::min(i,j);
             int i2=std::max(i,j);
             int index = i1*nsamples_AC - i1*(i1+1)/2 -(i1+1) + i2; 
             AC=AutoCorr[index];
          }
          AC = AC*rmsSampl[i]*rmsSampl[j];
          cov[k] = AC;
        }
      }

      larAutoCorrComplete->set(chid,gain,cov);
      ++nDone;
    }//end loop over all cells
    ATH_MSG_INFO ( "Gain " << gain << ": " << nDone << " channeles done, " << nSkipped  << " channels skipped (no Elec Noise AC in input)" );
  }//end loop over gains

  // Record LArAutoCorrComplete
  ATH_CHECK( detStore()->record(std::move(larAutoCorrComplete),m_acContName) );
  ATH_MSG_INFO ( "Recorded LArAutCorrComplete object with key " << m_acContName );
   
  return StatusCode::SUCCESS;
}








