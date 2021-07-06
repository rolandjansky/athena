// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * Name    : EFexEMClusterTool.h
 * PACKAGE : Trigger/TrigT1/TrigT1CaloFexPerf
 * AUTHOR  : Denis Oliveira Damazio
 * PURPOSE : emulate the eFex EM algorithm for phase 1 L1Calo (default clustering)
 */

#ifndef LVL1__EFEXEMCLUSTERTOOL
#define LVL1__EFEXEMCLUSTERTOOL

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "TileEvent/TileCellContainer.h"


#include <string>
#include <vector>


namespace LVL1
{
   class EFexEMClusterTool : public AthAlgTool
   {
   public:

      struct AlgResult {
         float eta{0}; // cluster eta
         float phi{0}; // cluster phi
         float clusterET{0}; // cluster transverse energy
         float rEta{0}; // R_eta
         float rHad{0}; // R_had
         float l1Width{0}; // Wstot
         float hadET{0}; // hadronic ET
         float l2ClusterET33{0}; // cluster ET (3x3)
         float l2ClusterET37{0}; // cluster ET (3x7)
         float rEtaL12{0}; // R_eta (layers 1+2)
         bool passClusterEnergy{false}; // passes energy cut of baseline selection
         bool passRHad{false}; // passes R_had cut of baseline selection
         bool passREta{false}; // passes R_eta cut of baseline selection
         bool passWstot{false}; // passes Wstot cut of baseline selection
      };

      EFexEMClusterTool(const std::string& type, const std::string& name, const IInterface* parent);

      /** find cluster and associated variables using a user defined selection */
      std::vector<AlgResult> clusterAlg( bool applyBaselineCuts,
                                         const CaloConstCellContainer* scells, const xAOD::TriggerTowerContainer* TTs,
                                         const CaloCell_SuperCell_ID* idHelper, const TileID* m_tileIDHelper,
                                         const CaloConstCellContainer* tileCellCon ) const;

   private:
      /**
       * algorithm fors cluster building
       */
      /** find cluster and associated variables using a "loose" algorithm */
      std::vector<AlgResult> looseAlg( const CaloConstCellContainer* SCs, const xAOD::TriggerTowerContainer* TTs, 
                                       const CaloCell_SuperCell_ID* idHelper, const TileID* m_tileIDHelper,
                                       const CaloConstCellContainer* tileCellCon) const;


      /* calculate the ET of an input cell */
      float CaloCellET(const CaloCell* const &inputCell, float digitScale, float digitThreshold) const;

      /** helper function calling localMax() */
      bool localMax( const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell, 
                     const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold) const;

      /** tests if the input cell has a local energy maximum with respect to neighbors */
      bool localMax( const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell,
                     int numOthers, const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold) const;

      /** check if both input cells belong to the same TT */
      bool SameTT( const CaloCell* inputCell1, const CaloCell* inputCell2, const CaloCell_SuperCell_ID* &idHelper) const;

      /** calculate cluster energy */
      double EMClusET( const CaloCell* centreCell, int etaWidth, int phiWidth,
                       const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const;

      /** calculate the energy isolation of the central cell along eta */
      double REta( const CaloCell* centreCell, int etaWidth1, int phiWidth1, int etaWidth2, int phiWidth2,
                   const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const;

      /** calculate the hadronic isolation of the central cell */
      double RHad( const CaloCell* centreCell, int etaWidth, int phiWidth,
                   const CaloConstCellContainer* scells, const xAOD::TriggerTowerContainer* &TTContainer,
                   const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh, float &HadronicET) const;

      /** calculate the hadronic isolation for a seed cell using TileCal cells */
      double RHadTile( const CaloCell* centreCell, int etaWidth, int phiWidth,
                       const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh,
                       const TileID* m_tileIDHelper, const CaloConstCellContainer* tileCellCon, float tileNoiseThresh, float &HadronicET) const;

      /** calculate cluster energy of cells in L2 around the central cell in a given eta/phi width */
      double L2clusET( const CaloCell* centreCell, int etaWidth, int phiWidth,
                       const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh) const;

      /** calculate the energy isolation of the central cell along eta using Layer 1 and Layer 2 */
      double REtaL12( const CaloCell* centreCell, int etaWidth1, int phiWidth1, int etaWidth2, int phiWidth2, 
                      const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const;

      /** calculate the lateral isolation aorund the central cell */
      double L1Width( const CaloCell* centreCell, int etaWidth, int phiWidth,
                      const CaloConstCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh) const;

      /** convert the TT phi to match the definition of SC phi  */
      double TT_phi(const xAOD::TriggerTower* &inputTower) const;

      /** calculate deltaR between two points in eta/phi space */
      double dR(double eta1, double phi1, double eta2, double phi2) const;

      /** Match each SC from L2 to one corresponding TT */
      const xAOD::TriggerTower* matchingHCAL_TT( const CaloCell* &inputCell, const xAOD::TriggerTowerContainer* &TTContainer) const;

      /** form the cluster around the central SC */
      std::vector<const CaloCell*> TDR_Clus( const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                                             const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh) const;

      /** calculate cluster energy from all SCs in PS, L1, L2, L3 */
      double sumVectorET( const std::vector<const CaloCell*> &inputVector, float digitScale = 0.,float digitThreshold = 0.) const;

      /** check if conversion from ET to energy after digitization was performed successfully */
      bool checkDig(float EM_ET, float digitScale,float digitThresh) const;

      /** form the cluster from cells of the second layer L2 */
      std::vector<const CaloCell*> L2cluster( const CaloCell* centreCell, int etaWidth, int phiWidth, const CaloConstCellContainer* scells,
                                              const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh ) const;

      /** calculate the energy of an input TT */
      double TT_ET(const xAOD::TriggerTower* &inputTower) const;

      /** calculate the energy in the HCAL (LAr + Tile) for SC/TT that match the EM cluster cells of L2 */
      double HadronicET( const std::vector<const CaloCell*> & inputVector, const CaloConstCellContainer* scells,
                         const xAOD::TriggerTowerContainer* &TTContainer, const CaloCell_SuperCell_ID* idHelper,
                         float digitScale, float digitThresh) const;

      /** match SCs from the cluster in L2 to L1 */
      void fromLayer2toLayer1( const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell,
                               std::vector<const CaloCell*> &outputVector, const CaloCell_SuperCell_ID* &idHelper) const;

      /** match SCs from the cluster in L2 to one cell of PS */
      const CaloCell* fromLayer2toPS(const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell, 
                                     const CaloCell_SuperCell_ID* &idHelper) const;

      /** match SCs from the cluster in L2 to one cell of L3 */
      const CaloCell* fromLayer2toLayer3(const CaloConstCellContainer* &inputContainer, const CaloCell* inputCell,
                                         const CaloCell_SuperCell_ID* &idHelper) const;

      /** adds SC to vector if the SC is not part of this vector yet */
      void addOnce(const CaloCell* inputCell, std::vector<const CaloCell*> &outputVector) const;

      /** match all Tile cells to a given L2Cluster and determine the summed energy per Tile layer */
      std::vector<double> EnergyPerTileLayer(const std::vector<const CaloCell*> & inputSCVector, const CaloConstCellContainer* CellCon,
                                             const TileID* tileIDHelper, bool isOW, float tileNoiseThresh) const;

      /** Match each SC from L2 to one corresponding HCAL SC */
      const CaloCell* matchingHCAL_LAr(const CaloCell* &inputCell, const CaloConstCellContainer* &SCContainer,
                                       const  CaloCell_SuperCell_ID* &idHelper) const;

      /** determine if Tile cell has already been taken into account */
      void checkTileCell(const TileCell* &inputCell, std::vector<const TileCell*> &tileCellVector, bool &isAlreadyThere) const;

      /** determine transverse energy and apply noise threshold to Tile cells */
      double tileCellEnergyCalib(float eIn, float etaIn, float tileNoiseThresh) const;

      /** determine the PMT position of the Tile cell to be matched */
      int detRelPos(const float inEta) const;

      /**
       * helper functions to find neighbouring cells
       */
      /** SC from container is returned according to its ID */
      const CaloCell* returnCellFromCont(Identifier inputID, const CaloConstCellContainer* &cellContainer, const CaloCell_SuperCell_ID* &idHelper) const;

      /** helper function calling NextEtaCell_Barrel(), NextEtaCell_OW(), NextEtaCell_IW() according to position of input cell */
      const CaloCell* NextEtaCell( const CaloCell* inputCell, bool upwards, const CaloConstCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper) const;

      /** returns the SC left/right to the input cell for the barrel */
      const CaloCell* NextEtaCell_Barrel (const CaloCell* inputCell, bool upwards, const CaloConstCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper) const;

      /** returns the SC left/right to the input cell for the OW */
      const CaloCell* NextEtaCell_OW (const CaloCell* inputCell, bool upwards, const CaloConstCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper) const;

      /** returns the SC left/right to the input cell for the IW */
      const CaloCell* NextEtaCell_IW(const CaloCell* inputCell, bool upwards, const CaloConstCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper) const;

      /** manager function for the phi index */
      int restrictPhiIndex(int input_index, bool is64) const;

      /** returns the SC above/below the input cell */
      const CaloCell* NextPhiCell ( const CaloCell* inputCell, bool upwards, 
                                    const CaloConstCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper) const;





      /**
       * properties
       */
      bool m_useProvenance;   ///<  clear up container from bad BC by skipping scells
      int m_qualBitMask;      ///< Configurable quality bitmask

      // baseline selection properties
      float m_clustET_thresh;        ///< threshold for minimum cluster energy (baseline selection)
      float m_clustET_NoIso_thresh;  ///< threshold for applying cluster isolation cuts (baseline selection)
      float m_REta_thresh;           ///< threshold for isolation REta (baseline selection)
      float m_RHad_thresh;           ///< threshold for isolation RHad (baseline selection)
      float m_L1Width_thresh;        ///< threshold for isolation L1Width (wstot) (baseline selection)
      float m_eta_dropL1Width;       ///< max eta for applying cut on L1Width (baseline selection)

      // loose selection properties
      bool m_use_REtaL12;                ///< boolean for caluclating REta using Layer 1 in addition to Layer 2
      bool m_use_tileCells;              ///< boolean for using Tile cells instead of Tile TT
      float m_nominalDigitization;       ///< value of nominal digitisation
      float m_nominalNoise_thresh;       ///< noise threshold
      float m_tileNoise_tresh;           ///< TileCal cell noise threshold
      int m_phiWidth_TDRCluster;         ///< phi width of the TDR cluster formation given in number of SCs (including the central cell), should be 2 or 3
      int m_etaWidth_TDRCluster;         ///< eta width of the TDR cluster formation given in number of SCs (including the central cell), should be >= 1
      int m_etaWidth_wstotIsolation;     ///< eta width for wstot isolation given in number of SCs
      int m_phiWidth_wstotIsolation;     ///< phi width for wstot isolation given in number of SCs
      int m_etaEMWidth_RHadIsolation;    ///< EM eta width for RHad isolation given in number of SCs
      int m_phiEMWidth_RHadIsolation;    ///< EM phi width for RHad isolation given in number of SCs
      int m_etaWidth_REtaIsolation_den;  ///< eta width for REta isolation given in number of SCs (denominator of fraction)
      int m_phiWidth_REtaIsolation_den;  ///< phi width for REta isolation given in number of SCs (denominator of fraction)
      int m_etaWidth_REtaIsolation_num;  ///< eta width for REta isolation given in number of SCs (numerator of fraction)
      int m_phiWidth_REtaIsolation_num;  ///< phi width for REta isolation given in number of SCs (numerator of fraction)
      int m_etaHadWidth_RHadIsolation;   ///< hadronic eta width for RHad isolation given in number of SCs
      int m_phiHadWidth_RHadIsolation;   ///< hadronic phi width for RHad isolation given in number of SCs
      float m_clustET_looseAlg_thresh;   ///< threshold for minimum cluster energy for the loose eFEX algorithm

   };
}

#endif
