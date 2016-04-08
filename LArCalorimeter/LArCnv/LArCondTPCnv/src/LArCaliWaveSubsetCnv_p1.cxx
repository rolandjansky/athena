/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p1.h"
 

void
LArCaliWaveSubsetCnv_p1::persToTrans(const LArCWPersType* persObj,  LArCWTransType* transObj, MsgStream & log){
  
  	log<<MSG::DEBUG<<"  LArCaliWaveSubsetCNV_p1  begin persToTrans"<<endreq;
		  
    unsigned int ncorrs	 = persObj->m_subset.m_corrChannels.size();
    unsigned int nfebids = persObj->m_subset.m_febIds.size();
 	
	log<<MSG::DEBUG<<"Total febs:"<<nfebids<<endreq;
    
	transObj->m_subset.resize(nfebids);
  
    unsigned int waveIndex    = 0;
    unsigned int chIndex    = 0;
   
    unsigned int febid        = 0; // Loop over febs
    unsigned int ifebWithData = 0; // counter for febs with data

    for (unsigned int i = 0; i < nfebids; ++i){	
      //std::cout<<"\nfeb index:"<<i;
  
      febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i]; // Set febid
      //		std::cout<<"\tfebID: "<<febid<<std::endl;
        
      transObj->m_subset[i].second.resize(NCHANNELPERFEB);// Fill channels with empty calowavevec's 
//		std::cout<<"test"<<std::endl;
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
      
      //        log<<MSG::DEBUG<<" Feb sparse? -> "<< hasSparseData <<endreq;
  
      for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){
	bool copyChannel = true;
	if (hasSparseData) {			
	  if (!(chansSet & (1 << (j - chansOffset)))) {
	    copyChannel = false;// Channel is missing data - skip
	    //					std::cout<<"0";
                }
//				else std::cout<<"1";
				
	  if (j%32 == 31 && j < 126) {
	    chansSet     = persObj->m_subset.m_febsWithSparseData[ifebWithData];
	    chansOffset += 32;
	    ifebWithData++;
//					std::cout<<" ";
	  }
	}
			
	if (copyChannel) { // This channel has wave, resize vectors		
	  LArCaliWaveVec& CWV = transObj->m_subset[i].second[j];	// what is this for?
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
//		std::cout<<std::endl;
    } // loop on FEBs
	
	
    //log<<MSG::DEBUG << "entering corr reading, ncorr=" << ncorrs << " waveIndex=" <<  waveIndex << " size=" << persObj->m_vAmplitudes.size() << endreq;
    transObj->m_correctionVec.resize(ncorrs);
    // Loop over corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
      // check indexes
        if (chIndex    >= persObj->m_dt.size()) {
            log << MSG::ERROR 
                << "LArCaliWaveSubsetCnv_p1::persToTrans - CaliWave index too large: WaveIndex/sizeInFile " 
                << chIndex << " " << persObj->m_dt.size() << " " 
                << endreq;
            return;
        }
		
		
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
	
	LArCaliWaveVec& CWV = transObj->m_correctionVec[i].second;
		
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
    }

	
	
    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;
    log<< MSG::DEBUG <<"CaliWave p1 done reading"<<endreq;
}


void
LArCaliWaveSubsetCnv_p1::transToPers(const LArCWTransType* transObj,  LArCWPersType* persObj, MsgStream & log){
    
	log<<MSG::DEBUG<<"LArCaliWaveSubsetCNV_p1  begin transToPers"<<endreq;
	
    // Copy conditions
  unsigned int nfebs	      = transObj->m_subset.size();		
//	log<<MSG::DEBUG<<"total febs:"<<nfebs<<endreq;
  unsigned int ncorrs	      = transObj->m_correctionVec.size();
	//	log<<MSG::DEBUG<<"total corrections: "<<ncorrs<<endreq;

  unsigned int nsubsetsNotEmpty = 0;
  unsigned int nchans           = 0;

  persObj->m_samples=0; //for now

  std::vector<unsigned int> febsWithSparseData; // collects IDs of febs with sparse data
	for (unsigned int i = 0; i < nfebs; ++i){
		unsigned int nfebChans = transObj->m_subset[i].second.size();
//		std::cout<<"feb index: "<<i<<" has "<<nfebChans<<" channels. "<<std::endl;

		if (nfebChans != 0 && nfebChans != NCHANNELPERFEB) {
			log << MSG::ERROR << "LArCaliWaveSubsetCnv_p1::transToPers - found incorrect number of channels per feb: " << nfebChans<< endreq;
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
				const LArCaliWaveVec& CWV = transObj->m_subset[i].second[j];
				std::vector<double> w=CWV[0].getWave();
				std::vector<double> e=CWV[0].getErrors();
				std::vector<int> 	t=CWV[0].getTriggers();
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
//		std::cout<<std::endl;
	}// over febs
	
	// Copy corrections
    for (unsigned int i = 0; i < ncorrs; ++i){
      	//  	log<<MSG::DEBUG<<"WRITING CORRECTION : "<<i<<endreq;
      	// Save channel id in febid vector
      	persObj->m_subset.m_corrChannels.push_back(transObj->m_correctionVec[i].first);
      	// Waves
      	const LArCaliWaveVec& CWV = transObj->m_correctionVec[i].second;
      	std::vector<double> w=CWV[0].getWave();
      	std::vector<double> e=CWV[0].getErrors();
      	std::vector<int>  t=CWV[0].getTriggers();
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
    persObj->m_subset.m_gain          = transObj->m_gain; 
    persObj->m_subset.m_channel       = transObj->m_channel;
    persObj->m_subset.m_groupingType  = transObj->m_groupingType;
    log<< MSG::DEBUG <<"CaliWave p1 written down."<<endreq;
}

