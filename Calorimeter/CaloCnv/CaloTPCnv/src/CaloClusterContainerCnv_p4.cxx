/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterContainerCnv_p4.h" 

#include "DataModel/DataPool.h"
    
#include "AthenaPoolCnvSvc/Compressor.h"
#include "CaloEvent/CaloSamplingData.h"
#include "CaloConditions/CaloBadChannel.h"
//#include "CaloEvent/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "AthenaKernel/errorcheck.h"


namespace {

inline
bool testbit (unsigned int x, unsigned int i)
{
  return (x & (1U << i)) != 0;
}

} // anonymous namespace


void CaloClusterContainerCnv_p4::persToTrans(const CaloClusterContainer_p4* pers, 
					     CaloClusterContainer* trans, MsgStream &log) {
  if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "Reading CaloClusterContainerCnv_p4" << endmsg;

  static CaloPhiRange range;
					     
  // reset element link converters, and provide container name lookup table
  m_showerElementLinkCnv.resetForCnv(pers->m_linkNames);
  m_cellElementLinkCnv.resetForCnv(pers->m_linkNames);

  // Use data pool for clusters to avoid calling constructor for each event
  DataPool<CaloCluster> clusters;

  trans->clear (SG::VIEW_ELEMENTS);
  trans->reserve(pers->m_vec.size());
  
  CaloClusterContainer_p4::const_iterator itp=pers->m_vec.begin();
  CaloClusterContainer_p4::const_iterator itp_e=pers->m_vec.end();
  
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

  std::vector<float> tmp_rawE;
  Compressor C;
  C.expandToFloat(pers->m_rawEEtaPhiM,tmp_rawE);
  std::vector<float>::const_iterator iraw1 = tmp_rawE.begin();
  std::vector<float>::const_iterator iraw2 = tmp_rawE.end();

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
                                            "CaloClusterContainerCnv_p4")
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
                                      "CaloClusterContainerCnv_p4")
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
    if (iraw1 != iraw2) {
      transCluster->m_rawEta = (*iraw1) + transCluster->eta();
      iraw1++;
    }
    if (iraw1 != iraw2) { 
      transCluster->m_rawPhi = range.fix((*iraw1)+transCluster->phi());
      iraw1++;
    }
    if (iraw1 == iraw2 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                  "CaloClusterContainerCnv_p4")
        << "Corrupted data: raw array overrun "
        << pers->m_vec.size() << " clusters "
        << tmp_rawE.size() << " raw values.";
      raw_overrun_err = true;
    }
    if (iraw1 != iraw2) {
      transCluster->m_rawM = (*iraw1);
      iraw1++;
    }
   // std::cout << " perstotrans rawE/eta/phi/M " << transCluster->m_rawE << " " << transCluster->m_rawEta << " " <<
   //   transCluster->m_rawPhi << " " << transCluster->m_rawM << std::endl;

   // no alt calib information available in p4 persistent
    transCluster->m_altE=0.;
    transCluster->m_altEta=0.;
    transCluster->m_altPhi=0.;
    transCluster->m_altM=0.;


    trans->push_back(transCluster);
  }

  if (i_mom != i_mom_e && !mom_overrun_err) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloClusterContainerCnv_p4")
      << "Corrupted data: not all moments read "
      << pers->m_vec.size() << " clusters "
      << nkeys << " moment keys "
      << temp_Moments.size() << " total moments "
      << i_mom-temp_Moments.begin() << " read.";
  }

  if (c1 != dataStore_size && !store_overrun_err) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloClusterContainerCnv_p4")
      << "Corrupted data: not all data store members  read "
      << dataStore_size << " elements "
      << c1 << " read.";
  }

  if (iraw1 != iraw2 && !raw_overrun_err) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                  "CaloClusterContainerCnv_p4")
        << "Corrupted data: not all raw values read "
        << pers->m_vec.size() << " clusters "
        << tmp_rawE.size() << " raw values "
        << iraw1-tmp_rawE.begin() << " read.";
  }

  if (ibad1 != ibad2) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloClusterContainerCnv_p4")
      << "Corrupted data: not all bad data read "
      << pers->m_badClusIndexList.size() << " bad values "
      << nbad << " read.";
  }

  //Convert TowerSegment
  CaloTowerSeg seg;
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&seg);
  trans->setTowerSeg (seg);
}



void CaloClusterContainerCnv_p4::transToPers(const CaloClusterContainer* trans, 
					     CaloClusterContainer_p4* pers, MsgStream &log) {
  if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "Writing CaloClusterContainerCnv_p4" << endmsg;


  static CaloPhiRange range;

  // reset element link converters, and provide container name lookup table
  m_showerElementLinkCnv.resetForCnv(pers->m_linkNames);
  m_cellElementLinkCnv.resetForCnv(pers->m_linkNames);

  pers->m_vec.resize(trans->size()); 
  CaloClusterContainer_p4::iterator itp=pers->m_vec.begin();
  CaloClusterContainer::const_iterator it=trans->begin(); 
  CaloClusterContainer::const_iterator it_e=trans->end();  

  pers->m_badClusIndexList.clear();
  pers->m_badLayerStatusList.clear();
  pers->m_badEtaList.clear();
  pers->m_badPhiList.clear();

  pers->m_rawEEtaPhiM.clear();
 
  // Set varTypePattern to the OR of the patterns of all clusters.
  // (For samplings.)
  // Trigger can make cluster collections with multiple pattern masks!
  //
  // Also pick up moment keys here.  Trigger can make clusters with no
  // stored moments, so we need to look until we find one that isn't empty.
  // Store the moment keys and their number.
  pers->m_varTypePattern=0;
  pers->m_momentContainer.m_Mkey.clear();
  pers->m_momentContainer.m_nMoments = 0;
  for (CaloClusterContainer::const_iterator it2 = it; it2 != it_e; ++it2)
  {
    const CaloCluster& cl = **it2;
    const CaloClusterMomentStore& ms = cl.m_momentStore;
    pers->m_varTypePattern |= cl.m_dataStore.m_varTypePattern;

    if (pers->m_momentContainer.m_nMoments == 0 && ms.size() != 0) {
      CaloClusterMomentStore::moment_iterator mit=ms.begin();
      int nMom=ms.size();
      pers->m_momentContainer.m_nMoments = nMom;
      for (int w=0; w<nMom; w++){  // here stores moment keys per container not cluster
        pers->m_momentContainer.m_Mkey.push_back(mit.getMomentType());  
        ++mit; 
      }
    }
    else if (ms.size() > 0 && pers->m_momentContainer.m_nMoments != ms.size())
    {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,
                                  "CaloClusterContainerCnv_p4")
        << "Inconsistent number of cluster moments: cluster "
        << it2 - trans->begin() << " of " << trans->size()
        << " have "  << ms.size()
        << " expect " << pers->m_momentContainer.m_nMoments;
    }
        
  }
	
  std::vector<float> temp_Moments;
  temp_Moments.reserve (trans->size() * pers->m_momentContainer.m_nMoments);
  std::vector<float> tmp_badChannelEta;
  std::vector<float> tmp_badChannelPhi;

  std::vector<float> tmp_rawE;
  tmp_rawE.reserve(4*trans->size());
  
  
  short index=0;
  for(;it!=it_e;++it,++itp) {  // LOOP over clusters
    const CaloCluster& cl = **it;
    //CaloClusterChangeSignalState statechange (&cl, P4SignalState::CALIBRATED);
    P4SignalState::State oldstate=(*it)->signalState();
    (*it)->setSignalState(P4SignalState::CALIBRATED);
    index++;
    transToPers(&cl,&(*itp),log); // tP of CaloCluster to CaloCluster_p 
	
    const CaloSamplingData* trDS=&cl.m_dataStore; //transient
    int nVar=trDS->getNumberOfVariableTypes();
    int nSamplings=trDS->getNumberOfSamplings();
//	std::cout<<"vars:"<<nVar<<"\t samplings:"<<nSamplings<<std::endl;
//	std::cout<<"b:"<<(*it)->m_barrel<<"\te:"<<(*it)->m_endcap<<std::endl;
//	std::cout<<"sPattern: "<<(*it)->m_samplingPattern<<"\tgB:"<<(0x001ff00f & (*it)->m_samplingPattern) <<"\tgEC:"<< (0x00e00ff0 & (*it)->m_samplingPattern)  <<std::endl;
	
        typedef CaloSamplingData::variable_key_type vartype;
        typedef CaloSamplingData::sampling_key_type samptype;

	for (int i=0;i<nVar;++i) {
          if (testbit (pers->m_varTypePattern, i)) {
            // We're writing this pattern.  Was it in the cluster?
            if (testbit (trDS->m_varTypePattern, i)) {
              // Yes, store it.
              for (int j=0;j<nSamplings; ++j)
                if ( testbit(cl.m_samplingPattern, j)) {
                  pers->m_dataStore.push_back(trDS->retrieveData
                                              (static_cast<vartype>(i),
                                               static_cast<samptype>(j)));
                }
            }
            else {
              // Not in the cluster.  Store 0's.
              for (int j=0;j<nSamplings; ++j)
                if ( testbit(cl.m_samplingPattern, j))
                  pers->m_dataStore.push_back(0);
            }
          }
          else{
            // We're not writing.  Check that nothing was stored...
            for (int j=0;j<nSamplings; ++j)
              if ( testbit(cl.m_samplingPattern, j))
                if(trDS->retrieveData(static_cast<vartype>(i),
                                      static_cast<samptype>(j)))
                  if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG<<"BIG PROBLEM ! not stored and has value:"
                     <<trDS->retrieveData(static_cast<vartype>(i),
                                          static_cast<samptype>(j))
                     <<"\t var: "<<i
                     <<"\t sampling: "<<j<<endmsg;
          }
        }
			
	
        //CaloClusterMomentStore::moment_iterator itm=cl.m_momentStore.begin(); 
        //CaloClusterMomentStore::moment_iterator itm_e=cl.m_momentStore.end();
	
        std::vector<unsigned short>::const_iterator itk = 
          pers->m_momentContainer.m_Mkey.begin();
        std::vector<unsigned short>::const_iterator itk_e = 
          pers->m_momentContainer.m_Mkey.end();
        CaloClusterMomentStore::moment_iterator itm =
          cl.m_momentStore.begin();
        CaloClusterMomentStore::moment_iterator itm_e =
          cl.m_momentStore.end();
        for (; itk != itk_e; ++itk) {
          float val = 0;
          if (itm != itm_e && itm.getMomentType() == *itk) {
            val = (*itm).getValue();
            ++itm;
          }
          else {
            itm = cl.m_momentStore.find
              (static_cast<CaloClusterMomentStore::moment_type> (*itk));
            if (itm != itm_e) {
              val = (*itm).getValue();
              ++itm;
            }
          }
          temp_Moments.push_back(val);
        }

// bad channel info
       const CaloCluster::badChannelList* badlist = cl.getBadChannel();
       // std::cout << " transtopers: bad channel size " << badlist->size() << std::endl;
       for (const CaloClusterBadChannelData& bad : *badlist) {
          pers->m_badClusIndexList.push_back(index);
          float etac = bad.getEta()-(cl.eta());
          float phic = range.diff(bad.getPhi(),cl.phi());
          CaloSampling::CaloSample sampl = bad.getLayer();
          CaloBadChannel flag = bad.getFlag();
          short status =  ((sampl & 0xff) | ((flag.packedData() & 0xff) << 8));
          tmp_badChannelEta.push_back(etac);
          tmp_badChannelPhi.push_back(phic);
          pers->m_badLayerStatusList.push_back(status);
       }

// raw E-eta-phi-M
       // std::cout << "transtopers: rawE/eta/phi/M " << (*it)->m_rawE << " " << (*it)->m_rawEta << " " << (*it)->m_rawPhi << " " << (*it)->m_rawM << std::endl;
       double e = cl.e();
       if (std::fabs(e)>0.1)  {
         float eratio = cl.m_rawE / e;
         tmp_rawE.push_back( eratio);
       }
       else
         tmp_rawE.push_back( cl.m_rawE);
       tmp_rawE.push_back( cl.m_rawEta-cl.eta());
       float dphi = range.diff ( cl.m_rawPhi, cl.phi());
       tmp_rawE.push_back( dphi);
       tmp_rawE.push_back( cl.m_rawM);

       //reset signal state
       (*it)->setSignalState(oldstate);
   } // end of loop over clusters
   
  Compressor A;
  A.setNrBits(16);
  // A.setIgnoreSign();
  A.reduce(temp_Moments,pers->m_momentContainer.m_Mvalue); // packs moments

  Compressor B;
  B.setNrBits(16);
  B.reduce(tmp_badChannelEta,pers->m_badEtaList);
  B.reduce(tmp_badChannelPhi,pers->m_badPhiList);

  Compressor C;
  C.setNrBits(16);
  C.reduce(tmp_rawE,pers->m_rawEEtaPhiM);
   
  m_caloTowerSegCnv.transToPers(&trans->getTowerSeg(),&(pers->m_towerSeg));
} 



void CaloClusterContainerCnv_p4::persToTrans(const CaloClusterContainer_p4::CaloCluster_p* pers, 
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




void CaloClusterContainerCnv_p4::transToPers(const CaloCluster* trans, 
					     CaloClusterContainer_p4::CaloCluster_p* pers, MsgStream& log) {

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


void CaloClusterContainerCnv_p4::persToTransUntyped(const void* pers,
                                                    void* trans,
                                                    MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloClusterContainer_p4*>(pers),
               reinterpret_cast<CaloClusterContainer*>(trans),
               log);
}


void CaloClusterContainerCnv_p4::transToPersUntyped(const void* trans,
                                                    void* pers,
                                                    MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloClusterContainer*>(trans),
               reinterpret_cast<CaloClusterContainer_p4*>(pers),
               log);
}


const std::type_info& CaloClusterContainerCnv_p4::transientTInfo() const
{
  return typeid (CaloClusterContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloClusterContainerCnv_p4::persistentTInfo() const
{
  return typeid (CaloClusterContainer_p4);
}


