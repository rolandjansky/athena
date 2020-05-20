/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArConditionsSubset.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p1.h"
 

void
LArCaliWaveSubsetCnv_p1::persToTrans(const LArCWPersType* persObj,
                                     LArCWTransType* transObj,
                                     MsgStream & log)
{
  log<<MSG::DEBUG<<"  LArCaliWaveSubsetCNV_p1  begin persToTrans"<<endmsg;

  transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

  unsigned int nfebids = persObj->m_subset.m_febIds.size();
  log<<MSG::DEBUG<<"Total febs:"<<nfebids<<endmsg;
    
  unsigned int waveIndex    = 0;
  unsigned int chIndex    = 0;
   
  unsigned int ifebWithData = 0; // counter for febs with data

  auto subsetIt = transObj->subsetBegin();
  for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){	
    //std::cout<<"\nfeb index:"<<i;
  
    unsigned int febid = subsetIt->first;
    //std::cout<<"\tfebID: "<<febid<<std::endl;
        
    bool hasSparseData       = false;
    unsigned int chansSet    = 0;
    unsigned int chansOffset = 0;
    
    if (ifebWithData < persObj->m_subset.m_febsWithSparseData.size() && febid == persObj->m_subset.m_febsWithSparseData[ifebWithData]) { 
      hasSparseData = true;// Found feb with sparse data
      ifebWithData++;
      chansSet    = persObj->m_subset.m_febsWithSparseData[ifebWithData];
      chansOffset = 0;
      ifebWithData++;
    }
      
    //log<<MSG::DEBUG<<" Feb sparse? -> "<< hasSparseData <<endmsg;
  
    for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){
      bool copyChannel = true;
      if (hasSparseData) {			
        if (!(chansSet & (1 << (j - chansOffset)))) {
          copyChannel = false;// Channel is missing data - skip
          //					std::cout<<"0";
        }
        //else std::cout<<"1";
				
        if (j%32 == 31 && j < 126) {
          chansSet     = persObj->m_subset.m_febsWithSparseData[ifebWithData];
          chansOffset += 32;
	    ifebWithData++;
            //std::cout<<" ";
        }
      }
			
      if (copyChannel) { // This channel has wave, resize vectors		
        LArCaliWaveVec& CWV = subsetIt->second[j];	// what is this for?
        double time 		=persObj->m_dt[chIndex];
        unsigned int f  	=persObj->m_flag[chIndex];
        int dac			=persObj->m_DAC[chIndex];
        chIndex++;
        std::vector<double> val;
        std::vector<double> err;
        std::vector<int> 	tri;
        for (unsigned int k = 0; k < persObj->m_samples; ++k){
          val.push_back(persObj->m_vAmplitudes[waveIndex]);
          err.push_back(persObj->m_vErrors[waveIndex]);
          tri.push_back(persObj->m_vTriggers[waveIndex]);
          waveIndex++;
        }		
	  
        LArCaliWave cv(val,err,tri, time,dac, f);
        val.clear();
        err.clear();
        tri.clear();
        
        CWV.push_back(cv);
      }
    } // loop on channels
    //std::cout<<std::endl;
  } // loop on FEBs
	
  unsigned int ncorrs	 = persObj->m_subset.m_corrChannels.size();
  //log<<MSG::DEBUG << "entering corr reading, ncorr=" << ncorrs << " waveIndex=" <<  waveIndex << " size=" << persObj->m_vAmplitudes.size() << endmsg;
  LArCWTransType::CorrectionVec corrs;
  corrs.resize(ncorrs);

  // Loop over corrections
  for (unsigned int i = 0; i < ncorrs; ++i){
    // check indexes
    if (chIndex    >= persObj->m_dt.size()) {
      log << MSG::ERROR 
          << "LArCaliWaveSubsetCnv_p1::persToTrans - CaliWave index too large: WaveIndex/sizeInFile " 
          << chIndex << " " << persObj->m_dt.size() << " " 
          << endmsg;
      return;
    }
		
    corrs[i].first = persObj->m_subset.m_corrChannels[i];
	
    LArCaliWaveVec& CWV = corrs[i].second;
		
    double time 	= persObj->m_dt[chIndex];
    unsigned int f  = persObj->m_flag[chIndex];
    int dac		= persObj->m_DAC[chIndex];
    chIndex++;
    std::vector<double> val;
    std::vector<double> err;
    std::vector<int> 	tri;
    for (unsigned int k = 0; k < persObj->m_samples; ++k){
      if (waveIndex>=persObj->m_vAmplitudes.size()) 
        log << MSG::ERROR << "Persistent LArCaliWave object is inconsistent. i=" << i << " WaveIndes=" 
            << waveIndex << " size=" << persObj->m_vAmplitudes.size() << "samples=" << persObj->m_samples <<endmsg;
      val.push_back(persObj->m_vAmplitudes[waveIndex]);
      err.push_back(persObj->m_vErrors[waveIndex]);
      tri.push_back(persObj->m_vTriggers[waveIndex]);
      waveIndex++;
    }		
	
    LArCaliWave cv(val,err,tri, time,dac, f);
    val.clear();
    err.clear();
    tri.clear();
        		
    CWV.push_back(cv);
  }
  transObj->insertCorrections (std::move (corrs));
	
  // Copy the rest
  transObj->setChannel       (persObj->m_subset.m_channel);
  transObj->setGroupingType  (persObj->m_subset.m_groupingType);
  log<< MSG::DEBUG <<"CaliWave p1 done reading"<<endmsg;
}


void
LArCaliWaveSubsetCnv_p1::transToPers(const LArCWTransType* transObj,  LArCWPersType* persObj, MsgStream & log)
{
	log<<MSG::DEBUG<<"LArCaliWaveSubsetCNV_p1  begin transToPers"<<endmsg;

    // Copy conditions
//	log<<MSG::DEBUG<<"total febs:"<<nfebs<<endmsg;
  unsigned int ncorrs	      = transObj->correctionVecSize();
	//	log<<MSG::DEBUG<<"total corrections: "<<ncorrs<<endmsg;

  unsigned int nsubsetsNotEmpty = 0;
  unsigned int nchans           = 0;

  persObj->m_samples=0; //for now

  std::vector<unsigned int> febsWithSparseData; // collects IDs of febs with sparse data

  const auto subsetEnd = transObj->subsetEnd();
  for (auto subsetIt = transObj->subsetBegin();
       subsetIt != subsetEnd;
       ++subsetIt)
  {
    unsigned int nfebChans = subsetIt->second.size();
    //		std::cout<<"feb index: "<<i<<" has "<<nfebChans<<" channels. "<<std::endl;

    if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
      log << MSG::ERROR << "LArCaliWaveSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans<< endmsg;
      return;
    }
		
    if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty FEBS
				
    bool isSparse = false;
    for (unsigned int j = 0; j < nfebChans; ++j) { // Loop over channels and check if this subset has sparse data
      const LArCaliWaveVec& CWV = subsetIt->second[j];
			
      if (!isSparse && CWV.size() == 0) {
        isSparse = true;
        febsWithSparseData.push_back(subsetIt->first);// save febids for sparse subsets
        //				std::cout<<"febID: "<<transObj->m_subset[i].first<<" is sparse"<<std::endl;
      }
      if (CWV.size()){  // should be like this no?
			//else { 
        nchans++; // count number of channels
        //std::cout<<":"<<CWV[0].getSize()<<std::endl; always 768
        persObj->m_samples=CWV[0].getSize(); // completely ineficient but fast to fix :-(
      }
    }
			
//		std::cout<<"\nFeb: "<<i<<" nonemptychannels till now: "<<nchans<<std::endl;
	
  } // loop over febs
	
//	std::cout<<"total nonempty febs = "<<nsubsetsNotEmpty<<std::endl;
	
  persObj->m_subset.m_febIds.reserve(nsubsetsNotEmpty);
	
  persObj->m_subset.m_corrChannels.reserve(ncorrs);
	
  if (febsWithSparseData.size())
    persObj->m_subset.m_febsWithSparseData.reserve(febsWithSparseData.size()*5); // this stores FEBID + 128 bitmask of existing channels
    
  persObj->m_dt.reserve(nchans);
  persObj->m_vAmplitudes.reserve(nchans*persObj->m_samples);
  persObj->m_vErrors.reserve(nchans*persObj->m_samples);
  persObj->m_vTriggers.reserve(nchans*persObj->m_samples);
  persObj->m_flag.reserve(nchans);
	

  int counterSparse=0;
  for (auto subsetIt = transObj->subsetBegin();
       subsetIt != subsetEnd;
       ++subsetIt)
  {
    unsigned int nfebChans = subsetIt->second.size();
		
    if (nfebChans == 0) continue; // skip subsets without any channels
        
    unsigned int febid = subsetIt->first;
    persObj->m_subset.m_febIds.push_back(febid);   // stores FEBID's of nonempty FEBs
		
    bool isSparse=false; // this is just to avoid search loop
    if (counterSparse < (int)febsWithSparseData.size() && febid == febsWithSparseData[counterSparse]){
      counterSparse++;
      isSparse=true;
      persObj->m_subset.m_febsWithSparseData.push_back(febid); 
    }
			
    unsigned int chansSet    = 0;
    unsigned int chansOffset = 0;
    for (unsigned int j = 0; j < nfebChans; ++j){

      bool saveAmplitudes=true;
      if (isSparse) { // subset with sparse data
				
        if (subsetIt->second[j].size() > 0) { // channel exists
                    
          assert (j >= chansOffset && (j - chansOffset) <= 31);
          chansSet |= (1 << (j - chansOffset)); //store the channel number in the bit map
          
          //std::cout<<"1";	
        }
        else { // channel does not exist
          saveAmplitudes = false;
          //std::cout<<"0";
        }
				
        // Save chansSet
        if  (j == (chansOffset + 31)) {
          persObj->m_subset.m_febsWithSparseData.push_back(chansSet);
          chansSet    =   0;
          chansOffset += 32;
          //std::cout<<" ";
        }
      } // if sparse
			
      if (saveAmplitudes) {// save amplitudes, errors and triggers
        const LArCaliWaveVec& CWV = subsetIt->second[j];
        const std::vector<double>& w=CWV[0].getWave();
        const std::vector<double>& e=CWV[0].getErrors();
        const std::vector<int>& t=CWV[0].getTriggers();
        persObj->m_dt.push_back(CWV[0].getDt());
        persObj->m_flag.push_back(CWV[0].getFlag());
        persObj->m_DAC.push_back(CWV[0].getDAC() | (CWV[0].getIsPulsedInt()<<16));
        for (unsigned int k = 0; k< w.size(); ++k){
          persObj->m_vAmplitudes.push_back(w[k]);
          persObj->m_vErrors.push_back(e[k]);
          persObj->m_vTriggers.push_back(t[k]);
        }
      }
			
    } // over channels
    //std::cout<<std::endl;
  }// over febs

  // Copy corrections
  const auto corrEnd = transObj->correctionVecEnd();
  for (auto corrIt = transObj->correctionVecBegin();
       corrIt != corrEnd;
       ++corrIt)
  {
    //  	log<<MSG::DEBUG<<"WRITING CORRECTION : "<<i<<endmsg;
    // Save channel id in febid vector
    const LArCaliWaveVec& CWV = corrIt->second;
    if (CWV.empty()) continue;
    persObj->m_subset.m_corrChannels.push_back(corrIt->first);
    // Waves
    const std::vector<double>& w=CWV[0].getWave();
    const std::vector<double>& e=CWV[0].getErrors();
    const std::vector<int>&  t=CWV[0].getTriggers();
    persObj->m_dt.push_back(CWV[0].getDt());
    persObj->m_flag.push_back(CWV[0].getFlag());
    persObj->m_DAC.push_back(CWV[0].getDAC() | (CWV[0].getIsPulsedInt()<<16));
    persObj->m_samples=w.size();
    for (unsigned int k = 0; k< w.size(); ++k){
      persObj->m_vAmplitudes.push_back(w[k]);
      persObj->m_vErrors.push_back(e[k]);
      persObj->m_vTriggers.push_back(t[k]);
    }
  }

  // Copy the rest
  persObj->m_subset.m_gain          = transObj->gain(); 
  persObj->m_subset.m_channel       = transObj->channel();
  persObj->m_subset.m_groupingType  = transObj->groupingType();
  log<< MSG::DEBUG <<"CaliWave p1 written down."<<endmsg;
}

