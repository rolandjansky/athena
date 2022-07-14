/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   ElectronMergerTool
//   Author: Jonathan Long, jonathan.long@cern.ch
//   This is a modified version of TrackParticleMerger (Bingxuan Liu, bingxuan.liu@cern.ch)
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Electron merger to be used downstream, mainly for 
//          combining LRT and standard electrons
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkEGamma/ElectronMergerTool.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/phihelper.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace DerivationFramework {

  ElectronMergerTool::ElectronMergerTool(const std::string& t,
      const std::string& n,
      const IInterface* p) :
      //base_class(t,n,p)
      AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    // The default goal of this merger is to create a collection combining standard and LRT electrons
  }

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////

  StatusCode ElectronMergerTool::initialize()
  {
    ATH_MSG_DEBUG("Initializing ElectronMergerTool");
    ATH_CHECK( m_electronLocation.initialize() );
    ATH_CHECK( m_outelectronLocation.initialize() );
    return StatusCode::SUCCESS;
  }

 
  StatusCode ElectronMergerTool::addBranches() const
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();

    std::unique_ptr<ConstDataVector<xAOD::ElectronContainer>> transientContainer = std::make_unique<ConstDataVector<xAOD::ElectronContainer>>(SG::VIEW_ELEMENTS);
    std::unique_ptr<xAOD::ElectronContainer> outputCol = std::make_unique<xAOD::ElectronContainer>();

    ATH_MSG_DEBUG("Electrons collection size " << m_electronLocation.size());
    
    std::unique_ptr<xAOD::ElectronAuxContainer> outputAuxCol;

    if(!m_createViewCollection) {
      outputAuxCol = std::make_unique<xAOD::ElectronAuxContainer>();
      outputCol->setStore(outputAuxCol.get());
    }   

    // pre-loop to reserve enough space in the output collection
    std::vector<const xAOD::ElectronContainer*> electronCollections;
    electronCollections.reserve(m_electronLocation.size());
    size_t ttNumber = 0;
    for (const auto & tcname : m_electronLocation){
      ///Retrieve electrons from StoreGate
      SG::ReadHandle<xAOD::ElectronContainer> electronCol (tcname, ctx);
      if (!electronCol.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve xAOD::ElectronContainer, \"" << tcname << "\", returning without running LRT merger!");
        return StatusCode::SUCCESS;
      }
      electronCollections.push_back(electronCol.cptr());
      ttNumber += electronCol->size();
    }
    outputCol->reserve(ttNumber);
    transientContainer->reserve(ttNumber);

    std::set<const xAOD::Electron*> LRTElectronsToRemove;
    checkOverlap(electronCollections, LRTElectronsToRemove);

    // merging loop over containers
    for(auto& tciter : electronCollections){
      // merge them in
      if (m_createViewCollection)  mergeElectron(tciter, transientContainer.get(), LRTElectronsToRemove);
      else                         mergeElectron(tciter, outputCol.get(), LRTElectronsToRemove);
    }

    SG::WriteHandle<xAOD::ElectronContainer> h_write(m_outelectronLocation, ctx);
    if(m_createViewCollection) {        
        //ATH_CHECK(h_write.record(std::move(transientContainer)));
        ATH_CHECK(evtStore()->record(transientContainer.release(), m_outelectronLocation.key()));
    } else {
        ATH_CHECK(h_write.record(std::move(outputCol), std::move(outputAuxCol)));
    }

    ATH_MSG_DEBUG("Done !");  
    return StatusCode::SUCCESS;
  }
  
  //////////////////////////////////////////////////////////////////////
  // Check overlap between the electron collections and decorate duplicates
  // TODO: This OR functionality should be merged into a common dual use tool called here and downstream
  //////////////////////////////////////////////////////////////////////
  void ElectronMergerTool::checkOverlap(const std::vector<const xAOD::ElectronContainer*>& electronCollections,
                                        std::set<const xAOD::Electron*> & LRTElectronsToRemove) const
  {
    // need to determine which is prompt and which is lrt
    const xAOD::ElectronContainer *promptElectronCol = electronCollections.at(0);
    const xAOD::ElectronContainer *LRTElectronCol = electronCollections.at(1);

    // loop over prompt electrons
    for (const xAOD::Electron *promptElectron : *promptElectronCol)
    {
      const ElementLink promptClusterLink = promptElectron->caloClusterLink(0);
      const xAOD::CaloCluster_v1 *prompt_cluster = (*promptClusterLink);

      // loop over lrt electrons
      for (const xAOD::Electron *LRTElectron : *LRTElectronCol)
      {
        const ElementLink LRTClusterLink = LRTElectron->caloClusterLink(0);
        const xAOD::CaloCluster_v1 *lrt_cluster = (*LRTClusterLink);

        // check that clusters exist (necessary? copied from MuonSpec overlap, but all electrons have clusters...)
        // TODO: This should then fall back to delta R if clusters are missing
        if (!lrt_cluster and !prompt_cluster) continue;

        // deltaR matching
        double dEta = lrt_cluster->calEta() - prompt_cluster->calEta();
        double dPhi = lrt_cluster->calPhi() - prompt_cluster->calPhi();

        dPhi = CxxUtils::wrapToPi(dPhi);

        if (sqrt(pow(dEta, 2) + pow(dPhi, 2)) < m_ORThreshold)
        {
          ATH_MSG_DEBUG("Found a Calo cluster shared by LRT electron and prompt electron !");
          // Save pointer to LRT electrons failing overlap
          LRTElectronsToRemove.insert(LRTElectron);
        }
      } // end lrt loop
    }  // end prompt loop
  }

  ///////////////////////////////////////////////////////////////////
  // Merge electron collections and remove duplicates
  ///////////////////////////////////////////////////////////////////

  void ElectronMergerTool::mergeElectron(const xAOD::ElectronContainer* electronCol,
                                         xAOD::ElectronContainer* outputCol,
                                         const std::set<const xAOD::Electron*>& LRTElectronsToRemove) const
  {
    // loop over electrons, accept them and add them into association tool
    if (electronCol && !electronCol->empty())
    {
      ATH_MSG_DEBUG("Size of output electron collection " << electronCol->size());
      // loop over electrons
      for (const auto *const rf : *electronCol)
      {
        // add electron into output and check if LRT electron failed overlap check
        if (m_doRemoval && LRTElectronsToRemove.find(rf) != LRTElectronsToRemove.end() ) continue;
        else 
        {
          std::unique_ptr<xAOD::Electron> newElectron = std::make_unique<xAOD::Electron>(*rf);

          outputCol->push_back( std::move(newElectron) );
        }
      }
      ATH_MSG_DEBUG("Size of merged output electron collection " << outputCol->size());
    }
  }

  ///////////////////////////////////////////////////////////////////
  // Merge electron collections and remove duplicates
  ///////////////////////////////////////////////////////////////////

  void ElectronMergerTool::mergeElectron(const xAOD::ElectronContainer* electronCol,
                                         ConstDataVector<xAOD::ElectronContainer>* outputCol,
                                         const std::set<const xAOD::Electron*>& LRTElectronsToRemove) const
  {
    // loop over electrons, accept them and add them into association tool
    if (electronCol && !electronCol->empty())
    {
      ATH_MSG_DEBUG("Size of transient electron collection " << electronCol->size());
      // loop over electrons
      for (const auto *const rf : *electronCol)
      {
        // add electron into output and check if LRT electron failed overlap check
        if (m_doRemoval && LRTElectronsToRemove.find(rf) != LRTElectronsToRemove.end() ) continue;
        else 
        {
          outputCol->push_back(rf);
        }
      }
      ATH_MSG_DEBUG("Size of transient merged electron collection " << outputCol->size());
    }
  }
}

