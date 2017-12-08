/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArAutoCorrToolToDB.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  G.Unal
 CREATED:  Feb. 24, 2010 
    
 PURPOSE:  write LArAutoCorr in Database from LArAutoCorrTotalTool output
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArAutoCorrToolToDB.h"
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "CaloIdentifier/CaloGain.h"


LArAutoCorrToolToDB::LArAutoCorrToolToDB(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_onlineHelper(0),
    m_autocorrTool("LArAutoCorrTotalTool")
{
   declareProperty("AutoCorrTool",m_autocorrTool);
   declareProperty("GroupingType",    m_groupingType="ExtendedFeedThrough");
   declareProperty("AutoCorrKey",   m_acContName="LArAutoCorr");
}


LArAutoCorrToolToDB::~LArAutoCorrToolToDB()
{}

//----------------------------------------------------------------------------
StatusCode LArAutoCorrToolToDB::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_CHECK( m_autocorrTool.retrieve() );
  return StatusCode::SUCCESS;
}

// ---------------------------------------
StatusCode LArAutoCorrToolToDB::finalize()
{
  return StatusCode::SUCCESS;
}

// ---------------------------------------
StatusCode LArAutoCorrToolToDB::execute()
{  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode LArAutoCorrToolToDB::stop() {

  ATH_MSG_INFO ( ">>> stop()" );

  LArAutoCorrComplete* larAutoCorrComplete = new LArAutoCorrComplete();
  // Initialize LArAutoCorrComplete 
  ATH_CHECK( larAutoCorrComplete->setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( larAutoCorrComplete->initialize() );

  //Loop over gains
  for (unsigned igain=0;igain<(int)CaloGain::LARNGAIN;igain++) {
    CaloGain::CaloGain gain=(CaloGain::CaloGain)igain;
    //Loop over cells
    std::vector<HWIdentifier>::const_iterator it   =m_onlineHelper->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e =m_onlineHelper->channel_end();
    unsigned nSkipped=0;
    unsigned nDone=0;
    for(;it!=it_e;++it) {

      HWIdentifier chid = (*it);
     
      const std::vector<double>  AutoCorr = m_autocorrTool->autoCorrTotal(chid,igain,-1);
      // Not to process channels not existing in input elec. autocorr. DB
      if(AutoCorr.size() < 2 || (AutoCorr[0]==0. && AutoCorr[1]==0.)) {
	++nSkipped;
	continue;
      }
      for(unsigned ii=0; ii<AutoCorr.size(); ++ii) std::cout<<AutoCorr[ii]<<" : ";
      std::cout<<std::endl;
      const std::vector<double> rmsSampl =  m_autocorrTool->samplRMS(chid,igain,-1);
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
  ATH_CHECK( detStore()->record(larAutoCorrComplete,m_acContName) );
  ATH_MSG_INFO ( "Recorded LArAutCorrComplete object with key " << m_acContName );
  // Make symlink
  ATH_CHECK( detStore()->symLink(larAutoCorrComplete, (ILArAutoCorr*)larAutoCorrComplete) );
   
  return StatusCode::SUCCESS;
}








