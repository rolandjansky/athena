/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

*/

#include "./RD53BEncodingTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetIdentifier/PixelID.h"

#include "TString.h"
#include <bitset>
#include <chrono>

using xclock = std::chrono::steady_clock;

// Constructor with parameters:
RD53BEncodingTool::RD53BEncodingTool(const std::string& t,
                                     const std::string& n,
                                     const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixIdHelper(nullptr),
  m_addressCompression(true),
  m_bitTreeCompression(true),
  m_suppressToT(false),
  m_enableLinkSharing(false),
  m_auroraFactor(1.04),
  m_safetyFactor(1.25),
  m_eventsPerStream(1),
  m_testevent(0),
  m_path("/RD53BCompressed/"),
  m_thistSvc("THistSvc", n),
  m_testStream(false),
  m_doExpertPlots(false),
  m_debug(false)
{ 
  declareInterface<RD53BEncodingTool>(this);
  
  declareProperty("DoAddressCompression"  , m_addressCompression);
  declareProperty("DoBitTreeCompression"  , m_bitTreeCompression);
  declareProperty("DoToTSuppression"      , m_suppressToT       );
  declareProperty("EnableLinkSharing"     , m_enableLinkSharing );
  declareProperty("EventsPerStream"       , m_eventsPerStream   );
  declareProperty("AuroraFactor"          , m_auroraFactor      ); 
  declareProperty("SafetyFactor"          , m_safetyFactor      ); 
  declareProperty("Path"                  , m_path              );
  declareProperty("TestStreamCreation"    , m_testStream        );
  declareProperty("TestStreamFileName"    , m_testFileName    ="testStream");
  declareProperty("TestModuleBarrelEndcap", m_testBarrelEndcap=0);
  declareProperty("TestModuleLayerDisc"   , m_testLayerDisc   =4);
  declareProperty("TestModuleEta"         , m_testModuleEta   =1);
  declareProperty("TestModulePhi"         , m_testModulePhi   =0);
  declareProperty("Debug"                 , m_debug             );
  declareProperty("DoExpertPlots"         , m_doExpertPlots     );
}

StatusCode RD53BEncodingTool::finalize() {
  if(m_testStream) {
    m_testChipFile.close();
    m_testStreamFile.close();
  }
  return StatusCode::SUCCESS;
}

StatusCode RD53BEncodingTool::initialize() {
  ATH_MSG_DEBUG( "initialize() --> RD53BEncodingTool" );
  
  // Grab Ntuple and histogramming service for tree
  CHECK(m_thistSvc.retrieve());
  
  if (detStore()->retrieve(m_pixIdHelper, "PixelID").isFailure())
    ATH_MSG_ERROR("Could not get Pixel ID helper");
    
  if (m_testStream) {
    // human readable format
    m_testChipFile.open(m_testFileName+"_chip.txt");
    m_testChipFile << "MODULE = " << m_testBarrelEndcap << "/" << m_testLayerDisc << "/" << m_testModuleEta << "/" << m_testModulePhi << "\n";    
    m_testChipFile << "Chip Tag       Pixel Eta       Pixel Phi       cCol        qRow       tot       (bitmap size)     (tot size)      \n";
    
    // stream format
    m_testStreamFile.open(m_testFileName+"_stream.txt");
  }
    
  return StatusCode::SUCCESS;
}

StatusCode RD53BEncodingTool::addStream(Identifier id, int chips) {
  if (m_stream_map.addStream(id,chips))
    return StatusCode::SUCCESS;

  return StatusCode::FAILURE;
  
}

void RD53BEncodingTool::resetStreamsIfNeeded(int event) {
  if (m_eventsPerStream==1 or (m_eventsPerStream>1 and (event)%m_eventsPerStream==0)) {
    m_stream_map.reset();
    if (m_testStream) {
      m_testStreams.clear();
      m_testCores.clear();
    }
  }
}

void RD53BEncodingTool::fillDataRates(int event) {
  if (m_eventsPerStream==1 or (m_eventsPerStream>1 and (event+1)%m_eventsPerStream==0)) {
    fillDataRates();
  }
}

void RD53BEncodingTool::fillStreams(std::vector<ChipMap>& chip_maps, Identifier id, int event) {
  for (unsigned int chip = 0; chip < chip_maps.size(); chip++) {    
    fillStreams(chip_maps.at(chip), id, chip, event%m_eventsPerStream);
  }
}

void RD53BEncodingTool::fillStreams(ChipMap& chip_map, Identifier Id, int chip, int event) {
  int streams_per_event = 0;
  
  const int barrel_endcap(m_pixIdHelper->barrel_ec(Id));
  const int layer_disk(m_pixIdHelper->layer_disk(Id));
  const int eta_module(m_pixIdHelper->eta_module(Id));
  const int phi_module(m_pixIdHelper->phi_module(Id));
  
  bool doPrint = (m_debug and  barrel_endcap == m_testBarrelEndcap and layer_disk == m_testLayerDisc and eta_module == m_testModuleEta and phi_module==m_testModulePhi and chip==0);
  
  if (doPrint) {
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "barrel_endcap/layer_disk/eta/phi/chip = " << barrel_endcap << "/" << layer_disk << "/" << eta_module << "/" <<  phi_module << "/" << chip << std::endl;
  }
  
  createStream(chip_map, 
               m_stream_map.getStream(Id,chip).getStreams   (),
               m_stream_map.getStream(Id,chip).getTags      (),
               m_stream_map.getStream(Id,chip).getAddressing(),
               m_stream_map.getStream(Id,chip).getTot       (),
               m_stream_map.getStream(Id,chip).getData      (),
               m_stream_map.getStream(Id,chip).getOrphans   (),
               m_stream_map.getStream(Id,chip).getCores     (),
               streams_per_event, event, doPrint);
  
  if (m_doExpertPlots) {
    bool isBarrel = m_pixIdHelper->is_barrel(Id);
    Region region = isBarrel ? BARREL : ENDCAP;
    float z = m_module_z_layer[region].at(layer_disk).at(eta_module);
    fillHistograms(chip_map, streams_per_event, z, isBarrel, layer_disk);
  }
    
  if (doPrint) {
    std::cout << "------------------------------------------" << std::endl;
  }
  
  bool dumpStream = (m_testStream and  barrel_endcap == m_testBarrelEndcap and layer_disk == m_testLayerDisc and eta_module == m_testModuleEta and phi_module==m_testModulePhi and chip==0);
  if (dumpStream) {
    testStream(chip_map, m_testStreams, m_testCores, chip, event);    
  }
}



void RD53BEncodingTool::testStream(ChipMap& chipmap,
                                   std::vector <float>& streams, 
                                   std::vector <float>& cores,
                                   int& chipId, int event) {
  ATH_MSG_DEBUG("In RD53BEncodingTool::testStream ... starting");
  ATH_MSG_DEBUG("testing " << m_testBarrelEndcap << ", " << m_testLayerDisc << ", " << m_testModuleEta << ", " << m_testModulePhi);
  
  ATH_MSG_DEBUG("In RD53BEncodingTool::testStream ... starting!" );
  
  // defining the bit for the different fields
  // they are defined in the RD53B manual 
  // as well as the algorithm
  // https://cds.cern.ch/record/2665301
  
  // adding the bits for the chipId to the NS if link sharing is enabled (for multi-chip encoding)
  std::bitset<2> chipIdTag;
  chipIdTag |= chipId;
  std::string chipId_bits_string = m_enableLinkSharing ? "" : chipIdTag.to_string();
  const std::string NS_new       = "1"+chipId_bits_string;
  const std::string NS_old       = "0"+chipId_bits_string;
  const int NS                   =  (int)NS_new.size();
  
  const int TagX               =  8 ;
  const int TagY               =  11;  
  const int ccol_bits          =  6 ;
  const int crow_bits          =  8 ;
  const int islast_isneighbour =  2 ;
  
  // the tag is a simple counter from 0 to 207 incrementing every event and wrapping around
  const int chiptag = m_testevent-207*floor(m_testevent/207.);
  m_testevent++;
  
  std::bitset<8> TagX_bitset;
  TagX_bitset |= chiptag;      
  std::string TagX_bits = TagX_bitset.to_string();
  std::string TagY_bits = "111"+TagX_bits ;
  
  bool new_event = true;
  
  m_testChipFile<< "\nEvent " << std::to_string(m_testevent) << "\n";
  
  if (streams.size()==0) {
    streams.push_back(NS+TagX);
    m_testStreamFile << NS_new << TagX_bits;
    cores.push_back(0.);    
  } 
  
  if (not chipmap.isFilled())
    chipmap.fillCores();
  
  if (chipmap.getFiredCores()==0) {
    if (event==0) {
      ATH_MSG_DEBUG("In RD53BEncodingTool::testStream ... done!" );
      return;
    }
    
    float current_stream = streams.back();
    const int orphan_bits = 64*ceil(current_stream/64.) - current_stream;
    
    if (m_eventsPerStream==1 or (m_eventsPerStream>1 and event%m_eventsPerStream==0)) {
      streams.back()+=orphan_bits;
      streams.push_back(0.);
      cores.push_back(0.);
      streams.back()+=(NS+TagX);
      if (orphan_bits>0) {
        for (int orp=0; orp<orphan_bits; orp++)
          m_testStreamFile << "0";
      }
      m_testStreamFile << "\n";
      m_testStreamFile << NS_new << TagX_bits;
    } else {
      
      streams.back()+=TagY;
      
      if (TagY>orphan_bits) {
        streams.back()+=NS;
      }
      
      int current_length = 0;
      for (auto& bit : TagY_bits) {
        if ((current_length+1)%64==0) {
          m_testStreamFile << (NS_old+bit);
          current_length+=2;            
        } else {
          m_testStreamFile << bit;
          current_length+=1;
        }
      }
    }
    // once all this is done, you return 
    ATH_MSG_DEBUG("In RD53BEncodingTool::testStream ... done!" );
    return;
  }
  
  // if the chip map is not empty you need to loop on all the fired cores and store the information
  // about the ccol, qrow, bittree, tot 
  // This requires some gymnastic in order to correctly control the orphan bits and the NS bits
  const int all_cores = chipmap.getFiredCores();
  int processed_core = 0;
  for (int ccol = 0; ccol < chipmap.getCcols(); ccol++) {
    bool isfirst_ccol = true;
    int previous_row = -10; // use a default negative value
    int processed_row = 0;
    for (int crow = 0; crow < chipmap.getCrows(); crow++) {  
      
      // -- STEP 1) Construct the data (bitmap+tot)
      // 1) get the bit map length
      // 1.1) if needed, skip the cores without hits
      // 2) get the tot for the selected core
      
      // 1) get the bit map length
      std::string bitmap_bits = chipmap.getBitTreeString(ccol, crow, m_bitTreeCompression);
      // 1.1) if needed, skip the cores without hits
      if (bitmap_bits=="") continue;
      
      // increase the number of processed cores and rows
      processed_core++;
      processed_row++;
      
      // 2) get the tot for the selected core
      std::vector<int> tots;
      std::string tot_bits = chipmap.getToTBitsString(ccol, crow, tots);
      if (m_suppressToT)
        tot_bits = "";
      
      // get the values to print in the chip and stream files
      const int min_eta = ccol*chipmap.getCcolsCore(); 
      const int max_eta = (ccol+1)*chipmap.getCcolsCore()-1;
      const int min_phi = crow*chipmap.getCrowsCore(); 
      const int max_phi = (crow+1)*chipmap.getCrowsCore()-1;
      
      int el = 0;
      bool is_first = true;
      for (int eta=min_eta; eta<=max_eta; eta++) {
        for (int phi=min_phi; phi<=max_phi; phi++) {
          if (tots.at(el)==0) { el++; continue; }
          if (is_first) {
            m_testChipFile << chiptag << "     " << eta << "     " << 
                              phi << "     " << ccol+1 << "     " <<
                              crow << "     " << tots.at(el++) << "     (" <<
                              bitmap_bits.length() << ")    (" << tot_bits.length() << ")\n";
            is_first = false;          
          }
          else
            m_testChipFile << chiptag << "     " << eta << "     " << 
                              phi << "     " << ccol+1 << "     " << 
                              crow << "     " << tots.at(el++) << "\n";
        }
      }
      
            
      // -- STEP 2) Add the tags
      // now you need the ccol, crow tags + islast_isneighbour:
      // 1) add 2 bits for islast and isneighbour bits
      // 2) if it is the first ccol you need to add the ccol tag
      // 3) add the crow if the current crow is not neighbour of previous crow
      // 4) update the stream
      
      // let's start with a stream that contains the data: bitmap + tot
      float stream = bitmap_bits.length() + tot_bits.length();
            
      // 1) add 2 bits for islast and isneighbour bits
      float address = 0;
      std::string string_to_add = "";     
      
      if (m_addressCompression) {
        address += islast_isneighbour;       
        
        // 2) if it is the first ccol you need to add the ccol tag
        if (isfirst_ccol) {
          address += ccol_bits;
          isfirst_ccol = false;
          std::bitset<6> ccol_bits_string;
          ccol_bits_string |= ccol+1;      
          string_to_add += ccol_bits_string.to_string();
        }
        
        std::string islast_isneighbour_crow_bits = "";
        if (processed_row==chipmap.getCrowsPerCcol().at(ccol))
          islast_isneighbour_crow_bits += "1";
        else
          islast_isneighbour_crow_bits += "0";
        
        // 3) add the crow if the current crow is not neighbour of previous crow
        if (previous_row!=(crow-1)) {   
          address += crow_bits;
          islast_isneighbour_crow_bits += "0";
          std::bitset<8> crow_bits_string;
          crow_bits_string |= crow;
          islast_isneighbour_crow_bits += crow_bits_string.to_string();
        } else islast_isneighbour_crow_bits += "1";
        
        // update the previous_row to the current
        previous_row = crow;
        
        string_to_add+=islast_isneighbour_crow_bits;
              
      } else {
        address+=ccol_bits;
        std::bitset<6> ccol_bits_string;
        ccol_bits_string |= ccol+1;      
        string_to_add += ccol_bits_string.to_string();
        
        address+=crow_bits;
        std::bitset<8> crow_bits_string;
        crow_bits_string |= crow;
        string_to_add += crow_bits_string.to_string();
      }
        
      string_to_add+=(bitmap_bits+tot_bits);
      
      // 4) update the stream
      stream += address;
      
      // read the description above
      float current_stream = streams.back();
      const int orphan_bits = 64*ceil(current_stream/64.) - current_stream;
  
      // -- STEP 3) Build a new stream if required:
      // 1) add all the orphans to the stream
      // 2) reset the number of cores per stream
      // 3) build the stream to add NS+TagX+ccol+islast+isneighbour+crow[opt.]+bitmap+tot
      // 4) add the needed bits to the new stream taking care of how many NS you need

      if (event>0 and new_event and (m_eventsPerStream==1 or (m_eventsPerStream>1 and event%m_eventsPerStream==0))) {

        // 1) add all the orphans to the stream
        streams.back()+=orphan_bits;
        streams.push_back(0.);
        // 2) reset the number of cores per stream
        cores.push_back(1.);
        // 3) build the stream to add NS+TagX+ccol+islast+isneighbour+crow[opt.]+bitmap+tot        
        const float toadd = NS+TagX+stream;        
        // 4) add the needed bits to the new stream taking care of how many NS you need
        streams.back()+=toadd;
        
        // adding NS bits if new 64bit containers need to be added
        const int n_NS = ceil(toadd/64.)-1;
        if (n_NS>0) {
          streams.back()+=n_NS;
        }
        
        if (orphan_bits>0) {
          for (int orp=0; orp<orphan_bits; orp++)
            m_testStreamFile << "0";
        }
        m_testStreamFile << "\n";
        string_to_add = NS_new+TagX_bits+string_to_add;
        
        int current_length = 0;
        for (auto& bit : string_to_add) {
          if (current_length>0 and current_length%64==0) {
            m_testStreamFile << "\n";
            m_testStreamFile << (NS_old+bit);
            current_length+=2;            
          } else {
            m_testStreamFile << bit;
            current_length+=1;
          }
        }
        
        if (processed_core==all_cores and current_length%64==0)
          m_testStreamFile << "\n";     
        
      } else {   
        // -- STEP 4) No need for a new stream: add the constructed stream to the current one
        // 1) construct the stream
        // 2) add the tagY if a new event comes
        // 3) add the stream to the existing stream
        // 4) take care of how many NS bits you need to include
        // 5) increase the number of cores processed in this stream
        
        // 1) construct the stream
        float toadd = stream;
        // 2) add the tagY if a new event comes
        
        if (event>0 and new_event) {
          toadd+=TagY;          
          string_to_add=TagY_bits+string_to_add;
        }
        // 3) add the stream to the existing stream
        streams.back()+=toadd;
        // 4) take care of how many NS bits you need to include
        // if you need more than the available orphans you add a new 64bit container, hence NS
        // the additional containers can be more than 1
        if ((toadd-orphan_bits)>0) {
          const int n_NS = ceil((toadd-orphan_bits)/64.);
          streams.back()+=n_NS;
        }
        // 5) increase the number of cores processed in this stream
        cores.back()+=1.;
        int current_length = int(current_stream);
        for (auto& bit : string_to_add) {
          if (current_length>0 and current_length%64==0) {
            m_testStreamFile << "\n";
            m_testStreamFile << (NS_old+bit);      
            current_length+=2;            
          } else {
            m_testStreamFile << bit;
            current_length+=1;
          }
        }
        if (processed_core==all_cores and current_length%64==0)
          m_testStreamFile << "\n";
      }
      new_event = false;
    }
  }
  
  ATH_MSG_DEBUG("In RD53BEncodingTool::testStream ... done!" );
  return;
}

void RD53BEncodingTool::createStream(ChipMap& chipmap, 
                                     std::vector<float>& streams,
                                     std::vector<float>& tags,
                                     std::vector<float>& addressing,
                                     std::vector<float>& totbits,
                                     std::vector<float>& data,
                                     std::vector<float>& orphans,
                                     std::vector<float>& cores,
                                     int& streams_per_event,
                                     int event, bool test) {
  
  ATH_MSG_DEBUG("In RD53BEncodingTool::createStream ... starting!" );
  
  if (test)
    std::cout << "Event --> " << event << std::endl;
  
  // defining the bit for the different fields
  // they are defined in the RD53B manual 
  // as well as the algorithm
  // https://cds.cern.ch/record/2665301
  
  // adding the bits for the chipId to the NS if link sharing is enabled (for multi-chip encoding)
  const int NS                 = m_enableLinkSharing ? (1+2) : 1 ;
  const int TagX               = 8 ;
  const int TagY               = 11;
  const int ccol_bits          = 6 ;
  const int crow_bits          = 8 ;
  const int islast_isneighbour = 2 ;
  
  // this call happens for each event 
  // --> trigger this to add the required TagY
  bool new_event = true;
  
  // if the vectors are not yet filled, fill them with zeros
  if (streams.size()==0) {
    //start the stream with the NS+TagX bits
    streams.push_back(NS+TagX);
    tags.push_back(NS+TagX);
    addressing.push_back(0.);
    totbits.push_back(0.);
    data.push_back(0.);
    orphans.push_back(0.);
    cores.push_back(0.); 
    streams_per_event++;
  }
  
  if (not chipmap.isFilled())
    chipmap.fillCores();
  
  // first check if the chip is empty, i.e. no pixels have been fired;
  // in this case you need to add the TagY to identify the new event, if event>0
  if (chipmap.getFiredCores()==0) {
    if (test)
      std::cout << "EMPTY chip!" << std::endl;
    
    // if this happens at the first event, no action is needed 
    // since the NS+TagX bits are already added
    if (event==0) {
      ATH_MSG_DEBUG("In RD53BEncodingTool::createStream ... done!" );
      return;
    }
    
    // if you are processing other events, you need to add the TagX or TagY (the latter when adding the tag to the existing stream)
    // and identify the case if a new stream has to be added.
    // If so, you need to control the number of orphan bits and the NS bit.
    float current_stream = streams.back();
    // number of orphans at the end of the 64bits container
    const int orphan_bits = 64*ceil(current_stream/64.) - current_stream;
    
    // if you have to create of a new stream you need to add a new stream:
    // 1) add all the orphans to the stream
    // 2) reset the number of cores per stream 
    // 3) add the NS bit
    // 4) add the needed bits to the new stream
    // 5) reset the tags and update it
    if (m_eventsPerStream==1 or (m_eventsPerStream>1 and event%m_eventsPerStream==0)) {
      if (test) {
        std::cout << "Adding new stream..." << std::endl;
        std::cout << "current stream --> " << current_stream << std::endl;
        std::cout << "adding orphans --> " << orphan_bits << std::endl;
        std::cout << "adding tags    --> " << (NS+TagX) << std::endl;
      }        
      
      // 1) add all the orphans to the stream
      streams.back()+=orphan_bits;
      streams.push_back(0.);
      streams_per_event++;
      orphans.back()+=orphan_bits;
      orphans.push_back(0.);
      // 2) reset the number of cores per stream 
      cores.push_back(0.);
      // 3-4) add the NS bit and add the needed bits to the new stream (NS+TagX)
      streams.back()+=(NS+TagX);
      // 5) reset the tags and update it
      tags.push_back(NS+TagX);
      addressing.push_back(0.);
      data.push_back(0.);
      totbits.push_back(0.);
    } else {
      // if a new stream is not needed, add the new bits to the existing stream (TagY)
      streams.back()+=TagY;
      tags.back()+=TagY;
      if (test) {
        std::cout << "current stream --> " << current_stream << std::endl;
        std::cout << "adding tags    --> " << TagY << std::endl;
      }        
      // if you need more than the available orphans you add a new 64bit container, hence NS
      if (TagY>orphan_bits) {
        streams.back()+=NS;
        tags.back()+=NS;
        if (test)
          std::cout << "adding NS --> " << NS << std::endl;
      }
    }
    // once all this is done, you return 
    ATH_MSG_DEBUG("In RD53BEncodingTool::createStream ... done!" );
    return;
  }
  
  // if the chip map is not empty you need to loop on all the fired cores and store the information
  // about the ccol, qrow, bittree, tot 
  // This requires some gymnastic in order to correctly control the orphan bits and the NS bits
  for (int ccol = 0; ccol < chipmap.getCcols(); ccol++) {
    bool isfirst_ccol = true;
    int previous_row = -10; // use a default negative value
    for (int crow = 0; crow < chipmap.getCrows(); crow++) {
      int core = chipmap.getCore(ccol,crow);      
      
      // -- STEP 1) Construct the data (bitmap+tot)
      // 1) get the bit map length
      // 1.1) if needed, skip the cores without hits
      // 2) get the tot for the selected core
      
      // 1) get the bit map length
      const float bitmap = chipmap.getBitTree(ccol, crow, m_bitTreeCompression);
      // 1.1) if needed, skip the cores without hits
      if (bitmap==0.) continue;

      // 2) get the tot for the selected core
      const float tot = m_suppressToT ? 0. : chipmap.getToTBitsCore(core);
      // the stream is bitmap + tot
      float stream = bitmap + tot;
            
      // -- STEP 2) Add the tags
      // now you need the ccol, crow tags + islast_isneighbour:
      // 1) add 2 bits for islast and isneighbour bits
      // 2) if it is the first ccol you need to add the ccol tag
      // 3) add the crow if the current crow is not neighbour of previous crow
      // 4) update the stream
      
      // 1) add 2 bits for islast and isneighbour bits
      float address = 0;
      
      if (m_addressCompression) {
        address += islast_isneighbour;
       
        // 2) if it is the first ccol you need to add the ccol tag
        if (isfirst_ccol) {
          address += ccol_bits;
          isfirst_ccol = false;
        }
        // 3) add the crow if the current crow is not neighbour of previous crow
        if (previous_row!=(crow-1)) {   
          address += crow_bits;
        }
        // update the previous_row to the current
        previous_row = crow;
      
      } else {
        address += ccol_bits;
        address += crow_bits;
      }
      
      // 4) update the stream
      stream += address;
      
      // read the description above
      const float current_stream = streams.back();
      const int orphan_bits = 64*ceil(current_stream/64.) - current_stream;
  
      // -- STEP 3) Build a new stream if required:
      // 1) add all the orphans to the stream
      // 2) reset the number of cores per stream 
      // 3) build the stream to add NS+TagX+ccol+islast+isneighbour+crow[opt.]+bitmap+tot
      // 4) add the needed bits to the new stream taking care of how many NS you need
      
      if (event>0 and new_event and (m_eventsPerStream==1 or (m_eventsPerStream>1 and event%m_eventsPerStream==0))) {
        // 1) add all the orphans to the stream
        streams.back()+=orphan_bits;
        streams.push_back(0.);
        streams_per_event++;
        orphans.back()+=orphan_bits;
        orphans.push_back(0.);
        // 2) reset the number of cores per stream
        cores.push_back(1.);
        // 3) build the stream to add NS+TagX+ccol+islast+isneighbour+crow[opt.]+bitmap+tot        
        const float toadd = NS+TagX+stream;        
        // 4) add the needed bits to the new stream taking care of how many NS you need
        streams.back()+=toadd;
        // split stream by type
        tags.push_back(NS+TagX);
        addressing.push_back(address);
        data.push_back(bitmap);
        totbits.push_back(tot);
        
        // adding NS bits if new 64bit containers need to be added
        const int n_NS = ceil(toadd/64.)-1;
        if (n_NS>0) {
          streams.back()+=n_NS;
          tags.back()+=n_NS;
        }
        
        if (test) {
          std::cout << "Adding new stream..." << std::endl;
          std::cout << "current stream --> " << current_stream << std::endl;
          std::cout << "adding orphans --> " << orphan_bits << std::endl;
          std::cout << "New stream has " << std::endl;
          std::cout << "      tags    = " << NS+TagX << std::endl;
          std::cout << "      address = " << address << std::endl;
          std::cout << "      hitmap  = " << bitmap << std::endl;
          std::cout << "      tot     = " << tot << std::endl;
          std::cout << "              = " << toadd << std::endl;
          std::cout << "adding NSs    = " << n_NS << std::endl;
          std::cout << "adding total  ===> " << toadd+n_NS << std::endl;
        }
        
      } else {   
        // -- STEP 4) No need for a new stream: add the constructed stream to the current one
        // 1) construct the stream
        // 2) add the tagY if a new event comes
        // 3) add the stream to the existing stream
        // 4) take care of how many NS bits you need to include
        // 5) increase the number of cores processed in this stream
        if (test)
          std::cout << "Adding to existing stream... current size ===> " << streams.back() << std::endl;
        // 1) construct the stream
        float toadd = stream;
        // 2) add the tagY if a new event comes
        if (event>0 and new_event) {
          toadd+=TagY;
          tags.back()+=TagY;
          if(test) std::cout << "      tags    = " << TagY << std::endl;
        }
        if(test) {
          std::cout << "      address = " << address << std::endl;
          std::cout << "      hitmap  = " << bitmap << std::endl;
          std::cout << "      tot     = " << tot << std::endl;
          std::cout << "              = " << toadd << std::endl;
          std::cout << "      orphans = " << orphan_bits << std::endl;
          std::cout << "adding NSs    = " << ceil((toadd-orphan_bits)/64.) << std::endl;
          std::cout << "adding total  ===> " << toadd+ceil((toadd-orphan_bits)/64.) << std::endl;
        }
        // 3) add the stream to the existing stream
        streams.back()+=toadd;
        addressing.back()+=address;
        data.back()+=bitmap;
        totbits.back()+=tot;
        // 4) take care of how many NS bits you need to include
        // if you need more than the available orphans you add a new 64bit container, hence NS
        // the additional containers can be more than 1
        if ((toadd-orphan_bits)>0) {
          const int n_NS = ceil((toadd-orphan_bits)/64.);
          streams.back()+=n_NS;
          tags.back()+=n_NS;
        }
        // 5) increase the number of cores processed in this stream
        cores.back()+=1.;
        if(test) {
          std::cout << "new stream size  ===> " << streams.back() << std::endl;
        }
      }
      new_event = false;
    }
  }
  
  ATH_MSG_DEBUG("In RD53BEncodingTool::createStream ... done!" );
  return;
}

void RD53BEncodingTool::fillHistograms(ChipMap chipmap, int streams_per_event, float z, bool isBarrel, int layer) {
  const int nhits    = chipmap.getFiredPixels();
  const int channels = chipmap.getTotalChannels();
  const int ncores   = chipmap.getFiredCores();
  const std::vector<int> hits_per_core  = chipmap.getHitsPerCore();
  const std::vector<int> crows_per_ccol = chipmap.getCrowsPerCcol();
  
  const Region region = isBarrel ? BARREL : ENDCAP;
  
  // fill histograms and profiles
  // getting information on the number of hits (it might be 0)
  m_p_chip_occupancy[layer][region]->Fill(z,float(nhits)/float(channels)*100.);
  m_p_hits_per_chip[layer][region]->Fill(z,nhits);
  m_h2_hits_per_chip[layer][region]->Fill(z,nhits);
  
  // filling only if at least 1 core is fired
  if (ncores>0) {
    m_p_cores_per_chip[layer][region]->Fill(z,ncores);
    m_h2_cores_per_chip[layer][region]->Fill(z,ncores);
    
    // loop on cores and save the hits per core if fired
    for (auto& hits: hits_per_core)
      if (hits>0) m_p_hits_per_core[layer][region]->Fill(z,hits);
      
    // loop on ccol and save the number of qrows if fired
    for (auto& crows: crows_per_ccol)
      if (crows>0) m_h2_qrows_per_ccol[layer][region]->Fill(z,crows);

    // fill the bits per core for data+tot
    // loop on the ccol and crow to get the bit core and save them into the 2D histogram
    for (int ccol = 0; ccol < chipmap.getCcols(); ccol++) {
      int neig_crows = 0;
      int previous_row = -10; // use a default negative value
      for (int crow = 0; crow < chipmap.getCrows(); crow++) {
        const int core = chipmap.getCore(ccol,crow);      
        const float bitmap = chipmap.getBitTree(ccol, crow, m_bitTreeCompression);
        if (bitmap==0.) continue;
        const float tot = m_suppressToT ? 0. : chipmap.getToTBitsCore(core);
        const float data = bitmap+tot;
        m_h2_bittree_per_core[layer][region]->Fill(z,bitmap);
        m_h2_tot_per_core[layer][region]->Fill(z,tot);
        m_h2_data_per_core[layer][region]->Fill(z,data);
        if (previous_row==(crow-1))
          neig_crows++;            
        previous_row = crow;
      }
      // fill the number of neighbour crows per ccol
      m_h2_neig_qrows_per_ccol[layer][region]->Fill(z,neig_crows);
    }
  }
  // number of streams per event
  m_h2_streams_per_event[layer][region]->Fill(z,streams_per_event);
  m_p_streams_per_event[layer][region]->Fill(z,streams_per_event);
  
}

void RD53BEncodingTool::fillDataRates() {
  
  ATH_MSG_DEBUG("In "<< name() << "::fillDataRates ... starting!" );
  
  for (auto& id_streams: m_stream_map.getStreams()) {
    const Identifier& id = id_streams.first;
    std::vector<Stream>& chip_streams = id_streams.second;
    const int barrel_endcap = m_pixIdHelper->barrel_ec(id);
    const int eta_module = m_pixIdHelper->eta_module(id);
    const int layer_disk = m_pixIdHelper->layer_disk(id);
    const bool isBarrel = m_pixIdHelper->is_barrel(id);
    const Region region = isBarrel ? BARREL : ENDCAP;
    
    float z = m_module_z_layer[region].at(layer_disk).at(eta_module);
    for (auto& single_streams : chip_streams) {
      auto& streams   = single_streams.getStreams();
      auto& cores     = single_streams.getCores();
      auto& orphans   = single_streams.getOrphans();
      auto& addresses = single_streams.getAddressing();
      auto& tags      = single_streams.getTags();
      auto& tots      = single_streams.getTot();
      auto& bittrees  = single_streams.getData();
      
      const int phi_module = m_pixIdHelper->phi_module(id);
      const int chip = single_streams.chip();
      const bool doPrint = (m_debug and  barrel_endcap == m_testBarrelEndcap and layer_disk == m_testLayerDisc and eta_module == m_testModuleEta and phi_module==m_testModulePhi and chip==0);
      if (doPrint) {
        std::cout << "------------------------------------------" << std::endl;
        std::cout << "phi/chip = " << phi_module << "/" << chip << std::endl;
        std::cout << "Number of Streams = " << streams.size() << std::endl;
        std::cout << "------------------------------------------" << std::endl;
      }
      
      for (unsigned int stream = 0; stream < streams.size(); stream++) {
        float current_orphans = orphans.at(stream);
        float length    = streams.at(stream);
        if (int(streams.at(stream))%64>0) {
          const int orphan_bits = 64-int(streams.at(stream))%64;
          length          += float(orphan_bits);
          current_orphans += float(orphan_bits);
        }
        if (doPrint) {
          std::cout << "------------------------------------------" << std::endl;
          std::cout << "Stream length was (" << stream << ") = " << streams.at(stream) << std::endl;
          std::cout << "Orphan length was (" << stream << ") = " << orphans.at(stream) << std::endl;
          std::cout << "Stream length is  (" << stream << ") = " << length << std::endl;
          std::cout << "New Orphans       (" << stream << ") = " << current_orphans << std::endl;
          std::cout << "addresses         (" << stream << ") = " << addresses.at(stream) << std::endl;
          std::cout << "tags              (" << stream << ") = " << tags     .at(stream) << std::endl;
          std::cout << "tots              (" << stream << ") = " << tots     .at(stream) << std::endl;
          std::cout << "bittrees          (" << stream << ") = " << bittrees .at(stream) << std::endl;
          std::cout << "------------------------------------------" << std::endl;
        }
        
        m_p_streamlength_per_stream[layer_disk][region]->Fill(z,length);
        m_p_streamlength_per_stream_incl_prot[layer_disk][region]->Fill(z,length*m_auroraFactor);
        m_p_datarate[layer_disk][region]->Fill(z,length*m_auroraFactor/float(m_eventsPerStream));
        m_p_datarate_w_safety_factor[layer_disk][region]->Fill(z,length*m_auroraFactor/float(m_eventsPerStream)*m_safetyFactor);
        m_p_cores_per_stream[layer_disk][region]->Fill(z,cores.at(stream));
        m_p_perc_orphans_per_stream[layer_disk][region]->Fill(z,current_orphans/length*100.);
        m_p_perc_tot_per_stream[layer_disk][region]->Fill(z,tots.at(stream)/length*100.);
        m_p_perc_bittree_per_stream[layer_disk][region]->Fill(z,bittrees.at(stream)/length*100.);
        m_p_perc_tags_per_stream[layer_disk][region]->Fill(z,tags.at(stream)/length*100.);
        m_p_perc_addressing_per_stream[layer_disk][region]->Fill(z,addresses.at(stream)/length*100.);
        m_p_orphans_per_stream[layer_disk][region]->Fill(z,current_orphans);
        m_p_tot_per_stream[layer_disk][region]->Fill(z,tots.at(stream));
        m_p_bittree_per_stream[layer_disk][region]->Fill(z,bittrees.at(stream));
        m_p_tags_per_stream[layer_disk][region]->Fill(z,tags.at(stream));
        m_p_addressing_per_stream[layer_disk][region]->Fill(z,addresses.at(stream));            
        m_h2_streamlength_per_stream[layer_disk][region]->Fill(z,length);
        m_h2_streamlength_per_stream_incl_prot[layer_disk][region]->Fill(z,length*m_auroraFactor);
        m_h2_datarate[layer_disk][region]->Fill(z,length*m_auroraFactor/float(m_eventsPerStream));
        if (m_doExpertPlots) {
          m_h2_cores_per_stream[layer_disk][region]->Fill(z,cores.at(stream));
          m_h2_orphans_per_stream[layer_disk][region]->Fill(z,current_orphans);
          m_h2_tot_per_stream[layer_disk][region]->Fill(z,tots.at(stream));
          m_h2_bittree_per_stream[layer_disk][region]->Fill(z,bittrees.at(stream));
          m_h2_tags_per_stream[layer_disk][region]->Fill(z,tags.at(stream));
          m_h2_addressing_per_stream[layer_disk][region]->Fill(z,addresses.at(stream));
        }
      }
    }
  }
  
  if (m_testStream) {
    if (int(m_testStreams.back())%64>0) {
      const int orphan_bits = 64-int(m_testStreams.back())%64;
      for (int orp=0; orp<orphan_bits; orp++)
        m_testStreamFile << "0";
      m_testStreamFile << "\n";
      m_testStreams.back() += float(orphan_bits);
    }
  }
  
  ATH_MSG_DEBUG("In RD53BEncodingAlg::fillDataRates ... done!" );
}
  
StatusCode RD53BEncodingTool::bookHistograms(std::vector < std::vector < float > > barrel_z,
                                             std::vector < std::vector < float > > endcap_z) {
  
  // caching z values of barrel and endcap modules
  // accordingly to layer index and eta module index
  // they are used in the code to fill histograms
  m_module_z_layer[BARREL]=barrel_z;
  m_module_z_layer[ENDCAP]=endcap_z;
  
  std::vector < std::vector < double > > m_bins[N_REGIONS]; 
  
  // add initial values
  m_bins[BARREL]={{0.,}, {0.,}, {0.,}, {0.,}, {0.,}};
  m_bins[ENDCAP]={{200.,}, {200.,}, {1000.,}, {1000.,}, {1000.,}};
  
  // this triggers the new identifier schema for new geometries. 
  unsigned int maxLayersBarrel = 0;
  for (auto& module_positions : m_module_z_layer[BARREL])
    if (not module_positions.empty()) maxLayersBarrel++;    
  unsigned int maxLayersEndcap = 0;
  for (auto& module_positions : m_module_z_layer[ENDCAP])
    if (not module_positions.empty()) maxLayersEndcap++;    
    
  ATH_MSG_DEBUG("--> Using " << maxLayersBarrel << "/" << maxLayersEndcap << " layers...");
  
  m_bins[BARREL] = {{0.,}, {0.,}, {0.,}, {0.,}, {0.,}};
  
  if (maxLayersEndcap>m_bins[ENDCAP].size()) {
    ATH_MSG_DEBUG("--> Resetting min value for bins...");
    m_bins[ENDCAP]={{200.,}, {200.,}, {350.,}, {1000.,}, {350.,}, {1000.,}, {350.,}, {1000.,}};
  }  
  
  unsigned int maxLayers = std::max(maxLayersBarrel, maxLayersEndcap);
  
  for (unsigned int layer = 0; layer<maxLayers ; layer++) {
    if (layer<maxLayersBarrel) {
      ATH_MSG_DEBUG("--> BARREL: " << layer);
      // sorting the z values before calculating the bin ranges
      std::sort(barrel_z.at(layer).begin(), barrel_z.at(layer).end());
      // skipping first bin since it is 0. for the barrel
      for (unsigned int z_bin = 1; z_bin<(barrel_z.at(layer).size()-1); z_bin++) {
        m_bins[BARREL].at(layer).push_back(0.5*(barrel_z.at(layer).at(z_bin)+barrel_z.at(layer).at(z_bin+1)));
      }
      // add last values
      double last_value = 2.*barrel_z.at(layer).back()-barrel_z.at(layer).at(barrel_z.at(layer).size()-2);
      if (last_value<1500.)
        m_bins[BARREL].at(layer).push_back(last_value);
      m_bins[BARREL].at(layer).push_back(1500.);
    }
    
    if (layer<maxLayersEndcap) {
      ATH_MSG_DEBUG("--> ENDCAP: " << layer);
      // sorting the z values before calculating the bin ranges
      std::sort(endcap_z.at(layer).begin(), endcap_z.at(layer).end());    
      for (unsigned int z_bin = 0; z_bin<(endcap_z.at(layer).size()-1); z_bin++) {
        m_bins[ENDCAP].at(layer).push_back(0.5*(endcap_z.at(layer).at(z_bin)+endcap_z.at(layer).at(z_bin+1)));
      }
      // add last values
      double last_value = 2.*endcap_z.at(layer).back()-endcap_z.at(layer).at(endcap_z.at(layer).size()-2);
      if (last_value<3000.)
        m_bins[ENDCAP].at(layer).push_back(last_value);
      m_bins[ENDCAP].at(layer).push_back(3000.);
    }
  }
  
  for (int region=0; region<N_REGIONS; region++) {
    
    for (unsigned int layer=0; layer<maxLayers; layer++) {
      
      if (region==BARREL and layer>=maxLayersBarrel)
        continue;
            
      std::vector < double >& bins = m_bins[region].at(layer);
        
      m_p_streamlength_per_stream[layer][region] = new TProfile(("m_p_streamlength_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Stream Length - Layer "+std::to_string(layer)+"; z[mm]; <stream length> [bits]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_streamlength_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_streamlength_per_stream[layer][region]->GetName(), m_p_streamlength_per_stream[layer][region]));
      
      m_p_streamlength_per_stream_incl_prot[layer][region] = new TProfile(("m_p_streamlength_per_stream_incl_prot_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Stream Length + "+std::to_string(int((m_auroraFactor-1.)*100))+"% - Layer "+std::to_string(layer)+"; z[mm]; <stream length> [bits] + "+std::to_string(int((m_auroraFactor-1.)*100))+"%").c_str(), int(bins.size()-1), &bins[0]);
      m_p_streamlength_per_stream_incl_prot[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_streamlength_per_stream_incl_prot[layer][region]->GetName(), m_p_streamlength_per_stream_incl_prot[layer][region])); 
      
      m_p_datarate[layer][region] = new TProfile(("m_p_datarate_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Datarate (@ 1 MHz) - Layer "+std::to_string(layer)+"; z[mm]; <Datarate [Mb/s] (@ 1 MHz)>").c_str(), int(bins.size()-1), &bins[0]);
      m_p_datarate[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_datarate[layer][region]->GetName(), m_p_datarate[layer][region])); 
      
      m_p_datarate_w_safety_factor[layer][region] = new TProfile(("m_p_datarate_w_safety_factor_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Datarate (@ 1 MHz) + "+std::to_string(int((m_safetyFactor-1.)*100))+"% - Layer "+std::to_string(layer)+"; z[mm]; <Datarate [Mb/s] (@ 1 MHz)> + "+std::to_string(int((m_safetyFactor-1.)*100))+"%").c_str(), int(bins.size()-1), &bins[0]);
      m_p_datarate_w_safety_factor[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_datarate_w_safety_factor[layer][region]->GetName(), m_p_datarate_w_safety_factor[layer][region]));       
      
      m_p_cores_per_stream[layer][region] = new TProfile(("m_p_cores_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Cores - Layer "+std::to_string(layer)+"; z[mm]; <cores/stream>").c_str(), int(bins.size()-1), &bins[0]);
      m_p_cores_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_cores_per_stream[layer][region]->GetName(), m_p_cores_per_stream[layer][region])); 
      
      m_p_perc_orphans_per_stream[layer][region] = new TProfile(("m_p_perc_orphans_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Orphans Prec. - Layer "+std::to_string(layer)+"; z[mm]; <orphans/stream> [%]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_perc_orphans_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_perc_orphans_per_stream[layer][region]->GetName(), m_p_perc_orphans_per_stream[layer][region])); 
      
      m_p_perc_tot_per_stream[layer][region] = new TProfile(("m_p_perc_tot_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" ToT Prec. - Layer "+std::to_string(layer)+"; z[mm]; <ToT/stream> [%]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_perc_tot_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_perc_tot_per_stream[layer][region]->GetName(), m_p_perc_tot_per_stream[layer][region])); 
      
      m_p_perc_bittree_per_stream[layer][region] = new TProfile(("m_p_perc_bittree_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" BitTree Prec. - Layer "+std::to_string(layer)+"; z[mm]; <BitTree/stream> [%]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_perc_bittree_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_perc_bittree_per_stream[layer][region]->GetName(), m_p_perc_bittree_per_stream[layer][region])); 
      
      m_p_perc_tags_per_stream[layer][region] = new TProfile(("m_p_perc_tags_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Tags Prec. - Layer "+std::to_string(layer)+"; z[mm]; <Tags/stream> [%]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_perc_tags_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_perc_tags_per_stream[layer][region]->GetName(), m_p_perc_tags_per_stream[layer][region])); 
      
      m_p_perc_addressing_per_stream[layer][region] = new TProfile(("m_p_perc_addressing_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Addressing Prec. - Layer "+std::to_string(layer)+"; z[mm]; <Addressing/stream> [%]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_perc_addressing_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_perc_addressing_per_stream[layer][region]->GetName(), m_p_perc_addressing_per_stream[layer][region])); 
      
      m_p_orphans_per_stream[layer][region] = new TProfile(("m_p_orphans_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Orphans - Layer "+std::to_string(layer)+"; z[mm]; <orphans/stream> [bits]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_orphans_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_orphans_per_stream[layer][region]->GetName(), m_p_orphans_per_stream[layer][region])); 
      
      m_p_tot_per_stream[layer][region] = new TProfile(("m_p_tot_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" ToT - Layer "+std::to_string(layer)+"; z[mm]; <ToT/stream> [bits]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_tot_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_tot_per_stream[layer][region]->GetName(), m_p_tot_per_stream[layer][region])); 
      
      m_p_bittree_per_stream[layer][region] = new TProfile(("m_p_bittree_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" BitTree - Layer "+std::to_string(layer)+"; z[mm]; <BitTree/stream> [bits]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_bittree_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_bittree_per_stream[layer][region]->GetName(), m_p_bittree_per_stream[layer][region])); 
      
      m_p_tags_per_stream[layer][region] = new TProfile(("m_p_tags_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Tags - Layer "+std::to_string(layer)+"; z[mm]; <Tags/stream> [bits]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_tags_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_tags_per_stream[layer][region]->GetName(), m_p_tags_per_stream[layer][region])); 
      
      m_p_addressing_per_stream[layer][region] = new TProfile(("m_p_addressing_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Addressing - Layer "+std::to_string(layer)+"; z[mm]; <Addressing/stream> [bits]").c_str(), int(bins.size()-1), &bins[0]);
      m_p_addressing_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_p_addressing_per_stream[layer][region]->GetName(), m_p_addressing_per_stream[layer][region])); 
      
      m_h2_streamlength_per_stream[layer][region] = new TH2F(("m_h2_streamlength_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Stream Length - Layer "+std::to_string(layer)+"; z[mm]; stream length [bits]").c_str(), int(bins.size()-1), &(bins[0]), 10000*m_eventsPerStream, 0., 10000.*m_eventsPerStream);
      m_h2_streamlength_per_stream[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_h2_streamlength_per_stream[layer][region]->GetName(), m_h2_streamlength_per_stream[layer][region])); 
      
      m_h2_streamlength_per_stream_incl_prot[layer][region] = new TH2F(("m_h2_streamlength_per_stream_incl_prot_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Stream Length + "+std::to_string(int((m_auroraFactor-1.)*100))+"% - Layer "+std::to_string(layer)+"; z[mm]; stream length [bits] + "+std::to_string(int((m_auroraFactor-1.)*100))+"%").c_str(), int(bins.size()-1), &bins[0], 10000*m_eventsPerStream, 0., 10000.*m_eventsPerStream);
      m_h2_streamlength_per_stream_incl_prot[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_h2_streamlength_per_stream_incl_prot[layer][region]->GetName(), m_h2_streamlength_per_stream_incl_prot[layer][region])); 
      
      m_h2_datarate[layer][region] = new TH2F(("m_h2_datarate_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" Datarate (@ 1 MHz) - Layer "+std::to_string(layer)+"; z[mm]; Datarate [Mb/s] (@ 1 MHz)").c_str(), int(bins.size()-1), &bins[0], 10000, 0., 10000.);
      m_h2_datarate[layer][region]->StatOverflows();
      CHECK(m_thistSvc->regHist(m_path + m_h2_datarate[layer][region]->GetName(), m_h2_datarate[layer][region])); 
      
      if (m_doExpertPlots) {      
        m_p_chip_occupancy[layer][region] = new TProfile(("m_p_chip_occupancy_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" % occupancy - Layer "+std::to_string(layer)+"; z[mm]; % channel occupancy").c_str(), int(bins.size()-1), &bins[0]);
        m_p_chip_occupancy[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_p_chip_occupancy[layer][region]->GetName(), m_p_chip_occupancy[layer][region])); 
        
        m_p_hits_per_chip[layer][region] = new TProfile(("m_p_hits_per_chip_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" <hits/chip> - Layer "+std::to_string(layer)+"; z[mm]; <hits/chip>").c_str(), int(bins.size()-1), &bins[0]);
        m_p_hits_per_chip[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_p_hits_per_chip[layer][region]->GetName(), m_p_hits_per_chip[layer][region])); 
        
        m_p_cores_per_chip[layer][region] = new TProfile(("m_p_cores_per_chip_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" <cores/chip> - Layer "+std::to_string(layer)+"; z[mm]; <cores/chip>").c_str(), int(bins.size()-1), &bins[0]);
        m_p_cores_per_chip[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_p_cores_per_chip[layer][region]->GetName(), m_p_cores_per_chip[layer][region])); 
        
        m_p_hits_per_core[layer][region] = new TProfile(("m_p_hits_per_core_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" <hits/core> - Layer "+std::to_string(layer)+"; z[mm]; <hits/core>").c_str(), int(bins.size()-1), &bins[0]);
        m_p_hits_per_core[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_p_hits_per_core[layer][region]->GetName(), m_p_hits_per_core[layer][region])); 
        
        m_p_streams_per_event[layer][region] = new TProfile(("m_p_streams_per_event_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("streams per event - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; <# streams per event>").c_str(), int(bins.size()-1), &bins[0]);
        m_p_streams_per_event[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_p_streams_per_event[layer][region]->GetName(), m_p_streams_per_event[layer][region])); 
        
        m_h2_hits_per_chip[layer][region] = new TH2F(("m_h2_hits_per_chip_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), (m_regionLabels[region]+" hits/chip - Layer "+std::to_string(layer)+"; z[mm]; hits/chip").c_str(), int(bins.size()-1), &bins[0], 2000, 0., 2000.);
        m_h2_hits_per_chip[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_hits_per_chip[layer][region]->GetName(), m_h2_hits_per_chip[layer][region]));       
              
        m_h2_data_per_core[layer][region] = new TH2F(("m_h2_data_per_core_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("Bits per Core (BitTree+ToT) - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; bits/core; # entries").c_str(), int(bins.size()-1), &bins[0], 1000, 0., 1000.);
        m_h2_data_per_core[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_data_per_core[layer][region]->GetName(), m_h2_data_per_core[layer][region]));
        
        m_h2_bittree_per_core[layer][region] = new TH2F(("m_h2_bittree_per_core_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("Bits per Core (BitTree) - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; bits/core; # entries").c_str(), int(bins.size()-1), &bins[0], 1000, 0., 1000.);
        m_h2_bittree_per_core[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_bittree_per_core[layer][region]->GetName(), m_h2_bittree_per_core[layer][region]));
        
        m_h2_tot_per_core[layer][region] = new TH2F(("m_h2_tot_per_core_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("Bits per Core (ToT) - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; bits/core; # entries").c_str(), int(bins.size()-1), &bins[0], 1000, 0., 1000.);
        m_h2_tot_per_core[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_tot_per_core[layer][region]->GetName(), m_h2_tot_per_core[layer][region]));
        
        m_h2_cores_per_chip[layer][region] = new TH2F(("m_h2_cores_per_chip_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("Cores per Chip - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; cores/chip; # entries").c_str(), int(bins.size()-1), &bins[0], 1000, 0., 1000.);
        m_h2_cores_per_chip[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_cores_per_chip[layer][region]->GetName(), m_h2_cores_per_chip[layer][region]));
        
        m_h2_qrows_per_ccol[layer][region] = new TH2F(("m_h2_qrows_per_ccol_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("qrows per ccol - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; rows/column; # entries").c_str(), int(bins.size()-1), &bins[0], 150, 0., 150.);
        m_h2_qrows_per_ccol[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_qrows_per_ccol[layer][region]->GetName(), m_h2_qrows_per_ccol[layer][region]));
        
        m_h2_neig_qrows_per_ccol[layer][region] = new TH2F(("m_h2_neig_qrows_per_ccol_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("neighbour qrows per ccol - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; neighbour rows/column; # entries").c_str(), int(bins.size()-1), &bins[0], 150, 0., 150.);
        m_h2_neig_qrows_per_ccol[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_neig_qrows_per_ccol[layer][region]->GetName(), m_h2_neig_qrows_per_ccol[layer][region]));
        
        m_h2_streams_per_event[layer][region] = new TH2F(("m_h2_streams_per_event_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(), ("streams per event - "+m_regionLabels[region]+" - Layer "+std::to_string(layer)+"; z[mm]; # streams per event; # entries").c_str(), int(bins.size()-1), &bins[0], 150, 0., 150.);
        m_h2_streams_per_event[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_streams_per_event[layer][region]->GetName(), m_h2_streams_per_event[layer][region]));
                
        m_h2_cores_per_stream[layer][region] = new TH2F(("m_h2_cores_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(),(m_regionLabels[region]+" Cores - Layer "+std::to_string(layer)+"; z[mm]; cores/stream").c_str(), int(bins.size()-1), &bins[0], 1000*m_eventsPerStream, 0., 1000.*m_eventsPerStream);
        m_h2_cores_per_stream[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_cores_per_stream[layer][region]->GetName(), m_h2_cores_per_stream[layer][region])); 
           
        m_h2_orphans_per_stream[layer][region] = new TH2F(("m_h2_orphans_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(),(m_regionLabels[region]+" Orphans - Layer "+std::to_string(layer)+"; z[mm]; orphans/stream [bits]").c_str(), int(bins.size()-1), &bins[0], 100, 0., 100.);
        m_h2_orphans_per_stream[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_orphans_per_stream[layer][region]->GetName(), m_h2_orphans_per_stream[layer][region])); 
        
        m_h2_tot_per_stream[layer][region] = new TH2F(("m_h2_tot_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(),(m_regionLabels[region]+" ToT - Layer "+std::to_string(layer)+"; z[mm]; ToT/stream [bits]").c_str(), int(bins.size()-1), &bins[0], 10000*m_eventsPerStream, 0., 10000.*m_eventsPerStream);
        m_h2_tot_per_stream[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_tot_per_stream[layer][region]->GetName(), m_h2_tot_per_stream[layer][region])); 
            
        m_h2_bittree_per_stream[layer][region] = new TH2F(("m_h2_bittree_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(),(m_regionLabels[region]+" BitTree - Layer "+std::to_string(layer)+"; z[mm]; bitTree/stream [bits]").c_str(), int(bins.size()-1), &bins[0], 10000*m_eventsPerStream, 0., 10000.*m_eventsPerStream);
        m_h2_bittree_per_stream[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_bittree_per_stream[layer][region]->GetName(), m_h2_bittree_per_stream[layer][region])); 
            
        m_h2_tags_per_stream[layer][region] = new TH2F(("m_h2_tags_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(),(m_regionLabels[region]+" Tags - Layer "+std::to_string(layer)+"; z[mm]; tags/stream [bits]").c_str(), int(bins.size()-1), &bins[0], 10000*m_eventsPerStream, 0., 10000.*m_eventsPerStream);
        m_h2_tags_per_stream[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_tags_per_stream[layer][region]->GetName(), m_h2_tags_per_stream[layer][region])); 
            
        m_h2_addressing_per_stream[layer][region] = new TH2F(("m_h2_addressing_per_stream_"+m_regionLabels[region]+"_"+std::to_string(layer)).c_str(),(m_regionLabels[region]+" Addressing - Layer "+std::to_string(layer)+"; z[mm]; addressing/stream [bits]").c_str(), int(bins.size()-1), &bins[0], 10000*m_eventsPerStream, 0., 10000.*m_eventsPerStream);
        m_h2_addressing_per_stream[layer][region]->StatOverflows();
        CHECK(m_thistSvc->regHist(m_path + m_h2_addressing_per_stream[layer][region]->GetName(), m_h2_addressing_per_stream[layer][region])); 
        
      }
      
    }
    
  }
  
  return StatusCode::SUCCESS;
}
