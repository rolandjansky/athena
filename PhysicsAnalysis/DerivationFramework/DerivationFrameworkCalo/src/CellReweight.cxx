/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CellReweight includes
#include "DerivationFrameworkCalo/CellReweight.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CxxUtils/make_unique.h"
#include "Identifier/IdentifierHash.h"
#include "PathResolver/PathResolver.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include <memory>
DerivationFramework::CellReweight::CellReweight( const std::string& t,
                          const std::string& n,
                          const IInterface* p ): 
  AthAlgTool(t,n,p),
  m_cellContainerName(""),
  m_newCellName(""),
  m_caloRegion(""),
  m_CellRatioPath(""),
  m_SGKey_electrons(""),
  m_SGKey_photons(""),
  neta_Lr2(7),
  nphi_Lr2(11),
  m_caloID(0),
  m_tool711(0),
  m_tool37(0),
  m_tool55(0),
  m_tool2(0)
{ 
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("CaloClusterName",m_cellContainerName="AODCellContainer");
  declareProperty("ReweightCellContainerName", m_newCellName="ReweightedCellContainer");
  declareProperty("ReweightRatioPath", m_CellRatioPath = "DerivationFrameworkCalo/CellReweight_v1/CellRatio.root");
  declareProperty("Lr2_nEta", neta_Lr2 = 7, "eta/phi region reweighted for Lr2 cells 37 55 711");
  declareProperty("Lr2_nPhi", nphi_Lr2 = 11, "eta/phi region reweighted for Lr2 cells 37 55 711");
  declareProperty("CaloFillRectangularTool711", m_caloFillRectangularTool711, "Handle of the CaloFillRectangularClusterTool7x11");
  declareProperty("CaloFillRectangularTool37", m_caloFillRectangularTool37, "Handle of the CaloFillRectangularClusterTool3x7");
  declareProperty("CaloFillRectangularTool55", m_caloFillRectangularTool55, "Handle of the CaloFillRectangularClusterTool5x5");
  declareProperty("CaloCellContainerFinalizerTool", m_caloCellContainerFinalizerTool, "Handle of the CaloCellContainerFinalizerTool");
  declareProperty("SGKey_photons", m_SGKey_photons);
  declareProperty("SGKey_electrons", m_SGKey_electrons);

}

DerivationFramework::CellReweight::~CellReweight() {}

StatusCode DerivationFramework::CellReweight::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  if(!m_caloFillRectangularTool711.empty()) ATH_CHECK(m_caloFillRectangularTool711.retrieve());
  if(!m_caloFillRectangularTool37.empty()) ATH_CHECK(m_caloFillRectangularTool37.retrieve());
  if(!m_caloFillRectangularTool55.empty()) ATH_CHECK(m_caloFillRectangularTool55.retrieve());
  if(!m_caloCellContainerFinalizerTool.empty()) ATH_CHECK(m_caloCellContainerFinalizerTool.retrieve());
  m_tool711 = dynamic_cast<CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool711 ));
  m_tool37 = dynamic_cast<CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool37 ));
  m_tool55 = dynamic_cast<CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool55 ));
  m_tool2 = dynamic_cast<CaloCellContainerFinalizerTool* >( &(*m_caloCellContainerFinalizerTool ));
  
  CHECK( detStore()->retrieve(m_caloID));
  
  if(m_SGKey_photons == "" and m_SGKey_electrons == ""){
    ATH_MSG_FATAL("No e-gamma collection provided");
    return StatusCode::FAILURE;
  }

  std::string filename = PathResolverFindCalibFile( m_CellRatioPath );
  if (filename.empty()){
    ATH_MSG_ERROR ( "Could NOT resolve file name " << m_CellRatioPath );
    return StatusCode::FAILURE ;
  } else{
    ATH_MSG_INFO ( "Path found = "<<filename );
  }
  m_CellRatioPath = filename;       
  
  
  TFile *fratio = TFile::Open(m_CellRatioPath.c_str(),"READ");
  for( int neta = 1; neta<15; neta++) {
    _hratio[("Lr1eta"+seta_bins[neta-1]+seta_bins[neta])] =(TH1F *) ((TH1F*)fratio->Get( ("Lr1Ratio_"+seta_bins[neta-1]+"_"+seta_bins[neta]).c_str()))->Clone(("Lr1ratio_"+seta_bins[neta-1]+"_"+seta_bins[neta]).c_str());
    _hratio[("Lr2eta"+seta_bins[neta-1]+seta_bins[neta])] =(TH1F *) ((TH1F*)fratio->Get( ("Lr2Ratio_"+seta_bins[neta-1]+"_"+seta_bins[neta]).c_str()))->Clone(("Lr2ratio_"+seta_bins[neta-1]+"_"+seta_bins[neta]).c_str());
  }
  _hratio[("Lr1eta"+seta_bins[8]+seta_bins[9])] = (TH1F *) ((TH1F*)fratio->Get( ("Lr1Ratio_060_080")))->Clone(("Lr1ratio_0_137152"));  //for [2.4, 2.6] and crack, 0 energy shift!
  _hratio[("Lr2eta"+seta_bins[8]+seta_bins[9])] = (TH1F *) ((TH1F*)fratio->Get( ("Lr2Ratio_060_080")))->Clone(("Lr2ratio_0_137152"));
  _hratio[("Lr1eta"+seta_bins[14]+seta_bins[15])] = (TH1F *) ((TH1F*)fratio->Get( ("Lr1Ratio_060_080")))->Clone(("Lr1ratio_0_240260"));
  _hratio[("Lr2eta"+seta_bins[14]+seta_bins[15])] = (TH1F *) ((TH1F*)fratio->Get( ("Lr2Ratio_060_080")))->Clone(("Lr2ratio_0_240260"));
  _hratio[("Lr1eta"+seta_bins[8]+seta_bins[9])]->Scale(0.);
  _hratio[("Lr2eta"+seta_bins[8]+seta_bins[9])]->Scale(0.);
  _hratio[("Lr1eta"+seta_bins[14]+seta_bins[15])]->Scale(0.);
  _hratio[("Lr2eta"+seta_bins[14]+seta_bins[15])]->Scale(0.);
  
  return StatusCode::SUCCESS;
}



StatusCode DerivationFramework::CellReweight::addBranches() const
{  
  if (evtStore()->contains<CaloConstCellContainer>(m_newCellName.c_str())) {
    ATH_MSG_WARNING("The cell container " << m_newCellName << " is already present in the event, no action will be taken. If this is not intended, modify your code to use a different container name.");
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<CaloConstCellContainer> NewCells = CxxUtils::make_unique<CaloConstCellContainer>(static_cast<SG::OwnershipPolicy>(static_cast<int>(SG::OWN_ELEMENTS)));

  const CaloCellContainer* cellCont(0);
  if (evtStore()->retrieve(cellCont,m_cellContainerName).isFailure())
    ATH_MSG_WARNING(m_cellContainerName<< " not found");

  IdentifierHash m_hashMax = m_caloID->calo_cell_hash_max();
  std::vector<bool> notInContainer(m_hashMax, true);
  for (const std::string& SGkey : {m_SGKey_photons, m_SGKey_electrons}){

    if(SGkey == "") continue;
    const xAOD::EgammaContainer* egammas = nullptr;
    ATH_CHECK( evtStore()->retrieve(egammas, SGkey) );

    int nstrip = 3;  //strips used in Lr1 reweighting
    bool PassSelect = true;
    for(auto egamma : *egammas) {
      PassSelect = ((egamma->pt())>7000.&& ( fabs( egamma->eta() ) < 1.37 || fabs( egamma->eta() ) > 1.52 )&& fabs( egamma->eta() ) < 2.40 ); // now reject [1.3,1.6]
      egamma->auxdecor<int>(Form("%dx%dClusterExist", neta_Lr2, nphi_Lr2)) = 0;
      if(PassSelect) { 
	egamma->auxdecor<int>(Form("%dx%dClusterExist", neta_Lr2, nphi_Lr2)) = 1;

	bool isBarrel = xAOD::EgammaHelpers::isBarrel(egamma);

	//Lr1 size determination
 	xAOD::CaloCluster::ClusterSize cluSizeLr1=xAOD::CaloCluster::CSize_Unknown;
	if (xAOD::EgammaHelpers::isElectron(egamma)) {
	  cluSizeLr1= (isBarrel ? xAOD::CaloCluster::SW_37ele : xAOD::CaloCluster::SW_55ele);
	}
	else if ( xAOD::EgammaHelpers::isConvertedPhoton(egamma)){
	  cluSizeLr1= (isBarrel ? xAOD::CaloCluster::SW_37Econv : xAOD::CaloCluster::SW_55Econv);
	}
	else  {//(egType == xAOD::EgammaParameters::unconvertedPhoton)
	  cluSizeLr1= (isBarrel ? xAOD::CaloCluster::SW_37gam : xAOD::CaloCluster::SW_55gam);
	}

	//Lr2 size determination
 	xAOD::CaloCluster::ClusterSize cluSizeLr2=xAOD::CaloCluster::CSize_Unknown;
	if (neta_Lr2 == 7) cluSizeLr2 = xAOD::CaloCluster::SW_7_11;
	else if (xAOD::EgammaHelpers::isElectron(egamma)) {
	  cluSizeLr2= ( (neta_Lr2 == 3) ? xAOD::CaloCluster::SW_37ele : xAOD::CaloCluster::SW_55ele);
	}
	else if ( xAOD::EgammaHelpers::isConvertedPhoton(egamma)){
	  cluSizeLr2= ( (neta_Lr2 == 3) ? xAOD::CaloCluster::SW_37Econv : xAOD::CaloCluster::SW_55Econv);
	}
	else  {//(egType == xAOD::EgammaParameters::unconvertedPhoton)
	  cluSizeLr2= ( (neta_Lr2 == 3) ? xAOD::CaloCluster::SW_37gam : xAOD::CaloCluster::SW_55gam);
	}
	
	double PhiCalo = 0.;
	if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME, PhiCalo))) {
	  ATH_MSG_ERROR("Failed to retrieve phi from calo");
	}

	double EtaCalo = 0.;
	if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME, EtaCalo))) {
	  ATH_MSG_ERROR("Failed to retrieve eta from calo");
	}

	double Eta0 =   egamma->caloCluster()->eta0() + EtaCalo - egamma->caloCluster()->eta();
	double Phi0 =   egamma->caloCluster()->phi0() + PhiCalo - egamma->caloCluster()->phi();

	if (Phi0 < -M_PI)  Phi0 = (Phi0 + 2*M_PI);
	if (Phi0 > M_PI)   Phi0 = (Phi0 - 2*M_PI);

	// Make new 7x11 Lr1 and Lr2 Clusters from AOD caloCluster
	xAOD::CaloCluster* egcCloneLr2(0);
	egcCloneLr2 = CaloClusterStoreHelper::makeCluster( cellCont,
							   Eta0,
							   Phi0,
							   cluSizeLr2);
	xAOD::CaloCluster* egcCloneLr1(0);
	egcCloneLr1 = CaloClusterStoreHelper::makeCluster( cellCont,
							   Eta0, 
							   Phi0,
							   cluSizeLr1);

	// corrections for Lr2 cluster (?)
	if(neta_Lr2 == 5) m_tool55->makeCorrection(egcCloneLr2);
	else if(neta_Lr2 == 3) m_tool37->makeCorrection(egcCloneLr2);
	else m_tool711->makeCorrection(egcCloneLr2);

	// corrections for Lr1 cluster (?)
	if( isBarrel )
	  {nstrip = 3; m_tool37->makeCorrection(egcCloneLr1);}
	else
	  {nstrip = 5; m_tool55->makeCorrection(egcCloneLr1);}

	if(!egcCloneLr1) {
	  egamma->auxdecor<int>(Form("%dx%dClusterExist", neta_Lr2, nphi_Lr2))= 0;
	  delete egcCloneLr2;
	  delete egcCloneLr1;
	  continue;
	}


	xAOD::CaloCluster* newClu;

	int _neta = 12;			//eta region of egamma
	float clus_eta = egamma->caloCluster()->eta0();
	_neta = LocateEta(clus_eta);
	int EtaSize = GetEtaSize(_neta);
	std::string temp_bin;
	std::map<std::string,TH1F *>::const_iterator temp_ratio; //loop all Lr1 Lr2 Cells to obtain Lr reweighted energy to fix energy with extra ratio

	//
	// reweighting of Lr2 cell energies
	//
	newClu= egcCloneLr2;								
	auto first_cell = newClu->cell_begin();
	auto last_cell = newClu->cell_end();
	int ncelll2(0);
	float sume_lr2_old = 0.;
	std::vector<float>  Lr2_e_old(11,0.);
	std::vector<float>  Lr2_eta;
	std::vector<int> Lr2_Eta_index;

	if (egcCloneLr2){

	  // first, compute sum of old energies
	  for(ncelll2 = 0; first_cell != last_cell; ++first_cell) {
	    const CaloCell* tcell = *first_cell;
	    int sampling = tcell->caloDDE()->getSampling();
	    if(sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){
	      int t_eta = ncelll2/nphi_Lr2;
	      if(ncelll2%nphi_Lr2 == 0)
		Lr2_eta.push_back(tcell->eta());
	      float e_temp = tcell->e();
	      sume_lr2_old += e_temp;
	      Lr2_e_old.at(t_eta) += e_temp;
	      ncelll2++;
	    }		
	  }
	  ATH_MSG_DEBUG ("Lr2 Reweighted cell number:" << ncelll2 );
	  if( ncelll2 == 0 ) {
	    egamma->auxdecor<int>(Form("%dx%dClusterExist", neta_Lr2, nphi_Lr2))= 0;
	    delete egcCloneLr2;
	    delete egcCloneLr1;
	    continue;
	  }

	  // second, compute the normalisation factors for the reweighted cell energies
	  std::vector<float> Lr2_e_new(Lr2_e_old);
	  std::vector<float> Lr2_function;
	  float sume_lr2_new = 0.;
	  SetIndexLr2(&Lr2_Eta_index, &Lr2_eta, &Lr2_e_old, clus_eta);
	  for( unsigned int neta = 0; neta < Lr2_Eta_index.size(); neta++) {
	    temp_bin = "Lr2eta"+seta_bins[_neta-1]+seta_bins[_neta];
	    temp_ratio = _hratio.find(temp_bin);
	    Lr2_e_new.at(neta) += sume_lr2_old*((temp_ratio->second)->GetBinContent(Lr2_Eta_index.at(neta)));
	    sume_lr2_new += Lr2_e_new.at(neta);
	  }
	  float knormalize2 = sume_lr2_old/sume_lr2_new;
	  for( unsigned int neta = 0; neta < Lr2_Eta_index.size(); neta++) {
	    if( Lr2_e_old.at(neta) == 0.)
	      Lr2_function.push_back(1.);
	    else
	      Lr2_function.push_back(  knormalize2 * Lr2_e_new.at(neta) / Lr2_e_old.at(neta) );
	  }

	  // third, set the new cell energies
	  first_cell = newClu->cell_begin();
	  for (ncelll2 = 0; first_cell != last_cell; ++first_cell) {
	    CaloCell* tcell = first_cell->clone();
	    int sampling = tcell->caloDDE()->getSampling();
	    if(sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){
	      int t_eta = ncelll2/nphi_Lr2;
	      float e_temp = tcell->e();
	      tcell->setEnergy(e_temp*Lr2_function.at(t_eta));
	      ncelll2++;
	    }			
	    if(sampling== CaloCell_ID::EMB1 || sampling== CaloCell_ID::EME1 ){
	      continue;
	    }
	    if(notInContainer[tcell->caloDDE()->calo_hash()]){
	      notInContainer[tcell->caloDDE()->calo_hash()] = false;
	      NewCells->push_back(tcell);
	    }
	    else 
	      delete tcell;
	  }
	} // done with full 7x11 good Lr2 
	else {	// if no full 7x11, then go to 5x5, 3x7
	  int lr2phi;
	  if(nstrip == 5) 
	    lr2phi = 5;
	  else 
	    lr2phi = 7;								
	  newClu=egcCloneLr1;
	  first_cell = newClu->cell_begin();
	  last_cell = newClu->cell_end();
	  for(ncelll2 = 0; first_cell != last_cell; ++first_cell) {
	    const CaloCell* tcell = *first_cell;
	    int sampling = tcell->caloDDE()->getSampling();
	    if(sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){
	      int t_eta = ncelll2/lr2phi;
	      if(ncelll2%lr2phi == 0) Lr2_eta.push_back(tcell->eta());
	      float e_temp = tcell->e();
	      sume_lr2_old += e_temp;		
	      Lr2_e_old.at(t_eta) += e_temp;
	      ncelll2++;
	    }		
	  }
	  if( ncelll2 == 0 ) {
	    egamma->auxdecor<int>(Form("%dx%dClusterExist", neta_Lr2, nphi_Lr2))= 0;
	    delete egcCloneLr2;
	    delete egcCloneLr1;
	    continue;
	  }
	  std::vector<float> Lr2_e_new(Lr2_e_old);
	  std::vector<float> Lr2_function;
	  float sume_lr2_new = 0.;
	  SetIndexLr2(&Lr2_Eta_index, &Lr2_eta, &Lr2_e_old, clus_eta);
	  for(unsigned int neta = 0; neta < Lr2_Eta_index.size(); neta++) {
	    temp_bin = "Lr2eta"+seta_bins[_neta-1]+seta_bins[_neta];
	    temp_ratio = _hratio.find(temp_bin);
	    Lr2_e_new.at(neta) += sume_lr2_old*((temp_ratio->second)->GetBinContent(Lr2_Eta_index.at(neta)));
	    sume_lr2_new += Lr2_e_new.at(neta);
	  }
	  float knormalize2 = sume_lr2_old/sume_lr2_new;
	  for(unsigned int neta = 0; neta < Lr2_Eta_index.size(); neta++) {
	    if( Lr2_e_old.at(neta) == 0.) 
	      Lr2_function.push_back(1.);
	    else
	      Lr2_function.push_back(  knormalize2 * Lr2_e_new.at(neta) / Lr2_e_old.at(neta) );
	  }
	  first_cell = newClu->cell_begin();
	  for (ncelll2 = 0; first_cell != last_cell; ++first_cell) {
	    CaloCell* tcell = first_cell->clone();
	    int sampling = tcell->caloDDE()->getSampling();
	    if(sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){
	      int t_eta = ncelll2/lr2phi;
	      float e_temp = tcell->e();
	      tcell->setEnergy(e_temp*Lr2_function.at(t_eta));
	      ncelll2++;
	    }			
	    if(sampling== CaloCell_ID::EMB1 || sampling== CaloCell_ID::EME1 ){
	      continue;
	    }
	    if(notInContainer[tcell->caloDDE()->calo_hash()]){
	      notInContainer[tcell->caloDDE()->calo_hash()] = false;
	      NewCells->push_back(tcell);
	    }
	    else 
	      delete tcell;
	  }
	}
      

	//
	// reweighting of Lr1 cell energies
	//
	newClu=egcCloneLr1;
	first_cell = newClu->cell_begin();
	last_cell = newClu->cell_end();
	float sume_lr1_old = 0.;
	int ncelll1(0);
	std::vector<float>  Lr1_e_all(120,0.);
	std::vector<float>  Lr1_eta, Lr1_eta_all,  Lr1_e_old;
	std::vector<int> Lr1_Eta_index;

	// why do we need the lines below?
	std::vector<float>  Lr2_e_temp(11,0.);
	Lr2_Eta_index.clear();
	Lr2_eta.clear();
	int lr2phi;
	if (nstrip == 5)
	  lr2phi = 5;
	else
	  lr2phi = 7;
	// why do we need the lines above?

	// first, compute sum of old energies
	for (ncelll1 = 0, ncelll2 =0 ;first_cell != last_cell; ++first_cell) {
	  const CaloCell* tcell = *first_cell;
	  float e_temp = tcell->e();
	  int sampling = tcell->caloDDE()->getSampling();
	  if(sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){
	    // why do we need this? do we use Lr2 info in this block???
	    int t_eta = ncelll2/lr2phi;
	    if(ncelll2%lr2phi == 0) Lr2_eta.push_back(tcell->eta());
	    Lr2_e_temp.at(t_eta) += e_temp;
	    ncelll2++;
	  }		
	  if(sampling== CaloCell_ID::EMB1 || sampling== CaloCell_ID::EME1 ){
	    sume_lr1_old += e_temp;
	    Lr1_e_all.at(ncelll1) += e_temp;
	    Lr1_eta_all.push_back(tcell->eta());
	    ncelll1++;
	  }
	  
	}
	ATH_MSG_DEBUG ("Lr1 Reweighted cell number: Lr2 " << ncelll2 << " Lr1" << ncelll1 );
	if(ncelll1 == 0 || Lr2_eta.size() == 0) {
	  egamma->auxdecor<int>(Form("%dx%dClusterExist", neta_Lr2, nphi_Lr2))= 0;
	  delete egcCloneLr2;
	  delete egcCloneLr1;
	  continue;
	}
	
	// second, compute the normalisation factors for the reweighted cell energies
	int Lr1_phi = 2;
	if(ncelll1 < 1.5*(float)(EtaSize*nstrip)) 		//1.5 to judge 1 or 2 phi strip
	  { 
	    Lr1_eta = Lr1_eta_all;
	    Lr1_e_old = Lr1_e_all;
	    Lr1_phi = 1;
	  }
	else {
	  for(int t = 0; t< ncelll1; t++) { 
	    if(fabs(Lr1_eta_all[t]-Lr1_eta_all[t+1])<0.0005){
	      Lr1_e_old.push_back(Lr1_e_all[t]+Lr1_e_all[t+1]);
	      Lr1_eta.push_back(Lr1_eta_all[t]);
	      t++;
	    } 
	    else{
	      Lr1_e_old.push_back(Lr1_e_all[t]);
	      Lr1_eta.push_back(Lr1_eta_all[t]); 
	    }
	    Lr1_phi =2;
	  }
	}
	std::vector<float> Lr1_e_new(Lr1_e_old);			
	std::vector<float> Lr1_function;
	float sume_lr1_new = 0.;
	std::vector<int> neta_index;
	SetIndexLr2(&Lr2_Eta_index, &Lr2_eta,  &Lr2_e_temp, clus_eta);
	SetIndexLr1(&Lr1_Eta_index, &neta_index, &Lr1_eta, &Lr1_e_old, &Lr2_eta, clus_eta); // Lr1 cells number controlled by Lr2 index. A bit tricky...
	for(unsigned int neta = 0; neta < Lr1_Eta_index.size(); neta++) {
	  temp_bin = "Lr1eta"+seta_bins[neta_index.at(neta)-1]+seta_bins[neta_index.at(neta)];	
	  temp_ratio = _hratio.find(temp_bin);
	  Lr1_e_new.at(neta) += sume_lr1_old*((temp_ratio->second)->GetBinContent(Lr1_Eta_index.at(neta)));
	  sume_lr1_new += Lr1_e_new.at(neta);	
	}
	float knormalize = sume_lr1_old/sume_lr1_new;
	for(unsigned int neta = 0; neta < Lr1_Eta_index.size(); neta++) {
	  if (Lr1_e_old.at(neta) == 0.)
	    Lr1_function.push_back( 1. );
	  else 
	    Lr1_function.push_back( knormalize * Lr1_e_new.at(neta) / Lr1_e_old.at(neta) );					
	}			
	
	// third, set the new cell energies
	first_cell = newClu->cell_begin();
	for (ncelll1 = 0; first_cell != last_cell; ++first_cell) {
	  CaloCell* tcell = first_cell->clone();
	  int sampling = tcell->caloDDE()->getSampling();
	  if(sampling== CaloCell_ID::EMB1 || sampling== CaloCell_ID::EME1 ){
	    int t_eta = ncelll1/Lr1_phi;
	    float e_temp = tcell->e();
	    tcell->setEnergy(e_temp * Lr1_function.at(t_eta));
	    ncelll1++;
	    if(notInContainer[tcell->caloDDE()->calo_hash()]){
	      notInContainer[tcell->caloDDE()->calo_hash()] = false;
	      NewCells->push_back(tcell);
	    }
	    else 
	      delete tcell;
	  }
	}
	if(egcCloneLr1)	delete egcCloneLr1;
	if(egcCloneLr2)	delete egcCloneLr2;
      }
      
    }
  }
  
  auto first_cell = cellCont->begin();
  auto last_cell = cellCont->end();
  for (; first_cell != last_cell; ++first_cell) {	
    CaloCell* tcell = (*first_cell)->clone();		
    if(notInContainer[tcell->caloDDE()->calo_hash()]){
      notInContainer[tcell->caloDDE()->calo_hash()] = false;
      NewCells->push_back(tcell);
    }
    else
      delete tcell;
  }
  ATH_CHECK(m_tool2->process(NewCells.get()));
  ATH_CHECK(evtStore()->record( std::move(NewCells),m_newCellName.c_str()));
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CellReweight::finalize()
{
  ATH_MSG_INFO ( "CellReweight::finalize()" );
  return StatusCode::SUCCESS;
}

int DerivationFramework::CellReweight::LocateMid(std::vector<float> * _eta, float m_eta ) const 
{
  float diff = 1000.;
  int _i = 0;
  for(unsigned int i = 0 ; i < _eta->size(); i++){
    float t_diff = fabs(_eta->at(i) - m_eta);	
    if(t_diff < diff) { _i = i; diff = t_diff;}
  }
  return _i;		
}

int DerivationFramework::CellReweight::LocateMidE(std::vector<float> * _e) const
{
  float maxe = -1.;
  int _i = 0;
  for(unsigned int i = 0 ; i < _e->size(); i++){
    if(_e->at(i) > maxe) { _i = i; maxe = _e->at(i);}
  }
  return _i;
}

int  DerivationFramework::CellReweight::LocateEta(float t_eta) const
{
  int n_eta = 15;
  for( int neta = 1; neta<16; neta++) {
    if ( (fabs(t_eta) <= eta_bins[neta] ) && (fabs(t_eta) > eta_bins[neta-1] )) {
      n_eta = neta; break;
    }
  }
  return n_eta;
}

int DerivationFramework::CellReweight::GetEtaSize(int n_eta) const
{
  int _etasize = 8;
  switch(n_eta) {
  case 9 : { _etasize = 8; break;}
  case 12 : { _etasize = 6; break;}
  case 13 : { _etasize = 4; break;}
  case 14 : { _etasize = 4; break;}
  case 15 : { _etasize = 8; break;}
  }
  return _etasize;
}

bool SortByEtaMinus(const std::pair<float, int > &pair1, const std::pair<float, int > &pair2){ // hight to low 
  return(pair1.first > pair2.first);
}

bool SortByEtaPlus(const std::pair<float, int > &pair1, const std::pair<float, int > &pair2){ //low to high
  return(pair1.first < pair2.first);
}

void DerivationFramework::CellReweight::SetIndexLr2(std::vector<int> * index,  std::vector<float> *_eta, std::vector<float> *_lr2e, float m_eta) const
{
  std::vector<std::pair<float, int >> eta_index;
  std::vector<float> new_eta;
  std::vector<int> old_index;
  index->clear();
  for(unsigned int i = 0; i< _eta->size(); i++){
    eta_index.push_back(std::make_pair(_eta->at(i), i));
  }
  if(m_eta<0)
    std::sort(eta_index.begin(),eta_index.end(), SortByEtaMinus);
  else
    std::sort(eta_index.begin(),eta_index.end(), SortByEtaPlus);
  for(auto itr = eta_index.begin(); itr<eta_index.end(); itr++){
    new_eta.push_back(itr->first);
    index->push_back(0);
  }
  int newmid = LocateMidE(_lr2e);
  for(unsigned int i =0; i<new_eta.size(); i++){
    if(((eta_index.at(i)).second) == newmid) 
      { newmid = i; break;}
  }
  int etasize = _eta->size();
  switch(etasize){
  case 7: {newmid =3; break;}
  case 5: {newmid =2; break;}
  case 3: {newmid =1; break;}
    break;
  }
  for(unsigned int i  = 0; i<new_eta.size(); i++){
    index->at((eta_index.at(i)).second) = i - newmid + 4;
  }	
  eta_index.clear();
  new_eta.clear();
  old_index.clear();
  return;
}


void DerivationFramework::CellReweight::SetIndexLr1(std::vector<int> * index, std::vector<int> * neta_index, std::vector<float> *_eta, std::vector<float> *_e, std::vector<float> *_etaLr2, float m_eta) const
{
  std::vector<std::pair<float, int >> eta_index;
  std::vector<float> new_eta;
  int neta_0;
  index->clear();
  neta_index->clear();
  //	now using hottest cell!
  m_eta = _eta->at((int)LocateMidE(_e));
  // then why do we pass m_eta as argument????
  for(unsigned int i = 0; i< _eta->size(); i++){
    eta_index.push_back(std::make_pair(_eta->at(i), i));
  }
  if(m_eta<0)
    std::sort(eta_index.begin(),eta_index.end(), SortByEtaMinus);
  else
    std::sort(eta_index.begin(),eta_index.end(), SortByEtaPlus);
  for(auto itr = eta_index.begin(); itr<eta_index.end(); itr++){
    index->push_back(0);
    neta_index->push_back(0);
    new_eta.push_back(itr->first);
  }
  neta_0 = LocateEta(_etaLr2->at(0));
  int _neta =  LocateEta(m_eta);
  int etasize0 = GetEtaSize(neta_0);
  int EtaSize = GetEtaSize(_neta);
  int t_neta = 0;
  int newmid = LocateMid(&new_eta, m_eta);
  int nchange = 0;
  int nchange2 = 0;
  //Lr1 cells number controlled by Lr2 index
  //a bit tricky...
  for(unsigned int i = 0; i<_etaLr2->size(); i++,nchange2++){
    t_neta = LocateEta(_etaLr2->at(i));
    if(t_neta != neta_0) break;
  }
  nchange = etasize0*nchange2;		
  int etasize1 = GetEtaSize(neta_0+1);
  if(nchange < ((int)new_eta.size()-1)){
    if(neta_0 == _neta){
      for(int i  = 0; i<nchange; i++){
	index->at((eta_index.at(i)).second) = i - newmid + (2*EtaSize+1);
	neta_index->at((eta_index.at(i)).second) = neta_0;
      }
      float tk = ((float)(nchange-1.0-newmid))/((float)etasize0);
      int extra_strip =(int) (tk*((float)etasize1)+0.5);
      for(int i  = nchange; i<(int)index->size(); i++){
	neta_index->at((eta_index.at(i)).second) = neta_0+1;
	index->at((eta_index.at(i)).second) = i - nchange + (2*etasize1+1)+extra_strip+1;
      }				
    }		
    else{
      for(int i  = nchange; i<(int)index->size(); i++){
	index->at((eta_index.at(i)).second) = i - newmid + (2*EtaSize+1);
	neta_index->at((eta_index.at(i)).second) = neta_0+1;
      }
      float tk = ((float)(newmid-nchange))/((float)etasize1);
      int extra_strip =(int) (tk*((float)etasize1)+0.5);
      for(int i  = nchange-1; i>= 0; i--){
	neta_index->at((eta_index.at(i)).second) = neta_0;
	index->at((eta_index.at(i)).second) = i - nchange + (2*etasize0+1)-extra_strip;
      }				
    }		
  }
  else {
    for(unsigned int i  = 0; i<new_eta.size(); i++){
      index->at((eta_index.at(i)).second) = i - newmid + (2*EtaSize+1);
      neta_index->at((eta_index.at(i)).second) = neta_0;
    }				
  }
  return;
}
