/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0BonnCreateROI.cxx
// package:     Reconstruction/tauEvent
// authors:     Will Davey
// date:        2012-10-09
//
//-----------------------------------------------------------------------------

#include <TString.h>
#include <TH1.h>

#include "GaudiKernel/IToolSvc.h"

#include "PathResolver/PathResolver.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloCellList.h"
#include "AthContainers/OwnershipPolicy.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "xAODCaloEvent/CaloCluster.h"

#include "tauRec/TauPi0BonnCreateROI.h"
#include "tauRec/TauPi0BonnParser.h"

#include "tauEvent/TauPi0Details.h"
#include "tauEvent/TauShot.h"

#include "CaloInterface/IHadronicCalibrationTool.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloIdentifier/CaloCell_ID.h"

using std::max;
using std::min;
using std::vector;
using std::string;
//using xAOD::PFO; // required for subtration in EM1


//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0BonnCreateROI::TauPi0BonnCreateROI(   const string& type,
                                            const string& name,
                                            const IInterface *parent) 
    : TauToolBase(type, name, parent)
    , m_caloWeightTool("H1WeightToolCSC12Generic")
    , m_trackToCaloTool("")
    , m_cellMakerTool("")
    , m_calo_dd_man(NULL)
    , m_calo_id(NULL)
    , m_latParser(new TauPi0BonnParser())
    , m_caloCellContainerName("AllCalo")
    , m_clusterContainerName("CaloCalTopoCluster")
    , m_pPi0CellContainer(NULL)
    , m_pi0CellContainerName("TauCommonPi0CellContainer")
    , m_chargedPFOContainerName("TauPi0BonnChargedPFOContainer")
{
    declareInterface<TauToolBase > (this);

    declareProperty("CaloWeightTool", m_caloWeightTool);
    declareProperty("ExtrapolateToCaloTool", m_trackToCaloTool);
    declareProperty("CellMakerTool", m_cellMakerTool);
    
    declareProperty("CaloCellContainerName",   m_caloCellContainerName); // TODO: May be replaced using tau clusters
    declareProperty("ClusterContainerName",    m_clusterContainerName);   // TODO: May be replaced using tau clusters
    declareProperty("Pi0CellContainerName",    m_pi0CellContainerName);
    declareProperty("ChargedPFOContainerName", m_chargedPFOContainerName);

    declareProperty("LatParFile", m_latParFile );
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0BonnCreateROI::~TauPi0BonnCreateROI() {
}

StatusCode TauPi0BonnCreateROI::initialize() {
    
    // retrieve tools
    ATH_MSG_DEBUG( "Retrieving tools" );
    CHECK( m_caloWeightTool.retrieve() );
    CHECK( m_trackToCaloTool.retrieve() );
    CHECK( m_cellMakerTool.retrieve() );

    // initialize calo cell geo
    m_calo_dd_man  = CaloDetDescrManager::instance();
    m_calo_id      = m_calo_dd_man->getCaloCell_ID();


    ////////////////////////////////////////////////////////////
    // Load lateral parameterisation 
    ////////////////////////////////////////////////////////////
    // check file given for lateral shower parameteristaion 
    if( m_latParFile == "" ){
        ATH_MSG_ERROR( "No lateral shower parameterisation given" );
        return StatusCode::FAILURE;
    }
    // parse lateral config file 
    m_latParser->parseROOTFile( PathResolver::find_file(m_latParFile, "DATAPATH") );
    
    // set addresses of all varaibles that can be used in the para.
    m_latParser->setVar( "PT",     m_pt ); 
    m_latParser->setVar( "ABSETA", m_abseta); 
    m_latParser->setVar( "HADF",   m_hadf); 
    m_latParser->setVar( "SAMP",   m_sampling); 

    // hadf bot used atm. TODO: REMOVE?
    m_hadf = 0.;

    // check that parser loaded correctly, and that the required 
    // variables are configured
    if( !m_latParser->checkInitialisationStatus() ){
        ATH_MSG_ERROR( "Failure loading lateral shower parameterisation" );
        ATH_MSG_ERROR( "Parser stream: " << m_latParser->getStream() );
        return StatusCode::FAILURE;
    }

    // Create vector with default values
    for (int layer = 0 ; layer != CaloCell_ID::FCAL0; ++layer) {
        m_defaultValues.push_back(-10.);
        m_defaultValuesZero.push_back(0.);
    } 

    return StatusCode::SUCCESS;
}

StatusCode TauPi0BonnCreateROI::eventInitialize(TauCandidateData * /*data*/) {

    
    const CaloCell_ID* cellID;
    if((detStore()->retrieve(cellID)).isFailure()){
        ATH_MSG_ERROR("Unable to retrieve caloCell_ID helper from DetectorStore");
        return StatusCode::FAILURE;
    }

    // Get hash range
    IdentifierHash hashMax;
    hashMax = cellID->calo_cell_hash_max();
    ATH_MSG_VERBOSE("CaloCell Hash Max: " << hashMax);

    // Reset addedCellsMap
    m_addedCellsMap.clear();
    for (unsigned i = 0; i < hashMax; i++) {
        m_addedCellsMap.push_back(NULL);
    }

    // Reset hist maps
    m_trackHistMapBarrel.clear();
    m_trackHistMapEndcap.clear();

    //---------------------------------------------------------------------
    // Create CustomCellContainer and register in StoreGate
    //---------------------------------------------------------------------
    m_pPi0CellContainer = new CaloCellContainer(SG::OWN_ELEMENTS);
    CHECK( evtStore()->record(m_pPi0CellContainer, m_pi0CellContainerName) );

    // symlink as INavigable4MomentumCollection (as in CaloRec/CaloCellMaker)
    CHECK(evtStore()->symLink(m_pPi0CellContainer, static_cast<INavigable4MomentumCollection*> (0)));

    //---------------------------------------------------------------------
    // Create charged PFO container
    //---------------------------------------------------------------------
    m_chargedPFOContainer = new xAOD::PFOContainer();
    CHECK( evtStore()->record(m_chargedPFOContainer, m_chargedPFOContainerName ) );
    m_chargedPFOAuxStore = new xAOD::PFOAuxContainer();
    CHECK( evtStore()->record( m_chargedPFOAuxStore, m_chargedPFOContainerName + "Aux." ) );
    m_chargedPFOContainer->setStore(m_chargedPFOAuxStore);

    return StatusCode::SUCCESS;
}

StatusCode TauPi0BonnCreateROI::execute(TauCandidateData *data) {

    xAOD::TauJet *pTau = data->xAODTau;

    // Any tau needs to have PFO vectors. Set empty vectors before nTrack cut
    vector<ElementLink<xAOD::PFOContainer> > empty;
    pTau->setCellBased_Charged_PFOLinks(empty);
    pTau->setCellBased_Neutral_PFOLinks(empty);
    pTau->setCellBased_Pi0_PFOLinks(empty);

    // Any tau needs to have PanTauCellBasedProto 4mom. Set it to 0 before nTrack cut
    pTau->setP4(xAOD::TauJetParameters::PanTauCellBasedProto, 0.0, 0.0, 0.0, 0.0);

    //---------------------------------------------------------------------
    // only run shower subtraction on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau->nTracks() == 0 || pTau->nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("new tau. \tpt = " << pTau->pt() << "\teta = " << pTau->eta() << "\tphi = " << pTau->phi() << "\tnprongs = " << pTau->nTracks());

    //---------------------------------------------------------------------
    // get tau tracks
    //---------------------------------------------------------------------
    vector<const xAOD::TrackParticle*> tracks;
    for(unsigned iTrack = 0; iTrack<pTau->nTracks();++iTrack){
        const xAOD::TrackParticle* track = pTau->track(iTrack);
        tracks.push_back(track);
    }

    //---------------------------------------------------------------------
    // retrieve cells around tau 
    //---------------------------------------------------------------------
    
    // get all calo cell container
    const CaloCellContainer *pCellContainer = NULL;
    CHECK( evtStore()->retrieve(pCellContainer, m_caloCellContainerName) );
    
    // get only EM cells within dR<0.2
    vector<CaloCell_ID::SUBCALO> emSubCaloBlocks;
    emSubCaloBlocks.push_back(CaloCell_ID::LAREM);
    boost::scoped_ptr<CaloCellList> pCells(new CaloCellList(pCellContainer,emSubCaloBlocks)); 

    // TODO: change hardcoded 0.2 to tau cone variable, (or func. from TauJet?)
    pCells->select(pTau->eta(), pTau->phi(), 0.4); 

    //---------------------------------------------------------------------
    // prepare extrapolation of tracks to calo layers 
    //---------------------------------------------------------------------
    // clear vectors related to track extrapolation
    // have to do that for each tau so it cannot be done in eventInitialize,
    // which is called once per event (and not once per tau)
    m_tracksEtaAtSampling.clear();
    m_tracksPhiAtSampling.clear();
    m_extrapolatedSamplings.clear();
    // Fill with default values
    for(int layer = 0 ; layer != CaloCell_ID::FCAL0; ++layer) {
         m_extrapolatedSamplings.push_back(false);
    }
    //vector<float> E_sub_track;
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        m_tracksEtaAtSampling.push_back( m_defaultValues );
        m_tracksPhiAtSampling.push_back( m_defaultValues );
        //E_sub_track.push_back( 0. );
    }

    //---------------------------------------------------------------------
    // get energy in HCal associated to the different tracks
    //---------------------------------------------------------------------
    const xAOD::CaloClusterContainer* clusterContainer = NULL;
    CHECK( evtStore()->retrieve( clusterContainer, m_clusterContainerName ) );

    vector<double> EestInEcal = this->getEstEcalEnergy(tracks,clusterContainer,pTau);

    //Create charged PFOs
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        const xAOD::TrackParticle* track = tracks.at(iTrack);
        xAOD::PFO* chargedPFO = new xAOD::PFO();
        m_chargedPFOContainer->push_back(chargedPFO);
        ElementLink<xAOD::TrackParticleContainer> myTrackLink = pTau->trackLinks().at(iTrack);
        if(!chargedPFO->setTrackLink(myTrackLink)) ATH_MSG_WARNING("Could not add Track to PFO");
        chargedPFO->setCharge(track->charge());
        chargedPFO->setP4(track->pt(),track->eta(),track->phi(),track->m());
        // Create element link from tau to charged PFO
        ElementLink<xAOD::PFOContainer> PFOElementLink;
        PFOElementLink.toContainedElement( *m_chargedPFOContainer, chargedPFO );
        pTau->addCellBased_Charged_PFOLink( PFOElementLink );
        // Store energy to subtract/subtracted for the track in CenterMag variable. This variables is otherwise never used for charged PFOs
        chargedPFO->setCenterMag( (float) EestInEcal.at(iTrack));
    }

    // Optional: Subtraction in EM1 (part 1 of 3)
    // FIXME: xAOD migration of subtration in EM1
    /*
    //---------------------------------------------------------------------
    // get cells in EM1 associated to the different tracks
    //---------------------------------------------------------------------
    std::vector<IdentifierHash> pipmCellHash;
    const std::vector<TauShot*> shotVector = pPi0Details->shotVector();

    for(unsigned iShot = 0;iShot<shotVector.size();++iShot){
        TauShot* currentShot = shotVector.at(iShot);
        const CaloCell* seedCell = currentShot->seedCell();

        int samp = seedCell->caloDDE()->getSampling();
        double seedEta = seedCell->eta();

        // Don't match shots in crack region. Cells are so large that pi0s are likely to be in the same cell.
        if( (samp == 1 && fabs(seedEta)>1.4) || (samp == 5 && fabs(seedEta) < 1.5) ) continue;

        // Subtract 3 cells
        double shotPt = currentShot->pt3();

        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
            // Do not match shots to tracks, which have E_est < shotPt
            if(EestInEcal.at(iTrack)<shotPt) continue;

            // Do not match shots to tracks that have shotPt > 20% of the track pt. 
            // The shot is unlikely to come from the charged pion in this case
            double maxFrac = 0.20;
            if(tracks.at(iTrack)->pt() * maxFrac < shotPt) continue;

            // check if tracks have been extrapolated to this sampling. Do so if this is not the case
            if(m_extrapolatedSamplings.at(samp)==false){
                this->getExtrapolatedPositions(tracks,samp);
                ATH_MSG_DEBUG("Extr to layer " << samp << "\teta = " << m_tracksEtaAtSampling.at(iTrack).at(samp) << "\tphi = " << m_tracksPhiAtSampling.at(iTrack).at(samp));
                m_extrapolatedSamplings.at(samp)=true;
            }
            // Check if shot is close to the extrapolated track
            double AbsDEta = fabs( seedEta - m_tracksEtaAtSampling.at(iTrack).at(samp));
            double AbsDPhi = fabs( seedCell->phi() - m_tracksPhiAtSampling.at(iTrack).at(samp)); 
            double cellWidthEta = seedCell->caloDDE()->deta();
            double cellWidthPhi = seedCell->caloDDE()->dphi();
            double allowedDev = 0.001;
            if( (AbsDEta > cellWidthEta/2.+allowedDev) || (AbsDPhi > cellWidthPhi/2.+allowedDev) ) continue;

            // Assign shot to track

            // Store shot cells in pipmCellHash
            double shotE = 0.;
            std::vector<std::vector<const CaloCell*> > shotCells = currentShot->getCellVector(m_calo_id);
            int nCells_eta = shotCells.at(0).size();
            int seedIndex = nCells_eta/2;
            int windowSize = 3; // must be 1, 3 or 5
            if( windowSize%2!=1 && windowSize > nCells_eta) ATH_MSG_WARNING("Set window size for subtractionin EM1 properly! No energy will be subtracted in EM1");
            for(int iCell = 0; iCell != nCells_eta; ++iCell ){
                if(fabs(iCell-seedIndex)>windowSize/2) continue;
                double wtCell0=0.;
                double wtCell1=0.;
                if(shotCells.at(0).at(iCell) != NULL){
                    wtCell0 = m_caloWeightTool->wtCell(shotCells.at(0).at(iCell));
                    shotE+=shotCells.at(0).at(iCell)->e()*wtCell0;
                    pipmCellHash.push_back(shotCells.at(0).at(iCell)->caloDDE()->calo_hash());
                }
                if(shotCells.at(1).at(iCell) != NULL){
                    wtCell1 = m_caloWeightTool->wtCell(shotCells.at(1).at(iCell));
                    shotE+=shotCells.at(1).at(iCell)->e()*wtCell1;
                    pipmCellHash.push_back(shotCells.at(1).at(iCell)->caloDDE()->calo_hash());
                }
            }
            // Reduce EestInEcal by shot energy. ShotE is slightly more preciese than shotPt*cosh(currentShot->cluster()->eta()) (difference<1MeV)
            EestInEcal.at(iTrack) -= shotE;
            // Set number of photons in shot to 0.
            currentShot->setNPhotons(0);
            ATH_MSG_DEBUG("Assigned shot " << iShot << " to track " << iTrack << ". \tshotE = "<< shotE << "\tshotEta = " << seedEta << "\tshotPhi = " << seedCell->phi());
        }
    }
    */

    //---------------------------------------------------------------------
    // Start creating output container:
    // PS:  Don't do subtraction, just put cells in output container
    // EM1: Set energies of cells to 0 that have been matched to a track. Put cells in output container.
    // EM2: Sum up subtraction weights for each track. Have to normalize and subtract after this loop.
    //---------------------------------------------------------------------

    // Create vectors that will be used in a second loop over EM2 cells, only
    // vector of cells
    std::vector<const CaloCell*> EM2Cells;
    // vector of lateral weights for each cell and each track. Cells are in same order as in EM2Cells vector
    std::vector<std::vector<double> > cellSubWeights;
    // vector of sum of lateral weights for each track. Will be used for the normalization
    std::vector<double> sumCellSubWeights;
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        sumCellSubWeights.push_back(0.);
    }

    CaloCellList::list_iterator cellItr(pCells->begin()), cellItrE(pCells->end());
    for(; cellItr != cellItrE; ++cellItr) {
        const CaloCell* cell = (*cellItr);

        // get cell sampling
        int samp = cell->caloDDE()->getSampling();

        // only keep cells that are in PS, EM1 and EM2.
        if(samp>=7 || samp == 3 ) continue;

        if( samp!=2 && samp != 6 ){ // PS and EM1
            // Optional: Subtraction in EM1 (part 2 of 3)
            /*
            // Check if cell is in EM1 and if it is matched to a track
            bool isPipmCell = false;
            if(samp == 1 || samp == 5){
                //Ask cell for it's hash
                const IdentifierHash cellHash = cell->caloDDE()->calo_hash();

                for(unsigned iPipmCellHash=0;iPipmCellHash<pipmCellHash.size();++iPipmCellHash){
                    if(pipmCellHash.at(iPipmCellHash)==cellHash){
                        ATH_MSG_DEBUG("Set cell energy to 0. " << "eta = " << cell->eta() << "\tphi = " << cell->phi() << "\tE = " << cell->e());
                        isPipmCell = true;
                        break;
                    }
                }
            }
            */
            double subtractedEnergy = 0.;

            // Optional: Subtraction in EM1 (part 3 of 3)
            // if(isPipmCell == true) subtractedEnergy = cell->e();

            // Store cell in output container
            storeCell(cell, subtractedEnergy);
        }
        else{ // EM2

            // Current procedure in crack region: Linear Interpolation from 1.375 to 1.475 
            double interpolationFactorForCrack = 1.;
            double crackMax = 1.475;
            double crackMin = 1.375;
            double crackW   = crackMax - crackMin;
            if(samp!=0 && samp<4 && fabs(cell->eta())>crackMin)
                interpolationFactorForCrack=1.-(fabs(cell->eta())-crackMin)/crackW;
            if(samp>3  && fabs(cell->eta())<crackMax)
                interpolationFactorForCrack=1.-(crackMax-fabs(cell->eta()))/crackW;

            // Store weights of this cell for each track
            vector<double> thisCellSubWeights;
            double sumThisCellSubWeights = 0.;
            for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
                // Don't need to determine weights if no energy is to be subtracted for this track
                if( EestInEcal.at(iTrack)==0. ){
                    thisCellSubWeights.push_back(0.);
                    continue;
                }

                // Get info on track
                const xAOD::TrackParticle* track = tracks.at(iTrack);
                
                // check if tracks have been extrapolated to this sampling. Do so if this is not the case
                if(m_extrapolatedSamplings.at(samp)==false){
                    this->getExtrapolatedPositions(tracks,samp);
                    ATH_MSG_DEBUG("Extr to layer " << samp << "\teta = " << m_tracksEtaAtSampling.at(iTrack).at(samp) << "\tphi = " << m_tracksPhiAtSampling.at(iTrack).at(samp));
                    m_extrapolatedSamplings.at(samp)=true;
                }

                // set extrapolated track direction
                TLorentzVector extTrack;

                extTrack.SetPtEtaPhiE(track->pt(), m_tracksEtaAtSampling.at(iTrack).at(samp), m_tracksPhiAtSampling.at(iTrack).at(samp), track->e());

                // get eta/phi distance of cell to track
                double deltaEta = extTrack.Eta()-cell->eta();
                double deltaPhi = TVector2::Phi_mpi_pi( extTrack.Phi() - cell->phi());

                // TODO:
                // - Determine lateral weight using TH1
                double cellWidthEta = cell->caloDDE()->deta();
                double cellWidthPhi = cell->caloDDE()->dphi();
                double cellSubWeight = getLatWeight(samp, deltaEta, deltaPhi, cellWidthEta, cellWidthPhi, iTrack, track);


                // Get final weight by multiplying with interpolationFactorForCrack
                cellSubWeight*=interpolationFactorForCrack;
                sumCellSubWeights.at(iTrack)+=cellSubWeight;
                thisCellSubWeights.push_back(cellSubWeight);
                sumThisCellSubWeights+=cellSubWeight;
            }
            if(sumThisCellSubWeights == 0.){
                // No need to run subtraction in the second loop
                storeCell(cell, 0.);
            }
            else{
                EM2Cells.push_back(cell);
                cellSubWeights.push_back(thisCellSubWeights);
            }
        }
    }
    // Loop over EM2 cells. Subtract energy assigned to the cell and store it in the output cell container. 
    for(unsigned iCell=0; iCell!= EM2Cells.size();++iCell){
        const CaloCell* cell = EM2Cells.at(iCell);

        // Get cell weight 
        double wtCell = m_caloWeightTool->wtCell(cell);
        // wtCell=0 (happens very rarely)  would cause a FPE later
        if( wtCell == 0. ) continue;

        // Determine how much energy to subtract
        double subtractedEnergy = 0.;
        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
            if(EestInEcal.at(iTrack)==0. || sumCellSubWeights.at(iTrack)==0.) continue;
            subtractedEnergy+=EestInEcal.at(iTrack)*cellSubWeights.at(iCell).at(iTrack)/sumCellSubWeights.at(iTrack);
            // E_sub_track.at(iTrack)+=EestInEcal.at(iTrack)*cellSubWeights.at(iCell).at(iTrack)/sumCellSubWeights.at(iTrack);
        }
        // Store cell in output container
        storeCell(cell, subtractedEnergy);
    }
    // Check if total amount of subtracted energy is correct
    /*
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        ATH_MSG_INFO("EestInEcal.at(iTrack) = "<<EestInEcal.at(iTrack)<<"\tE_sub_track.at(iTrack) = "<<E_sub_track.at(iTrack)<<"\tsumCellSubWeights.at(iTrack) = "<<sumCellSubWeights.at(iTrack));
    }
    */
    return StatusCode::SUCCESS;
}

StatusCode TauPi0BonnCreateROI::eventFinalize(TauCandidateData* /* data */) {

    //---------------------------------------------------------------------
    // use the m_cellMakerTool to finalize the 
    // custom CaloCellContainer
    //---------------------------------------------------------------------
    CHECK( m_cellMakerTool->process(static_cast<CaloCellContainer*> (m_pPi0CellContainer)) );

    return StatusCode::SUCCESS;
}

StatusCode TauPi0BonnCreateROI::finalize() {
    // delete TH1 from parser
    return StatusCode::SUCCESS;
}

void TauPi0BonnCreateROI::getExtrapolatedPositions(
    vector<const xAOD::TrackParticle*> tracks,
    int sampling)
{
    for (unsigned iTrack = 0 ; iTrack < tracks.size(); ++iTrack ) {
        // extrapolate track to sampling 
        // FIXME: xAOD migration as soon as tool is available
        ATH_MSG_DEBUG( "Try extrapolation of track with pt = " << tracks.at(iTrack)->pt() << ", eta " << tracks.at(iTrack)->eta() << ", phi" << tracks.at(iTrack)->phi() 
                      << " to layer " << sampling);
        const Trk::TrackParameters* param_at_calo = 0;
        param_at_calo = m_trackToCaloTool->extrapolate( 
                *tracks.at(iTrack),
                (CaloCell_ID::CaloSample) sampling, 
                0.0, Trk::alongMomentum, Trk::pion);
     
        // store if track extrapolation successful, else use dummy values 
        if(param_at_calo){
            ATH_MSG_DEBUG( "Extrapolated track with eta=" << tracks.at(iTrack)->eta() 
                            << " phi="<<tracks.at(iTrack)->phi() 
                            << " to eta=" << param_at_calo->position().eta() 
                            << " phi="<<param_at_calo->position().phi() 
                            );
            m_tracksEtaAtSampling.at(iTrack).at(sampling)=param_at_calo->position().eta();
            m_tracksPhiAtSampling.at(iTrack).at(sampling)=param_at_calo->position().phi();
            delete param_at_calo;
        } 
        else ATH_MSG_DEBUG("Could not extrapolate track with pt = " << tracks.at(iTrack)->pt() << ", eta " << tracks.at(iTrack)->eta() << ", phi" << tracks.at(iTrack)->phi()
                          << " to layer " << sampling);
        //just keep default values in case extrapolation failed
    }
}

vector<double> TauPi0BonnCreateROI::getEstEcalEnergy(
    vector<const xAOD::TrackParticle*> tracks,
    const xAOD::CaloClusterContainer* clusterContainer, 
    const xAOD::TauJet* rtau)
{
    // Vector that stores hadronic energy associated to one track
    vector<double> EHcal;
        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        EHcal.push_back(0.);
    }

    ATH_MSG_DEBUG("new tau. eta = " << rtau->eta() << "\t phi = " << rtau->phi() );
        
    xAOD::CaloClusterContainer::const_iterator clusterItr  = clusterContainer->begin();
    xAOD::CaloClusterContainer::const_iterator clusterItrE = clusterContainer->end();

    int clusterNumber = -1;
    
    for(;clusterItr!=clusterItrE;++clusterItr){
        const xAOD::CaloCluster* cluster = (xAOD::CaloCluster*) *clusterItr;

        double deltaEtaToTau = rtau->eta()-cluster->eta();
        double deltaPhiToTau = TVector2::Phi_mpi_pi( rtau->phi() - cluster->phi());

        // Check deltaR^2<0.2^2 instead of deltaR<0.2, because it is computationally less expensive.
        double deltaRToTau_squared = deltaEtaToTau*deltaEtaToTau+deltaPhiToTau*deltaPhiToTau;
        if(deltaRToTau_squared>0.04) continue;
        

        clusterNumber++;

        ATH_MSG_DEBUG("Cluster number << " << clusterNumber << "\t energy = " << cluster->e() << "\t eta = " << cluster->eta() << "\t phi = " << cluster->phi() <<
                     "\t deltaEtaToTau = " << deltaEtaToTau << "\t deltaPhiToTau = " << deltaPhiToTau << "\t deltaRToTau_squared = " << deltaRToTau_squared );

           
        // Get sample with maximum energy and cluster energy in Hcal
        vector<double> energyInSamples=m_defaultValuesZero;
        // Get energy in Hcal samplings: Loop over cells
        const CaloClusterCellLink* theCellLink = cluster->getCellLinks(); 

        CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
        CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();

        for(;cellInClusterItr!=cellInClusterItrE;++cellInClusterItr){
            CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
            int sampling = cellInCluster->caloDDE()->getSampling();
            if(sampling <= 2 || (sampling >=4 && sampling <=6 ) || sampling >= CaloCell_ID::FCAL0) continue;
            energyInSamples.at(sampling) += cellInCluster->e()*cellInClusterItr.weight();
        }


        double cluster_EHcal = 0.;
        int maxSample = -1;
        double maxESample = 0.;
        for(unsigned iSample = 0; iSample < energyInSamples.size(); ++iSample){
            if(iSample == 3 || iSample >= 7 ) cluster_EHcal+=energyInSamples.at(iSample);
            if(energyInSamples.at(iSample)<maxESample) continue;
            maxSample = iSample;
            maxESample = energyInSamples.at(iSample);
        }
        // Apply cryostat correction
        if(energyInSamples.at(3) > 0. && energyInSamples.at(12) > 0.){
            cluster_EHcal += m_caloWeightTool->wtCryo() * sqrt(energyInSamples.at(3)*energyInSamples.at(12));
        }

        
        if(cluster_EHcal <= 0.){
            ATH_MSG_DEBUG("cluster_EHcal = " << cluster_EHcal/1000. << "<=0. Skip this cluster for Hcal estimate.");
            continue;
        }
        
        // check if tracks have been extrapolated to this sampling. Do so if this is not the case
        if(m_extrapolatedSamplings.at(maxSample)==false){
           this->getExtrapolatedPositions(tracks,maxSample);
           ATH_MSG_DEBUG("Extrapolate to layer " << maxSample << "\teta = " 
                         << m_tracksEtaAtSampling.at(0).at(maxSample) << "\t phi = " << m_tracksPhiAtSampling.at(0).at(maxSample) );
           m_extrapolatedSamplings.at(maxSample)=true;
        }

        // Assign cluster to track
        int closestTrack = -1;
        //double dEtaClosestTrack = 10;
        //double dPhiClosestTrack = 10;
        double dRToClosestTrack_squared = 0.16; // XXX can be tuned later
        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
            const xAOD::TrackParticle* track = tracks.at(iTrack);


            // set extrapolated track direction
            TLorentzVector extTrack;
            ATH_MSG_DEBUG("filling extrapolated Track number " << iTrack << ", pt = " << track->pt() << ", eta = " << m_tracksEtaAtSampling.at(iTrack).at(maxSample) << ", phi " <<  m_tracksPhiAtSampling.at(iTrack).at(maxSample) << ", e = " << track->e() );

            extTrack.SetPtEtaPhiE(track->pt(), m_tracksEtaAtSampling.at(iTrack).at(maxSample), m_tracksPhiAtSampling.at(iTrack).at(maxSample), track->e());

            // get eta/phi distance of cell to track
            double deltaEta = extTrack.Eta()-cluster->eta();
            double deltaPhi = TVector2::Phi_mpi_pi( extTrack.Phi() - cluster->phi());;

            double deltaRToTrack_squared = deltaEta*deltaEta+deltaPhi*deltaPhi;
            ATH_MSG_DEBUG("Track number " << iTrack << ", extTrack.Eta() = " << extTrack.Eta() << ", extTrack.Phi() = " << extTrack.Phi());
            
            if(deltaRToTrack_squared>=dRToClosestTrack_squared) continue;
            closestTrack = iTrack;
            //dEtaClosestTrack = deltaEta;
            //dPhiClosestTrack = deltaPhi;
            dRToClosestTrack_squared = deltaRToTrack_squared;
        }
        if(closestTrack == -1 && dRToClosestTrack_squared > 0.04){
            //ATH_MSG_DEBUG("dRToClosestTrack_squared = " << dRToClosestTrack_squared << ", dEta = " << dEtaClosestTrack << ", dPhi = " << dPhiClosestTrack 
            //                << ". Skip cluster for Hcal estimate. \tcluster_EHcal = " << cluster_EHcal/1000.);
            continue; // Didn't find a track
        }
        EHcal.at(closestTrack) += cluster_EHcal;
        ATH_MSG_DEBUG("Cluster associated to track " << closestTrack << "\tcluster_EHcal = " << cluster_EHcal/1000.);
    }
    // Get energy estimate in ECAL
    vector<double> E_ests;
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        double E_est = tracks.at(iTrack)->e()-EHcal.at(iTrack);
        // energy cant be less than 0
        if(E_est < 0) E_est = 0.;
        // energy cant be more than track momentum
        if(E_est > tracks.at(iTrack)->e()) E_est = tracks.at(iTrack)->e();
        E_ests.push_back(E_est);
    }
    return E_ests;
}

double TauPi0BonnCreateROI::getLatWeight(int samp,
                                        double deltaEta,
                                        double deltaPhi,
                                        double cellWidthEta,
                                        double cellWidthPhi,
                                        unsigned trackNumber,
                                        const xAOD::TrackParticle* track
                                       )
{
    // No need to subtract very far away from the track
    if(fabs(deltaEta)>0.15 || fabs(deltaPhi)>0.15) return 0.;

    TH1* histo;

    // Store histograms in order to speed up the algorithm
    // The 

    // Check if TH1 is already in one of the maps
    std::map<int, TH1*>::iterator itr;
    if( samp==2 ) itr = m_trackHistMapBarrel.find(trackNumber);
    else          itr = m_trackHistMapEndcap.find(trackNumber);

    if( (samp==2 && itr != m_trackHistMapBarrel.end()) || (samp==6 && itr != m_trackHistMapEndcap.end()) ){
        histo = itr->second;
    }
    else{
        //---------------------------------------------------------------------
        // Need to get bin key and histogram from parser
        // set variables that can be used for parameterisation 
        // Note: if value outside bin range, parser will choose first or last 
        //       bin
        //---------------------------------------------------------------------
        m_pt     = track->pt();
        m_abseta = fabs(track->eta());
        // FIXME: Preliminary fix for crack:
        if(m_abseta>1.34 && samp==2 ) m_abseta = 1.34;
        if(m_abseta<1.56 && samp==6 ) m_abseta = 1.56;
        m_sampling = (double) samp;
        string key = m_latParser->getBinKey();

        ATH_MSG_DEBUG("m_pt = " << m_pt << "\tm_abseta = " << m_abseta << "\tm_hadf = " << m_hadf << "\tm_sampling = " << m_sampling << "\tkey = " << key);

        histo = m_latParser->getTH1();

        if(samp==2) m_trackHistMapBarrel[trackNumber] = histo;
        else        m_trackHistMapEndcap[trackNumber] = histo;
    }
    
    // Get cell weight from histogram
    int bin = histo->FindBin(fabs(deltaEta), deltaPhi*track->charge());
    double cellWeight = histo->GetBinContent(bin);

    // multiply by cell area in order to account for varying cell sizes in the endcap
    cellWeight*=cellWidthEta*cellWidthPhi;

    return cellWeight;
}

void TauPi0BonnCreateROI::storeCell(const CaloCell* cell, 
                                    double subtractedEnergy){
    // Store cell in output container if it is a new cell
    // Produce a copy of the cell, in order to prevent 
    // the energy of the original cell to be changed. 
    // Store unweighted cells, since the cell weights are applied during reclustering
    
    //Ask cell for it's hash
    const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
    //Check if this cell is already part of reducedCellContainer
    bool isNewCell = (m_addedCellsMap.at(cellHash)==NULL);

    if(isNewCell){
        CaloCell* copyCell = cell->clone();
        if(subtractedEnergy>0.){
            // Get cell weight 
            double wtCell = m_caloWeightTool->wtCell(cell);
            // wtCell=0 (happens very rarely)  would cause a FPE
            if( wtCell != 0. ) copyCell->setEnergy( cell->e() - subtractedEnergy/wtCell );
        }
        m_pPi0CellContainer->push_back(const_cast<CaloCell*> (copyCell));
        m_addedCellsMap[cellHash] = copyCell;
    }
    // If the cell has been already stored the energy is subtracted for the already existing copy
    else if(subtractedEnergy>0.){
        CaloCell* copyCell = m_addedCellsMap.at(cellHash);
        // Get cell weight 
        double wtCell = m_caloWeightTool->wtCell(cell);
        // wtCell=0 (happens very rarely)  would cause a FPE
        if( wtCell != 0. ) copyCell->setEnergy( cell->e() - subtractedEnergy/wtCell );
    }
}

