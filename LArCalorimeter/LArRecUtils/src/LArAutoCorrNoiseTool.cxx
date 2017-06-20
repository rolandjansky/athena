/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecUtils/LArAutoCorrNoiseTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "LArElecCalib/LArConditionsException.h"
#include "StoreGate/StoreGateSvc.h"

#include <cmath>

/////////////////////////////////////////////////////////////////////////////

LArAutoCorrNoiseTool::LArAutoCorrNoiseTool(const std::string& type, 
					   const std::string& name, 
					   const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_Nsampl(5),m_MCSym(true), 
    m_lar_on_id(nullptr),
    m_lar_scon_id(nullptr),
    m_cablingService("LArCablingService"),
    m_larmcsym("LArMCSymTool"),
    m_keyAutoCorr("LArAutoCorr"),
    m_cacheValid(false),m_loadAtBegin(true),
    m_isSC(false)

{
  declareInterface<ILArAutoCorrNoiseTool>(this);

  declareProperty("keyAutoCorr",m_keyAutoCorr);
  declareProperty("NSamples",m_Nsampl);
  declareProperty("MCSym",m_MCSym);
  declareProperty("LoadAtBegin",m_loadAtBegin);
  declareProperty("SuperCell",m_isSC);

}

/////////////////////////////////////////////////////////////////////////////

StatusCode LArAutoCorrNoiseTool::initialize()
{
  ATH_MSG_DEBUG( "LArAutoCorrNoiseTool initialize() begin"  );
  
  if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrNoiseTool::LoadCalibration,
                                              dynamic_cast<ILArAutoCorrNoiseTool*>(this),m_dd_autocorr,m_keyAutoCorr)) {
    ATH_MSG_INFO( "Registered callback for key: " << m_keyAutoCorr  );
  } else {
    ATH_MSG_ERROR( "Cannot register testCallback function for key " << m_keyAutoCorr  );
  }
  
  if ( m_isSC ){
    ATH_CHECK(  detStore()->retrieve(m_lar_scon_id,"LArOnline_SuperCellID") );
    ATH_CHECK( m_cablingSCService.retrieve() );
  } // m_isSC
  else {
    ATH_CHECK(  detStore()->retrieve(m_lar_on_id,"LArOnlineID") );
    ATH_CHECK( m_cablingService.retrieve() );
    if (m_MCSym) {
      ATH_CHECK( m_larmcsym.retrieve() );
    }
  }
  
  if (m_loadAtBegin) {
    ATH_MSG_DEBUG( "Setting callback function to load calibration at begin of run"  );
    // Incident Service: 
    IIncidentSvc* incSvc = nullptr;
    ATH_CHECK(  service("IncidentSvc", incSvc) );
    
    //start listening to "BeginRun". The incident should be fired AFTER the IOV callbacks and only once.
    const long priority=std::numeric_limits<long>::min(); //Very low priority
    incSvc->addListener(this, "BeginRun", priority ,false,true); //single-shot incident
  }
  
  ATH_MSG_DEBUG( "LArAutoCorrNoiseTool initialize() end"  );
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
StatusCode LArAutoCorrNoiseTool::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

// *** retrieves needed data from the DB *** 
StatusCode LArAutoCorrNoiseTool::LoadCalibration(IOVSVC_CALLBACK_ARGS)
{
  m_cacheValid = false; 
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

// *** compute some terms of the calculation of the autocorr function *** 
StatusCode LArAutoCorrNoiseTool::getTerms()
{
  
#ifndef NDEBUG
  ATH_MSG_DEBUG( "in getAutoCorrNoise::getTerms"  );
#endif

  // get HWIdentifier iterator
  std::vector<HWIdentifier>::const_iterator it  ; 
  std::vector<HWIdentifier>::const_iterator it_e;
  LArCablingBase* cable(0);
  unsigned int ngains(3);
  if ( m_isSC ){
  it   =m_lar_scon_id->channel_begin();
  it_e =m_lar_scon_id->channel_end();
  LArSuperCellCablingTool* tcable = &(*m_cablingSCService);
  cable = (LArCablingBase*)tcable;
  ngains=1;
  } else {
  it   =m_lar_on_id->channel_begin();
  it_e =m_lar_on_id->channel_end();
  LArCablingService* tcable = &(*m_cablingService);
  cable = (LArCablingService*)tcable;
  }
  
  // resize vector to #(gain) = 3
  m_terms.resize(ngains);
  
  int count = 0;
  int count2 = 0;
  // loop over em Identifiers
#ifndef NDEBUG
  ATH_MSG_DEBUG( "start loop over cells in getAutoCorrNoise"  );
#endif
  for(;it!=it_e;++it)
  {    
    count ++;
    const HWIdentifier id = *it;
    unsigned int id32     = id.get_identifier32().get_compact();

    if(cable->isOnlineConnected(id)) 
    {    
      if(!m_isSC && m_MCSym) {
        HWIdentifier id2 = m_larmcsym->symOnline(id);
        if (id2 != id) continue;
      }
      count2++;
      // the Shape is a function of gain
      for(unsigned int igain=0;igain<ngains;igain++)
      {    
	ILArAutoCorr::AutoCorrRef_t AC =  m_dd_autocorr->autoCorr(id,igain);

	std::vector<float> vTerms;
        int size = m_Nsampl*m_Nsampl;
        vTerms.resize(size);

// compute sqrt to covariance matrix
        for (int j=0;j<m_Nsampl;j++) {
// a) diagonal terms
            double ck=0.;
            for (int k=0;k<=j-1;k++) {
                int index=j*m_Nsampl+k;
                ck=ck+vTerms[index]*vTerms[index];
            }
//     diagonal term of autocorr matrix = 1
            vTerms[j*m_Nsampl+j]=sqrt(std::fabs(1.-ck));
// b) off-diagonal terms
            for (int i=j+1;i<m_Nsampl;i++) {
                double ck=0;
                for(int k=0;k<=j-1;k++) {
                 int index1=i*m_Nsampl+k; 
                 int index2=j*m_Nsampl+k; 
                 ck=ck+vTerms[index1]*vTerms[index2];
                }
                int index=i*m_Nsampl+j;
                int index2=j*m_Nsampl+j;
                unsigned int index3=(i-j)-1;
                double AutoCorr=0.;
                if (index3<(AC.size()))  AutoCorr=AC[index3];
                vTerms[index]=(AutoCorr-ck)/vTerms[index2];
            }
        }

	m_terms[igain][id32] = vTerms;

      }//(loop on gains)

    } 
    else//unconnected
      for(unsigned int igain=0;igain<ngains;igain++) 
      {
	m_terms[igain][id32] = std::vector<float>(1, 0.);
      }

  }
#ifndef NDEBUG
  ATH_MSG_INFO( "LArAutoCorrNoise Ncell " << count  );
  ATH_MSG_INFO( "LArAutoCorrNoise Nsymcell " << count2  );
  ATH_MSG_DEBUG( "end of loop over cells "  );
#endif

  m_cacheValid = true;
  return StatusCode::SUCCESS;
}
 
/////////////////////////////////////////////////////////////////////////////

const std::vector<float> &
LArAutoCorrNoiseTool::autoCorrSqrt(const HWIdentifier& CellID,
				    int gain, int Nsampl) 
{

  if(!m_cacheValid){
    StatusCode  sc = this->getTerms();
    if (sc.isFailure()) 
      {
	ATH_MSG_ERROR( "getTerms failed " );
	throw LArConditionsException("Could not compute in LArAutoCorrNoiseTool::autoCorrSqrt");
      }
  }
// recompute everything if number of samples is different from initial ones
  if (Nsampl>0 && Nsampl != m_Nsampl) {
    m_Nsampl = Nsampl;
    if ( (this->getTerms()).isFailure() ){
      ATH_MSG_ERROR( "getTerms failed"  );
    }
  }

  HWIdentifier id;
  if (!m_isSC && m_MCSym) 
    id = m_larmcsym->symOnline(CellID);
  else 
    id = CellID;
  unsigned int id32 = id.get_identifier32().get_compact();
  
  MAP::const_iterator it = (m_terms[gain]).find(id32) ; 
  if(it == (m_terms[gain]).end()){
    ATH_MSG_ERROR( "Unable to find ID = " << CellID << " in m_terms" );
    static std::vector<float> empty; 
    return empty; 
  }
  
  return ((*it).second);  
}

/////////////////////////////////////////////////////////////////////////////

const std::vector<float> &
LArAutoCorrNoiseTool::autoCorrSqrt(const Identifier& CellID,
                                   int gain, int /*Nsampl*/) 
{

  if(!m_cacheValid){
    StatusCode  sc = this->getTerms();
    if (sc.isFailure()) 
      {
	ATH_MSG_ERROR( "getTerms failed " );
	throw LArConditionsException("Could not compute in LArAutoCorrNoiseTool::autoCorrSqrt");
      }
  }
  HWIdentifier id;
  if (!m_isSC && m_MCSym)
    id = m_larmcsym->symOnline(CellID);
  else 
    id = m_cablingService->createSignalChannelID(CellID);
  unsigned int id32 = id.get_identifier32().get_compact();

  MAP::const_iterator it = (m_terms[gain]).find(id32) ; 
  if(it == (m_terms[gain]).end()){
    ATH_MSG_ERROR( "Unable to find ID = " << CellID << " in m_terms" );
    static std::vector<float> empty;  
    return empty;  
  }
  
  return ((*it).second); 

}


void LArAutoCorrNoiseTool::handle(const Incident&) {
     ATH_MSG_DEBUG( "In Incident-handle"  );

     if(!m_cacheValid){
       StatusCode  sc = this->getTerms();
       if (sc.isFailure()) 
	 {
	   ATH_MSG_ERROR( "getTerms failed " );
	   throw LArConditionsException("Could not getTerms in LArAutoCorrNoiseTool::handle");
	 }
     }
     
}
