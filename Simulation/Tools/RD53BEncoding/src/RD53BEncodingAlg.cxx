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
  m_event(0)
  {
    declareProperty("RD53BEncodingTools",m_encodingTools);
    declareProperty("Path", m_path);  
  }
  
StatusCode RD53BEncodingAlg::initialize() {  
  ATH_MSG_DEBUG( "Initializing RD53BEncodingAlg" );
  
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
  
  CHECK ( bookHistograms() );
  
  return StatusCode::SUCCESS;
  
}


StatusCode RD53BEncodingAlg::initializeStreams(const ToolHandleArray< RD53BEncodingTool >& encondingTools) {
  
  std::vector < std::vector < float > > barrel_module_z = {{0.}, {0.}, {0.}, {0.}, {0.}};
  std::vector < std::vector < float > > endcap_module_z = {{}, {}, {}, {}, {}, {}, {}, {}};
    
  InDetDD::SiDetectorElementCollection::const_iterator element;
  for (element = m_pixelManager->getDetectorElementBegin(); element != m_pixelManager->getDetectorElementEnd(); ++element) {
    
    if (!(*element)) {
      ATH_MSG_ERROR("Problems with pointer to Detector Element!!!");
      return StatusCode::FAILURE;
    }
    
    // get the element indices      
    const Identifier Id((*element)->identify());
    const int pixBrlEc(m_pixIdHelper->barrel_ec(Id));
    const int pixLayerDisk(m_pixIdHelper->layer_disk(Id));
    const int pixEtaMod(m_pixIdHelper->eta_module(Id));
    const int pixPhiMod(m_pixIdHelper->phi_module(Id));
    
    // skip the elements you don't want to process
    if (pixBrlEc==0 and pixEtaMod<0) continue;
    if (pixBrlEc<0) continue; 
    
    const InDetDD::PixelModuleDesign *design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&((*element)->design()));
    
    // get the number of chips per module
    int chips = design->numberOfCircuits();
    
    // use one module to save the z location:
    // using phi_module == 0 is an arbitrary choice
    if (pixPhiMod==0) {
      float module_z = (*element)->center().z();
      ATH_MSG_DEBUG("--> MODULES: " << pixBrlEc << "/" << pixLayerDisk << "/" << pixEtaMod << "/" << pixPhiMod << " --> " << module_z << "| chips: " << chips << " --> rows/cols: " << design->rowsPerCircuit() << "/" << design->columnsPerCircuit());
      (*element)->isBarrel() ? barrel_module_z.at(pixLayerDisk).push_back(module_z) : endcap_module_z.at(pixLayerDisk).push_back(module_z);  
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
      const int pixBrlEc(m_pixIdHelper->barrel_ec(rdoCollID));
      const int pixLayerDisk(m_pixIdHelper->layer_disk(rdoCollID));
      const int pixEtaMod(m_pixIdHelper->eta_module(rdoCollID));
      const int pixPhiMod(m_pixIdHelper->phi_module(rdoCollID));
      
      // skip the elements you don't want to process
      if (pixBrlEc==0 and pixEtaMod<0) continue;
      if (pixBrlEc<0) continue; 
      
      const InDetDD::PixelModuleDesign *design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&((*element)->design()));
      
      // get the module and chip definitions
      const int chips = design->numberOfCircuits();            
      int rowsPerChip = design->rowsPerCircuit();
      int columnsPerChip = design->columnsPerCircuit();
      const int chipsInPhi = design->rows()/rowsPerChip;
      const int chipsinEta = design->columns()/columnsPerChip;            
      
      const float phiPitch = design->phiPitch();
      bool use50x50 = true;
      if (phiPitch < s_pitch50x50)
        use50x50 = false;        
      
      // get the chip type
      ChipType ctype = (chips==1) ? SINGLE : QUAD;
      
      // get the region and the layer ltype for filling the histograms
      Region region = (*element)->isBarrel() ? BARREL : ENDCAP;
        
      LayerType ltype;
      if (m_pixIdHelper->is_innermost(rdoCollID))
        ltype = INNERMOST;
      else if (m_pixIdHelper->is_nexttoinnermost(rdoCollID))
        ltype = NEXT_TO_INNERMOST;
      else 
        ltype = OUTER;
      
      bool doSwapCoordinates = false;
      // takle the case where the chips are rotated. You need to swap the phi/eta indices for the pixel
      // since the front-end as well is rotated and the chip map has to get the right coordinates
      // It happens for all the single chip modules in the endcap:
      // - innermost layer
      // - shorties in the next-to-innermost layer          
      if (region==ENDCAP and chips==1) {
        doSwapCoordinates = true;
      }
      
      // swap dimensions if needed
      if (doSwapCoordinates) 
        std::swap(columnsPerChip, rowsPerChip);
      
      std::vector<ChipMap> chip_maps = std::vector<ChipMap>(chips, ChipMap(columnsPerChip, rowsPerChip, use50x50));
            
      // get the RDO collection associated to the detector element
      PixelRDO_Container::const_iterator rdoCont_itr(p_pixelRDO_cont->indexFind(IdHash));
      
      //TESTING
      //       bool isTest = false;
      //       int pixPhiMod(m_pixIdHelper->phi_module(rdoCollID));
      //       if (pixBrlEc==0 and pixLayerDisk==4 and pixEtaMod==1 and pixPhiMod==0) isTest = true;      
      //       if (isTest and rdoCont_itr!=p_pixelRDO_cont->end()) {
      
      // if the collection is filled, fill the chip map, otherwise leave it empty       
      if (rdoCont_itr!=p_pixelRDO_cont->end()) {        
        
        ATH_MSG_DEBUG("Element: " << pixBrlEc << "/" << pixLayerDisk << "/" << pixEtaMod <<"/" << pixPhiMod << " --> " << chips << "/" << columnsPerChip << "/" << rowsPerChip << " (" << design->columns() << "," << design->rows() << ")  -  (" << chipsinEta << "," << chipsInPhi << ")" << " - is50x50 = " << use50x50 << " --- " << design->phiPitch() << ", " << design->etaPitch());
        
        PixelRDO_Collection::const_iterator rdo_itr((*rdoCont_itr)->begin());
        const PixelRDO_Collection::const_iterator rdo_end((*rdoCont_itr)->end());
        
        for ( ; rdo_itr != rdo_end; ++rdo_itr ) {          
          const Identifier rdoID((*rdo_itr)->identify());
          int pixPhiIx(m_pixIdHelper->phi_index(rdoID));
          int pixEtaIx(m_pixIdHelper->eta_index(rdoID));
          const int tot((*rdo_itr)->getToT());
          
          // swap coordinates if needed
          if (doSwapCoordinates) {
            std::swap(pixEtaIx,pixPhiIx);
          }
          
          // evaluating the chip number considering the number of rows and columns per chip and
          // the total number of rows and columns on the sensor
          int chip = std::ceil(pixEtaIx/columnsPerChip) + chipsInPhi*std::ceil(pixPhiIx/rowsPerChip);
                    
          ATH_MSG_VERBOSE("            --> " << pixEtaIx << "(" << std::ceil(pixEtaIx/columnsPerChip) << ")/" << pixPhiIx << "(" << std::ceil(pixPhiIx/rowsPerChip) << ") - " << chip); 
          
          // get the eta/phi index wrt to the chip, not the module
          int pixEta = pixEtaIx - std::ceil(pixEtaIx/columnsPerChip)*columnsPerChip;
          int pixPhi = pixPhiIx - std::ceil(pixPhiIx/rowsPerChip)*rowsPerChip;          
                       
          chip_maps.at(chip).fillChipMap(pixEta, pixPhi, tot);
          
          // filling histograms only for the first events to keep them small
          if (m_event<10) {
            m_chips[region][ltype][ctype]->SetBinContent(pixEtaIx, pixPhiIx, chip+1);
            int core = chip_maps.at(chip).getCoreIndex(pixEta, pixPhi);
            m_cores_on_chip[region][ltype][ctype]->SetBinContent(pixEtaIx, pixPhiIx, core);
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
  
  std::vector< int > chipTypes;
  chipTypes.reserve(2);
  
  for (int region=0; region<N_REGIONS; region++) {
    for (int ltype=0; ltype<N_TYPES; ltype++) {
      
      chipTypes.clear();      
      // for the innermost layer you have always single modules, both in the barrel and in the endcap
      // for the next-to-innermost layer, you have quads in the barrel and quads and singles in the endcap
      // you only have quads for the outer system
      if (ltype == INNERMOST)
        chipTypes = {SINGLE};
      else if (ltype == NEXT_TO_INNERMOST and region == ENDCAP)
        chipTypes = {SINGLE, QUAD};
      else 
        chipTypes = {QUAD};
      
      for (auto& ctype : chipTypes) {
        m_chips[region][ltype][ctype] = new TH2F(("m_chips_"+m_layerTypeLabels[ltype]+"_"+m_regionLabels[region]+"_"+m_chipLabels[ctype]).c_str(), (m_layerTypeLabels[ltype]+" Layer(s) - "+m_regionLabels[region]+" - "+m_chipLabels[ctype]+"; Eta Index; Phi Index").c_str(), bin_eta, min_eta, max_eta, bin_phi, min_phi, max_phi);
        m_chips[region][ltype][ctype]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_chips[region][ltype][ctype]->GetName(), m_chips[region][ltype][ctype]));
        
        m_cores_on_chip[region][ltype][ctype] = new TH2F(("m_cores_on_chip_"+m_layerTypeLabels[ltype]+"_"+m_regionLabels[region]+"_"+m_chipLabels[ctype]).c_str(), (m_layerTypeLabels[ltype]+" Layer(s) - "+m_regionLabels[region]+" - "+m_chipLabels[ctype]+"; Eta Index; Phi Index").c_str(), bin_eta, min_eta, max_eta, bin_phi, min_phi, max_phi);
        m_cores_on_chip[region][ltype][ctype]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_cores_on_chip[region][ltype][ctype]->GetName(), m_cores_on_chip[region][ltype][ctype]));
      }
    }
  }
  return StatusCode::SUCCESS;
  
}

  
