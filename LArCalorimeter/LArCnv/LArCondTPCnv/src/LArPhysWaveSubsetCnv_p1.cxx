/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArPhysWaveSubsetCnv_p1.h" 

void
LArPhysWaveSubsetCnv_p1::persToTrans(const LArPWPersType* persObj,  LArPWTransType* transObj, MsgStream & log){
	
	log<<MSG::DEBUG<<"LArPhysWaveSubsetCNV_p1  begin persToTrans"<<endreq;
		  
	unsigned int ncorrs	 = persObj->m_subset.m_corrChannels.size();	log<<MSG::DEBUG<<"Total corrections:"<<ncorrs;
	unsigned int nfebids = persObj->m_subset.m_febIds.size();		log<<MSG::DEBUG<<"\t\tTotal febs:"<<nfebids<<endreq;
	transObj->m_subset.resize(nfebids);

	unsigned int waveIndex	= 0;
	unsigned int chIndex	= 0;

	unsigned int febid        = 0; // Loop over febs
	unsigned int ifebWithData = 0; // counter for febs with data

	for (unsigned int i = 0; i < nfebids; ++i){	

		febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i]; // Set febid
		log<<MSG::DEBUG<<"feb index:"<<i<<"\tfebID: "<<febid;
		  
		transObj->m_subset[i].second.resize(NCHANNELPERFEB);// Fill channels with empty physvec's 

		bool hasSparseData  	 = false;
		unsigned int chansSet	 = 0;
		unsigned int chansOffset = 0;
		
		if (ifebWithData < persObj->m_subset.m_febsWithSparseData.size() && febid == persObj->m_subset.m_febsWithSparseData[ifebWithData]) { 
			hasSparseData = true;// Found feb with sparse data
			ifebWithData++;
			chansSet    = persObj->m_subset.m_febsWithSparseData[ifebWithData];
			chansOffset = 0;
			ifebWithData++;
      	}
      
		log<<MSG::DEBUG<<"\t\tsparse?  "<< hasSparseData <<endreq;
  
		for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){
			
			bool copyChannel = true;
			if (hasSparseData) {			
				if (!(chansSet & (1 << (j - chansOffset)))) {// Channel is missing data - skip
					copyChannel = false;	//	std::cout<<"0";
                }
				//else std::cout<<"1";
				
				if (j%32 == 31 && j < 126) {
					chansSet     = persObj->m_subset.m_febsWithSparseData[ifebWithData];
					chansOffset += 32;
					ifebWithData++;
//					std::cout<<" ";
	  			}
			}
			
			if (copyChannel) { // This channel has wave, resize vectors		
				LArPhysWave& PW =	transObj->m_subset[i].second[j];    
				double time   	=	persObj->m_dt[chIndex];
				unsigned int f  =	persObj->m_flag[chIndex];
				int timeOffset	=	persObj->m_timeOffset[chIndex];
				chIndex++;
				std::vector<double> val;
				std::vector<double> err;
				std::vector<int>  tri;
				for (unsigned int k = 0; k < persObj->m_samples; ++k){
					val.push_back(persObj->m_vAmplitudes[waveIndex]);
					err.push_back(persObj->m_vErrors[waveIndex]);
					tri.push_back(persObj->m_vTriggers[waveIndex]);
					waveIndex++;
				}		
	  
				LArPhysWave pw(val,err,tri, time,timeOffset, f);
				val.clear();
				err.clear();
				tri.clear();
					  
				PW=pw;
			}
		} // loop on channels
//		std::cout<<std::endl;
	} // loop on FEBs
	
	
    log<<MSG::DEBUG << "Corrections reading, ncorr=" << ncorrs << " waveIndex=" <<  waveIndex << " size=" << persObj->m_vAmplitudes.size() << endreq;
    transObj->m_correctionVec.resize(ncorrs);

    for (unsigned int i = 0; i < ncorrs; ++i){    // Loop over corrections
      
        if (chIndex    >= persObj->m_dt.size()) {	// check indexes
            log << MSG::ERROR << "LArPhysWaveSubsetCnv_p1::persToTrans - PhysWave index too large: WaveIndex/sizeInFile " 
                << chIndex << " " << persObj->m_dt.size()<<" "<< endreq;
            return;
        }
		
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
	
		LArPhysWave& PW = transObj->m_correctionVec[i].second;
		
		double time 	= persObj->m_dt[chIndex];
		unsigned int f  = persObj->m_flag[chIndex];
		int timeOffset	=	persObj->m_timeOffset[chIndex];
		chIndex++;
		std::vector<double> val;
		std::vector<double> err;
		std::vector<int> 	tri;
		for (unsigned int k = 0; k < persObj->m_samples; ++k){
			if (waveIndex>=persObj->m_vAmplitudes.size()) 
				log << MSG::ERROR << "Persistent LArPhysWave object is inconsistent. i=" << i << " WaveIndes=" << waveIndex << " size=" << persObj->m_vAmplitudes.size() << "samples=" << persObj->m_samples <<endreq;
			val.push_back(persObj->m_vAmplitudes[waveIndex]);
			err.push_back(persObj->m_vErrors[waveIndex]);
			tri.push_back(persObj->m_vTriggers[waveIndex]);
			waveIndex++;
		}
	
		LArPhysWave pw(val,err,tri, time,timeOffset, f);
		val.clear();
		err.clear();
		tri.clear();
        		
		PW=pw;
    }// over corrections

	
    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;
    log<< MSG::DEBUG <<"PhysWave  successfully read."<<endreq;
}





void
LArPhysWaveSubsetCnv_p1::transToPers(const LArPWTransType* transObj,  LArPWPersType* persObj, MsgStream & log){
    
  log<<MSG::DEBUG<<"LArPhysWaveSubsetCNV_p1  begin transToPers"<<endreq;
	
  // Copy conditions
  unsigned int nfebs	      = transObj->m_subset.size();			log<<MSG::DEBUG<<"total febs:"<<nfebs;
  unsigned int ncorrs	      = transObj->m_correctionVec.size();	log<<MSG::DEBUG<<"\t\ttotal corrections: "<<ncorrs<<endreq;

  unsigned int nsubsetsNotEmpty = 0;
  unsigned int nchans           = 0;

  persObj->m_samples=0; //for now
  bool haveSampleSize=false;

	std::vector<unsigned int> febsWithSparseData; // collects IDs of febs with sparse data
	for (unsigned int i = 0; i < nfebs; ++i){
		unsigned int nfebChans = transObj->m_subset[i].second.size();
		log<<MSG::DEBUG<<"feb index: "<<i<<" has "<<nfebChans<<" channels. ";

		if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
			log << MSG::ERROR << "LArPhysWaveSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans<< endreq;
			return;
		}
		
		if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty FEBS
				
		bool isSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) { // Loop over channels and check if this subset has sparse data
			
			const LArPhysWave& PW = transObj->m_subset[i].second[j];
			
            if (!isSparse && PW.isEmpty()) {
            	isSparse = true;
                febsWithSparseData.push_back(transObj->m_subset[i].first);// save febids for sparse subsets
				log<<MSG::DEBUG<<"\t\tfebID: "<<transObj->m_subset[i].first<<" is sparse"<<endreq;
            }
			
	    if (!PW.isEmpty()){
	      nchans++; // count number of channels
	      if (!haveSampleSize) {
		haveSampleSize=true;
		persObj->m_samples=PW.getSize();
	      }
	      else
		if (persObj->m_samples!=PW.getSize()) {
		  log << MSG::ERROR << "Waves of unequal length found in same conditions channel. Truncating to the shorter of " 
		      << persObj->m_samples << " and " << PW.getSize() << endreq;
		  persObj->m_samples=std::min(persObj->m_samples,(unsigned)PW.getSize());
		}
            } //else if !PS.isEmpty
	      
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
    persObj->m_timeOffset.reserve(nchans);
	
	
    int counterSparse=0;
    for (unsigned int i = 0; i < nfebs; ++i){ // new loop over febs

    	unsigned int nfebChans = transObj->m_subset[i].second.size();
		
		if (nfebChans == 0) continue; // skip subsets without any channels
        
        unsigned int febid = transObj->m_subset[i].first;
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
				
				if (!transObj->m_subset[i].second[j].isEmpty()) { // channel exists
                    
                    if (j < chansOffset || (j - chansOffset) > 31) // store the channel number in bit map
                        log << MSG::ERROR << "LArCWSubsetCnv_p1::tTP - incorrect ch. ind: j, chansOffset: " << j << " " << chansOffset << endreq;
                    
                    chansSet |= (1 << (j - chansOffset)); //store the channel number in the bit map
					
//					std::cout<<"1";	
                }
                else { // channel does not exist
                    saveAmplitudes = false;
//					std::cout<<"0";
                }
				
                // Save chansSet
                if  (j == (chansOffset + 31)) {
                    persObj->m_subset.m_febsWithSparseData.push_back(chansSet);
                    chansSet    =   0;
                    chansOffset += 32;
//					std::cout<<" ";
                }
            } // if sparse
			
			if (saveAmplitudes) {// save amplitudes, errors and triggers
			  const LArPhysWave& PW = transObj->m_subset[i].second[j];
			  persObj->m_dt.push_back(PW.getDt());
			  persObj->m_flag.push_back(PW.getFlag());
			  persObj->m_timeOffset.push_back(PW.getTimeOffset());
			  std::vector<double> w=PW.getWave();
			  std::vector<double> e=PW.getErrors();
			  std::vector<int> 	t=PW.getTriggers();
			  for (unsigned int k = 0; k< persObj->m_samples; ++k){
			    persObj->m_vAmplitudes.push_back(w[k]);
			    persObj->m_vErrors.push_back(e[k]);
			    persObj->m_vTriggers.push_back(t[k]);
			  }
			}
			
		} // over channels
//		std::cout<<std::endl;
	}// over febs
	
	// Copy corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
      	//  	log<<MSG::DEBUG<<"WRITING CORRECTION : "<<i<<endreq;
      	// Save channel id in febid vector
      	persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
      	
      	const LArPhysWave& PW = transObj->m_correctionVec[i].second;
      	std::vector<double> w=PW.getWave();
      	std::vector<double> e=PW.getErrors();
      	std::vector<int>  	t=PW.getTriggers();
      	persObj->m_dt.push_back(PW.getDt());
      	persObj->m_flag.push_back(PW.getFlag());
      	persObj->m_timeOffset.push_back(PW.getTimeOffset());
      	persObj->m_samples=w.size();
      	for (unsigned int k = 0; k< w.size(); ++k){
			 persObj->m_vAmplitudes.push_back(w[k]);
			 persObj->m_vErrors.push_back(e[k]);
			 persObj->m_vTriggers.push_back(t[k]);
      	}
    }

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    log<< MSG::DEBUG <<"PhysWave successfully written down."<<endreq;
	
}

