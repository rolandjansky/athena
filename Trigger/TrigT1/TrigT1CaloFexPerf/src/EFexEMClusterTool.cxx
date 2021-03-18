// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * Name    : EFexEMClusterTool.cxx
 * PACKAGE : Trigger/TrigT1/TrigT1CaloFexPerf
 * AUTHOR  : Denis Oliveira Damazio
 * PURPOSE : emulate the eFex EM algorithm for phase 1 L1Calo (default clustering)
 */

#include "EFexEMClusterTool.h"
#include "TileEvent/TileCellContainer.h"


LVL1::EFexEMClusterTool::EFexEMClusterTool(const std::string& type, const std::string& name, const IInterface* parent)
   : AthAlgTool(type, name, parent)
{
   declareProperty("CleanCellContainer", m_useProvenance=true);
   declareProperty("QualBitMask", m_qualBitMask=0x40);

   // baseline selection properties
   declareProperty("ClusterEnergyThreshold", m_clustET_thresh = 28., "Cluster energy threshold for baseline selection");
   declareProperty("EnergyThresholdToApplyIsolation", m_clustET_NoIso_thresh = 60., "Cluster energy above which no isolation cut is applied for baseline selection");
   declareProperty("REtaThreshold", m_REta_thresh = 0.12, "Reta cut for baseline selection");
   declareProperty("RHadThreshold", m_RHad_thresh = 0.16, "Rhad cut for baseline selection");
   declareProperty("L1WidthThreshold", m_L1Width_thresh = 0.02, "L1Width cut for baseline selection");
   declareProperty("EtaThresholdToApplyL1Width", m_eta_dropL1Width = 2.3, "Eta outside of which no L1Width cut is applied for baseline selection");

   // loose selection properties
   declareProperty("UseTileCells", m_use_tileCells = false);
   declareProperty("NominalDigitizationValue", m_nominalDigitization = 25.);
   declareProperty("NominalNoiseThreshold", m_nominalNoise_thresh = 100.);
   declareProperty("TileNoiseThreshold", m_tileNoise_tresh = 100.);
   declareProperty("EtaWidthTDRCluster", m_etaWidth_TDRCluster = 3);
   declareProperty("PhiWidthTDRCluster", m_phiWidth_TDRCluster = 2);
   declareProperty("EtaWidthWStotIsolation", m_etaWidth_wstotIsolation = 5);
   declareProperty("PhiWidthWStotIsolation", m_phiWidth_wstotIsolation = 3);
	declareProperty("EtaEMWidthRHadIsolation", m_etaEMWidth_RHadIsolation = 3);  // 1 for a 1-eta-tower had cluster, 5 for 2-tower, 9 for 3-tower
	declareProperty("PhiEMWidthRHadIsolation", m_phiEMWidth_RHadIsolation = 3);
   declareProperty("EtaWidthREtaIsolationDenominator", m_etaWidth_REtaIsolation_den = 7);
   declareProperty("PhiWidthREtaIsolationDenominator", m_phiWidth_REtaIsolation_den = 3);
   declareProperty("EtaWidthREtaIsolationNumerator", m_etaWidth_REtaIsolation_num = 3);
   declareProperty("PhiWidthREtaIsolationNumerator", m_phiWidth_REtaIsolation_num = 2);
   declareProperty("ClusterEnergyThresholdLooseEFEX", m_clustET_looseAlg_thresh = 10.);
   declareProperty("EtaHadWidthRHadIsolation", m_etaHadWidth_RHadIsolation = 9);  // 1 for a 1-eta-tower had cluster, 5 for 2-tower, 9 for 3-tower
	declareProperty("PhiHadWidthRHadIsolation", m_phiHadWidth_RHadIsolation = 3);
}

std::vector<LVL1::EFexEMClusterTool::AlgResult>
LVL1::EFexEMClusterTool::clusterAlg(bool applyBaselineCuts,
                                    const CaloConstCellContainer* scells, const xAOD::TriggerTowerContainer* TTs, 
                                    const CaloCell_SuperCell_ID* idHelper, const TileID* tileIDHelper,
                                    const CaloConstCellContainer* tileCellCon) const
{
   std::vector<AlgResult> baselineClusters;
   for (auto & cluster : looseAlg(scells, TTs, idHelper, tileIDHelper, tileCellCon) ) {

      // cluster E_T
      cluster.passClusterEnergy = cluster.clusterET >= m_clustET_thresh; // if ET cut passes

      // R_eta
      cluster.passREta = cluster.rEta <= m_REta_thresh || // if reta cut passes
                         cluster.clusterET > m_clustET_NoIso_thresh; // or ET above threshold where any isolation is applied

      // R_had
      cluster.passRHad = cluster.rHad <= m_RHad_thresh || // if rhad cut passes
                         cluster.clusterET > m_clustET_NoIso_thresh; // or ET above threshold where any isolation is applied

      // Wstot
      cluster.passWstot = cluster.l1Width < m_L1Width_thresh ||  // if cut passes
                          std::abs(cluster.eta) > m_eta_dropL1Width || // or eta outside range where cut is applied
                          cluster.clusterET > m_clustET_NoIso_thresh; // or ET above threshold where any isolation is applied

      bool passBaseLineSelection = cluster.passClusterEnergy &&
                                   cluster.passRHad && 
                                   cluster.passREta &&
                                   cluster.passWstot;

      if (applyBaselineCuts and not passBaseLineSelection ) {
         continue;
      }
      
      baselineClusters.push_back(cluster);
   }
   return baselineClusters;
}

std::vector<LVL1::EFexEMClusterTool::AlgResult>
LVL1::EFexEMClusterTool::looseAlg(const CaloConstCellContainer* SCs, const xAOD::TriggerTowerContainer* TTs, 
                                  const CaloCell_SuperCell_ID* idHelper, const TileID* tileIDHelper,
                                  const CaloConstCellContainer* tileCellCon ) const
{
   std::vector<AlgResult> result;
   // Loops through and find L2 SCs that are local maxes and adds to list of local maxes if cluster ET is at least 10GeV
   std::vector<const CaloCell*> potentialCentres;
   for (auto ithCell : *SCs) {
      if ( !( std::abs(CaloCellET(ithCell, m_nominalDigitization, m_nominalNoise_thresh)) > 0) ) {
         continue;
      }
      Identifier ithID = ithCell->ID();
      if (idHelper->sampling(ithID) != 2) {
         continue;
      }

      if (idHelper->sub_calo(ithID) != 0) {
         continue;
      }

      bool inEfexCoverage = false;
      if ( std::abs(idHelper->pos_neg(ithID)) < 3) {
         inEfexCoverage = true;
      }

      if (!inEfexCoverage) {
         continue;
      }

      if (localMax(SCs, ithCell, idHelper, m_nominalDigitization, m_nominalNoise_thresh)) {
         potentialCentres.push_back(ithCell);
      }
   }

   // Looops through the local maxes and skips the less energetic ones that belong to the same TT
   for (auto ithCell : potentialCentres){
      bool useSC = true;
      for (auto jthCell : potentialCentres){
         if (jthCell == ithCell) continue;
         if (!SameTT(ithCell, jthCell, idHelper)) continue;
         float ithEt = CaloCellET(ithCell, m_nominalDigitization, m_nominalNoise_thresh);
         float jthEt = CaloCellET(jthCell, m_nominalDigitization, m_nominalNoise_thresh);
         if (ithEt > jthEt) continue;
         if (ithEt == jthEt && ithCell->eta() > jthCell->eta()) continue;
         useSC = false; 
      }
      /// Now we can do the minimum cluster ET test
      float clustET = EMClusET(ithCell, m_etaWidth_TDRCluster, m_phiWidth_TDRCluster, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh)/1000.;
      if (clustET < m_clustET_looseAlg_thresh) useSC = false; 

      if (useSC) {
         float HadET = -999;
         float ithRHad = -1;
         float ithEta = ithCell->eta();
         float ithPhi = ithCell->phi();
         float ithREta = REta( ithCell, m_etaWidth_REtaIsolation_num, m_phiWidth_REtaIsolation_num, m_etaWidth_REtaIsolation_den,
                               m_phiWidth_REtaIsolation_den, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh); 
         if (!m_use_tileCells) {
            ithRHad = RHad(ithCell, m_etaEMWidth_RHadIsolation, m_phiEMWidth_RHadIsolation, SCs, TTs, idHelper, m_nominalDigitization, m_nominalNoise_thresh, HadET);
         } else {
            ithRHad = RHadTile(ithCell, m_etaEMWidth_RHadIsolation, m_phiEMWidth_RHadIsolation, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh, tileIDHelper, tileCellCon, m_tileNoise_tresh, HadET);
         }

         float ithL1Width    = L1Width( ithCell, m_etaWidth_wstotIsolation, m_phiWidth_wstotIsolation, SCs, 
                                        idHelper, m_nominalDigitization, m_nominalNoise_thresh);
         float L2ClusterET33 = L2clusET( ithCell, 3, 3, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh)/1e3;
         float L2ClusterET37 = L2clusET( ithCell, 7, 3, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh)/1e3;

         float ithREtaL12{-1};
         if (m_use_REtaL12) {
            ithREtaL12 = REtaL12(ithCell, m_etaWidth_REtaIsolation_num, m_phiWidth_REtaIsolation_num,
                                 m_etaWidth_REtaIsolation_den, m_phiWidth_REtaIsolation_den, SCs, idHelper,
                                 m_nominalDigitization, m_nominalNoise_thresh);
         }
         result.push_back(AlgResult{ithEta, ithPhi, clustET, ithREta, ithRHad, ithL1Width, HadET, L2ClusterET33, L2ClusterET37, ithREtaL12});
      }
   }
   return result;
}

/**
 * private algorithms
 */

float
LVL1::EFexEMClusterTool::CaloCellET(const CaloCell* const &inputCell, float digitScale, float digitThreshold) const
{
   if (inputCell==nullptr) return 0.;
   // Check that timing is correct 
   if ( m_useProvenance ) {
      bool correctProv = (inputCell->provenance() & m_qualBitMask);
      if (!correctProv) return 0.;
   }
   // Calculates the ET (before digitization)
   float inputCell_energy = inputCell->energy();
   float inputCell_eta = inputCell->eta();
   float inputCell_ET = inputCell_energy / cosh(inputCell_eta);
   // Check to see if negative ET values are allowed
   bool allowNegs = false;
   if (digitScale < 0.){
      digitScale = std::abs(digitScale);
      allowNegs = true;
   }
   if (inputCell_ET==0) return 0.;
   else if (digitScale==0) return inputCell_ET;
   if (allowNegs || inputCell_ET>0.){
      // Split up ET into magnitude & whether it's positive or negative
      float posOrNeg = inputCell_ET / std::abs(inputCell_ET);
      inputCell_ET = std::abs(inputCell_ET);
      // If no digitisation, return ET following noise cut
      if (digitScale == 0){
         if (inputCell_ET>digitThreshold) return inputCell_ET*posOrNeg;
         else return 0.;
      }
      // Apply digitization & then noise cut
      else {
         float divET = inputCell_ET / digitScale;
         int roundET = divET;
         float result = digitScale * roundET;
         if (digitThreshold == 0) return result*posOrNeg;
         else if (result >= digitThreshold) return result*posOrNeg;
         else return 0;
      }
   }
   else return 0.;
}

bool
LVL1::EFexEMClusterTool::SameTT(const CaloCell* inputCell1, const CaloCell* inputCell2, const CaloCell_SuperCell_ID* &idHelper) const
{
   const Identifier ID1 = inputCell1->ID();
   int phi1 = idHelper->phi(ID1);
   const Identifier ID2 = inputCell2->ID();
   int phi2 = idHelper->phi(ID2);
   if (phi1 != phi2) {
      return false;
   }
   int pn1 = idHelper->pos_neg(ID1);
   int pn2 = idHelper->pos_neg(ID2);
   if (pn1 != pn2) {
      return false;
   }
   // Is barrel
   if (abs(pn1)==1) {
      int reg1 = idHelper->region(ID1);
      int reg2 = idHelper->region(ID2);
      if (reg1 != reg2) {
         return false;
      }
      int etaDiv1 = idHelper->eta(ID1)/4;
      int etaDiv2 = idHelper->eta(ID2)/4;
      if (etaDiv1 == etaDiv2) {
         return true;
      }
      else {
         return false;
      }
   }
   // OW
   else if (abs(pn1)==2){
      int reg1 = idHelper->region(ID1);
      int reg2 = idHelper->region(ID2);
      int eta1 = idHelper->eta(ID1);
      int eta2 = idHelper->eta(ID2);
      if ((reg1 == 0 && reg2 == 1 && eta2 < 3 ) || (reg2 == 0 && reg1 == 1 && eta1 < 3 )) return true;
      else {
         if (reg1 != reg2) return false;
         int etaDiv1 = (idHelper->eta(ID1) - 3)/4;
         int etaDiv2 = (idHelper->eta(ID2) - 3)/4;
         if (etaDiv1 == etaDiv2) return true;
         else return false;   
      }
   }  
   else return false;
}

bool
LVL1::EFexEMClusterTool::localMax(const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell,
                                  const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold) const
{
   return localMax(inputContainer, inputCell, 0, idHelper, digitScale, digitThreshold);
}

bool
LVL1::EFexEMClusterTool::localMax(const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell, int numOthers,
                                  const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold) const 
{
   if (inputCell == nullptr) return false;
   // Get ID info
   const Identifier inputID = inputCell->ID();
   const int sub_calo = idHelper->sub_calo(inputID);
   const int pos_neg = idHelper->pos_neg(inputID);
   if (!(sub_calo == 0 || sub_calo == 1) || !(abs(pos_neg) < 4)){
      ATH_MSG_WARNING ( "ERROR with local max logic");
      return false;
   }
   double seedCandidateEnergy = CaloCellET(inputCell, digitScale, digitThreshold);
   int nCellsMoreEnergetic = 0;
   const CaloCell* leftCell = NextEtaCell(inputCell, true, inputContainer, idHelper); 
   if (leftCell != nullptr){
      double leftEnergy = CaloCellET(leftCell, digitScale, 0.);
      if (leftEnergy>seedCandidateEnergy)  nCellsMoreEnergetic++;
   }
   const CaloCell* rightCell = NextEtaCell(inputCell, false, inputContainer, idHelper); 
   if (rightCell != nullptr){
      double rightEnergy = CaloCellET(rightCell, digitScale, 0.);
      if (rightEnergy>=seedCandidateEnergy)  nCellsMoreEnergetic++;
   }
   const CaloCell* upCell = NextPhiCell(inputCell, true, inputContainer, idHelper);
   if (upCell != nullptr){
      double upEnergy = CaloCellET(upCell, digitScale, 0.);
      if (upEnergy>=seedCandidateEnergy) nCellsMoreEnergetic++;
   }
   const CaloCell* downCell = NextPhiCell(inputCell, false, inputContainer, idHelper);
   if (downCell != nullptr){
      double downEnergy = CaloCellET(downCell, digitScale, 0.);
      if (downEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
   }	
   if (upCell != nullptr){
      const CaloCell* upRightCell = NextEtaCell(upCell, false, inputContainer, idHelper);
      if (upRightCell != nullptr){
         double upRightEnergy = CaloCellET(upRightCell, digitScale, 0.);
         if (upRightEnergy>=seedCandidateEnergy) nCellsMoreEnergetic++;
      }
      const CaloCell* upLeftCell = NextEtaCell(upCell, true, inputContainer, idHelper);
      if (upLeftCell != nullptr){
         double upLeftEnergy = CaloCellET(upLeftCell, digitScale, 0.);
         if (upLeftEnergy>=seedCandidateEnergy) nCellsMoreEnergetic++;
      }
   }
   if (downCell != nullptr){
      const CaloCell* downRightCell = NextEtaCell(downCell, false, inputContainer, idHelper);
      if (downRightCell != nullptr){
         double downRightEnergy = CaloCellET(downRightCell, digitScale, 0.);
         if (downRightEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
      }
      const CaloCell* downLeftCell = NextEtaCell(downCell, true, inputContainer, idHelper);
      if (downLeftCell != nullptr){
         double downLeftEnergy = CaloCellET(downLeftCell, digitScale, 0.);
         if (downLeftEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
      }
   }
   // If candidate is more energetic than all of neighbours, it is a local max 
   if (nCellsMoreEnergetic <= numOthers) return true;
   else return false;
}

void
LVL1::EFexEMClusterTool::addOnce(const CaloCell* inputCell, std::vector<const CaloCell*> &outputVector) const
{
   if (inputCell==nullptr) return;
   bool alreadyThere = false;
   for (auto oCell : outputVector){
      if (oCell==nullptr) ATH_MSG_WARNING ( "nullptr cell in vector");
      if (inputCell->ID() == oCell->ID()) alreadyThere=true;
   }
   if (!alreadyThere) outputVector.push_back(inputCell);
}

double
LVL1::EFexEMClusterTool::EMClusET(const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                                 const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const
{
   // Sums the ET of the vector
   std::vector<const CaloCell*> fullClus = TDR_Clus(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale,digitThresh);
   double EMcomp = sumVectorET(fullClus, digitScale, digitThresh);  
   bool EMcheck = checkDig(EMcomp, digitScale, digitThresh);
   if (!EMcheck) ATH_MSG_WARNING ( "EMcomp not digitised  " << EMcomp << "  " << digitScale << "  " << digitThresh);
   double total = EMcomp;
   return total;
}

double
LVL1::EFexEMClusterTool::REta(const CaloCell* centreCell, int etaWidth1, int phiWidth1, int etaWidth2, int phiWidth2, 
                              const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const
{
   // Check windows sizes are right way round
   if (etaWidth1 > etaWidth2) ATH_MSG_WARNING ( "REta ERROR: eta1 = " << etaWidth1 << ", eta2 = " << etaWidth2);
   if (phiWidth1 > phiWidth2) ATH_MSG_WARNING ( "Rphi ERROR: phi1 = " << phiWidth1 << ", phi2 = " << phiWidth2);
   // Finds ET of windows
   double inner_ET = L2clusET(centreCell, etaWidth1, phiWidth1, scells, idHelper, digitScale, digitThresh);
   double outer_ET = L2clusET(centreCell, etaWidth2, phiWidth2, scells, idHelper, digitScale, digitThresh);
   // Find normal value of REta & changes it to my version
   double normal_REta;
   if (inner_ET != 0. && outer_ET==0.) normal_REta = 0.;
   else if (inner_ET==0.) normal_REta = 0.;
   else normal_REta = inner_ET / outer_ET;
   if (normal_REta < 0) normal_REta = 0.;
   double my_REta = 1-normal_REta;
   return my_REta;
}

double
LVL1::EFexEMClusterTool::RHad(const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                              const xAOD::TriggerTowerContainer* &TTContainer, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh, float &HadET) const
{
   std::vector<const CaloCell*> fullClus = TDR_Clus(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh);
   double EMcomp = sumVectorET(fullClus, digitScale, digitThresh);
   double HCALcomp = HadronicET(L2cluster(centreCell, m_etaHadWidth_RHadIsolation, m_phiHadWidth_RHadIsolation, scells, idHelper, digitScale, digitThresh), scells, TTContainer, idHelper, digitScale, digitThresh);
   HadET = HCALcomp/1e3;
   double result = HCALcomp/(EMcomp+HCALcomp);
   if (result < 0. || result > 1.){
      ATH_MSG_WARNING ( "RHAD ERROR -> " << etaWidth << " * " << phiWidth);
      ATH_MSG_WARNING ( "fullClus count = " << fullClus.size() << ", EMcomp = " << EMcomp << ", HCALcomp = " << HCALcomp);
   }
   return result;
}

void
LVL1::EFexEMClusterTool::checkTileCell(const TileCell* &inputCell, std::vector<const TileCell*> &tileCellVector, bool &isAlreadyThere) const
{
   for (auto ithCell : tileCellVector){
      if (ithCell->ID() == inputCell->ID()) isAlreadyThere = true;
   }
   if (!isAlreadyThere) tileCellVector.push_back(inputCell);
}

double
LVL1::EFexEMClusterTool::tileCellEnergyCalib(float eIn, float etaIn, float tileNoiseThresh) const
{
   if (eIn <= 0) return 0.;
   float eOut = eIn/cosh(etaIn);
   if (tileNoiseThresh == 0.) return eOut;
   else {
      if (eOut > tileNoiseThresh) return eOut;
      else return 0.;
   }
}

int
LVL1::EFexEMClusterTool::detRelPos(const float inEta) const
{
   float pos_neg = inEta/std::abs(inEta);
   // Right PMT : inPos = 0, Left PMT : inPos = 1, Both PMTs : inPos = 2
   int inPos = -1;
   // True if even, false if odd
   bool isEven = false;
   if (((int)(std::abs(inEta)*10)) % 2 == 0) isEven = true;
   if (pos_neg > 0){
      // A side of TileCal
      if (inEta < 0.1) inPos = 0;
      else if (inEta > 0.8 && inEta < 0.9) inPos = 2;
      else {
         if (isEven) inPos = 0;
         else        inPos = 1;
      }
   }
   else {
      // C side of TileCal
      if (inEta > -0.1) inPos = 1;
      else if (inEta > -0.9 && inEta < -0.8) inPos = 2;
      else {
         if (isEven) inPos = 1;
         else        inPos = 0;
      }
   }
   return inPos;
}

double
LVL1::EFexEMClusterTool::L1Width(const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                                     const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const
{
   // Finds a L2 cluster and the corresponding L1 cells
   std::vector<const CaloCell*> L2cells = L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper,digitScale, digitThresh);
   /// This loop adds L1 cells matching L2 cluster and finds offsets in eta from central L2 cell
   /// This offset-calculating logic relies on the order that L2cluster adds cells in: 
   ///     from 1 cell add neighbours at +/1 1 step in eta, +/- 2 steps, etc.
   ///     then move to next row in phi and repeat
   
   float oldPhi = centreCell->phi();
   int counter = 0;
   std::vector<int> offsets;
   std::vector<const CaloCell*> frontLayerCells;
   for (auto ithL2Cell : L2cells){
      // How many cells added already?
      unsigned int oldsize = frontLayerCells.size();
      // Add cells matching this L2 cell
      fromLayer2toLayer1(scells, ithL2Cell, frontLayerCells, idHelper);
      // HoW many were added?
      unsigned int additions = frontLayerCells.size() - oldsize;
      // Reset counter if phi has changed significantly
      float dPhi = std::abs(ithL2Cell->phi() - oldPhi);
      if (dPhi > M_PI) dPhi = 2*M_PI - dPhi;
      if (dPhi > 0.09) { 
         counter = 0;
         oldPhi = ithL2Cell->phi();
      }
      // Try storing signed offsets
      int sign = (ithL2Cell->eta()-centreCell->eta() > 0 ? 1 : -1);
      // Store current eta offset value for all added cells
      for (unsigned int adds = 0; adds < additions; ++adds) offsets.push_back(sign*((counter+1)/2));
      counter++;
   }

   // Finds the 'width' for the cluster, based on eta offsets found above
   float sumET = 0, sumET_Eta2=0;
   unsigned int cellCount = 0;
   //for (auto ithCell : frontLayerCells){
   for (std::vector<const CaloCell*>::iterator ithCell = frontLayerCells.begin(); ithCell != frontLayerCells.end(); ++ithCell){

      // Find offset. As a precaution ignore cells where this can't be found, but warn user
      int offset = (cellCount < offsets.size() ? offsets[cellCount] : -999);
      if (offset < -2 || offset > 2) {
         ATH_MSG_WARNING("Offset out of range, cell skipped");
         offset = 0;   // This will result in a weight of zero for the cell
      }

      // Is this one of the cells between 1.8-2.0 that will be divided?
      Identifier cellID = (*ithCell)->ID();
      int pos_neg = idHelper->pos_neg(cellID);
      int region = idHelper->region(cellID);
      int eta_index = idHelper->eta(cellID);
      bool halfCell = false;
      if (abs(pos_neg) == 2 && region == 3 && (eta_index == 1 || eta_index == 4 || eta_index == 7 || eta_index == 10)) halfCell = true; 

      // Total and weighted ET sums (integer weights to match firmware)
      float ithET = CaloCellET((*ithCell), digitScale, digitThresh);
      sumET += ithET;

      // 4 cells will be shared with neighbours. Jiggery-pokery required here:
      if (halfCell) {
         sumET_Eta2 += 0.5*ithET*pow(offset,2); 
         // Now what should be the offset for the other half?
         // Is this one shared with the previous cell?
         // If so, which cell is shares with depends on which side of that cell it is
         if ((int)cellCount-1 >= 0 && offsets[cellCount-1] == offset) {
            auto ithPrev = std::prev(ithCell,1);
            int sign = ((*ithCell)->eta() > (*ithPrev)->eta() ? 1 : -1);
            int nextOffset = offset+sign;
            if (abs(nextOffset) <= 2) sumET_Eta2 += 0.5*ithET*pow(nextOffset,2);
         }
      }
      // Alternatively may be shared with next cell
      else if (cellCount+1 < offsets.size() && offsets[cellCount+1] == offset) {
         auto ithNext = std::next(ithCell,1);
         int sign = ((*ithCell)->eta() > (*ithNext)->eta() ? 1 : -1);
         int nextOffset = offset+sign;
         if (abs(nextOffset) <= 2) sumET_Eta2 += 0.5*ithET*pow(nextOffset,2);
      }    
      // For everything else just add cell with weight to the second sum
      else {
         sumET_Eta2 += ithET*pow(offset,2);  
      }
      cellCount++;
   }

   /// Offline version (floating point)
   /// Firmware version (integer weights, no sqrt)
   float result = 4.;
   if (sumET > 0.) result = sumET_Eta2/sumET;
   return result;
}

double
LVL1::EFexEMClusterTool::L2clusET(const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                                      const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const
{
   return sumVectorET(L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh), digitScale, digitThresh);
}

double
LVL1::EFexEMClusterTool::RHadTile(const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells, 
                                      const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh, const TileID* tileIDHelper,
                                      const CaloConstCellContainer* tileCellCon, float tileNoiseThresh, float &HadronicET) const
{
   std::vector<float> outVec;
   double HadET = 0.;
   std::vector<const CaloCell*> L2Cells = L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh);
   std::vector<const CaloCell*> fullClus = TDR_Clus(centreCell, m_etaHadWidth_RHadIsolation, m_phiHadWidth_RHadIsolation, scells, idHelper, digitScale, digitThresh);
   // Last Tile cell boundary: eta = 1.6
   // Last outer wheel SC seed that still falls into Tile boundary: eta = 1.5625
   if (std::abs(centreCell->eta()) < 1.57){
      const int barrel_ec = idHelper->pos_neg(centreCell->ID());
      bool isOW = false;
      if (std::abs(barrel_ec) == 2) isOW = true;
      std::vector<double> energyPerLayer = EnergyPerTileLayer(L2Cells, tileCellCon, tileIDHelper, isOW, tileNoiseThresh);
      if (energyPerLayer.size() > 0){
         for (auto ithLayerEnergy : energyPerLayer){
            HadET += ithLayerEnergy;
         }
      }
   }
   else {
      std::vector<const CaloCell*> HCAL_LAr_vector;
      for (auto ithCell : L2Cells){
         if (std::abs(ithCell->eta()) > 2.5) continue; 
         const CaloCell* tempLArHad = matchingHCAL_LAr(ithCell, scells, idHelper);
         if (tempLArHad != nullptr) HCAL_LAr_vector.push_back(tempLArHad);
      }
      for (auto ithSC : HCAL_LAr_vector){
         HadET += CaloCellET(ithSC, digitScale, digitThresh);
      }
   }
   HadronicET = HadET/1e3;
   double EMcomp = sumVectorET(fullClus, digitScale, digitThresh);
   double result = HadET/(EMcomp+HadET);
   if (result < 0. || result > 1.){
      ATH_MSG_WARNING ( "RHADTILE ERROR -> " << etaWidth << " * " << phiWidth);
      ATH_MSG_WARNING ( "fullClus count = " << fullClus.size() << ", EMcomp = " << EMcomp << ", HCALcomp = " << HadET);
      return 1.;
   }
   return result;
}

double
LVL1::EFexEMClusterTool::REtaL12(const CaloCell* centreCell, int etaWidth1, int phiWidth1, int etaWidth2, int phiWidth2,
                                     const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper,
                                     float digitScale, float digitThresh) const
{
   // Check windows sizes are right way round
   if (etaWidth1 > etaWidth2) ATH_MSG_WARNING ( "REta ERROR: eta1 = " << etaWidth1 << ", eta2 = " << etaWidth2);
   if (phiWidth1 > phiWidth2) ATH_MSG_WARNING ( "Rphi ERROR: phi1 = " << phiWidth1 << ", phi2 = " << phiWidth2);
   // Finds ET of windows
   double inner_ET = L2clusET(centreCell, etaWidth1, phiWidth1, scells, idHelper, digitScale, digitThresh);
   double outer_ET = L2clusET(centreCell, etaWidth2, phiWidth2, scells, idHelper, digitScale, digitThresh);
   // Find corresponding L1 cells, calculate the L1 ET and add them to L2 ET
   std::vector<const CaloCell*> L2cells_inner = L2cluster(centreCell, etaWidth1, phiWidth1, scells, idHelper,digitScale, digitThresh);
   std::vector<const CaloCell*> L1cells_inner;
   for (auto ithL2Cell : L2cells_inner){
      fromLayer2toLayer1(scells, ithL2Cell, L1cells_inner, idHelper);
   }
   inner_ET += sumVectorET(L1cells_inner, digitScale, digitThresh);
   std::vector<const CaloCell*> L2cells_outer = L2cluster(centreCell, etaWidth2, phiWidth2, scells, idHelper,digitScale, digitThresh);
   std::vector<const CaloCell*> L1cells_outer;
   for (auto ithL2Cell : L2cells_outer){
      fromLayer2toLayer1(scells, ithL2Cell, L1cells_outer, idHelper);
   }
   outer_ET += sumVectorET(L1cells_outer, digitScale, digitThresh);
   // Find normal value of REta & changes it to my version
   double normal_REta;
   if (inner_ET != 0. && outer_ET==0.) normal_REta = 0.;
   else if (inner_ET==0.) normal_REta = 0.;
   else normal_REta = inner_ET / outer_ET;
   if (normal_REta < 0) normal_REta = 0.;
   double my_REta = 1-normal_REta;
   return my_REta;
}

void
LVL1::EFexEMClusterTool::fromLayer2toLayer1(const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell, 
                                                std::vector<const CaloCell*> &outputVector, const CaloCell_SuperCell_ID* &idHelper) const
{
   if (inputCell==nullptr) return;
   // Gets ID info
   Identifier inputID = inputCell->ID();
   int sampling = idHelper->sampling(inputID);
   const int sub_calo = idHelper->sub_calo(inputID);
   int pos_neg = idHelper->pos_neg(inputID);
   int region = idHelper->region(inputID);
   int eta_index = idHelper->eta(inputID);
   const int phi_index = idHelper->phi(inputID);
   int tracker = 0;
   if (sampling != 2) return;
   // Default values are same as input
   int outputRegion = region;
   int outputEta = eta_index;
   bool oneCell = false; // True if layer 2 SC only matches to a single layer 1 SC
   // Barrel reg 0 (which is a simple one)
   if ((abs(pos_neg) == 1)&&(region == 0)){
      oneCell = true;
   }
   // Barrel reg 1: 3 layer 1 SCs for 1 layer 2 SC
   // But we should map one of these onto the barrel SC, the other 2 onto EC SCs
   else if ((abs(pos_neg) == 1)&&(region == 1)){
      tracker = 2;
      outputRegion = 1;
      outputEta = 0;
      oneCell = true;
        /* This code produces a one-to-many matching, which is not how things work
      for (unsigned int i = 0; i < 3; i++){
         Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 1, region, i, phi_index);
         const CaloCell* resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
         addOnce(resultCell,outputVector);
      }
      */
   }
   /// Now we have to map the other 3 SC in the transition tower correctly
   /// Sample 2 region 0 we treat as layer 3
   else if (abs(pos_neg)==2 && region == 0) {
      tracker = -1;
   }
   /// Sample 2 region 1 is 1.425-2.5. First 2 cells here map onto last 2 barrel layer 1
   /// (hence rescaling pos_neg to +/- 1)
   else if (abs(pos_neg)==2&&((region==1 && eta_index < 2))){
      tracker = 3;
      outputRegion = 1;
      outputEta = eta_index + 1;
      pos_neg /= abs(pos_neg);
      oneCell = true;           
   }
   /// Third cell in Sample 2 region 1 maps onto Sample 1 region 0
   else if (abs(pos_neg)==2&&((region==1 && eta_index == 2))){
      tracker = 4;
      outputRegion = 0;
      outputEta = 0;
      oneCell = true;
   }
   /// After which the OW is normal for a little bit...
   else if (abs(pos_neg)==2&&region==1 && eta_index <= 14){
      // OW region 1 (on doc): 1:1 match
      tracker = 5;
      outputRegion = 2;
      outputEta = eta_index - 3;
      oneCell = true;
   }
   /// Then we get to the next weird region (1.8=2.0)
   else if (abs(pos_neg) == 2 && region == 1 && eta_index <= 22){
      // In this region there are 6 L1 supercells for every 4 L2 ones
      // The code below groups them 2:1:1:2 2:1:1:2, which is an old proposal
      // This is not what is actually done, but the structure of this code
      // makes it impossible to do this correctly.
      outputRegion = 3;
      // Middle 2 layer cells match central 2 layer 1 cells
      if (eta_index%4 == 0 || eta_index%4 ==1){
         tracker = 6;
         oneCell = true;
         if (eta_index < 20) outputEta = eta_index -14;
         else outputEta = eta_index - 12;
      }
      // Edges have a 2:1 ratio. 2 L1s for each L2
      else {
         tracker = 7;
         int offset = 0;
         if (eta_index == 15) offset = 15;
         else if (eta_index == 18) offset = 14;
         else if (eta_index == 19) offset = 13;
         else if (eta_index == 22) offset = 12;
         else {
            ATH_MSG_WARNING ( "ERROR: " << __LINE__);
         }
         for (unsigned int  i = 0; i < 2; i++){
            outputEta = i+eta_index - offset;
            Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 1, outputRegion, outputEta, phi_index);
            const CaloCell* resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
            addOnce(resultCell,outputVector);
         }
      } 
   }
   /// Followed by normal again (2.0-2.4)
   else if (abs(pos_neg)==2 && region == 1 && eta_index <= 38){
      // OW Reg 3 (on doc): 1:1 match
      tracker = 8;
      oneCell = true;
      outputRegion = 4;
      outputEta = eta_index - 23;
   }
   /// And finally one odd one (2.4-2.5)
   else if (abs(pos_neg)==2 && region == 1 && eta_index == 40){
      // OW Reg 4 (on doc): 1 L1 for all 4 L2s
      // But this must be mapped onto a specific cell: second one seems best
      // Note: to try alternative mapping of this cell (to Layer 0) should return without adding cell here
      tracker = 9;
      oneCell = true;
      outputEta = 0;
      outputRegion = 5;
   }

   if (oneCell){
      Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 1, outputRegion, outputEta, phi_index);
      const CaloCell* resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
      addOnce(resultCell,outputVector);
   }
   ATH_MSG_DEBUG("L2->L1: sampling = " << sampling << ", region = " << region << ", eta = " << pos_neg*eta_index<< " tracker = " << tracker);
}

const CaloCell*
LVL1::EFexEMClusterTool::fromLayer2toLayer3(const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper) const
{
   // Gets ID info
   int tracker = 0;
   if ( inputCell == nullptr ) return nullptr;
   const CaloCell* resultCell = nullptr;
   Identifier inputID = inputCell->ID();
   int sampling = idHelper->sampling(inputID);
   const int sub_calo = idHelper->sub_calo(inputID);
   const int pos_neg = idHelper->pos_neg(inputID);
   int region = idHelper->region(inputID);
   int eta_index = idHelper->eta(inputID);  
   const int phi_index = idHelper->phi(inputID);
   if (sampling != 2) return nullptr;
   else if (abs(pos_neg)==1 && ((region==0 && eta_index>53)||region==1)) return nullptr;
   else if ((abs(pos_neg)==2) && (region == 0 || (region == 1 && eta_index < 3))) return nullptr;
   else if (abs(pos_neg)==3) return nullptr;
   // Default values are same as input
   int outputRegion = region;
   int outputEta = eta_index;
   // Is barrel Reg 0
   if (abs(pos_neg)==1 && region ==0){
      int outputEta = eta_index/4;
      Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 3, outputRegion, outputEta, phi_index);
      resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
      tracker = 1;
   }
   /// Special case: transition tower treats endcap layer 2 as layer 3
   else if (abs(pos_neg)==1 && region ==1) {
      int output_pos_neg = pos_neg*2;
         outputRegion = 0;
         int outputEta = 0;
         Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, output_pos_neg, 2, outputRegion, outputEta, phi_index);
         resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
         tracker = 2;                  
   }
   /// Now in the endcap
   else if (abs(pos_neg)==2 && region ==1){
      outputEta = (eta_index - 3)/4;
      outputRegion = 0;
      Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 3, outputRegion, outputEta, phi_index);
      resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
      tracker = 3;
   }
   ATH_MSG_DEBUG("L2->L3: sampling = " << sampling << ", region = " << region << ", eta = " << pos_neg*eta_index<< " tracker = " << tracker);
   return resultCell;
}

const CaloCell*
LVL1::EFexEMClusterTool::fromLayer2toPS(const CaloConstCellContainer* & inputContainer, const CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper) const
{
   // Gets ID info
   if (inputCell==nullptr) return nullptr;
   const CaloCell* resultCell = nullptr;
   Identifier inputID = inputCell->ID();
   int sampling = idHelper->sampling(inputID);
   const int sub_calo = idHelper->sub_calo(inputID);
   const int pos_neg = idHelper->pos_neg(inputID);
   int region = idHelper->region(inputID);
   int eta_index = idHelper->eta(inputID);  
   const int phi_index = idHelper->phi(inputID);
   if (sampling != 2) return nullptr;
   if (abs(pos_neg)==2 && (eta_index<3 || eta_index>14)) return nullptr;
   if (abs(pos_neg)==3) return nullptr;
   // Default values are same as input
   int outputRegion = region;
   int outputEta = eta_index;
   // Is barrel Reg 0
   if (abs(pos_neg)==1 && region ==0){
      int outputEta = eta_index/4;
      Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 0, outputRegion, outputEta, phi_index);
      resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
   }
   else if (abs(pos_neg)==1 && region ==1){
      Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 0, 0, 14, phi_index);
      resultCell = returnCellFromCont(resultID, inputContainer, idHelper);
   }
   else if (abs(pos_neg)==2 && region ==1){
      outputEta = (eta_index - 3)/4;
      outputRegion = 0;
      Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 0, outputRegion, outputEta, phi_index);
      resultCell = returnCellFromCont(resultID, inputContainer, idHelper); 
   }
   return resultCell;
}

std::vector<const CaloCell*>
LVL1::EFexEMClusterTool::L2cluster( const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                                        const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh ) const
{
   // Forms the central band of cells, spread in phi
   std::vector<const CaloCell*> centCells;
   centCells.push_back(centreCell);
   const CaloCell* upPhiCell = NextPhiCell(centreCell,true,scells,idHelper);
   const CaloCell* downPhiCell = NextPhiCell(centreCell,false,scells,idHelper);
   const CaloCell* energeticPhiCell;
   // Finds the most energetic phi neighbour, defaulting to the 'down' side if they are equal
   if ( CaloCellET(upPhiCell, digitScale, digitThresh) > CaloCellET(downPhiCell, digitScale, digitThresh)) energeticPhiCell = upPhiCell;
   else energeticPhiCell = downPhiCell;
   if (phiWidth == 2) addOnce(energeticPhiCell, centCells); //centCells.push_back(energeticPhiCell);
   else if (phiWidth == 3){
      addOnce(upPhiCell, centCells); //centCells.push_back(upPhiCell);
      addOnce(downPhiCell, centCells); //centCells.push_back(downPhiCell);
   }
   else if (phiWidth > 3) {
      ATH_MSG_WARNING ( "ERROR: phiWidth not 2 or 3!!!");
   }
   // Forms the main cluster. Starts with each SC in the central band and spreads outward in eta
   std::vector<const CaloCell*> clusCells;
   int halfEtaWidth = (etaWidth-1)/2;
   int backToEta = (2*halfEtaWidth)+1;
   if (backToEta != etaWidth) {
      ATH_MSG_WARNING ( "Eta width doesn't match! " << backToEta << " -> " << halfEtaWidth << " -> " << etaWidth << "  " << __LINE__);
   }
   for (auto ithCentCell : centCells){
      addOnce(ithCentCell, clusCells); //clusCells.push_back(ithCentCell);
      if (etaWidth > 1){
         const CaloCell* tempRightCell = NextEtaCell(ithCentCell,true,scells,idHelper);
         const CaloCell* tempLeftCell = NextEtaCell(ithCentCell,false,scells,idHelper);
         addOnce(tempRightCell, clusCells); //clusCells.push_back(tempRightCell);
         addOnce(tempLeftCell, clusCells); //clusCells.push_back(tempLeftCell);
         for (int i = 1; i < halfEtaWidth; i++){
            tempRightCell = NextEtaCell(tempRightCell,true,scells,idHelper);
            tempLeftCell = NextEtaCell(tempLeftCell,false,scells,idHelper);
            addOnce(tempRightCell, clusCells); //clusCells.push_back(tempRightCell);
            addOnce(tempLeftCell, clusCells); //clusCells.push_back(tempLeftCell);
         }
      }
   }
   return clusCells;
}

std::vector<double>
LVL1::EFexEMClusterTool::EnergyPerTileLayer( const std::vector<const CaloCell*> & inputSCVector, const CaloConstCellContainer* CellCon,
                                                 const TileID* tileIDHelper, bool isOW, float tileNoiseThresh) const
{
   std::vector<double> layerEnergy;
   if (CellCon==nullptr) return layerEnergy;
   if (CellCon->size()==0) return layerEnergy;
   if (inputSCVector.size()==0) return layerEnergy;
   double ELayer0 = 0, ELayer1 = 0, ELayer2 = 0;
   std::vector<const TileCell*> tileCellVector;
   for (auto ithSC : inputSCVector){
      float ithSCEta = ithSC->eta();
      float ithSCPhi = ithSC->phi();
      int matchingCells = 0;
      CaloCellContainer::const_iterator fCell = CellCon->beginConstCalo(CaloCell_ID::TILE);
      CaloCellContainer::const_iterator lCell = CellCon->endConstCalo(CaloCell_ID::TILE);
      for ( ; fCell != lCell; ++fCell){
         const TileCell* tileCell = static_cast<const TileCell*>(*fCell);
         if (!tileCell){
            ATH_MSG_WARNING ( "Failed to cast from CaloCell to TileCell");
            return layerEnergy;
         }
         int layer = tileIDHelper->sample(tileCell->ID());
         float ithdR = dR(tileCell->eta(), tileCell->phi(), ithSCEta, ithSCPhi);
         if (layer < 2){
            float matchingDistance = 0.;
            if (isOW && (std::abs(ithSCEta) > 1.38 && std::abs(ithSCEta) < 1.42)) matchingDistance = 0.065;
            else matchingDistance = 0.05;
            if (ithdR <= matchingDistance){
               bool isAlreadyThere = false;
               checkTileCell(tileCell, tileCellVector, isAlreadyThere);
               if (isAlreadyThere) continue;
               matchingCells++;
               if (layer == 0) ELayer0 += tileCellEnergyCalib(tileCell->e(), tileCell->eta(), tileNoiseThresh);
               if (layer == 1) ELayer1 += tileCellEnergyCalib(tileCell->e(), tileCell->eta(), tileNoiseThresh);
            }
         }
         else if (layer == 2){
            float matchingDistance = 0.;
            if (std::abs(ithSCEta) > 0.7 && std::abs(ithSCEta) < 0.8) matchingDistance = 0.05;
            else if (std::abs(ithSCEta) > 0.9 && std::abs(ithSCEta) < 1.0) matchingDistance = 0.05;
            else matchingDistance = 0.09;
            if (ithdR < matchingDistance){
               bool isAlreadyThere = false;
               checkTileCell(tileCell, tileCellVector, isAlreadyThere);
               if (isAlreadyThere) continue;
               matchingCells++;
               int tempPos = detRelPos(ithSCEta);
               // Unknown : tempPos = -1, Right PMT : tempPos = 0, Left PMT : tempPos = 1, Both PMTs : tempPos = 2
               if (tempPos < 0){
                  ATH_MSG_WARNING ( "Unknown behaviour matching Tile cells to the SC");
                  layerEnergy.clear();
                  return layerEnergy;
               }
               else if (tempPos == 0) ELayer2 += tileCellEnergyCalib(tileCell->ene2(), tileCell->eta(), tileNoiseThresh);
               else if (tempPos == 1) ELayer2 += tileCellEnergyCalib(tileCell->ene1(), tileCell->eta(), tileNoiseThresh);
               else	           ELayer2 += tileCellEnergyCalib(tileCell->e(),    tileCell->eta(), tileNoiseThresh);
            }
         }
      }
      if ((matchingCells > 3 && !isOW) || (matchingCells > 3 && isOW && std::abs(ithSCEta) > 1.42) || (matchingCells > 4 && isOW && std::abs(ithSCEta) < 1.42)){
         ATH_MSG_WARNING ( matchingCells << " matching Tile cells:");
         ATH_MSG_WARNING ( "Input SC: (eta,phi) = (" << ithSCEta << "," << ithSCPhi << ")");
         for (auto cell : tileCellVector){
            ATH_MSG_WARNING ( "Tile cell: (eta,phi) = (" << cell->eta() << "," << cell->phi() << ")" << " dR = " << dR(cell->eta(), cell->phi(), ithSCEta, ithSCPhi) << " layer = " << tileIDHelper->sample(cell->ID()));
         }
         layerEnergy.clear();
         return layerEnergy;
      }
   }
   layerEnergy = {ELayer0, ELayer1, ELayer2};
   return layerEnergy;
}

double
LVL1::EFexEMClusterTool::TT_phi(const xAOD::TriggerTower* &inputTower) const
{
   if (inputTower == nullptr){
      ATH_MSG_WARNING ( "Tower is nullptr in phi transformation!");
      return 0.;
   }
   else {
      double phi = inputTower->phi();
      if (phi > M_PI) phi = phi - 2*M_PI;
      return phi;
   } 
}

double
LVL1::EFexEMClusterTool::dR(double eta1, double phi1, double eta2, double phi2) const
{
   double etaDif = eta1 - eta2;
   double phiDif = std::abs(phi1 - phi2);
   if (phiDif > M_PI) phiDif = phiDif - (2*M_PI);
   double result = sqrt(pow(etaDif,2)+pow(phiDif,2));
   return result;
}

const xAOD::TriggerTower*
LVL1::EFexEMClusterTool::matchingHCAL_TT(const CaloCell* &inputCell, const xAOD::TriggerTowerContainer* &TTContainer) const
{
   std::vector<const xAOD::TriggerTower*> matchingTTs;
   if (TTContainer==nullptr) return nullptr;
   if (TTContainer->size()==0) return nullptr;
   if (inputCell==nullptr) return nullptr;
   for (auto ithTT : *TTContainer){
      if (ithTT->sampling()==1){
         float ithTT_eta = ithTT->eta();
         float ithTT_phi = TT_phi(ithTT);
         float ithdR = dR(ithTT_eta, ithTT_phi, inputCell->eta(), inputCell->phi());
         if (ithdR < 0.05) matchingTTs.push_back(ithTT);
      }
   }
   if (matchingTTs.size()==1) return matchingTTs[0];
   else if (matchingTTs.size()!=0){	
      ATH_MSG_WARNING ( "ERROR: More than one matching HCAL TT!!! (Returned Null)");
   }
   return nullptr;
}

const CaloCell*
LVL1::EFexEMClusterTool::matchingHCAL_LAr(const CaloCell* &inputCell, const CaloConstCellContainer* &SCContainer, const  CaloCell_SuperCell_ID* &idHelper) const
{
   std::vector<const CaloCell*> matchingCells;
   if (inputCell==nullptr) return nullptr;
   for (auto ithSC : *SCContainer){
      Identifier ithID = ithSC->ID();
      int ithSub_calo = idHelper->sub_calo(ithID);
      if (ithSub_calo == 1){
         double ithdR = dR(inputCell->eta(), inputCell->phi(), ithSC->eta(), ithSC->phi());
         if (ithdR < 0.05) matchingCells.push_back(ithSC);
      }
   }

   if (matchingCells.size()==1)
      return matchingCells[0];
  

   if (matchingCells.size()==0){

      ATH_MSG_WARNING ( "ERROR: No match betweem LAr ECAL SC and LAr HCAL SC!!! Input coords: " << inputCell->eta() << ", " << inputCell->phi());

   } else if (matchingCells.size()!=0) {

      ATH_MSG_WARNING ( "More than one matching LAr HCAL SC!!! (Returned Null)");
      ATH_MSG_WARNING ( "Input cell coords: " << inputCell->eta() << " x " << inputCell->phi());
      for (auto ithMatch : matchingCells){
         ATH_MSG_WARNING ( "    " << ithMatch->eta() << " x " << ithMatch->phi() << ", dR = " 
                           << dR(inputCell->eta(), inputCell->phi(), ithMatch->eta(), ithMatch->phi()));
      }
   }
   return nullptr;
}

double
LVL1::EFexEMClusterTool::TT_ET(const xAOD::TriggerTower* &inputTower) const
{
   if (inputTower == nullptr){
      ATH_MSG_WARNING ( "Tower is nullptr!");
      return 0.;
   }
   else if (inputTower->cpET() < 0.) {
      return 0;
   } else {
      return 500*inputTower->cpET();
   }
}

std::vector<const CaloCell*>
LVL1::EFexEMClusterTool::TDR_Clus( const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                                       const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh ) const
{
   // Find the L2 cells
   std::vector<const CaloCell*> L2cells = L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh);
   // Forms a vector of the centre L2 cells (to be used to find L0/3 SCs)
   std::vector<const CaloCell*> centCells;
   centCells.push_back(centreCell);
   const CaloCell* upPhiCell = NextPhiCell(centreCell,true,scells,idHelper);
   const CaloCell* downPhiCell = NextPhiCell(centreCell,false,scells,idHelper);
   const CaloCell* energeticPhiCell;
   // If the phi width is 2, the most energetic neighbour is chosen (defaulting to the 'down' side)
   // If the phi width is 3, both neighbours are added
   if (phiWidth > 1){  
      if (CaloCellET(upPhiCell, digitScale, digitThresh) > CaloCellET(downPhiCell, digitScale, digitThresh)) energeticPhiCell = upPhiCell;
      else energeticPhiCell = downPhiCell;
      if (phiWidth == 2) addOnce(energeticPhiCell, centCells); //centCells.push_back(energeticPhiCell);
      else if (phiWidth == 3){
         addOnce(upPhiCell, centCells); //centCells.push_back(upPhiCell);
         addOnce(downPhiCell, centCells); //centCells.push_back(downPhiCell);
      }
      else if (phiWidth > 3) ATH_MSG_WARNING ( "ERROR: phiWidth not 2 or 3!!!. Value = " << phiWidth);
   }
   // The actual cluster is initialised
   std::vector<const CaloCell*> fullClus;
   // The L1&2 SCs are added that match the full width
   for (auto ithL2Cell : L2cells){
      fullClus.push_back(ithL2Cell);
      fromLayer2toLayer1(scells, ithL2Cell, fullClus, idHelper);
   }
   // The L0&3 SCs are added that match the central L2 cells
   for (auto ithL2CentCell : centCells){
      addOnce( fromLayer2toPS( scells, ithL2CentCell, idHelper),fullClus);
      addOnce( fromLayer2toLayer3( scells, ithL2CentCell, idHelper),fullClus);
   }
   return fullClus;
}

double
LVL1::EFexEMClusterTool::sumVectorET(const std::vector<const CaloCell*> &inputVector, float digitScale, float digitThreshold) const
{
   double TotalET=0.0;
   for (auto ithCell : inputVector){
      if (ithCell!=nullptr) TotalET += CaloCellET(ithCell, digitScale, digitThreshold);
   }
   return TotalET;
}

bool
LVL1::EFexEMClusterTool::checkDig(float EM_ET, float digitScale, float digitThresh) const
{
   if (EM_ET == 0 || digitScale == 0) return true;
   else {
      int div = EM_ET / digitScale;
      if (div * digitScale == EM_ET) return true;
      else {
         ATH_MSG_WARNING ( "ET = " << EM_ET << ", digitThresh = " << digitThresh << " digitScale = " << digitScale << " div = " << div << " " << " -> div * digitScale");
         return false;
      }
   }
}

double
LVL1::EFexEMClusterTool::HadronicET( const std::vector<const CaloCell*> & inputVector, const CaloConstCellContainer* scells,
                                         const xAOD::TriggerTowerContainer* &TTContainer, const CaloCell_SuperCell_ID* idHelper,
                                         float digitScale, float digitThresh) const
{
   // Finds the HCAL SCs & TTs matching the input cluster
   std::vector<const CaloCell*> HCAL_LAr_vector;
   std::vector<const xAOD::TriggerTower*> HCAL_TT_vector;
   for (auto ithCell : inputVector){
      if (std::abs(ithCell->eta())<1.5){
         const xAOD::TriggerTower* tempTT = matchingHCAL_TT(ithCell, TTContainer);
         if (tempTT != nullptr) HCAL_TT_vector.push_back(tempTT);
      }
      else if (std::abs(ithCell->eta())<2.5){
         const CaloCell* tempLArHad = matchingHCAL_LAr(ithCell, scells, idHelper);
         if (tempLArHad != nullptr) HCAL_LAr_vector.push_back(tempLArHad);
      }
   }
   // Sums the ET in the HCAL
   double HadET = 0.;
   for (auto ithTT : HCAL_TT_vector) {HadET += TT_ET(ithTT);}
   for (auto ithSC : HCAL_LAr_vector) {HadET += CaloCellET(ithSC, digitScale, digitThresh);}
   return HadET;
}


/**
 * helper functions to find neighbouring cells
 */
const CaloCell*
LVL1::EFexEMClusterTool::returnCellFromCont(Identifier inputID, const CaloConstCellContainer* &cellContainer, const CaloCell_SuperCell_ID* &idHelper) const
{
   const CaloCell* isCell = cellContainer->findCell(idHelper->CaloCell_SuperCell_ID::calo_cell_hash(inputID));
   if (isCell) return isCell;
   else return nullptr;
}

const CaloCell*
LVL1::EFexEMClusterTool::NextEtaCell( const CaloCell* inputCell, bool upwards, const CaloConstCellContainer* &cellContainer,
                                          const  CaloCell_SuperCell_ID* &idHelper) const
{
   if (inputCell==nullptr) return nullptr;
   Identifier ithID = inputCell->ID();
   int ithSub_calo = idHelper->sub_calo(ithID);
   int ithPos_neg = idHelper->pos_neg(ithID);
   const CaloCell* tempCell = nullptr;
   // Only works for LArEM
   if (ithSub_calo==0){
      // Barrel regions
      if (abs(ithPos_neg)==1) tempCell = NextEtaCell_Barrel(inputCell, upwards, cellContainer, idHelper);
      // EC OW
      else if (abs(ithPos_neg)==2) tempCell = NextEtaCell_OW(inputCell, upwards, cellContainer, idHelper);
      // EC IW  
      else if (abs(ithPos_neg)==3) tempCell = NextEtaCell_IW(inputCell, upwards, cellContainer, idHelper);
      // Not barrel or end cap
      else {
         ATH_MSG_WARNING ( "Layer 2 cell not passed to specific method at" << inputCell->eta() << " , " << inputCell->phi());
         return nullptr;
      }
      return tempCell;
   }
   // Is FCAL
   else {
      ATH_MSG_WARNING ( "Next eta cell called for non-EM SC!");
      return nullptr;
   }
}

const CaloCell*
LVL1::EFexEMClusterTool::NextEtaCell_Barrel(const CaloCell* inputCell, bool upwards, const CaloConstCellContainer* &cellContainer, 
                                                const CaloCell_SuperCell_ID* &idHelper) const
{
   const Identifier ithID = inputCell->ID();
   const int ithEta_index = idHelper->eta(ithID);
   const int ithPhi_index = idHelper->phi(ithID);
   const int ithSampling = idHelper->sampling(ithID);
   const int ithSub_calo = idHelper->sub_calo(ithID);
   const int ithPos_neg = idHelper->pos_neg(ithID);
   const int ithRegion = idHelper->region(ithID);

   // Extreme indices of each region
   int maxEta_index = 0;
   int minEta_index = 0;
   if (ithRegion==0){
      if (ithSampling == 0) maxEta_index = 14;
      else if (ithSampling == 1 || ithSampling == 2) maxEta_index = 55;
      else if (ithSampling == 3) maxEta_index = 13;
      else ATH_MSG_WARNING ( "ERROR: " << __LINE__);
   }
   else if (ithRegion==1){
      if (ithSampling == 1) maxEta_index =2;
      else if (ithSampling == 2) maxEta_index=0;
      else ATH_MSG_WARNING ( "ERROR: " << __LINE__);
   }
   else ATH_MSG_WARNING ( "ERROR: " << __LINE__);    
   // Declare next values, default initialisation is the same as cell
   int nextEta_index = ithEta_index;
   // Phi shouldn't change!
   // One special case where sampling does change, otherwise stays same
   int nextSampling = ithSampling;
   int nextSub_calo = ithSub_calo;
   int nextPos_neg = ithPos_neg;
   int nextRegion = ithRegion;

   // Calculate the increment for eta: it depends on whether we are moving 'up' & which side we are on
   int incrementEta;
   if (upwards) incrementEta = ithPos_neg;
   else incrementEta = -1*ithPos_neg;

   int tracker = 0;    
    
   // If first cell in region & moving more inwards
   if (ithEta_index==minEta_index && incrementEta==-1){
      if (ithRegion == 0){
         nextEta_index = 0;
         nextPos_neg = ithPos_neg * -1;
         tracker = 1;
      }
      else if (ithRegion == 1){
         nextEta_index = 55;
         nextRegion = 0;
         tracker = 2;
      }
      else ATH_MSG_WARNING ( "ERROR: " << __LINE__);
   }

   // If last cell in region & moving outwards
   else if ((ithEta_index == maxEta_index) && (incrementEta == 1)) {
      // Reg 0, Layers 1 & 2 go to barrel region 1
      if ((ithRegion == 0)&&(ithSampling == 1 || ithSampling == 2)){
         nextRegion = 1;
         nextEta_index = 0;
         tracker = 3;
      }
      // Reg 0, Layer 0 goes to OW region 0
      else if ((ithRegion == 0)&&(ithSampling == 0)){
         nextEta_index = 0;
         nextRegion = 0;
         nextPos_neg = 2*ithPos_neg;
         tracker = 4;
      }
      // Reg 0, Layer 3 goes to OW Layer 2 region 0 (change by ATW)
      else if ((ithRegion == 0)&&(ithSampling == 3)){
         nextSampling = 2;
			nextEta_index = 0;
			nextRegion = 0;
			nextPos_neg = 2*ithPos_neg;
			tracker = 5;
      }
      // Reg 1, Layer 1 go to OW region 0 (change by ATW)
      else if ((ithRegion == 1)&&(ithSampling == 1)){
         nextEta_index=0;
         nextRegion = 0;
         nextPos_neg = 2 * ithPos_neg;
         tracker = 6;
      }
      // Reg 1, Layer 2 goes to OW region 1
      else if ((ithRegion == 1)&&(ithSampling == 2)){
         nextEta_index=0;
         nextRegion = 1;
         nextPos_neg = 2 * ithPos_neg;
         tracker = 7;
      }
      else ATH_MSG_WARNING ( "ERROR: " << __LINE__);
   }
   // Otherwise 'simply' next cell along
   else {
      nextEta_index = ithEta_index + incrementEta;
      tracker = 8;
   }
   //ATH_MSG_DEBUG ( "B Tracker = " << tracker);
   // Form identifier, find cell & return it
   // sub_calo, left_pos_neg, 2, region, eta_index, down_phi_index
   Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(nextSub_calo, nextPos_neg, nextSampling, nextRegion, nextEta_index, ithPhi_index);
   const CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer, idHelper);      
   if (nextCell == nullptr) {
      ATH_MSG_WARNING ( "ERROR: " << __LINE__);
      ATH_MSG_WARNING ( "Barrel Tracker = " << tracker);
      ATH_MSG_WARNING ( "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID);            
   }
   else {
      Identifier newID = nextCell->ID();
      int IDsample = idHelper->sampling(nextCell->ID());
      /// Leave this in for debug purposes, but I now expect it to happen
      if (IDsample!=ithSampling){
         ATH_MSG_WARNING ( "Layer has changed " << " tracker = " << tracker);
         ATH_MSG_WARNING ( "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID);
         ATH_MSG_WARNING ( "from ID from new cell: "<<idHelper->sub_calo(newID)<<", "<<idHelper->pos_neg(newID)<<", "<<idHelper->sampling(newID)<<", "<<idHelper->region(newID)<<", "<<idHelper->eta(newID)<<", "<<idHelper->phi(newID)<<", "<<idHelper->calo_cell_hash(newID)<<", "<<newID);
         ATH_MSG_WARNING ( "comp indices: "<< (nextCellID == newID));
      }
   }
   if (nextCell && (nextCell->ID() != nextCellID)) ATH_MSG_WARNING ( __LINE__ << " does not match");
   return nextCell;
}

const CaloCell*
LVL1::EFexEMClusterTool::NextEtaCell_OW( const CaloCell*inputCell, bool upwards, const CaloConstCellContainer* &cellContainer,
                                             const CaloCell_SuperCell_ID* &idHelper) const
{
   Identifier ithID = inputCell->ID();
   int ithEta_index = idHelper->eta(ithID);
   const int ithPhi_index = idHelper->phi(ithID);
   const int ithSampling = idHelper->sampling(ithID);
   int ithSub_calo = idHelper->sub_calo(ithID);
   int ithPos_neg = idHelper->pos_neg(ithID);
   int ithRegion = idHelper->region(ithID);   
   // Declare next values, default initialisation is the same as cell
   int nextEta_index = ithEta_index;
   int nextPhi_index = ithPhi_index;
   // Sampling may change in a couple of special cases (transition tower)
   int nextSampling = ithSampling;
   int nextSub_calo = ithSub_calo;
   int nextPos_neg = ithPos_neg;
   int nextRegion = ithRegion;
   // Maximum indices for barrel region 0:
   int maxEta_index = 0;
   int minEta_index = 0;
   // Set max / min values based on ithRegion
   if (ithSampling==0) maxEta_index = 2;
   else if (ithSampling==2 && ithRegion==0) maxEta_index = 0;
   else if (ithSampling==2 && ithRegion==1) maxEta_index = 42; 
   else if (ithSampling==3) maxEta_index=9;
   else if (ithSampling==1) {
      switch(ithRegion){
      case 0:
         maxEta_index=0;
         break;
      case 1:
         ATH_MSG_WARNING ( "ERROR " << __LINE__);
         break;
      case 2:
         maxEta_index=11;
         break;
      case 3:
         maxEta_index=11;// Should this be 11? - it was 7
         break;
      case 4:
         maxEta_index=15;  
         break;
      case 5:
         maxEta_index=0;
         break;
      default:
         ATH_MSG_WARNING ( "OW region is not covered: " << ithRegion);
      }
   }        
   else ATH_MSG_WARNING ( "ERROR: " << __LINE__ );

   // Calculate the increment for eta: it depends on whether we are moving 'up' & which side we are on
   int incrementEta;
   int ithSide = ithPos_neg / abs(ithPos_neg);
   if (upwards) incrementEta = ithSide;
   else incrementEta = ithSide * -1;
   int tracker = 0;
   // Lower end of OW, going inwards
   if (ithEta_index==minEta_index && ithRegion==0 && incrementEta==-1){
      nextPos_neg = ithSide;
      if (ithSampling==0){
         nextRegion = 0;
         nextEta_index = 14;
         tracker = 1;
      }
      else if (ithSampling==1){
         nextRegion = 1;
         nextEta_index = 2;
         tracker = 2;
      }
      /// The OW region 0 layer 2 is treated as layer 3 in transition
      else if (ithSampling==2){
         nextRegion = 0;
         nextSampling = 2;
         nextEta_index = 13;
         tracker = 3;
      }
      /// Fix by ATW to allow for strange mapping in transition
      else if (ithSampling==3){
         nextRegion = 0;
         nextSampling = 2;
         nextEta_index = 0;
         nextPos_neg = ithPos_neg;
         tracker = 4;
      }
   }
   // Higher end of OW, going outwards
   else if (ithEta_index==maxEta_index && incrementEta==1){
      // Layers 0 & 3 aren't in IW
      if (ithSampling==0 || ithSampling==3) return nullptr;
      else if (ithSampling==2 && ithRegion==0){
         nextRegion = 1;
         nextEta_index = 0;
         tracker = 5;
      }
      else if ((ithSampling==2 && ithRegion==1)||(ithSampling==1 && ithRegion==5)){
         // Reaches IW
         nextEta_index=0;
         nextRegion=0;
         nextPhi_index=ithPhi_index/2;
         nextPos_neg=3*ithSide;
         tracker=6;
      }
      else if (ithSampling==1 && ithRegion==0){
         // Unsure what to do??
         nextRegion = 2;
         nextEta_index = 0;  
         tracker = 7;
      }
      else if (ithSampling==1){
         nextRegion=ithRegion + 1;
         nextEta_index=0;
         tracker = 8;
      }
   }
   // Lower end of region in OW, going inwards
   else if (ithEta_index==minEta_index && incrementEta==-1){
      // Shouldn't apply to layers 0 & 3
      // Only case for layer 2 should be in region 1
      // But this one is special because we want to step into barrel (ATW)
      if (ithSampling==2){
         nextRegion = 1;
         nextEta_index = 0;
         nextPos_neg = ithPos_neg;
         tracker = 9;
      }
      else if (ithSampling==1){
         if (ithRegion==0){     // haven't we covered this? (ATW)
            nextPos_neg = ithSide;
            nextRegion = 1;
            nextEta_index = 2;
            tracker = 10;
         }
         else {
            tracker = 11;
            // Layer one has muliple regions
            nextRegion = ithRegion-1;
            if (nextRegion==0) {
               nextEta_index=0;
               ATH_MSG_WARNING ( "ERROR: "<< __LINE__);
            }
            else if (nextRegion==1) {
               nextRegion = 0;
               nextEta_index= 0;
            }
            else if (nextRegion==2) nextEta_index=11;
            else if (nextRegion==3) nextEta_index=7;
            else if (nextRegion==4) nextEta_index=15;
         }
      }
   }
   // Middle of region in middle of endcap
   else {
      nextEta_index = ithEta_index+incrementEta;
      tracker = 12;
   }
   Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(nextSub_calo, nextPos_neg, nextSampling, nextRegion, nextEta_index, nextPhi_index);
   const CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer, idHelper);
   if (nextCell == nullptr) {
      ATH_MSG_WARNING ( "ERROR: "<<__LINE__);
      ATH_MSG_WARNING ( "OW Tracker = "<<tracker);
      ATH_MSG_WARNING ( "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID);
      ATH_MSG_WARNING ( "Increment eta = "<<incrementEta<<", max_eta = "<<maxEta_index<<", min_eta = "<<minEta_index);     
   }
   else {
      Identifier newID = nextCell->ID();
      int IDsample = idHelper->sampling(nextCell->ID());
      if (IDsample!=ithSampling){
         ATH_MSG_WARNING ( "Layer has changed "<<" tracker = "<<tracker);
         ATH_MSG_WARNING ( "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID);
         ATH_MSG_WARNING ( "from ID from new cell: "<<idHelper->sub_calo(newID)<<", "<<idHelper->pos_neg(newID)<<", "<<idHelper->sampling(newID)<<", "<<idHelper->region(newID)<<", "<<idHelper->eta(newID)<<", "<<idHelper->phi(newID)<<", "<<idHelper->calo_cell_hash(newID)<<", "<<newID);
         ATH_MSG_WARNING ( "comp indices: "<<(nextCellID == newID));
      }
   }
   if (nextCell && (nextCell->ID() != nextCellID)) ATH_MSG_WARNING ( __LINE__<< " does not match");
   return nextCell;
}

const CaloCell*
LVL1::EFexEMClusterTool::NextEtaCell_IW( const CaloCell* inputCell, bool upwards, const CaloConstCellContainer* &cellContainer,
                                             const  CaloCell_SuperCell_ID* &idHelper) const
{
   const Identifier ithID = inputCell->ID();
   const int ithEta_index = idHelper->eta(ithID);
   const int ithPhi_index = idHelper->phi(ithID);
   const int ithSampling = idHelper->sampling(ithID);
   const int ithSub_calo = idHelper->sub_calo(ithID);
   const int ithPos_neg = idHelper->pos_neg(ithID);
   const int ithRegion = idHelper->region(ithID);
   //int tracker =0;
   // Declare next values, default initialisation is the same as cell
   int nextEta_index = ithEta_index;
   int nextPhi_index = ithPhi_index;
   // Sampling shouldn't change!
   int nextSub_calo = ithSub_calo;
   int nextPos_neg = ithPos_neg;
   int nextRegion = ithRegion;

   // Maximum indices for barrel region 0:
   int maxEta_index = 0;
   int minEta_index = 0;

   if (ithRegion==0){
      maxEta_index=2;
      minEta_index=0;
   }
   else if (ithRegion!=1) ATH_MSG_WARNING ( "ERROR: " <<__LINE__);

   // Calculate the increment for eta: it depends on whether we are moving 'up' & which side we are on
   int incrementEta;
   int ithSide = ithPos_neg / abs(ithPos_neg);
   if (upwards) incrementEta = ithSide;
   else incrementEta = ithSide * -1;
   // Lower end of region IW, going inwards
   if (ithEta_index==minEta_index&& incrementEta==-1){
      // Goes to OW
      if (ithRegion == 0){
         nextPos_neg = 2*ithSide;
         nextPhi_index=2*ithPhi_index;
         if (ithSampling==1){	
            // tracker=1;
            nextRegion=5;
            nextEta_index=0;
         }
         else if (ithSampling==2){
            // tracker=2;
            nextRegion=1;
            nextEta_index=42;
         }
         else ATH_MSG_WARNING ( "ERROR: " <<__LINE__);
      }
      // Goes to IW region 0
      else if (ithRegion == 1){
         // tracker=3;
         nextRegion=0;
         nextEta_index=2;
      }
   }
   // Upper end of region in IW
   else if (ithEta_index==maxEta_index && incrementEta==1){
      // Goes to region 1
      if (ithRegion==0){
         // tracker=4;
         nextRegion=1;
         nextEta_index=0;      
      }
      // Reaches FCAL
      else if (ithRegion==1) return nullptr; 
   }
   // Increment eta like normal
   else {
      // tracker=5;
      nextEta_index=ithEta_index+incrementEta;
   }
   Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(nextSub_calo, nextPos_neg, ithSampling, nextRegion, nextEta_index, nextPhi_index);
   const CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer, idHelper);
   if (nextCell && (nextCell->ID() != nextCellID)) ATH_MSG_WARNING ( __LINE__<<" does not match");
   return nextCell;
}

int
LVL1::EFexEMClusterTool::restrictPhiIndex(int input_index, bool is64) const
{
   if (is64&&input_index<0) return input_index+64;
   else if (is64&&input_index>63) return input_index-64;
   else if (!(is64)&&input_index<0) return input_index+32;
   else if (!(is64)&&input_index>31) return input_index-32;
   else return input_index;
}

const CaloCell*
LVL1::EFexEMClusterTool::NextPhiCell( const CaloCell * inputCell, bool upwards, const CaloConstCellContainer* &cellContainer, 
                                          const  CaloCell_SuperCell_ID* &idHelper) const
{
   if (inputCell==nullptr)
      return nullptr;

   const Identifier ithID = inputCell->ID();
   const int ithEta_index = idHelper->eta(ithID);
   const int ithPhi_index = idHelper->phi(ithID);
   const int ithSampling = idHelper->sampling(ithID);
   const int ithSub_calo = idHelper->sub_calo(ithID);
   const int ithPos_neg = idHelper->pos_neg(ithID);
   const int ithRegion = idHelper->region(ithID);

   bool is64;
   if (abs(ithPos_neg)==3) is64 = false;
   else is64 = true;

   int incrementPhi;
   if (upwards==true) incrementPhi=1;
   else incrementPhi=-1;

   const int nextPhi_index = restrictPhiIndex(ithPhi_index+incrementPhi, is64);
   Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(ithSub_calo, ithPos_neg, ithSampling, ithRegion, ithEta_index, nextPhi_index);
   const CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer, idHelper);
   if (nextCell && (nextCell->ID() != nextCellID)) ATH_MSG_WARNING ( __LINE__ << " does not match");
   if (nextCell == nullptr) ATH_MSG_WARNING ( "Next phi cell is nullptr at " << __LINE__);
   return nextCell;
}
