/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHVCorrTool.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "xAODEventInfo/EventInfo.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"
#include "LArCOOLConditions/LArHVScaleCorrFlat.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "LArIdentifier/LArElectrodeID.h"

#include <cmath>
#include <bitset>

using CLHEP::microsecond;
using CLHEP::second;

LArHVCorrTool::LArHVCorrTool(const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent) 
  : 
    AthAlgTool(type, name, parent),
    m_lar_on_id(nullptr), 	    
    m_calocell_id(nullptr),	
    m_larem_id(nullptr),
    m_larhec_id(nullptr),
    m_larfcal_id(nullptr),	
    m_electrodeID(nullptr),
    m_calodetdescrmgr(nullptr),
    m_cablingService("LArCablingLegacyService"),
    m_hvtool("LArHVToolMC",this)
{
  declareInterface<ILArHVCorrTool>(this);

  m_Tdrift = nullptr;
  m_ownScale = true;
  m_keyOutput = "LArHVScaleCorr";
  m_keyOutputTd = "LArTdrift";
  m_cacheFilled = false;
  m_updateOnLastCallback=true;
  m_doTdrift = true;
  m_deltatupdate = 0;
  m_T0 = 90.371;   // parameter for vdrift
  m_allCallBack=false;

  declareProperty("keyOutput",m_keyOutput="LArHVScaleCorr","Output key for LArHVScaleCorr");
  declareProperty("keyOutputTd",m_keyOutputTd,"Output key for LArTdrift");
  declareProperty("folderName",m_folderName="/LAR/ElecCalibFlat/HVScaleCorr",
		  "Folder to store the CondAttrListCollection containing the HVScale correction");
  declareProperty("HVTool",m_hvtool,"HV tool used");
  declareProperty("doTdrift",m_doTdrift,"Compute drift time");
  declareProperty("allHVCallBack",m_allCallBack,"recompute correction for each HV db callback instead of only once per run/lbn");
  declareProperty("fixHVCorr",    m_fixHVStrings);
  declareProperty("DeltaTupdate", m_deltatupdate);
  declareProperty("UpdateIfChanged",m_updateIfChanged=true,"Update HV regions for which the HV has acutally changed");

}


LArHVCorrTool::~LArHVCorrTool() {
   if (m_ownScale) {
     if (m_Tdrift) delete m_Tdrift;
   }
}

StatusCode LArHVCorrTool::initialize() {
  ATH_MSG_DEBUG("LArHVCorrTool initialize() begin");
  
  ATH_CHECK( detStore()->retrieve(m_lar_on_id,"LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_calocell_id,"CaloCell_ID") );

  m_larem_id   = m_calocell_id->em_idHelper();
  m_larhec_id   = m_calocell_id->hec_idHelper();
  m_larfcal_id   = m_calocell_id->fcal_idHelper();

  ATH_CHECK( m_cablingService.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );
  ATH_CHECK( detStore()->retrieve(m_electrodeID) );
  ATH_CHECK( m_hvtool.retrieve() );

  ATH_CHECK( detStore()->regFcn(&ILArHVTool::LoadCalibration,dynamic_cast<ILArHVTool*>(&(*m_hvtool)),
                                &ILArHVCorrTool::LoadCalibration,dynamic_cast<ILArHVCorrTool*>(this)) );

  ATH_CHECK( this->buildFixHVList() );

  //Initialize hash-ranges
  //FIXME: Hardcoded numbers are not ideal
  //But the identifier helpers don't provide them. 

  const unsigned A=0; //According to LArElectrodeID.h A side =0 C-side=1
  const unsigned C=1;
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

  m_vScale.resize(182468,(float)1.0);

  if (m_updateIfChanged) 
    msg(MSG::INFO) << "Will re-compute HV correction for channels with updated voltage" << endmsg;
  else {
    if (m_allCallBack) 
      msg(MSG::INFO) << "Will re-compute HV corrections for all channels on each callback" << endmsg;
    else {
      if (m_deltatupdate) 
	msg(MSG::INFO) << "Will re-compute HV corrections after " << m_deltatupdate <<" seconds." << endmsg;
      msg(MSG::INFO) << "Will re-compute HV corrections for each new LumiBlock" << endmsg;
    }
  }

  ATH_MSG_DEBUG("LArHVCorrTool initialize() end");
  return StatusCode::SUCCESS;
}

StatusCode LArHVCorrTool::LoadCalibration(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_DEBUG("LArHVCorrTool LoadCalibration()"); 
  m_updateOnLastCallback=false;
  if (m_updateIfChanged) {
    const std::vector<HWIdentifier>& updatedElectrodes=m_hvtool->getUpdatedElectrodes();
    if (updatedElectrodes.size()) {
      const HASHRANGEVEC hashranges=this->electrodeIDsToPartition(updatedElectrodes);
      StatusCode sc=this->getScale(hashranges);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << " LArHVCorrTool::LoadCalibration error in getScale" << endmsg;
	return sc;
      }
    }
    else {
      ATH_MSG_DEBUG("No real voltage change, no update necessary");
      return StatusCode::SUCCESS;
    }
  }//end if updateIfChanges
  else {
    static unsigned int timestamp_old = 0; 
    static unsigned int lumiblock_old = 0;
    static unsigned int run_old = 0;
    const xAOD::EventInfo* eventInfo;
    if (evtStore()->retrieve(eventInfo).isFailure()) {
      msg(MSG::WARNING) << " Cannot access to event info " << endmsg;
      return StatusCode::SUCCESS;
    }
    const unsigned int lumiblock = eventInfo->lumiBlock();
    const unsigned int run       = eventInfo->runNumber();
    const unsigned int timestamp = eventInfo->timeStamp();
  
    ATH_MSG_DEBUG("run|lbn|timestamp [CURRENT][CACHED] --> [ " 
		  << run << " | " << lumiblock << " | " << timestamp << " ] [ " 
		  << run_old << " | " << lumiblock_old << " | " << timestamp_old << " ] ");
    
    const bool updateAfterDeltaT = ( m_deltatupdate && (timestamp - timestamp_old) >= m_deltatupdate );

    if (lumiblock != lumiblock_old || run != run_old || m_allCallBack || updateAfterDeltaT ) {
      
      timestamp_old = timestamp;    
      lumiblock_old = lumiblock;
      run_old       = run;

      const std::vector<HWIdentifier>& updatedElectrodes=m_hvtool->getUpdatedElectrodes();
      if (updatedElectrodes.size()) {
	IChronoStatSvc* chrono;
	if (StatusCode::SUCCESS!=service("ChronoStatSvc" , chrono )) {
	  msg(MSG::ERROR) <<"cannot find chronostat " << std::endl;
	  return StatusCode::FAILURE;
	}
	std::string chronoName = "LArHVCorrTool";
	chrono -> chronoStart( chronoName); 
	StatusCode sc=this->getScale(m_completeRange);
	if (sc.isFailure()) {
	  msg(MSG::ERROR) << " LArHVCorrTool::LoadCalibration error in getScale" << endmsg;
	  return sc;
	}
	chrono -> chronoStop( chronoName );
	ATH_MSG_DEBUG("LArHVCorrTool Chrono stop : delta " 
		      << chrono->chronoDelta (chronoName,IChronoStatSvc::USER ) 
		      * (microsecond / second) << " second ");

      }
    }//end if 
  }//end else m_updateIfChanged
  return StatusCode::SUCCESS;
}


StatusCode LArHVCorrTool::finalize()
{
  return StatusCode::SUCCESS;
}


// *** compute global ADC2MeV factor from subfactors *** 
StatusCode LArHVCorrTool::getScale(const HASHRANGEVEC& hashranges) const {
  
  if (m_doTdrift && m_Tdrift==nullptr) {
    m_Tdrift = new LArTdriftComplete();
    if( (m_Tdrift->setGroupingType("ExtendedSubDetector",msg())).isFailure()) {
      msg(MSG::ERROR) << " cannot setGroupingType " << endmsg;
      return StatusCode::FAILURE;
    }
    if( (m_Tdrift->initialize()).isFailure()) {
      msg(MSG::ERROR) << "cannot initialize tdrift " << endmsg;
      return StatusCode::FAILURE;
    }
  }
  float drift_time = 460.;  // test
  //const size_t nCells=m_vScale.size();

  unsigned nChannelsUpdates=0;

  HASHRANGEVEC::const_iterator rit=hashranges.begin();
  HASHRANGEVEC::const_iterator rit_e=hashranges.end();
  for (;rit!=rit_e;++rit) {
    for (size_t idx=rit->first;idx<rit->second;++idx) {
      const IdentifierHash oflHash(idx);
      float mycorr;
      float mynorm;
      float mySum;
      float myTdrift;

      double d=.2;  // nominal gap  in cm
      double nominal=2000.;  // nominal HV  in Volts
      double T=88.;  // temperature  in Kelvin

      bool isbarrelEM=false;
      bool isFCAL1=false;

      unsigned int subdet=99;
      unsigned int layer=99;
      const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(oflHash);
      if (!calodde) {
	msg(MSG::WARNING) << "No DDE found for cell " << oflHash << endmsg;
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
      std::vector<ILArHVTool::HV_t> hvlist;
      StatusCode sc = m_hvtool->getHV(offid,hvlist);

      if (sc.isFailure() || hvlist.size()==0) {
	mycorr=1.;
	mynorm=1.;
	mySum=0.;
	msg(MSG::WARNING) << " HV value no found for cell " << m_larem_id->show_to_string(offid) << endmsg;
      }

      else {
	// dump values
	bool notfound=false;
	//        if (m_larem_id->is_lar_em(offid))
	//           log << MSG::DEBUG << " EM offlineID,onlineID,HV values " << m_larem_id->barrel_ec(offid) << " " << m_larem_id->show_to_string(offid)
	//            << " " << m_lar_on_id->show_to_string(id);
	//        if (m_larhec_id->is_lar_hec(offid))
	//            log << MSG::DEBUG << " HEC offlineID,onlineID,HV values " << m_larhec_id->show_to_string(offid)
	//            << " " << m_lar_on_id->show_to_string(id);
	//        if (m_larfcal_id->is_lar_fcal(offid))
	//            log << MSG::DEBUG << " FCAL  offlineID,onlineID,HV values " << m_larhec_id->show_to_string(offid)
	//              << " " << m_lar_on_id->show_to_string(id);
	for (unsigned int i=0;i<hvlist.size();i++) {
	  //           log << MSG::DEBUG << " " << hvlist[i].hv;
	  if (hvlist[i].hv<-10000) notfound=true;
	}
	//        log << MSG::DEBUG << endmsg;

	if (notfound) {
	  msg(MSG::WARNING) << " At least one HV value not found in database for cell " << m_larem_id->show_to_string(offid) << endmsg;
	}

	mycorr=0.;
	mySum=0.;
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
	    if (m_doTdrift) mySum+= corr/(drift_time*t_drift(E,E_nominal,d,T)/t_drift(E_nominal,E_nominal,d,T));
	  }
//FCAL module 1
	  else if (isFCAL1) {
	    const double corr = this->Scale_FCAL1(hvlist[i].hv) * hvlist[i].weight;
	    mycorr+=corr;
	    if (m_doTdrift) mySum+=corr/t_drift(E,E_nominal,d,T);
	  }
// other subdetectors
	  else {
	    const double corr = Respo(E,E_nominal,T)*hvlist[i].weight;
	    mycorr+= corr;
	    if (m_doTdrift) mySum+= corr/t_drift(E,E_nominal,d,T);
	  }    
	  mynorm += hvlist[i].weight;
	}
      }

      if(mySum>1e-9) myTdrift=mycorr/mySum;
      else myTdrift = 460.;

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
      m_vScale[idx]=mycorr;
      if (m_doTdrift) {
	const HWIdentifier hwid=m_cablingService->createSignalChannelIDFromHash(oflHash);
	m_Tdrift->set(hwid,myTdrift);
      }
    }// end loop over cells 
  }//end loop over ranges'
  m_cacheFilled=true;
  m_updateOnLastCallback=true;
  ATH_MSG_DEBUG("(re)computed HV scale corrections for " << nChannelsUpdates << " channels");
  return StatusCode::SUCCESS;  
}

float LArHVCorrTool::champ_e(float hv, float d) const
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

float LArHVCorrTool::vdrift(float e, float tempe) const
{	    
  const float T = tempe;                
  static const float P[6] = {-0.01481,-0.0075,0.141,12.4,1.627,0.317};
  if ( e < -999.) return 0.;
  float vd = (P[0]*T+1)*( P[2]*e*log(1+ (P[3]/e)) + P[4]*pow(e,P[5])) + P[1]*T; // vdrift formula walcowialk mm/micro_s
  return vd;
}

float LArHVCorrTool::InvCharge(float e) const
// returns 1/charge from recombination effect
{ 
  float q = 1.;
  if ( e > 2.) q=(1.+0.36/e);
  else if (e>1e-6) q =(1.04+0.25/e);
  return q;
}

float LArHVCorrTool::Respo(float e, float e_nominal,float tempe) const
{ 
  if (e < -999.) return 1.;
  if (e < 0.01) return 0;
  if ( e > e_nominal ) return 1;
  float resp = (InvCharge(e_nominal)*vdrift(e,tempe))/(InvCharge(e)*vdrift(e_nominal,tempe));
  return resp;
}

float LArHVCorrTool::t_drift(float e, float e_nominal, float d, float tempe) const
{
  if ( e < -999.) return (d*1e4)/vdrift(e_nominal, tempe) ;
  if (e > e_nominal ) e = e_nominal;
  return (d*1e4)/vdrift(e, tempe); // ns
}

float LArHVCorrTool::EMEC_nominal(const float aeta) const
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

float LArHVCorrTool::EMEC_gap(const float aeta, float Zeta) const
{
  float EMECg;
  if (aeta<=2.5 ) EMECg = 0.9 +1.9*(  ( Zeta - 40. )/(10*sinh(aeta)) - 60.)/140.;
  else EMECg = 1.8 + 1.3*(  ( Zeta - 40. )/(10*sinh(aeta)) - 30.)/40.;
  return EMECg;
}

float LArHVCorrTool::Scale_FCAL1(const float hv) const
{
  if (hv<5.0) return 0;
  const float R0=-2.612;
  const float alpha=2.336;
  const float beta=0.079;
  const float scale=R0+alpha*pow(hv,beta);
  return scale;
}
  


float LArHVCorrTool::Scale_barrel(const float hv) const
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


// *** retrieve HVCorr for a given cell ***
float LArHVCorrTool::Scale(const HWIdentifier& CellID) const {
  const Identifier id=m_cablingService->cnvToIdentifier(CellID);
  return this->Scale(id);
}

// *** retrieve HVCorr for a given cell ***
float LArHVCorrTool::Scale(const Identifier& CellID) const {
  const IdentifierHash hash=m_calocell_id->calo_cell_hash(CellID);
  return this->Scale(hash);
}


float LArHVCorrTool::Scale(const IdentifierHash& hash) const {
  if (!m_cacheFilled) {
    StatusCode sc=this->getScale(m_completeRange);
    if (sc.isFailure())  {
      msg(MSG::ERROR) << " cannot compute HV correction values ... " << endmsg;
      return 1;
    }
  }
  if (hash>m_vScale.size()) {
    msg(MSG::ERROR) << "Cell outside hash range! hash=" << hash << " range=" << m_vScale.size() << endmsg;
    return 1;
  }
  return m_vScale[hash];
}

StatusCode LArHVCorrTool::record()  {

  if (!m_cacheFilled) {
    StatusCode sc=this->getScale(m_completeRange);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << " cannot compute HV correction values ... " << endmsg;
      return sc;
    }
  }

  const unsigned hashMax=m_lar_on_id->channelHashMax();
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("HVScaleCorr", "blob");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  coral::AttributeList* attrList = new coral::AttributeList(*spec);
  (*attrList)["version"].setValue(0U);
  coral::Blob& blob=(*attrList)["HVScaleCorr"].data<coral::Blob>();
  blob.resize(hashMax*sizeof(float));
  float* pblob=static_cast<float*>(blob.startingAddress());
  //Loop over online hash (to make sure that *every* field of the blob gets filled
  for (unsigned hs=0;hs<hashMax;++hs) {
    float value=1.0;
    if (m_cablingService->isOnlineConnectedFromHash(hs)) {
      const Identifier id=m_cablingService->cnvToIdentifierFromHash(hs);
      ATH_MSG_VERBOSE("Filling value for id " << id.get_identifier32().get_compact());
      value=this->Scale(id);
    }
    pblob[hs]=value;
  }
  coll->add(1,*attrList); //Add as channel 1 to AttrListCollection

  StatusCode sc=detStore()->record(coll,m_folderName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << " cannot record CondAttrListCollection with key " << m_folderName << endmsg;
    delete coll;
    return sc;
  }
 
  LArHVScaleCorrFlat* flatHVScale=new LArHVScaleCorrFlat(coll);
  sc=detStore()->record(flatHVScale,m_keyOutput);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record LArHVScaleCorrFlat with key " << m_keyOutput << endmsg;
    delete flatHVScale;
    return sc;
  }
  

  if (m_doTdrift) {
    sc=detStore()->record(m_Tdrift,m_keyOutputTd);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << " cannot record LArTdrift in detStore " << endmsg;
      return sc;
    }
  }
  m_ownScale = false; //Matters only for m_Tdrift

  if (m_doTdrift) {
    sc=detStore()->symLink(m_Tdrift, (ILArTdrift*)m_Tdrift);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to symlink LArTdrift object" << endmsg;
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}

// *** Build list of correction to hardcode by jobOptions
StatusCode LArHVCorrTool::buildFixHVList() {

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

  msg(MSG::INFO) << "  Number of regions with overwritten HV corrections from jobOptions " << m_HVfix.size() << endmsg;


  return StatusCode::SUCCESS;
}


LArHVCorrTool::HASHRANGEVEC LArHVCorrTool::electrodeIDsToPartition(const std::vector<HWIdentifier>& electrodeIDvec) const {
  HASHRANGEVEC ranges;
  if (electrodeIDvec.size()>=m_electrodeID->electrodeHashMax()) {
    ranges.push_back(std::make_pair<IdentifierHash,IdentifierHash>(0,m_vScale.size()));
  }
  else {
    std::bitset<NHVPARTITION> hasPartition[2]; 
    std::vector<HWIdentifier>::const_iterator it=electrodeIDvec.begin();
    std::vector<HWIdentifier>::const_iterator it_e=electrodeIDvec.end();
    for (;it!=it_e;++it) {
      const HWIdentifier elId=*it;
      const int zside=m_electrodeID->zside(elId);
      const int detector=m_electrodeID->detector(elId);
      switch (detector) {
      case 0: //EMB
	hasPartition[zside].set(EMB);
	break;
      case 1: // EMBPS
	hasPartition[zside].set(EMBPS);
	break;
      case 2: //EMEC
	if (m_electrodeID->hv_eta(elId) < 7)
	  hasPartition[zside].set(EMECOW);
	else
	  hasPartition[zside].set(EMECIW);
	break;
      case 3: //EMECPS
	hasPartition[zside].set(EMECPS);
	break;
      case 4: //HEC
	hasPartition[zside].set(HEC);
	break;
      case 5:
	hasPartition[zside].set(FCAL);
	break;
      default: //Ignore electrodes of purity monitors
	break;
      }//end switch statement
    }//end loop over electrode IDs
    for (unsigned z=0;z<2;++z) { //Loop over detector sides
      for (unsigned i=0;i<NHVPARTITION;++i) {
	if (hasPartition[z].test(i))  ranges.push_back(m_hashRanges[z][i]);
      } //end loop over partitions
    }// end loop over sides
  }//end else not complete
  return ranges;
}

