/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArConditionsSubset.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"

void
LArPedestalSubsetCnv_p1::persToTrans(const LArPedestalPersType* persObj, 
                                  LArPedestalTransType* transObj, 
                                  MsgStream & log)
{
    transObj->initialize (persObj->m_subset.m_febIds, persObj->m_subset.m_gain);

    // Copy conditions
    unsigned int nfebids          = persObj->m_subset.m_febIds.size();
    unsigned int nPedestals       = persObj->m_vPedestalSize;
    unsigned int nPedestalRMSs    = persObj->m_vPedestalRMSSize;
    unsigned int pedestalIndex    = 0;
    unsigned int pedestalrmsIndex = 0;
    const unsigned int nChannelsPerFeb  = persObj->m_subset.subsetSize();    
    // Loop over febs
    unsigned int ifebWithData = 0; // counter for febs with data

    auto subsetIt = transObj->subsetBegin();
    for (unsigned int i = 0; i < nfebids; ++i, ++subsetIt){
        // Set febid
        unsigned int febid = subsetIt->first;
        bool hasSparseData       = false;
        unsigned int chansSet    = 0;
        unsigned int chansOffset = 0;
	if (ifebWithData < persObj->m_subset.m_febsWithSparseData.size() &&
            febid == persObj->m_subset.m_febsWithSparseData[ifebWithData]) {
            // Found feb with sparse data
            hasSparseData = true;
            ifebWithData++;
            chansSet    = persObj->m_subset.m_febsWithSparseData[ifebWithData];
            chansOffset = 0;
            ifebWithData++;
        }
            
        // Loop over channels in feb - only some channels are filled
        for (unsigned int j = 0; j < nChannelsPerFeb; ++j){

            bool copyChannel = true;
            if (hasSparseData) {
                if (!(chansSet & (1 << (j - chansOffset)))) {
                    // Channel is missing data - skip
                    copyChannel = false;
                }
                if (j%32 == 31 && j < nChannelsPerFeb-2) {
                    chansSet     = persObj->m_subset.m_febsWithSparseData[ifebWithData];
                    chansOffset += 32;
                    ifebWithData++;
                }
            }
            if (copyChannel) {
                        
                // Channel has pedestals - loop over pedestals per channel
                // and copy to the persistent object

                // check indexes
                if (pedestalIndex    >= persObj->m_vPedestal.size() ||
                    pedestalrmsIndex >= persObj->m_vPedestalRMS.size()) {
                    log << MSG::ERROR 
                        << "LArPedestalSubsetCnv_p1::persToTrans - pedestal index too large: pedestal/size, pedestalrms/size " 
                        << pedestalIndex << " " << persObj->m_vPedestal.size() << " " 
                        << pedestalrmsIndex << " " << persObj->m_vPedestalRMS.size() 
                        << endmsg;
                    return;
                }

                // This channel has pedestals, resize vectors
                //subsetIt->second[j].m_vPedestal.resize(nPedestals);
                //subsetIt->second[j].m_vPedestalRMS.resize(nPedestalRMSs);
		if (nPedestals)
		  subsetIt->second[j].m_Pedestal = persObj->m_vPedestal[pedestalIndex];
		pedestalIndex+=nPedestals;

		if (nPedestalRMSs)
		  subsetIt->second[j].m_PedestalRMS = persObj->m_vPedestalRMS[pedestalrmsIndex];
		pedestalrmsIndex+=nPedestalRMSs;
	    }//end if copychannel

        }//end loop over channels in feb
    }// end loop over febs
    
    // Copy corrections

    unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
    LArPedestalTransType::CorrectionVec corrs;
    corrs.resize (ncorrs);
    if (ncorrs) {
      // Loop over corrections
      for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
        if (pedestalIndex    >= persObj->m_vPedestal.size() ||
            pedestalrmsIndex >= persObj->m_vPedestalRMS.size()) {
            log << MSG::ERROR 
                << "LArPedestalSubsetCnv_p1::persToTrans - pedestal index too large: pedestal/size, pedestalrms/size " 
                << pedestalIndex << " " << persObj->m_vPedestal.size() << " " 
                << pedestalrmsIndex << " " << persObj->m_vPedestalRMS.size() 
                << endmsg;
            return;
        }
        corrs[i].first = persObj->m_subset.m_corrChannels[i];
	
	if (nPedestals)
	  corrs[i].second.m_Pedestal = persObj->m_vPedestal[pedestalIndex];
	pedestalIndex+=nPedestals;


	if (nPedestalRMSs)
	  corrs[i].second.m_PedestalRMS = persObj->m_vPedestalRMS[pedestalrmsIndex];
	pedestalrmsIndex+=nPedestalRMSs;

      }// end loop over corrections

    }//end if ncorr
    transObj->insertCorrections (std::move (corrs));

  // Copy the rest
  transObj->setChannel       (persObj->m_subset.m_channel);
  transObj->setGroupingType  (persObj->m_subset.m_groupingType);
}


        
    
void
LArPedestalSubsetCnv_p1::transToPers(const LArPedestalTransType* /*transObj*/, 
				     LArPedestalPersType* /*persObj*/, 
				     MsgStream &log) 
{
    
  log << MSG::ERROR << "LArPedestalSubset_p1 is obsolete and can't be written out any more." << endmsg;
  return;
}
 
