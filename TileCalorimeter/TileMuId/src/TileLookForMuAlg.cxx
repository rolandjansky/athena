/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileLookForMuAlg.cxx
// Author   : G Usai
// Created  : June 2003 
//***************************************************************************
//Library Includes
#include <algorithm>
#include <cmath>
// Athena includes
#include "AthenaKernel/errorcheck.h"
// Calo includes
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloCell_ID.h"
// Tile includes
#include "TileEvent/TileCell.h"
#include "TileEvent/TileMuContainer.h"
#include "TileMuId/TileLookForMuAlg.h"

TileLookForMuAlg::TileLookForMuAlg(const std::string& name,
    ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_etaD()
  , m_etaBC()
  , m_etaA()
  , m_nMuMax(30)
{
  declareProperty("CellsNames", m_cellContainer);
  declareProperty("TileMuTagsOutputName", m_tileTagContainer);
  declareProperty("LowerTresh0MeV", m_loThrA=80.0);
  declareProperty("LowerTresh1MeV", m_loThrBC=80.0);
  declareProperty("LowerTresh2MeV", m_loThrD=80.0);
  declareProperty("LowerTreshScinMeV", m_loThrITC=160.0);
  declareProperty("UpperTresh2MeV", m_hiThrD);
  declareProperty("UpperTresh1MeV", m_hiThrBC);
  declareProperty("UpperTresh0MeV", m_hiThrA);
  declareProperty("From3to2", m_fromDtoBC);
  declareProperty("From2to1", m_fromBCtoA);
  return;
}

TileLookForMuAlg::~TileLookForMuAlg() {
}

//////////////////////////////////////////////////////////////////////////////
//Initialization                                                            /
/////////////////////////////////////////////////////////////////////////////
StatusCode TileLookForMuAlg::initialize() {

  // retrieve TileID helper and TileIfno from det store

  CHECK( detStore()->retrieve(m_tileID) );


  // define a numbering scheme for the cells  
  double eta_D = -1.2;
  for (int iCellD = 0; iCellD < N_CELLS_D; ++iCellD) {
    m_etaD[iCellD] = eta_D;
    eta_D += 0.2;
  }

  if (msgLvl(MSG::DEBUG)) {
    for (int iCellD = 0; iCellD < N_CELLS_D; ++iCellD) {
      msg(MSG::DEBUG) << " etaD[" << iCellD << "] = " << m_etaD[iCellD] << endmsg;
    }
  }


  double eta_BC_A = -1.45;
  for (int iCell = 0; iCell < N_CELLS_BC; ++iCell) {
    m_etaBC[iCell] = eta_BC_A;
    m_etaA[iCell] = eta_BC_A;
    eta_BC_A += 0.1;
  }


  if (msgLvl(MSG::DEBUG)) {
    for (int iCell = 0; iCell < N_CELLS_BC; ++iCell) {
      msg(MSG::DEBUG) << " etaBC[" << iCell << "] = " << m_etaBC[iCell] << endmsg;
      msg(MSG::DEBUG) << " etaA[" << iCell << "] = " << m_etaA[iCell] << endmsg;
    }
  }

  ATH_MSG_DEBUG("TileLookForMuAlg initialization completed");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
//Execution                                                                     /
/////////////////////////////////////////////////////////////////////////////////
StatusCode TileLookForMuAlg::execute() {
  double eneA[N_CELLS_A][N_MODULES]; // calorimeter cell matrices  
  double eneBC[N_CELLS_BC][N_MODULES];
  double eneD[N_CELLS_D][N_MODULES];

  memset(eneA, 0, sizeof(eneA));
  memset(eneBC, 0, sizeof(eneBC));
  memset(eneD, 0, sizeof(eneD));

  TileMuContainer* muons = new TileMuContainer; //muon tag container

  ATH_MSG_DEBUG("TileLookForMuAlg execution  started");

  //  Get CaloCell  Container

  std::vector<const CaloCell*> cellList;
  const CaloCellContainer* cell_container;

  if (evtStore()->retrieve(cell_container, m_cellContainer).isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve the   " << m_cellContainer);
    return StatusCode::SUCCESS;
  }

  CaloCell_ID::SUBCALO tileCell_ID = CaloCell_ID::TILE;
  CaloCellContainer::const_iterator currentCell =
      cell_container->beginConstCalo(tileCell_ID);
  CaloCellContainer::const_iterator lastCell = 
      cell_container->endConstCalo(tileCell_ID);

  ATH_MSG_DEBUG( "Calo Container size "
                << cell_container->nCellsCalo(tileCell_ID));

  double phi[64] = { 0 };
//  TileID::SAMPLE cellSample;
  for (; currentCell != lastCell; ++currentCell) {
    int iCell = -1;
    double cellEta = (*currentCell)->eta();
    if (cellEta > -1.5 && cellEta < 1.5) {
      int cellModule;
      int cellSample = m_tileID->sample((*currentCell)->ID());
      switch (cellSample) {
        case TileID::SAMP_A:
          iCell = (cellEta + 1.5) * 10;
          cellModule = m_tileID->module((*currentCell)->ID());
          eneA[iCell][cellModule] = (*currentCell)->energy();
          phi[cellModule] = (*currentCell)->phi();
          break;
        case TileID::SAMP_BC:
          iCell = (cellEta + 1.5) * 10;
          cellModule = m_tileID->module((*currentCell)->ID());
          eneBC[iCell][cellModule] = (*currentCell)->energy();
          phi[cellModule] = (*currentCell)->phi();
          break;
        case TileID::SAMP_D:
          iCell = (cellEta + 1.3) * 5;
          cellModule = m_tileID->module((*currentCell)->ID());
          eneD[iCell][cellModule] = (*currentCell)->energy();
          phi[cellModule] = (*currentCell)->phi();
          cellList.push_back(*currentCell);
          break;
        case TileID::SAMP_E:
          iCell = (cellEta + 1.5) * 10;
          if (iCell == 4 || iCell == 25) {
            cellModule = m_tileID->module((*currentCell)->ID());
            eneA[iCell][cellModule] = (*currentCell)->energy();
            phi[cellModule] = (*currentCell)->phi();
          }
          break;
      }
    }

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "scintillators sample, eta, phi, ene => "
                        << m_tileID->sample((*currentCell)->ID()) << ","
                        << cellEta << ","
                        << (*currentCell)->phi() << ","
                        << (*currentCell)->energy() << endmsg;

      msg(MSG::VERBOSE) << "sample, tower, eta => " 
                        << m_tileID->sample((*currentCell)->ID()) << ", "
                        << m_tileID->tower((*currentCell)->ID()) << ", "
                        << cellEta << endmsg;
    }
  } /* end of Tile cells loop*/

  std::vector<double> muEtaD;
  std::vector<float> muEneD;
  std::vector<int> muModule;
  std::vector<int> muCellD;
  std::vector<int> muSplitted;
  std::vector<int> muQualityD;
  std::vector<int> muFound;
  muEtaD.reserve(m_nMuMax);
  muEneD.reserve(m_nMuMax);
  muModule.reserve(m_nMuMax);
  muCellD.reserve(m_nMuMax);
  muSplitted.reserve(m_nMuMax);
  muQualityD.reserve(m_nMuMax);
  muFound.reserve(m_nMuMax);

  int nCandidates = 0, ntri = 0;
  int nSplitted = 0;

  ATH_MSG_VERBOSE("Start the  muon search candidates ");

  // find muon candidates
  int lastMuCell = -3;
  int lastMuModule = -3;
  for (int iModule = 0; iModule < N_MODULES; ++iModule) {
    int prevCell = -2;
    for (int iCellD = 0; iCellD < N_CELLS_D; ++iCellD) {
      float energy = eneD[iCellD][iModule];
      if (energy >= m_loThrD) {
        int splitted = -1;
        double eta = m_etaD[iCellD];
        double hiThr = m_hiThrD[iCellD];
        int quality = (energy < hiThr) ? 0 : 1;
        // check for muon splitting (same phi bin and neighboring eta bin)
        if (prevCell == lastMuCell && iModule == lastMuModule) {
          int sumQuality = quality + muQualityD.back();
          float sumEnergy = energy + eneD[prevCell][iModule];
          double hiPrevThr = m_hiThrD[prevCell];
          double maxHiThr = (hiThr > hiPrevThr) ? hiThr : hiPrevThr;
          if ((sumQuality == 0 && sumEnergy < maxHiThr) || sumQuality == 1) {
            // possible splitting with last found muon candidate 
            splitted = nCandidates - 1; // idx of splitting mu candidate
            energy = sumEnergy;
            eta = (eta + m_etaD[prevCell]) / 2;
            ++nSplitted;
            ATH_MSG_VERBOSE( "Possible splits between mu candidates ("
                            << (splitted + 1) << ", " << splitted
                            << "): etaD1, etaD2, eta => "
                            << m_etaD[iCellD] << ", "
                            << m_etaD[prevCell] << ", "
                            << eta
                            << "; eneD1, eneD2, energy => "
                            << eneD[iCellD][iModule] << ", "
                            << eneD[prevCell][iModule] << ", "
                            << energy);
          }
        }
        muModule.push_back(iModule);
        muCellD.push_back(iCellD);
        muSplitted.push_back(splitted);
        muFound.push_back(0);
        muEneD.push_back(energy);
        muQualityD.push_back(quality);
        muEtaD.push_back(eta);
        ++nCandidates;
        lastMuCell = iCellD;
        lastMuModule = iModule;
        ATH_MSG_VERBOSE( "Candidate  number= " << nCandidates
                        << ", tower index (iCellD)= " << iCellD
                        << ", module index(iModuleD)= " << iModule
                        << ", Energy(iCellD)(iModuleD) = " << eneD[iCellD][iModule]
                        << ", threshold2(iCellD)= " << m_hiThrD[iCellD]);
      } else {
        // ATH_MSG_VERBOSE ( "no candidates" );
      }
      prevCell = iCellD;
    }
  }

  // debug ---------------------------------------- 
  if (msgLvl(MSG::VERBOSE)) {
    for (int i = 0; i < nCandidates; ++i) {
      msg(MSG::VERBOSE) << "Candidates list: number   phi,ene, eta "
                        << i << ","
                        << muModule[i] << ","
                        << muEneD[i] << ","
                        << muCellD[i]
                        << "nSplitted,muSplitted(cand)" << nSplitted << ", "
                        << muSplitted[i] << endmsg;
    }
  }

  //*************** loop on the candidates----------------------* 

  for (int iMu = 0; iMu < nCandidates; ++iMu) {
    int splitted = muSplitted[iMu];
    if (splitted < 0 || muFound[splitted] == 0) {

      // the most  important information on mu  is in the 3rd sample
      // to avoid multiple counting due to splitting in  2nd and 1st Sp. cells
      // the  loop  stop  when the  candidate is found

      int module = muModule[iMu];
      ATH_MSG_VERBOSE(
          "loop on mu candidates: iMu, module = " << iMu << ", " << module);
      int idxD = 6 * muCellD[iMu];
      ATH_MSG_VERBOSE ("number of cells in BC layer to check = " << m_fromDtoBC[idxD]);
      int endIdxD = idxD + m_fromDtoBC[idxD];
      while (++idxD <= endIdxD && muFound[iMu] != 1) {
        int cellBC = m_fromDtoBC[idxD];
        float energyBC = eneBC[cellBC][module];
        ATH_MSG_VERBOSE( "cellBC = " << cellBC
                        << ", module=" << module
                        << ", eneBC =" << energyBC);
        if (energyBC > m_loThrBC) {
          int qualityBC = (energyBC < m_hiThrBC[cellBC]) ? 0 : 1;
          int idxBC = 6 * cellBC;
          ATH_MSG_VERBOSE ("number of cells in A layer to check for mu = " << m_fromBCtoA[idxBC]);
          int endIdxBC = idxBC + m_fromBCtoA[idxBC];
          while (++idxBC <= endIdxBC && muFound[iMu] != 1) {
            int cellA = m_fromBCtoA[idxBC];
            float energyA = eneA[cellA][module];
            ATH_MSG_VERBOSE( "cellA index = " << cellA
                            << ", module=" << module
                            << ", eneA =" << energyA);

            if ( energyA >  ( (cellA == 4 || cellA == 25) ? m_loThrITC : m_loThrA ) ) {

              int qualityA = (energyA < m_hiThrA[cellA]) ? 0 : 1;

              // We have a   muon like signature  

              int muQuality = muQualityD[iMu] + qualityBC + qualityA;
              if (muQuality <= 1) {
                muFound[iMu] = 1;
                double muEta = (muEtaD[iMu] + m_etaBC[cellBC] + m_etaA[cellA]) / 3;
                double muPhi = phi[module];
                std::vector<float> muEnergy;
                muEnergy.reserve(4);
                muEnergy.push_back(energyA);
                muEnergy.push_back(energyBC);
                muEnergy.push_back(muEneD[iMu]);
                float eneAround = 0;

                ATH_MSG_VERBOSE( "tag ntri eta,phi,ene[0]= " << (++ntri) << ", "
                                << muEta << ", "
                                << muPhi << ", "
                                << muEnergy[0] << ", "
                                << muEnergy[1] << ", "
                                << muEnergy[2] << ", "
                                << muEnergy[3]
                                << " tag  eta 1st, 2nd, 3rd,"
                                << m_etaA[cellA] << ", "
                                << m_etaBC[cellBC] << ", "
                                << m_etaD[muCellD[iMu]]);

                int nextModule = (module != 63) ? (module + 1) : 0;
                int prevModule = (module != 0) ? (module - 1) : 63;
                eneAround = eneA[cellA][nextModule] + eneA[cellA][prevModule]
                    + eneBC[cellBC][nextModule] + eneBC[cellBC][prevModule]; //phi neigh

                int nextCellA = cellA + 1;
                int prevCellA = cellA - 1;
                if (nextCellA < N_CELLS_A && prevCellA > 0) {
                  eneAround += eneA[nextCellA][module] + eneA[prevCellA][module]
                      + eneA[nextCellA][nextModule]
                      + eneA[nextCellA][prevModule]
                      + eneA[prevCellA][nextModule]
                      + eneA[prevCellA][prevModule];

                }

                int nextCellBC = cellBC + 1;
                int prevCellBC = cellBC - 1;
                if (nextCellBC < N_CELLS_BC && prevCellBC > 0) {
                  eneAround += eneBC[nextCellBC][module]
                      + eneBC[prevCellBC][module]
                      + eneBC[nextCellBC][nextModule]
                      + eneBC[nextCellBC][prevModule]
                      + eneBC[prevCellBC][nextModule]
                      + eneBC[prevCellBC][prevModule];

                }

                muEnergy.push_back(eneAround);

                TileMu* muon = new TileMu((float) muEta, (float) muPhi,
                    muEnergy, muQuality);
                muons->push_back(muon);
                ATH_MSG_VERBOSE( "muon tag eta=" << muon->eta()
                                << " muon tag phi=" << muon->phi()
                                << " energydepVec[0]=" << muon->enedep()[0]
                                << " energydepVec[1]=" << muon->enedep()[1]
                                << " energydepVec[2]=" << muon->enedep()[2]
                                << " energydepVec[3]=" << muon->enedep()[3]
                                << " muon tag Q factor=" << muon->quality()
                                << " ene around= " << eneAround);

              }

            } else {
              ATH_MSG_VERBOSE(" tag eneA out");
            } //endif eneA is in the range
          } //end loop on pattern  1st layer
        } else {
          ATH_MSG_VERBOSE(" tag eneBC out");
        } //endif eneBC is in the range
      } //endloop on pattern 2nd layer
    } else {
      break;
    }
  } //endloop on candidate 

//-----debug-----------------------------------------------
  // cellList contain the 3rd layer cell information 
  if (msgLvl(MSG::VERBOSE)) {
    for (const CaloCell* cell : cellList) {
      msg(MSG::VERBOSE) << " tag Cell (sect,side,mod,tow,sam)=("
                        << m_tileID->section(cell->ID()) << " "
                        << m_tileID->side(cell->ID()) << " "
                        << m_tileID->module(cell->ID()) << " "
                        << m_tileID->tower(cell->ID()) << " "
                        << m_tileID->sample(cell->ID())
                        << "),(eta,phi,energy)=("
                        << cell->eta() << ","
                        << cell->phi() << ","
                        << cell->energy() << ")" << endmsg;
    }
  }

  //debug--------------------------------------------------- 
  // check the mu tag container  
  if (msgLvl(MSG::DEBUG)) {
    TileMuContainer::const_iterator ind = muons->begin();
    TileMuContainer::const_iterator lastpippo = muons->end();
    for (; ind != lastpippo; ++ind) {
      msg(MSG::DEBUG) << "Container name = " << m_tileTagContainer
                      << "  eta = " << (*ind)->eta()
                      << "  phi = " << (*ind)->phi()
                      << "  enedep[0] = " << ((*ind)->enedep())[0]
                      << "  enedep[1] = " << ((*ind)->enedep())[1]
                      << "  enedep[2] = " << ((*ind)->enedep())[2]
                      << "  enedep[3] = " << ((*ind)->enedep())[3]
                      << "  quality = " << (*ind)->quality() << endmsg;
    }
  }
  //-------------------debug-------------------------

  //store the muon tags container  on the  TDS 
  CHECK( evtStore()->record(muons, m_tileTagContainer, false) );

  return StatusCode::SUCCESS;
}

StatusCode TileLookForMuAlg::finalize() {
  ATH_MSG_DEBUG("TileLookForMuAlg finalization completed");
  return StatusCode::SUCCESS;
}

