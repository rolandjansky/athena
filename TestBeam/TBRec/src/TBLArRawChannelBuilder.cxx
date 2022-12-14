/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBLArRawChannelBuilder.h"
#include "LArIdentifier/LArOnlineID.h"

#include <cmath>


using CLHEP::MeV;
using CLHEP::nanosecond;


TBLArRawChannelBuilder::TBLArRawChannelBuilder (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_emId(0),
  m_fcalId(0),
  m_hecId(0),
  m_onlineHelper(0),
  m_DataLocation("FREE"),
  m_ChannelContainerName("LArRawChannels"),
  m_ADCtoMeVFCAL(),
  m_ADCtoMeVHEC(),
  m_ADCtoMeVEMECInner(),
  m_ADCtoMeVEMECOuter(),
  m_ADCtoMeVEMB(),
  m_iPedestal(0)                              // jobO ?
{
   //m_useIntercept={false,false,false,false};
 declareProperty("LArRawChannelContainerName",m_ChannelContainerName);
 declareProperty("DataLocation", m_DataLocation );
 declareProperty("maxSamp",m_imaxSamp=8);
 declareProperty("RecoMode",m_mode="CUBIC");
 declareProperty("CubicAdcCut",m_cubicAdcCut=50.0);
}

StatusCode TBLArRawChannelBuilder::initialize(){
  ATH_MSG_DEBUG ( "In Initialize." );

  const CaloCell_ID* caloId = nullptr;
  ATH_CHECK( detStore()->retrieve (caloId, "CaloCell_ID") );
  m_emId=caloId->em_idHelper();
  m_fcalId=caloId->fcal_idHelper();
  m_hecId=caloId->hec_idHelper();

  ATH_CHECK( m_cablingKey.initialize() );

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  
  m_ADCtoMeVFCAL[0]      =   87.0 * MeV;  // FCAL1 High gain
  m_ADCtoMeVFCAL[1]      =  117.0 * MeV;  // FCAL2 High gain
  m_ADCtoMeVFCAL[2]      =  193.0 * MeV;  // FCAL3 High gain
  m_ADCtoMeVHEC[0]       =  136.0 / 9.8 * MeV;  // HEC 1 Medium gain from Monika.  Need / 9.8  ??
  m_ADCtoMeVHEC[1]       =  136.0 / 9.8 * MeV;  // HEC 2 Medium gain from Monika.  Need / 9.8  ??
  m_ADCtoMeVEMECInner[0] =   25.22 * MeV; // EMEC  High gain from Pascal, approximate
  m_ADCtoMeVEMECInner[1] =   19.4  * MeV;  // EMEC  High gain from Pascal, approximate
  m_ADCtoMeVEMECOuter[0] =   16.0  * MeV;  // EMEC  High gain from Monika, approximate
  m_ADCtoMeVEMECOuter[1] =   16.0  * MeV;  // EMEC  High gain from Monika, approximate
  m_ADCtoMeVEMB[0]       =   15.0  * MeV;  // EMB   High gain from Isabelle, approximate
  m_ADCtoMeVEMB[1]       =   15.0  * MeV;  // EMB   High gain from Isabelle, approximate
  
  return StatusCode::SUCCESS;
}


StatusCode TBLArRawChannelBuilder::execute() {
  ATH_MSG_DEBUG ( "In execute" );
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not hame cabling mapping from key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  //Pointer to input data container
  const LArDigitContainer* digitContainer;//Pointer to LArDigitContainer

  //Retrieve Digit Container
  ATH_CHECK( evtStore()->retrieve(digitContainer,m_DataLocation) );
  ATH_MSG_DEBUG ( "1) LArDigitContainer container size = " <<  digitContainer->size() );

  ATH_MSG_DEBUG ( "2) LArDigitContainer container size = " <<  digitContainer->size() );
  if( digitContainer->size() < 1 ) {
    ATH_MSG_INFO ( "Empty LArDigitContainer container." );
    return StatusCode::SUCCESS;
  }
  
  //Prepare LArRawChannelContainer
  ATH_MSG_DEBUG ( "Making new LArRawChannelContainer " <<  digitContainer->size() );
  LArRawChannelContainer* larRawChannelContainer=new LArRawChannelContainer();
  larRawChannelContainer->reserve(digitContainer->size());
  ATH_CHECK( evtStore()->record(larRawChannelContainer,m_ChannelContainerName) );
  
  //Now all data is available, start loop over Digit Container
  ATH_MSG_DEBUG ( "Loop over Digit Container " );

  for (const LArDigit* digit : *digitContainer) {
    
    //Get data from LArDigit
    const std::vector<short>& samples  = digit->samples();
    unsigned int              nSamples = samples.size(); 
    const HWIdentifier        chid     = digit->channelID();
    const CaloGain::CaloGain  gain     = digit->gain();

    //>>>> PL June 20, 2004: subtract pedestal first, assume first sample ->JO?
    std::vector<float> mySamples;
    mySamples.resize(samples.size());
    // FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! only for 5 leading noise samples!
    float thePedestal = (float)samples[m_iPedestal];
    //    log << MSG::INFO
    //	<< "pedestal " << thePedestal << endmsg;
    for ( unsigned int i=0; i<samples.size(); i++ )
      {
	mySamples[i] = ((float)samples[i]) - thePedestal;
      }
    //<<<<

    float GainFactor;
    if( gain == CaloGain::LARLOWGAIN ) {
      GainFactor = 9.8*9.8;
    } else if (gain == CaloGain::LARMEDIUMGAIN ) {
      GainFactor = 9.8;
    } else if (gain == CaloGain::LARHIGHGAIN ) {
      GainFactor = 1.0;
    } else {
      GainFactor = 1.0;
      ATH_MSG_ERROR ( "Channel " << chid << "unknown gain: " << gain );
    }
    
    // Find peak time sample
    float        maxADCPeak = 0.;
    unsigned int iPeakSamp  = 0;
    for( unsigned i=0;i<nSamples;i++ ) {
      if ( maxADCPeak < mySamples[i] ) 
	{
	  maxADCPeak = mySamples[i];
	  iPeakSamp  = i;
	}
    }

    bool CubicFailed = false;
    float ADCPeak=0.;
    float time=-999.;

    // maximum amplitude/variable time slice method
    if( m_mode == "MAX" ) 
      {
	ADCPeak = maxADCPeak;
	time    = ((float)iPeakSamp+0.5) * 25.0 * nanosecond;
      }

    // cubic extrapolation for selected channels
    else if ( m_mode == "CUBIC" &&
	      ! ( CubicFailed = maxADCPeak <= m_cubicAdcCut ) )  
      {
	// linear extrapolation table
	unsigned int it0;
	const float invT[4][4] 
	  = { {        1,       0,       0,        0},
	      { -1.83333,       3,    -1.5, 0.333333},
	      {        1,    -2.5,       2,     -0.5},
	      {-0.166666,     0.5,    -0.5, 0.166666}  };

	// peak slice very early
	if ( iPeakSamp <= 1 ) 
	  {
	    it0 = m_iPedestal + 1;
	  }
	// peak slice very late
	else if ( iPeakSamp >= nSamples - 2 ) 
	  {
	    it0 = nSamples - 4;
	  }
	// peak in safe region 
	else 
	  {
	    it0 = ( mySamples[iPeakSamp-2] > mySamples[iPeakSamp+2] )
	      ? iPeakSamp - 2
	      : iPeakSamp - 1;
	  }

	// 4 samples to be used start at 0 <= t0 <= nsamples-4
	float A[4] = {0, 0, 0, 0};
	float dtmax = 0.0;
	float disc;
	// S = TA -> A = inv(T)S
	for (int ia = 0; ia < 4; ia++) 
	  for (int it = 0; it < 4; it++) 
	    A[ia] += invT[ia][it] * mySamples[it0+it];

	// fit parameters
	disc = A[2]*A[2] - 3*A[1]*A[3];
	if ( ! ( CubicFailed = ( disc < 0 || A[3] == 0 ) ) ) 
	  {
	    dtmax = (-A[2]-sqrt(disc))/(3*A[3]);
	    if ( ! ( CubicFailed = ( dtmax < 0 || dtmax > 3 ) ) ) 
	      {
		time = (float(it0) + dtmax) * 25.0 * nanosecond; // nsec
		for(int ia = 0; ia < 4; ia++)
		  ADCPeak += A[ia] * pow(dtmax, ia);
	      }
	  }
      }
    ATH_MSG_DEBUG
      ( "Flag: "
	<< CubicFailed
	<< " Mode: "
	<< m_mode
	<< " Signal: "
	<<  ADCPeak
	<< " Peak: "
	<< maxADCPeak
	);

    // fixed time slice or insufficient signal for cubic fit    
    if(m_mode == "FIXED" || CubicFailed ) 
      {
	ADCPeak = mySamples[m_imaxSamp];
	time    = (float(m_imaxSamp)+0.5) * 25.0 * nanosecond;
      }


    // Now must get subdetector ID and feed in here ...
    float ADCtoMeV      = 10.0;
    const Identifier id = cabling->cnvToIdentifier(chid);
    if (m_emId->is_em_barrel(id)) {
      // m_emId->sampling(id);
      ADCtoMeV = m_ADCtoMeVEMB[0];
      ATH_MSG_DEBUG ( " in EMB s="<<m_emId->sampling(id)<<", using ADCtoMeV = " << ADCtoMeV );
    } else if (m_emId->is_em_endcap_inner(id)) {
      int samp = m_emId->sampling(id);
      if (samp > 0) {
        ADCtoMeV = m_ADCtoMeVEMECInner[samp-1];
        ATH_MSG_DEBUG ( " in EMEC inner s="<<m_emId->sampling(id)<<", using ADCtoMeV = " << ADCtoMeV );
      }
    } else if (m_emId->is_em_endcap_outer(id)) {
      int samp = m_emId->sampling(id);
      if (samp > 0) {
        ADCtoMeV = m_ADCtoMeVEMECOuter[samp-1];
        ATH_MSG_DEBUG ( " in EMEC outer s="<<m_emId->sampling(id)<<", using ADCtoMeV = " << ADCtoMeV );
      }
    } else if (m_fcalId->is_lar_fcal(id)) {
      int module = m_fcalId->module(id);
      if (module > 0) {
        ADCtoMeV = m_ADCtoMeVFCAL[module-1];
        ATH_MSG_DEBUG ( " in FCAL m=" << m_fcalId->module(id)<<", using ADCtoMeV = " << ADCtoMeV );
      }
    } else if (m_hecId->is_lar_hec(id)) {
      // m_layer[m_cellIndex]=m_hecId->sampling(id);
      ADCtoMeV = m_ADCtoMeVHEC[0];
      ATH_MSG_DEBUG ( " in HEC s="<<m_hecId->sampling(id)<<", using ADCtoMeV = " << ADCtoMeV );
    }
    
    float Energy  = ADCPeak * ADCtoMeV * GainFactor;
    uint16_t quality=0; 
    uint16_t provenance=0x2000; 

    //Make LArRawChannel Object with new data
    LArRawChannel larRawChannel(chid,(int)Energy,(int)time,quality, provenance, gain);
    larRawChannelContainer->add(larRawChannel); //Add to container
    
  }// End loop over LArDigits

  ATH_MSG_DEBUG ( "Finished loop over Digit Container " );

  //  larRawChannelContainer->print();
  
  ATH_MSG_DEBUG ( "sorted RawChannelContainer, now lock it " );
  // lock raw channel container
  ATH_CHECK( evtStore()->setConst(larRawChannelContainer) );

  return StatusCode::SUCCESS;
}

StatusCode TBLArRawChannelBuilder::finalize()
{
  ATH_MSG_INFO ( "TBLArRawChannelBuilder finished." );
  return StatusCode::SUCCESS;
}
