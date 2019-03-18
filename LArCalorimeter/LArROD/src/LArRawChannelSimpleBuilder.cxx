/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArRawChannelSimpleBuilder.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "LArElecCalib/ILArPedestal.h"

#include <cmath>

using CLHEP::nanosecond;
using CLHEP::ns;
using CLHEP::MeV;

LArRawChannelSimpleBuilder::LArRawChannelSimpleBuilder (const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator),
  //  m_roiMap("LArRoI_Map"),
  m_emId(0),
  m_fcalId(0),
  m_hecId(0),
  m_onlineHelper(0),
  m_adc2mevTool("LArADC2MeVTool"),
  m_peakParabolaTool("LArParabolaPeakTool"),
  m_iPedestal(0)// jobO ?
{
   //m_useIntercept={false,false,false,false};
 declareProperty("maxSamp",m_imaxSamp=8);
 declareProperty("RecoMode",m_mode="CUBIC");
 declareProperty("CubicRecoTimeModeFCAL",m_FCALmodeTime="LINEAR");
 declareProperty("CubicAdcCut",m_cubicAdcCut=15.0);
 declareProperty("PedestalSample",m_iPedestal=0);
 declareProperty("UsePedestalDB",m_usePedestalDB=false);
 declareProperty("UseRampDB",m_useRampDB=false);
 declareProperty("AverageSamplesEM",m_averageSamplesEM=5); 
 declareProperty("AverageSamplesHEC",m_averageSamplesHEC=5); 
 declareProperty("AverageSamplesFCAL",m_averageSamplesFCAL=3); 
 declareProperty("AverageScaleEM",m_averageScaleEM=2.6); 
 declareProperty("AverageScaleHEC",m_averageScaleHEC=2.6); 
 declareProperty("AverageScaleFCAL",m_averageScaleFCAL=1.8); 
 declareProperty("ADC2MeVTool",m_adc2mevTool);

 // m_peakParabolaTool = NULL ; // FIXME RS use empty ToolHandle - python

  m_ADCtoMeVFCAL[0]      =   87.0 * MeV;  // FCAL1 High gain
  m_ADCtoMeVFCAL[1]      =  117.0 * MeV;  // FCAL2 High gain
  m_ADCtoMeVFCAL[2]      =  193.0 * MeV;  // FCAL3 High gain
  m_ADCtoMeVHEC[0]       =  136.0 / 9.8 * MeV;  // HEC 1 Medium gain from Monika.  Need / 9.8  ?? 
  m_ADCtoMeVHEC[1]       =  136.0 / 9.8 * MeV;  // HEC 2 Medium gain from Monika.  Need / 9.8  ?? 
  // m_ADCtoMeVEMECInner[0] =   25.22 * MeV; // EMEC  High gain from Pascal, approximate
  // m_ADCtoMeVEMECInner[1] =   19.4  * MeV;  // EMEC  High gain from Pascal, approximate
  m_ADCtoMeVEMECInner[0] =   20.0 * MeV;  // EMEC IW s=1  High gain : fixed 18/8/2004 RMcP
  m_ADCtoMeVEMECInner[1] =   20.0 * MeV;  // EMEC IW s=2 High gain : fixed 18/8/2004 RMcP
  m_ADCtoMeVEMECOuter[0] =   16.0 * MeV;  // EMEC OW pre, s=0 
  m_ADCtoMeVEMECOuter[1] =   16.0 * MeV;  // EMEC OW s=1 High gain from Monika, approximate
  m_ADCtoMeVEMECOuter[2] =   16.0 * MeV;  // EMEC OW s=2 High gain from Monika, approximate
  m_ADCtoMeVEMECOuter[3] =   16.0 * MeV;  // EMEC OW s=3 High gain from Monika, approximate
  m_ADCtoMeVEMB[0]       =   7.0  * MeV;  // EMB pre, s=0 High gain from Isabelle, approximate
  m_ADCtoMeVEMB[1]       =   2.5  * MeV;  // EMB s=1 High gain from Isabelle, approximate
  m_ADCtoMeVEMB[2]       =   18.0 * MeV;  // EMB s=2 High gain from Isabelle, approximate
  m_ADCtoMeVEMB[3]       =   9.0  * MeV;  // EMB s=3 High gain from Isabelle, approximate


}

StatusCode LArRawChannelSimpleBuilder::initialize()
{
  ATH_MSG_DEBUG( "In Initialize."  );

  if (m_useRampDB) {
    ATH_CHECK( m_adc2mevTool.retrieve() );
  }
  
  if ( m_mode == "PARABOLA"){
    if (m_peakParabolaTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR( "Can't get LArParabolaPeakRecoTool"  );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "LArParabolaPeakRecoTool retrieved with success!"  );
  }

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();
  m_fcalId=idHelper->fcal_idHelper();
  m_hecId=idHelper->hec_idHelper();

  ATH_CHECK(m_cablingKey.initialize());

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );


  ATH_CHECK( m_DataLocation.initialize() );
  ATH_CHECK( m_ChannelContainerName.initialize() );
  return StatusCode::SUCCESS;
}



StatusCode LArRawChannelSimpleBuilder::execute (const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "In execute"  );
  
  //Retrieve Digit Container
  ATH_MSG_DEBUG( "About to retrieve LArDigitContainer with key " << m_DataLocation  );
  SG::ReadHandle<LArDigitContainer> digitContainer (m_DataLocation, ctx);
  ATH_MSG_DEBUG( "1) LArDigitContainer container size = " <<  digitContainer->size()  );

  ATH_MSG_DEBUG( "2) LArDigitContainer container size = " <<  digitContainer->size()  );
  if( digitContainer->size() < 1 ) {
    ATH_MSG_INFO( "Empty LArDigitContainer container."  );
    return StatusCode::SUCCESS;
  }
  
  auto larRawChannelContainer = std::make_unique<LArRawChannelContainer>();

  //Pointer to conditions data objects 
  const ILArPedestal* larPedestal=NULL;
  if (m_usePedestalDB) {
    if (detStore()->retrieve(larPedestal).isFailure()) {
      larPedestal=NULL;
      ATH_MSG_DEBUG( "No pedestal found in database. Use default values."  );
    }
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  //loop twice over the digits. In the first pass the best window is
  //found for the averaged signal. In the second pass the full raw
  //channel is created and the averaging in the window found in the
  //first path is used in case the cubic fit fails or the max ADC is
  //below threshold

  int nMinEM(0),nMinHEC(0),nMinFCAL(0);
  std::vector<float> fSumEM,fSumHEC,fSumFCAL;
  std::vector<float> *pSum = 0;
  
  fSumEM.resize(0);
  fSumHEC.resize(0);
  fSumFCAL.resize(0);

  for(int iloop=0;iloop<2;iloop++) {
   
    //Now all data is available, start loop over Digit Container
    ATH_MSG_DEBUG( "Loop over Digit Container "  );

    for (const LArDigit* digit : *digitContainer) {
    
      //Get data from LArDigit
      const std::vector<short>& samples  = digit->samples();
      unsigned int              nSamples = samples.size(); 
      const HWIdentifier        chid     = digit->channelID();
      const CaloGain::CaloGain  gain     = digit->gain();
      
      
      float thePedestal=-1;    
      if (larPedestal) {
	float DBpedestal =larPedestal->pedestal(chid,gain);
	if (DBpedestal <= (1.+LArElecCalib::ERRORCODE))
	  thePedestal=DBpedestal;
      }
      if (thePedestal<0) {
	thePedestal = (float)samples[m_iPedestal];
	ATH_MSG_DEBUG( "No pedestal found for this cell. Use default value " << thePedestal  );
      }
      //>>>> PL June 20, 2004: subtract pedestal first, assume first sample ->JO?
      std::vector<float> mySamples;
      mySamples.resize(samples.size());
      // FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! only for 5 leading noise samples!
      
      //    log << MSG::INFO
      //  << "pedestal " << thePedestal << endmsg;
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
	ATH_MSG_ERROR( "Channel " << chid << "unknown gain: " << gain  );
      }
      
      // Get hardware identifier for later use.
      HWIdentifier FebID = m_onlineHelper->feb_Id(chid);
      unsigned int channel  = m_onlineHelper->channel(chid);
      ATH_MSG_DEBUG( std::hex << " FebID / chid / channel = " << FebID << " / " << chid << " / " << channel );
      
      // And note if this is an FCAL channel with fast pulses (cubic fails)
      // EM and HEC are also tested to adjust the signal range for averaging
      bool isFCAL = false;
      bool isEM   = false;
      bool isHEC  = false;
      int nMin = 0;
      unsigned int nAverage = 1;
      float myScale = 1;
      try {
	const Identifier id = cabling->cnvToIdentifier(chid);    
	if (m_fcalId->is_lar_fcal(id)) {
	  isFCAL = true;
	  nMin = nMinFCAL;
	  pSum = &fSumFCAL;
	  nAverage = m_averageSamplesFCAL;
	  myScale = m_averageScaleFCAL;
	}
	else if (m_emId->is_lar_em(id)) {
	  isEM = true;
	  nMin = nMinEM;
	  pSum = &fSumEM;
	  nAverage = m_averageSamplesEM;
	  myScale = m_averageScaleEM;
	}
	else if (m_hecId->is_lar_hec(id)) {
	  isHEC = true;
	  nMin = nMinHEC;
	  pSum = &fSumHEC;
	  nAverage = m_averageSamplesHEC;
	  myScale = m_averageScaleHEC;
	}
      }
      //catch( LArOnlID_Exception & except) 
      catch (LArID_Exception & execpt) {
	ATH_MSG_DEBUG (" is disconnected.");
        // The question now being:  do we want to skip this channel???
        // Probably yes, so do so.  RMcP 9 June 2006
        continue;
      }
      // In the unrealistic case, that pSum is still not defined
      if(!pSum) continue;

      // Find peak time sample and scaled average for selected sample range 
      // (i.e. poor man's digital filter)

      // sanity checks
      if ( nAverage > nSamples ) {
	ATH_MSG_WARNING( " Number of samples to average (" 
                         << nAverage << ") is larger than total number of samples (" 
                         << nSamples << ")! adjusting nAverage ... "  );
	nAverage = nSamples;
      }


      if ( iloop == 0 ) {

        if (pSum == 0) {
          ATH_MSG_ERROR( " don't have pSum pointer." );
          continue;
        }
  
	if ( pSum->size() == 0) 
//	  pSum->resize(nSamples-nAverage+1,0);
	  pSum->resize(nSamples,0);

	for( unsigned i=0;i<nSamples-nAverage+1;i++ ) {
	  for( unsigned j=0;j<nAverage;j++ ) {
	    (*pSum)[i] += mySamples[i+j];
	  }
	} 
      }
      else {
	float        maxADCPeak = 0.;
	unsigned int iPeakSamp  = 0;
	float        averagedADC = 0;
	for( unsigned i=0;i<nSamples;i++ ) {
	  if ( maxADCPeak < mySamples[i] ) 
	    {
	      maxADCPeak = mySamples[i];
	      iPeakSamp  = i;
	    }
	  if ( (int)i >= nMin && i < nMin+nAverage ) 
	    averagedADC += mySamples[i];
	}
	averagedADC /= myScale;

	bool CubicFailed = false;
	float ADCPeak=0.;
	float time=0;
      
      
	// maximum amplitude/variable time slice method
	if( m_mode == "MAX" ) 
	  {
	    ADCPeak = maxADCPeak;
	    time    = ((float)iPeakSamp) * 25.0 * nanosecond;
	  }
      
      
	// other choices are different for FCAL and others
	else {
	
	  // first, deal with FCAL: only one reconstruction is available
	  if(isFCAL &&   m_mode == "CUBIC" &&
	     ! ( CubicFailed = maxADCPeak <= m_cubicAdcCut ) ) {
	  
	    ATH_MSG_DEBUG( " Special reconstruction for FCAL."  );
          
	    unsigned int it0;
	  
	    const float invT[3][3] 
	      = { {    1,       0,       0 },
		  { -1.5,       2,    -0.5 },
		  {  0.5,      -1,     0.5 }  };
	  
	    // peak slice very early
	    if ( iPeakSamp <= 1 ) {
	      it0 = 1;
	    } else if ( iPeakSamp >= nSamples - 1 ) {  // peak is late
	      it0 = nSamples - 3;
	    } else { // peak in safe region
	      it0 = iPeakSamp - 1;
	    }
	  
	    // Quadratic interpolation using
	    // 3 samples to be used start at 0 <= t0 <= nsamples-3
	    float A[3] = {0, 0, 0};
	    float dtmax = 0.0;
	    // S = TA -> A = inv(T)S
	    for (int ia = 0; ia < 3; ia++) 
	      for (int it = 0; it < 3; it++) 
		A[ia] += invT[ia][it] * mySamples[it0+it];
	  
	    // fit parameters
	    if ( not ( CubicFailed = ( A[2] == 0 ) ) )   {
	      dtmax = -1.0 * A[1] / 2.0 / A[2];
	      if ( ! ( CubicFailed = ( dtmax < 0 || dtmax > 2 ) ) ) {
		//time = (float(it0) + dtmax) * 25.0 * nanosecond; // nsec
		time=dtmax*25.0*ns;
		for(int ia = 0; ia < 3; ia++)
		  ADCPeak += A[ia] * pow(dtmax, ia);
	      }
	    }
	  
	    // Now use jobOptions to pick time and height of FCAL pulses
	    if( m_FCALmodeTime == "LINEAR" ) {
	      float weightSum = 0.;
	      float timeSum   = 0.;
	      for( int it=0; it<3; it++) {
		timeSum   += float(mySamples[it0+it]) * float(it);
		weightSum += float(mySamples[it0+it]);
	      }
	      time    = (float(it0) + timeSum / weightSum) * 25.0 * nanosecond;
	      // ADCPeak = mySamples[iPeakSamp];
	      ADCPeak = mySamples[m_imaxSamp];
	      CubicFailed = false;
	    }
	  
	    // then, deal with non-FCAL
	  } else if(not isFCAL ||  m_mode != "CUBIC" ) {
	  
	    // bias-corrected parabola extrapolation for selected channels
	    if ( m_mode == "PARABOLA" &&
		 ! ( CubicFailed = maxADCPeak <= m_cubicAdcCut ) ) {
	      if( m_peakParabolaTool) {
		int layer = 0;
		try {
		  const Identifier id = cabling->cnvToIdentifier(chid);
		  ATH_MSG_DEBUG( std::hex << "     id = " << id );
		  if (m_emId->is_em_barrel(id)) {
		    layer= m_emId->sampling(id);
		  }
		}
		catch (LArID_Exception & execpt){
                  ATH_MSG_DEBUG( std::hex
                                 << " Cannot get offline identifier from online ID = " << chid );
		}
	      
		std::vector<float> peak =m_peakParabolaTool->peak(samples,layer,thePedestal); 
		if(peak.size() >1){
		  ADCPeak = peak[0]-thePedestal;
		  if (peak.size()==2)time = peak[1];
		  else time = peak[2];
		}else{
		  ATH_MSG_DEBUG( "No pic is computed from Parabola. Use scaled average of selected samples"  );
		  ADCPeak = averagedADC;
		  time    = -99.;
		}
	      }else{
		ATH_MSG_FATAL( "No parabola tool available ! Choose another mode"  );
	      }
	    }
	  
	    // cubic extrapolation for selected channels
	    else if ( m_mode == "CUBIC" &&
		      ! ( CubicFailed = maxADCPeak <= m_cubicAdcCut ) ) {
	    
	      unsigned int it0;
	      const float invT[4][4] 
		= { {        1,       0,       0,        0},
		    { -1.83333,       3,    -1.5, 0.333333},
		    {        1,    -2.5,       2,     -0.5},
		    {-0.166666,     0.5,    -0.5, 0.166666}  };
	    
	      // peak slice very early
	      if ( iPeakSamp <= 1 ) {
		it0 = m_iPedestal + 1;
	      } else if ( iPeakSamp >= nSamples - 2 ) {  // peak is late
		it0 = nSamples - 4;
	      } else { // peak in safe region
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
	      if ( ! ( CubicFailed = ( disc < 0 || A[3] == 0 ) ) )   {
		dtmax = (-A[2]-sqrt(disc))/(A[3]*3);
		if ( ! ( CubicFailed = ( dtmax < 0 || dtmax > 3 ) ) ) {
		  time = (float(it0) + dtmax) * 25.0 * nanosecond; // nsec
		  for(int ia = 0; ia < 4; ia++)
		    ADCPeak += A[ia] * pow(dtmax, ia);
		}
	      }
	    }
	  }
	}
      
	// fixed time slice or insufficient signal for cubic fit : use scaled average of selected samples  
	if(m_mode == "FIXED" || CubicFailed ) {
	  ADCPeak = averagedADC;
	  time    = -99. ;
	}
      
	ATH_MSG_DEBUG( "Flag: "
                       << CubicFailed
                       << " Detector: "
                       << (isEM?"EM":(isHEC?"HEC":(isFCAL?"FCAL":"none")))
                       << " Mode: "
                       << m_mode
                       << " Signal: "
                       <<  ADCPeak
                       << " Peak: "
                       << maxADCPeak
                       << " PeakSample: "
                       << iPeakSamp
                       );
      
	float energy=-9999;
	if (m_useRampDB) {
	  float ADCPeakPower=ADCPeak;
	  //ADC2MeV (a.k.a. Ramp)   
	  const std::vector<float>& ramp=m_adc2mevTool->ADC2MEV(chid,gain);
	  //Check ramp coefficents
	  if (ramp.size()>0 && ramp[1]<500 && ramp[1]>0) {
	    energy=0;
	    for (unsigned i=1;i<ramp.size();i++)
	      {energy+=ramp[i]*ADCPeakPower; //pow(ADCPeak,i);
	      //std::cout << "Step "<< i <<":" << ramp[i] << " * " << pow(ADCPeak,i) << "Sum=" << energy << std::endl;
	      ADCPeakPower*=ADCPeak;
	      }
	  }
	}
	if (energy==-9999) {
	  ATH_MSG_DEBUG( "No Ramp found for this cell. Use default values"  );
	  //Apply default values
	  // Now must get subdetector ID and feed in here ...
	  float ADCtoMeV      = 10.0;
	
	  //     HWIdentifier FebID = m_onlineHelper->feb_Id(chid);
	  //     unsigned int channel  = m_onlineHelper->channel(chid);
	  //     MSG::hex(log) << MSG::DEBUG
	  //                   << " FebID / chid / channel = " << FebID << " / " << chid << " / " << channel << endmsg;
	
	  try {
	    const Identifier id = cabling->cnvToIdentifier(chid);
	  
	    ATH_MSG_DEBUG( std::hex << "     id = " << id );
	  
	    if (m_emId->is_em_barrel(id)) {
	      const int layer= m_emId->sampling(id);
	      const int eta=m_emId->eta(id);
	      ADCtoMeV = m_ADCtoMeVEMB[layer];
	      if (layer==2 && eta<32)
		ADCtoMeV=ADCtoMeV*(12./18.); //Correct for lead thickness
	      ATH_MSG_DEBUG( " in EMB s="<< layer <<", using ADCtoMeV = " << ADCtoMeV  );
	    } else if (m_emId->is_em_endcap_inner(id)) {
	      // m_emId->sampling(id);
	      ADCtoMeV = m_ADCtoMeVEMECInner[m_emId->sampling(id)-1];
	      ATH_MSG_DEBUG( " in EMEC inner s="<<m_emId->sampling(id)<<", using ADCtoMeV = " << ADCtoMeV  );
	    } else if (m_emId->is_em_endcap_outer(id)) {
	      // m_emId->sampling(id);
	      // ADCtoMeV = m_ADCtoMeVEMECOuter[m_emId->sampling(id)-1];
              ADCtoMeV = m_ADCtoMeVEMECOuter[m_emId->sampling(id)];
	      ATH_MSG_DEBUG( " in EMEC outer s="<<m_emId->sampling(id)<<", using ADCtoMeV = " << ADCtoMeV  );
	    } else if (m_fcalId->is_lar_fcal(id)) {
	      // m_fcalId->module(chid);
	      ADCtoMeV = m_ADCtoMeVFCAL[m_fcalId->module(id)-1];
	      ATH_MSG_DEBUG( " in FCAL m=" << m_fcalId->module(id)<<", using ADCtoMeV = " << ADCtoMeV  );
	    } else if (m_hecId->is_lar_hec(id)) {
	      // m_layer[m_cellIndex]=m_hecId->sampling(id);
	      ADCtoMeV = m_ADCtoMeVHEC[0];
	      ATH_MSG_DEBUG( " in HEC s="<<m_hecId->sampling(id)<<", using ADCtoMeV = " << ADCtoMeV  );
	    }
	  }
	  //catch( LArOnlID_Exception & except) 
	  catch (LArID_Exception & execpt)
	    {
              ATH_MSG_DEBUG( " is disconnected. Set ADCtoMeV to 1" );
              ADCtoMeV=1;
	    }
	  energy  = ADCPeak * ADCtoMeV * GainFactor;
	}
        uint16_t iquality=0;
        uint16_t iprovenance=0;
      
	time*=1000; //Convert to picoseconds
	//Make LArRawChannel Object with new data
	LArRawChannel larRawChannel(chid,(int)energy,(int)time,iquality,iprovenance, gain);   
	larRawChannelContainer->add(larRawChannel); //Add to container
      }
    }// End loop over LArDigits
    if ( iloop == 0 ) {
      // select the best windows for the event
      unsigned int i;
      float tmpSum = 0;

      for(i=0;i<fSumEM.size();i++) {
	if (i == 0 || fSumEM[i] > tmpSum ) {
	  nMinEM = i;
	  tmpSum = fSumEM[i];
	}
      }
      if ( fSumEM.size() > 0 ) {
	ATH_MSG_DEBUG( "Found best EM window starting at sample <" << nMinEM << ">"  );
      }

      for(i=0;i<fSumHEC.size();i++) {
	if (i == 0 || fSumHEC[i] > tmpSum ) {
	  nMinHEC = i;
	  tmpSum = fSumHEC[i];
	}
      }
      if ( fSumHEC.size() > 0 ) {
	ATH_MSG_DEBUG( "Found best HEC window starting at sample <" << nMinHEC << ">"  );
      }

      for(i=0;i<fSumFCAL.size();i++) {
	if (i == 0 || fSumFCAL[i] > tmpSum ) {
	  nMinFCAL = i;
	  tmpSum = fSumFCAL[i];
	}
      }
      if ( fSumFCAL.size() > 0 ) {
	ATH_MSG_DEBUG( "Found best FCAL window starting at sample <" << nMinFCAL << ">"  );
      }

    }
  }// End of double loop over Digit containers

  ATH_MSG_DEBUG( "Finished loop over Digit Container "  );

  // Put this LArRawChannel container in the transient store
  ATH_CHECK( SG::makeHandle (m_ChannelContainerName, ctx).record (std::move (larRawChannelContainer)) );

  return StatusCode::SUCCESS;
}

StatusCode LArRawChannelSimpleBuilder::finalize()
{//Error and Warning Summary for this job:
  
  ATH_MSG_INFO( "LArRawChannelSimpleBuilder finished."  );

  return StatusCode::SUCCESS;
}
