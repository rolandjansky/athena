/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//#include "GaudiKernel/IToolSvc.h"

//#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
//#include "CaloIdentifier/CaloCell_ID.h"
//#include "CaloIdentifier/LArNeighbours.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
//#include "CaloUtils/CaloCellESort.h"
//#include "CaloInterface/IHadronicCalibrationTool.h"
//#include "FourMomUtils/P4Helpers.h"
#include "TauShotFinder.h"
#include "TauShotVariableHelpers.h"
//#include "TMVA/Reader.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/PFO.h"

using std::vector;
using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauShotFinder::TauShotFinder(   const string& name ) :
    TauRecToolBase(name)
    , m_caloWeightTool("H1WeightToolCSC12Generic")
    , m_caloCellContainerName("AllCalo")
    , m_shotClusterContainer(NULL)
    , m_shotClusterContainerName("TauShotClusters")
    , m_PFOShotContainer(0)
    , m_shotPFOContainerName("TauShotParticleFlowObjects")
    , m_PFOShotAuxStore(0)
    , m_calo_dd_man(NULL)
    , m_calo_id(NULL)
    , m_pt1(0)
    , m_pt3(0)
    , m_pt5(0)
{
    declareProperty("CaloWeightTool", m_caloWeightTool);
    declareProperty("CaloCellContainerName", m_caloCellContainerName); 
    declareProperty("ShotClusterContainerName", m_shotClusterContainerName);
    declareProperty("ShotPFOContainerName",  m_shotPFOContainerName);
//    declareProperty("ReaderOption",          m_readerOption);
//    declareProperty("BDTWeightFile_barrel",  m_weightfile_barrel);
//    declareProperty("BDTWeightFile_endcap1", m_weightfile_endcap1);
//    declareProperty("BDTWeightFile_endcap2", m_weightfile_endcap2);
    declareProperty("NCellsInEta",           m_nCellsInEta);
    declareProperty("MinPtCut",              m_minPtCut);
    declareProperty("AutoDoubleShotCut",     m_autoDoubleShotCut);
    declareProperty("MergedBDTScoreCut",     m_mergedBDTScoreCut);
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

    // initialize calo cell geo
    m_calo_dd_man  = CaloDetDescrManager::instance();
    m_calo_id      = m_calo_dd_man->getCaloCell_ID();

    /*
    //---------------------------------------------------------------------
    // Create TMVA readers
    //---------------------------------------------------------------------
    m_tmvaReader_barrel = new TMVA::Reader(TString(m_readerOption));
    if (msgLvl(MSG::DEBUG)) m_tmvaReader_barrel->SetVerbose(true);
    m_tmvaReader_barrel->AddVariable("(shot_pt_3-shot_pt)/shot_pt_3",&G_PTFRAC);
    m_tmvaReader_barrel->AddVariable("shot_stdpt_5"                 ,&G_STDPT_5 );
    m_tmvaReader_barrel->AddVariable("shot_stdeta_5"                ,&G_STDETA_5 );
    m_tmvaReader_barrel->AddVariable("shot_deltapt_min"             ,&G_DELTAPT_MIN );

    m_tmvaReader_endcap1 = new TMVA::Reader(TString(m_readerOption));
    if (msgLvl(MSG::DEBUG)) m_tmvaReader_endcap1->SetVerbose(true);
    m_tmvaReader_endcap1->AddVariable("(shot_pt_3-shot_pt)/shot_pt_3",&G_PTFRAC);
    m_tmvaReader_endcap1->AddVariable("shot_stdpt_5"                 ,&G_STDPT_5 );
    m_tmvaReader_endcap1->AddVariable("shot_stdeta_5"                ,&G_STDETA_5 );
    m_tmvaReader_endcap1->AddVariable("shot_deltapt_min"             ,&G_DELTAPT_MIN );

    m_tmvaReader_endcap2 = new TMVA::Reader(TString(m_readerOption));
    if (msgLvl(MSG::DEBUG)) m_tmvaReader_endcap2->SetVerbose(true);
    m_tmvaReader_endcap2->AddVariable("(shot_pt_3-shot_pt)/shot_pt_3",&G_PTFRAC);
    m_tmvaReader_endcap2->AddVariable("shot_stdpt_5"                 ,&G_STDPT_5 );
    m_tmvaReader_endcap2->AddVariable("shot_stdeta_5"                ,&G_STDETA_5 );
    m_tmvaReader_endcap2->AddVariable("shot_deltapt_min"             ,&G_DELTAPT_MIN );

    if (bookMethod(m_tmvaReader_barrel, m_tmvaReader_endcap1, m_tmvaReader_endcap2, "BDT method").isFailure()) return StatusCode::FAILURE;
    */

    // setupCuts();

    return StatusCode::SUCCESS;
}

StatusCode TauShotFinder::finalize()
{
  StatusCode sc = AlgTool::finalize();
  /*
  delete m_tmvaReader_barrel;
  delete m_tmvaReader_endcap1;
  delete m_tmvaReader_endcap2;
  */
  return sc;
}

StatusCode TauShotFinder::eventInitialize() {

    //---------------------------------------------------------------------
    // Create Shot ClusterContainer and register in StoreGate
    //---------------------------------------------------------------------
    m_shotClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(),   
								 m_shotClusterContainerName,    
								 msg()                  
								 );

    //---------------------------------------------------------------------
    // Create Shot PFO container
    //---------------------------------------------------------------------
    m_PFOShotContainer = new xAOD::PFOContainer();
    CHECK( evtStore()->record(m_PFOShotContainer, m_shotPFOContainerName ) );
    m_PFOShotAuxStore = new xAOD::PFOAuxContainer();
    CHECK( evtStore()->record( m_PFOShotAuxStore, m_shotPFOContainerName + "Aux." ) );
    m_PFOShotContainer->setStore(m_PFOShotAuxStore);

    return StatusCode::SUCCESS;
}

StatusCode TauShotFinder::execute(xAOD::TauJet& pTau) {

    // Any tau needs to have shot PFO vectors. Set empty vectors before nTrack cut
    vector<ElementLink<xAOD::PFOContainer> > empty;
    pTau.setShotPFOLinks(empty);
    
    //---------------------------------------------------------------------
    // only run shower subtraction on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
       return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("");
    ATH_MSG_DEBUG("New tau");

    //---------------------------------------------------------------------
    // retrieve cells around tau 
    //---------------------------------------------------------------------
    // get all calo cell container
    const CaloCellContainer *pCellContainer = NULL;
    CHECK( evtStore()->retrieve(pCellContainer, m_caloCellContainerName) );
    
    // get only EM cells within dR<0.2
    // TODO: might be possible to select only EM1 cells, but probbaly wont 
    //       speed things up much anyway
    vector<CaloCell_ID::SUBCALO> emSubCaloBlocks;
    emSubCaloBlocks.push_back(CaloCell_ID::LAREM);
    boost::scoped_ptr<CaloCellList> pCells(new CaloCellList(pCellContainer,emSubCaloBlocks)); 

    // TODO: change hardcoded 0.2 to tau cone variable, (or func. from TauJet?)
    pCells->select(pTau.eta(), pTau.phi(), 0.4); 

    // Dump cells into a std::vector since CaloCellList wont allow sorting
    // (maybe this can be done faster at some point).
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
        shotCluster->getCellLinks()->reserve(windowNeighbours.size()+1);
        shotCluster->addCell(pCellContainer->findIndex(seedHash), 1.);
        cellItr = windowNeighbours.begin();
        for( ; cellItr!=windowNeighbours.end(); ++cellItr)
            shotCluster->addCell(pCellContainer->findIndex((*cellItr)->caloDDE()->calo_hash()),1.0);
        CaloClusterKineHelper::calculateKine(shotCluster,true,true);
        m_shotClusterContainer->push_back(shotCluster);
        
        // create shot PFO and store it in output container
        xAOD::PFO* shot = new xAOD::PFO();
        m_PFOShotContainer->push_back( shot );

        // Create element link from tau to shot
        ElementLink<xAOD::PFOContainer> PFOElementLink;
        PFOElementLink.toContainedElement( *m_PFOShotContainer, shot );
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
        // No need to calculate cluster moments atm.
        //if( !shotCluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 404, center_mag) ) ATH_MSG_WARNING("Couldn't retrieve CENTER_MAG moment. Set it to 0.");
        shot->setCenterMag( (float) center_mag);
        
        ElementLink<xAOD::CaloClusterContainer> clusElementLink;
        clusElementLink.toContainedElement( *m_shotClusterContainer, shotCluster );
        shot->setClusterLink( clusElementLink );
        shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_nCellsInEta, m_nCellsInEta);
        shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHash);

        // Get cell block for variable calculations
        std::vector<std::vector<const CaloCell*> > cellBlock = TauShotVariableHelpers::getCellBlock(shot, m_calo_id);

        // Some DEBUG statements
        if (msgLvl(MSG::DEBUG)) { 
          if(cell->pt()*m_caloWeightTool->wtCell(cell)>300){
            ATH_MSG_DEBUG("");
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
        m_pt1=TauShotVariableHelpers::ptWindow(cellBlock,1,m_caloWeightTool);
        m_pt3=TauShotVariableHelpers::ptWindow(cellBlock,3,m_caloWeightTool);
        m_pt5=TauShotVariableHelpers::ptWindow(cellBlock,5,m_caloWeightTool);
        // The folliwing variables are not needed atm
        /*
        m_ws5=TauShotVariableHelpers::ws5(cellBlock,m_caloWeightTool);
        m_sdevEta5_WRTmean=TauShotVariableHelpers::sdevEta_WRTmean(cellBlock,m_caloWeightTool);
        m_sdevEta5_WRTmode=TauShotVariableHelpers::sdevEta_WRTmode(cellBlock,m_caloWeightTool);
        m_sdevPt5=TauShotVariableHelpers::sdevPt(cellBlock,m_caloWeightTool);
        m_deltaPt12_min=TauShotVariableHelpers::deltaPt12_min(cellBlock,m_caloWeightTool);
        m_Fside_3not1=TauShotVariableHelpers::Fside(cellBlock,3,1,m_caloWeightTool);
        m_Fside_5not1=TauShotVariableHelpers::Fside(cellBlock,5,1,m_caloWeightTool);
        m_Fside_5not3=TauShotVariableHelpers::Fside(cellBlock,5,3,m_caloWeightTool);
        m_fracSide_3not1=TauShotVariableHelpers::fracSide(cellBlock,3,1,m_caloWeightTool);
        m_fracSide_5not1=TauShotVariableHelpers::fracSide(cellBlock,5,1,m_caloWeightTool);
        m_fracSide_5not3=TauShotVariableHelpers::fracSide(cellBlock,5,3,m_caloWeightTool);
        m_pt1OverPt3=TauShotVariableHelpers::ptWindowFrac(cellBlock,3,1,m_caloWeightTool);
        m_pt3OverPt5=TauShotVariableHelpers::ptWindowFrac(cellBlock,5,3,m_caloWeightTool);
        

        // Same variable names as in Stephanie's private code
        G_PTFRAC=m_fracSide_3not1;
        G_STDPT_5=m_sdevPt5;
        G_STDETA_5=fmin(m_sdevEta5_WRTmean,0.0036);
        G_DELTAPT_MIN=fmax(-1000.,fmin(m_deltaPt12_min,2000));

        // Calculate BDT scores
        float mergedBDTScore=getMergedBDTScore(etaBin);
        */

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Calculate number of photons in shot
        ////////////////////////////////////////////////////////////////////////////////////////////
        //int nPhotons = getNPhotons(etaBin, mergedBDTScore, m_pt1);
        int nPhotons = getNPhotons(etaBin, 1, m_pt1);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Set variables in shot PFO
        ////////////////////////////////////////////////////////////////////////////////////////////
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt1, m_pt1);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt3, m_pt3);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt5, m_pt5);
        // The folliwing variables are not needed atm
        /*
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_ws5, m_ws5);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_sdevEta5_WRTmean, m_sdevEta5_WRTmean);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_sdevEta5_WRTmode, m_sdevEta5_WRTmode);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_sdevPt5, m_sdevPt5);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_deltaPt12_min, m_deltaPt12_min);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_Fside_3not1, m_Fside_3not1);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_Fside_5not1, m_Fside_5not1);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_Fside_5not3, m_Fside_5not3);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_fracSide_3not1, m_fracSide_3not1);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_fracSide_5not1, m_fracSide_5not1);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_fracSide_5not3, m_fracSide_5not3);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt1OverPt3, m_pt1OverPt3);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt3OverPt5, m_pt3OverPt5);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_mergedScore, mergedBDTScore);
        shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_signalScore, -1.);
        */
        shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_nPhotons, nPhotons);

        // remove shot(s) from list
        vector<const CaloCell*>::iterator cellItrNonConst;
        cellItrNonConst = std::find(seedCells.begin(),seedCells.end(),cell);
        seedCells.erase(cellItrNonConst);
        if( mergePhi ){
            cellItrNonConst = std::find(seedCells.begin(),seedCells.end(),mergePhi);
            seedCells.erase(cellItrNonConst);
        }

        /*
        ATH_MSG_DEBUG("Storing shot. pt: " << shot->pt()
                        << ", eta: " << shot->eta()
                        << ", phi: " << shot->phi()
                        );
        */
          
    } // seed cells
    
    
    return StatusCode::SUCCESS;
}

StatusCode TauShotFinder::eventFinalize() {
    CHECK( CaloClusterStoreHelper::finalizeClusters(&*evtStore(),
                            m_shotClusterContainer,
                            m_shotClusterContainerName,
                            msg()) );

    return StatusCode::SUCCESS;
}

std::vector<const CaloCell*> TauShotFinder::getNeighbours(const CaloCellContainer* pCellContainer, 
                                           const CaloCell* cell, 
                                           int maxDepth)
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
                                  bool next)
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

bool TauShotFinder::isPhiNeighbour(IdentifierHash cell1Hash, IdentifierHash cell2Hash, bool next){
    std::vector<IdentifierHash> neigHashes;
    if( next ) m_calo_id->get_neighbours(cell1Hash,LArNeighbours::nextInPhi,neigHashes);
    else       m_calo_id->get_neighbours(cell1Hash,LArNeighbours::prevInPhi,neigHashes);
    std::vector<IdentifierHash>::iterator itr = neigHashes.begin();
    for( ; itr!=neigHashes.end(); ++itr ){
        if(cell2Hash == (*itr)) return true;
    } 
    return false;
}

float TauShotFinder::getEtaBin(float seedEta){
    float absSeedEta=fabs(seedEta);
    if(fabs(absSeedEta)<0.80)      return 0; // Central Barrel
    else if(fabs(absSeedEta)<1.39) return 1; // Outer Barrel
    else if(fabs(absSeedEta)<1.51) return 2; // crack
    else if(fabs(absSeedEta)<1.80) return 3; // endcap, fine granularity
    else return 4;                           // endcap, coarse granularity
}

/*
float TauShotFinder::getMergedBDTScore(int etaBin){
    float BDTScore = -1;
    if(etaBin==0)      BDTScore = m_tmvaReader_barrel->EvaluateMVA( "BDT method" );  // barrel1
    else if(etaBin==1) BDTScore = m_tmvaReader_barrel->EvaluateMVA( "BDT method" );  // barrel2
    else if(etaBin==2) BDTScore = m_tmvaReader_barrel->EvaluateMVA( "BDT method" );  // just use barrel BDT for now to check how it looks in data
    else if(etaBin==3) BDTScore = m_tmvaReader_endcap1->EvaluateMVA( "BDT method" ); // endcap1
    else if(etaBin==4) BDTScore = m_tmvaReader_endcap2->EvaluateMVA( "BDT method" ); // endcap2
    return BDTScore;
}
*/

float TauShotFinder::getNPhotons(int etaBin, float mergedBDTScore, float seedEnergy){
    if(etaBin==2) return 0; // no photon counting in crack atm
    ATH_MSG_DEBUG("etaBin = " << etaBin  << ", seedEnergy = " << seedEnergy << ", m_minPtCut.at(etaBin) = " << m_minPtCut.at(etaBin) << ", m_autoDoubleShotCut.at(etaBin) = " 
       << m_autoDoubleShotCut.at(etaBin) << ", mergedBDTScore = " << mergedBDTScore << ", m_mergedBDTScoreCut.at(etaBin) = " << m_mergedBDTScoreCut.at(etaBin) );
    if( seedEnergy < m_minPtCut.at(etaBin) ) return 0;
    if( seedEnergy > m_autoDoubleShotCut.at(etaBin) ) return 2;
    if( mergedBDTScore < m_mergedBDTScoreCut.at(etaBin) ) return 2;
    return 1;
}

// some really slick c++ way of doing sort (since we need to use the member m_caloWeightTool)
// how about learing a thing or two from python...
TauShotFinder::ptSort::ptSort( const TauShotFinder& info ) : m_info(info) { } 
bool TauShotFinder::ptSort::operator()( const CaloCell* c1, const CaloCell* c2 ){
     return  c1->pt()*m_info.m_caloWeightTool->wtCell(c1) > c2->pt()*m_info.m_caloWeightTool->wtCell(c2);  
}

/*
StatusCode TauShotFinder::bookMethod(TMVA::Reader *reader_barrel, 
                                     TMVA::Reader *reader_endcap1, 
                                     TMVA::Reader *reader_endcap2, 
                                     const std::string &methodName) const 
{
    if (m_weightfile_barrel == ""){
        ATH_MSG_ERROR("No weight m_weightfile_barrel given");
        return StatusCode::FAILURE;
    }
    if (m_weightfile_endcap1 == ""){
        ATH_MSG_ERROR("No weight m_weightfile_endcap1 given");
        return StatusCode::FAILURE;
    }
    if (m_weightfile_endcap2 == ""){
        ATH_MSG_ERROR("No weight m_weightfile_endcap2 given");
        return StatusCode::FAILURE;
    }
    std::string resolvedFileName = find_file(m_weightfile_barrel);
    if (resolvedFileName != ""){
        ATH_MSG_DEBUG( "Parameterisation file found: " << resolvedFileName );
    }
    else {
        ATH_MSG_ERROR( "Parameterisation file " << m_weightfile_barrel << " not found" );
        return StatusCode::FAILURE;
    }
    reader_barrel->BookMVA( methodName, resolvedFileName);

    resolvedFileName = find_file(m_weightfile_endcap1);
    if (resolvedFileName != ""){
        ATH_MSG_DEBUG( "Parameterisation file found: " << resolvedFileName );
    }
    else {
        ATH_MSG_ERROR( "Parameterisation file " << m_weightfile_endcap1 << " not found" );
        return StatusCode::FAILURE;
    }
    reader_endcap1->BookMVA( methodName, resolvedFileName);

    resolvedFileName = find_file(m_weightfile_endcap2);
    if (resolvedFileName != ""){
        ATH_MSG_DEBUG( "Parameterisation file found: " << resolvedFileName );
    }
    else {
        ATH_MSG_ERROR( "Parameterisation file " << m_weightfile_endcap2 << " not found" );
        return StatusCode::FAILURE;
    }
    reader_endcap2->BookMVA( methodName, resolvedFileName);
    return StatusCode::SUCCESS;
}
*/
// EOF
#endif
