/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIFlow.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HIFlow.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>
using std::cout;using std::endl;
#include <string>

// #include "TH2D.h"
#include "TMath.h"

#include "HIGlobal/HIFlowData.h"
#include "HIGlobal/HICaloUtil.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloTowerContainer.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "Particle/TrackParticleContainer.h"

// 1 = calculate v2 , 0 = do not calculate
const bool HIFlow::CaloTagv2[HICaloUtil::Unknown] = {
  1, 1, 1, 1,                 // LAr barrel  N                       
  1, 1, 1, 1,		      // LAr EM endcap (eta negative side) 
  1, 1, 1, 1,		      // Hadronic end cap cal. N side	  
  1, 1, 1,		      // Tile bar (ITC & scint) N side	  
  1, 1, 1,		      // Tile gap (ITC & scint) N side	  
  1, 1, 1, 		      // Tile extended barrel   N side	  
  1, 1, 1, 		      // Forward EM endcap      N side 	  
  1, 1, 1, 1,                 // LAr barrel  P                      
  1, 1, 1, 1,		      // LAr EM endcap (eta positive side) 
  1, 1, 1, 1,		      // Hadronic end cap cal. P side	  
  1, 1, 1,		      // Tile bar (ITC & scint) P side	  
  1, 1, 1,		      // Tile gap (ITC & scint) P side	  
  1, 1, 1, 		      // Tile extended barrel   P side	  
  1, 1, 1		      // Forward EM endcap      P side     
};

// 1 = calculte psi , 0 = do not calcultate
const bool HIFlow::CaloTagPsi[HICaloUtil::Unknown] = {
  1, 1, 1, 1,                 // LAr barrel  N                       
  1, 1, 1, 1,		      // LAr EM endcap (eta negative side) 
  1, 1, 1, 1,		      // Hadronic end cap cal. N side	  
  1, 1, 1,		      // Tile bar (ITC & scint) N side	  
  1, 1, 1,		      // Tile gap (ITC & scint) N side	  
  1, 1, 1, 		      // Tile extended barrel   N side	  
  1, 1, 1, 		      // Forward EM endcap      N side 	  
  1, 1, 1, 1,                 // LAr barrel  P                      
  1, 1, 1, 1,		      // LAr EM endcap (eta positive side) 
  1, 1, 1, 1,		      // Hadronic end cap cal. P side	  
  1, 1, 1,		      // Tile bar (ITC & scint) P side	  
  1, 1, 1,		      // Tile gap (ITC & scint) P side	  
  1, 1, 1, 		      // Tile extended barrel   P side	  
  1, 1, 1		      // Forward EM endcap      P side     
  
};

//================ Constructor =================================================

 HIFlow::HIFlow(const std::string& name, ISvcLocator* pSvcLocator)
  :
  Algorithm(name,pSvcLocator),
  m_log(msgSvc(),name)
{
  //  template for property decalration
  declareProperty("doCalo", m_doCalo=true);
  declareProperty("doTrack", m_doTrack=true);
  declareProperty("doSi", m_doSi=false);
  declareProperty("HIFlowDataContainerName", m_HIFlowDataContainerName="ReactionPlaneAndV2");
  declareProperty("isData",m_isData);
  declareProperty("HIFlowWeightFileName", m_HIFlowWeightFileName="HIFlowWeight.root"); 
  declareProperty("Harmonic",m_harmonic=2.0);
  declareProperty("DoReweighting",m_do_reweighting=false);
  m_isData = false;
  m_weightfile = 0;
  m_FCAL0_weight = 0; 
}

//================ Destructor =================================================

 HIFlow::~HIFlow()
{}


//================ Initialisation =================================================

StatusCode  HIFlow::initialize()
{
  // Code entered here will be executed once at program start.
  
  m_log.setLevel(outputLevel());
  m_log << MSG::DEBUG << name() << " initialize()" << endreq;

  // retrieve the StoreGate Service (delete if not needed)
  StatusCode sc= service("StoreGateSvc",m_sgSvc);
  if (sc.isFailure()) m_log << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
  else m_log << MSG::DEBUG << "StoreGateSvc retrieved!" << endreq;
  
  m_HICaloUtil = new HICaloUtil();
  m_HICaloUtil->InitBinSizeEtaRange(HIFlow::CaloTagv2);
 
  m_weightfile = new TFile(m_HIFlowWeightFileName.c_str());
  
  if(m_weightfile)
  {
    m_FCAL0_weight = (TH2D*) m_weightfile->Get("wFcal0_EtaPhi");
    m_log << MSG::INFO << "Using file " << m_weightfile << " for event plane reweighting" << endreq;
    if(!m_FCAL0_weight)
    {
      m_log << MSG::WARNING << "Could not read reweighting file. No reweighting being applied" << endreq;
      m_do_reweighting=false;
    }
  }
  else 
  {
    m_log << MSG::INFO << "Using no event plane reweighting" << endreq;
    if(m_do_reweighting) m_log << MSG::WARNING << "Conflicting options, reweighting requested but no file specified. No reweighting being applied." << endreq;
    m_do_reweighting=false;
  }
  
  m_log << MSG::DEBUG << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation ===================================
StatusCode  HIFlow::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ======================================
StatusCode  HIFlow::execute()
{
  // Code entered here will be executed once per event
  
  m_log << MSG::DEBUG << "  execute() ..." << endreq;
  
  // Code entered here will be executed once per event
  m_HIFlowData =  new HIFlowData;

  const CaloCellContainer * CellContainer=0;
  const CaloTowerContainer* cTowers=0;
  const InDet::SiClusterContainer * prdContainer=0;
  const Rec::TrackParticleContainer * TPcontainer=0;
  StatusCode sc;
  if(m_doCalo || m_doSi)
  {
    sc = m_sgSvc->retrieve (CellContainer,"AllCalo");
    if ( sc.isFailure() )
    {
      m_log<<MSG::ERROR << "Could not retrieve AllCalo" << endreq;
      return StatusCode::FAILURE;
    }
    GetPsiCaloCells(CellContainer);
    sc = m_sgSvc->retrieve (cTowers,"CombinedTower");
    if ( sc.isFailure() )
    {
      m_log<<MSG::ERROR << "Could not retrieve CombinedTower" << endreq;
      return StatusCode::FAILURE;
    }
    GetPsiTowers(cTowers);
    
    if(m_doCalo) Getv2EtaDependentCaloCells(CellContainer);
    if(m_doSi)
    {
      sc = m_sgSvc->retrieve(prdContainer, "PixelClusters");
      if (sc.isFailure() )
      {
	m_log<<MSG::ERROR << "Pixel Cluster Container NOT found"<< endreq;
	return StatusCode::FAILURE;
      } 
      Getv2sil(prdContainer);
    }
  }

  if(m_doTrack)
  {
    sc = m_sgSvc->retrieve(TPcontainer, "TrackParticleCandidate");
    if (sc.isFailure())
    {
      m_log << MSG::ERROR << "Unable to retrieve TrackParticleCandidate" << endreq;
      return StatusCode::FAILURE;
    }
    GetPsitrack(TPcontainer);
    m_HIFlowData->v2track( Getv2track(TPcontainer) );  
  }

  sc = m_sgSvc->record( m_HIFlowData,m_HIFlowDataContainerName );
  if (sc.isFailure())
    m_log << MSG::ERROR << "Could not record " << m_HIFlowDataContainerName << endreq;
  else
    m_log << MSG::DEBUG << m_HIFlowDataContainerName << " recorded !" << endreq;
  return StatusCode::SUCCESS;
}

void HIFlow::GetPsiCaloCells(const CaloCellContainer * cCellc)
{ 
  float psi[HICaloUtil::Unknown]  ={0.0}; 
  float ssin[HICaloUtil::Unknown] ={0.0};
  float scos[HICaloUtil::Unknown] ={0.0};
  float w=0;

  float psiEt[HICaloUtil::Unknown]  ={0.0}; 
  float ssinEt[HICaloUtil::Unknown] ={0.0};
  float scosEt[HICaloUtil::Unknown] ={0.0};
  float wEt=0;
  
  int idf; 
  int* MapN = m_HICaloUtil->GetMapN();
  int* MapP = m_HICaloUtil->GetMapP();
  
  CaloCellContainer::const_iterator f_cell=cCellc->begin();
  CaloCellContainer::const_iterator l_cell=cCellc->end();
  for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ;
      int id= cell->caloDDE()->getSampling();
      idf=-1;
      if ( cell->caloDDE()->eta_raw() > 0.2) {
	idf = MapP[id]; 
      }
      if ( cell->caloDDE()->eta_raw() < - 0.2) {
	idf = MapN[id]; 
      }
      if(HIFlow::CaloTagPsi[idf]){
	w   = cell->e();
	wEt = cell->et();
	if(idf!=-1){
	  float cp = cell->phi();
	  ssin[idf]    +=  w   * sin(m_harmonic*cp) ;
	  scos[idf]    +=  w   * cos(m_harmonic*cp) ;
	  ssinEt[idf]  +=  wEt * sin(m_harmonic*cp) ;
	  scosEt[idf]  +=  wEt * cos(m_harmonic*cp) ;
	}
      }
    }

  for(int i=0;i<HICaloUtil::Unknown;i++)
    {
      if(HIFlow::CaloTagPsi[i]){
	psi[i] = (1.0/m_harmonic)*atan2(ssin[i],scos[i]);
	psiEt[i] = (1.0/m_harmonic)*atan2(ssinEt[i],scosEt[i]);
	m_log << MSG::DEBUG  <<  HICaloUtil::CaloDetImage(i)
	      << " psi e/et " << psi[i] <<" / " << psiEt[i] 
	      << " ssin / scos " << ssin[i] <<" / " << scos[i] 
	      << endreq;
      }
    }

  m_HIFlowData->Ssin( ssin );
  m_HIFlowData->Scos( scos );
  m_HIFlowData->Psi( psi );

  m_HIFlowData->SsinEt( ssinEt );
  m_HIFlowData->ScosEt( scosEt );
  m_HIFlowData->PsiEt(  psiEt );
  
  return;
}

void HIFlow::Getv2EtaDependentCaloCells(const CaloCellContainer * cCellc)
{
  m_log << MSG::DEBUG << " Getv2EtaDependentCaloCells " << endreq;
  
  std::vector<float> v2EtaEMB1[HICaloUtil::Unknown];
  std::vector<float> v2EtaFCAL0[HICaloUtil::Unknown];
  std::vector<float> sumw[HICaloUtil::Unknown];

  std::vector<float> v2EtaEtEMB1[HICaloUtil::Unknown];
  std::vector<float> v2EtaEtFCAL0[HICaloUtil::Unknown];
  std::vector<float> sumwEt[HICaloUtil::Unknown];

  std::vector<float> entry[HICaloUtil::Unknown];
  
  m_HICaloUtil->SetToZero(v2EtaEMB1);
  m_HICaloUtil->SetToZero(v2EtaFCAL0);
  m_HICaloUtil->SetToZero(sumw);

  m_HICaloUtil->SetToZero(v2EtaEtEMB1);
  m_HICaloUtil->SetToZero(v2EtaEtFCAL0);
  m_HICaloUtil->SetToZero(sumwEt);

  m_HICaloUtil->SetToZero(entry);
  
  float w  =0;
  float wEt=0;
  float psiEMB1 =0  , psiFCAL0   =0; 
  float psiEtEMB1 =0, psiEtFCAL0 =0; 
  
  std::vector<float>* EtaRange= m_HICaloUtil->GetEtaRange();
  float* BinSize =  m_HICaloUtil->GetBinSize();
  int* MapN = m_HICaloUtil->GetMapN();
  int* MapP = m_HICaloUtil->GetMapP();
  
  int j_etaBin=0;
  
  CaloCellContainer::const_iterator f_cell=cCellc->begin();
  CaloCellContainer::const_iterator l_cell=cCellc->end();
  for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ;
      int id=cell->caloDDE()->getSampling();
      int idf; 
      if ( cell->caloDDE()->eta_raw() >= 0.0){
	idf = MapP[id]; 
      } 
      else{
	idf = MapN[id]; 
      }
      if(HIFlow::CaloTagv2[idf]){
	w   = cell->e();
	wEt = cell->et();
	if ( cell->caloDDE()->eta_raw() >= 0.0){ 
	  psiEMB1    = m_HIFlowData->Psi()[HICaloUtil::EMBN1];
	  psiFCAL0   = m_HIFlowData->Psi()[HICaloUtil::FCALN0];
	  psiEtEMB1  = m_HIFlowData->PsiEt()[HICaloUtil::EMBN1];
	  psiEtFCAL0 = m_HIFlowData->PsiFCAL0_w(); 
	} 
	else{
	  psiEMB1    = m_HIFlowData->Psi()[HICaloUtil::EMBP1];
	  psiFCAL0   = m_HIFlowData->Psi()[HICaloUtil::FCALP0]; 
	  psiEtEMB1  = m_HIFlowData->PsiEt()[HICaloUtil::EMBP1];
	  psiEtFCAL0 = m_HIFlowData->PsiFCAL0_w(); 
	}
	if(cell->caloDDE()->eta_raw() >= EtaRange[idf][0]){
	  j_etaBin = (int) ( (cell->caloDDE()->eta_raw() - EtaRange[idf][0])/BinSize[idf]); 
          float cp =   cell->phi();   
	  v2EtaEMB1 [idf][j_etaBin]   += w * cos(m_harmonic*(cp - psiEMB1));
	  v2EtaFCAL0[idf][j_etaBin]   += w * cos(m_harmonic*(cp - psiFCAL0));
	  sumw      [idf][j_etaBin]   += w;

	  v2EtaEtEMB1 [idf][j_etaBin] += wEt * cos(m_harmonic*(cp - psiEtEMB1));
	  v2EtaEtFCAL0[idf][j_etaBin] += wEt * cos(m_harmonic*(cp - psiEtFCAL0));
	  sumwEt      [idf][j_etaBin] += wEt;

	  entry[idf][j_etaBin]++;
	}
      }
    }
  float sum_entry=0;
  for(int i=0;i<HICaloUtil::Unknown;i++){
    if(HIFlow::CaloTagv2[i]) m_log << MSG::DEBUG << HICaloUtil::CaloDetImage(i)<< endreq;
    for(int j=0;j< (int)v2EtaEMB1[i].size();j++){
      if(entry[i][j]&& sumw[i][j] !=0. && sumwEt[i][j]!=0.){
	sum_entry+=entry[i][j];
	v2EtaEMB1[i][j]=(v2EtaEMB1[i][j])/sumw[i][j];
	v2EtaFCAL0[i][j]=(v2EtaFCAL0[i][j])/sumw[i][j];

	v2EtaEtEMB1[i][j]=(v2EtaEtEMB1[i][j])/sumwEt[i][j];
	v2EtaEtFCAL0[i][j]=(v2EtaEtFCAL0[i][j])/sumwEt[i][j];
      }
      else{
	v2EtaEMB1[i][j] =9999;
	v2EtaFCAL0[i][j]=9999;
	v2EtaEtEMB1[i][j] =9999;
	v2EtaEtFCAL0[i][j]=9999;
      }
      float eta_cent = (j+0.5)*BinSize[i] + EtaRange[i][0];
      m_log << MSG::DEBUG << 
	" " << j << 
	"eta_cent "  << eta_cent <<
	", v2EtaEMB1 e/et" << v2EtaEMB1[i][j]<< " / " << v2EtaEtEMB1[i][j]<< 
	", v2EtaFCAL0 e/et " << v2EtaFCAL0[i][j] << " / " << v2EtaEtFCAL0[i][j] << 
	", entry " << entry[i][j] <<
	", sumw " << sumw[i][j] << endreq;
    }
  }
  m_log << MSG::DEBUG  << " Number of all cells used in v2 calculations= " << sum_entry << endreq;
  m_HIFlowData->v2EtaEMB1(v2EtaEMB1);
  m_HIFlowData->v2EtaFCAL0(v2EtaFCAL0);
  m_HIFlowData->SumE(sumw);
  
  m_HIFlowData->v2EtaEtEMB1(v2EtaEtEMB1);
  m_HIFlowData->v2EtaEtFCAL0(v2EtaEtFCAL0);
  m_HIFlowData->SumEt(sumwEt);
  
  return;
}

void HIFlow::Getv2sil(const InDet::SiClusterContainer * prdContainer)
{
  // iterator over containers
  InDet::SiClusterContainer::const_iterator colNext = prdContainer->begin();
  InDet::SiClusterContainer::const_iterator lastCol = prdContainer->end(); 
  float v2silEMB1 =0, v2NEMB1=0, v2PEMB1=0;
  float phi1NEMB1=0, phi1PEMB1=0;
  
  float v2silFCAL0 =0, v2NFCAL0=0, v2PFCAL0=0;
  float phi1NFCAL0=0, phi1PFCAL0=0;
  
  
  float x=0, y=0, z=0;
  float phi=0, theta=0, eta=0;
  int count=0;
// obsolete, CBNT are no longer used   
//  CBNTAA_HIGlobalVar *cn; 
  for (; colNext != lastCol; ++colNext) {
    // iterators over PRDs in container
    InDet::SiClusterCollection::const_iterator nextCluster = (*colNext)->begin();
    InDet::SiClusterCollection::const_iterator lastCluster = (*colNext)->end();    
    for (; nextCluster!=lastCluster; ++nextCluster){      
      const  InDetDD::SiDetectorElement * element ;
      element = (*nextCluster)->detectorElement();
      int detector=-1;
      if(element){
        std::string result=element->getIdHelper()->print_to_string(element->identify());
	//detector=cn->GetSiLayerNumber(result);
	x = (*nextCluster)->globalPosition().x();
	y = (*nextCluster)->globalPosition().y();
	z = (*nextCluster)->globalPosition().z();
	phi = atan2(y,x);
	theta = atan2((sqrt(x*x+y*y)),z);
	eta = -log(tan(theta/2));    
	if ( detector == 2000 ){
	  if ( eta > 0.0) {
	    phi1NEMB1 = phi - m_HIFlowData->Psi()[HICaloUtil::EMB1];
	    v2NEMB1 += cos(m_harmonic*phi1NEMB1);
	    
	    phi1NFCAL0 = phi - m_HIFlowData->Psi()[HICaloUtil::FCAL0];
	    v2NFCAL0 += cos(m_harmonic*phi1NFCAL0);
	  }
	  
	  if ( eta < 0.0) {
	    phi1PEMB1 = phi - m_HIFlowData->Psi()[HICaloUtil::EMB1];
	    v2PEMB1 += cos(m_harmonic*phi1PEMB1);
	    
	    phi1PFCAL0 = phi - m_HIFlowData->Psi()[HICaloUtil::FCAL0];
	    v2PFCAL0 += cos(m_harmonic*phi1PFCAL0);
	  }
	  count++;
	}
      }
    }
  }
  
  if(count)v2silEMB1  = (v2NEMB1 + v2PEMB1)/count;
  if(count)v2silFCAL0 = (v2NFCAL0 + v2PFCAL0)/count;
  m_log << MSG::DEBUG << " v2silEMB1 " << v2silEMB1  << endreq;
  m_log << MSG::DEBUG << " v2silFCAL0 " << v2silFCAL0 << endreq;
  m_HIFlowData->v2EMB1sil(v2silEMB1 );
  m_HIFlowData->v2FCAL0sil( v2silFCAL0  );
  
  return;
}

void HIFlow::GetPsiTowers(const CaloTowerContainer *cTower)
{
  float psi_FCAL0_w  = 0;
  float psiN_FCAL0_w = 0;
  float psiP_FCAL0_w = 0;
  float ssinEtFCAL0P = 0;
  float ssinEtFCAL0N = 0;
  float scosEtFCAL0P = 0;
  float scosEtFCAL0N = 0;
  
  CaloTowerContainer::const_iterator iTower = cTower->begin();
  CaloTowerContainer::const_iterator lastTower = cTower->end();
  
  for( ; iTower != lastTower; ++iTower) 
  {
    const CaloTower* tower_ptr = *iTower;
    
    
    if((tower_ptr->eta() > 3.2) && (tower_ptr->eta() < 4.8))
    {
      CaloTower::cell_iterator fCell = tower_ptr->begin();
      CaloTower::cell_iterator lCell = tower_ptr->end();
      float wEt = 0; 
      for ( ; fCell != lCell; fCell++ )
      {
	const CaloCell *cell = (*fCell) ;      
	int det = cell->caloDDE()->getSampling();
	if(det == 21)
	{
	  double cellweight = tower_ptr->getCellWeight(cell);
	  wEt += cell->et() * cellweight;    
	}
      }
      
      if(m_isData)
      {
	ssinEtFCAL0P  +=  get_weight(tower_ptr->eta(), tower_ptr->phi(), m_FCAL0_weight) * wEt * sin(m_harmonic*tower_ptr->phi()) ;
	scosEtFCAL0P  +=  get_weight(tower_ptr->eta(), tower_ptr->phi(), m_FCAL0_weight) * wEt * cos(m_harmonic*tower_ptr->phi()) ;
      }
      else
      {
	ssinEtFCAL0P  +=  wEt * sin(m_harmonic*tower_ptr->phi()) ;
	scosEtFCAL0P  +=  wEt * cos(m_harmonic*tower_ptr->phi()) ;
      }
    }
    if((tower_ptr->eta() < -3.2) && (tower_ptr->eta() > -4.8))
    {
      CaloTower::cell_iterator fCell = tower_ptr->begin();
      CaloTower::cell_iterator lCell = tower_ptr->end();
      float wEt = 0;
      for ( ; fCell != lCell; fCell++ )
      {
	const CaloCell *cell = (*fCell) ;      
	int det = cell->caloDDE()->getSampling();
	if(det == 21)
	{
	  double cellweight = tower_ptr->getCellWeight(cell);
	  wEt += cell->et() * cellweight;    
	}
      }
      if(m_isData)
      {
	ssinEtFCAL0N  +=  get_weight(tower_ptr->eta(), tower_ptr->phi(), m_FCAL0_weight) * wEt * sin(m_harmonic*tower_ptr->phi()) ;
	scosEtFCAL0N  +=  get_weight(tower_ptr->eta(), tower_ptr->phi(), m_FCAL0_weight) * wEt * cos(m_harmonic*tower_ptr->phi()) ;
      }
      else
      {
	ssinEtFCAL0N  +=  wEt * sin(m_harmonic*tower_ptr->phi()) ;
	scosEtFCAL0N  +=  wEt * cos(m_harmonic*tower_ptr->phi()) ;
      }
    }
  }
  
  psi_FCAL0_w =(1.0/m_harmonic)*atan2((ssinEtFCAL0N+ssinEtFCAL0P),(scosEtFCAL0N+scosEtFCAL0P));     
  psiN_FCAL0_w=(1.0/m_harmonic)*atan2(ssinEtFCAL0N,scosEtFCAL0N);    
  psiP_FCAL0_w=(1.0/m_harmonic)*atan2(ssinEtFCAL0P,scosEtFCAL0P); 

  m_HIFlowData->PsiFCAL0_w(psi_FCAL0_w);
  m_HIFlowData->PsiNFCAL0_w(psiN_FCAL0_w);
  m_HIFlowData->PsiPFCAL0_w(psiP_FCAL0_w);
}

//........Tracks............
void HIFlow::GetPsitrack(const Rec::TrackParticleContainer * TPcontainer)
{
  
  float psiP_track =0;
  float psiN_track =0;
  float ssinP=0, scosP=0;
  float ssinN=0, scosN=0;
  float phi=0, theta=0, eta=0;	
  
  typedef DataVector<Rec::TrackParticle>::const_iterator TPIter;
  
  for ( TPIter fz=TPcontainer->begin(); fz!=TPcontainer->end(); ++fz)
    {
      //const Rec::TrackParticle * myTrackParticle = (*fz);      
	const Trk::Perigee* aMeasPer;
        aMeasPer= (*fz)->measuredPerigee();  
	
	phi = aMeasPer->parameters()[Trk::phi];
        theta = aMeasPer->parameters()[Trk::theta];
	eta = -log(tan(theta/2));
	
	
	if ( eta > 0.2) {
	  ssinP  +=  sin(m_harmonic*phi) ;
	  scosP  +=  cos(m_harmonic*phi) ;
	}
	
	 if ( eta < -0.2) {
	   ssinN  +=  sin(m_harmonic*phi) ;
	   scosN  +=  cos(m_harmonic*phi) ;
         }	 
    }
  
  psiP_track = (1.0/m_harmonic)*atan2(ssinP,scosP);
  psiN_track = (1.0/m_harmonic)*atan2(ssinN,scosN); 
  m_HIFlowData->PsiPtrack( psiP_track );
  m_HIFlowData->PsiNtrack( psiN_track );
  
  m_log << MSG::DEBUG << " psiP_track " << psiP_track << " psiN_track " << psiN_track << endreq;
  return;
}

float HIFlow::Getv2track(const Rec::TrackParticleContainer * TPcontainer)
{                                        
  typedef DataVector<Rec::TrackParticle>::const_iterator TPIter;
  
  float v2track =0, v2N=0, v2P=0;
  if(TPcontainer==0)return 0.;
  float phi1N=0, phi1P=0;
  float phi=0, theta=0, eta=0;
  int countN=0, countP=0;
  for ( TPIter fz=TPcontainer->begin(); fz!=TPcontainer->end(); ++fz)
    {
      const Trk::Perigee* aMeasPer;
      aMeasPer= (*fz)->measuredPerigee(); 
      phi = aMeasPer->parameters()[Trk::phi];
      theta = aMeasPer->parameters()[Trk::theta];
      if(theta==0)
        eta=100.;
      else
        eta = -log(tan(theta/2));
      
         if ( eta > 0.0) {
	   phi1N = phi - m_HIFlowData->PsiNtrack();
	   v2N += cos(m_harmonic*phi1N);
	   countN++;
         }
	 
         if ( eta < 0.0) {
	   phi1P = phi - m_HIFlowData->PsiPtrack();
	   v2P += cos(m_harmonic*phi1P);
	   countP++;
         }
    }
                                                      
  if(countN+countP > 0) v2track = (v2N + v2P)/(countN + countP);
  //std::cout << " v2track " << v2track << " PsiNtrack " << m_HIFlowData->PsiNtrack() << " PsiPtrack " << m_HIFlowData->PsiPtrack() << std::endl;
  return v2track;  
}

float HIFlow::get_weight(float eta, float phi, TH2D *hist)
{
  if(m_do_reweighting)
  {
    int bin_num_phi = (int)((phi+TMath::Pi())/hist->GetYaxis()->GetBinWidth(1));
    int bin_num_eta = (int)((eta+5)/hist->GetXaxis()->GetBinWidth(1));
    return hist->GetBinContent(bin_num_eta+1, bin_num_phi+1);
  }
  return 1.0;
}
