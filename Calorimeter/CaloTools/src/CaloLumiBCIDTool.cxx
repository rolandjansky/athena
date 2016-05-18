/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTools/CaloLumiBCIDTool.h" 
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
//#include "xAODEventInfo/EventID.h"
#include "xAODEventInfo/EventInfo.h"
#include <sys/time.h>

CaloLumiBCIDTool::CaloLumiBCIDTool (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_larmcsym("LArMCSymTool"),
    m_OFCTool("LArOFCTool"),
    m_lumiTool("LuminosityTool"),
    m_bunchCrossingTool("BunchCrossingTool"),
    m_lar_on_id(nullptr),
    m_caloIdMgr(nullptr),
    m_calocell_id(nullptr),
    m_isMC(false),
    m_keyShape("LArShape"), m_keyMinBiasAverage("LArPileupAverage"),m_keyOFC("LArOFC"),
    m_bcidMax(3564),
    m_ncell(0),
    m_bcid(0xFFFF), //Larger than m_bcidmax 
    m_firstSampleEMB(0),
    m_firstSampleEMEC(0),
    m_firstSampleHEC(1),
    m_firstSampleFCAL(0),
    m_cacheValid(false)
{ 
  declareInterface<ICaloLumiBCIDTool>(this);
  declareProperty("LArOFCTool",m_OFCTool,"Tool handle for OFC");
  declareProperty("LumiTool",m_lumiTool,"Tool handle for Luminosity");
  declareProperty("BunchCrossingTool",m_bunchCrossingTool,"Tool handle for bunch crossing tool");
  declareProperty("isMC",m_isMC);
  declareProperty("keyShape",m_keyShape);
  declareProperty("keyMinBiasAverge",m_keyMinBiasAverage);
  declareProperty("keyOFC",m_keyOFC);
  declareProperty("firstSampleEMB",m_firstSampleEMB,"First sample EMB in 4 samples mode");
  declareProperty("firstSampleEMEC",m_firstSampleEMEC,"First sample EMEC in 4 samples mode");
  declareProperty("firstSampleHEC",m_firstSampleHEC,"First sample HEC in 4 samples mode");
  declareProperty("firstSampleFCAL",m_firstSampleFCAL,"First sample FCAL in 4 samples mode");
}
                                                                                
//-----------------------------------------------------------------

CaloLumiBCIDTool::~CaloLumiBCIDTool() {}


//-------------------------------------------------------------------

StatusCode CaloLumiBCIDTool::initialize() {

  msg(MSG::INFO) << " initialize " << endreq;

  IIncidentSvc* incSvc; 
  if (service( "IncidentSvc", incSvc ).isFailure()) {
    msg(MSG::ERROR) << "Unable to get the IncidentSvc" << endreq; 
    return StatusCode::FAILURE;
  }
  long int priority=100;
  incSvc->addListener(this,"BeginEvent",priority);


  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    msg(MSG::ERROR) << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  m_lumiVec.reserve(m_bcidMax);
  m_lumiVec.assign(m_bcidMax,0.0);

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore()->regFcn(&IGeoModelSvc::geoInit,
                          geoModel,
                          &CaloLumiBCIDTool::geoInit,this);
    if(sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return sc;

}

// ------------------------------------------------------------------------------

StatusCode CaloLumiBCIDTool::geoInit(IOVSVC_CALLBACK_ARGS) {

  msg(MSG::INFO) << " geoInit " << endreq;

// callback for Shape

  StatusCode sc=detStore()->regFcn(&ICaloLumiBCIDTool::LoadCalibration,
				   dynamic_cast<ICaloLumiBCIDTool*>(this),
				   m_dd_shape,m_keyShape,true);
  if(sc.isSuccess()){
    msg(MSG::INFO) << "Registered callback for key: "
		   << m_keyShape << endreq;
  } else {
    msg(MSG::ERROR) << "Cannot register callback function for key "
		    << m_keyShape << endreq;
    return sc;
  }

// callback for MinBiasAverage

  sc=detStore()->regFcn(&ICaloLumiBCIDTool::LoadCalibration,
			dynamic_cast<ICaloLumiBCIDTool*>(this),
			m_dd_minbiasAverage,m_keyMinBiasAverage,true);
  if(sc.isSuccess()){
    msg(MSG::INFO) << "Registered callback for key: "
		   << m_keyMinBiasAverage << endreq;
  } else { 
    msg(MSG::ERROR) << "Cannot register callback function for key "
		    << m_keyMinBiasAverage << endreq;
    return sc;
  }

  sc=m_cablingService.retrieve();
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Unable to get CablingService" << endreq;
    return sc;
  }


// get OFC tool and register callback

  if (m_isMC) {
    sc=m_OFCTool.retrieve();
    if (sc.isFailure())   {
      msg(MSG::ERROR) << "Unable to retrieve LArOFCTool" << endreq; 
      return sc;
    } else {
      msg(MSG::DEBUG) << " -- LArOFCTool retrieved" << endreq;
    }


    sc=detStore()->regFcn(&ILArOFCTool::LoadCalibration,&(*m_OFCTool),
			  &ICaloLumiBCIDTool::LoadCalibration,dynamic_cast<ICaloLumiBCIDTool*>(this),true);
    if (sc.isSuccess()) {
      msg(MSG::INFO) << "Registered callbacks for LArOFCTool -> CaloLumiBCIDTool"
		     << endreq;
    } else { 
      msg(MSG::ERROR) << "Cannot register callbacks for LArOFCTool -> CaloLumiBCIDTool"
	<< endreq;
      return sc;
    }

  } else {
    sc=detStore()->regFcn(&ICaloLumiBCIDTool::LoadCalibration,
			  dynamic_cast<ICaloLumiBCIDTool*>(this),
			  m_dd_ofc,m_keyOFC,true);
    if(sc.isSuccess()){
      msg(MSG::INFO) << "Registered callback for key: "
		     << m_keyOFC << endreq;
    } else {
      msg(MSG::ERROR) << "Cannot register callback function for key "
		      << m_keyOFC << endreq;
      return sc;
    }

  }

// get LumiTool
  if (m_isMC) {
    sc=m_bunchCrossingTool.retrieve();
    if (sc.isFailure())   {
      msg(MSG::ERROR) << "Unable to retrieve bunch crossing Tool" << endreq; 
      return sc;
    } else {
      ATH_MSG_DEBUG(" -- bunch crossing Tool retrieved");
    }

  } else {
      sc=m_lumiTool.retrieve();
      if (sc.isFailure())   {
        msg(MSG::ERROR) << "Unable to retrieve Lumi Tool" << endreq;
        return sc;
      } else {
        ATH_MSG_DEBUG(" -- Lumi Tool retrieved");
      }

  }

 //
 sc = m_larmcsym.retrieve();
 if (sc.isFailure()) {
   msg(MSG::ERROR) << "Unable to retrieve LArMCSym tool " << endreq;
      return sc;
  }
  else {
    ATH_MSG_DEBUG(" -- LArMCSmy tool retrieved ");
  }

 sc = detStore()->retrieve(m_lar_on_id,"LArOnlineID");
 if (sc.isFailure()) {
   msg(MSG::ERROR) << "Cannot retrieve LArOnlineID from detector store " << endreq;
   return sc;
 }
  
  sc = detStore()->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve CaloIdMgr in  " << endreq;
    return StatusCode::FAILURE;
  }
  m_calocell_id = m_caloIdMgr->getCaloCell_ID();

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------

void CaloLumiBCIDTool::getListOfCells()
{
  m_ncell = m_calocell_id->calo_cell_hash_max();

  m_symCellIndex.resize(m_ncell,-1);
  m_hwid_sym.reserve(2000);
  m_eshift_sym.reserve(2000);
  std::vector<int> doneCell;
  doneCell.resize(m_ncell,-1);

  int nsym=0;
  for (int i=0;i<m_ncell;i++) {
     IdentifierHash idHash=i;
     Identifier id=m_calocell_id->cell_id(idHash);
     if (m_calocell_id->is_tile(id)) continue;
     // convert cell id to symetric identifier
     HWIdentifier hwid2=m_larmcsym->symOnline(id);
     Identifier id2 = m_cablingService->cnvToIdentifier(hwid2);
     int i2 = (int) (m_calocell_id->calo_cell_hash(id2));
     // we have already processed this hash => just need to associate cell i to the same symetric cell
     if (doneCell[i2]>=0) {
        m_symCellIndex[i]=doneCell[i2];
     }
     // we have not already processed this hash, add an entry for this new symmetric cell
     else {
        doneCell[i2]=nsym;
        m_symCellIndex[i] = nsym;
        m_hwid_sym.push_back(hwid2);
        m_eshift_sym.push_back(0.);
        nsym++;
     }
  }

  //std::cout << " --- number of symmetric cells found " << nsym << std::endl; 

  //for (int i=0;i<m_ncell;i++) {
  //  IdentifierHash idHash=i;
  // Identifier id=m_calocell_id->cell_id(idHash);
  // std::cout << " Calo cell " << m_calocell_id->show_to_string(id) << "   sym index " << m_symCellIndex[i] << " symmetric hwid " << m_lar_on_id->show_to_string(m_hwid_sym[m_symCellIndex[i]]) << std::endl;
  //}


  m_minBias.reserve(2000);
  m_minBias.clear();
  m_minBias.assign(2000,0.0);
  m_first.reserve(2000);
  m_first.clear();
  m_first.assign(2000,0.0);
  m_isOnl.reserve(2000);
  m_isOnl.clear();
  m_isOnl.assign(2000,0.0);

  m_shape.reserve(2000);
  m_OFC.reserve(2000);

  m_shape.clear();
  m_OFC.clear();

  std::vector<float> vec;


  std::vector<HWIdentifier>::iterator it = m_hwid_sym.begin();
  std::vector<HWIdentifier>::iterator it_end = m_hwid_sym.end();
  int index=0;
  unsigned int ns = 32;
  unsigned int nsofc = 32;
  for(;it!=it_end;++it) {
    const HWIdentifier id  = *it;
    m_minBias.push_back(0);
    m_first.push_back(0);
    m_isOnl.push_back(0);
    if(m_cablingService->isOnlineConnected(id)) {
          //  get MinBiasAverage
          float MinBiasAverage = m_dd_minbiasAverage->minBiasAverage(id);
          m_minBias[index]=MinBiasAverage;
          m_isOnl[index]=1;

          unsigned int ifirst= 0;
          if (m_lar_on_id->isEMBchannel(id)) ifirst=m_firstSampleEMB;
          if (m_lar_on_id->isEMECchannel(id)) ifirst=m_firstSampleEMEC;
          if (m_lar_on_id->isHECchannel(id)) ifirst=m_firstSampleHEC;
          if (m_lar_on_id->isFCALchannel(id)) ifirst=m_firstSampleFCAL;
          m_first[index] = ifirst;

          //  get Shape
          ILArShape::ShapeRef_t Shape = m_dd_shape->Shape(id,0);
          ns = Shape.size();
          std::vector<float> s;
          for(unsigned int i = 0 ; i<ns ; i++) s.push_back(Shape[i]);
          m_shape.push_back(s);

          //  get OFC
          ILArOFCTool::OFCRef_t OFC;
          if (m_isMC) OFC = m_OFCTool->OFC_a(id,0) ;
          else  OFC = m_dd_ofc->OFC_a(id,0,0);
          nsofc = OFC.size();
          std::vector<float> sofc;
          for(unsigned int i = 0 ; i<nsofc ; i++) sofc.push_back(OFC[i]);
          m_OFC.push_back(sofc);

    } else {
	  std::vector<float> s;
          for(unsigned int i = 0 ; i<ns ; i++) s.push_back(0);
          m_shape.push_back(s);
          std::vector<float> sofc;
          for(unsigned int i = 0 ; i<nsofc ; i++) sofc.push_back(0);
          m_OFC.push_back(sofc);
    }
    index++;
  }


  return;
}

//-------------------------------------------------------------------------------------

StatusCode CaloLumiBCIDTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_K(keys)) {

  msg(MSG::INFO) << "Callback invoked for " << keys.size() << " keys " << endreq;

  m_cacheValid=false;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------

void CaloLumiBCIDTool::handle(const Incident& inc) {

  if (inc.type()!="BeginEvent") return;

  //std::cout << " in  CaloLumiBCIDTool event handle " << std::endl;
  m_cacheValid=false;
  return;

}


//----------------------------------------------------------------------------------------

StatusCode CaloLumiBCIDTool::computeValues(unsigned int bcid)
{

  if (m_ncell==0) getListOfCells();

// get mu value and convert to luminosity for MC
  float xlumiMC=0.;
  if (m_isMC) {
     const xAOD::EventInfo* eventInfo;
     if (! evtStore()->retrieve(eventInfo).isSuccess() ) {
       msg(MSG::WARNING) << " Event info not found in event store . Pileup offsets computed for 0 lumi " << endreq;
     }
     else {
       xlumiMC = eventInfo->averageInteractionsPerCrossing()*0.158478605;     // convert from mu/bunch to lumi in 10**30 units per bunch
                                                                              // 25ns*Nbcid*71mb*10**30 = 25e-9*3564*71e-27*1e30 = 6.31 Use 1/6.31 = 0.158478605
       //std::cout << " mu and xlumiMC for this bcid " << eventInfo->averageInteractionsPerCrossing() << " " << xlumiMC << std::endl;
     }
  }

  if (bcid==0) {
    const xAOD::EventInfo* eventInfo;
    if (! evtStore()->retrieve(eventInfo).isSuccess() ) {
      msg(MSG::WARNING) << " Event info not found in event store . Pileup offsets computed for bcid=1 " << endreq;
      std::cout << " Event info not found in event store . Pileup offsets computed for bcid=1 " << std::endl;
       bcid=1;
     }
     else {
       //const EventID* myEventID=eventInfo->event_ID();
       bcid = eventInfo->bcid();
     }
  }

  // Calculate Luminosity values ONLY around the places Luminosity will be needed
  accumulateLumi(bcid,xlumiMC);
  //std::cout << " start loop over cells  bcid= " << bcid << std::endl;
  //if (!m_isMC) std::cout << "   lumi for this bcid " << m_lumiTool->lbLuminosityPerBCID(bcid) << std::endl;
  size_t total = m_hwid_sym.size();

  if ( (bcid > 34) && (bcid < m_bcidMax-34)) { // Not close to bcid boundary

  for(int index=0 ; index<(int)total; index++) {
    //const HWIdentifier id  = *it;
    //std::cout << " Identifier " << m_lar_on_id->show_to_string(id) << std::endl;
    float eOFC=0.; 
    if(m_isOnl[index]) {

      //  get MinBiasAverage
      float MinBiasAverage = m_minBias[index];

      if (MinBiasAverage<0.) MinBiasAverage=0.;

      std::vector<float>& ofc = m_OFC[index];
      std::vector<float>& samp = m_shape[index];
      unsigned int nsamples = ofc.size();

      // choise of first sample : i.e sample on the pulse shape to which first OFC sample is applied
      unsigned int ifirst= 0;
      if (nsamples==4) ifirst = m_first[index];


      unsigned int nshapes = samp.size();
      if (nshapes < nsamples) {
//	msg(MSG::ERROR) << " Not enough samples in Shape " << nshapes << "   less than in OFC " << nsamples << endreq;
	return StatusCode::FAILURE;
      }

      unsigned int ishift = ifirst + bcid ; // correct for first sample
      for (unsigned int i=0;i<nsamples;i++) {
          float sumShape=0.;
          //unsigned int ishift = i + ifirst + bcid ; // correct for first sample
          unsigned int k = ishift;
          for (unsigned int j=0;j<nshapes;j++) {
               float lumi = m_lumiVec[k];
               sumShape += samp[j]*lumi;
               k--;
          }
          eOFC += sumShape * (ofc[i] );
          ishift++;
      }
      eOFC = eOFC * MinBiasAverage;
    }     // connected cell
    else {
     //std::cout << " disconnected cell ? " << std::endl;
     eOFC=0.;
    }

    //std::cout << " index, eOFC " << index << " " << eOFC << std::endl;
    m_eshift_sym[index]=eOFC;

  }      // loop over cells


  } else { // I am close to the boundary

  for(int index=0 ; index<(int)total; index++) {
    //const HWIdentifier id  = *it;
    //std::cout << " Identifier " << m_lar_on_id->show_to_string(id) << std::endl;
    float eOFC=0.; 
    if(m_isOnl[index]) {

      //  get MinBiasAverage
      float MinBiasAverage = m_minBias[index];
      //float MinBiasAverage=1.;

      if (MinBiasAverage<0.) MinBiasAverage=0.;

      std::vector<float>& ofc = m_OFC[index];
      std::vector<float>& samp = m_shape[index];
      unsigned int nsamples = ofc.size();

      // choise of first sample : i.e sample on the pulse shape to which first OFC sample is applied
      unsigned int ifirst= 0;
      if (nsamples==4) ifirst = m_first[index];


      unsigned int nshapes = samp.size();
      if (nshapes < nsamples) {
//	msg(MSG::ERROR) << " Not enough samples in Shape " << nshapes << "   less than in OFC " << nsamples << endreq;
	return StatusCode::FAILURE;
      }

      for (unsigned int i=0;i<nsamples;i++) {
          float sumShape=0.;
          unsigned int ishift = i + ifirst ; // correct for first sample
          for (unsigned int j=0;j<nshapes;j++) {
               unsigned int k;
               if ((bcid+ishift)<j) k = m_bcidMax+bcid+ishift-j;
               else if ((bcid+ishift)>=(m_bcidMax+j)) k = ishift-j+bcid-m_bcidMax;
               else k = bcid+ishift-j;
               float lumi = m_lumiVec[k];

               sumShape += samp[j]*lumi;
          }
          eOFC += sumShape* (ofc[i] );
      }
      //std::cout << std::endl;
      eOFC = eOFC * MinBiasAverage;
    }     // connected cell
    else {
     //std::cout << " disconnected cell ? " << std::endl;
     eOFC=0.;
    }

    //std::cout << " index, eOFC " << index << " " << eOFC << std::endl;
    m_eshift_sym[index]=eOFC;

  }      // loop over cells

  } // end of the check bcid boundary

#ifdef DONTDO // some debug code, please, ignore
  std::cout << "corrections for BCID : " << bcid << std::endl;
  for(int index1=0 ; index1<total; index1++) {
	if ( fabsf(1e9*(m_eshift_sym[index1]) ) > 0.001 ){
	std::cout << "cell [" << index1 <<"] = " <<(double)(m_eshift_sym[index1]) 
	<< std::endl;
	}
  }
#endif


  m_cacheValid=true;
  m_bcid = bcid;

  return StatusCode::SUCCESS;
}

//-------------------------------------------------

float CaloLumiBCIDTool::average(const CaloCell* caloCell,unsigned int bcid)
{
  return this->average(caloCell->caloDDE(),bcid);

}

//-------------------------------------------------

float CaloLumiBCIDTool::average(const CaloDetDescrElement* caloDDE,unsigned int bcid)
{
  if (bcid != m_bcid && bcid !=0 ) m_cacheValid=false;
  if (!m_cacheValid) {
    StatusCode sc = this->computeValues(bcid);
    if (sc.isFailure()) return 0.;
  }
  // const Identifier CellID = caloDDE->identify();
  //if (m_calocell_id->is_tile(CellID)) return 0.;

  if (caloDDE->is_tile()) return 0;
  int i2 = (int) caloDDE->calo_hash();//(m_calocell_id->calo_cell_hash(CellID));
  if (i2>=m_ncell) return 0.;
  unsigned int index = m_symCellIndex[i2];
  if (index>=m_eshift_sym.size()) return 0.;
  return m_eshift_sym[index];
}

//-------------------------------------------------

float CaloLumiBCIDTool::average(const Identifier CellID,unsigned int bcid)
{
  if (bcid != m_bcid && bcid !=0 ) m_cacheValid=false;
  if (!m_cacheValid) {
    StatusCode sc = this->computeValues(bcid);
    if (sc.isFailure()) return 0.;
  }

  if (m_calocell_id->is_tile(CellID)) return 0.;  
  int i2 = (int) (m_calocell_id->calo_cell_hash(CellID));
  if (i2>=m_ncell) return 0.;
  unsigned int index = m_symCellIndex[i2];
  if (index>=m_eshift_sym.size()) return 0.;
  return m_eshift_sym[index];
}

void CaloLumiBCIDTool::accumulateLumi(const unsigned int bcid, const float xlumiMC) {

  //m_lumiVec.clear();
  unsigned int keep_samples=32;
  unsigned int keep_ofcsamples=32;
  if ( (bcid > keep_samples+5) && (bcid < m_bcidMax-keep_ofcsamples-5))  {

  unsigned int a=bcid-(keep_samples+4);
  unsigned int b=bcid+(keep_ofcsamples+4);
  for(unsigned int i=a;i<b;i++){
     float lumi=0.0;
     if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
     else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
     m_lumiVec[i]=(lumi);
  }

  } else {

  int a=bcid-(keep_samples+4);
  if ( a < 0 ) a=0;
  unsigned int b=bcid+(keep_ofcsamples+4);
  if ( b >= m_bcidMax ) b=m_bcidMax;
  for(unsigned int i=(unsigned int)a;i<b;i++){
     float lumi=0.0;
     if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
     else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
     m_lumiVec[i]=(lumi);
  }

  for(unsigned int i=0;i<keep_ofcsamples+4;i++){
     float lumi=0.0;
     if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
     else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
     m_lumiVec[i]=(lumi);
  }
  for(unsigned int i=m_bcidMax-keep_samples-5;i<m_bcidMax;i++){
     float lumi=0.0;
     if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
     else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
     m_lumiVec[i]=(lumi);
  }

  }

}
