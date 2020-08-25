/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//-----------------------------------------------------------------------------
// file:        TauShotFinder.cxx
// package:     Reconstruction/tauRec
// authors:     Will Davey, Benedict Winter, Stephanie Yuen
// date:        2013-05-22
//
//-----------------------------------------------------------------------------

#include <boost/scoped_ptr.hpp>

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
#include "TauShotFinder.h"
#include "TauShotVariableHelpers.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/PFO.h"

using std::vector;
using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauShotFinder::TauShotFinder(   const string& name ) :
    TauRecToolBase(name) {
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauShotFinder::~TauShotFinder() {
}

StatusCode TauShotFinder::initialize() {
    
    // retrieve tools
    ATH_MSG_DEBUG( "Retrieving tools" );
    CHECK( m_caloWeightTool.retrieve() );

    ATH_CHECK( m_caloCellInputContainer.initialize() );

    // initialize calo cell geo
    ATH_CHECK( detStore()->retrieve (m_calo_id, "CaloCell_ID") );

    return StatusCode::SUCCESS;
}

StatusCode TauShotFinder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TauShotFinder::executeShotFinder(xAOD::TauJet& pTau, xAOD::CaloClusterContainer& tauShotClusterContainer,
					    xAOD::PFOContainer& tauShotPFOContainer) const {

    ATH_MSG_DEBUG("execute");
    // Any tau needs to have shot PFO vectors. Set empty vectors before nTrack cut
    vector<ElementLink<xAOD::PFOContainer> > empty;
    pTau.setShotPFOLinks(empty);
    
    //---------------------------------------------------------------------
    // only run shower subtraction on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
       return StatusCode::SUCCESS;
    }
    //---------------------------------------------------------------------
    // retrieve cells around tau 
    //---------------------------------------------------------------------
    // get all calo cell container
    SG::ReadHandle<CaloCellContainer> caloCellInHandle( m_caloCellInputContainer );
    if (!caloCellInHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << caloCellInHandle.key());
      return StatusCode::FAILURE;
    }
    const CaloCellContainer *pCellContainer = caloCellInHandle.cptr();;
    
    // get only EM cells within dR<0.4
    vector<CaloCell_ID::SUBCALO> emSubCaloBlocks;
    emSubCaloBlocks.push_back(CaloCell_ID::LAREM);
    boost::scoped_ptr<CaloCellList> pCells(new CaloCellList(pCellContainer,emSubCaloBlocks)); 
    pCells->select(pTau.eta(), pTau.phi(), 0.4); 

    // Dump cells into a std::vector since CaloCellList wont allow sorting
    // Also apply very basic preselection
    std::vector<const CaloCell*> cells;
    CaloCellList::list_iterator cellItr = pCells->begin();
    for(; cellItr!=pCells->end();++cellItr){
        // require cells above 100 MeV
        if( (*cellItr)->pt()*m_caloWeightTool->wtCell(*cellItr) < 100. ) continue;
        // require cells in EM1 
        int samp = (*cellItr)->caloDDE()->getSampling();
        if( !( samp == CaloCell_ID::EMB1 || samp == CaloCell_ID::EME1 ) ) continue;
        cells.push_back(*cellItr);
    }
    // sort cells in descending pt    
    std::sort(cells.begin(),cells.end(),ptSort(*this));
    
    //---------------------------------------------------------------------
    // shot seeding 
    //---------------------------------------------------------------------
    // get seed cells
    std::vector<const CaloCell*> seedCells; 
    std::set<IdentifierHash> seedCellHashes;
    cellItr = cells.begin();
    for(; cellItr != cells.end(); ++cellItr) {
        const CaloCell* cell = (*cellItr);
        const IdentifierHash cellHash = cell->caloDDE()->calo_hash();

        // apply seed selection on nearest neighbours
        std::vector<IdentifierHash> nextEta, prevEta;
        m_calo_id->get_neighbours(cellHash,LArNeighbours::nextInEta,nextEta);
        m_calo_id->get_neighbours(cellHash,LArNeighbours::prevInEta,prevEta);
        std::vector<IdentifierHash> neighbours = nextEta;
        neighbours.insert(neighbours.end(),prevEta.begin(),prevEta.end()); 
        bool status = true;
        std::vector<IdentifierHash>::iterator hashItr = neighbours.begin();
        for(;hashItr!=neighbours.end();++hashItr){
            // must not be next to seed cell (TODO: maybe this requirement can be removed)
            if( seedCellHashes.find(*hashItr) != seedCellHashes.end() ){
                status = false;
                break;
            }
            // must be maximum
            const CaloCell* neigCell = pCellContainer->findCell(*hashItr);
            if( !neigCell ) continue;
            if( neigCell->pt()*m_caloWeightTool->wtCell(neigCell) >= cell->pt()*m_caloWeightTool->wtCell(cell) ){
                status = false;
                break;
            }
        }
        if( !status ) continue;        
        seedCells.push_back(cell); 
        seedCellHashes.insert(cellHash);
    } // preselected cells
    ATH_MSG_DEBUG("seedCells.size() = " << seedCells.size());
    
    // merge across phi and construct shots
    while( seedCells.size() ){
        
        const CaloCell* cell = seedCells.front(); 
        const IdentifierHash seedHash = cell->caloDDE()->calo_hash();

        // look for match across phi in current seeds
        const CaloCell* nextPhi = NULL;
        const CaloCell* prevPhi = NULL;
        for( cellItr = seedCells.begin(); cellItr!=seedCells.end(); ++cellItr){
            if( (*cellItr) == cell ) continue;
            IdentifierHash shotCellHash = (*cellItr)->caloDDE()->calo_hash();
            if( this->isPhiNeighbour(seedHash,shotCellHash,true) )       nextPhi = (*cellItr);
            else if( this->isPhiNeighbour(seedHash,shotCellHash,false) ) prevPhi = (*cellItr);
        }
       
        const CaloCell* mergePhi = NULL;
        if( nextPhi && prevPhi ){
            // take higest-pt if merged up and down
            if( nextPhi->pt()*m_caloWeightTool->wtCell(nextPhi) > prevPhi->pt()*m_caloWeightTool->wtCell(prevPhi) ) mergePhi = nextPhi;
            else                                mergePhi = prevPhi;
        }
        else if (nextPhi) mergePhi = nextPhi;
        else if (prevPhi) mergePhi = prevPhi;

        // get neighbours in 5x1 window
        std::vector<const CaloCell*> windowNeighbours = this->getNeighbours(pCellContainer,cell,2);
        if( mergePhi ){
            std::vector<const CaloCell*> mergeCells = this->getNeighbours(pCellContainer,mergePhi,2);
            windowNeighbours.push_back(mergePhi);
            windowNeighbours.insert(windowNeighbours.end(),mergeCells.begin(),mergeCells.end());
        }

        
        // create seed cluster
        xAOD::CaloCluster* shotCluster = CaloClusterStoreHelper::makeCluster(pCellContainer);
        shotCluster->getOwnCellLinks()->reserve(windowNeighbours.size()+1);
        shotCluster->addCell(pCellContainer->findIndex(seedHash), 1.);
        cellItr = windowNeighbours.begin();
        for( ; cellItr!=windowNeighbours.end(); ++cellItr)
            shotCluster->addCell(pCellContainer->findIndex((*cellItr)->caloDDE()->calo_hash()),1.0);
        CaloClusterKineHelper::calculateKine(shotCluster,true,true);
        tauShotClusterContainer.push_back(shotCluster);
        
        // create shot PFO and store it in output container
        xAOD::PFO* shot = new xAOD::PFO();
        tauShotPFOContainer.push_back( shot );

        // Create element link from tau to shot
        ElementLink<xAOD::PFOContainer> PFOElementLink;
        PFOElementLink.toContainedElement( tauShotPFOContainer, shot );
        pTau.addShotPFOLink( PFOElementLink );
       
        if( mergePhi ){
            // interpolate position
            double dPhi = TVector2::Phi_mpi_pi( mergePhi->phi() - cell->phi());
            double ratio = mergePhi->pt()*m_caloWeightTool->wtCell(mergePhi)/(cell->pt()*m_caloWeightTool->wtCell(cell) + mergePhi->pt()*m_caloWeightTool->wtCell(mergePhi));
            float phi = cell->phi()+dPhi*ratio;
            float pt = cell->pt()*m_caloWeightTool->wtCell(cell)+mergePhi->pt()*m_caloWeightTool->wtCell(mergePhi);

            shot->setP4( (float) pt, (float) cell->eta(), (float) phi, (float) cell->m());
        }
        else shot->setP4( (float) cell->pt()*m_caloWeightTool->wtCell(cell), (float) cell->eta(), (float) cell->phi(), (float) cell->m());
        
        shot->setBDTPi0Score( (float) -9999. );
        shot->setCharge( 0. );
        double center_mag = 0.0;
        shot->setCenterMag( (float) center_mag);
        
        ElementLink<xAOD::CaloClusterContainer> clusElementLink;
        clusElementLink.toContainedElement( tauShotClusterContainer, shotCluster );
        shot->setClusterLink( clusElementLink );
        shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_nCellsInEta, m_nCellsInEta);
        shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHash);

        // Get cell block for variable calculations
        std::vector<std::vector<const CaloCell*> > cellBlock = TauShotVariableHelpers::getCellBlock(shot, m_calo_id);

        // Some DEBUG statements
        if (msgLvl(MSG::DEBUG)) { 
          if(cell->pt()*m_caloWeightTool->wtCell(cell)>300){
            ATH_MSG_DEBUG("New shot. \t block size phi = " << cellBlock.size() << " \t block size eta = " << cellBlock.at(0).size() << "\t shot->pt() = " << shot->pt());
            for(unsigned iCellPhi = 0; iCellPhi<cellBlock.size();++iCellPhi){
              for(unsigned iCellEta = 0; iCellEta<cellBlock.at(iCellPhi).size();++iCellEta){
                const CaloCell* cell = cellBlock.at(iCellPhi).at(iCellEta);
                if( cell==NULL ) ATH_MSG_DEBUG("Cell" << iCellPhi << iCellEta << ": \t NULL" );
                else            ATH_MSG_DEBUG("Cell"<<iCellPhi<<iCellEta<<":\tPt = "<< cell->pt()*m_caloWeightTool->wtCell(cell)<<"\teta = "<<cell->eta()<<"\tphi = "<<cell->phi());
              }
            }
          }
        }
        // Get eta bin
        int etaBin = getEtaBin(cell->eta());

        // set variables used for photon counting
        float pt1=TauShotVariableHelpers::ptWindow(cellBlock,1,m_caloWeightTool);
        float pt3=TauShotVariableHelpers::ptWindow(cellBlock,3,m_caloWeightTool);
        float pt5=TauShotVariableHelpers::ptWindow(cellBlock,5,m_caloWeightTool);

        // Calculate number of photons in shot
        int nPhotons = getNPhotons(etaBin, pt1);

        // Set variables in shot PFO
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt1, pt1);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt3, pt3);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt5, pt5);
        shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_nPhotons, nPhotons);

        // remove shot(s) from list
        vector<const CaloCell*>::iterator cellItrNonConst;
        cellItrNonConst = std::find(seedCells.begin(),seedCells.end(),cell);
        seedCells.erase(cellItrNonConst);
        if( mergePhi ){
            cellItrNonConst = std::find(seedCells.begin(),seedCells.end(),mergePhi);
            seedCells.erase(cellItrNonConst);
        }
    } // seed cells
    
    
    return StatusCode::SUCCESS;
}

std::vector<const CaloCell*> TauShotFinder::getNeighbours(const CaloCellContainer* pCellContainer, 
							  const CaloCell* cell, 
							  int maxDepth) const
{
    std::vector<const CaloCell*> cells;
    this->addNeighbours(pCellContainer,cell,cells,0,maxDepth,true);  //next
    this->addNeighbours(pCellContainer,cell,cells,0,maxDepth,false); //prev
    return cells; 
}

void TauShotFinder::addNeighbours(const CaloCellContainer* pCellContainer,
                                  const CaloCell* cell, 
                                  std::vector<const CaloCell*>& cells,
                                  int depth,
                                  int maxDepth,
                                  bool next) const
{
    depth++; 
    if( depth > maxDepth ) return;

    const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
    std::vector<IdentifierHash> neigHashes;
    if( next )
        m_calo_id->get_neighbours(cellHash,LArNeighbours::nextInEta,neigHashes);
    else
        m_calo_id->get_neighbours(cellHash,LArNeighbours::prevInEta,neigHashes);
    
    std::vector<IdentifierHash>::iterator hashItr = neigHashes.begin();
    for( ; hashItr!=neigHashes.end(); ++hashItr ){
        const CaloCell* newCell = pCellContainer->findCell(*hashItr);
        if(!newCell)continue;
        cells.push_back(newCell);
        this->addNeighbours(pCellContainer,newCell,cells,depth,maxDepth,next);
        // no EM1 cell should have more than one neighbor. Just add this neigbor for now
        // FIXME: Check whether it happens that a cell has > 1 neighbors
        break; 
    } 
}

bool TauShotFinder::isPhiNeighbour(IdentifierHash cell1Hash, IdentifierHash cell2Hash, bool next) const{
    std::vector<IdentifierHash> neigHashes;
    if( next ) m_calo_id->get_neighbours(cell1Hash,LArNeighbours::nextInPhi,neigHashes);
    else       m_calo_id->get_neighbours(cell1Hash,LArNeighbours::prevInPhi,neigHashes);
    std::vector<IdentifierHash>::iterator itr = neigHashes.begin();
    for( ; itr!=neigHashes.end(); ++itr ){
        if(cell2Hash == (*itr)) return true;
    } 
    return false;
}

float TauShotFinder::getEtaBin(float seedEta) const {
    float absSeedEta=std::abs(seedEta);
    if(absSeedEta < 0.80)      return 0; // Central Barrel
    else if(absSeedEta<1.39) return 1; // Outer Barrel
    else if(absSeedEta<1.51) return 2; // crack
    else if(absSeedEta<1.80) return 3; // endcap, fine granularity
    else return 4;                           // endcap, coarse granularity
}

float TauShotFinder::getNPhotons(int etaBin, float seedEnergy) const {
    // no photon counting in crack region, e.g. [1.39, 1.51]
    if(etaBin==2) return 0;

    const std::vector<float>& minPtCut = m_minPtCut.value();
    const std::vector<float>& autoDoubleShotCut = m_autoDoubleShotCut.value();
    ATH_MSG_DEBUG("etaBin = " << etaBin  << ", seedEnergy = " << seedEnergy);
    ATH_MSG_DEBUG("MinPtCut: " << minPtCut.at(etaBin) << "DoubleShotCut: " << autoDoubleShotCut.at(etaBin));

    if( seedEnergy < minPtCut.at(etaBin) ) return 0;
    if( seedEnergy > autoDoubleShotCut.at(etaBin) ) return 2;
    return 1;
}

// some really slick c++ way of doing sort (since we need to use the member m_caloWeightTool)
TauShotFinder::ptSort::ptSort( const TauShotFinder& info ) : m_info(info) { } 
bool TauShotFinder::ptSort::operator()( const CaloCell* c1, const CaloCell* c2 ){
     return  c1->pt()*m_info.m_caloWeightTool->wtCell(c1) > c2->pt()*m_info.m_caloWeightTool->wtCell(c2);  
}

#endif
