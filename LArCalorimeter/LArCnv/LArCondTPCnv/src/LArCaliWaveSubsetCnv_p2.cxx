/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p2.h"
 

void
LArCaliWaveSubsetCnv_p2::persToTrans(const LArCaliWaveSubset_p2* persObj,  LArCWTransType* transObj, MsgStream & log){
  
  	log<<MSG::DEBUG<<"LArCaliWaveSubsetCNV_p2  begin persToTrans"<<endreq;
		  
    unsigned int nfebids = persObj->m_subset.m_febIds.size();			log<<MSG::DEBUG<<"Total febs:"<<nfebids;
    unsigned int ncorrs	 = persObj->m_subset.m_corrChannels.size();		log<<MSG::DEBUG<<"\tCorrections:"<<ncorrs<<endreq;

    transObj->m_subset.resize(nfebids);
  
    unsigned int cwvIndex=0, waveIndex=0, chIndex = 0;
   
    unsigned int febid        = 0; // Loop over febs
    unsigned int ifebWithData = 0; // counter for febs with data

    for (unsigned int i = 0; i < nfebids; ++i){
		
		febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i]; // Set febid
		log<<MSG::DEBUG<<"feb index: "<<i<<"\tfebID: "<<febid;
        
		transObj->m_subset[i].second.resize(NCHANNELPERFEB);// Fill channels with empty calowavevec's 
		bool hasSparseData  	 = false;
		unsigned int chansSet	 = 0;
		unsigned int chansOffset = 0;
		
		if (ifebWithData < persObj->m_subset.m_febsWithSparseData.size() && febid == persObj->m_subset.m_febsWithSparseData[ifebWithData]) { 
			hasSparseData = true;// Found feb with sparse data
			ifebWithData++;
			chansSet	= persObj->m_subset.m_febsWithSparseData[ifebWithData];
			chansOffset = 0;
			ifebWithData++;
		}
      
		log<<MSG::DEBUG<<"\tFeb sparse? "<< hasSparseData <<endreq;
  
		for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){
			bool copyChannel = true;
			if (hasSparseData) {  		  
				if (!(chansSet & (1 << (j - chansOffset)))) {	// Channel is missing data - skip
					copyChannel = false;						// std::cout<<"0";
					}											//	else std::cout<<"1";
		    			  
				if (j%32 == 31 && j < 126) {
					chansSet     = persObj->m_subset.m_febsWithSparseData[ifebWithData];
					chansOffset += 32;
					ifebWithData++;
					//				  std::cout<<" ";
					}
				}
		    		  
			if (copyChannel) { // This channel has wave, resize vectors	  
				LArCaliWaveVec& CWV = transObj->m_subset[i].second[j]; 
				   
				unsigned int nDAC=persObj->m_vDAC[cwvIndex];
				cwvIndex++;
				for (unsigned int nD=0;nD<nDAC;nD++){
					double time   	=persObj->m_dt[chIndex];
					unsigned int f  =persObj->m_flag[chIndex];
					int dac			=persObj->m_DAC[chIndex];
					chIndex++;
				
					std::vector<double> val, err;
					std::vector<int>  tri;
		
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
				}
			} // loop on channels
//		std::cout<<std::endl;
		} // loop on FEBs
	
	
    log<<MSG::DEBUG << "Correction reading, ncorr:" << ncorrs << "\twaveIndex=" <<  waveIndex << "\tsize:" << persObj->m_vAmplitudes.size()<<endreq;
    transObj->m_correctionVec.resize(ncorrs);
   
    for (unsigned int i = 0; i < ncorrs; ++i){ // Loop over corrections
      
        if (chIndex    >= persObj->m_dt.size()) {// check indexes
            log << MSG::ERROR 
                << "LArCaliWaveSubsetCnv_p2::persToTrans - CaliWave index too large: WaveIndex/sizeInFile " 
                << chIndex << " " << persObj->m_dt.size() << " " << endreq;
            return;
        }
		
		
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
	
		LArCaliWaveVec& CWV = transObj->m_correctionVec[i].second;
		
		unsigned int nDAC=persObj->m_vDAC[cwvIndex];
		cwvIndex++;
		for (unsigned int nD=0;nD<nDAC;nD++){	
		
			double time 	= persObj->m_dt[chIndex];
			unsigned int f  = persObj->m_flag[chIndex];
			int dac			= persObj->m_DAC[chIndex];
			chIndex++;
			std::vector<double> val, err;
			std::vector<int> 	tri;
			for (unsigned int k = 0; k < persObj->m_samples; ++k){
		  	
				if (waveIndex>=persObj->m_vAmplitudes.size()) 
			  	  log << MSG::ERROR << "Persistent LArCaliWave object is inconsistent. i=" << i << " WaveIndes=" 
				  << waveIndex << " size=" << persObj->m_vAmplitudes.size() << "samples=" << persObj->m_samples <<endreq;
			
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
			
    	}// over DAC values
	}//over corrections		
	
	
    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;
    log<< MSG::DEBUG <<"CaliWave ver p2 successfully read in."<<endreq;
}






void
LArCaliWaveSubsetCnv_p2::transToPers(const LArCWTransType* transObj,  LArCaliWaveSubset_p2* persObj, MsgStream & log){
    
	log<<MSG::DEBUG<<" LArCaliWaveSubsetCNV_p2  begin Writing"<<endreq;
	
	unsigned int nfebs  		= transObj->m_subset.size();		  log<<MSG::DEBUG<<"total febs:"<<nfebs;
	unsigned int ncorrs 		= transObj->m_correctionVec.size();   log<<MSG::DEBUG<<"\tCorrections: "<<ncorrs<<endreq;

	unsigned int nsubsetsNotEmpty = 0;
	unsigned int nchans 		  = 0;

	persObj->m_samples=0;

	std::vector<unsigned int> febsWithSparseData; // collects IDs of febs with sparse data
	
	for (unsigned int i = 0; i < nfebs; ++i){
		unsigned int nfebChans = transObj->m_subset[i].second.size();
		log<<MSG::DEBUG<<"feb index: "<<i<<" has "<<nfebChans<<" channels. ";

		if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
			log << MSG::ERROR << "LArCaliWaveSubsetCnv_p2::transToPers - found incorrect number of channels per feb: " << nfebChans<< endreq;
			return;
		}
		
		if (nfebChans) ++nsubsetsNotEmpty; // count number of non-empty FEBS
				
		bool isSparse = false;
        for (unsigned int j = 0; j < nfebChans; ++j) { // Loop over channels and check if this subset has sparse data
			
			const LArCaliWaveVec& CWV = transObj->m_subset[i].second[j];
			
            if (!isSparse && CWV.size() == 0) {
            	isSparse = true;
                febsWithSparseData.push_back(transObj->m_subset[i].first);// save febids for sparse subsets
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
				
				if (transObj->m_subset[i].second[j].size() > 0) { // channel exists
                    
                    if (j < chansOffset || (j - chansOffset) > 31) // store the channel number in bit map
                        log << MSG::ERROR << "LArCWSubsetCnv_p2::tTP - incorrect ch. ind: j, chansOffset: " << j << " " << chansOffset << endreq;
                    
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
			
			if (saveAmplitudes) {				// save amplitudes, errors and triggers
				const LArCaliWaveVec& CWV = transObj->m_subset[i].second[j];
				int dacValues=CWV.size();
				persObj->m_vDAC.push_back(dacValues);	//std::cout<<"CWV size: "<<dacValues<<std::endl;	
				
				for(int dv=0;dv<dacValues;dv++){
					persObj->m_dt.push_back(CWV[dv].getDt());
					persObj->m_flag.push_back(CWV[dv].getFlag());
					persObj->m_DAC.push_back(CWV[dv].getDAC() | (CWV[dv].getIsPulsedInt()<<16));
					std::vector<double> w=CWV[dv].getWave();
					std::vector<double> e=CWV[dv].getErrors();
					std::vector<int> 	t=CWV[dv].getTriggers();
                	for (unsigned int k = 0; k< w.size(); ++k){
						persObj->m_vAmplitudes.push_back(w[k]);
						persObj->m_vErrors.push_back(e[k]);
						persObj->m_vTriggers.push_back(t[k]);
                	} // over samples
				}// over DAC values
            }
			
		} // over channels
//		std::cout<<std::endl;
	}// over febs
	
	// Copy corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
      	log<<MSG::DEBUG<<"WRITTING CORRECTION: "<<i<<" ch id: "<<transObj->m_correctionVec[i].first;
      	
      	persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);// Save channel id in febid vector
    
      	const LArCaliWaveVec& CWV = transObj->m_correctionVec[i].second;	
		
		int dacValues=CWV.size();
		persObj->m_vDAC.push_back(dacValues);	 
		log<<MSG::DEBUG<<"\t CWV size: "<<dacValues;	
      	
		for(int dv=0;dv<dacValues;dv++){
      		persObj->m_dt.push_back(CWV[dv].getDt());
      		persObj->m_flag.push_back(CWV[dv].getFlag());
      		persObj->m_DAC.push_back(CWV[dv].getDAC() | (CWV[dv].getIsPulsedInt()<<16));
			std::vector<double> w=CWV[dv].getWave();
      		std::vector<double> e=CWV[dv].getErrors();
      		std::vector<int>  	t=CWV[dv].getTriggers();
      		persObj->m_samples=w.size();					//	std::cout<<" samples: "<<w.size()<<std::endl;
      		for (unsigned int k = 0; k< w.size(); ++k){
				persObj->m_vAmplitudes.push_back(w[k]);
			 	persObj->m_vErrors.push_back(e[k]);
			 	persObj->m_vTriggers.push_back(t[k]);
      		}// over samples
    	}// over dac values
	}// over corrections

    // Copy the rest
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    log<< MSG::DEBUG <<" Cali Wave p2 successfully written down."<<endreq;
}

