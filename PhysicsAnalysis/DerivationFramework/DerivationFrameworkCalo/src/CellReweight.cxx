/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>

#include "DerivationFrameworkCalo/CellReweight.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloIdentifier/LArEM_Base_ID.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloIDHelper.h"
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
#include "CaloUtils/CaloCellDetPos.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include <TFile.h>
#include <sstream>


DerivationFramework::CellReweight::CellReweight( const std::string& t,
						 const std::string& n,
						 const IInterface* p ):
  AthAlgTool(t,n,p),
  m_oldCellContainerName(""),
  m_newCellContainerName(""),
  m_caloRegion(""),
  m_cellRatioPath(""),
  m_cellRatio2DPath(""),
  m_SGKey_electrons(""),
  m_SGKey_photons(""),
  m_caloID(0),
  m_tool711(0),
  m_tool37(0),
  m_tool55(0),
  m_tool2(0)
{ 
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("CellContainerName", m_oldCellContainerName = "AODCellContainer", "Input cell container");
  declareProperty("ReweightCellContainerName", m_newCellContainerName = "ReweightedCellContainer", "Output (reweighted) cell container");
  declareProperty("ReweightRatioPath", m_cellRatioPath = "DerivationFrameworkCalo/CellReweight_v1/CellRatio.root", "Coefficients for the 1D reweighting");
  declareProperty("ReweightCoefficients2DPath", m_cellRatio2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs10.root", "Coefficients for the 2D reweighting" );
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

  //
  // retrieve the tools that are needed
  //
  if (!m_caloFillRectangularTool711.empty()) ATH_CHECK( m_caloFillRectangularTool711.retrieve() );
  if (!m_caloFillRectangularTool37.empty()) ATH_CHECK( m_caloFillRectangularTool37.retrieve() );
  if (!m_caloFillRectangularTool55.empty()) ATH_CHECK( m_caloFillRectangularTool55.retrieve() );
  if (!m_caloCellContainerFinalizerTool.empty()) ATH_CHECK( m_caloCellContainerFinalizerTool.retrieve() );
  m_tool711 = dynamic_cast<CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool711 ) );
  m_tool37 = dynamic_cast<CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool37 ) );
  m_tool55 = dynamic_cast<CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool55 ) );
  m_tool2 = dynamic_cast<CaloCellContainerFinalizerTool* >( &(*m_caloCellContainerFinalizerTool ) );
  
  CHECK( detStore()->retrieve(m_caloID) );
  
  //
  // retrieve the name of the photon and/or electron collections whose cells will be reweighted
  //
  if(m_SGKey_photons == "" and m_SGKey_electrons == ""){
    ATH_MSG_FATAL("No e-gamma collection provided");
    return StatusCode::FAILURE;
  }
  
  //
  // retrieve 1D histograms for 1D reweighting (eta, phi)
  //
  std::string filename = PathResolverFindCalibFile( m_cellRatioPath );
  if (filename.empty()){
    ATH_MSG_ERROR ( "Could NOT resolve file name " << m_cellRatioPath );
    return StatusCode::FAILURE ;
  } else{
    ATH_MSG_INFO ( "Path found = " << filename );
  }
  m_cellRatioPath = filename;
  
  TFile *ratioFile = TFile::Open(m_cellRatioPath.c_str(),"READ");
  for (unsigned int neta = 1; neta<(m_nEtaBins-1); neta++) {
    m_hratio[("Lr1eta"+m_setaBins[neta-1]+m_setaBins[neta])] =(TH1F *) ((TH1F*)ratioFile->Get( ("Lr1Ratio_"+m_setaBins[neta-1]+"_"+m_setaBins[neta]).c_str()))->Clone(("Lr1ratio_"+m_setaBins[neta-1]+"_"+m_setaBins[neta]).c_str());
    m_hratio[("Lr2eta"+m_setaBins[neta-1]+m_setaBins[neta])] =(TH1F *) ((TH1F*)ratioFile->Get( ("Lr2Ratio_"+m_setaBins[neta-1]+"_"+m_setaBins[neta]).c_str()))->Clone(("Lr2ratio_"+m_setaBins[neta-1]+"_"+m_setaBins[neta]).c_str());
  }
  //for [2.4, 2.6] (bin 14) and crack [1.37-1.52] (bin 8), 0 energy shift!
  m_hratio[("Lr1eta"+m_setaBins[8]+m_setaBins[9])] = (TH1F *) ((TH1F*)ratioFile->Get( ("Lr1Ratio_060_080")))->Clone(("Lr1ratio_0_137152"));  
  m_hratio[("Lr2eta"+m_setaBins[8]+m_setaBins[9])] = (TH1F *) ((TH1F*)ratioFile->Get( ("Lr2Ratio_060_080")))->Clone(("Lr2ratio_0_137152"));
  m_hratio[("Lr1eta"+m_setaBins[14]+m_setaBins[15])] = (TH1F *) ((TH1F*)ratioFile->Get( ("Lr1Ratio_060_080")))->Clone(("Lr1ratio_0_240260"));
  m_hratio[("Lr2eta"+m_setaBins[14]+m_setaBins[15])] = (TH1F *) ((TH1F*)ratioFile->Get( ("Lr2Ratio_060_080")))->Clone(("Lr2ratio_0_240260"));
  m_hratio[("Lr1eta"+m_setaBins[8]+m_setaBins[9])]->Scale(0.);
  m_hratio[("Lr2eta"+m_setaBins[8]+m_setaBins[9])]->Scale(0.);
  m_hratio[("Lr1eta"+m_setaBins[14]+m_setaBins[15])]->Scale(0.);
  m_hratio[("Lr2eta"+m_setaBins[14]+m_setaBins[15])]->Scale(0.);
  
  //
  // retrieve 2D histograms for 2D reweighting
  //
  filename = PathResolverFindCalibFile( m_cellRatio2DPath );
  if (filename.empty()){
    ATH_MSG_ERROR ( "Could NOT resolve file name " << m_cellRatio2DPath );
    return StatusCode::FAILURE ;
  } else{
    ATH_MSG_INFO ( "Path found = " << filename );
  }
  m_cellRatio2DPath = filename;       
  
  TFile *ratio2DFile = TFile::Open(m_cellRatio2DPath.c_str(),"READ");
  for (unsigned int neta = 1; neta<(m_nEtaBins-1); neta++) {
    m_hratio2D[("Lr2eta"+m_setaBins[neta-1]+m_setaBins[neta])] = (TH2F*) ratio2DFile->Get(Form("cellsDelta_Eta_%1.2f_%1.2f", m_etaBins[neta-1], m_etaBins[neta]));
    ATH_MSG_DEBUG ("Lr2 neta: " << neta << " eta: " << m_setaBins[neta-1] << "-" << m_setaBins[neta]);
    ATH_MSG_DEBUG ("6,3 bin content: " << m_hratio2D[("Lr2eta"+m_setaBins[neta-1]+m_setaBins[neta])]->GetBinContent(6,3));
  }
  //for [2.4, 2.6] (bin 14) and crack [1.37-1.52] (bin 8), 0 energy shift!
  m_hratio2D[("Lr2eta"+m_setaBins[8]+m_setaBins[9])] = (TH2F*) ratio2DFile->Get("cellsDelta_Eta_0.60_0.80")->Clone("Lr2eta137152");
  m_hratio2D[("Lr2eta"+m_setaBins[14]+m_setaBins[15])] = (TH2F*) ratio2DFile->Get("cellsDelta_Eta_0.60_0.80")->Clone("Lr2eta240260");
  ATH_MSG_DEBUG( "neta: " << 9 << " 6,3 bin content: " << m_hratio2D[("Lr2eta"+m_setaBins[8]+m_setaBins[9])] ->GetBinContent(6,3) );  
  ATH_MSG_DEBUG( "neta: " << 15 << " 6,3 bin content: " << m_hratio2D[("Lr2eta"+m_setaBins[14]+m_setaBins[15])]->GetBinContent(6,3) );
  m_hratio2D[("Lr2eta"+m_setaBins[8]+m_setaBins[9])]->Scale(0.);
  m_hratio2D[("Lr2eta"+m_setaBins[14]+m_setaBins[15])]->Scale(0.);
  ATH_MSG_DEBUG( "neta: " << 9 << " 6,3 bin content: " << m_hratio2D[("Lr2eta"+m_setaBins[8]+m_setaBins[9])] ->GetBinContent(6,3) );  
  ATH_MSG_DEBUG( "neta: " << 15 << " 6,3 bin content: " << m_hratio2D[("Lr2eta"+m_setaBins[14]+m_setaBins[15])]->GetBinContent(6,3) );

  return StatusCode::SUCCESS;
}



StatusCode DerivationFramework::CellReweight::addBranches() const
{
  //
  // only execute this function if new cell container not already present in SG. If not present, create new cell container
  //
  if (evtStore()->contains<CaloConstCellContainer>(m_newCellContainerName.c_str())) {
    ATH_MSG_WARNING("The cell container " << m_newCellContainerName << " is already present in the event, no action will be taken. If this is not intended, modify your code to use a different container name.");
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<CaloConstCellContainer> newCellContainer = CxxUtils::make_unique<CaloConstCellContainer>(static_cast<SG::OwnershipPolicy>(static_cast<int>(SG::OWN_ELEMENTS)));
  

  //
  // retrieve cell container from SG 
  //
  const CaloCellContainer* oldCellContainer(0);
  if (evtStore()->retrieve(oldCellContainer,m_oldCellContainerName).isFailure())
    ATH_MSG_WARNING(m_oldCellContainerName << " not found");
  
  
  //
  // create vector notInContainer to keep track for each cell whether it has been added to the new cell container or not
  //
  size_t hashMax = m_caloID->calo_cell_hash_max();
  std::vector<bool> notInContainer(hashMax, true);


  //
  // do reweighting for cells belonging to photons and/or electrons (if corresponding SGKey is set)
  //
  for (const std::string& SGkey : {m_SGKey_photons, m_SGKey_electrons}) {
    
    // retrieve electron or photon container
    if(SGkey == "") continue;
    const xAOD::EgammaContainer* egammas = nullptr;
    ATH_CHECK( evtStore()->retrieve(egammas, SGkey) );
    
    // loop over the electrons/photons
    for (auto egamma : *egammas) {
      
      egamma->auxdecor<int>(Form("%dx%dClusterExist", m_neta_Lr2, m_nphi_Lr2)) = 0;
      
      // apply loose preselection
      bool passSelection = ( (egamma->pt() > 7000.) && 
			     fabs( egamma->eta() ) < 2.40 &&
			     ( fabs( egamma->eta() ) < 1.37 || fabs( egamma->eta() ) > 1.52 ) );
      if (!passSelection) continue;
      
      egamma->auxdecor<int>(Form("%dx%dClusterExist", m_neta_Lr2, m_nphi_Lr2)) = 1;
      
      bool isBarrel = xAOD::EgammaHelpers::isBarrel(egamma);
      ATH_MSG_DEBUG ( "Is this cluster in the barrel? " << isBarrel );

      // Lr1 cluster size determination
      xAOD::CaloCluster::ClusterSize cluSizeLr1=xAOD::CaloCluster::CSize_Unknown;
      if (xAOD::EgammaHelpers::isElectron(egamma)) {
	cluSizeLr1 = (isBarrel ? xAOD::CaloCluster::SW_37ele : xAOD::CaloCluster::SW_55ele);
      }
      else if (xAOD::EgammaHelpers::isConvertedPhoton(egamma)){
	cluSizeLr1 = (isBarrel ? xAOD::CaloCluster::SW_37Econv : xAOD::CaloCluster::SW_55Econv);
      }
      else  {
	assert(xAOD::EgammaHelpers::isConvertedPhoton(egamma) && !xAOD::EgammaHelpers::isConvertedPhoton(egamma));
	cluSizeLr1 = (isBarrel ? xAOD::CaloCluster::SW_37gam : xAOD::CaloCluster::SW_55gam);
      }
      
      // Lr2 cluster size determination
      xAOD::CaloCluster::ClusterSize cluSizeLr2=xAOD::CaloCluster::SW_7_11;
	
      //  compute eta0 and phi0 (cluster seed), needed to make new clusters
      double etaCalo = 0.;
      if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME, etaCalo))) {
	ATH_MSG_ERROR("Failed to retrieve eta from calo");
      }
      double phiCalo = 0.;
      if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME, phiCalo))) {
	ATH_MSG_ERROR("Failed to retrieve phi from calo");
      }

      double eta0 = egamma->caloCluster()->eta0() + etaCalo - egamma->caloCluster()->eta();
      double phi0 = egamma->caloCluster()->phi0() + phiCalo - egamma->caloCluster()->phi();    
      if (phi0 < -M_PI)  phi0 = (phi0 + 2*M_PI);
      if (phi0 > M_PI)   phi0 = (phi0 - 2*M_PI);
      ATH_MSG_DEBUG( "eta0 = " << eta0 << " , phi0 = " << phi0 );

      // Make new clusters from AOD caloCluster (note, both Lr2 and Lr1 contain cells from all layers.
      // They (can) differ by cluster size (cluSizeLr2 vs cluSizeLr1) and later usage
      xAOD::CaloCluster* egcCloneLr2(0);
      egcCloneLr2 = CaloClusterStoreHelper::makeCluster( oldCellContainer,
							 eta0,
							 phi0,
							 cluSizeLr2);
      xAOD::CaloCluster* egcCloneLr1(0);
      egcCloneLr1 = CaloClusterStoreHelper::makeCluster( oldCellContainer,
							 eta0, 
							 phi0,
							 cluSizeLr1);

      // make corrections for Lr2 cluster
      m_tool711->makeCorrection(egcCloneLr2);

      // make corrections for Lr1 cluster
      int nStrips(3);  //number of strips used in Lr1 reweighting and default value
      if (isBarrel) {
	nStrips = 3; m_tool37->makeCorrection(egcCloneLr1);
      }
      else {
	nStrips = 5; m_tool55->makeCorrection(egcCloneLr1);
      }
	
      // check that new Lr1 cluster is valid. If not, go to next candidate
      if(!egcCloneLr1) {
	egamma->auxdecor<int>(Form("%dx%dClusterExist", m_neta_Lr2, m_nphi_Lr2))= 0;
	ATH_MSG_DEBUG( "New Lr1 cluster is 0, skipping candidate" );
	delete egcCloneLr2;
	delete egcCloneLr1;
	continue;
      }


      xAOD::CaloCluster* newClu;
      float clusterSeedEta = egamma->caloCluster()->eta0();
      int etaBin = getEtaBin(clusterSeedEta);
      int etaSizeL1 = getEtaSize(etaBin);
      std::string temp_bin;
      std::map<std::string,TH1F *>::const_iterator temp_ratio;   // loop all Lr1 Lr2 Cells to obtain Lr reweighted energy to fix energy with extra ratio
      std::map<std::string,TH2F *>::const_iterator temp_ratio2D; // 2D reweighting coefficients for Lr2


      //
      // reweighting of layer-2 cell energies
      //
      double sume_lr2_old = 0.;            // the total energy of layer-2 cells before reweighting
      std::vector<float> Lr2_e_old(11,0.);  // the energies for each eta row. In principle it should be a vector of 7 elements but some clusters have more than 7x11 cells so let's make it a bit longer
      std::vector<float> Lr2_eta;          // the eta of each row of the cluster
      std::vector<int> Lr2_eta_index;
      int ncelll2(0);                      // the number of layer-2 cells of the cluster
	
      if (egcCloneLr2){

	newClu = egcCloneLr2;								
	auto thisCell = newClu->cell_begin();
	auto lastCell = newClu->cell_end();
	  
	// first, compute sum of old energies (sume_lr2_old) and find the number (maxElement) of the hottest cell (energy=maxE) to know if the cluster is complete (77=7x11 cells) and regular (hottest cell=37, 38 or 39)
	int maxElement = 0;
	double maxE =0;
	bool healthyCluster = true; 
	int nCellsLr2 = 0;
	std::map <int, IdentifierHash> currHashMap;
	std::map <int, float> currPhiMap;
	std::map <int, float> currEMap;
	std::map <IdentifierHash, float> hashEnergyMap;
	std::vector <IdentifierHash> cellsHashes;
	  
	for (ncelll2 = 0; thisCell != lastCell; ++thisCell) { // begin loop over cells

	  const CaloCell* aCell = *thisCell;

	  int sampling = aCell->caloDDE()->getSampling();

	  if (sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){ // keep only layer-2 cells of Lr2 cluster

	    if (ncelll2 % m_nphi_Lr2 == 0) Lr2_eta.push_back(aCell->eta()); // we record the eta once per row

	    float e_temp = aCell->e();
	    sume_lr2_old += e_temp;

	    int t_eta = ncelll2/m_nphi_Lr2;
	    Lr2_e_old.at(t_eta) += e_temp;    // for each eta we calculate the sum of energies in phi

	    currHashMap[ncelll2] = aCell->caloDDE()->calo_hash();  // contains hashes of the initial cluster
	    currPhiMap[ncelll2] = aCell->caloDDE()->phi();         // used for debugging
	    currEMap[ncelll2] = e_temp;                            // used for debugging
	    hashEnergyMap[aCell->caloDDE()->calo_hash()] = e_temp; // maps initial cluster cells energy to hashes for further reweighting
	    cellsHashes.push_back(aCell->caloDDE()->calo_hash());
	      
	    if (maxE < e_temp) {
	      maxE = e_temp;
	      maxElement = ncelll2;
	    } 
	      
	    ncelll2++;
	  }
	} // end loop over cells
	nCellsLr2 = ncelll2;

	ATH_MSG_DEBUG( "There are " << nCellsLr2 << " layer-2 cells in the cluster." );
	if (isBarrel) ATH_MSG_DEBUG( "Highest energy cell in the cluster (standard method): " << newClu->energy_max(CaloCell_ID::EMB2) );
	else          ATH_MSG_DEBUG( "Highest energy cell in the cluster (standard method): " << newClu->energy_max(CaloCell_ID::EME2) );
	  
	ATH_MSG_DEBUG( "Seed eta coordinate from cluster: " << egamma->caloCluster()->eta0() );
	ATH_MSG_DEBUG( "Seed eta coordinate corrected by hand: " << eta0 );
	//ATH_MSG_DEBUG( "Seed eta coordinate corrected by getDetPosition: " << etaCal );
	  
	//ATH_MSG_DEBUG( "Hottest cell eta coordinate from cluster: " << etamaxClus );
	//ATH_MSG_DEBUG( "Hottest cell eta cluster corrected by getDetPosition: " << etamaxClusCalo );
	//ATH_MSG_DEBUG( "Hottest cell eta cluster corrected by hand: " << etamaxClus + etaCalo - egamma->caloCluster()->eta());
	ATH_MSG_DEBUG( "maxElement cell eta coordinate: " << oldCellContainer->findCell(currHashMap.find(maxElement)->second)->eta() );
	  
	ATH_MSG_DEBUG( "Seed phi coordinate from cluster: " << egamma->caloCluster()->phi0() );
	ATH_MSG_DEBUG( "Seed phi coordinate corrected by hand: " << phi0 );
	//ATH_MSG_DEBUG( "Seed phi coordinate corrected by getDetPosition: " << phiCal );

	//ATH_MSG_DEBUG( "Hottest cell phi cluster corrected by hand: " << phimaxClus + phiCalo - egamma->caloCluster()->phi());
	//ATH_MSG_DEBUG( "Hottest cell phi coordinate from cluster: " << phimaxClus );
	//ATH_MSG_DEBUG( "Hottest cell phi cluster correcred by getDetPosition: " << phimaxClusCalo );
	ATH_MSG_DEBUG( "maxElement cell phi coordinate: " << currPhiMap.find(maxElement)->second );

	ATH_MSG_DEBUG( "Self-calculated highest energy cell number: " << maxElement << " energy is: " << maxE );


	// check that new Lr2 cluster is valid. If not, go to next candidate
	if( nCellsLr2 == 0 ) {
	  egamma->auxdecor<int>(Form("%dx%dClusterExist", m_neta_Lr2, m_nphi_Lr2))= 0;
	  delete egcCloneLr2;
	  delete egcCloneLr1;
	  ATH_MSG_DEBUG( "Candidates has 0 cells in layer-2, skipping" );
	  continue;
	}
	

	IdentifierHash clusterMap[m_neta_Lr2][m_nphi_Lr2]; //7x11 map of cells around the hottest cell, contains cells hashes
	// replace these magic numbers by constants: 77 = 7*11 = number of cells in 7x11 clusters
	// 38 = 77/2 = cell exacly in the middle of the cluster
	const int nCellsLr2Healthy = m_neta_Lr2*m_nphi_Lr2; // 7x11=77
	const int seedCellLr2Healthy = nCellsLr2Healthy/2; // 38
	if ( (maxElement == seedCellLr2Healthy+1) &&  (nCellsLr2 == nCellsLr2Healthy) )
	  ATH_MSG_DEBUG( "Right shift!" );

	if ( (maxElement == seedCellLr2Healthy-1) &&  (nCellsLr2 == nCellsLr2Healthy) )
	  ATH_MSG_DEBUG( "Left shift!" );

	if ( (maxElement == seedCellLr2Healthy) &&  (nCellsLr2 == nCellsLr2Healthy) )
	  ATH_MSG_DEBUG( "Center!" );

	if ( (maxElement != seedCellLr2Healthy) ||  (nCellsLr2 != nCellsLr2Healthy) ) {

	  ATH_MSG_DEBUG( "This seems not to be a nice healthy cluster. Getting cluster map by hand" );

	  healthyCluster = 0;
	  for (int i = 0; i < m_neta_Lr2; i++) {
	    for (int j = 0; j < m_nphi_Lr2; j++) {
	      clusterMap[i][j] = 0;         
	    }
	  }
	
	  int seedEta = m_neta_Lr2/2; // 3: in the middle of 0..6
	  int seedPhi = m_nphi_Lr2/2; // 5: in the middle of 0..10
	  clusterMap[seedEta][seedPhi] = currHashMap.find(maxElement)->second;
	  ATH_MSG_DEBUG( "Center has hash: " << clusterMap[seedEta][seedPhi] << " and energy: " << currEMap.find(maxElement)->second );
	   
	  getClusterMap (seedEta, seedPhi, clusterMap, oldCellContainer);  // fills the map array with cells hashes 
	  ATH_MSG_DEBUG( "Hashes of cluster cells:" );
	  std::ostringstream os;
	  for (int i = 0; i < m_neta_Lr2; i++) {
	    for (int j = 0; j < m_nphi_Lr2; j++) {
	      os << clusterMap[i][j] << " " ;
	    }
	  }
	  ATH_MSG_DEBUG( os.str() );
	   
	  ATH_MSG_DEBUG( "Energies of cluster cells:" );
	  os.str("");
	  os.clear();
	  for (int i = 0; i < m_neta_Lr2; i++) {
	    for (int j = 0; j < m_nphi_Lr2; j++) {
	      if (clusterMap[i][j]) os << (oldCellContainer->findCell(clusterMap[i][j]))->e() << " " ;
	    }
	  }
	  ATH_MSG_DEBUG( os.str() );

	  ATH_MSG_DEBUG( "Hashes of layer-2 cluster cells (avoiding duplicates):" );
	  os.str("");
	  os.clear();
	  for (int i = 0; i < nCellsLr2; i++){
	    os << " | " <<  currHashMap.find(i)->second << " | " ;
	    if ( (currHashMap.find(i+1)->second != currHashMap.find(i)->second + 1) )	{
	      ATH_MSG_DEBUG( os.str() );
	      os.str("");
	      os.clear();
	    }
	  }

	  ATH_MSG_DEBUG( "Phi of layer-2 cluster cells (avoiding duplicates)");
	  os.str("");
	  os.clear();
	  for (int i = 0; i < nCellsLr2; i++) {
	    os << " | " <<  currPhiMap.find(i)->second << " | " ;
	    if ( abs(currPhiMap.find(i+1)->second - currPhiMap.find(i)->second) > 0.03 ) {
	      ATH_MSG_DEBUG( os.str() );
	      os.str("");
	      os.clear();
	    }
	  }

	  ATH_MSG_DEBUG( "E of layer-2 cluster cells (avoiding duplicates)");
	  os.str("");
	  os.clear();
	  for (int i = 0; i < nCellsLr2; i++) {
	    os << " | " <<  currEMap.find(i)->second << " | " ;
	    if ( abs(currPhiMap.find(i+1)->second - currPhiMap.find(i)->second) > 0.03 ) {
	      ATH_MSG_DEBUG( os.str() );
	      os.str("");
	      os.clear();
	    }
	  }
	}
	
	// now retrieve the 2D histogram for the layer-2 cell reweighting, based on the eta of the cluster
	// and print some debug information
	temp_bin = "Lr2eta"+m_setaBins[etaBin-1]+m_setaBins[etaBin];
	ATH_MSG_DEBUG( "Eta bin is:" << temp_bin );
	temp_ratio2D = m_hratio2D.find(temp_bin);
	ATH_MSG_DEBUG( "Largest correction is: " << temp_ratio2D->second->GetBinContent(6,4) ); // 6,4 = center of 1..11 x 1..7
	ATH_MSG_DEBUG( "Max element number: " << maxElement );
	ATH_MSG_DEBUG( "Hottest element hash: " << currHashMap.find(maxElement)->second );
	ATH_MSG_DEBUG( "notInContainer vector status: " << notInContainer[currHashMap.find(maxElement)->second] );

	double newClusterEnergy = 0;
	double normCoefficient = 0;

	ATH_MSG_DEBUG( "Starting reweighting...  " );

	if (healthyCluster) {
	  // here the "usual" reweighting is performed, where we don't need any mapping. The cluster is a full 7x11 cluster with hottest cell in the center
	  ATH_MSG_DEBUG( "This seems to be a nice healthy cluster.  " );

	  for (int i = 0; i < m_neta_Lr2; i++) {
	    for (int j = 0; j < m_nphi_Lr2; j++) {

	      int linearIndex = j + m_nphi_Lr2*i;   
	      if (oldCellContainer->findCell(cellsHashes[linearIndex])) { 
		double weight = (temp_ratio2D->second)->GetBinContent(j+1, i+1);
		if (fabs(weight)>1e-9) {
		  CaloCell* newCell = (oldCellContainer->findCell(cellsHashes[linearIndex]))->clone();
		  double oldEnergy = newCell->e();
		  double correction = sume_lr2_old*weight;
		  double newEnergy = oldEnergy + correction;
		  ATH_MSG_DEBUG( "eta = " << i << " phi = " << j << " cell energy is: " << oldEnergy );
		  ATH_MSG_DEBUG( "Correction is: " << correction << " reweighted cell energy is: " << newEnergy );
		  newCell->setEnergy(newEnergy);
		  newClusterEnergy += newEnergy;
		   
		  if(notInContainer[newCell->caloDDE()->calo_hash()]){
		    notInContainer[newCell->caloDDE()->calo_hash()] = false;
		    newCellContainer->push_back(newCell);
		  }
		  else delete newCell;
		}
		else ATH_MSG_DEBUG( "Something's wrong with the reweighting coefficient" );
	      }
	      else ATH_MSG_DEBUG( "This cell is absent in the initial cluster" );
	    } // end inner loop over cells
	  } // end outer loop over cells
	  assert(fabs(newClusterEnergy)>1e-5);
	  normCoefficient = sume_lr2_old/newClusterEnergy;
	  ATH_MSG_DEBUG( "Old cluster energy: " << sume_lr2_old << " reweighted cluster energy is: " << newClusterEnergy << " normalization coefficient is: " << normCoefficient );
	  // For a normal 7x11 cluster the weights are designed so that the new energy after reweighting is the same as the old one before reweighting so no need to normalise (normCoefficient should be 1)
	} // end if (healthyCluster)
	else {
	  // here we consider the case when either the cluster hasn't 7x11=77 cells or the hottest cell is displaced from center, and the mapping is used
	  ATH_MSG_DEBUG( "Curing the sick cluster. Doing what we can...  " );

	  // loop over the cells, get the old energy and the weight, calculate the new energy and store in hashEnergyMap
	  for (int i = 0; i < m_neta_Lr2; i++) {
	    for (int j = 0; j < m_nphi_Lr2; j++) {
	      if (clusterMap[i][j]) {
		//the line below makes sure that the reweighted cell is present in the original cluster, so we don't reweight some neighbouring cells
		if (hashEnergyMap.find(clusterMap[i][j]) != hashEnergyMap.end()) {
		  double oldEnergy = (oldCellContainer->findCell(clusterMap[i][j]))->e();
		  double correction = sume_lr2_old*(temp_ratio2D->second)->GetBinContent(j+1, i+1);
		  double newEnergy = oldEnergy + correction;
		  ATH_MSG_DEBUG( "eta = " << i << " phi = " << j << " cell energy is: " << oldEnergy );
		  if ((temp_ratio2D->second)->GetBinContent(j+1, i+1)){
		    ATH_MSG_DEBUG( "Correction is " << correction << " reweighted cell energy is: " << newEnergy );
		    hashEnergyMap[clusterMap[i][j]] = newEnergy;
		  }
		  else ATH_MSG_DEBUG( "Something's wrong with the reweighting coefficient" );
		}
		else ATH_MSG_DEBUG( "This cell is absent in the initial cluster" );
	      }
	      else ATH_MSG_DEBUG( "Map element is zero" );
	    }
	  }
	  
	  // now, we need to renormalise the energies so that the total cluster energy is unchanged
	  // so first, we compute the sum of the new energies of the cells
	  ATH_MSG_DEBUG( "Calculating the reweighted cluster energy" );
	  std::map<IdentifierHash, float>::iterator it;
	  for (it = hashEnergyMap.begin(); it != hashEnergyMap.end(); it ++ ) {
	    newClusterEnergy += it->second;
	  }
	  assert(fabs(newClusterEnergy)>1e-5);

	  // then we compute the normalisation coefficient
	  normCoefficient = sume_lr2_old/newClusterEnergy;
	  ATH_MSG_DEBUG( "Old cluster energy: " << sume_lr2_old << " reweighted cluster energy is: " << newClusterEnergy << " normalization coefficient is: " << normCoefficient );

	  // finally scale the reweighted energies by this coefficient, create the new cells with these new energies, and add them to the container
	  for (it = hashEnergyMap.begin(); it != hashEnergyMap.end(); it ++ ) {
	    CaloCell* newCell = (oldCellContainer->findCell(it->first))->clone();
	    newCell->setEnergy(it->second*normCoefficient);
	    if(notInContainer[newCell->caloDDE()->calo_hash()]){
	      notInContainer[newCell->caloDDE()->calo_hash()] = false;
	      newCellContainer->push_back(newCell);
	    }
	    else delete newCell;   
	  }    
	}
      } // end of if (egcCloneLr2)
      //
      // end of reweighting of layer-2 cell energies
      //


      //
      // reweighting of layer-1 cell energies
      //
      newClu = egcCloneLr1;
      auto thisCell = newClu->cell_begin();
      auto lastCell = newClu->cell_end();
      float sume_lr1_old = 0.;
      int ncelll1(0);
      std::vector<float>  Lr1_e_all;
      std::vector<float>  Lr1_eta, Lr1_eta_all,  Lr1_e_old;
      std::vector<int> Lr1_eta_index;
      
      // we need the lines below (i.e. the Lr2 vectors) because Lr1 cells number controlled by Lr2 index - a bit tricky...
      std::vector<float>  Lr2_e_temp(11,0.); // why 11 and not 7?
      Lr2_eta_index.clear();
      Lr2_eta.clear();
      int lr2phi;
      if (nStrips == 5)
	lr2phi = 5; // 5x5 cluster
      else
	lr2phi = 7; // 3x7 cluster

      // first, compute sum of old energies in sume_lr1_old
      for (ncelll1 = 0, ncelll2 =0 ;thisCell != lastCell; ++thisCell) {
	const CaloCell* aCell = *thisCell;
	float e_temp = aCell->e();
	int sampling = aCell->caloDDE()->getSampling();
	if(sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){ // compute number of cells and energies in layer-2
	  if(ncelll2 % lr2phi == 0) Lr2_eta.push_back(aCell->eta()); // store eta of each row in Lr2_eta vector

	  int t_eta = ncelll2/lr2phi;
	  Lr2_e_temp.at(t_eta) += e_temp; // store vector with energy of each row 

	  ncelll2++;
	}		
	if(sampling== CaloCell_ID::EMB1 || sampling== CaloCell_ID::EME1 ){ // compute number of cells and energies / eta in layer-1
	  sume_lr1_old += e_temp; // total sum of old energies in layer-1 before reweighting
	  Lr1_e_all.push_back(e_temp); // store each energy in vector Lr1_e_all
	  Lr1_eta_all.push_back(aCell->eta()); // store each eta in vector Lr1_eta_all
	  ncelll1++;
	}
      }
      
      ATH_MSG_DEBUG ("Lr1 reweighted cell number: layer-2 = " << ncelll2 << ", layer-1 = " << ncelll1 );
      if(ncelll1 == 0 || ncelll2 == 0) {
	egamma->auxdecor<int>(Form("%dx%dClusterExist", m_neta_Lr2, m_nphi_Lr2))= 0;
	delete egcCloneLr2;
	delete egcCloneLr1;
	continue;
      }	

      // sum over 1 or 2 phi strips
      int Lr1_phi = 2;
      if(ncelll1 < 1.5*(float)(etaSizeL1*nStrips)) 		//1.5 to judge 1 or 2 phi strip
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

      // second, compute the normalisation factor for the layer-1 reweighted cell energies
      std::vector<float> Lr1_e_new(Lr1_e_old);
      std::vector<float> Lr1_function;
      float sume_lr1_new = 0.;
      std::vector<int> neta_index;
      setIndexLr2(&Lr2_eta_index, &Lr2_eta,  &Lr2_e_temp, clusterSeedEta);
      setIndexLr1(&Lr1_eta_index, &neta_index, &Lr1_eta, &Lr1_e_old, &Lr2_eta, clusterSeedEta); // Lr1 cells number controlled by Lr2 index. A bit tricky...
      // loop over the layer-1 eta cells to calculate the new energy 
      for(unsigned int neta = 0; neta < Lr1_eta_index.size(); neta++) {
	// get the reweighting factor
	temp_bin = "Lr1eta"+m_setaBins[neta_index.at(neta)-1]+m_setaBins[neta_index.at(neta)];
	temp_ratio = m_hratio.find(temp_bin); 
	// get the correction and the new energy
	double correction = sume_lr1_old*((temp_ratio->second)->GetBinContent(Lr1_eta_index.at(neta)));
	double newEnergy = Lr1_e_old.at(neta) + correction;
	Lr1_e_new.at(neta) = newEnergy;
	sume_lr1_new += newEnergy;
      }
      assert(fabs(sume_lr1_new)>1e-5);
      // from the new energy calculate the normalisation factors
      float knormalize = sume_lr1_old/sume_lr1_new;
      for(unsigned int neta = 0; neta < Lr1_eta_index.size(); neta++) {
	if (fabs(Lr1_e_old.at(neta)) < 1e-5)
	  Lr1_function.push_back( 1. );
	else 
	  Lr1_function.push_back( knormalize * Lr1_e_new.at(neta) / Lr1_e_old.at(neta) );					
      }			
      
      // third, set the new layer-1 cell energies and add to new cell container
      thisCell = newClu->cell_begin();
      for (ncelll1 = 0; thisCell != lastCell; ++thisCell) {
	CaloCell* newCell = thisCell->clone();
	int sampling = newCell->caloDDE()->getSampling();
	if(sampling== CaloCell_ID::EMB1 || sampling== CaloCell_ID::EME1 ){
	  int t_eta = ncelll1/Lr1_phi;
	  float e_temp = newCell->e();
	  newCell->setEnergy(e_temp * Lr1_function.at(t_eta));
	  ncelll1++;
	  if(notInContainer[newCell->caloDDE()->calo_hash()]){
	    notInContainer[newCell->caloDDE()->calo_hash()] = false;
	    newCellContainer->push_back(newCell);
	  }
	  else 
	    delete newCell;
	}
      }
      //
      // end of reweighting of layer-1 cell energies
      //
      
      // delete temporary objects from memory
      if(egcCloneLr1)	delete egcCloneLr1;
      if(egcCloneLr2)	delete egcCloneLr2;
      
    } // end for loop on egamma objects

  } // end for loop on egamma containers
  

  //
  // add to new cell container the remaining cells (not reweighted)
  //
  for (auto thisCell = oldCellContainer->begin(); thisCell != oldCellContainer->end(); ++thisCell) {
    CaloCell* newCell = (*thisCell)->clone();
    if (notInContainer[newCell->caloDDE()->calo_hash()]) {
      notInContainer[newCell->caloDDE()->calo_hash()] = false;
      newCellContainer->push_back(newCell);
    }
    else
      delete newCell;
  }


  //
  // properly finalize the cell container (with CaloCellContainerFinalizerTool) and store new cell container in SG
  //
  ATH_CHECK(m_tool2->process(newCellContainer.get()));
  ATH_CHECK(evtStore()->record( std::move(newCellContainer), m_newCellContainerName.c_str()));


  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CellReweight::finalize()
{
  ATH_MSG_INFO ( "CellReweight::finalize()" );
  return StatusCode::SUCCESS;
}

// getClusterMap: fill map[eta][phi] with hashes of cells in container cellCont that are neighbours to cell with position seedEta, seedPhi (eta, phi in units of cell index inside cluster: 0..6, 0..10)
// note: I have commented the ATH_MSG_DEBUG statements otherwise they are too many and reach the limit (500) and other debug messages will be suppressed from output
void DerivationFramework::CellReweight::getClusterMap(int seedEta, int seedPhi,
						      IdentifierHash (*map)[11],
						      const CaloCellContainer* cellCont) const {
  const int nNeighbours = 4;

  const LArNeighbours::neighbourOption option[nNeighbours] = {
    LArNeighbours::nextInPhi,
    LArNeighbours::prevInEta, 
    LArNeighbours::prevInPhi, 
    LArNeighbours::nextInEta};

  std::string direction[nNeighbours] = {"right", "up", "left", "down"};

  int stepEtaSeed[nNeighbours] = {0, -1, 0, 1}; 

  int stepPhiSeed[nNeighbours] = {1, 0, -1, 0}; 

  if (m_caloID) {
    for (int n = 0; n < nNeighbours; n++) {
      std::vector<IdentifierHash> theNeighbours;		                              // vector to be filled with hashes that point to the neighbours
      IdentifierHash hash = map[seedEta][seedPhi];
      //ATH_MSG_DEBUG( "starting recursion, eta = " << seedEta << " phi = " << seedPhi << " hash = " << hash);
      if ( (seedPhi+stepPhiSeed[n] >= 0) && (seedPhi+stepPhiSeed[n] < m_nphi_Lr2) && 
	   (seedEta+stepEtaSeed[n] >= 0) && (seedEta+stepEtaSeed[n] < m_neta_Lr2) ) {         //setting the limits
	if (!map[seedEta+stepEtaSeed[n]][seedPhi+stepPhiSeed[n]])   {                         //checking if this element of the map was filled already
	  //ATH_MSG_DEBUG( "getting the " << direction[n] <<" neighbour" );
	  m_caloID->get_neighbours(hash, option[n], theNeighbours);                           //getting the neighbour (vector size = 1)
	  if (theNeighbours.size()) {  					                      //checking if vector exists (sometimes it doesn't)
	    if (theNeighbours[0]) { 										
	      //ATH_MSG_DEBUG( direction[n] << " neighbour's hash is:" << theNeighbours[0] );
	      if (cellCont->findCell(theNeighbours[0])) {                                     //checking if cell with the given address exists
		//ATH_MSG_DEBUG ("and this cell exists, going " << direction[n] <<"...");
		map[seedEta+stepEtaSeed[n]][seedPhi+stepPhiSeed[n]] = theNeighbours[0];       //filling the map
		getClusterMap(seedEta+stepEtaSeed[n],seedPhi+stepPhiSeed[n], map, cellCont);  //recurring the function with the new cell as a new seed
	      }
	      //else ATH_MSG_DEBUG( "this cell does not exist" );
	    }
	  }
	}
      }	
    }
  }
}

unsigned int DerivationFramework::CellReweight::locateMid(std::vector<float> * _eta, float m_eta ) const 
{
  float diff = 1000.;
  unsigned int idx = 0;
  for (unsigned int i = 0 ; i < _eta->size(); i++){
    float t_diff = fabs(_eta->at(i) - m_eta);	
    if (t_diff < diff) {
      idx = i; 
      diff = t_diff;
    }
  }
  return idx;
}

unsigned int DerivationFramework::CellReweight::locateMidE(std::vector<float> * _e) const
{
  float maxe = -1.;
  unsigned int idx = 0;
  for (unsigned int i = 0 ; i < _e->size(); i++){
    if (_e->at(i) > maxe) {
      idx = i;
      maxe = _e->at(i);
    }
  }
  return idx;
}

unsigned int DerivationFramework::CellReweight::getEtaBin(float t_eta) const
{
  for (unsigned int iEta = 1; iEta<m_nEtaBins; iEta++) {
    if ( (fabs(t_eta) <= m_etaBins[iEta]) && (fabs(t_eta) > m_etaBins[iEta-1]) ) {
      return (iEta);
    }
  }
  return (m_nEtaBins-1);
}

unsigned int DerivationFramework::CellReweight::getEtaSize(int n_eta) const
{
  unsigned int _etasize = 8;
  switch(n_eta) {
  case 9 : { _etasize = 8; break;} // GM: should be 1 here? L1 granularity in crack = 0.025
  case 12 : { _etasize = 6; break;}
  case 13 : { _etasize = 4; break;}
  case 14 : { _etasize = 4; break;}
  case 15 : { _etasize = 8; break;} // GM: should be 1 here? L1 granularity in crack = 0.025
  }
  return _etasize;
}

bool SortByEtaMinus(const std::pair<float, int > &pair1, const std::pair<float, int > &pair2){ // hight to low 
  return(pair1.first > pair2.first);
}

bool SortByEtaPlus(const std::pair<float, int > &pair1, const std::pair<float, int > &pair2){ //low to high
  return(pair1.first < pair2.first);
}

void DerivationFramework::CellReweight::setIndexLr2(std::vector<int> * index,  std::vector<float> *_eta, std::vector<float> *_lr2e, float m_eta) const
{
  std::vector<std::pair<float, int >> eta_index;
  std::vector<float> new_eta;
  std::vector<int> old_index;
  index->clear();

  for (unsigned int i = 0; i< _eta->size(); i++) {
    eta_index.push_back(std::make_pair(_eta->at(i), i));
  }

  if (m_eta<0) {
    std::sort(eta_index.begin(),eta_index.end(), SortByEtaMinus);
  }
  else {
    std::sort(eta_index.begin(),eta_index.end(), SortByEtaPlus);
  }

  for (auto itr = eta_index.begin(); itr<eta_index.end(); itr++) {
    new_eta.push_back(itr->first);
    index->push_back(0);
  }

  int newmid = locateMidE(_lr2e);
  for (unsigned int i =0; i<new_eta.size(); i++) {
    if (((eta_index.at(i)).second) == newmid) {
      newmid = i; 
      break;
    }
  }

  int etaSize = _eta->size();
  switch(etaSize){
  case 7: {newmid = 3; break;}
  case 5: {newmid = 2; break;}
  case 3: {newmid = 1; break;}
  default: break;
  }
  for (unsigned int i  = 0; i<new_eta.size(); i++) {
    index->at((eta_index.at(i)).second) = i - newmid + 4;
  }
  eta_index.clear();
  new_eta.clear();
  old_index.clear();
  return;
}


void DerivationFramework::CellReweight::setIndexLr1(std::vector<int> * index, std::vector<int> * neta_index, std::vector<float> *_eta, std::vector<float> *_e, std::vector<float> *_etaLr2, float m_eta) const
{
  std::vector<std::pair<float, int >> eta_index;
  std::vector<float> new_eta;
  int neta_0;
  index->clear();
  neta_index->clear();
  //	now using hottest cell!
  m_eta = _eta->at((int)locateMidE(_e));
  for (unsigned int i = 0; i< _eta->size(); i++) {
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
  neta_0 = getEtaBin(_etaLr2->at(0));
  int _neta =  getEtaBin(m_eta);
  int etaSize0 = getEtaSize(neta_0);
  int etaSize = getEtaSize(_neta);
  int newmid = locateMid(&new_eta, m_eta);
  int nchange = 0;
  int nchange2 = 0;
  int t_neta = 0;
  //Lr1 cells number controlled by Lr2 index
  //a bit tricky...
  for(unsigned int i = 0; i<_etaLr2->size(); i++,nchange2++){
    t_neta = getEtaBin(_etaLr2->at(i));
    if(t_neta != neta_0) break;
  }
  nchange = etaSize0*nchange2;		
  int etaSize1 = getEtaSize(neta_0+1);
  if(nchange < ((int)new_eta.size()-1)){
    if(neta_0 == _neta){
      for(int i  = 0; i<nchange; i++){
	index->at((eta_index.at(i)).second) = i - newmid + (2*etaSize+1);
	neta_index->at((eta_index.at(i)).second) = neta_0;
      }
      float tk = ((float)(nchange-1.0-newmid))/((float)etaSize0);
      int extra_strip =(int) (tk*((float)etaSize1)+0.5);
      for(int i  = nchange; i<(int)index->size(); i++){
	neta_index->at((eta_index.at(i)).second) = neta_0+1;
	index->at((eta_index.at(i)).second) = i - nchange + (2*etaSize1+1)+extra_strip+1;
      }				
    }		
    else{
      for(int i  = nchange; i<(int)index->size(); i++){
	index->at((eta_index.at(i)).second) = i - newmid + (2*etaSize+1);
	neta_index->at((eta_index.at(i)).second) = neta_0+1;
      }
      float tk = ((float)(newmid-nchange))/((float)etaSize1);
      int extra_strip =(int) (tk*((float)etaSize1)+0.5);
      for(int i  = nchange-1; i>= 0; i--){
	neta_index->at((eta_index.at(i)).second) = neta_0;
	index->at((eta_index.at(i)).second) = i - nchange + (2*etaSize0+1)-extra_strip;
      }				
    }		
  }
  else {
    for(unsigned int i  = 0; i<new_eta.size(); i++){
      index->at((eta_index.at(i)).second) = i - newmid + (2*etaSize+1);
      neta_index->at((eta_index.at(i)).second) = neta_0;
    }				
  }
  return;
}
