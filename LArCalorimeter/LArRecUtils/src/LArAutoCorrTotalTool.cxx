/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArAutoCorrTotalTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "LArElecCalib/LArConditionsException.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArCabling/LArCablingLegacyService.h"
#include <cmath>

/////////////////////////////////////////////////////////////////////////////

LArAutoCorrTotalTool::LArAutoCorrTotalTool(const std::string& type, 
					   const std::string& name, 
					   const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_Nminbias(0),
    m_NoPile(0),
    m_MCSym(false),
    m_useMixedOFCOpt(false),
    m_lar_on_id(nullptr),
    m_cablingService(nullptr),
    m_larmcsym("LArMCSymTool"),m_adc2mevTool("LArADC2MeVTool"),
    m_calo_id_man(nullptr),
    m_lar_em_id(nullptr),
    m_lar_hec_id(nullptr),
    m_lar_fcal_id(nullptr),
    m_keyShape("LArShape"), m_keyAutoCorr("LArAutoCorr"),
    m_keyNoise("LArNoise"), m_keyfSampl("LArfSampl"),
    m_keyMinBias("LArMinBias"), m_keyPedestal("LArPedestal"),
    m_isMC(true),
    m_nsamples_AC_OFC(0),
    m_cacheValid(false),m_loadAtBegin(true),m_deltaBunch(1),m_nsamples(5),m_firstSample(0),m_isSC(false)
{
  declareInterface<ILArAutoCorrTotalTool>(this);

  declareProperty("keyShape",m_keyShape);
  declareProperty("keyAutoCorr",m_keyAutoCorr);
  declareProperty("keyNoise",m_keyNoise);
  declareProperty("keyPedestal",m_keyPedestal);
  declareProperty("keyMinBias",m_keyMinBias);
  declareProperty("keyfSampl",m_keyfSampl);
  declareProperty("NMinBias",m_Nminbias);
  declareProperty("MCSym",m_MCSym);
  declareProperty("IsMC",m_isMC);
  declareProperty("LoadAtBegin",m_loadAtBegin);
  declareProperty("ADC2MeVTool",m_adc2mevTool);
  declareProperty("deltaBunch",m_deltaBunch,"Delta between filled bunches in 25 ns units");
  declareProperty("NSamples",m_nsamples,"Max number of samples to use");
  declareProperty("firstSample",m_firstSample,"First sample to use for in-time event on the full pulse shape");
  declareProperty("UseMixedOFCOpt",m_useMixedOFCOpt);
  declareProperty("IsSuperCell",m_isSC);
}

/////////////////////////////////////////////////////////////////////////////

StatusCode LArAutoCorrTotalTool::initialize()
{
  ATH_MSG_DEBUG( "LArAutoCorrTotalTool initialize() begin"  );

  if ( !m_isSC ) {

    const LArOnlineID* laron = nullptr;
    ATH_CHECK(  detStore()->retrieve(laron,"LArOnlineID") );
    m_lar_on_id = (LArOnlineID_Base*) laron;

    ToolHandle<LArCablingLegacyService> larcab("LArCablingLegacyService");
    ATH_CHECK( larcab.retrieve() );
    m_cablingService = (LArCablingBase*) &(*larcab);

  } else {

     ATH_MSG_ERROR("LArAutoCorrTotalTool deprecated, not working for SC");
     return StatusCode::FAILURE;
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
  


  if (m_MCSym) {
    ATH_CHECK( m_larmcsym.retrieve() );
  }
  
  ATH_CHECK( m_adc2mevTool.retrieve() );
  ATH_MSG_DEBUG( " -- ILArADC2MeVTool retrieved"  );
  
  
  if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrTotalTool::LoadCalibration,
					      dynamic_cast<ILArAutoCorrTotalTool*>(this),m_dd_shape,m_keyShape)) {
    ATH_MSG_INFO( "Registered callback for key: " <<m_keyShape  );
  } else {
    ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyShape  );
  }
  
  if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrTotalTool::LoadCalibration,
					      dynamic_cast<ILArAutoCorrTotalTool*>(this),m_dd_autocorr,m_keyAutoCorr)) {
    ATH_MSG_INFO( "Registered callback for key: " << m_keyAutoCorr  );
  } else {
    ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyAutoCorr  );
  }
  
  m_NoPile=false;
  if(m_Nminbias<=0) m_NoPile=true;
  
  if(!m_NoPile)
    {
      if(m_isMC){
	if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrTotalTool::LoadCalibration,
						    dynamic_cast<ILArAutoCorrTotalTool*>(this),m_dd_noise,m_keyNoise)) {
	  ATH_MSG_INFO( "Registered callback for key: " << m_keyNoise  );
	} else {
	  ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyNoise  );
	}
      }else{
	if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrTotalTool::LoadCalibration,
						    dynamic_cast<ILArAutoCorrTotalTool*>(this),m_dd_pedestal,m_keyPedestal)) {
	  ATH_MSG_INFO( "Registered callback for key: " << m_keyPedestal  );
	} else {
	  ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyPedestal  );
	}
      }
      
      if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrTotalTool::LoadCalibration,
						  dynamic_cast<ILArAutoCorrTotalTool*>(this),m_dd_fSampl,m_keyfSampl)) {
	ATH_MSG_INFO( "Registered callback for key: " << m_keyfSampl  );
      } else {
	ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyfSampl  );
      }
      
      if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrTotalTool::LoadCalibration,
						  dynamic_cast<ILArAutoCorrTotalTool*>(this),m_dd_minbias,m_keyMinBias)) {
	ATH_MSG_INFO( "Registered callback for key: " << m_keyMinBias  );
      } else {
	ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyMinBias  );
      }
      
      // force calling first callback function of LArADC2MeV, and then callback of LArAutoCorrTotalTool
      if (StatusCode::SUCCESS==detStore()->regFcn(&ILArADC2MeVTool::LoadCalibration,&(*m_adc2mevTool),
						  &ILArAutoCorrTotalTool::LoadCalibration,dynamic_cast<ILArAutoCorrTotalTool*>(this))) {
	ATH_MSG_INFO( "Registered callback for LArAutoCorrTool/LArADC2MeVTool"  );
      } else {
	ATH_MSG_ERROR( "Cannot register testCallback function for LArAutoCorrTotalTool/LArAdc2MeVTool"  );
      }
      
    }
  
  
  if (m_loadAtBegin) {
    ATH_MSG_DEBUG( "Setting callback function to load calibration at begin of run"  );
    // Incident Service: 
    IIncidentSvc* incSvc = nullptr;
    ATH_CHECK(  service("IncidentSvc", incSvc) );
    
    //start listening to "BeginRun". The incident should be fired AFTER the IOV callbacks and only once.
    //const long priority=std::numeric_limits<long>::min(); //Very low priority
    // FIXME.. somehow this class can not be a listener.
    // incSvc->addListener(this, "BeginRun", priority ,false,true); //single-shot incident
  }
    
  //
  ATH_MSG_DEBUG( "LArAutoCorrTotalTool initialize() end"  );
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

StatusCode LArAutoCorrTotalTool::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

// *** retrieves needed data from the DB *** 
StatusCode LArAutoCorrTotalTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_K(keys))
{
  ATH_MSG_DEBUG( "in LoadCalibration "  );
  
  ATH_MSG_DEBUG( "Callback invoked for " << keys.size() << " keys"  );
  
  m_cacheValid = false; 

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

// *** compute some terms of the calculation of the autocorr function *** 
StatusCode LArAutoCorrTotalTool::getTerms()
{
  ATH_MSG_DEBUG( "in getAutoCorrTotal"  );
  ATH_MSG_INFO( "  Bunch spacing (25 ns units ) " << m_deltaBunch  );
  ATH_MSG_INFO( "  N(MB)/bunch crossing " << m_Nminbias  );

  // get HWIdentifier iterator
  std::vector<HWIdentifier>::const_iterator it   =m_lar_on_id->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e =m_lar_on_id->channel_end();
  int ngains = (m_isSC ? 1 : 3 );
  
  // resize vector to #(gain) = 3
  m_terms.resize(ngains);
  
  int count = 0;
  int count2 = 0;
  // loop over em Identifiers
  ATH_MSG_DEBUG( "start loop over cells in getAutoCorrTotal"  );
  for(;it!=it_e;++it)
  {    
    count ++;
    const HWIdentifier id  = *it;
    unsigned int id32      = id.get_identifier32().get_compact();

    if(m_cablingService->isOnlineConnected(id)) 
    {    
      if(m_MCSym) {
        HWIdentifier id2 = m_larmcsym->symOnline(id);
        if (id2 != id) continue;
      }
      count2++;

      // mixed OFC optimization: force no pileup in EMB and EMEC-OW
      if (m_useMixedOFCOpt) {
	const bool isEMB    = m_lar_on_id->isEMBchannel(id);
	const bool isEMECOW = m_lar_on_id->isEMECOW(id);
	if (isEMB || isEMECOW ) {
	  ATH_MSG_DEBUG( "No Pileup AutoCorr for ChID 0x" << MSG::hex << id << MSG::dec  );
	  m_NoPile = true;
	} else { 
	  ATH_MSG_DEBUG( "Using Pileup AutoCorr for ChID 0x" << MSG::hex << id << MSG::dec  );
	  m_NoPile = false;
	}
      }


      static std::vector<float> empty; 
      // the Shape is a function of gain
      for(int igain=0;igain<ngains;igain++)
      {    
	//:::::::::::::::::::::::::::::::
	ILArShape::ShapeRef_t Shape = m_dd_shape->Shape(id,igain); 
	int nsamples_shape=(int)(Shape.size());
        if (nsamples_shape==0) 
	{
	  m_terms[igain][id32] = empty;       
	  continue;
        }

	ILArAutoCorr::AutoCorrRef_t AC = m_dd_autocorr->autoCorr(id,igain);
	if (AC.size()==0) {
	  m_terms[igain][id32] = empty;       
	  continue;
        }


	m_nsamples_AC_OFC=AC.size()+1;

        if (m_nsamples_AC_OFC > m_nsamples) m_nsamples_AC_OFC=m_nsamples;

        // fix HEC first sample +1 if the firstSample is 0 and nsamples 4
        unsigned int ihecshift=0;
        if(m_lar_on_id->isHECchannel(id) && m_nsamples_AC_OFC == 4 && m_firstSample == 0) {
             ihecshift=1;
             //ATH_MSG_DEBUG( "Using firstSample +1 for HEC ChID 0x" << MSG::hex << id << MSG::dec  );
        }

	//:::::::::::::::::::::::::::::::
	//NB: 
	// nsamples_shape    = number of samples of the Shape function (e.g 32)
	// m_nsamples_AC_OFC = size of AC matrix & OFC vector (e.g 5 in Atlas)
	//:::::::::::::::::::::::::::::::
        float fSigma2=0.;
        if(!m_NoPile) 
	{
	  float SigmaNoise;
	  if(m_isMC)
	    SigmaNoise = m_dd_noise->noise(id,igain);
	  else          
	  {
	   float RMSpedestal = 
	     m_dd_pedestal->pedestalRMS(id,igain);
	   if(RMSpedestal>(1.0+LArElecCalib::ERRORCODE)) 
	     SigmaNoise = RMSpedestal;
	   else
	     SigmaNoise = 0.;//(we will have the ERROR message below)
	  }
	  float fSampl     = m_dd_fSampl->FSAMPL(id);
	  float MinBiasRMS = m_dd_minbias->minBiasRMS(id);
	  if(fSampl!=0) MinBiasRMS/=fSampl;
	  const std::vector<float> *
	    polynom_adc2mev = &(m_adc2mevTool->ADC2MEV(id,igain));
          float Adc2MeV=0.;
          if (polynom_adc2mev->size()>0) {
	   Adc2MeV=(*polynom_adc2mev)[1];
          } 
	  if(SigmaNoise!=0 && Adc2MeV!=0) 
	    fSigma2 = pow(MinBiasRMS/(SigmaNoise*Adc2MeV),2);

	  if(fSampl==0 || SigmaNoise==0 || Adc2MeV==0) 	 
	  {
            if (m_isMC) {
              ATH_MSG_ERROR( m_lar_em_id
                             ->show_to_string(m_cablingService->cnvToIdentifier(id))
                             << "fSampl ("<<fSampl<<"), SigmaNoise ("
                             <<SigmaNoise<<") or Adc2MeV ("<<Adc2MeV<<") null "
                             <<"=> AutoCorrTotal = only AutoCorr elect. part " 
                             );
            }
	    fSigma2=0.;
	  }
	  //warning: MinBiasRMS is in MeV (at the scale of the hits)
	  //         SigmaNoise is in ADC counts
	  //  so MinBiasRMS/fScale and SigmaNoise*Adc2MeV are the same scale 
	  //  (MeV at the scale of the cells)
	}//end if m_NoPile

        // get in vTerms all the possible non trivial N(N-1)/2 terms of the autocorrelation matrix
        int nsize_tot = (m_nsamples_AC_OFC-1)*(m_nsamples_AC_OFC)/2;

	std::vector<float> vTerms;   

        vTerms.resize(2*nsize_tot+m_nsamples_AC_OFC,0.);
	//:::::::::::::::::::::::::::::::

        for (int j1=0;j1<m_nsamples_AC_OFC-1;j1++) {
          for (int j2=j1+1;j2<m_nsamples_AC_OFC;j2++) {
            int l=abs(j2-j1)-1;
            int index = j1*m_nsamples_AC_OFC- j1*(j1+1)/2 + j2 - (j1+1);
            vTerms[index] = AC[l];
          }
        }

	//2nd terms : 
	for(int j1=0;j1<m_nsamples_AC_OFC-1;++j1) {
          for (int j2=j1+1;j2<m_nsamples_AC_OFC;j2++) {
            int index = j1*m_nsamples_AC_OFC- j1*(j1+1)/2 + j2 - (j1+1);
	    float Rij=0;
	    for(int k=0;k<nsamples_shape;++k)	 {
              if ((j2-j1+k)>=0 && (j2-j1+k)< nsamples_shape) {
                int ibunch=0;
                if ((j1+m_firstSample+ihecshift-k)%m_deltaBunch == 0) ibunch=1;
	        Rij += Shape[k] * Shape[j2-j1+k] *ibunch;
              }
	    }
	    vTerms[nsize_tot+index]= fSigma2*Rij;
          }
	}      

	//3rd term : RMS of pileup per samples (multiplied by fSigma2)
        for (int j1=0;j1<m_nsamples_AC_OFC;j1++) {
  	   float Rms2i=0;
	   for(int k=0;k<nsamples_shape;++k) {
              int ibunch=0;
              if ((j1+m_firstSample+ihecshift-k)%m_deltaBunch == 0) ibunch=1;
	      Rms2i += pow(Shape[k],2) *ibunch;
           }
	   vTerms[2*nsize_tot+j1]=fSigma2*Rms2i;
        }

	//storage
	m_terms[igain][id32] = vTerms;
	
      }//(loop on gains)
      
    } 
    else//unconnected
      for(unsigned int igain=0;igain<3;igain++) 
      {
        unsigned nsize_tot=(m_nsamples-1)*(m_nsamples)+m_nsamples;
	m_terms[igain][id32] = std::vector<float>(nsize_tot, 0.); 
      }

  }
  ATH_MSG_INFO( "LArAutoCorrTotal Ncell " << count  );
  ATH_MSG_INFO( "LArAutoCorrTotal Nsymcell " << count2  );
  ATH_MSG_DEBUG( "end of loop over cells "  );

  m_cacheValid = true;
  return StatusCode::SUCCESS;
}
 
/////////////////////////////////////////////////////////////////////////////

// *** compute AutoCorrTotal (nsamples-1 coeffs) for a given cell ***
const std::vector<double> 
LArAutoCorrTotalTool::computeAutoCorr(const std::vector<float>& terms,
				      float Nminbias) const
{


  /*
              Rij (elec) +  Nminbias*fSigma^2 * sum(k=0->32or800)[G(i-k)G(j-k)]
Rij(total) = ------------------------------------------------------------------
                  1      +  Nminbias*fSigma^2 * sum(k=0->32or800)[G(k)^2]

  with: o fSigma = Sigma(E minimum-bias) / Sigma(electronic noise)
        o G the Shape function
        o Nminbias = number of minimum-bias events per bunch-crossing
	          (= L/Lo)

  if nsamples==5, we need only R01, R02, R03, R04 (for example)  (ROO=1)
 
  */

  if(Nminbias<0) Nminbias=m_Nminbias;// takes the value in the property

  std::vector<double> vResult;
  int tsize = int(sqrt(terms.size()));
  int nsize_tot = (tsize-1)*(tsize)/2;
  for (int i1=0;i1<tsize-1;i1++)  {
   for (int i2=i1+1;i2<tsize;i2++) {

    int index = i1*tsize - i1*(i1+1)/2 + i2 - (i1+1); 

    vResult.push_back(   (terms[index]+Nminbias*terms[nsize_tot+index])
	               / sqrt((1.+Nminbias*terms[2*nsize_tot+i1])*(1.+Nminbias*terms[2*nsize_tot+i2])) );
   }
  }
  return (vResult);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

const std::vector<double>
LArAutoCorrTotalTool::computeRMS(const std::vector<float>& terms,
                                 float Nminbias) const
{

  if (Nminbias<0) Nminbias = m_Nminbias;

  std::vector<double> vResult;
  int tsize = int(sqrt(terms.size()));
  vResult.reserve(tsize);
  int nsize_tot =  (tsize-1)*(tsize)/2;
  for (int i=0;i<tsize;i++) {
     vResult.push_back(sqrt(1.+Nminbias*terms[2*nsize_tot+i]));
  }
  return (vResult);

}

/////////////////////////////////////////////////////////////////////////////

// *** retrieve AutoCorrTotal (nsamples*(nsamples-1)/2 coeffs) for a given cell ***
const std::vector<double> 
LArAutoCorrTotalTool::autoCorrTotal(const HWIdentifier& CellID,
				    int gain, float Nminbias) const
{
  int thisgain = (m_isSC ? 0 : gain);

  if(!m_cacheValid){
    LArAutoCorrTotalTool* this2 = const_cast<LArAutoCorrTotalTool*>(this); 
    StatusCode  sc = this2->getTerms();
    if (sc.isFailure()) 
      {
	ATH_MSG_ERROR( "getTerms failed " );
	throw LArConditionsException("Could not compute in LArAutoCorrTotalTool::autoCorrTotal");
      }
  }

  HWIdentifier id;
  if (m_MCSym) 
    id = m_larmcsym->symOnline(CellID);
  else 
    id = CellID;
  unsigned int id32 = id.get_identifier32().get_compact();

  MAP::const_iterator it = (m_terms[thisgain]).find(id32) ; 
  if(it == (m_terms[thisgain]).end()){
    ATH_MSG_DEBUG( "Unable to find ID = " << CellID << " in m_terms" );
    static std::vector<double> empty; 
    return empty; 
  }
  
  return ( this->computeAutoCorr((*it).second,Nminbias) );  
}

/////////////////////////////////////////////////////////////////////////////

// *** retrieve AutoCorrTotal (4 coeffs) for a given cell ***
const std::vector<double> 
LArAutoCorrTotalTool::autoCorrTotal(const Identifier& CellID,
				    int gain, float Nminbias) const
{
  HWIdentifier id = m_cablingService->createSignalChannelID(CellID);  
  return this->autoCorrTotal(id, gain, Nminbias);
}

void LArAutoCorrTotalTool::handle(const Incident&) {
     ATH_MSG_DEBUG( "In Incident-handle"  );

     if(!m_cacheValid){
       StatusCode  sc = this->getTerms();
       if (sc.isFailure()) 
	 {
	   ATH_MSG_ERROR( "getTerms failed " );
	   throw LArConditionsException("Could not getTerms in LArAutoCorrTotalTool::handle ");
	 }
     }
     
}
///////////////////////////////////////////////////////////////////////////
const std::vector<double>
LArAutoCorrTotalTool::samplRMS(const HWIdentifier& CellID, 
                                int gain, float Nminbias) const
{
  int thisgain = (m_isSC ? 0 : gain);
  HWIdentifier id;
  if (m_MCSym) 
    id = m_larmcsym->symOnline(CellID);
  else    
    id = CellID; 
  unsigned int id32 = id.get_identifier32().get_compact();

  MAP::const_iterator it = (m_terms[thisgain]).find(id32) ;
  if(it == (m_terms[thisgain]).end()){
    ATH_MSG_ERROR( "Unable to find ID = " << CellID << " in m_terms" );
    static std::vector<double> empty;  
    return empty;  
  }
  
  return ( this->computeRMS((*it).second,Nminbias) );  
}

///////////////////////////////////////////////////////////
const std::vector<double>
LArAutoCorrTotalTool::samplRMS(const Identifier& CellID, 
                                    int gain, float Nminbias) const
  
{
  HWIdentifier id = m_cablingService->createSignalChannelID(CellID);  
  return this->samplRMS(id, gain, Nminbias);
}

