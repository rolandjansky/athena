/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibDelayMonAlg.h"

/*---------------------------------------------------------*/
LArCalibDelayMonAlg::LArCalibDelayMonAlg(const std::string& name,ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_onlineHelper(nullptr)
{}

/*---------------------------------------------------------*/
StatusCode LArCalibDelayMonAlg::initialize() {
  ATH_MSG_INFO( "Initialize LArCalibDelayMonAlg"  );

/*For pedestal run ONLY, not delay and ramp*/
  ATH_MSG_INFO( "m_accCalibDigitContainerKey.empty() " << m_accCalibDigitContainerKey.empty() );
  ATH_CHECK( m_accCalibDigitContainerKey.initialize(SG::AllowEmpty));
  if (m_accCalibDigitContainerKey.empty()) {
     ATH_MSG_FATAL("LArAccumulatedCalibDigitContainerKey must be set"); 
     return StatusCode::FAILURE;
  }
  
  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( m_hdrContKey.initialize() );
  
  m_histoGroups.reserve(m_SubDetNames.size());
  for (unsigned i=0; i<m_SubDetNames.size(); ++i) {
    std::vector<std::string> part;
    part.push_back(m_partitions[2*i]);
    part.push_back(m_partitions[2*i+1]);
    m_histoGroups.push_back(Monitored::buildToolMap<int>(m_tools,m_SubDetNames[i],part));
  }

  return AthMonitorAlgorithm::initialize();
}


/*---------------------------------------------------------*/
StatusCode LArCalibDelayMonAlg::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG( "in fillHists()"  );
  
  //get digit container
  SG::ReadHandle<LArAccumulatedCalibDigitContainer> pLArAccCalibDigitContainer;
  
  std::unordered_set<unsigned int> chanids;
  
  auto dac = Monitored::Scalar<int>("dac",0);  
  auto delay = Monitored::Scalar<int>("delay",0); 
  auto sample_max = Monitored::Scalar<double>("sample_max",0.);
   auto sample_max30 = Monitored::Scalar<double>("sample_max30",0.);
  auto sample_min = Monitored::Scalar<double>("sample_min",0.);
  auto chid = Monitored::Scalar<unsigned int>("chid",-1);
  auto slot = Monitored::Scalar<int>("slot",-1);
  auto ft = Monitored::Scalar<int>("FT",-1);
  auto slot30 = Monitored::Scalar<int>("slot30",-1);
  auto ft30 = Monitored::Scalar<int>("FT30",-1);
  auto febchid = Monitored::Scalar<int>("febchid",-1);
  
  if(!m_accCalibDigitContainerKey.empty()) {
      
    pLArAccCalibDigitContainer= SG::ReadHandle<LArAccumulatedCalibDigitContainer>{m_accCalibDigitContainerKey,ctx};
    if(pLArAccCalibDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArAccumulatedCalibDigitContainer with key "<< m_accCalibDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArAcumulatedCalibDigitContainer with key "<< m_accCalibDigitContainerKey.key());
    }  
    if(pLArAccCalibDigitContainer->empty()) return StatusCode::SUCCESS; // Nothing to fill
      
    /** Define iterators to loop over AccCalibDigits containers*/
    for (LArAccumulatedCalibDigitContainer::const_iterator ijDig = pLArAccCalibDigitContainer->begin(); 
         ijDig != pLArAccCalibDigitContainer->end(); ++ijDig) {
        HWIdentifier id = (*ijDig)->hardwareID();
        unsigned int ids = ((*ijDig)->hardwareID()).get_identifier32().get_compact();
        if(chanids.find(ids) == chanids.end()) chanids.emplace(ids);
        
        int channel = m_onlineHelper->channel(id);
        
        unsigned ntriggers = (*ijDig)->nTriggers();
        std::vector <double> sample;
        
        // transform sampleSum vector from uint32_t to double
        std::vector <double> samplesum;     

        for (std::vector < uint64_t >::const_iterator samplesum_it=(*ijDig)->sampleSum().begin();
             samplesum_it!=(*ijDig)->sampleSum().end(); ++samplesum_it) {
            samplesum.push_back((double)(*samplesum_it));
            // Get the vector of sample values
            sample.push_back((double)(*samplesum_it)/ntriggers);
        }
        
        // Get highest and lowest energy samples
        double samplemax = * std::max_element(sample.begin(), sample.end());
        double samplemin = * std::min_element(sample.begin(), sample.end());
        // Get dac and delay
        dac = (*ijDig)->DAC();
        delay = (*ijDig)->delay();
        
        // Then fill histo about max/min sample
        chid = ids;
        febchid = channel;
        sample_max = samplemax;
        sample_min = samplemin;
        fill(m_MonGroupName, sample_max, sample_min, slot, febchid, ft, dac, delay, chid);
        
        int Slot = m_onlineHelper->slot(id);
        int feedthrough = m_onlineHelper->feedthrough(id);
        int barrel_ec = m_onlineHelper->barrel_ec(id);
        int pos_neg   = m_onlineHelper->pos_neg(id);
        unsigned int partitionNb = returnPartition(barrel_ec,pos_neg,feedthrough,Slot);
        unsigned int subdet = partitionNb / 2;
        
        slot = Slot;
        ft = feedthrough;
        fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitionNb])],slot,ft,sample_max,sample_min);
        
        if (samplemax <= 3000.) {
            sample_max30 = samplemax;
            slot30 = Slot;
            ft30 = feedthrough;
            fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitionNb])],slot30,ft30,sample_max30);
        }
                
    }//end loop over pLArAccCalibDigitContainer    
     
  ATH_MSG_DEBUG("Filling nbChan: "<<chanids.size());
  auto nbchan = Monitored::Scalar<unsigned int>("nbChan",chanids.size());
  fill(m_MonGroupName,nbchan);
  }
        
  return StatusCode::SUCCESS;
}

unsigned int LArCalibDelayMonAlg::returnPartition(int be,int pn,int ft,int sl) const {
  // partitionNb = 0 : EMBC / 1 : EMBA / 2 : EMECC / 3 : EMECA / 4 : HECC / 5 : HECA / 6 : FCALC / 7 : FCALA
  unsigned int part = be*2+pn;
  if (be == 1){
    // This is a HEC FEB - Dirty method as IsHECOnlineFEBId is buggy!
    if ((ft == 3 || ft == 10 || ft == 16 || ft == 22) && (sl > 2)) part = be*2+pn + 2;
    if (ft == 6) part = be*2 + pn + 4; // This is FCAL FEB
  }        
  return part;
}
