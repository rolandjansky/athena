/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
/**
 * @brief implementation of photon shot variable calculation 
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch>
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 */

#include "TauShotVariableHelpers.h"

using xAOD::PFO;
using std::vector;

namespace TauShotVariableHelpers {
    std::vector<std::vector<const CaloCell*> > getCellBlock(xAOD::PFO* shot, const CaloCell_ID* calo_id){
        std::vector<std::vector<const CaloCell*> > cellVector;
        std::vector<const CaloCell*> oneEtaLayer;
        int nCellsInEta = 0;
        if( shot->attribute(xAOD::PFODetails::PFOAttributes::tauShots_nCellsInEta, nCellsInEta) == false) {
            std::cout << "WARNING: Couldn't find nCellsInEta. Return empty cell block." << std::endl;
            return cellVector;
        }
        int seedHash = 0;
        if( shot->attribute(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHash) == false) {
            std::cout << "WARNING: Couldn't find seed hash. Return empty cell block." << std::endl;
            return cellVector;
        }
        for(int iCell=0;iCell<nCellsInEta;++iCell) oneEtaLayer.push_back(NULL);
        // have two layers in phi
        cellVector.push_back(oneEtaLayer);
        cellVector.push_back(oneEtaLayer);
        // get cluster from shot
        const xAOD::CaloCluster* cluster = shot->cluster(0);
        const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
        CaloClusterCellLink::const_iterator cellItr  = theCellLink->begin();
        CaloClusterCellLink::const_iterator cellItrE = theCellLink->end();

        // get seed cell from shot cluster
        const CaloCell* seedCell=NULL;
        for(;cellItr!=cellItrE;++cellItr){
            if((*cellItr)->caloDDE()->calo_hash()!=(unsigned) seedHash) continue;
            seedCell = *cellItr;
            break;
        }
        if(seedCell==NULL){
          std::cout << "WARNING: Couldn't find seed cell in shot cluster. Return empty cell block." << std::endl;
          return cellVector;
        }
        
        // get merged cell in phi. Keep NULL if shot is not merged across phi
        const CaloCell* mergedCell = NULL;
        std::vector<IdentifierHash> nextInPhi;
        std::vector<IdentifierHash> prevInPhi;
        calo_id->get_neighbours(seedCell->caloDDE()->calo_hash(),LArNeighbours::nextInPhi,nextInPhi);
        calo_id->get_neighbours(seedCell->caloDDE()->calo_hash(),LArNeighbours::prevInPhi,prevInPhi);
        for(cellItr=theCellLink->begin();cellItr!=cellItrE;++cellItr){
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
        cellVector.at(0).at(nCellsInEta/2) = seedCell; // store seed cell
        std::vector<IdentifierHash> next;
        while(lastCell!=NULL && nCellsFromSeed<nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::nextInEta,next);
            lastCell = NULL;
            for(cellItr=theCellLink->begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(0).at(nCellsInEta/2+nCellsFromSeed) = (*cellItr);
                    lastCell = (*cellItr);
                }
            }
            nCellsFromSeed++;
        }
        nCellsFromSeed = 1;
        lastCell = seedCell;
        while(lastCell!=NULL && nCellsFromSeed<nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::prevInEta,next);
            lastCell = NULL;
            for(cellItr=theCellLink->begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(0).at(nCellsInEta/2-nCellsFromSeed) = (*cellItr);
                    lastCell = (*cellItr);
                }
            }
            nCellsFromSeed++;
        }
        // store cells in the eta layer, which contains the merged cell
        int nCellsFromMerged = 1;
        lastCell = mergedCell; // is NULL if shot is not merged
        cellVector.at(1).at(nCellsInEta/2) = mergedCell; // store merged cell
        while(lastCell!=NULL && nCellsFromMerged<nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::nextInEta,next);
            lastCell = NULL;
            for(cellItr=theCellLink->begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(1).at(nCellsInEta/2+nCellsFromMerged) = (*cellItr);
                    lastCell = (*cellItr);
                }
            }
            nCellsFromMerged++;
        }
        nCellsFromMerged = 1;
        lastCell = mergedCell;
        while(lastCell!=NULL && nCellsFromMerged<nCellsInEta/2+1){
            calo_id->get_neighbours(lastCell->caloDDE()->calo_hash(),LArNeighbours::prevInEta,next);
            lastCell = NULL;
            for(cellItr=theCellLink->begin();cellItr!=cellItrE;++cellItr){
                std::vector<IdentifierHash>::iterator itr = next.begin();
                for( ; itr!=next.end(); ++itr ){
                    if((*cellItr)->caloDDE()->calo_hash() != (*itr)) continue;
                    cellVector.at(1).at(nCellsInEta/2-nCellsFromMerged) = (*cellItr);
                    lastCell = (*cellItr);
                }
            }
            nCellsFromMerged++;
        }
        return cellVector;
    
    }


    float mean_eta(vector<vector<const CaloCell*> > shotCells, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        float sumEta=0.;
        float sumWeight=0.;
        vector<vector<const CaloCell*> >::iterator itrPhi = shotCells.begin();
        for( ; itrPhi!=shotCells.end(); ++itrPhi ){
            vector<const CaloCell*>::iterator itrEta = itrPhi->begin();
            for( ; itrEta!=itrPhi->end(); ++itrEta ){
                if((*itrEta) == NULL) continue;
                sumWeight += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta);
                sumEta    += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta) * (*itrEta)->eta();
            }
        }
        if(sumWeight<=0.) return -99999.;
        return sumEta/sumWeight;
    }

    float mean_pt(vector<vector<const CaloCell*> > shotCells, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        float sumPt=0.;
        int nCells = 0;
        vector<vector<const CaloCell*> >::iterator itrPhi = shotCells.begin();
        for( ; itrPhi!=shotCells.end(); ++itrPhi ){
            vector<const CaloCell*>::iterator itrEta = itrPhi->begin();
            for( ; itrEta!=itrPhi->end(); ++itrEta ){
                if((*itrEta) == NULL) continue;
                sumPt  += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta);
                nCells ++;
            }
        }
        if(nCells==0) return -99999.;
        return sumPt/nCells;
    }

    float ptWindow(vector<vector<const CaloCell*> > shotCells, int windowSize, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        // window size should be odd and noti be larger than eta window of shotCells
        int nCells_eta = shotCells.at(0).size();
        int seedIndex = nCells_eta/2;
        if( windowSize%2!=1 )        return 0.;
        if( windowSize > nCells_eta) return 0.;
        float ptWindow  = 0.;
        for(int iCell = 0; iCell != nCells_eta; ++iCell ){
	  if(std::abs(iCell-seedIndex)>windowSize/2) continue;
            if(shotCells.at(0).at(iCell) != NULL) ptWindow+=shotCells.at(0).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(iCell));
            if(shotCells.at(1).at(iCell) != NULL) ptWindow+=shotCells.at(1).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(iCell));
        }
        return ptWindow;
    }

    float ws5(vector<vector<const CaloCell*> > shotCells, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        int nCells_eta = shotCells.at(0).size();
        int seedIndex = nCells_eta/2;
        float sumWeight=0.;
        float sumDev2=0.;
        vector<vector<const CaloCell*> >::iterator itrPhi = shotCells.begin();
        for( ; itrPhi!=shotCells.end(); ++itrPhi ){
            for(unsigned iCell = 0; iCell != itrPhi->size(); ++iCell ){
                if(itrPhi->at(iCell) == NULL) continue;
                sumWeight += itrPhi->at(iCell)->pt()*caloWeightTool->wtCell(itrPhi->at(iCell));
                sumDev2   += itrPhi->at(iCell)->pt()*caloWeightTool->wtCell(itrPhi->at(iCell)) * pow(iCell-seedIndex,2);
            }
        }
        if(sumWeight<=0. || sumDev2 <0.) return -99999.;
        return sqrt( sumDev2 / sumWeight );
    }

    float sdevEta_WRTmean(vector<vector<const CaloCell*> > shotCells, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        float mean = mean_eta(shotCells, caloWeightTool); 
        float sumWeight=0.;
        float sumDev2=0.;
        vector<vector<const CaloCell*> >::iterator itrPhi = shotCells.begin();
        for( ; itrPhi!=shotCells.end(); ++itrPhi ){
            vector<const CaloCell*>::iterator itrEta = itrPhi->begin();
            for( ; itrEta!=itrPhi->end(); ++itrEta ){
                if((*itrEta) == NULL) continue;
                sumWeight += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta);
                sumDev2   += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta) * pow((*itrEta)->eta() - mean,2);
            }
        }
        if(sumWeight<=0. || sumDev2 <0.) return -99999.;
        return sqrt( sumDev2 / sumWeight );
    }

    float sdevEta_WRTmode(vector<vector<const CaloCell*> > shotCells, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        int nCells_eta = shotCells.at(0).size();
        int seedIndex = nCells_eta/2;
        float mode = shotCells.at(0).at(seedIndex)->eta();
        float sumWeight=0.;
        float sumDev2=0.;
        vector<vector<const CaloCell*> >::iterator itrPhi = shotCells.begin();
        for( ; itrPhi!=shotCells.end(); ++itrPhi ){
            vector<const CaloCell*>::iterator itrEta = itrPhi->begin();
            for( ; itrEta!=itrPhi->end(); ++itrEta ){
                if((*itrEta) == NULL) continue;
                sumWeight += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta);
                sumDev2   += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta) * pow((*itrEta)->eta() - mode,2);
            }
        }
        if(sumWeight<=0. || sumDev2 <0.) return -99999.;
        return sqrt( sumDev2 / sumWeight );
    }

    float sdevPt(vector<vector<const CaloCell*> > shotCells, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        float mean = mean_pt(shotCells, caloWeightTool);
        float sumWeight=0.;
        float sumDev2=0.;
        vector<vector<const CaloCell*> >::iterator itrPhi = shotCells.begin();
        for( ; itrPhi!=shotCells.end(); ++itrPhi ){
            vector<const CaloCell*>::iterator itrEta = itrPhi->begin();
            for( ; itrEta!=itrPhi->end(); ++itrEta ){
                if((*itrEta) == NULL) continue;
                sumWeight += (*itrEta)->pt()*caloWeightTool->wtCell(*itrEta);
                sumDev2   += pow((*itrEta)->pt()*caloWeightTool->wtCell(*itrEta) - mean,2);
            }
        }
        if(sumWeight<=0. || sumDev2 <0.) return -99999.;
        return sqrt(sumDev2)/sumWeight;
    }

    float deltaPt12_min(vector<vector<const CaloCell*> > shotCells, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        int nCells_eta = shotCells.at(0).size();
        int seedIndex = nCells_eta/2;
        bool haveLeft  = false;
        bool haveRight = false;
        float deltaPt_left  = 0.;
        float deltaPt_right = 0.;
        if(shotCells.at(0).at(seedIndex-1)!=NULL && shotCells.at(0).at(seedIndex-2)!=NULL){
            haveLeft  = true;
            deltaPt_left =  shotCells.at(0).at(seedIndex-1)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(seedIndex-1))
                           -shotCells.at(0).at(seedIndex-2)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(seedIndex-2));
            if(shotCells.at(1).at(seedIndex-1)!=NULL && shotCells.at(1).at(seedIndex-2)!=NULL){
                deltaPt_left += shotCells.at(1).at(seedIndex-1)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(seedIndex-1))
                               -shotCells.at(1).at(seedIndex-2)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(seedIndex-2));
            }
        }
        if(shotCells.at(0).at(seedIndex+1)!=NULL && shotCells.at(0).at(seedIndex+2)!=NULL){
            haveRight = true;
            deltaPt_right =  shotCells.at(0).at(seedIndex+1)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(seedIndex+1))
                            -shotCells.at(0).at(seedIndex+2)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(seedIndex+2));
            if(shotCells.at(1).at(seedIndex+1)!=NULL && shotCells.at(1).at(seedIndex+2)!=NULL){
                deltaPt_right += shotCells.at(1).at(seedIndex+1)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(seedIndex+1))
                                -shotCells.at(1).at(seedIndex+2)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(seedIndex+1));
            }
        }
        if(haveLeft && haveRight) return fmin(deltaPt_left,deltaPt_right);
        if(haveLeft)              return deltaPt_left;
        if(haveRight)             return deltaPt_right;
        else                      return -1.;
    }


    float Fside(vector<vector<const CaloCell*> > shotCells, int largerWindow, int smallerWindow, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        // window sizes should be odd and windows should be not larger than eta window of shotCells
        int nCells_eta = shotCells.at(0).size();
        int seedIndex = nCells_eta/2;
        if( largerWindow%2!=1 || smallerWindow%2!=1) return 0.;
        if( largerWindow <= smallerWindow)           return 0.;
        if( largerWindow > nCells_eta)   return 0.;
        float pt_largerWindow  = 0.;
        float pt_smallerWindow = 0.;
        for(int iCell = 0; iCell != nCells_eta; ++iCell ){
	    if(std::abs(iCell-seedIndex)>largerWindow/2) continue;
            if(shotCells.at(0).at(iCell)!=NULL) pt_largerWindow+=shotCells.at(0).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(iCell));
            if(shotCells.at(1).at(iCell)!=NULL) pt_largerWindow+=shotCells.at(1).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(iCell));
            if(std::abs(iCell-seedIndex)>smallerWindow/2) continue;
            if(shotCells.at(0).at(iCell)!=NULL) pt_smallerWindow+=shotCells.at(0).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(iCell));
            if(shotCells.at(1).at(iCell)!=NULL) pt_smallerWindow+=shotCells.at(1).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(iCell));
        }
        if(pt_smallerWindow==0.) return -99999.;
        return (pt_largerWindow-pt_smallerWindow)/pt_smallerWindow;
    }

    float fracSide(vector<vector<const CaloCell*> > shotCells, int largerWindow, int smallerWindow, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        // window sizes should be odd and windows should be not larger than eta window of shotCells
        int nCells_eta = shotCells.at(0).size();
        int seedIndex = nCells_eta/2;
        if( largerWindow%2!=1 || smallerWindow%2!=1) return 0.;
        if( largerWindow <= smallerWindow)           return 0.;
        if( largerWindow > nCells_eta)   return 0.;
        float pt_largerWindow  = 0.;
        float pt_smallerWindow = 0.;
        for(int iCell = 0; iCell != nCells_eta; ++iCell ){
            if(abs(iCell-seedIndex)>largerWindow/2) continue;
            if(shotCells.at(0).at(iCell)!=NULL) pt_largerWindow+=shotCells.at(0).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(iCell));
            if(shotCells.at(1).at(iCell)!=NULL) pt_largerWindow+=shotCells.at(1).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(iCell));
            if(abs(iCell-seedIndex)>smallerWindow/2) continue;
            if(shotCells.at(0).at(iCell)!=NULL) pt_smallerWindow+=shotCells.at(0).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(iCell));
            if(shotCells.at(1).at(iCell)!=NULL) pt_smallerWindow+=shotCells.at(1).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(iCell));
        }
        if(pt_largerWindow==0.) return -99999.;
        return (pt_largerWindow-pt_smallerWindow)/pt_largerWindow;
    }

    float ptWindowFrac(vector<vector<const CaloCell*> > shotCells, int largerWindow, int smallerWindow, ToolHandle<IHadronicCalibrationTool>& caloWeightTool){
        // window sizes should be odd and windows should be not larger than eta window of shotCells
        int nCells_eta = shotCells.at(0).size();
        int seedIndex = nCells_eta/2;
        if( largerWindow%2!=1 || smallerWindow%2!=1) return 0.;
        if( largerWindow <= smallerWindow)           return 0.;
        if( largerWindow > nCells_eta)   return 0.;
        float pt_largerWindow  = 0.;
        float pt_smallerWindow = 0.;
        for(int iCell = 0; iCell != nCells_eta; ++iCell ){
            if(abs(iCell-seedIndex)>largerWindow/2) continue;
            if(shotCells.at(0).at(iCell)!=NULL) pt_largerWindow+=shotCells.at(0).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(iCell));
            if(shotCells.at(1).at(iCell)!=NULL) pt_largerWindow+=shotCells.at(1).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(iCell));
            if(abs(iCell-seedIndex)>smallerWindow/2) continue;
            if(shotCells.at(0).at(iCell)!=NULL) pt_smallerWindow+=shotCells.at(0).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(0).at(iCell));
            if(shotCells.at(1).at(iCell)!=NULL) pt_smallerWindow+=shotCells.at(1).at(iCell)->pt()*caloWeightTool->wtCell(shotCells.at(1).at(iCell));
        }
        if(pt_largerWindow==0.) return -99999.;
        return pt_smallerWindow/pt_largerWindow;
    }
}

#endif
