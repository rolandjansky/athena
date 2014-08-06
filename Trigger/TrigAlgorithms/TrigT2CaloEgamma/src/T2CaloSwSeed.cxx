/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************

 NAME:     T2CaloSwSeed.cxx
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   A. Phan

 NOTES:    Based loosely on T2CaloEgamma.cxx and closely to EgammaSamp2Fex.cxx.

 *****************************************************************************/

#include "TrigT2CaloEgamma/T2CaloSwSeed.h"

#include "IRegionSelector/IRoiDescriptor.h"


class ISvcLocator;

T2CaloSwSeed::T2CaloSwSeed(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator),
  m_data("TrigDataAccess/TrigDataAccess"),
  m_log(0),
  m_calibs(this) {

  declareProperty("TrigEMClusterKey",m_trigEmClusterKey = "T2CaloTrigEMCluster");

  declareProperty("CalibList",m_calibs,"List of Calibration Tools");

  declareProperty("EtaWidth", m_etaWidth = 0.2, "Eta Width of the RoI");
  declareProperty("PhiWidth", m_phiWidth = 0.2, "Phi Width of the RoI");

  declareProperty("EtaWidthForID", m_etaWidthForID = 0.1, "Eta Width of the RoI for ID");
  declareProperty("PhiWidthForID", m_phiWidthForID = 0.1, "Phi Width of the RoI for ID");

  declareProperty("WindowThreshold", m_windowThreshold = 500, "Sliding Window Threshold");
  declareProperty("NoiseThreshold", m_noiseThreshold = 0, "Noise Threshold");

  declareProperty("OverlapDistance",m_overlap = 0.1,"Duplicate Cluster Distance");

  declareProperty("NoCut",m_noCut = 0,"No Cut Chain Flag");

  declareProperty("TrigDataAccess",m_data, "Data Access for Algorithm");
}


T2CaloSwSeed::~T2CaloSwSeed() {
  if (m_log) delete m_log;
}


HLT::ErrorCode T2CaloSwSeed::hltInitialize() {

  if (!m_log) m_log = new MsgStream(msgSvc(), name());

  // Store Gate will handle input/output data to/from the algorithm.
  if (service("StoreGateSvc", m_storeGate).isFailure()) {
    (*m_log) << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return HLT::FATAL;
  }

  // Algorithms need m_larMgr
  StoreGateSvc* detStore = 0;
  if ( service("DetectorStore",detStore).isSuccess() ) {
    if ((detStore->retrieve(m_larMgr)).isFailure()) {
      (*m_log) << MSG::ERROR
               << "Unable to retrieve CaloIdManager from DetectorStore"
               << endreq;
      return HLT::FATAL;
    } // End of if for retrieve of CaloIdManager
  } // End of if for DetectorStore
  else return HLT::FATAL;

  if ((m_data.retrieve()).isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get m_data" << endreq;
    return HLT::FATAL;
  }

  if ( m_calibs.retrieve().isFailure() )
    (*m_log) << MSG::DEBUG << "No Calibration Available" << endreq;

  // Monitored variables
  declareMonitoredVariable("nRoIs",mm_nRoIs);
  declareMonitoredVariable("nCachedRoIs",mm_nCachedRoIs);
  declareMonitoredStdContainer("nCachedClusters",mm_nCachedClusters);
  declareMonitoredStdContainer("nWindows",mm_nWindows);
  declareMonitoredStdContainer("nSeeds",mm_nSeeds);
  declareMonitoredStdContainer("nClusters",mm_nClusters);
  declareMonitoredStdContainer("nOverlaps",mm_nOverlaps);
  declareMonitoredStdContainer("Eta",mm_Eta);
  declareMonitoredStdContainer("Phi",mm_Phi);
  declareMonitoredStdContainer("L1dR",mm_L1dR);
  declareMonitoredStdContainer("L2dR",mm_L2dR);

  // Reset cluster variables
  m_cellSeeds.resize(m_maxSeeds,0);
  m_etaSeeds.resize(m_maxSeeds,0);
  m_phiSeeds.resize(m_maxSeeds,0);

  m_energy37Lay2.resize(m_maxSeeds,0);
  m_energy37Lay2NegPhi.resize(m_maxSeeds,0);
  m_energy37Lay2PosPhi.resize(m_maxSeeds,0);
  m_energy77Lay2.resize(m_maxSeeds,0);
  m_energy35Lay2.resize(m_maxSeeds,0);
  m_weightEta1.resize(m_maxSeeds,0);
  m_weightEta2.resize(m_maxSeeds,0);
  m_clusterWidth35.resize(m_maxSeeds,0);

  m_totalEnergy.resize(m_maxSeeds,0);
  m_energySamp2BR.resize(m_maxSeeds,0);
  m_energyRawSamp2BR.resize(m_maxSeeds,0);
  m_energySamp2EC.resize(m_maxSeeds,0);
  m_energyRawSamp2EC.resize(m_maxSeeds,0);
  m_energyEta.resize(m_maxSeeds,0);
  m_energyPhi.resize(m_maxSeeds,0);

  m_energyNegPhi.resize(m_maxSeeds,0);
  m_energyNegPhiConv.resize(m_maxSeeds,0);
  m_energyPosPhi.resize(m_maxSeeds,0);

  return HLT::OK;
}


HLT::ErrorCode T2CaloSwSeed::hltFinalize() {
  return HLT::OK;
}


HLT::ErrorCode T2CaloSwSeed::hltExecute(
  std::vector<std::vector<HLT::TriggerElement*> >& inputVector,
  unsigned int outputType) {

  std::vector<HLT::TriggerElement*> m_roiList;
  std::vector<xAOD::TrigEMCluster*> m_outputClusterList;

  // Start monitoring and reset variables
  beforeExecMonitors().ignore();
  mm_nRoIs = 0;
  mm_nCachedRoIs = 0;
  mm_nCachedClusters.clear();
  mm_nWindows.clear();
  mm_nSeeds.clear();
  mm_nClusters.clear();
  mm_nOverlaps.clear();
  mm_Eta.clear();
  mm_Phi.clear();
  mm_L1dR.clear();
  mm_L2dR.clear();

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG )
    (*m_log) << MSG::DEBUG << "in execute()" << endreq;
#endif

  // Go through all given inputTEs
  //   Check if they have been run on before
  //     If so, add cached information to list
  //     If not, add to list of rois to analyse
  HLT::ErrorCode hltStatus;

  std::vector<HLT::TEVec>::iterator it;
  for(it = inputVector.begin();
      it != inputVector.end();
      it++) {
    HLT::TEVec::iterator inner_it;
    for(inner_it = it->begin();
        inner_it != it->end();
        inner_it++) {

      const TrigRoiDescriptor* roiDescriptor;
	    hltStatus = getFeature((*inner_it),roiDescriptor);

	    mm_nRoIs++;

      if(!roiDescriptor) {
        (*m_log) <<  MSG::WARNING << " Failed to find RoiDescriptor " << endreq;
        return StatusCode::FAILURE;
      }

      std::multimap <unsigned int, HLT::TriggerElement*>::iterator
      cachedTEList_it = m_cachedTEList.find(roiDescriptor->roiId());

      // Not previously analysed
      if(cachedTEList_it == m_cachedTEList.end()) {

#ifndef NDEBUG
          (*m_log) << MSG::DEBUG
                   << "analyse roi "
                   << roiDescriptor->roiId()
                   << endreq;
#endif

        m_roiList.push_back(*inner_it);
      }
      // Previously analysed
      else {

#ifndef NDEBUG
          (*m_log) << MSG::DEBUG
                   << "roi "
                   << roiDescriptor->roiId()
                   << " previously analysed"
                   << endreq;
#endif

        mm_nCachedRoIs++;
        int nCachedClusters = 0;

        // Get all TEs from roi from cache
        std::pair< std::multimap <unsigned int, HLT::TriggerElement*>::iterator,
          std::multimap <unsigned int, HLT::TriggerElement*>::iterator >
          cachedTEList_pair = m_cachedTEList.equal_range(roiDescriptor->roiId());

        std::multimap <unsigned int, HLT::TriggerElement*>::iterator
          cachedTEList_it;

        for (cachedTEList_it = cachedTEList_pair.first;
             cachedTEList_it != cachedTEList_pair.second;
             cachedTEList_it++) {

          // TE had no clusters
          if(cachedTEList_it->second == 0) {
            break;
          }

          nCachedClusters++;

          // Get cluster
          const xAOD::TrigEMCluster *cluster;
          getFeature(cachedTEList_it->second,cluster,m_trigEmClusterKey);
          m_outputClusterList.push_back(const_cast<xAOD::TrigEMCluster*>(cluster));

          // Create new outputTE
          HLT::TriggerElement* outputTE =
            config()->getNavigation()->addNode((*inner_it),outputType);
            //addRoI(outputType,cluster->eta(),cluster->phi());
          outputTE->setActiveState(true);

          // Save (cached) feature to outputTE:
          m_config->getNavigation()->copyAllFeatures(
            cachedTEList_it->second, outputTE);
        }

        mm_nCachedClusters.push_back(nCachedClusters);
      }
    }
  }

  std::vector<HLT::TriggerElement*>::iterator m_roiList_it;

  // Search each new RoI for cluster seeds
  for(m_roiList_it = m_roiList.begin(); m_roiList_it != m_roiList.end(); m_roiList_it++) {

    // Clear variables from previous RoI
    m_cellMap.clear();
    m_stripMap.clear();
    m_neighboursMap.clear();

    m_seedMap.clear();
    m_windowMap.clear();

    const TrigRoiDescriptor* roiDescriptor;
    getFeature(*m_roiList_it,roiDescriptor);

#ifndef NDEBUG
      (*m_log) << MSG::DEBUG
               << "analysing roi: " << roiDescriptor->roiId()
               << " eta: " << roiDescriptor->eta()
               << " phi: " << roiDescriptor->phi()
               << endreq;
#endif


      /// NB: here are we defining a new roiDescriptor ?? The sizes must come from the 
      ///     roiDescriptor now so if we want to define a*new* region, we should create
      ///     a new roi descritor and pass this in to the RegionSelector, and *then* 
      ///     we should attach the RoiDescriptor to the navigation 

    // Restrict search within the roi
  
    double etamin = std::max(-2.5,roiDescriptor->eta() - m_etaWidth);
    double etamax = std::min( 2.5,roiDescriptor->eta() + m_etaWidth);
    double phimin = HLT::wrap_phi(roiDescriptor->phi() - m_phiWidth);
    double phimax = HLT::wrap_phi(roiDescriptor->phi() + m_phiWidth);
    float etaL1 = roiDescriptor->eta();
    float phiL1 = roiDescriptor->phi();

    /// NB: need to wrap to -pi .. pi (not 0 .. 2pi)
    while (phimin < -M_PI) phimin += 2. * M_PI;
    while (phimax >  M_PI) phimax -= 2. * M_PI;
    if (phiL1+0.001>=M_PI) phiL1-=0.001;
    if (phiL1-0.001<=-M_PI) phiL1+=0.001;

    /// create a roi descriptor here

    TrigRoiDescriptor newroi( roiDescriptor->eta(), etamin, etamax, 
			      roiDescriptor->phi(), phimin, phimax ); 

    // 1. Find seed cells (local maxima windows in layer 2 ROI above threshold)
    // 2. Find Et weighted eta, phi in 3*7 cell (layer 2) (photon + e id)
    // 3. Find Et in cells of sizes 3*3, 3*7, 7*7 (layer 2 + strips)
    // 4. Find cluster width in 3*5 cell (layer 2) (photon id)
    // 5. Find strip energies and eta 2*5 (layer 1)
    // 6. Find frac73 (photon id), (E1-E2)/(E1+E2) (e + photon id)

    const LArEM_ID*    emID  = m_larMgr->getEM_ID();
    const CaloCell_ID* caloID = m_larMgr->getCaloCell_ID();

    // Access sampling 1 RoI data
    int sampling = 1;
    m_data->RegionSelector(sampling,newroi);

    uint32_t error = 0;
    if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() )
      return StatusCode::FAILURE;
    error |= m_data->report_error();

    // Map strips
    for(m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
      const LArCell* larcell = (*m_it);

      Identifier cellID = larcell->ID();
      IdentifierHash cellHash = caloID->calo_cell_hash(cellID);

      // Do not map if below noise threshold
      if(larcell->energy() < m_noiseThreshold) {
        continue;
      }

      m_stripMap[cellHash] = larcell;
    }

    // Access sampling 2 RoI data
    sampling = 2;
    m_data->RegionSelector(sampling,newroi);

    if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() )
      return StatusCode::FAILURE;
    error |= m_data->report_error();

    // Map cells
    int nCells = 0;
    for(m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
      const LArCell* larcell = (*m_it);
      nCells++;

      Identifier cellID = larcell->ID();
      IdentifierHash cellHash = caloID->calo_cell_hash(cellID);

      // Do not map if below noise threshold
      if(larcell->energy() < m_noiseThreshold) {
        continue;
      }

      m_cellMap[cellHash] = larcell;

      std::vector<IdentifierHash> neighbours;
      emID->get_neighbours(cellHash,LArNeighbours::all3DwithCorners,neighbours);
      m_neighboursMap[cellHash] = neighbours;
    }

    // Calculate window energies and map if above threshold
    std::map < IdentifierHash , const LArCell* >::iterator cell_it;
    for(cell_it = m_cellMap.begin();
        cell_it != m_cellMap.end();
        cell_it++) {

      double windowEnergy = (cell_it->second)->energy();

      std::vector<IdentifierHash> neighbours =
        m_neighboursMap.find(cell_it->first)->second;

      std::vector<IdentifierHash>::iterator neighbours_it;
      for(neighbours_it = neighbours.begin();
          neighbours_it != neighbours.end();
          neighbours_it++) {
        std::map < IdentifierHash , const LArCell* >::iterator neighbourCell
          = m_cellMap.find(*neighbours_it);
        if(neighbourCell != m_cellMap.end()) {
          windowEnergy += (neighbourCell->second)->energy();
        }

        std::map < IdentifierHash , const LArCell* >::iterator neighbourStrip
          = m_stripMap.find(*neighbours_it);
        if(neighbourStrip != m_stripMap.end()) {
          windowEnergy += (neighbourStrip->second)->energy()/4;
        }
      }

#ifndef NDEBUG
      (*m_log) << MSG::DEBUG << "Window energy: " << windowEnergy << endreq;
#endif

      if(windowEnergy > m_windowThreshold) {
        m_windowMap[cell_it->first] = windowEnergy;
      }
    }

#ifndef NDEBUG
      (*m_log) << MSG::DEBUG
               << "nWindows found: "
               << m_windowMap.size()
               << " above threshold "
               << m_windowThreshold
               << endreq;
#endif

    mm_nWindows.push_back(m_windowMap.size());

    // Select local maxima
    std::map<IdentifierHash , double>::iterator window_it;
    for(window_it = m_windowMap.begin();
        window_it != m_windowMap.end();
        window_it++) {

      double seedEnergy = window_it->second;

      std::vector<IdentifierHash> neighbours =
        m_neighboursMap.find(window_it->first)->second;

      int max = 1;

      // Go through neighbouring window energies
      std::vector<IdentifierHash>::iterator neighbours_it;
      for(neighbours_it = neighbours.begin();
          neighbours_it != neighbours.end();
          neighbours_it++) {

        std::map < IdentifierHash,double >::iterator window =
          m_windowMap.find(*neighbours_it);

        if(window != m_windowMap.end()) {
          double windowEnergy = window->second;
          // Found a neighbour with higher energy
          if(windowEnergy > seedEnergy) {
            max = 0;
          }
        }
        if(max == 0) {
          break;
        }
      }

      if(max == 0) {
        continue;
      }

      // No neighbours have lower energy
      //  Check that no seeds are within the overlap distance
      std::map<IdentifierHash , double>::iterator seed_it;
      std::map<IdentifierHash , double>::iterator remove_it = m_seedMap.end();
      for(seed_it = m_seedMap.begin(); seed_it != m_seedMap.end(); seed_it++) {
        if(remove_it != m_seedMap.end() )
           m_seedMap.erase(remove_it);
        remove_it = m_seedMap.end();

        double otherEnergy = seed_it->second;

        const LArCell* otherCell = (m_cellMap.find(seed_it->first))->second;
        const LArCell* seedCell = (m_cellMap.find(window_it->first))->second;

        double deta = fabs(otherCell->eta() - seedCell->eta());
        double dphi = fabs(otherCell->phi() - seedCell->phi());

        // If seed is found within distance
        if(deta < m_overlap && dphi < m_overlap) {
          if(otherEnergy > seedEnergy) { // don't add if lower energy
            max = 0;
          } else if(otherEnergy < seedEnergy) { // remove the other if high energy
            remove_it = seed_it;
          }
        }

        // If a seed is found with the same energy, don't add this one
        if(otherEnergy == seedEnergy) {
          max = 0;
          break;
        }
      }

      if(max == 0) {
        continue;
      }

      // If the window has reached here, it is a possible cluster seed
      m_seedMap[window_it->first] = seedEnergy;
    }


    // Initialise all seed quantities
    int nSeed;
    for(nSeed = 0; nSeed < m_maxSeeds; nSeed++) {
      m_energy37Lay2[nSeed] = 0;
      m_energy37Lay2NegPhi[nSeed] = 0;
      m_energy37Lay2PosPhi[nSeed] = 0;
      m_energy77Lay2[nSeed] = 0;
      m_energy35Lay2[nSeed] = 0;
      m_weightEta1[nSeed] = 0;
      m_weightEta2[nSeed] = 0;
      m_clusterWidth35[nSeed] = 0;

      m_totalEnergy[nSeed] = 0;
      m_energySamp2BR[nSeed] = 0;
      m_energyRawSamp2BR[nSeed] = 0;
      m_energySamp2EC[nSeed] = 0;
      m_energyRawSamp2EC[nSeed] = 0;

      m_energyEta[nSeed] = 0;
      m_energyPhi[nSeed] = 0;

      m_energyNegPhi[nSeed] = 0;
      m_energyNegPhiConv[nSeed] = 0;
      m_energyPosPhi[nSeed] = 0;
    }

    // Put seed information into arrays and make cluster vector
    int nSeeds = m_seedMap.size();

#ifndef NDEBUG
      (*m_log) << MSG::DEBUG
               << "nSeeds found: "
               << nSeeds
               << endreq;
#endif

    mm_nSeeds.push_back(nSeeds);

    if(nSeeds == 0) {
      // No seeds were found
      mm_nClusters.push_back(0);

      if(m_noCut) {

        // If in a NoCut chain
        //  make a cluster with position from L1
        m_cellSeeds[0] = NULL;
        m_etaSeeds[0] = etaL1;
        m_phiSeeds[0] = phiL1;

        nSeeds = 1;
      } else {
        // If in a normal chain
        //  just add the RoI to the cache list
        m_cachedTEList.insert(std::pair<unsigned int , HLT::TriggerElement*>
          (roiDescriptor->roiId(),0));

        // Skip to next RoI
        continue;
      }

    } else {
      // Some seeds were found
      nSeed = 0;
      std::map<IdentifierHash , double>::iterator seed_it;
      for(seed_it = m_seedMap.begin(); seed_it != m_seedMap.end(); seed_it++) {

        const LArCell* cell = (m_cellMap.find(seed_it->first))->second;

        m_cellSeeds[nSeed] = cell;
        m_etaSeeds[nSeed] = cell->eta();
        m_phiSeeds[nSeed] = cell->phi();

#ifndef NDEBUG
        (*m_log) << MSG::DEBUG
                 << " seed found:"
                 << " eta: " << cell->eta()
                 << " phi: " << cell->phi()
                 << " energy: " << seed_it->second
                 << endreq;
#endif
        nSeed++;
      }
    }

    // For each RoI cell
    for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {
      const LArCell *larcell = (*m_it);

      //CaloSampling::CaloSample samplingCell = CaloSampling::getSampling(*larcell);
      CaloSampling::CaloSample samplingCell = larcell->caloDDE()->getSampling();

      double etaCell = larcell->eta();
      double phiCell = larcell->phi();
      double energyCell = larcell->energy();

      double cellSizeEta;
      double cellSizePhi;
      if(emID->is_em_barrel( larcell->ID() ) ) {
        cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
        cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
      } else {
        cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
        cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
      }

      // For each seed
      for(nSeed = 0; nSeed < nSeeds; nSeed++) {

        // Find position of current cell w.r.t. seed
        double deta = fabs( etaCell - m_etaSeeds[nSeed] );
        double dphi = fabs( phiCell - m_phiSeeds[nSeed] );

        if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28

        // Electromagnetic measurements, done by layer
        // layer 2: Et weighted eta, phi, summed Et in 3*7,
        //          cluster width in 3*5 cell
        // emEnergy in 3*7 cell is also calculated, all samplings

        // 1. do the 7*7 stuff
        int nCellsEta = 7;
        int nCellsPhi = 7;
        if ( deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 &&
             dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01 ) {
          // 2. do the 3*7 stuff
          nCellsEta = 3;
          nCellsPhi = 7;
          if ( deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 &&
               dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01 ) {
            m_energyEta[nSeed] += energyCell * etaCell;
            m_energy37Lay2[nSeed] += energyCell;

            m_totalEnergy[nSeed] += energyCell;

            if (samplingCell == CaloSampling::EMB2) {
              m_energySamp2BR[nSeed] += energyCell;
              m_energyRawSamp2BR[nSeed] += energyCell;
            } else if (samplingCell == CaloSampling::EME2) {
              m_energySamp2EC[nSeed] += energyCell;
              m_energyRawSamp2EC[nSeed] += energyCell;
            }

            if ( phiCell > 0.) {  // phi wrap-around
              m_energyPosPhi[nSeed] += double(energyCell) * phiCell;
              m_energy37Lay2PosPhi[nSeed] += energyCell;
            } else {
              m_energyNegPhi[nSeed] += double(energyCell) * phiCell;
              m_energyNegPhiConv[nSeed] += double(energyCell)*(phiCell+2.0*M_PI);
              m_energy37Lay2NegPhi[nSeed] += energyCell;
            }

            // 3. do the 3*5 stuff
            nCellsEta = 3;
            nCellsPhi = 5;
            if ( deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 &&
                 dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01 ) {
              m_weightEta1[nSeed] += energyCell * etaCell;
              m_weightEta2[nSeed] += energyCell * etaCell * etaCell;
              m_energy35Lay2[nSeed] += energyCell;
            } // End of do the 3*5 stuff
          } // End of do the 3x7 stuff
          m_energy77Lay2[nSeed] += energyCell;
        } // End of do the 7*7 stuff
      } // end loop over seeds
    } // end of loop over cells

    int nClusters = 0;
    int nOverlaps = 0;

    // Normalise energy weighted angles and calculate values for each cluster
    for(nSeed = 0; nSeed < nSeeds; nSeed++) {
      // Phi wrap-around
      double AvgNegPhi = 0.;
      double AvgPosPhi = 0.;

      if( (m_energy37Lay2PosPhi[nSeed] + m_energy37Lay2NegPhi[nSeed]) > 0. ) {
        m_energyEta[nSeed] /=
          (m_energy37Lay2PosPhi[nSeed] + m_energy37Lay2NegPhi[nSeed]);

        if(m_energy37Lay2NegPhi[nSeed] > 0. ){
           AvgNegPhi = m_energyNegPhi[nSeed] / m_energy37Lay2NegPhi[nSeed];
        } else {
          AvgNegPhi = -999.0;
        }

        if(m_energy37Lay2PosPhi[nSeed] > 0. ){
          AvgPosPhi = m_energyPosPhi[nSeed] / m_energy37Lay2PosPhi[nSeed];
        } else {
          AvgPosPhi = -999.0;
        }

        if(AvgPosPhi == -999.0) {
          if(AvgNegPhi != -999.0) {
            m_energyPhi[nSeed] = AvgNegPhi;
          }
        }

        if(AvgNegPhi == -999.0) {
          if(AvgPosPhi != -999.0) {
            m_energyPhi[nSeed] = AvgPosPhi;
          }
        }

        if(AvgNegPhi != -999.0 && AvgPosPhi != -999.0) {
          if( (AvgNegPhi > (-M_PI/2.0)) && (AvgPosPhi < (M_PI/2.0)) ) {
            m_energyPhi[nSeed] =
              (m_energyNegPhi[nSeed] + m_energyPosPhi[nSeed]) /
              (m_energy37Lay2NegPhi[nSeed] + m_energy37Lay2PosPhi[nSeed]);
          } else {
            if((AvgNegPhi < (-M_PI/2.0)) && (AvgPosPhi > (M_PI/2.0))) {
              m_energyPhi[nSeed] =
                (m_energyNegPhiConv[nSeed] + m_energyPosPhi[nSeed]) /
                (m_energy37Lay2NegPhi[nSeed] + m_energy37Lay2PosPhi[nSeed]);
              if(m_energyPhi[nSeed] > M_PI) {
                m_energyPhi[nSeed] = m_energyPhi[nSeed] - 2*M_PI;
              }
            }
          }
        }

      } else {
        m_energyEta[nSeed] = 99. ;
        m_energyPhi[nSeed] = 0. ;
      }

      // Calculate cluster width
      if ( m_energy35Lay2[nSeed] > 0. ){
        m_clusterWidth35[nSeed] =
          (m_weightEta2[nSeed]/m_energy35Lay2[nSeed]) -
          (m_weightEta1[nSeed]/m_energy35Lay2[nSeed]) *
          (m_weightEta1[nSeed]/m_energy35Lay2[nSeed]);
        m_clusterWidth35[nSeed] > 0. ? m_clusterWidth35[nSeed] =
          sqrt(m_clusterWidth35[nSeed]) : m_clusterWidth35[nSeed] = 99.;
      } else {
        m_clusterWidth35[nSeed] = 99.;
      }

      xAOD::TrigEMCluster *cluster = new xAOD::TrigEMCluster();
      for(int i = 0; i < CaloSampling::CaloSample::Unknown; i++) {
        cluster->setEnergy((CaloSampling::CaloSample) i,0.);
      }

      cluster->setClusterQuality(error);
      cluster->setE237(m_energy37Lay2[nSeed]);
      cluster->setE277(m_energy77Lay2[nSeed]);
      cluster->setWeta2(m_clusterWidth35[nSeed]);
      cluster->setEnergy(CaloSampling::EMB2, m_energySamp2BR[nSeed]);
      cluster->setRawEnergy(CaloSampling::EMB2, m_energyRawSamp2BR[nSeed]);
      cluster->setEnergy(CaloSampling::EME2, m_energySamp2EC[nSeed]);
      cluster->setRawEnergy(CaloSampling::EME2, m_energyRawSamp2EC[nSeed]);
      cluster->setRawEnergy(m_totalEnergy[nSeed]);
      if ( m_energyPhi[nSeed] < -M_PI || m_energyPhi[nSeed] > +M_PI ||
           fabsf ( m_energyEta[nSeed] ) > 10.0 ) {
        cluster->setEta(etaL1);
        cluster->setPhi(phiL1);
        cluster->setRawEta(etaL1);
        cluster->setRawPhi(phiL1);
      } else {
        cluster->setEta(m_energyEta[nSeed]);
        cluster->setPhi(m_energyPhi[nSeed]);
        cluster->setRawEta(m_energyEta[nSeed]);
        cluster->setRawPhi(m_energyPhi[nSeed]);
      }
      cluster->setNCells(nCells);
      cluster->setRoIword(roiDescriptor->roiWord());

      // Now we have all the information for this new cluster,
      //   check for overlaps
      bool remove = 0;

      std::vector<xAOD::TrigEMCluster*>::iterator m_outputClusterList_it;

      for(m_outputClusterList_it = m_outputClusterList.begin();
          m_outputClusterList_it < m_outputClusterList.end();
          m_outputClusterList_it++) {

        double otherEta = (*m_outputClusterList_it)->eta();
        double otherPhi = (*m_outputClusterList_it)->phi();

        double deta = fabs(otherEta - cluster->eta());
        double dphi = fabs(otherPhi - cluster->phi());

        // if an overlap is found, remove the cluster
        if(deta < m_overlap && dphi < m_overlap) {
          nOverlaps++;
          remove = 1;

          break;
        }
      }

      if(remove == 1) {
        delete cluster;
      }
      // otherwise,
      //  run calibrations,
      //  add it to output and cached clusters
      //  and create outputTE
      else {
        if(m_cellSeeds[nSeed] != NULL) {
          setCaloDetDescrElement(m_cellSeeds[nSeed]->caloDDE());

	  if( m_caloDDE )
          for( ToolHandleArray<IEgammaCalibration>::const_iterator
               ical=m_calibs.begin();
               ical != m_calibs.end(); ++ical )
            (*ical)->makeCorrection(cluster,m_caloDDE);
        }

        nClusters++;
        m_outputClusterList.push_back(cluster);

        mm_Eta.push_back(cluster->eta());
        mm_Phi.push_back(cluster->phi());
        double dEta = etaL1 - cluster->eta();
        double dPhi = phiL1 - cluster->phi();
        double dR = sqrt(dEta*dEta + dPhi*dPhi);
        mm_L1dR.push_back(dR);

#ifndef NDEBUG
        if ( (*m_log).level() <= MSG::DEBUG )
          (*m_log) << MSG::DEBUG
                   << "cluster added: "
                   << " eta: " <<  cluster->eta()
                   << " phi: " <<  cluster->phi()
                   << endreq;
#endif

        HLT::TriggerElement* outputTE =
          config()->getNavigation()->addNode(*m_roiList_it,outputType);
          //addRoI(outputType,cluster->eta(),cluster->phi());
        outputTE->setActiveState(true);
        attachFeature(outputTE,cluster,m_trigEmClusterKey);
        m_cachedTEList.insert(std::pair<unsigned int , HLT::TriggerElement*>
          (roiDescriptor->roiId(),outputTE));
      }
    }

    mm_nClusters.push_back(nClusters);
    mm_nOverlaps.push_back(nOverlaps);

  } // end loop over TEs

  std::vector<xAOD::TrigEMCluster*>::iterator
    m_outputClusterList_it = m_outputClusterList.begin();
  std::vector<xAOD::TrigEMCluster*>::iterator
    m_outputClusterList_end = m_outputClusterList.end();

  for(;
      m_outputClusterList_it != m_outputClusterList_end;
      m_outputClusterList_it++) {

    double eta = (*m_outputClusterList_it)->eta();
    double phi = (*m_outputClusterList_it)->phi();

    std::vector<xAOD::TrigEMCluster*>::iterator
      m_outputClusterList_itCmp = m_outputClusterList_it + 1;

    for(;
        m_outputClusterList_itCmp != m_outputClusterList_end;
        m_outputClusterList_itCmp++ ) {

      double dEta = (*m_outputClusterList_itCmp)->eta() - eta;
      double dPhi = (*m_outputClusterList_itCmp)->phi() - phi;
      double dR = sqrt(dEta*dEta + dPhi*dPhi);
      mm_L2dR.push_back(dR);

    }
  }

  // Stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;
}
