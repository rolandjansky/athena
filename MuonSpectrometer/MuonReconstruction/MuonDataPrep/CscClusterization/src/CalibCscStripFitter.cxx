/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibCscStripFitter.h"
#include "MuonPrepRawData/CscStripPrepData.h"

#include <cmath>

using Muon::CscStripPrepData;

typedef ICscStripFitter::Result Result;
typedef ICscStripFitter::ChargeList ChargeList;

//**********************************************************************

CalibCscStripFitter::CalibCscStripFitter(std::string type, std::string aname, const IInterface* parent) :
    AthAlgTool(type, aname, parent),m_noiseOption(rms),
    m_cscCalibTool("CscCalibTool/CscCalibTool", this) {
  declareInterface<ICscStripFitter>(this);
  declareProperty("timeError", m_terr =5.0);
  declareProperty("failTimeError", m_terr_fail =50.0);
  declareProperty("chargeCalibrationError", m_qerrprop = 0.000);
  declareProperty("cscCalibTool",  m_cscCalibTool);
  declareProperty("noiseOption", m_noiseOptionStr="f001");
  declareProperty("doCorrection", m_doCorrection=true);
  declareProperty("chargeErrorScaler", m_chargeErrorScaler = 1.0);
}

//**********************************************************************

StatusCode CalibCscStripFitter::initialize() {

  ATH_MSG_DEBUG ( "Initializing " << name() );
  
  ATH_MSG_DEBUG ( "Properties for " << name() << ":" );
  ATH_MSG_DEBUG ( "                Time error: "                 << m_terr );
  ATH_MSG_DEBUG ( "           Fail time error: "                 << m_terr_fail );
  ATH_MSG_DEBUG ( "  Charge calibration error: "                 << m_qerrprop );
  ATH_MSG_DEBUG ( "  Charge Error Option is "                    << m_noiseOptionStr);
  ATH_MSG_DEBUG ( "  Charge Error Scaler is "                    << m_chargeErrorScaler);
  ATH_MSG_DEBUG ( "  Correction applied for para to bipolar is " << m_doCorrection);
  
  if ( m_noiseOptionStr != "rms"
       && m_noiseOptionStr != "sigma"
       && m_noiseOptionStr != "f001" ) {
    ATH_MSG_DEBUG (" noiseOption is not among rms/sigma/f001. rms is used for default!!");
    m_noiseOptionStr = "rms";
  }
  if( m_noiseOptionStr == "rms"   )      m_noiseOption = rms;
  else if( m_noiseOptionStr == "sigma" ) m_noiseOption = sigma;
  else if( m_noiseOptionStr == "f001" )  m_noiseOption = f001;

  //  ATH_MSG_DEBUG ( " (failed) Charge error is noise(DB)*1.03588 = 4300 " << m_errorScaler );
  /** CSC calibratin tool for the Condtiions Data base access */
  if ( m_cscCalibTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Can't get handle on CSC calibration tools" );
    return StatusCode::RECOVERABLE;
  }

  // Retrieve the detector descriptor.
  ATH_CHECK(m_idHelperSvc.retrieve());

  return StatusCode::SUCCESS;
}

//**********************************************************************

Result CalibCscStripFitter::fit(const ChargeList& chgs,
                                double period, bool samplingPhase,  
                                Identifier& stripId) const {
  // period ==> frequency  either 50ns or 25ns
  
  Result res;

  // It has been used 4300e- as charge error so far.
  // 3.5 ADC was noise -> 4151.06 e- (as of Mar 10, 2009)
  // From DB, we got it as noise. error scaler is from 4300/3546.03;
  //  double errorScaler = 4300./4151.06; // 1.03588
  
  IdentifierHash stripHash;
  if (m_idHelperSvc->cscIdHelper().get_channel_hash(stripId, stripHash)){ 
    ATH_MSG_WARNING ( "Unable to get CSC striphash id " << " the identifier is " );
    stripId.show();
  }

  int zsec = m_idHelperSvc->cscIdHelper().stationEta(stripId);
  int phisec = m_idHelperSvc->cscIdHelper().stationPhi(stripId);
  int station = m_idHelperSvc->cscIdHelper().stationName(stripId) - 49;

  int sector =  zsec*(2*phisec-station+1);
  
  int wlay = m_idHelperSvc->cscIdHelper().wireLayer(stripId);
  int measphi = m_idHelperSvc->cscIdHelper().measuresPhi(stripId);
  int istrip = m_idHelperSvc->cscIdHelper().strip(stripId);


  double ped = m_cscCalibTool->stripPedestal(stripHash);
  // Get noise information
  double noise =0;
  if (m_noiseOption == rms) {
    noise = m_cscCalibTool->stripRMS(stripHash);
  } else if (m_noiseOption == sigma) {
    noise = m_cscCalibTool->stripNoise(stripHash);
  } else if (m_noiseOption == f001) { // f001 is rawADC +1
    noise = m_cscCalibTool->stripF001(stripHash) - m_cscCalibTool->stripPedestal(stripHash);
    noise /= 3.251;
  }

  //  bool isGood = m_cscCalibTool->isGood(stripHash);
  int stripStatusWord = m_cscCalibTool->stripStatusBit(stripHash);

  bool showDetail = false;
  if (noise <=0.0 ) {
    showDetail = true;
    ATH_MSG_DEBUG (" Noise is zero!! DB needs to be fixed for this channel.  4151.06 (3.5ADC) is assigned.");
    noise = 4151.06;
  }

  noise = m_chargeErrorScaler*noise; 
  
  if (ped < 1.0) {
    showDetail = true;
    ATH_MSG_DEBUG (" Pedestal is zero!! DB needs to be fixed for this channel. 2048*1013 (2048ADC) is assigned.");
    //    ped = 0; // 2048 ADC as a default...
    // unrealistic.... DB should be corrupted....
  }

  if (showDetail) 
    ATH_MSG_DEBUG ("noiseOption " << m_noiseOptionStr << " noise= " << noise << " pedestal= " << ped
                   << " strIdentifier= " << stripId << "  strHash= " << (unsigned int) stripHash
                   << " zsec:phisec:istation  sector " << zsec << ":" << phisec << ":" << station << " " << sector 
                   << " wlay:measphi:istr = " << wlay << " " << measphi << " " << istrip
                   << " Charges: " << chgs << "  (Sample time: " << period << " ns)" );
  
  // Fit with calib tool.
  res.phase = samplingPhase;
  res.dcharge = noise;
  
  if ( m_cscCalibTool->findCharge(period, samplingPhase, chgs, res.charge, res.time) ) { // success
    res.stripStatus = Muon::CscStrStatSuccess;
    res.status += 1;
  } else { 
    res.stripStatus = Muon::CscStrStatParabolaFailed;
    res.status += 2;
  }

  res.charge_beforeBPCorr = res.charge;
  res.time_beforeBPCorr = res.time;
  
  if(samplingPhase==1) // This is bipolar correction purpose and will be subtracted...
    res.time=res.time+25;
      
  if(m_doCorrection) { // m_ denotes global variables...

    bool doPositionCorrection = false;

    ATH_MSG_DEBUG ( "Without any correction  res.time = " << res.time );
    ATH_MSG_DEBUG ( "Without any correction  res.charge = " << res.charge );
    
    double parabolapTime[150];
    for (int i=0; i<150; ++i) {
      parabolapTime[i] = 0.5 +i;
    }
    
    double bipolarpTime[150] = {7.2767,14.6497,14.9118,15.3129,15.5765,15.8889,16.2474,16.6022,16.9897,17.3629,17.7400,18.2157,18.6235,19.0662,19.4730,19.9940,20.4652,20.9776,21.4932,22.0852,22.6554,23.2433,23.8237,24.5097,25.1759,25.8815,26.6378,27.4112,28.2157,29.0271,29.9248,30.7977,31.7510,32.7041,33.7583,34.8513,35.9103,37.1485,38.3861,39.7035,41.0022,42.3003,43.6234,45.0080,46.3782,47.7918,49.1901,50.6580,52.0537,53.4410,54.8608,56.2165,57.5229,58.8066,60.0287,61.2283,62.3879,63.5055,64.5241,65.5107,66.4050,67.3491,68.2172,68.9896,69.8158,70.4611,71.1646,71.7663,72.4042,73.0496,73.5342,74.1307,74.5450,75.0907,75.6212,76.0865,76.8541,77.6080,78.4420,79.2248,80.0880,81.0277,81.9300,82.9188,83.9960,85.0072,86.1661,87.2706,88.4430,89.6940,90.9562,92.2918,93.6533,95.0087,96.3996,97.7745,99.1749,100.6474,102.0441,103.4479,104.8626,106.2086,107.5305,108.8386,110.0599,111.2366,112.4078,113.4710,114.5671,115.5359,116.4890,117.3761,118.1778,119.0282,119.7657,120.4136,121.1364,121.8754,122.4186,123.0246,123.5694,124.1640,124.5444,125.1546,125.5514,126.0263,126.4062,126.8301,127.1727,127.5432,127.7796,128.2254,128.4639,128.7937,129.1810,129.3844,129.6880,129.9609,130.1609,130.4174,130.6324,130.8404,131.0484,131.3148,131.5413,131.6463,131.8371,132.1471,132.1629,132.3846};

    double amplCorrValue[150] ={0.0011,0.0019,0.0028,0.0047,0.0077,0.0111,0.0145,0.0207,0.0261,0.0322,0.0412,0.0491,0.0576,0.0663,0.0749,0.0830,0.0875,0.0912,0.0929,0.0931,0.0920,0.0901,0.0875,0.0841,0.0800,0.0756,0.0711,0.0663,0.0609,0.0557,0.0506,0.0453,0.0402,0.0351,0.0299,0.0260,0.0216,0.0173,0.0141,0.0109,0.0076,0.0049,0.0022,0.0003,-0.0014,-0.0021,-0.0029,-0.0029,-0.0025,-0.0023,-0.0013,0.0004,0.0017,0.0043,0.0072,0.0104,0.0141,0.0181,0.0232,0.0277,0.0338,0.0387,0.0451,0.0514,0.0582,0.0648,0.0720,0.0787,0.0855,0.0924,0.0992,0.1064,0.1125,0.1187,0.1232,0.1079,0.0779,0.0672,0.0622,0.0570,0.0515,0.0463,0.0415,0.0362,0.0314,0.0266,0.0221,0.0181,0.0143,0.0103,0.0073,0.0047,0.0021,0.0003,-0.0010,-0.0024,-0.0027,-0.0030,-0.0026,-0.0022,-0.0012,0.0003,0.0019,0.0045,0.0071,0.0108,0.0141,0.0186,0.0233,0.0285,0.0340,0.0388,0.0451,0.0512,0.0575,0.0645,0.0715,0.0788,0.0856,0.0924,0.0992,0.1064,0.1126,0.1186,0.1236,0.1283,0.1320,0.1346,0.1360,0.1351,0.1318,0.1253,0.1148,0.1030,0.0908,0.0796,0.0702,0.0597,0.0506,0.0424,0.0345,0.0283,0.0220,0.0165,0.0122,0.0082,0.0051,0.0027,0.0013,0.0004};


    unsigned int j;
    for(j=0; j<150; j++) {
      if(res.time < parabolapTime[j]) {
        //with that way we keep the index, from which bin we get values from the tables
        break;
      }
    }

    if ( j>0 && j < 149 ) {  // j should be at least 1 and at most 148..
      double a=0.;
      double b=0.;
      double c=0.;
      
      double a1 = (bipolarpTime[j]-bipolarpTime[j-1]);
      //std::cout << "a1 = " << a1 << std::endl;
      double a2 = (parabolapTime[j-1]+parabolapTime[j]);
      //std::cout << "a2 = " << a2 << std::endl;
      double a3 = ((std::pow(parabolapTime[j+1],2)-std::pow(parabolapTime[j-1],2))*(bipolarpTime[j]-bipolarpTime[j-1])-(bipolarpTime[j+1]-bipolarpTime[j-1])*(std::pow(parabolapTime[j],2)-std::pow(parabolapTime[j-1],2))); 
      //std::cout << "a3 = " << a3 << std::endl;
      double a4 = (parabolapTime[j-1]+parabolapTime[j+1])*((parabolapTime[j-1]-parabolapTime[j+1])*(parabolapTime[j-1]+parabolapTime[j])+std::pow(parabolapTime[j],2)-std::pow(parabolapTime[j-1],2));
      //std::cout << "a4 = " << a4 << std::endl;
      double a5 = std::pow(parabolapTime[j],2)-std::pow(parabolapTime[j-1],2);
      //std::cout << "a5 = " << a5 << std::endl;
      
      //first constant
      a = (a1 + (a2 * a3 / a4)) / a5 ;
      
      //std::cout << "a = " << a << std::endl;
      double b1 = (bipolarpTime[j+1]-bipolarpTime[j-1]);
      double b2 = std::pow(parabolapTime[j],2)-std::pow(parabolapTime[j-1],2);
      double b3 = std::pow(parabolapTime[j+1],2)-std::pow(parabolapTime[j-1],2);
      double b4 = (bipolarpTime[j]-bipolarpTime[j-1]);
      double b5 = (parabolapTime[j-1]+parabolapTime[j+1]);
      double b6 = (parabolapTime[j-1]-parabolapTime[j+1])*(parabolapTime[j-1]+parabolapTime[j])+std::pow(parabolapTime[j],2) - std::pow(parabolapTime[j-1],2);
      //second constant
      b = (b1 * b2 - b3 * b4) / (b5 * b6) ;
      //std::cout << "b = " << b << std::endl;
      
      //third constant
      c = bipolarpTime[j-1] - a * std::pow(parabolapTime[j-1],2) + b * parabolapTime[j-1];
      
      //std::cout << "c = " << c << std::endl;

      double correctedTime = a*res.time*res.time + b*res.time + c;

      if (correctedTime != 0.0)
	{
	  doPositionCorrection = true;

	  if(samplingPhase==0)
	    res.time = correctedTime;
	  if(samplingPhase==1)
	    res.time = correctedTime-25;
	}
      
      ATH_MSG_DEBUG( "After Correction time = " <<  res.time );
    } //if ( j>0 && j < 149 ) {


    if(doPositionCorrection) {

      unsigned int l;
      for(l=0; l<150; l++) {
        if(res.time < parabolapTime[l]) {
          //with that way we keep the index, from which bin we get values from the tables
          break;
        }
      }
      
      if(l>0 && l<149) {
	
        double a_1 = (amplCorrValue[l]-amplCorrValue[l-1]);	    
        double a_2 = (parabolapTime[l-1]+parabolapTime[l]);	    
        double a_3 = ((std::pow(parabolapTime[l+1],2)-std::pow(parabolapTime[l-1],2))*(amplCorrValue[l]-amplCorrValue[l-1])-(amplCorrValue[l+1]-amplCorrValue[l-1])*(std::pow(parabolapTime[l],2)-std::pow(parabolapTime[l-1],2))); 
        double a_4 = (parabolapTime[l-1]+parabolapTime[l+1])*((parabolapTime[l-1]-parabolapTime[l+1])*(parabolapTime[l-1]+parabolapTime[l])+std::pow(parabolapTime[l],2)-std::pow(parabolapTime[l-1],2));
        double a_5 = std::pow(parabolapTime[l],2)-std::pow(parabolapTime[l-1],2);
        //first constant
        double aConst = (a_1 + (a_2 * a_3 / a_4)) / a_5 ;
	
        double b_1 = (amplCorrValue[l+1]-amplCorrValue[l-1]);
        double b_2 = std::pow(parabolapTime[l],2)-std::pow(parabolapTime[l-1],2);
        double b_3 = std::pow(parabolapTime[l+1],2)-std::pow(parabolapTime[l-1],2);
        double b_4 = (amplCorrValue[l]-amplCorrValue[l-1]);
        double b_5 = (parabolapTime[l-1]+parabolapTime[l+1]);
        double b_6 = (parabolapTime[l-1]-parabolapTime[l+1])*(parabolapTime[l-1]+parabolapTime[l])+std::pow(parabolapTime[l],2) - std::pow(parabolapTime[l-1],2);	    
        //second constant
        double bConst = (b_1 * b_2 - b_3 * b_4) / (b_5 * b_6) ;
	
        //third constant
        double cConst = amplCorrValue[l-1] - aConst * std::pow(parabolapTime[l-1],2) + bConst * parabolapTime[l-1];	  
        double fixedValue = aConst*res.time*res.time + bConst*res.time + cConst;
	
        res.charge = res.charge / (1 - fixedValue);
	
        ATH_MSG_DEBUG( "After Correction charge = " <<  res.charge );     
      }
    } //  if(doPositionCorrection) 


    ////////////// TO Correction/////////////////////////
    
    res.time_beforeT0Corr = res.time;

    double t0base  = m_cscCalibTool->stripT0base(stripHash);
    int    t0phase = int(m_cscCalibTool->stripT0phase(stripHash));
    

    res.time = res.time_beforeT0Corr - t0base - 12.5*t0phase;
  
  }

  for (unsigned int i=0; i<chgs.size(); ++i) {
    if (m_cscCalibTool->numberOfElectronsToADCCount(stripHash,int(ped+chgs[i]) ) >= 4090 ) {// ADC unit
      res.stripStatus = Muon::CscStrStatSaturated;
      res.status += 4;
    }
  }

  if (stripStatusWord & 0x1) {
    res.stripStatus = Muon::CscStrStatHot;
    res.status += 8;
  } else if ( (stripStatusWord >>1) & 0x1 ) {
    res.stripStatus = Muon::CscStrStatDead;
    res.status += 16;
  }

  float max = -4090000;
  //unsigned int imax =0;
  for (unsigned int i=0; i<chgs.size(); ++i) {
    if (chgs[i]>max) {
      max =chgs[i];
      //imax = i;
    }
  }


  if (res.stripStatus == Muon::CscStrStatSuccess
      || res.stripStatus == Muon::CscStrStatSaturated) {
    res.timeStatus = Muon::CscTimeSuccess;
    if (res.time < -500) {
      res.timeStatus = Muon::CscTimeEarly;
    } else if (res.time > 500) {
      res.timeStatus = Muon::CscTimeLate;
    }
  } else if (res.stripStatus == Muon::CscStrStatUndefined) {
    res.timeStatus = Muon::CscTimeStatusUndefined;
  } else {
    res.timeStatus = Muon::CscTimeUnavailable;
  }

  
  /*
    Completed later
  
  if (imax <= 1 && chgs[imax+1] <= chgs[imax+2] ) {
    res.stripStatus = Muon::CscStrStatNoBipolarShape;
  }
  */
  
  res.dtime = !res.stripStatus ? m_terr : m_terr_fail;

  // Add an error proportional to the charge.
  double dqprop = m_qerrprop*res.charge;
  res.dcharge = sqrt(res.dcharge*res.dcharge + dqprop*dqprop);

  //  if (res.charge ==0) res.charge =40;
  
  return res;
}


//**********************************************************************
  //  for (int i=1; i<2000; i++){
  //    ATH_MSG_DEBUG ( " ADC counts: " << 1.0*i
  //        << "  Charges:  " << m_cscCalibTool->adcCountToNumberOfElectrons(i) << endl;
  //  }


