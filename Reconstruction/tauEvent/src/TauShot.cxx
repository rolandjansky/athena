/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauEvent/TauShot.h"
#include "tauEvent/TauDetails.h"
//#include <sstream>

namespace Analysis
{
    // Default Constructor
    TauShot::TauShot() 
        : P4PtEtaPhiM()
        , NavigableTerminalNode()
        , m_cluster()
        , m_seedCell()
        , m_nCellsInEta(TauDetails::DEFAULT_INT)                  
        , m_pt1(TauDetails::DEFAULT)
        , m_pt3(TauDetails::DEFAULT)
        , m_pt5(TauDetails::DEFAULT)
        , m_ws5(TauDetails::DEFAULT)
        , m_sdevEta5_WRTmean(TauDetails::DEFAULT)
        , m_sdevEta5_WRTmode(TauDetails::DEFAULT)
        , m_sdevPt5(TauDetails::DEFAULT)
        , m_deltaPt12_min(TauDetails::DEFAULT)
        , m_Fside_3not1(TauDetails::DEFAULT)
        , m_Fside_5not1(TauDetails::DEFAULT)
        , m_Fside_5not3(TauDetails::DEFAULT)
        , m_fracSide_3not1(TauDetails::DEFAULT)
        , m_fracSide_5not1(TauDetails::DEFAULT)
        , m_fracSide_5not3(TauDetails::DEFAULT)
        , m_pt1OverPt3(TauDetails::DEFAULT)
        , m_pt3OverPt5(TauDetails::DEFAULT)
        , m_mergedScore(TauDetails::DEFAULT)
        , m_signalScore(TauDetails::DEFAULT)    
        , m_nPhotons(TauDetails::DEFAULT_INT)
    {}


    // Copy Constructor
    TauShot::TauShot(const TauShot& rhs)
        : IAthenaBarCode(rhs)
        , INavigable(rhs)
        , I4Momentum(rhs)
        , INavigable4Momentum(rhs)
        //, P4PxPyPzEBase(rhs)
        , P4PtEtaPhiM(rhs)
        , NavigableTerminalNode(rhs)
        , m_cluster(rhs.m_cluster)
        , m_seedCell(rhs.m_seedCell)
        , m_nCellsInEta(rhs.m_nCellsInEta)
        , m_pt1(rhs.m_pt1)
        , m_pt3(rhs.m_pt3)
        , m_pt5(rhs.m_pt5)
        , m_ws5(rhs.m_ws5)
        , m_sdevEta5_WRTmean(rhs.m_sdevEta5_WRTmean)
        , m_sdevEta5_WRTmode(rhs.m_sdevEta5_WRTmode)
        , m_sdevPt5(rhs.m_sdevPt5)
        , m_deltaPt12_min(rhs.m_deltaPt12_min)
        , m_Fside_3not1(rhs.m_Fside_3not1)
        , m_Fside_5not1(rhs.m_Fside_5not1)
        , m_Fside_5not3(rhs.m_Fside_5not3)
        , m_fracSide_3not1(rhs.m_fracSide_3not1)
        , m_fracSide_5not1(rhs.m_fracSide_5not1)
        , m_fracSide_5not3(rhs.m_fracSide_5not3)
        , m_pt1OverPt3(rhs.m_pt1OverPt3)
        , m_pt3OverPt5(rhs.m_pt3OverPt5)
        , m_mergedScore(rhs.m_mergedScore)
        , m_signalScore(rhs.m_signalScore)
        , m_nPhotons(rhs.m_nPhotons)
    {
    }
    
    // Assignment operator
    TauShot &TauShot::operator= ( const TauShot& rhs )
    {
        if (this!=&rhs){
            m_cluster  = rhs.m_cluster;
            m_seedCell = rhs.m_seedCell;
            m_nCellsInEta = rhs.m_nCellsInEta;
            m_pt1 = rhs.m_pt1;
            m_pt3 = rhs.m_pt3;
            m_pt5 = rhs.m_pt5;
            m_ws5 = rhs.m_ws5;
            m_sdevEta5_WRTmean = rhs.m_sdevEta5_WRTmean;
            m_sdevEta5_WRTmode = rhs.m_sdevEta5_WRTmode;
            m_sdevPt5 = rhs.m_sdevPt5;
            m_deltaPt12_min = rhs.m_deltaPt12_min;
            m_Fside_3not1 = rhs.m_Fside_3not1;
            m_Fside_5not1 = rhs.m_Fside_5not1;
            m_Fside_5not3 = rhs.m_Fside_5not3;
            m_fracSide_3not1 = rhs.m_fracSide_3not1;
            m_fracSide_5not1 = rhs.m_fracSide_5not1;
            m_fracSide_5not3 = rhs.m_fracSide_5not3;
            m_pt1OverPt3 = rhs.m_pt1OverPt3;
            m_pt3OverPt5 = rhs.m_pt3OverPt5;
            m_mergedScore = rhs.m_mergedScore;
            m_signalScore = rhs.m_signalScore;
            m_nPhotons = rhs.m_nPhotons;
        }
        return *this;
    }

    // Comparison Operator
    bool TauShot::operator== ( const TauShot& rhs ) const
    {
        if(m_cluster != rhs.m_cluster) return false;
        if(m_seedCell != rhs.m_seedCell) return false;
        if(m_nCellsInEta != rhs.m_nCellsInEta) return false;
        if(m_pt1 != rhs.m_pt1) return false;
        if(m_pt3 != rhs.m_pt3) return false;
        if(m_pt5 != rhs.m_pt5) return false;
        if(m_ws5 != rhs.m_ws5) return false;
        if(m_sdevEta5_WRTmean != rhs.m_sdevEta5_WRTmean) return false;
        if(m_sdevEta5_WRTmode != rhs.m_sdevEta5_WRTmode) return false;
        if(m_sdevPt5 != rhs.m_sdevPt5) return false;
        if(m_deltaPt12_min != rhs.m_deltaPt12_min) return false;
        if(m_Fside_3not1 != rhs.m_Fside_3not1) return false;
        if(m_Fside_5not1 != rhs.m_Fside_5not1) return false;
        if(m_Fside_5not3 != rhs.m_Fside_5not3) return false;
        if(m_fracSide_3not1 != rhs.m_fracSide_3not1) return false;
        if(m_fracSide_5not1 != rhs.m_fracSide_5not1) return false;
        if(m_fracSide_5not3 != rhs.m_fracSide_5not3) return false;
        if(m_pt1OverPt3 != rhs.m_pt1OverPt3) return false;
        if(m_pt3OverPt5 != rhs.m_pt3OverPt5) return false;
        if(m_mergedScore != rhs.m_mergedScore) return false;
        if(m_signalScore != rhs.m_signalScore) return false;
        if(m_nPhotons != rhs.m_nPhotons) return false;
        return true;
    }

    // Destructor
    TauShot::~TauShot() 
    {
    }

    
    std::vector<std::vector<const CaloCell*> > TauShot::getCellVector(const CaloCell_ID* calo_id) const
    {
        std::vector<std::vector<const CaloCell*> > cellVector;
        std::vector<const CaloCell*> oneEtaLayer;
        for(int iCell=0; iCell<m_nCellsInEta;++iCell) oneEtaLayer.push_back(NULL);
        // have two layers in phi
        cellVector.push_back(oneEtaLayer);
        cellVector.push_back(oneEtaLayer);
        const CaloCell* seedCell = this->seedCell();
        const CaloCell* mergedCell = NULL;
        // get merged cell in phi. Keep NULL if shot is not merged across phi
        std::vector<IdentifierHash> nextInPhi;
        std::vector<IdentifierHash> prevInPhi;
        calo_id->get_neighbours(seedCell->caloDDE()->calo_hash(),LArNeighbours::nextInPhi,nextInPhi);
        calo_id->get_neighbours(seedCell->caloDDE()->calo_hash(),LArNeighbours::prevInPhi,prevInPhi);
        CaloCluster::cell_iterator cellItr  = this->cluster()->cell_begin();
        CaloCluster::cell_iterator cellItrE = this->cluster()->cell_end();
        for(;cellItr!=cellItrE;++cellItr){
            std::vector<IdentifierHash>::iterator itr = nextInPhi.begin();
            for( ; itr!=nextInPhi.end(); ++itr ){
                if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                mergedCell = (*cellItr);
                break;
            }
            if(mergedCell!=NULL) break;
            itr = prevInPhi.begin();
            for( ; itr!=prevInPhi.end(); ++itr ){
                if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                mergedCell = (*cellItr);
                break;
            }
            if(mergedCell!=NULL) break;
        }
        // store cells in the eta layer, which contains the seed cell
        int nCellsFromSeed = 1;
        const CaloCell* lastCell = seedCell;
        cellVector.at(0).at(m_nCellsInEta/2) = seedCell; // store seed cell
        std::vector<IdentifierHash> next;
        while(lastCell!=NULL && nCellsFromSeed<m_nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::nextInEta,next);
            lastCell = NULL;
            for(cellItr=this->cluster()->cell_begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(0).at(m_nCellsInEta/2+nCellsFromSeed) = (*cellItr);
                    lastCell = (*cellItr);
                }
            }
            nCellsFromSeed++;
        }
        nCellsFromSeed = 1;
        lastCell = seedCell;
        while(lastCell!=NULL && nCellsFromSeed<m_nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::prevInEta,next);
            lastCell = NULL;
            for(cellItr=this->cluster()->cell_begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(0).at(m_nCellsInEta/2-nCellsFromSeed) = (*cellItr);
                    lastCell = (*cellItr);
                }
            }
            nCellsFromSeed++;
        }
        // store cells in the eta layer, which contains the merged cell
        int nCellsFromMerged = 1;
        lastCell = mergedCell; // is NULL if shot is not merged
        cellVector.at(1).at(m_nCellsInEta/2) = mergedCell; // store merged cell
        while(lastCell!=NULL && nCellsFromMerged<m_nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::nextInEta,next);
            lastCell = NULL;
            for(cellItr=this->cluster()->cell_begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(1).at(m_nCellsInEta/2+nCellsFromMerged) = (*cellItr);
                    lastCell = (*cellItr);
                }
            }
            nCellsFromMerged++;
        }
        nCellsFromMerged = 1;
        lastCell = mergedCell;
        while(lastCell!=NULL && nCellsFromMerged<m_nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::prevInEta,next);
            lastCell = NULL;
            for(cellItr=this->cluster()->cell_begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(1).at(m_nCellsInEta/2-nCellsFromMerged) = (*cellItr);
                    lastCell = (*cellItr);
                } 
            }
            nCellsFromMerged++;
        }
        return cellVector;
    }
    

    // just for testing
    void TauShot::print() const{
        int oldpr = std::cout.precision(5);
        // can probably do this better through IMomentum::dump(), but whatevs
        std::cout << "in TauShot::dump()" << std::endl;
        std::cout << "pt:  " << this->pt()  << std::endl;
        std::cout << "eta: " << this->eta() << std::endl;
        std::cout << "phi: " << this->phi() << std::endl;
        std::cout << "m:   " << this->m()   << std::endl;
        std::cout.precision(oldpr);
    }

}//end of namespace definitions

