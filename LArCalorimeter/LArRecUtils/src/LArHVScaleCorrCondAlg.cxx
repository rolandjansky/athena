/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHVScaleCorrCondAlg.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "LArIdentifier/LArElectrodeID.h"

#include <cmath>
#include <bitset>

using CLHEP::microsecond;
using CLHEP::second;

LArHVScaleCorrCondAlg::LArHVScaleCorrCondAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : 
    AthAlgorithm(name, pSvcLocator ), m_condSvc("CondSvc",name),
    m_lar_on_id(nullptr), 	    
    m_calocell_id(nullptr),	
    m_larem_id(nullptr),
    m_larhec_id(nullptr),
    m_larfcal_id(nullptr),	
    m_electrodeID(nullptr),
    m_calodetdescrmgr(nullptr)
{

  m_deltatupdate = 0;
  m_T0 = 90.371;   // parameter for vdrift

  declareProperty("fixHVCorr",    m_fixHVStrings);
  declareProperty("DeltaTupdate", m_deltatupdate);
  declareProperty("UpdateIfChanged",m_updateIfChanged=true,"Update HV regions for which the HV has acutally changed");
  declareProperty("UndoOnlineHVCorr",m_undoOnlineHVCorr=true,"Undo the HVCorr done online");

}


LArHVScaleCorrCondAlg::~LArHVScaleCorrCondAlg() {
}

StatusCode LArHVScaleCorrCondAlg::initialize() {
  ATH_MSG_DEBUG("LArHVScaleCorrCondAlg initialize() begin");
  
  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // retrieve id helpers
  ATH_CHECK(detStore()->retrieve(m_lar_on_id,"LArOnlineID"));
  ATH_CHECK(detStore()->retrieve(m_calocell_id,"CaloCell_ID"));

  m_larem_id   = m_calocell_id->em_idHelper();
  m_larhec_id   = m_calocell_id->hec_idHelper();
  m_larfcal_id   = m_calocell_id->fcal_idHelper();

  ATH_CHECK(detStore()->retrieve(m_calodetdescrmgr));

  ATH_CHECK(detStore()->retrieve(m_electrodeID));

  ATH_CHECK(this->buildFixHVList());

  //Initialize hash-ranges
  //FIXME: Hardcoded numbers are not ideal
  //But the identifier helpers don't provide them. 

  const unsigned A=1; 
  const unsigned C=0;
  //Offline hash ranges:
  m_hashRanges[A].resize(NHVPARTITION);
  m_hashRanges[C].resize(NHVPARTITION);
  m_hashRanges[C][EMECIW]=std::make_pair<IdentifierHash,IdentifierHash>(0,896);
  m_hashRanges[C][EMECPS]=std::make_pair<IdentifierHash,IdentifierHash>(896,1664);
  m_hashRanges[C][EMECOW]=std::make_pair<IdentifierHash,IdentifierHash>(1664,31872);
  m_hashRanges[C][EMBPS]=std::make_pair<IdentifierHash,IdentifierHash>(31872,35776);
  m_hashRanges[C][EMB]=std::make_pair<IdentifierHash,IdentifierHash>(35776,86656);
  m_hashRanges[A][EMBPS]=std::make_pair<IdentifierHash,IdentifierHash>(86656,90560);
  m_hashRanges[A][EMB]=std::make_pair<IdentifierHash,IdentifierHash>(90560,141440);
  m_hashRanges[A][EMECPS]=std::make_pair<IdentifierHash,IdentifierHash>(141440,142208);
  m_hashRanges[A][EMECOW]=std::make_pair<IdentifierHash,IdentifierHash>(142208,172416);
  m_hashRanges[A][EMECIW]=std::make_pair<IdentifierHash,IdentifierHash>(172416,173312);
  m_hashRanges[C][HEC]=std::make_pair<IdentifierHash,IdentifierHash>(173312,176128);
  m_hashRanges[A][HEC]=std::make_pair<IdentifierHash,IdentifierHash>(176128,178944);
  m_hashRanges[C][FCAL]=std::make_pair<IdentifierHash,IdentifierHash>(178944,180706);
  m_hashRanges[A][FCAL]=std::make_pair<IdentifierHash,IdentifierHash>(180706,182468);

  m_completeRange.push_back(std::make_pair<IdentifierHash,IdentifierHash>(0,182468));


  if (m_updateIfChanged) 
    ATH_MSG_INFO( "Will re-compute HV correction for channels with updated voltage" );
  else {
      if (m_deltatupdate) ATH_MSG_INFO( "Will re-compute HV corrections after " << m_deltatupdate <<" seconds." );
      ATH_MSG_INFO( "Will re-compute HV corrections for each new LumiBlock" );
  }

  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_hvKey.initialize());
  if(m_undoOnlineHVCorr) ATH_CHECK(m_onlineHVScaleCorrKey.initialize());

  ATH_CHECK(m_outputHVScaleCorrKey.initialize());

  // Register write handle
  ATH_CHECK(m_condSvc->regHandle(this, m_outputHVScaleCorrKey)); 

  ATH_MSG_DEBUG("LArHVScaleCorrCondAlg initialize() end");
  return StatusCode::SUCCESS;
}

StatusCode LArHVScaleCorrCondAlg::execute() {
  
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // test validity of write handle
  SG::WriteCondHandle<ILArHVScaleCorr>  writeHandle(m_outputHVScaleCorrKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG( "Found valid " << writeHandle.fullKey() << " handle.");
    return StatusCode::SUCCESS;
  }

  // get DCS HVData
  SG::ReadCondHandle<LArHVData> hvDataHdl(m_hvKey, ctx);
  const LArHVData *hvdata = *hvDataHdl;

  // Online HVScaleCorr (if needed to subtract)
  const ILArHVScaleCorr *onlHVCorr = nullptr;
  if(m_undoOnlineHVCorr) {
     SG::ReadCondHandle<ILArHVScaleCorr> onlHVCorrHdl(m_onlineHVScaleCorrKey, ctx);
     // onlHVCorr = dynamic_cast<const LArHVScaleCorrFlat*>(*onlHVCorrHdl);
     onlHVCorr = *onlHVCorrHdl;
     if(!onlHVCorr) {
         ATH_MSG_ERROR("Do not have online HV corr. conditions object, but asked to undo !!!!");
         return StatusCode::FAILURE;
     }

  } 

  SG::ReadCondHandle<LArOnOffIdMapping> larCablingHdl(m_cablingKey, ctx);
  const LArOnOffIdMapping* cabling=*larCablingHdl;
  if(!cabling) {
     ATH_MSG_ERROR("Could not get LArOnOffIdMapping !!");
     return StatusCode::FAILURE;
  }


  // Define validity of the output cond object
  EventIDRange rangeW;
  if(!hvDataHdl.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << hvDataHdl.key());
    return StatusCode::FAILURE;
  }

  std::vector<float> vScale;
  vScale.resize(182468,(float)1.0);

  ATH_MSG_DEBUG("LArHVScaleCorrCondAlg execute()"); 
  if (m_updateIfChanged) {
    const std::set<Identifier>& updatedCells=hvdata->getUpdatedCells();
    if (updatedCells.size()) {
      const HASHRANGEVEC hashranges=this->cellsIDsToPartition(updatedCells);
      StatusCode sc=this->getScale(hashranges, vScale, hvdata, onlHVCorr, cabling);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( " LArHVScaleCorrCondAlg::LoadCalibration error in getScale" );
	return sc;
      }
    }
    else {
      ATH_MSG_DEBUG("No real voltage change, no update necessary");
      return StatusCode::SUCCESS;
    }
  }//end if updateIfChanges
  else {
    // FIXME: statics are not MT-safe!!!
    static unsigned int timestamp_old = 0; 
    static unsigned int lumiblock_old = 0;
    static unsigned int run_old = 0;

    const unsigned int lumiblock = ctx.eventID().lumi_block();
    const unsigned int run       = ctx.eventID().run_number();
    const unsigned int timestamp = ctx.eventID().time_stamp();
  
    ATH_MSG_DEBUG("run|lbn|timestamp [CURRENT][CACHED] --> [ " 
		  << run << " | " << lumiblock << " | " << timestamp << " ] [ " 
		  << run_old << " | " << lumiblock_old << " | " << timestamp_old << " ] ");
    
    const bool updateAfterDeltaT = ( m_deltatupdate && (timestamp - timestamp_old) >= m_deltatupdate );
    const unsigned int deltat = m_deltatupdate > 0?m_deltatupdate:120;

    if (lumiblock != lumiblock_old || run != run_old || updateAfterDeltaT ) {
      
      timestamp_old = timestamp;    
      lumiblock_old = lumiblock;
      run_old       = run;

      const EventIDBase start{run, EventIDBase::UNDEFEVT, timestamp, 0, lumiblock, EventIDBase::UNDEFNUM};
      const EventIDBase stop{run, EventIDBase::UNDEFEVT, timestamp+deltat, 0, lumiblock+1, EventIDBase::UNDEFNUM};
      const EventIDRange rangeWLB{start, stop};

      rangeW = EventIDRange::intersect(rangeW,rangeWLB);
      const std::set<Identifier>& updatedCells=hvdata->getUpdatedCells();
      if (updatedCells.size()) {
	IChronoStatSvc* chrono;
	if (StatusCode::SUCCESS!=service("ChronoStatSvc" , chrono )) {
	  ATH_MSG_ERROR("cannot find chronostat " );
	  return StatusCode::FAILURE;
	}
	std::string chronoName = "LArHVScaleCorrCondAlg";
	chrono -> chronoStart( chronoName); 
	StatusCode sc=this->getScale(m_completeRange, vScale, hvdata, onlHVCorr,cabling);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR( " LArHVScaleCorrCondAlg::LoadCalibration error in getScale" );
	  return sc;
	}
	chrono -> chronoStop( chronoName );
	ATH_MSG_DEBUG("LArHVScaleCorrCondAlg Chrono stop : delta " 
		      << chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) 
		      * (microsecond / second) << " second ");

      }
    }//end if 
  }//end else m_updateIfChanged

  // and now record output object
  std::unique_ptr<LArHVCorr> hvCorr = std::make_unique<LArHVCorr>(std::move(vScale), cabling, m_calocell_id );
  //LArHVScaleCorrFlat * hvCorr = new LArHVScaleCorrFlat(vScale);
  const EventIDRange crangeW(rangeW);
  if(writeHandle.record(crangeW,hvCorr.release()).isFailure()) {
           ATH_MSG_ERROR("Could not record LArHVCorr object with " << writeHandle.key()
                         << " with EventRange " << crangeW << " into Conditions Store");
           return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << crangeW << " into Conditions Store");
  
  return StatusCode::SUCCESS;
}


StatusCode LArHVScaleCorrCondAlg::finalize()
{
  return StatusCode::SUCCESS;
}


// *** compute global ADC2MeV factor from subfactors *** 

StatusCode LArHVScaleCorrCondAlg::getScale(const HASHRANGEVEC& hashranges, 
					   std::vector<float> &vScale, const LArHVData *hvdata, 
					   const ILArHVScaleCorr *onlHVCorr, const LArOnOffIdMapping* cabling) const {

  
  unsigned nChannelsUpdates=0;

  HASHRANGEVEC::const_iterator rit=hashranges.begin();
  HASHRANGEVEC::const_iterator rit_e=hashranges.end();
  for (;rit!=rit_e;++rit) {
    for (size_t idx=rit->first;idx<rit->second;++idx) {
      const IdentifierHash oflHash(idx);
      float mycorr;
      float mynorm;

      double d=.2;  // nominal gap  in cm
      double nominal=2000.;  // nominal HV  in Volts
      double T=88.;  // temperature  in Kelvin

      bool isbarrelEM=false;
      bool isFCAL1=false;

      unsigned int subdet=99;
      unsigned int layer=99;
      const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(oflHash);
      if (!calodde) {
	ATH_MSG_WARNING( "No DDE found for cell " << oflHash );
        continue;
      }
      const Identifier offid=calodde->identify();
      const float eta_raw = calodde->eta_raw();
      const float phi_raw = calodde->phi_raw();


      // EM calorimeter
      if (m_larem_id->is_lar_em(offid)) {
	// barrel
	if (abs(m_larem_id->barrel_ec(offid))==1) {
	  subdet=0;
	  layer = m_larem_id->sampling(offid);
	  // EMB
	  if (m_larem_id->sampling(offid)>0) {
	    d=0.209;
	    nominal=2000.;
	    T=88.37;
	    isbarrelEM=true;
	  }
	  // EMBPS
	  else {
	    nominal = 2000.;
	    T = 88.37;
	    const int ieta=m_larem_id->eta(offid);
	    if (ieta>=0 && ieta<16)        d = 0.196; //cm
	    else if (ieta>=16 && ieta<32)  d = 0.193; //cm
	    else if (ieta>=32 && ieta<48)  d = 0.2; //cm
	    else  d = 0.190; //cm
	  }
	}
	// endcap
	else {
	  subdet=1;
	  layer = m_larem_id->sampling(offid);
	  // End-Cap pre-sampler
	  if (abs(m_larem_id->barrel_ec(offid))==2 && m_larem_id->sampling(offid)==0){
	    T = 88.65; // temperature remainder Calorimeter
	    nominal = -2000.;
	    d = 0.2; 
	  }
	  //EMEC      
	  else {
	    T = 88.65;
	    float aeta_raw = std::fabs(eta_raw);
	    double Zsamp;
	    if ( m_larem_id->sampling(offid)==1 && m_larem_id->region(offid)>=0 )  Zsamp = 3760.; //mm
	    else if ( m_larem_id->sampling(offid)==2 && m_larem_id->region(offid)<=1 )  Zsamp = 3880.; //mm
	    else if(aeta_raw< 2.) Zsamp=4200.-40.*(aeta_raw-1.5);
	    else Zsamp = 4180. - 160.*(aeta_raw-2.);
	    nominal = EMEC_nominal(aeta_raw);
	    d = EMEC_gap(aeta_raw, Zsamp)*0.1; //cm
	  }//end EMEC
	}//end endcap
      }//end is-em
      // Forward Calorimeter
      else if ( m_larfcal_id->is_lar_fcal(offid)) {
	subdet=3;
	layer = m_larfcal_id->module(offid);
	T = 88.65;
	if (m_larfcal_id->module(offid)==1){ 
	  d =0.0269; //cm
	  nominal = 250.;
	  isFCAL1=true;
	}
	else  if (m_larfcal_id->module(offid)==2){
	  d =0.0376;//cm
	  nominal = 375.;
	}
	else { 
	  d =0.0508;//cm
	  nominal = 500.;
	}
      }
      // Hadronic Calorimeter
      else if ( m_larhec_id->is_lar_hec(offid)) {
	subdet=2;
	layer = m_larhec_id->sampling(offid);
	T = 88.65;
	nominal = 1800.;
	d = 0.196;//cm
      }
      T = T - m_T0;

      const double E_nominal = champ_e(nominal,d);
      std::vector<LArHVData::HV_t> hvlist;
      StatusCode sc = hvdata->getHV(offid,hvlist);

      if (sc.isFailure() || hvlist.size()==0) {
	mycorr=1.;
	mynorm=1.;
	ATH_MSG_WARNING( " HV value no found for cell " << m_larem_id->show_to_string(offid) );
      }

      else {
	// dump values
	bool notfound=false;
	for (unsigned int i=0;i<hvlist.size();i++) {
	  //           log << MSG::DEBUG << " " << hvlist[i].hv;
	  if (hvlist[i].hv<-10000) notfound=true;
	}
	//        log << MSG::DEBUG );

	if (notfound) {
	  ATH_MSG_WARNING( " At least one HV value not found in database for cell " << m_larem_id->show_to_string(offid) );
	}

	mycorr=0.;
	mynorm=0.;
	for (unsigned int i=0;i<hvlist.size();i++) {
	  double E = champ_e(hvlist[i].hv,d);

	  // dont correct if E is very close to E nominal to avoid small glitches
	  if (std::fabs(E_nominal)>1e-3) {
	    const double deviation = std::fabs((E-E_nominal)/E_nominal);
	    if (deviation<1e-3) E = E_nominal;
	  }

// barrel accordion
	  if (isbarrelEM) {
	    const double corr = this->Scale_barrel(hvlist[i].hv)*hvlist[i].weight;
	    mycorr += corr;
	  }
//FCAL module 1
	  else if (isFCAL1) {
	    const double corr = this->Scale_FCAL1(hvlist[i].hv) * hvlist[i].weight;
	    mycorr+=corr;
	  }
// other subdetectors
	  else {
	    const double corr = Respo(E,E_nominal,T)*hvlist[i].weight;
	    mycorr+= corr;
	  }    
	  mynorm += hvlist[i].weight;
	}
      }


      if (mycorr>1e-2) mycorr = mynorm/mycorr;
      else mycorr=1.;

// Add protection against correction factor > 10
      if (mycorr>10.) {
          ATH_MSG_DEBUG("Correction factor > 10, ignore it... for cell " <<  m_larem_id->show_to_string(offid) << " " << mycorr);
          mycorr=1.;
      }   

      for (unsigned int ii=0;ii<m_HVfix.size();ii++) {
	if (subdet == m_HVfix[ii].subdet && layer >= m_HVfix[ii].layer_min && layer <= m_HVfix[ii].layer_max &&
	    eta_raw >= m_HVfix[ii].eta_min && eta_raw < m_HVfix[ii].eta_max &&
	    phi_raw >= m_HVfix[ii].phi_min && phi_raw < m_HVfix[ii].phi_max ) {
	  ATH_MSG_DEBUG("Overwrite correction for cell " <<  m_larem_id->show_to_string(offid) << " " << m_HVfix[ii].corr);
	  mycorr = m_HVfix[ii].corr;
	}
      }
    
      ++nChannelsUpdates;
      if(onlHVCorr) { // undo the online one
	float hvonline = onlHVCorr->HVScaleCorr(cabling->createSignalChannelID(offid));
	if (hvonline>0. && hvonline<100.) mycorr = mycorr/hvonline;
      }
      vScale[idx]=mycorr;
    }// end loop over cells 
  }//end loop over ranges'
  ATH_MSG_DEBUG("(re)computed HV scale corrections for " << nChannelsUpdates << " channels");
  return StatusCode::SUCCESS;  
}

float LArHVScaleCorrCondAlg::champ_e(float hv, float d) const
{
  float e1;
  if (hv < -3000.){ 
    return -1000.;
  }
  else
    e1 = fabs(hv)/(d*1e3);
  if ( e1 < 0.01 ) e1 = 0.01;
  return e1;
}

float LArHVScaleCorrCondAlg::vdrift(float e, float tempe) const
{	    
  const float T = tempe;                
  static const float P[6] = {-0.01481,-0.0075,0.141,12.4,1.627,0.317};
  if ( e < -999.) return 0.;
  float vd = (P[0]*T+1)*( P[2]*e*log(1+ (P[3]/e)) + P[4]*pow(e,P[5])) + P[1]*T; // vdrift formula walcowialk mm/micro_s
  return vd;
}

float LArHVScaleCorrCondAlg::InvCharge(float e) const
// returns 1/charge from recombination effect
{ 
  float q = 1.;
  if ( e > 2.) q=(1.+0.36/e);
  else if (e>1e-6) q =(1.04+0.25/e);
  return q;
}

float LArHVScaleCorrCondAlg::Respo(float e, float e_nominal,float tempe) const
{ 
  if (e < -999.) return 1.;
  if (e < 0.01) return 0;
  if ( e > e_nominal ) return 1;
  float resp = (InvCharge(e_nominal)*vdrift(e,tempe))/(InvCharge(e)*vdrift(e_nominal,tempe));
  return resp;
}

float LArHVScaleCorrCondAlg::t_drift(float e, float e_nominal, float d, float tempe) const
{
  if ( e < -999.) return (d*1e4)/vdrift(e_nominal, tempe) ;
  if (e > e_nominal ) e = e_nominal;
  return (d*1e4)/vdrift(e, tempe); // ns
}

float LArHVScaleCorrCondAlg::EMEC_nominal(const float aeta) const
{
  if ( aeta<1.5 ) return 2500.;
  else if (aeta<1.6) return 2300.;
  else if (aeta<1.8 ) return 2100.;
  else if (aeta < 2.0 ) return 1700.;
  else if (aeta < 2.1 ) return 1500.;
  else if (aeta < 2.3 ) return 1250.;
  else if (aeta < 2.5 ) return 1000.;
  else if (aeta < 2.8 ) return 2300.;
  else return 1800.;
}

float LArHVScaleCorrCondAlg::EMEC_gap(const float aeta, float Zeta) const
{
  float EMECg;
  if (aeta<=2.5 ) EMECg = 0.9 +1.9*(  ( Zeta - 40. )/(10*sinh(aeta)) - 60.)/140.;
  else EMECg = 1.8 + 1.3*(  ( Zeta - 40. )/(10*sinh(aeta)) - 30.)/40.;
  return EMECg;
}

float LArHVScaleCorrCondAlg::Scale_FCAL1(const float hv) const
{
  if (hv<5.0) return 0;
  const float R0=-2.612;
  const float alpha=2.336;
  const float beta=0.079;
  const float scale=R0+alpha*pow(hv,beta);
  return scale;
}
  


float LArHVScaleCorrCondAlg::Scale_barrel(const float hv) const
{
   const float hvref[18]={1.,50.,100.,150.,200.,300.,400.,500.,600.,700.,800.,900.,1000.,1200.,1400.,1600.,1800.,2000.};
   const float hvmax = 1998.;
   const float facteur[18]={0.,0.1209,0.2135,0.2829,0.3390,0.4270,0.4961,0.5556,0.6065,0.6527,0.6906,
			    0.7290,0.7626,0.8224,0.8754,0.9190,0.9606,1.};

// 0 HV, returns 0 response
   if (hv<-999.) {
      return 0;
   }
   else if (hv<hvref[0]) {
      float resp=facteur[0];
      return resp;
   }
// 2000 HV, returns response=1
   if (hv>hvmax) {
      float resp=facteur[17];
      return resp;
   }

// intermediate HV, find values between which to extrapolate
   int i1,i2;
   i1=1;
   for (int i=0;i<18;i++) {
    if (hv<hvref[i]) {
       i1=i-1;
       break;
    }
   }
   i2=i1+1;
   float resp=0.;

// if lowHV>=50 V, logarithmic extrapolation
   if (i1>0) {
     const float b=(log(facteur[i2])-log(facteur[i1]))/(log(hvref[i2])-log(hvref[i1]));
     const float a=log(facteur[i2])-b*log(hvref[i2]);
     resp = exp(a+b*log(hv));
   } 
// if between 0 and 50 V, scales linearly
   else {
     resp=facteur[0]*(hv/hvref[0]);
   }
   //std::cout << " hv,i1,i2,resp " << hv << " " << i1 << " " << i2 << " " << resp << std::endl;
   return resp;
}




// *** Build list of correction to hardcode by jobOptions
StatusCode LArHVScaleCorrCondAlg::buildFixHVList() {

  m_HVfix.clear();
  std::vector<std::string>::const_iterator itrStringID=m_fixHVStrings.begin();
  for (;itrStringID!=m_fixHVStrings.end();++itrStringID) {
    std::string theString=*itrStringID;
    std::stringstream is;
    is << theString << std::endl;
  
    unsigned int iDetector,ilayer_min,ilayer_max;
    float eta_min,eta_max,phi_min,phi_max,corr;
    is >> iDetector >> ilayer_min >> ilayer_max >> eta_min >> eta_max >> phi_min >> phi_max >> corr;

    HVfix_t myfix;
    myfix.subdet = iDetector;
    myfix.layer_min = ilayer_min;
    myfix.layer_max = ilayer_max;
    myfix.eta_min = eta_min;
    myfix.eta_max = eta_max;
    myfix.phi_min = phi_min;
    myfix.phi_max = phi_max;
    myfix.corr = corr;
    m_HVfix.push_back(myfix);
  }

  ATH_MSG_INFO( "  Number of regions with overwritten HV corrections from jobOptions " << m_HVfix.size() );


  return StatusCode::SUCCESS;
}


LArHVScaleCorrCondAlg::HASHRANGEVEC LArHVScaleCorrCondAlg::cellsIDsToPartition(const std::set<Identifier>& cellsIDvec) const {
  HASHRANGEVEC ranges;
  if (cellsIDvec.size()>=m_electrodeID->electrodeHashMax()) {
    ranges.push_back(std::make_pair<IdentifierHash,IdentifierHash>(0,182468));
  }
  else {
    std::bitset<NHVPARTITION> hasPartition[2]; 
    std::set<Identifier>::const_iterator it=cellsIDvec.begin();
    std::set<Identifier>::const_iterator it_e=cellsIDvec.end();
    for (;it!=it_e;++it) {
      const Identifier elId=*it;
      const int zside=m_calocell_id->pos_neg(elId);
      const int sample=m_calocell_id->calo_sample(elId);
      switch (sample) {
         case 1: case 2: case 3: //EMB
	hasPartition[zside].set(EMB);
	break;
      case 0: // EMBPS
	hasPartition[zside].set(EMBPS);
	break;
      case 5: case 6: case 7: //EMEC
	if (m_calocell_id->is_em_endcap_outer(elId))
	  hasPartition[zside].set(EMECOW);
	else
	  hasPartition[zside].set(EMECIW);
	break;
      case 4: //EMECPS
	hasPartition[zside].set(EMECPS);
	break;
      case 8: case 9: case 10: case 11: //HEC
	hasPartition[zside].set(HEC);
	break;
      case 21: case 22: case 23:
	hasPartition[zside].set(FCAL);
	break;
      default: //Ignore
	break;
      }//end switch statement
    }//end loop over IDs
    for (unsigned z=0;z<2;++z) { //Loop over detector sides
      for (unsigned i=0;i<NHVPARTITION;++i) {
	if (hasPartition[z].test(i))  ranges.push_back(m_hashRanges[z][i]);
      } //end loop over partitions
    }// end loop over sides
  }//end else not complete
  return ranges;
}

