/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterContainerCnv_p6.h" 

#include "AthAllocators/DataPool.h"
    
#include "AthenaPoolCnvSvc/Compressor.h"
#include "CaloEvent/CaloSamplingData.h"
#include "CaloConditions/CaloBadChannel.h"
//#include "CaloEvent/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloUtils/CaloClusterSignalState.h"
#include "AthenaKernel/errorcheck.h"


namespace {

inline
bool testbit (unsigned int x, unsigned int i)
{
  return (x & (1U << i)) != 0;
}

} // anonymous namespace


void CaloClusterContainerCnv_p6::persToTrans(const CaloClusterContainer_p6* pers, 
					     CaloClusterContainer* trans, MsgStream &log) {
  if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "Reading CaloClusterContainerCnv_p6" << endmsg;

  static CaloPhiRange range;
					     
  // reset element link converters, and provide container name lookup table
  m_showerElementLinkCnv.resetForCnv(pers->m_linkNames);
  m_cellElementLinkCnv.resetForCnv(pers->m_linkNames);

  // Use data pool for clusters to avoid calling constructor for each event
  DataPool<CaloCluster> clusters;

  trans->clear (SG::VIEW_ELEMENTS);
  trans->reserve(pers->m_vec.size());
  
  CaloClusterContainer_p6::const_iterator itp=pers->m_vec.begin();
  CaloClusterContainer_p6::const_iterator itp_e=pers->m_vec.end();
  
   std::vector<float> temp_Moments;
   Compressor A;
   A.expandToFloat( pers->m_momentContainer.m_Mvalue, temp_Moments); // unpacks moments
   
   std::vector<float>::const_iterator i_mom = temp_Moments.begin();
   std::vector<float>::const_iterator i_mom_e = temp_Moments.end();
   
   const std::vector<unsigned short>& keys = pers->m_momentContainer.m_Mkey;
   unsigned int nkeys = keys.size();
  
   unsigned int c1=0;

  std::vector<float> tmp_badChannelEta;
  std::vector<float> tmp_badChannelPhi;
  Compressor B;
  B.expandToFloat(pers->m_badEtaList,tmp_badChannelEta);
  B.expandToFloat(pers->m_badPhiList,tmp_badChannelPhi);

// raw 4-momentum state
  std::vector<float> tmp_rawE;
  std::vector<float> tmp_rawEtaPhiM;
  Compressor C;
  C.expandToFloat(pers->m_rawE,tmp_rawE);
  std::vector<float>::const_iterator iraw1 = tmp_rawE.begin();
  std::vector<float>::const_iterator iraw2 = tmp_rawE.end();

  Compressor D;
  D.expandToFloat(pers->m_rawEtaPhiM,tmp_rawEtaPhiM);
  std::vector<float>::const_iterator iraw3 = tmp_rawEtaPhiM.begin();
  std::vector<float>::const_iterator iraw4 = tmp_rawEtaPhiM.end();

// alternative calibration 4-momentum state
  std::vector<float> tmp_altE;
  std::vector<float> tmp_altEtaPhiM;
  Compressor E;
  E.expandToFloat(pers->m_altE,tmp_altE);
  std::vector<float>::const_iterator ialt1 = tmp_altE.begin();
  std::vector<float>::const_iterator ialt2 = tmp_altE.end();
  Compressor F;
  F.expandToFloat(pers->m_altEtaPhiM,tmp_altEtaPhiM);
  std::vector<float>::const_iterator ialt3 = tmp_altEtaPhiM.begin();
  std::vector<float>::const_iterator ialt4 = tmp_altEtaPhiM.end();


  std::vector<short>::const_iterator ibad1 = pers->m_badClusIndexList.begin();
  std::vector<short>::const_iterator ibad2 = pers->m_badClusIndexList.end();
  int nbad=0;

  bool mom_overrun_err = false;
  bool store_overrun_err = false;
  bool raw_overrun_err = false;
  size_t dataStore_size = pers->m_dataStore.size();

  bool fillBad=true;
  if (tmp_badChannelEta.size() != pers->m_badClusIndexList.size() ||
      tmp_badChannelPhi.size() != pers->m_badClusIndexList.size() ||
      pers->m_badLayerStatusList.size() != pers->m_badClusIndexList.size()) {
    log << MSG::WARNING << " problem to decode bad channel information, not filled..." << endmsg;
    fillBad=false;
  }
  
  short index=0;
  for(;itp!=itp_e;++itp) {
    index++;
    
    CaloCluster* transCluster = clusters.nextElementPtr();// Get next ptr for next cluster
    persToTrans(&(*itp),transCluster,log);  //Convert Cluster-quantities

    CaloSamplingData *trDS =&(transCluster->m_dataStore);
	
    trDS->m_varTypePattern = pers->m_varTypePattern;
    int nVar=trDS->getNumberOfVariableTypes();
    int nSamplings=trDS->getNumberOfSamplings();

    typedef CaloSamplingData::variable_key_type vartype;
    typedef CaloSamplingData::sampling_key_type samptype;
        
    for (int i=0;i<nVar;++i) {// here loops to read them in ...
      if (testbit (trDS->m_varTypePattern, i)) {

        // can be optimized: no decoding for each variable.
        for (int j=0;j<nSamplings; ++j){ 

          if ( testbit(transCluster->m_samplingPattern, j) ){
            float val;
            if (c1 < dataStore_size)
              val = pers->m_dataStore[c1++];
            else {
              val = 0;
              if (!store_overrun_err) {
                REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                            "CaloClusterContainerCnv_p6")
                  << "Corrupted data: data store array overrun "
                  << pers->m_vec.size() << " clusters "
                  << nkeys << " moment keys "
                  << temp_Moments.size() << " total moments.";
                store_overrun_err = true;
              }
            }
            trDS->storeData(static_cast<vartype>(i),
                            static_cast<samptype>(j),
                            val);
          }
          else{
            
            trDS->removeVariable(static_cast<vartype>(i),
                                 static_cast<samptype>(j));
            //std::cout<<"a:0"<<"\t";
          }
        }
        //			std::cout<<std::endl;	
      }
      else
        trDS->removeVariable(static_cast<vartype>(i));
    }
	
    transCluster->m_barrel= (bool)(0x001ff00f & transCluster->m_samplingPattern);
    transCluster->m_endcap= (bool)(0x00e00ff0 & transCluster->m_samplingPattern);
    //	std::cout<<" <<< Read"<<std::endl;
  
			
    //Convert moment store
    CaloClusterMomentStore::moment_store transStore;
    for (unsigned short i=0;i<nkeys;++i) {
      float val;
      if (i_mom == i_mom_e) {
        val = 0;
        if (!mom_overrun_err) {
          REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                      "CaloClusterContainerCnv_p6")
            << "Corrupted data: moment array overrun "
            << pers->m_vec.size() << " clusters "
            << nkeys << " moment keys "
            << temp_Moments.size() << " total moments.";
          mom_overrun_err = true;
        }
      }
      else {
        val = *i_mom;
        ++i_mom;
      }
      transStore.insert (transStore.end(),  CaloClusterMomentStore::moment_store::value_type( keys[i], val ) );
    }
    transCluster->m_momentStore.setMomentStore (std::move (transStore));
    
    // fill bad channel information
    transCluster->resetBadChannel();
    if (fillBad){
        while (ibad1 != ibad2 && (*ibad1) ==index) {
           float eta = tmp_badChannelEta[nbad] + transCluster->eta();
           float phi = range.fix(tmp_badChannelPhi[nbad] + transCluster->phi());
           short status=pers->m_badLayerStatusList[nbad];
           CaloSampling::CaloSample layer = (CaloSampling::CaloSample) (status & 0xff);
           CaloBadChannel flag = CaloBadChannel( ((status>>8) & 0xff) );
           CaloClusterBadChannelData data(eta,phi,layer,flag);
           // std::cout << " add bad channel data " << transCluster << " " << eta << " " << phi << " " << layer << " " << status << " " << flag.packedData() << std::endl;
           transCluster->addBadChannel(data); 
           ibad1++;
           nbad++;   
      }
    }

// fill raw E,eta,phi,M
    if (iraw1 != iraw2) {
      if (std::fabs(transCluster->e())>0.1) {
        transCluster->m_rawE = (*iraw1) * transCluster->e();
      }
      else
        transCluster->m_rawE = (*iraw1);
      iraw1++;
    }
    if (iraw3 != iraw4) {
      transCluster->m_rawEta = (*iraw3) + transCluster->eta();
      iraw3++;
    }
    if (iraw3 != iraw4) { 
      transCluster->m_rawPhi = range.fix((*iraw3)+transCluster->phi());
      iraw3++;
    }
    if (iraw3 == iraw4 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                  "CaloClusterContainerCnv_p6")
        << "Corrupted data: raw array overrun "
        << pers->m_vec.size() << " clusters "
        << tmp_rawE.size() << " raw values.";
      raw_overrun_err = true;
    }
    if (iraw3 != iraw4) {
      transCluster->m_rawM = (*iraw3);
      iraw3++;
    }
    //std::cout << " perstotrans rawE/eta/phi/M " << transCluster->m_rawE << " " << transCluster->m_rawEta << " " <<
    //  transCluster->m_rawPhi << " " << transCluster->m_rawM << std::endl;

// fill alt calibrated E,eta,phi,M
    if (ialt1 != ialt2) {
      if (std::fabs(transCluster->e())>0.1) {
        transCluster->m_altE = (*ialt1) * transCluster->e();
      }
      else
        transCluster->m_altE = (*ialt1);
      ialt1++;
    }
    if (ialt3 != ialt4) {
      transCluster->m_altEta = (*ialt3) + transCluster->eta();
      ialt3++;
    }
    if (ialt3 != ialt4) { 
      transCluster->m_altPhi = range.fix((*ialt3)+transCluster->phi());
      ialt3++;
    }
    if (ialt3 == ialt4 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,
                                  "CaloClusterContainerCnv_p6")
        << "Corrupted data: raw array overrun "
        << pers->m_vec.size() << " clusters "
        << tmp_altE.size() << " alt values.";
      raw_overrun_err = true;
    }
    if (ialt3 != ialt4) {
      transCluster->m_altM = (*ialt3);
      ialt3++;
    }
    //std::cout << " perstotrans altE/eta/phi/M " << transCluster->m_altE << " " << transCluster->m_altEta << " " <<
    //  transCluster->m_altPhi << " " << transCluster->m_altM << std::endl;


    trans->push_back(transCluster);
  }

  if (i_mom != i_mom_e && !mom_overrun_err) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloClusterContainerCnv_p6")
      << "Corrupted data: not all moments read "
      << pers->m_vec.size() << " clusters "
      << nkeys << " moment keys "
      << temp_Moments.size() << " total moments "
      << i_mom-temp_Moments.begin() << " read.";
  }

  if (c1 != dataStore_size && !store_overrun_err) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloClusterContainerCnv_p6")
      << "Corrupted data: not all data store members  read "
      << dataStore_size << " elements "
      << c1 << " read.";
  }

  if (iraw1 != iraw2 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                  "CaloClusterContainerCnv_p6")
        << "Corrupted data: not all raw values read "
        << pers->m_vec.size() << " clusters "
        << tmp_rawE.size() << " raw values "
        << iraw1-tmp_rawE.begin() << " read.";
  }

  if (iraw3 != iraw4 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,
                                  "CaloClusterContainerCnv_p6")
        << "Corrupted data: not all raw values read "
        << pers->m_vec.size() << " clusters "
        << tmp_rawEtaPhiM.size() << " raw values "
        << iraw3-tmp_rawEtaPhiM.begin() << " read.";
  }

  if (ialt1 != ialt2 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,
                                  "CaloClusterContainerCnv_p6")
        << "Corrupted data: not all alt values read "
        << pers->m_vec.size() << " clusters "
        << tmp_altE.size() << " alt values "
        << ialt1-tmp_altE.begin() << " read.";
  }

  if (ialt3 != ialt4 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,
                                  "CaloClusterContainerCnv_p6")
        << "Corrupted data: not all alt values read "
        << pers->m_vec.size() << " clusters "
        << tmp_altEtaPhiM.size() << " raw values "
        << ialt3-tmp_altEtaPhiM.begin() << " read.";
  }



  if (ibad1 != ibad2) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloClusterContainerCnv_p6")
      << "Corrupted data: not all bad data read "
      << pers->m_badClusIndexList.size() << " bad values "
      << nbad << " read.";
  }

  //Convert TowerSegment
  CaloTowerSeg seg;
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&seg);
  trans->setTowerSeg (seg);
}



void CaloClusterContainerCnv_p6::transToPers(const CaloClusterContainer* /*trans*/, 
					     CaloClusterContainer_p6* /*pers*/, MsgStream &log) {
  log << MSG::ERROR << "Writing of CaloClusterContainer_p6 not implemented any more!" << endmsg;

} 



void CaloClusterContainerCnv_p6::persToTrans(const CaloClusterContainer_p6::CaloCluster_p* pers, 
					     CaloCluster* trans, MsgStream& log) {
  trans->setDefaultSignalState (P4SignalState::CALIBRATED);
  trans->setBasicEnergy (pers->m_basicSignal);
  trans->setTime (pers->m_time);
  trans->m_samplingPattern=pers->m_samplingPattern; 
  trans->m_eta0=pers->m_eta0;
  trans->m_phi0=pers->m_phi0;   
  trans->m_status = CaloRecoStatus(pers->m_caloRecoStatus);
  trans->setClusterSize (pers->m_clusterSize);

  //Convert base class and element links
  m_P4EEtaPhiMCnv.persToTrans(&pers->m_P4EEtaPhiM,(P4EEtaPhiM*)trans,log);
  m_showerElementLinkCnv.persToTrans(&pers->m_dataLink,&trans->m_dataLink,log);
  m_cellElementLinkCnv.persToTrans(&pers->m_cellLink,&trans->m_cellLink,log);
  trans->setAthenaBarCode (IAthenaBarCode::UNDEFINEDBARCODE);
}




void CaloClusterContainerCnv_p6::transToPers(const CaloCluster* trans, 
					     CaloClusterContainer_p6::CaloCluster_p* pers, MsgStream& log) {

  pers->m_basicSignal=trans->getBasicEnergy();
  pers->m_time=trans->getTime();
  pers->m_samplingPattern=trans->m_samplingPattern; 
  pers->m_eta0=trans->eta0();
  pers->m_phi0=trans->phi0(); 
  pers->m_caloRecoStatus=trans->m_status.getStatusWord();
  pers->m_clusterSize=trans->getClusterSize();
 
  //Convert base class and element links
  m_P4EEtaPhiMCnv.transToPers((P4EEtaPhiM*)trans,&pers->m_P4EEtaPhiM,log);
  m_showerElementLinkCnv.transToPers(&trans->m_dataLink,&pers->m_dataLink,log);
  m_cellElementLinkCnv.transToPers(&trans->m_cellLink,&pers->m_cellLink,log);
}


void CaloClusterContainerCnv_p6::persToTransUntyped(const void* pers,
                                                    void* trans,
                                                    MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloClusterContainer_p6*>(pers),
               reinterpret_cast<CaloClusterContainer*>(trans),
               log);
}


void CaloClusterContainerCnv_p6::transToPersUntyped(const void* trans,
                                                    void* pers,
                                                    MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloClusterContainer*>(trans),
               reinterpret_cast<CaloClusterContainer_p6*>(pers),
               log);
}


const std::type_info& CaloClusterContainerCnv_p6::transientTInfo() const
{
  return typeid (CaloClusterContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloClusterContainerCnv_p6::persistentTInfo() const
{
  return typeid (CaloClusterContainer_p6);
}


