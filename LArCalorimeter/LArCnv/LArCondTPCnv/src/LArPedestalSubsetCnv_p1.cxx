/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"

void
LArPedestalSubsetCnv_p1::persToTrans(const LArPedestalPersType* persObj, 
                                  LArPedestalTransType* transObj, 
                                  MsgStream & log)
{
    // Copy conditions
    unsigned int ncorrs           = persObj->m_subset.m_corrChannels.size();
    unsigned int nfebids          = persObj->m_subset.m_febIds.size();
    unsigned int nPedestals       = persObj->m_vPedestalSize;
    unsigned int nPedestalRMSs    = persObj->m_vPedestalRMSSize;
    unsigned int pedestalIndex    = 0;
    unsigned int pedestalrmsIndex = 0;
    
    // resize subset to with then number of febids
    transObj->m_subset.resize(nfebids);

    // Loop over febs
    unsigned int febid        = 0;
    unsigned int ifebWithData = 0; // counter for febs with data

    for (unsigned int i = 0; i < nfebids; ++i){
        // Set febid
        febid = transObj->m_subset[i].first = persObj->m_subset.m_febIds[i];
        // Fill channels with empty pedestal vectors
        transObj->m_subset[i].second.resize(NCHANNELPERFEB);
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
        for (unsigned int j = 0; j < NCHANNELPERFEB; ++j){

            bool copyChannel = true;
            if (hasSparseData) {
                if (!(chansSet & (1 << (j - chansOffset)))) {
                    // Channel is missing data - skip
                    copyChannel = false;
                }
                if (j%32 == 31 && j < 126) {
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
                        << endreq;
                    return;
                }

                // This channel has pedestals, resize vectors
                //transObj->m_subset[i].second[j].m_vPedestal.resize(nPedestals);
                //transObj->m_subset[i].second[j].m_vPedestalRMS.resize(nPedestalRMSs);
		if (nPedestals)
		  transObj->m_subset[i].second[j].m_Pedestal = persObj->m_vPedestal[pedestalIndex];
		pedestalIndex+=nPedestals;

		if (nPedestalRMSs)
		  transObj->m_subset[i].second[j].m_PedestalRMS = persObj->m_vPedestalRMS[pedestalrmsIndex];
		pedestalrmsIndex+=nPedestalRMSs;
	    }//end if copychannel

        }//end loop over channels in feb
    }// end loop over febs
    
    // Copy corrections
    
    if (ncorrs) {
      transObj->m_correctionVec.resize(ncorrs);
      // Loop over corrections
      for (unsigned int i = 0; i < ncorrs; ++i){
        // check indexes
        if (pedestalIndex    >= persObj->m_vPedestal.size() ||
            pedestalrmsIndex >= persObj->m_vPedestalRMS.size()) {
            log << MSG::ERROR 
                << "LArPedestalSubsetCnv_p1::persToTrans - pedestal index too large: pedestal/size, pedestalrms/size " 
                << pedestalIndex << " " << persObj->m_vPedestal.size() << " " 
                << pedestalrmsIndex << " " << persObj->m_vPedestalRMS.size() 
                << endreq;
            return;
        }
        transObj->m_correctionVec[i].first = persObj->m_subset.m_corrChannels[i];
	
	if (nPedestals)
	  transObj->m_correctionVec[i].second.m_Pedestal = persObj->m_vPedestal[pedestalIndex];
	pedestalIndex+=nPedestals;


	if (nPedestalRMSs)
	  transObj->m_correctionVec[i].second.m_PedestalRMS = persObj->m_vPedestalRMS[pedestalrmsIndex];
	pedestalrmsIndex+=nPedestalRMSs;

      }// end loop over corrections

    }//end if ncorr

    // Copy the rest
    transObj->m_gain          = persObj->m_subset.m_gain; 
    transObj->m_channel       = persObj->m_subset.m_channel;
    transObj->m_groupingType  = persObj->m_subset.m_groupingType;

}


        
    
void
LArPedestalSubsetCnv_p1::transToPers(const LArPedestalTransType* /*transObj*/, 
				     LArPedestalPersType* /*persObj*/, 
				     MsgStream &log) 
{
    
  log << MSG::ERROR << "LArPedestalSubset_p1 is obsolete and can't be written out any more." << endreq;
  return;
}
 
