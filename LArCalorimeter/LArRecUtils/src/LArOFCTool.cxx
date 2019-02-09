/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArOFCTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "LArElecCalib/LArConditionsException.h"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <cmath>

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "LArCabling/LArCablingLegacyService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

using CLHEP::HepMatrix;
using CLHEP::HepVector;

/////////////////////////////////////////////////////////////////////////////

LArOFCTool::LArOFCTool(const std::string& type, 
		       const std::string& name, 
		       const IInterface* parent) 
  : AthAlgTool(type, name, parent), 
    m_lar_on_id(nullptr),
    m_cablingService(nullptr),
    m_larmcsym("LArMCSymTool"),
    m_calo_id_man(nullptr),
    m_lar_em_id(nullptr),
    m_lar_hec_id(nullptr),
    m_lar_fcal_id(nullptr),
    m_calo_dd_man(nullptr),
    m_keyShape("LArShape"), m_keyOFC("LArOFC"), 
    m_keyNoise("LArNoise"), m_keyPedestal("LArPedestal"),
    m_autocorrTool("LArAutoCorrTotalTool"),
    m_FromDB(false),
    m_MCSym(false), 
    m_Dump(false),
    m_isMC(true),
    m_deltaBunch(1),
    m_cacheValid(false), m_loadAtBegin(false),m_firstSample(0)
{
  declareInterface<ILArOFCTool>(this);

  declareProperty("keyShape",m_keyShape);
  declareProperty("keyOFC",m_keyOFC);
  declareProperty("keyNoise",m_keyNoise);
  declareProperty("keyPedestal",m_keyPedestal);
  declareProperty("FromDatabase",m_FromDB);
  declareProperty("MCSym",m_MCSym);
  declareProperty("Dump",m_Dump);
  declareProperty("IsMC",m_isMC);
  declareProperty("LoadAtBegin",m_loadAtBegin);
  declareProperty("AutoCorrTool",m_autocorrTool);
  declareProperty("UseDelta",m_useDelta=0); // 0 = not use Delta, 1 = only EMECIW/HEC/FCAL, 2 = all , 3 = only EMECIW/HEC/FCAL1+high eta FCAL2-3
  declareProperty("DeltaBunch",m_deltaBunch);
  declareProperty("firstSample",m_firstSample);
  declareProperty("IsSuperCell",m_isSC=false);
  declareProperty("useHighestGainAutoCorr",m_useHighestGainAutoCorr=false);
}

/////////////////////////////////////////////////////////////////////////////

StatusCode LArOFCTool::initialize()
{
  ATH_MSG_DEBUG( "LArOFCTool initialize() begin"  );

  if ( m_isSC ){
    const LArOnline_SuperCellID * ll;
    ATH_CHECK(  detStore()->retrieve(ll,"LArOnline_SuperCellID") );
    m_lar_on_id = (const LArOnlineID_Base*)ll;
  }
  else{
    const LArOnlineID * ll;
    ATH_CHECK( detStore()->retrieve(ll,"LArOnlineID") );
    m_lar_on_id = (const LArOnlineID_Base*)ll;
  }

  //retrieves helpers for LArCalorimeter
  m_calo_id_man  = CaloIdManager::instance();
  if ( m_isSC ) {
  m_lar_em_id   = m_calo_id_man->getEM_SuperCell_ID();
  m_lar_hec_id  = m_calo_id_man->getHEC_SuperCell_ID();
  m_lar_fcal_id = m_calo_id_man->getFCAL_SuperCell_ID();
  }  else  {
  m_lar_em_id   = m_calo_id_man->getEM_ID();
  m_lar_hec_id  = m_calo_id_man->getHEC_ID();
  m_lar_fcal_id = m_calo_id_man->getFCAL_ID();
  }
  IToolSvc* toolSvc = nullptr;
  ATH_CHECK(  service( "ToolSvc",toolSvc ) );
  if ( m_isSC ) {
     ATH_MSG_ERROR("LArOFCTool deprecated, not working for SC");
     return StatusCode::FAILURE;
  } else {
    ToolHandle<LArCablingLegacyService> tool("LArCablingLegacyService");
    ATH_CHECK( tool.retrieve() );
    m_cablingService = (LArCablingBase*)& (*tool);
  }
  if(m_MCSym) {
    ATH_CHECK( m_larmcsym.retrieve() );
  }
  ATH_CHECK( m_autocorrTool.retrieve() );
  
  // retrieve CaloDetDescrManager only for m_delta=3
  if (m_useDelta == 3 ){
    if ( m_isSC ){
      const CaloSuperCellDetDescrManager* cc;
      ATH_CHECK(  detStore()->retrieve(cc) );
      m_calo_dd_man = (const CaloDetDescrManager_Base*) cc;
    }else{
      const CaloDetDescrManager* cc;
      ATH_CHECK(  detStore()->retrieve(cc) );
      m_calo_dd_man = (const CaloDetDescrManager_Base*) cc;
    }
  }  

  // callback functions
  if(m_FromDB)
    {
      // for OFC
      if (StatusCode::SUCCESS==detStore()->regFcn(&ILArOFCTool::LoadCalibration,
						  dynamic_cast<ILArOFCTool*>(this),m_dd_ofc,m_keyOFC)) {
	ATH_MSG_INFO( "Registered callback for key: " <<m_keyOFC  );
      } else {
	ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyOFC  );
      }
    }
  else
    {

      // for Shape
      if (StatusCode::SUCCESS==detStore()->regFcn(&ILArOFCTool::LoadCalibration,
						  dynamic_cast<ILArOFCTool*>(this),m_dd_shape,m_keyShape)) {
	ATH_MSG_INFO( "Registered callback for key: " <<m_keyShape  );
      } else {
	ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyShape  );
      }

      // if running on MC, noise is obtained from LArNoise
      // If running on real data, noise is obtained from LArPedestal
      if(m_isMC){
	// for Noise
	if (StatusCode::SUCCESS==detStore()->regFcn(&ILArOFCTool::LoadCalibration,
						    dynamic_cast<ILArOFCTool*>(this),m_dd_noise,m_keyNoise)) {
	  ATH_MSG_INFO( "Registered callback for key: " << m_keyNoise  );
	} else {
	  ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyNoise  );
	}
      }else{
	// for Pedestal
	if (StatusCode::SUCCESS==detStore()->regFcn(&ILArOFCTool::LoadCalibration,
						    dynamic_cast<ILArOFCTool*>(this),m_dd_pedestal,m_keyPedestal)) {
	  ATH_MSG_INFO( "Registered callback for key: " << m_keyPedestal  );
	} else {
	  ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyPedestal  );
	}
      }
    }
  // force calling first callback function of LArAutoCorrTotalTool, and then callback of LArOFCTool  
  if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrTotalTool::LoadCalibration,dynamic_cast<ILArAutoCorrTotalTool*>(&(*m_autocorrTool)),
					      &ILArOFCTool::LoadCalibration,dynamic_cast<ILArOFCTool*>(this))) {
    ATH_MSG_INFO( "Registered callback for LArOFCTool/LArAutoCorrTotalTool"  );
  } else {
    ATH_MSG_ERROR( "Cannot register testCallback function for LArOFCTool/LArAutoCorrTotalTool"  );
  }


  //
  ATH_MSG_DEBUG( "LArOFCTool initialize() end"  );

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

StatusCode LArOFCTool::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////


StatusCode LArOFCTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_K(keys))
{

  ATH_MSG_DEBUG( "Callback invoked for " << keys.size() << " keys"  );
  
  m_cacheValid = false; 

  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////

// *** compute OFC *** 
StatusCode LArOFCTool::getOFC(StatusCode databaseRetrieved)
{
  ATH_MSG_DEBUG( "in getOFC"  );

  // get HWIdentifier iterator
  std::vector<HWIdentifier>::const_iterator it   =m_lar_on_id->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e =m_lar_on_id->channel_end();
    
  // resize OFC to #(gain) = 3
  unsigned int ngains = ( m_isSC ? 1 : 3 );
  m_OFCa.resize(ngains);
  m_OFCb.resize(ngains);

  int count = 0;
  int count2 = 0;
  // loop over em Identifiers
  ATH_MSG_DEBUG( "start loop over cells in LArOFCTool"  );
  for(;it!=it_e;++it)
  {    
    count ++;
    const HWIdentifier id  = *it;
    unsigned int id32      = id.get_identifier32().get_compact();
    if(!m_cablingService->isOnlineConnected(id)) continue;
    if (m_MCSym) {
      HWIdentifier id2 = m_larmcsym->symOnline(id);
      if (id2 != id ) continue; 
    }
    count2++;
    
    // the OFC are a function of gain
    for(unsigned int igain=0;igain<ngains;igain++)
    {
      if(databaseRetrieved==StatusCode::SUCCESS)
      {
	m_OFCa[igain][id32]=this->computeOFC(OFCA,id,igain,-1.).asVector();
	m_OFCb[igain][id32]=this->computeOFC(OFCB,id,igain,-1.).asVector();
	//-1 : use the default Nminbias already taken for AutoCorrTotal
	//     so it is consistant
      }
      else
      {
	m_OFCa[igain][id32].clear();
	m_OFCb[igain][id32].clear();
	//returns empty vectors
      }
    }     
  }
  ATH_MSG_INFO( "LArOFCTool: number of cells   " << count  );
  ATH_MSG_INFO( "LArOFCTool: number of cells after sym " << count2  );

  m_cacheValid=true;
  return StatusCode::SUCCESS;  
}

/////////////////////////////////////////////////////////////////////////////

ILArOFC::OFCRef_t
LArOFCTool::computeOFC(int aORb, const HWIdentifier& CellID, 
		       int igain, float Nminbias)  const
{

    bool thisChan_useDelta = false;
    m_OFCtmp.clear();

    if (m_useDelta==2) {
      thisChan_useDelta = true; 
    }
    else if (m_useDelta==1) { // only HEC/EMECIW/FCAL
      if (m_lar_on_id->isEMECIW(CellID) || m_lar_on_id->isFCALchannel(CellID) || m_lar_on_id->isHECchannel(CellID)) {
        thisChan_useDelta = true; 
      }
    }
    else if (m_useDelta==3) { // only HEC/EMECIW/FCAL1 and high eta FCAL2-3 
      if (m_lar_on_id->isEMECIW(CellID) ||  m_lar_on_id->isHECchannel(CellID)) {
        thisChan_useDelta = true; 
      }
      else if (m_lar_on_id->isFCALchannel(CellID) ) {       
        if (m_cablingService->isOnlineConnected (CellID)) { 
          Identifier ofl_id = m_cablingService->cnvToIdentifier(CellID);
          const CaloDetDescrElement* dde = m_calo_dd_man->get_element(ofl_id);
          if (!dde) {
            ATH_MSG_ERROR( " dde = 0 , onl_id, ofl_id= "<< CellID << " "<< ofl_id  );
            return (m_OFCtmp);
          }
          CaloCell_ID::CaloSample sampling = dde->getSampling();
          float eta = dde->eta();
          if (sampling==CaloCell_ID::FCAL0){
            thisChan_useDelta = true;
          } else {
            if (fabs(eta)>4.0) {
              thisChan_useDelta = true;
            }
          }    
        }
      }     
    }

  if (m_cablingService->isOnlineConnected(CellID)) 
  {
    if(m_FromDB) 
    {
      if(aORb==OFCA) return m_dd_ofc->OFC_a(CellID,igain);
      if(aORb==OFCB) return m_dd_ofc->OFC_b(CellID,igain);
    }
    else
    {
      //:::::::::::::::::::::::::::::::
      //retrieve the data
      //:::::::::::::::::::::::::::::::
      ILArShape::ShapeRef_t Shape = m_dd_shape->Shape(CellID,igain);
      unsigned int nsamples_shape = Shape.size();
      if(nsamples_shape==0) 
      {
        static std::vector<float> empty;
        return(empty);
      }
      ILArShape::ShapeRef_t ShapeDer = m_dd_shape->ShapeDer(CellID,igain);
      //:::::::::::::::::::::::::::::::

      // get Noise autocorrelation for gain
      int igain_autocorr=igain;
      // to use only Autocorr fro highest gain in optimization: HEC/FCAL=> medium gain    EM=>high gain
      if (m_useHighestGainAutoCorr) {
          if  (m_lar_on_id->isHECchannel(CellID) || m_lar_on_id->isFCALchannel(CellID) ) igain_autocorr=1;
          else igain_autocorr=0;
      }
      
      const std::vector<double> AutoCorr = 
	m_autocorrTool->autoCorrTotal(CellID,igain_autocorr,Nminbias);
      //unsigned int nsamples_AC_OFC=AutoCorr.size()+1;
      unsigned int nsamples_AC_OFC = (1+((int)(sqrt(1+8*AutoCorr.size()))))/2;

      const std::vector<double>& rmsSampl =
        m_autocorrTool->samplRMS(CellID,igain_autocorr,Nminbias);
      unsigned int nsamples2 = rmsSampl.size();
      if (nsamples2 != nsamples_AC_OFC) {
        ATH_MSG_WARNING( " bad size for rmsSampl "  );
        return (m_OFCtmp);  // return empty vector
      }
      //:::::::::::::::::::::::::::::::
      //unsigned int iBeginOfNSamples=findTheNSamples(Shape,
      //						    nsamples_AC_OFC,
      //						    nsamples_shape); 
      unsigned int firstSample = m_firstSample; 
      if(m_lar_on_id->isHECchannel(CellID) && m_firstSample == 0 && nsamples_AC_OFC==4){ 
          firstSample=1; 
      } 
      unsigned int iBeginOfNSamples = firstSample; 
      if(nsamples_AC_OFC + iBeginOfNSamples > nsamples_shape) 
	iBeginOfNSamples=0;      
      //:::::::::::::::::::::::::::::::

      if(m_isMC) {
      }
      else          
      {	 
        float RMSpedestal = m_dd_pedestal->pedestalRMS(CellID,igain);
	if(RMSpedestal>= (1.0+LArElecCalib::ERRORCODE))
	  ;
	else
        {
	  ATH_MSG_WARNING(" PedestalRMS vector empty for "
                          <<CellID<<" at gain "<<igain );
	}	
      }
      //:::::::::::::::::::::::::::::::
      //protection against missing data
      //:::::::::::::::::::::::::::::::
      if(Shape.size()==0 || ShapeDer.size()==0 || AutoCorr.size()==0)
      {
	ATH_MSG_WARNING("Some data are missing -> OFC will be empty for "
                        <<CellID<<" at gain "<<igain );
	return (m_OFCtmp);
	//returns an empty vector
      }
      //:::::::::::::::::::::::::::::::
      unsigned int l,c,i; 
      //:::::::::::::::::::::::::::::::
      //calculations
      //:::::::::::::::::::::::::::::::
      // fill and inverrt AC matrix
      HepMatrix AC(nsamples_AC_OFC,nsamples_AC_OFC),
	        ACinv(nsamples_AC_OFC,nsamples_AC_OFC);    
      for(l=0;l<nsamples_AC_OFC;++l) {  //  l=line c=column      	
	for(c=0;c<nsamples_AC_OFC;++c) {
           if (l==c) {
             AC[l][c]=1.;
           }
           else {
             int i1=std::min(l,c);
             int i2=std::max(l,c);
             int index = i1*nsamples_AC_OFC - i1*(i1+1)/2 -(i1+1) + i2;
             AC[l][c]=AutoCorr[index];
           }
           AC[l][c] = AC[l][c]*rmsSampl[l]*rmsSampl[c];
        }
      }
      int ierr;             
      ACinv=AC.inverse(ierr);
      //:::::::::::::::::::::::::::::::           

      if (!thisChan_useDelta) { // STANDARD CALCULATION

	float ACinv_PS[32];//ACinv_PS
	float ACinv_PSD[32]; //ACinv_PSD
	//Q1 Q2 Q3 DELTA
	float Q1=0.;
	float Q2=0.;
	float Q3=0.;
	
	for(l=0;l<nsamples_AC_OFC;++l)
	  {
	    ACinv_PS[l]=0.; ACinv_PSD[l]=0.;
	    for(c=0;c<nsamples_AC_OFC;++c){
	      ACinv_PS[l]+=ACinv[l][c]*Shape[c+iBeginOfNSamples];
	      ACinv_PSD[l]+=ACinv[l][c]*ShapeDer[c+iBeginOfNSamples];
	    }
	    Q1+=Shape[l+iBeginOfNSamples]*ACinv_PS[l];
	    Q2+=ShapeDer[l+iBeginOfNSamples]*ACinv_PSD[l];
	    Q3+=ShapeDer[l+iBeginOfNSamples]*ACinv_PS[l];
	  } 
	float DELTA=Q1*Q2-Q3*Q3;  
	//:::::::::::::::::::::::::::::::
	if(aORb==OFCA) 
	  //OFCa  
	  for(i=0;i<nsamples_AC_OFC;++i) 
	    m_OFCtmp.push_back( (ACinv_PS[i]*Q2-ACinv_PSD[i]*Q3)/DELTA );
	if(aORb==OFCB)
	  //OFCb  
	  for(i=0;i<nsamples_AC_OFC;++i) 
	    m_OFCtmp.push_back( (ACinv_PS[i]*Q3-ACinv_PSD[i]*Q1)/DELTA ); 
	
	//for debugging only
	if(m_Dump)
	  { 
	    std::cout<<m_lar_em_id
	      ->show_to_string(m_cablingService->cnvToIdentifier(CellID))
		     <<" gain="<<igain<<" Nminbias="<<Nminbias<<std::endl;
	    std::cout<<"Shape: ";
	    for(c=0;c<nsamples_shape;++c)
	      std::cout<<Shape[c]<<" ";
	    std::cout<<std::endl;
	    std::cout<<"ShapeDer: ";
	    for(c=0;c<nsamples_shape;++c)
	      std::cout<<ShapeDer[c]<<" ";
	    std::cout<<std::endl;
	    for(c=0;c<nsamples_AC_OFC;++c)
	      std::cout<<Shape[c+iBeginOfNSamples]<<" ";
	    std::cout<<" <- "<<iBeginOfNSamples<<std::endl;
	    for(i=0;i<nsamples_AC_OFC;++i) std::cout<<ACinv_PS[i]<<" ";
	    std::cout<<std::endl;
	    for(i=0;i<nsamples_AC_OFC;++i) std::cout<<ACinv_PSD[i]<<" ";
	    std::cout<<std::endl;
	    std::cout<<" Q1="<<Q1<<" Q2="<<Q2<<" Q3="<<Q3
		     <<" DELTA="<<DELTA<<std::endl;
            std::cout << " OFCa: ";
	    for(i=0;i<nsamples_AC_OFC;++i) 
	      std::cout<<(ACinv_PS[i]*Q2-ACinv_PSD[i]*Q3)/DELTA<<" ";
	    std::cout<<std::endl;
	  }
      } else { // OPTIMIZATION WRT NOISE AND PEDESTAL SHIFTS
	ATH_MSG_DEBUG( " Computing pulse averages for " 
                       << CellID << " at gain " << igain );

	std::vector<float> averages = getShapeAverages(nsamples_AC_OFC,m_deltaBunch,Shape.asVector(),firstSample);

	// Fill shape, derivative and delta vectors as HepVector
	HepVector gResp(nsamples_AC_OFC);
	HepVector gDerivResp(nsamples_AC_OFC);
	HepVector gDelta(nsamples_AC_OFC);
	for(c=0;c<nsamples_AC_OFC;++c){
	  gResp[c]      = Shape[c+iBeginOfNSamples];
	  gDerivResp[c] = ShapeDer[c+iBeginOfNSamples];
	  gDelta[c]     = averages[c];
	}

	ATH_MSG_DEBUG( " Computing OFC optimized for noise and offsets for " 
                       << CellID << " at gain " << igain );

	HepMatrix isol(3,3); 

	isol[0][0] = (gResp.T()*ACinv*gResp)[0];
	isol[0][1] = (gResp.T()*ACinv*gDerivResp)[0];
	isol[0][2] = (gResp.T()*ACinv*gDelta)[0];
	
	isol[1][0] = (gDerivResp.T()*ACinv*gResp)[0];
	isol[1][1] = (gDerivResp.T()*ACinv*gDerivResp)[0];
	isol[1][2] = (gDerivResp.T()*ACinv*gDelta)[0];
	
	isol[2][0] = (gDelta.T()*ACinv*gResp)[0];
	isol[2][1] = (gDelta.T()*ACinv*gDerivResp)[0];
	isol[2][2] = (gDelta.T()*ACinv*gDelta)[0];
	
	int ifail;
	HepMatrix isolInv = isol.inverse(ifail);
	//if(ifail != 0) {
	  // do something 
	//} 

        //for debugging only
        if(m_Dump)
          {
            std::cout<<m_lar_em_id
              ->show_to_string(m_cablingService->cnvToIdentifier(CellID))
                     <<" gain="<<igain<<" Nminbias="<<Nminbias<<std::endl;
            std::cout<<"Shape: ";
            for(c=0;c<nsamples_shape;++c)
              std::cout<<Shape[c]<<" ";
            std::cout<<std::endl;
            std::cout<<"ShapeDer: ";
            for(c=0;c<nsamples_shape;++c)
              std::cout<<ShapeDer[c]<<" ";
            std::cout<<std::endl;
            std::cout << " Shape for the n samples ";
            for(c=0;c<nsamples_AC_OFC;++c)
              std::cout<<Shape[c+iBeginOfNSamples]<<" ";
            std::cout<<" <- "<<iBeginOfNSamples<<std::endl;
            std::cout << " averages: ";
            for(i=0;i<nsamples_AC_OFC;++i)
              std::cout<<averages[i]<< " ";
            std::cout<<std::endl;
          }


	if (aORb==OFCA) { // OFCa 
	  HepVector Amp(3); 
	  HepVector Ktemp(3);
	  Ktemp[0] = 1.;
	  Ktemp[1] = 0.;
	  Ktemp[2] = 0.;
	  Amp = isolInv*Ktemp;
	  HepVector OFCa(nsamples_AC_OFC);
	  OFCa = Amp[0]*ACinv*gResp + Amp[1]*ACinv*gDerivResp + Amp[2]*ACinv * gDelta;
	  for(i=0;i<nsamples_AC_OFC;++i)  
	    m_OFCtmp.push_back( OFCa[i] );
          if (m_Dump) {
             std::cout << "OFCa: ";
             for(i=0;i<nsamples_AC_OFC;++i) std::cout << OFCa[i] << " ";
             std::cout << std::endl;
          }
	}

	if (aORb==OFCB) { // OFCb
	  HepVector Atau(3);
	  HepVector Ktemp(3);
	  Ktemp[0] = 0.; 
	  Ktemp[1] = -1.;
	  Ktemp[2] = 0.;
	  Atau = isolInv*Ktemp;
	  HepVector OFCb(nsamples_AC_OFC);
	  OFCb = Atau[0]*ACinv*gResp + Atau[1]*ACinv*gDerivResp + Atau[2]*ACinv * gDelta  ;
	  for(i=0;i<nsamples_AC_OFC;++i) 
	    m_OFCtmp.push_back( OFCb[i] );
	}
		
      }

    }
  }
  return (m_OFCtmp);
}

/////////////////////////////////////////////////////////////////////////////

unsigned int LArOFCTool::findTheNSamples(ILArShape::ShapeRef_t Shape,
					 unsigned int nsamples_AC_OFC,
					 unsigned int nsamples_shape) const
{
  unsigned int  i_ShapeMax=0;
  double ShapeMax=0;
  for(unsigned int i=0;i<nsamples_shape;++i)
  {
    double value=Shape[i];
    if(value>ShapeMax) { ShapeMax=value; i_ShapeMax=i; }
    else if(value<0 && i>3) break;//after the peak  
  }
  
  unsigned int tmp=int(nsamples_AC_OFC/2.);
  if(tmp>i_ShapeMax) return 0;
  else               return i_ShapeMax-tmp;
}

/////////////////////////////////////////////////////////////////////////////

// *** retrieve OFC_a for a given cell ***
ILArOFCTool::OFCRef_t LArOFCTool::OFC_a(const HWIdentifier& CellID, 
                                        int gain, float Nminbias)  const
{  
  int thisgain = (m_isSC ? 0 : gain);
  if(!m_cacheValid){
    LArOFCTool* this2 = const_cast<LArOFCTool*>(this); 
    StatusCode  sc = this2->condDataReady();
    sc = this2->getOFC(sc);
    if (sc.isFailure()) 
      {
	ATH_MSG_ERROR( "getOFC failed " );
	throw LArConditionsException("Could not compute in  LArOFCTool::OFC_a");
      }
  }

  HWIdentifier id;
  if (m_MCSym) 
    id = m_larmcsym->symOnline(CellID);
  else 
    id = CellID;
  unsigned int id32 = id.get_identifier32().get_compact();

  if(Nminbias<0)   
    //uses the default value,   
    //the one used to calculate stored OFC   
  { 
    MAP::const_iterator it = (m_OFCa[thisgain]).find(id32) ; 
    if(it == (m_OFCa[thisgain]).end())
    {
      ATH_MSG_ERROR( "Unable to find ID = " << CellID << " in m_OFCa" );
      static std::vector<float> empty; 
      return empty;       
    }
    return (((*it).second));
  } 
  else return (this->computeOFC(OFCA,id,thisgain,Nminbias)); 
}

/////////////////////////////////////////////////////////////////////////////

// *** retrieve OFC_a for a given cell ***
ILArOFCTool::OFCRef_t LArOFCTool::OFC_a(const Identifier& CellID, 
                                        int gain, float Nminbias)  const
{
  HWIdentifier id = m_cablingService->createSignalChannelID(CellID);  
  return this->OFC_a(id, gain, Nminbias);
}

/////////////////////////////////////////////////////////////////////////////

// *** retrieve OFC_b for a given cell ***
ILArOFCTool::OFCRef_t LArOFCTool::OFC_b(const HWIdentifier& CellID, 
                                        int gain, float Nminbias)  const
{

  int thisgain = (m_isSC ? 0 : gain);
  if(!m_cacheValid){
    LArOFCTool* this2 = const_cast<LArOFCTool*>(this); 
    StatusCode  sc = this2->condDataReady();
    sc = this2->getOFC(sc);
    if (sc.isFailure()) 
      {
	ATH_MSG_ERROR( "getOFC failed " );
	throw LArConditionsException("Could not compute in  LArOFCTool::OFC_b");
      }
  }

  HWIdentifier id;
  if (m_MCSym) 
    id = m_larmcsym->symOnline(CellID);
  else 
    id = CellID;
  unsigned int id32 = id.get_identifier32().get_compact();

  if(Nminbias<0)   
    //uses the default value,   
    //the one used to calculate stored OFC   
  {    
    MAP::const_iterator it = (m_OFCb[thisgain]).find(id32) ; 
    if(it == (m_OFCb[thisgain]).end())
    {
      ATH_MSG_ERROR( "Unable to findID = " << CellID << " in m_OFCb" );
      static std::vector<float> empty; 
      return empty; 
    }  
    return (((*it).second));
  } 
  else return (this->computeOFC(OFCB,id,thisgain,Nminbias)); 

}

/////////////////////////////////////////////////////////////////////////////

// *** retrieve OFC_b for a given cell ***
ILArOFCTool::OFCRef_t LArOFCTool::OFC_b(const Identifier& CellID, 
                                        int gain, float Nminbias)  const
{
  HWIdentifier id = m_cablingService->createSignalChannelID(CellID);  
  return this->OFC_b(id, gain, Nminbias);
}


void LArOFCTool::handle(const Incident&) {
     ATH_MSG_DEBUG( "In Incident-handle"  );

     if(!m_cacheValid){
       StatusCode  sc = this->condDataReady();
       sc = this->getOFC(sc);
       if (sc.isFailure()) 
	 {
	   ATH_MSG_ERROR( "getTerms failed " );
	   throw LArConditionsException("Could not getOFC in LArOFCTool::handle ");
	 }
     }
     
}


StatusCode LArOFCTool::condDataReady() 
{
  bool ready=true;
  if(m_FromDB)
    {
      bool valid = m_dd_ofc.isValid();
      ready = valid; 
      // for OFC
      if (valid) {
	ATH_MSG_DEBUG( " LArOFC valid "  );
      } else {
	ATH_MSG_ERROR( " LArOFC not valid "  );
      }


    }
  else
    {

      bool valid = m_dd_shape.isValid();
      ready = ready && valid ; 
      // for Shape
      if (valid)
	{
	  ATH_MSG_DEBUG( " shape valid "  );
	} else {
          ATH_MSG_ERROR( " shape not valid "  );
	}
      
      // if running on MC, noise is obtained from LArNoise
      // If running on real data, noise is obtained from LArPedestal// compute pedestal shifts from 32 sample pulse
      if(m_isMC){
	// for Noise
	valid = m_dd_noise.isValid();

	ready = ready && valid ; 
	if (valid) 
	  {
	    ATH_MSG_INFO( " noise valid "  );
	  } else {
            ATH_MSG_ERROR( " noise not valid "  );
	  }
      }else{
	// for Pedestal
	valid = m_dd_pedestal.isValid();
	ready = ready&&valid;
	if (valid)
	  {
	    ATH_MSG_DEBUG( " Pedestal valid "  );
	  } else {
            ATH_MSG_ERROR( " Pedestal NOT valid "  );
	  }
      }
    }
  // force calling first callback function of LArAutoCorrTotalTool, and then callback of LArOFCTool  
 

  if(ready) return StatusCode::SUCCESS ;
  // else 
  ATH_MSG_ERROR( " condDataReady failed "  );
  return StatusCode::FAILURE;
}

std::vector<float> LArOFCTool::getShapeAverages( const unsigned n_samples, 
						 const unsigned n_deltaBunch, // in unit of 25 ns
						 const std::vector<float>& shape, unsigned int firstSample ) const
{
  std::vector<float> averages(n_samples);
  for (unsigned int i=0;i<n_samples;++i) {
    float sumShape = 0.;
    for (unsigned int j=0;j<shape.size();++j) {
      int k=i-j+firstSample;
      if (k%n_deltaBunch == 0 ) sumShape += shape[j];
    }
    averages[i] = sumShape;
  }
  return averages;
}
