/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

*/

#include "RD53BEncoding/RD53BEncodingAlg.h"
#include "./RD53BEncodingTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetIdentifier/PixelID.h"

#include "TString.h"


RD53BEncodingAlg::RD53BEncodingAlg(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
  m_pixelManager(nullptr),
  m_pixIdHelper(nullptr),
  m_path("/ValidationPlots/"),
  m_thistSvc("THistSvc", name),
  m_use50x50(true),
  m_event(0)
  {
    declareProperty("RD53BEncodingTools",m_encodingTools);
    declareProperty("Use50x50",m_use50x50);
    declareProperty("Path", m_path);  
  }
  
StatusCode RD53BEncodingAlg::initialize() {  
  ATH_MSG_DEBUG( "Initializing RD53BEncodingAlg" );
  
  // TODO: TEMPORARELY
  // QUAD CHIP MODULES ARE SIMULATED AS BIG SINGLE-CHIP MODULES
  // THESE GLOBAL PARAMETERS SHOULD DISAPPEAR ONCE THE IMPLEMENTATION 
  // IS DONE PROPERLY, AS THE CHIP QUANTITIES ARE OBTAINED USING
  // design->numberOfCircuits(), design->rowsPerCircuit(), design->columnsPerCircuit()
    
  m_cols         = 400;
  m_rows         = 384;  
  m_cols_core    =   8;
  m_rows_core    =   2;
  m_colsbtwchips =   4;
  m_rowsbtwchips =   4;
  m_split_chip_eta = m_cols+m_colsbtwchips*0.5;
  m_split_chip_phi = m_rows+m_rowsbtwchips*0.5;
  
  if (not m_use50x50) {
    // define parameters for 25x100 pixel pitches
    m_cols         = 200;
    m_rows         = 768;  
    m_cols_core    =   4;
    m_rows_core    =   4;
    m_colsbtwchips =   2;
    m_rowsbtwchips =   8;
    m_split_chip_eta = m_cols+m_colsbtwchips*0.5;
    m_split_chip_phi = m_rows+m_rowsbtwchips*0.5;
  }
  
  // Grab Ntuple and histogramming service for tree
  CHECK(m_thistSvc.retrieve());
  
  // get the encoding tools
  CHECK( m_encodingTools.retrieve() );
  
  if (detStore()->retrieve(m_pixIdHelper, "PixelID").isFailure())
    ATH_MSG_ERROR("Could not get Pixel ID helper");
  
  if (detStore()->retrieve(m_pixelManager, "Pixel").isFailure())
    ATH_MSG_ERROR("Could not get Pixel Manager");
  
  // initialize stream maps
  CHECK( initializeStreams(m_encodingTools) );
  
  return bookHistograms();
  
}


StatusCode RD53BEncodingAlg::initializeStreams(const ToolHandleArray< RD53BEncodingTool >& encondingTools) {
  
  std::vector < std::vector < float > > barrel_module_z = {{0.}, {0.}, {0.}, {0.}, {0.}};
  std::vector < std::vector < float > > endcap_module_z = {{}, {}, {}, {}, {}};
    
  InDetDD::SiDetectorElementCollection::const_iterator element;
  for (element = m_pixelManager->getDetectorElementBegin(); element != m_pixelManager->getDetectorElementEnd(); ++element) {
    
    if (!(*element)) {
      ATH_MSG_ERROR("Problems with pointer to Detector Element!!!");
      return StatusCode::FAILURE;
    }
    
    // get the element indices      
    const Identifier Id((*element)->identify());
    int pixBrlEc(m_pixIdHelper->barrel_ec(Id));
    int pixLayerDisk(m_pixIdHelper->layer_disk(Id));
    int pixEtaMod(m_pixIdHelper->eta_module(Id));
    int pixPhiMod(m_pixIdHelper->phi_module(Id));
    
    // skip the elements you don't want to process
    if (pixBrlEc==0 and pixEtaMod<=0) continue;
    if (pixBrlEc<0) continue; 
    
    const InDetDD::PixelModuleDesign *design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&((*element)->design()));
    
    // TODO: TEMPORARELY
    // QUAD CHIP MODULES ARE SIMULATED AS BIG SINGLE-CHIP MODULES
    // THE RIGHT CALL IS
    // int chips = design->numberOfCircuits();
    // BUT IT IS CURRENTLY RETURNING 1 FOR ALL MODULES
    // THEREFORE WE CHECK THE NUMBERS OF ROWS
        
    int chips = (design->rowsPerCircuit() == m_rows) ? 1 : 4;
    
    // TODO: TO BE TEMPORARELY USED TO TRANSLATE THE INDICES
    // WAITING FOR SAMPLES WITH ATLAS-P2-ITK-22-02-00
    // WHEN UPDATING THIS, CHANGE THE ABOVE INDICES TO CONST
    if (pixBrlEc==2) {
      if (pixLayerDisk>29) {
        pixLayerDisk = pixLayerDisk-28;          
      }
      else if (pixLayerDisk>16) {
        pixEtaMod=pixLayerDisk;
        pixLayerDisk=1;
      } else {
        std::swap(pixLayerDisk, pixEtaMod);
      }
    }
    
    // use one module to save the z location:
    // using phi_module == 0 is an arbitrary choice
    if (pixPhiMod==0) {
      float module_z = (*element)->center().z();
      (*element)->isBarrel() ? barrel_module_z.at(pixLayerDisk).push_back(module_z) : endcap_module_z.at(pixLayerDisk).push_back(module_z);  
      ATH_MSG_DEBUG("--> MODULES: " << pixBrlEc << "/" << pixLayerDisk << "/" << pixEtaMod << "/" << pixPhiMod << " --> " << module_z);
    }
    
    for (const ToolHandle<RD53BEncodingTool>& encodingTool : encondingTools) {
      // for each module, set up the stream container 
      // it also depends on the number of chips you have per module
      if (encodingTool->addStream(Id, chips)==StatusCode::FAILURE)  {
        ATH_MSG_ERROR("A problem occurs when initialising streams in " << encodingTool->name());
        return StatusCode::FAILURE;
      }
    } 
    
  }
    
  for (const ToolHandle<RD53BEncodingTool>& encodingTool : encondingTools) {
    // when booking the histograms, you pass as well the module position
    // it is then stored in the tools and is used to bin the histograms accordingly
    if (encodingTool->bookHistograms(barrel_module_z, endcap_module_z)==StatusCode::FAILURE)  {
      ATH_MSG_ERROR("A problem occurs when booking histograms in " << encodingTool->name());
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
  
}

// TODO: TEMPORARELY
// QUAD CHIP MODULES ARE SIMULATED AS BIG SINGLE-CHIP MODULES
// THIS FINCTION NEED TO BE REMOVED WHEN A PROPER IMPLEMENTATION IS THERE
int RD53BEncodingAlg::findChip(int eta, int phi) {
  if (eta<m_split_chip_eta and phi<m_split_chip_phi)
    return 0;
  else if (eta>=m_split_chip_eta and phi<m_split_chip_phi)
    return 1;
  else if (eta<m_split_chip_eta and phi>=m_split_chip_phi)
    return 2;
  else 
    return 3;
  
  return 0;
}

StatusCode RD53BEncodingAlg::execute() {
  ATH_MSG_DEBUG(" In RD53BEncodingAlg::execute()" );
    
  for (const ToolHandle<RD53BEncodingTool>& encodingTool : m_encodingTools) {
    encodingTool->resetStreamsIfNeeded(m_event);    
  }
  
  fillChipMaps();  
  
  for (const ToolHandle<RD53BEncodingTool>& encodingTool : m_encodingTools) {
    encodingTool->fillDataRates(m_event);    
  }  
  
  m_event++;
  
  return StatusCode::SUCCESS;
}

StatusCode RD53BEncodingAlg::finalize() {
  ATH_MSG_DEBUG(" In RD53BEncodingAlg::finalize()" );
  return StatusCode::SUCCESS;
}

void RD53BEncodingAlg::fillChipMaps() {
  
  ATH_MSG_DEBUG("In RD53BEncodingAlg::fillChipMaps ... starting!" );
    
  const PixelRDO_Container* p_pixelRDO_cont;
  if (evtStore()->retrieve(p_pixelRDO_cont, "PixelRDOs") == StatusCode::SUCCESS) {
    
    InDetDD::SiDetectorElementCollection::const_iterator element;
    for (element = m_pixelManager->getDetectorElementBegin(); element != m_pixelManager->getDetectorElementEnd(); ++element) {
      
      if (!(*element)) continue;
      
      // get the IdHash of the detector element
      IdentifierHash IdHash = (*element)->identifyHash();

      // get the element indices      
      const Identifier rdoCollID((*element)->identify());
      int pixBrlEc(m_pixIdHelper->barrel_ec(rdoCollID));
      int pixLayerDisk(m_pixIdHelper->layer_disk(rdoCollID));
      int pixEtaMod(m_pixIdHelper->eta_module(rdoCollID));
      
      // skip the elements you don't want to process
      if (pixBrlEc==0 and pixEtaMod<=0) continue;
      if (pixBrlEc<0) continue; 
      
      // TODO: TO BE TEMPORARELY USED TO TRANSLATE THE INDICES
      // WAITING FOR SAMPLES WITH ATLAS-P2-ITK-22-02-00
      // WHEN UPDATING THIS, CHANGE THE ABOVE INDICES TO CONST
      if (pixBrlEc==2) {
        if (pixLayerDisk>29) {
          pixLayerDisk = pixLayerDisk-28;          
        }
        else if (pixLayerDisk>16) {
          pixEtaMod=pixLayerDisk;
          pixLayerDisk=1;
        } else {
          std::swap(pixLayerDisk, pixEtaMod);
        }
      }
      
      const InDetDD::PixelModuleDesign *design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&((*element)->design()));
      
      // TODO: TEMPORARELY
      // QUAD CHIP MODULES ARE SIMULATED AS BIG SINGLE-CHIP MODULES
      // THESE GLOBAL PARAMETERS SHOULD DISAPPEAR ONCE THE IMPLEMENTATION 
      // IS DONE PROPERLY, AS THE CHIP QUANTITIES ARE OBTAINED USING
      // design->numberOfCircuits(), design->rowsPerCircuit(), design->columnsPerCircuit()
      int chips = (design->rowsPerCircuit() == m_rows) ? 1 : 4;
      
      std::vector<ChipMap> chip_maps = std::vector<ChipMap>(chips, ChipMap(m_cols, m_rows, m_cols_core, m_rows_core, m_colsbtwchips, m_rowsbtwchips, m_use50x50));
            
      // get the RDO collection associated to the detector element
      PixelRDO_Container::const_iterator rdoCont_itr(p_pixelRDO_cont->indexFind(IdHash));
      
      //TESTING
      //       bool isTest = false;
      //       int pixPhiMod(m_pixIdHelper->phi_module(rdoCollID));
      //       if (pixBrlEc==0 and pixLayerDisk==4 and pixEtaMod==1 and pixPhiMod==0) isTest = true;      
      //       if (isTest and rdoCont_itr!=p_pixelRDO_cont->end()) {
      
      // if the collection is filled, fill the chip map, otherwise leave it empty       
      if (rdoCont_itr!=p_pixelRDO_cont->end()) {        
        
        PixelRDO_Collection::const_iterator rdo_itr((*rdoCont_itr)->begin());
        const PixelRDO_Collection::const_iterator rdo_end((*rdoCont_itr)->end());
        
        // get the region and the layer ltype for filling the histograms
        Region region = (*element)->isBarrel() ? BARREL : ENDCAP;
        
        LayerType ltype;
        if (m_pixIdHelper->is_innermost(rdoCollID))
          ltype = INNERMOST;
        else if (m_pixIdHelper->is_nexttoinnermost(rdoCollID))
          ltype = NEXT_TO_INNERMOST;
        else 
          ltype = OUTER;
        
        for ( ; rdo_itr != rdo_end; ++rdo_itr ) {          
          const Identifier rdoID((*rdo_itr)->identify());
          const int pixPhiIx(m_pixIdHelper->phi_index(rdoID));
          const int pixEtaIx(m_pixIdHelper->eta_index(rdoID));
          const int tot((*rdo_itr)->getToT());
             
          int chip = this->findChip(pixEtaIx, pixPhiIx);
          
          chip_maps.at(chip).fillChipMap(pixEtaIx, pixPhiIx, tot);
          
          // filling histograms only for the first events to keep them small
          if (m_event<10) {
            m_chips[region][ltype]->SetBinContent(pixEtaIx, pixPhiIx, chip+1);
            int core = chip_maps.at(chip).getCoreIndex(pixEtaIx, pixPhiIx);
            m_cores_on_chip[region][ltype]->SetBinContent(pixEtaIx, pixPhiIx, core);
          }
        }        
      }
      
      // you have filled the chip map for the given rdo collection, i.e. for the given chips 
      // you can now fill the stream corresponding to the corresponding chips
      for (const ToolHandle<RD53BEncodingTool>& encodingTool : m_encodingTools) {
        encodingTool->fillStreams(chip_maps, rdoCollID, m_event);
      }
    }
  }
          
  ATH_MSG_DEBUG("In RD53BEncodingAlg::fillChipMaps ... done!" );
}

StatusCode RD53BEncodingAlg::bookHistograms() {
  
  float min_eta = 0; float max_eta =  820; int bin_eta =  820;
  float min_phi = 0; float max_phi = 1600; int bin_phi = 1600;
  
  for (int region=0; region<N_REGIONS; region++) {
    for (int ltype=0; ltype<N_TYPES; ltype++) {
      m_chips[region][ltype] = new TH2F(("m_chips_"+m_layerTypeLabels[ltype]+"_"+m_regionLabels[region]).c_str(), (m_layerTypeLabels[ltype]+" Layer(s) - "+m_regionLabels[region]+"; Eta Index; Phi Index").c_str(), bin_eta, min_eta, max_eta, bin_phi, min_phi, max_phi);
      m_chips[region][ltype]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_chips[region][ltype]->GetName(), m_chips[region][ltype]));
      
      m_cores_on_chip[region][ltype] = new TH2F(("m_cores_on_chip_"+m_layerTypeLabels[ltype]+"_"+m_regionLabels[region]).c_str(), (m_layerTypeLabels[ltype]+" Layer(s) - "+m_regionLabels[region]+"; Eta Index; Phi Index").c_str(), bin_eta, min_eta, max_eta, bin_phi, min_phi, max_phi);
      m_cores_on_chip[region][ltype]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_cores_on_chip[region][ltype]->GetName(), m_cores_on_chip[region][ltype]));
    }
  }
  
  return StatusCode::SUCCESS;
  
}

  
