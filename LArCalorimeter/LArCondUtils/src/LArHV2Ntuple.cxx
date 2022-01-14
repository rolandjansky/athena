/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV2Ntuple.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVSubgap.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ThreadLocalContext.h"

  //Constructor
  LArHV2Ntuple:: LArHV2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_thistSvc(0),
    m_tree(0),
    m_addcells(false),
    m_bec(0),
    m_isPresampler(0),
    m_eta(0),
    m_phi(0),
    m_electrode(0),
    m_gap(0),
    m_hvline(0),
    m_hv(0),
    m_current(0),
    m_barrelec(0), m_posneg(0), m_FT(0),m_slot(0),m_channel(0),
    m_caloId(0), m_onlId(0)
  {
    declareProperty("AddCellID",m_addcells);
  }
  
  //__________________________________________________________________________
  //Destructor
  LArHV2Ntuple::~LArHV2Ntuple()
  {
    ATH_MSG_DEBUG ( "LArHV2Ntuple destructor called" );
  }
  //__________________________________________________________________________
  StatusCode LArHV2Ntuple::initialize()
  {
    ATH_MSG_INFO  ("LArHV2Ntuple initialize()" );
    ATH_CHECK( service("THistSvc",m_thistSvc) );

    ATH_CHECK( m_cablingKey.initialize() );
    ATH_CHECK( m_caloMgrKey.initialize() );

  m_tree = new TTree("mytree","Calo Noise ntuple");
  m_tree->Branch("bec",&m_bec,"bec/I");
  m_tree->Branch("isPresampler",&m_isPresampler,"isPresampler/I");
  m_tree->Branch("eta",&m_eta,"eta/F");
  m_tree->Branch("phi",&m_phi,"phi/F");
  m_tree->Branch("HVline",&m_hvline,"HVline/I");
  m_tree->Branch("electrode",&m_electrode,"electrode/I");
  m_tree->Branch("gap",&m_gap,"gap/I");
  m_tree->Branch("hv",&m_hv,"hv/F");
  m_tree->Branch("current",&m_current,"current/F");
  if(m_addcells) {
     m_tree->Branch("barrel_ec",&m_barrelec,"barrel_ec/I");
     m_tree->Branch("side",&m_posneg,"side/I");
     m_tree->Branch("FT",&m_FT,"FT/I");
     m_tree->Branch("slot",&m_slot,"slot/I");
     m_tree->Branch("channel",&m_channel,"channel/I");
     ATH_CHECK( detStore()->retrieve(m_caloId, "CaloCell_ID") );
     ATH_CHECK( detStore()->retrieve(m_onlId, "LArOnlineID") );
  }

  ATH_CHECK( m_hvCablingKey.initialize() );
  ATH_CHECK( m_DCSFolderKeys.initialize() );
 
  ATH_CHECK( m_thistSvc->regTree("/file1/hv/mytree",m_tree) );
  return StatusCode::SUCCESS; 

  }
  //__________________________________________________________________________
  StatusCode LArHV2Ntuple::execute()
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();

    //.............................................

    // FIXME: Use LArHVData instead?
    SG::ReadCondHandle<LArHVIdMapping> hvCabling (m_hvCablingKey, ctx);
    std::vector<const CondAttrListCollection*> attrLists;
    for (const SG::ReadCondHandleKey<CondAttrListCollection>& k : m_DCSFolderKeys)
    {
      SG::ReadCondHandle<CondAttrListCollection> attrList (k, ctx);
      attrLists.push_back (*attrList);
    }
    
    const LArHVManager *manager = nullptr;
    ATH_CHECK( detStore()->retrieve(manager) );

    const EMBHVManager& hvManager_EMB=manager->getEMBHVManager();
    const EMBHVManager::EMBHVData hvdata_EMB = hvManager_EMB.getData (**hvCabling, attrLists);

    const EMBPresamplerHVManager& hvManager_EMBPS=manager->getEMBPresamplerHVManager();
    const EMBPresamplerHVManager::EMBPresamplerHVData hvdata_EMBPS = hvManager_EMBPS.getData (**hvCabling, attrLists);

    const EMECPresamplerHVManager& hvManager_EMECPS=manager->getEMECPresamplerHVManager();
    const EMECPresamplerHVManager::EMECPresamplerHVData hvdata_EMECPS = hvManager_EMECPS.getData (**hvCabling, attrLists);

    const EMECHVManager& hvManager_EMEC_OUT=manager->getEMECHVManager(EMECHVModule::OUTER);
    const EMECHVManager::EMECHVData hvdata_EMEC_OUT = hvManager_EMEC_OUT.getData (**hvCabling, attrLists);

    const EMECHVManager& hvManager_EMEC_IN=manager->getEMECHVManager(EMECHVModule::INNER);
    const EMECHVManager::EMECHVData hvdata_EMEC_IN = hvManager_EMEC_IN.getData (**hvCabling, attrLists);

    const HECHVManager& hvManager_HEC=manager->getHECHVManager();
    const HECHVManager::HECHVData hvdata_HEC = hvManager_HEC.getData (**hvCabling, attrLists);

    const FCALHVManager& hvManager_FCAL=manager->getFCALHVManager();
    const FCALHVManager::FCALHVData hvdata_FCAL = hvManager_FCAL.getData (**hvCabling, attrLists);

    ATH_MSG_DEBUG ( "LArHV2Ntuple execute()" );
    if(m_hvonlId_map.size()==0) {
      SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
      const LArOnOffIdMapping* cabling{*cablingHdl};
      if(!cabling) {
        ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
        return StatusCode::FAILURE;
      }

      SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey, ctx};
      ATH_CHECK(caloMgrHandle.isValid());
      const CaloDetDescrManager* calodetdescrmgr=*caloMgrHandle;
      std::vector<Identifier>::const_iterator cell_b=m_caloId->cell_begin();
      std::vector<Identifier>::const_iterator cell_e=m_caloId->cell_end();
      for(;cell_b!=cell_e; ++cell_b) {
        if(m_caloId->is_tile(*cell_b)) continue;
        HWIdentifier onlid = cabling->createSignalChannelID(*cell_b);
        std::vector<int> hvlines = GetHVLines (hvdata_EMB,
                                               hvdata_EMBPS,
                                               hvdata_EMEC_OUT,
                                               hvdata_EMEC_IN,
                                               hvdata_EMECPS,
                                               hvdata_HEC,
                                               hvdata_FCAL,
                                               *cell_b,
					       calodetdescrmgr);
        for(unsigned i=0; i<hvlines.size(); ++i ) {
          if(m_hvonlId_map.find(hvlines[i]) == m_hvonlId_map.end()) { // new key
            std::vector<HWIdentifier> vec;
            vec.push_back(onlid);
            m_hvonlId_map[hvlines[i]] = vec;
          } else { // existing key
            m_hvonlId_map[hvlines[i]].push_back(onlid);
          }
        }
      }// end map filling
    }

    for (unsigned int iSide=hvManager_EMB.beginSideIndex();iSide<hvManager_EMB.endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMB.beginPhiIndex();iPhi<hvManager_EMB.endPhiIndex();iPhi++) {
        for (unsigned int iSector=hvManager_EMB.beginSectorIndex();iSector<hvManager_EMB.endSectorIndex();iSector++) {
          for (unsigned int iEta=hvManager_EMB.beginEtaIndex();iEta<hvManager_EMB.endEtaIndex();iEta++) { //0 to 7
            const EMBHVModule& hvMod = hvManager_EMB.getHVModule(iSide,iEta,iPhi,iSector);
            float eta=0.5*(hvMod.getEtaMin()+hvMod.getEtaMax());
            for (unsigned int ielec=0;ielec<32;ielec++) { //use hvMod->getNumElectrodes when bug is corrected
              const EMBHVElectrode& electrode = hvMod.getElectrode(ielec);
              for (unsigned int iGap=0;iGap<2;iGap++) { // EMB : 2, TRY TO FIND AUTOMATICALLY NB OF GAPS
                float hv = hvdata_EMB.voltage (electrode, iGap);
                float current = hvdata_EMB.current (electrode, iGap);
                float phi = electrode.getPhi();

                m_bec=0;
                m_isPresampler=0;
                m_eta=eta;
                m_phi=phi;
                m_electrode = ielec;
                m_gap = iGap;
                m_hv = hv;
                m_current= current;
                m_hvline = hvdata_EMB.hvLineNo (electrode, iGap);

                if(m_addcells) {
                  for(unsigned i=0; i<m_hvonlId_map[m_hvline].size(); ++i) {
                     m_barrelec=m_onlId->barrel_ec(m_hvonlId_map[m_hvline][i]);
                     m_posneg=m_onlId->pos_neg(m_hvonlId_map[m_hvline][i]);
                     m_FT=m_onlId->feedthrough(m_hvonlId_map[m_hvline][i]);
                     m_slot=m_onlId->slot(m_hvonlId_map[m_hvline][i]);
                     m_channel=m_onlId->channel(m_hvonlId_map[m_hvline][i]);
                     m_tree->Fill();
                  }
                } else m_tree->Fill();

              } //end for iGap
            }
          }
        }
      }
    } //EMBHVManager

    for (unsigned int iSide=hvManager_EMBPS.beginSideIndex();iSide<hvManager_EMBPS.endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMBPS.beginPhiIndex();iPhi<hvManager_EMBPS.endPhiIndex();iPhi++) {
         for (unsigned int iEta=hvManager_EMBPS.beginEtaIndex();iEta<hvManager_EMBPS.endEtaIndex();iEta++) { //0 to 7
            const EMBPresamplerHVModule& hvMod = hvManager_EMBPS.getHVModule(iSide,iEta,iPhi);
            for (int iGap=0;iGap<2;iGap++) {
             float hv = hvdata_EMBPS.voltage (hvMod, iGap);
             float current = hvdata_EMBPS.current (hvMod, iGap);
             float eta = 0.5*(hvMod.getEtaMin()+hvMod.getEtaMax()); 
             float phi= 0.5*(hvMod.getPhiMin()+hvMod.getPhiMax());

             m_bec=0;
             m_isPresampler=1;
             m_eta=eta;
             m_phi=phi;
             m_electrode = 0;
             m_gap = iGap;
             m_hv = hv; 
             m_current= current;
             m_hvline = hvdata_EMBPS.hvLineNo (hvMod, iGap);

             if(m_addcells) {
                  for(unsigned i=0; i<m_hvonlId_map[m_hvline].size(); ++i) {
                     m_barrelec=m_onlId->barrel_ec(m_hvonlId_map[m_hvline][i]);
                     m_posneg=m_onlId->pos_neg(m_hvonlId_map[m_hvline][i]);
                     m_FT=m_onlId->feedthrough(m_hvonlId_map[m_hvline][i]);
                     m_slot=m_onlId->slot(m_hvonlId_map[m_hvline][i]);
                     m_channel=m_onlId->channel(m_hvonlId_map[m_hvline][i]);
                     m_tree->Fill();
                  }
             } else m_tree->Fill();

            } //end for iGap
         }
      }
    } //EMBPresampler

    for (unsigned int iSide=hvManager_EMECPS.beginSideIndex();iSide<hvManager_EMECPS.endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMECPS.beginPhiIndex();iPhi<hvManager_EMECPS.endPhiIndex();iPhi++) {
            const EMECPresamplerHVModule& hvMod = hvManager_EMECPS.getHVModule(iSide,iPhi);
            for (int iGap=0;iGap<2;iGap++) {
             float hv = hvdata_EMECPS.voltage (hvMod, iGap);
             float current = hvdata_EMECPS.current (hvMod, iGap);
             float eta = 0.5*(hvMod.getEtaMin()+hvMod.getEtaMax());
             float phi=0.5*(hvMod.getPhiMin()+hvMod.getPhiMax());

             m_bec=1;
             m_isPresampler=1;
             m_eta=eta;
             m_phi=phi;
             m_electrode = 0;
             m_gap = iGap;
             m_hv = hv;   
             m_current= current;
             m_hvline = hvdata_EMECPS.hvLineNo (hvMod, iGap);

             if(m_addcells) {
                  for(unsigned i=0; i<m_hvonlId_map[m_hvline].size(); ++i) {
                     m_barrelec=m_onlId->barrel_ec(m_hvonlId_map[m_hvline][i]);
                     m_posneg=m_onlId->pos_neg(m_hvonlId_map[m_hvline][i]);
                     m_FT=m_onlId->feedthrough(m_hvonlId_map[m_hvline][i]);
                     m_slot=m_onlId->slot(m_hvonlId_map[m_hvline][i]);
                     m_channel=m_onlId->channel(m_hvonlId_map[m_hvline][i]);
                     m_tree->Fill();
                  }
             } else m_tree->Fill();

            } //end for iGap
      }
    }//EMECPresampler



    for (unsigned int iSide=hvManager_EMEC_OUT.beginSideIndex();iSide<hvManager_EMEC_OUT.endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC_OUT.beginPhiIndex();iPhi<hvManager_EMEC_OUT.endPhiIndex();iPhi++) {
        for (unsigned int iSector=hvManager_EMEC_OUT.beginSectorIndex();iSector<hvManager_EMEC_OUT.endSectorIndex();iSector++) {
          for (unsigned int iEta=hvManager_EMEC_OUT.beginEtaIndex();iEta<hvManager_EMEC_OUT.endEtaIndex();iEta++) {
            const EMECHVModule& hvMod=hvManager_EMEC_OUT.getHVModule(iSide,iEta,iPhi,iSector);
            float eta=0.5*(hvMod.getEtaMin()+hvMod.getEtaMax());
            for (unsigned int ielec=0;ielec<hvMod.getNumElectrodes();ielec++) { //use hvMod.getNumElectrodes when bug is corrected
              const EMECHVElectrode& electrode = hvMod.getElectrode(ielec);
              for (unsigned int iGap=0;iGap<2;iGap++) { //EMEC : 2 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
                float hv = hvdata_EMEC_OUT.voltage (electrode, iGap);
                float current = hvdata_EMEC_OUT.current (electrode, iGap);
                float phi = electrode.getPhi();

                m_bec=1;
                m_isPresampler=0;
                m_eta=eta;
                m_phi=phi;
                m_electrode = ielec;
                m_gap = iGap;
                m_hv = hv;
                m_current= current;
                m_hvline = hvdata_EMEC_OUT.hvLineNo (electrode, iGap);

                if(m_addcells) {
                  for(unsigned i=0; i<m_hvonlId_map[m_hvline].size(); ++i) {
                     m_barrelec=m_onlId->barrel_ec(m_hvonlId_map[m_hvline][i]);
                     m_posneg=m_onlId->pos_neg(m_hvonlId_map[m_hvline][i]);
                     m_FT=m_onlId->feedthrough(m_hvonlId_map[m_hvline][i]);
                     m_slot=m_onlId->slot(m_hvonlId_map[m_hvline][i]);
                     m_channel=m_onlId->channel(m_hvonlId_map[m_hvline][i]);
                     m_tree->Fill();
                  }
                } else m_tree->Fill();

              } //end for iGap
            }
          }
        }
      }
    }//EMEC Outer

    for (unsigned int iSide=hvManager_EMEC_IN.beginSideIndex();iSide<hvManager_EMEC_IN.endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC_IN.beginPhiIndex();iPhi<hvManager_EMEC_IN.endPhiIndex();iPhi++) {
        for (unsigned int iSector=hvManager_EMEC_IN.beginSectorIndex();iSector<hvManager_EMEC_IN.endSectorIndex();iSector++) {
          for (unsigned int iEta=hvManager_EMEC_IN.beginEtaIndex();iEta<hvManager_EMEC_IN.endEtaIndex();iEta++) {
            const EMECHVModule& hvMod=hvManager_EMEC_IN.getHVModule(iSide,iEta,iPhi,iSector);
            float eta=0.5*(hvMod.getEtaMin()+hvMod.getEtaMax());
            for (unsigned int ielec=0;ielec<hvMod.getNumElectrodes();ielec++) { //use hvMod.getNumElectrodes when bug is corrected
              const EMECHVElectrode& electrode = hvMod.getElectrode(ielec);
              for (unsigned int iGap=0;iGap<2;iGap++) { //EMEC : 2 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
                float hv = hvdata_EMEC_IN.voltage (electrode, iGap);
                float current = hvdata_EMEC_IN.current (electrode, iGap);
                float phi = electrode.getPhi();
            
                m_bec=2;
                m_isPresampler=0;
                m_eta=eta;
                m_phi=phi;
                m_electrode = ielec;
                m_gap = iGap;
                m_hv = hv;
                m_current= current;
                m_hvline = hvdata_EMEC_IN.hvLineNo (electrode, iGap);

                if(m_addcells) {
                  for(unsigned i=0; i<m_hvonlId_map[m_hvline].size(); ++i) {
                     m_barrelec=m_onlId->barrel_ec(m_hvonlId_map[m_hvline][i]);
                     m_posneg=m_onlId->pos_neg(m_hvonlId_map[m_hvline][i]);
                     m_FT=m_onlId->feedthrough(m_hvonlId_map[m_hvline][i]);
                     m_slot=m_onlId->slot(m_hvonlId_map[m_hvline][i]);
                     m_channel=m_onlId->channel(m_hvonlId_map[m_hvline][i]);
                     m_tree->Fill();
                  }
                } else m_tree->Fill();

              } //end for iGap
            }
          }
        }
      }
    }// EMEC Inner

    float etamax_layer[4]={3.3,3.1,3.1,3.3};
    float etamin_layer[4]={1.5,1.5,1.6,1.7};

    
    for (unsigned int iSide=hvManager_HEC.beginSideIndex();iSide<hvManager_HEC.endSideIndex();iSide++) { // loop over HV modules      
      for (unsigned int iPhi=hvManager_HEC.beginPhiIndex();iPhi<hvManager_HEC.endPhiIndex();iPhi++) {
        for (unsigned int iSampling=hvManager_HEC.beginSamplingIndex();iSampling<hvManager_HEC.endSamplingIndex();iSampling++) {
          float eta_min,eta_max;
          if (iSide==1) {
           eta_min = etamin_layer[iSampling];
           eta_max = etamax_layer[iSampling];
          } else {
           eta_min = -1.*etamax_layer[iSampling];
           eta_max = -1.*etamin_layer[iSampling];
         }
         float eta = 0.5*(eta_min+eta_max);
         const HECHVModule& hvMod = hvManager_HEC.getHVModule(iSide,iPhi,iSampling);
         float phi = 0.5*(hvMod.getPhiMin()+hvMod.getPhiMax());

         for (unsigned int iGap=0;iGap<hvMod.getNumSubgaps();iGap++) {//HEC : 4 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
            const HECHVSubgap& subgap=hvMod.getSubgap(iGap);
            float hv = hvdata_HEC.voltage (subgap);
            float current = hvdata_HEC.current (subgap);
            m_bec = 10+iSampling;
            m_isPresampler=0;
            m_eta=eta;
            m_phi=phi;
            m_electrode = 0;
            m_gap = iGap;
            m_hv=hv;
            m_current=current;
            m_hvline = hvdata_HEC.hvLineNo (subgap);
            if(m_addcells) {
                  for(unsigned i=0; i<m_hvonlId_map[m_hvline].size(); ++i) {
                     m_barrelec=m_onlId->barrel_ec(m_hvonlId_map[m_hvline][i]);
                     m_posneg=m_onlId->pos_neg(m_hvonlId_map[m_hvline][i]);
                     m_FT=m_onlId->feedthrough(m_hvonlId_map[m_hvline][i]);
                     m_slot=m_onlId->slot(m_hvonlId_map[m_hvline][i]);
                     m_channel=m_onlId->channel(m_hvonlId_map[m_hvline][i]);
                     m_tree->Fill();
                  }
            } else m_tree->Fill();
         }// end for iGap
       }
     }
   }//HECHVManager 

   for (unsigned int iSide=hvManager_FCAL.beginSideIndex();iSide<hvManager_FCAL.endSideIndex();iSide++) { // loop over HV modules
       float eta_min=3.1,eta_max=4.9;
       if (iSide==0) { eta_min=-4.9; eta_max=-3.1; }

       float eta = 0.5*(eta_min+eta_max);
       for (unsigned int iSampling=hvManager_FCAL.beginSamplingIndex();iSampling<hvManager_FCAL.endSamplingIndex();iSampling++) {
            for (unsigned int iSector=hvManager_FCAL.beginSectorIndex(iSampling);iSector<hvManager_FCAL.endSectorIndex(iSampling);iSector++) {
 
                 const FCALHVModule& hvMod = hvManager_FCAL.getHVModule(iSide,iSector,iSampling);
  
                 float dphi=CaloPhiRange::twopi()/16;
                 if (iSampling==1) dphi=CaloPhiRange::twopi()/8.;
                 if (iSampling==2) dphi=CaloPhiRange::twopi()/4.;
                 float phi_min = ((float)(iSector))*dphi;
                 phi_min =   CaloPhiRange::fix(phi_min);
                 float phi_max = CaloPhiRange::fix(dphi+phi_min);
                 float phi = 0.5*(phi_min+phi_max);
         
                 for (unsigned int iLine=0;iLine<hvMod.getNumHVLines();iLine++) {
                     const FCALHVLine& hvline = hvMod.getHVLine(iLine);
                     float hv = hvdata_FCAL.voltage (hvline);
                     float current = hvdata_FCAL.current (hvline);
                     m_bec = 14+iSampling;
                     m_isPresampler=0;
                     m_eta=eta;
                     m_phi=phi;
                     m_electrode = iSector;
                     m_gap = iLine;
                     m_hv=hv;
                     m_current=current;
                     m_hvline = hvdata_FCAL.hvLineNo (hvline);
                     if(m_addcells) {
                       for(unsigned i=0; i<m_hvonlId_map[m_hvline].size(); ++i) {
                         m_barrelec=m_onlId->barrel_ec(m_hvonlId_map[m_hvline][i]);
                         m_posneg=m_onlId->pos_neg(m_hvonlId_map[m_hvline][i]);
                         m_FT=m_onlId->feedthrough(m_hvonlId_map[m_hvline][i]);
                         m_slot=m_onlId->slot(m_hvonlId_map[m_hvline][i]);
                         m_channel=m_onlId->channel(m_hvonlId_map[m_hvline][i]);
                         m_tree->Fill();
                       }
                     } else m_tree->Fill();
                 }
            } //iSector
       }//iSampling
   }//iSide

  return StatusCode::SUCCESS;
 }

std::vector<int> LArHV2Ntuple::GetHVLines (const EMBHVManager::EMBHVData& hvdata_EMB,
                                           const EMBPresamplerHVManager::EMBPresamplerHVData& hvdata_EMBPS,
                                           const EMECHVManager::EMECHVData& hvdata_EMEC_OUT,
                                           const EMECHVManager::EMECHVData& hvdata_EMEC_IN,
                                           const EMECPresamplerHVManager::EMECPresamplerHVData& hvdata_EMECPS,
                                           const HECHVManager::HECHVData& hvdata_HEC,
                                           const FCALHVManager::FCALHVData& hvdata_FCAL,
                                           const Identifier& id,
					   const CaloDetDescrManager* calodetdescrmgr) {

   std::set<int> hv;
 
   // LAr EMB
   if (m_caloId->is_em(id) && m_caloId->sampling(id)>0) {
     if (abs(m_caloId->em_idHelper()->barrel_ec(id))==1) {
       const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(calodetdescrmgr->get_element(id));
       if (!embElement) std::abort();
       const EMBCellConstLink cell = embElement->getEMBCell();
       unsigned int nelec = cell->getNumElectrodes();
       for (unsigned int i=0;i<nelec;i++) {
         const EMBHVElectrode& electrode = cell->getElectrode(i);
         for (unsigned int igap=0;igap<2;igap++) hv.insert(hvdata_EMB.hvLineNo(electrode, igap));
       }
     } else { // LAr EMEC
       const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(calodetdescrmgr->get_element(id));
       if (!emecElement) std::abort();
       const EMECCellConstLink cell = emecElement->getEMECCell();
       unsigned int nelec = cell->getNumElectrodes();
       for (unsigned int i=0;i<nelec;i++) {
         const EMECHVElectrode& electrode = cell->getElectrode(i);
         const EMECHVModule& module = electrode.getModule();
         const EMECHVManager::EMECHVData& hvdata =
           module.getWheelIndex() == EMECHVModule::INNER ?
             hvdata_EMEC_IN :
             hvdata_EMEC_OUT;
         for (unsigned int igap=0;igap<2;igap++) hv.insert(hvdata.hvLineNo (electrode, igap));
       }
     }
   } else if (m_caloId->is_hec(id)) { // LAr HEC
     const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(calodetdescrmgr->get_element(id));
     if (!hecElement) std::abort();
     const HECCellConstLink cell = hecElement->getHECCell();
     unsigned int nsubgaps = cell->getNumSubgaps();
     for (unsigned int igap=0;igap<nsubgaps;igap++) {
       const HECHVSubgap& subgap = cell->getSubgap(igap);
       hv.insert(hvdata_HEC.hvLineNo (subgap));
     }
   } else if (m_caloId->is_fcal(id)) { // LAr FCAL
     const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(calodetdescrmgr->get_element(id));
     if (!fcalElement) std::abort();
     const FCALTile* tile = fcalElement->getFCALTile();
     unsigned int nlines = tile->getNumHVLines();
     for (unsigned int i=0;i<nlines;i++) {
       const FCALHVLine* line = tile->getHVLine(i);
       if (line) hv.insert(hvdata_FCAL.hvLineNo (*line));
     }
   } else if (m_caloId->is_em(id) && m_caloId->sampling(id)==0) { // Presamplers
     if (abs(m_caloId->em_idHelper()->barrel_ec(id))==1) {
       const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(calodetdescrmgr->get_element(id));
       if (!embElement) std::abort();
       const EMBCellConstLink cell = embElement->getEMBCell();
       const EMBPresamplerHVModule& hvmodule = cell->getPresamplerHVModule();
       for (unsigned int igap=0;igap<2;igap++) hv.insert(hvdata_EMBPS.hvLineNo (hvmodule, igap));
     } else {
       const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(calodetdescrmgr->get_element(id));
       if (!emecElement) std::abort();
       const EMECCellConstLink cell = emecElement->getEMECCell();
       const EMECPresamplerHVModule& hvmodule = cell->getPresamplerHVModule ();
       for (unsigned int igap=0;igap<2;igap++) hv.insert(hvdata_EMECPS.hvLineNo (hvmodule, igap));
     }
   }
 
   std::vector<int> hvlines;
   for (std::set<int>::iterator i=hv.begin();i!=hv.end();++i) hvlines.push_back(*i);
   return hvlines;
 }
