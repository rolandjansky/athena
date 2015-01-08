/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondUtils/LArHV2Ntuple.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVModuleConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVModuleConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVModuleConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"



  //Constructor
  LArHV2Ntuple:: LArHV2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_thistSvc(0),
    m_tree(0),
    m_bec(0),
    m_isPresampler(0),
    m_eta(0),
    m_phi(0),
    m_electrode(0),
    m_gap(0),
    m_hvline(0),
    m_hv(0),
    m_current(0)
  {

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
 
  ATH_CHECK( m_thistSvc->regTree("/file1/hv/mytree",m_tree) );
  return StatusCode::SUCCESS; 

  }
  //__________________________________________________________________________
  StatusCode LArHV2Ntuple::execute()
  {
    //.............................................
    
  ATH_MSG_DEBUG ( "LArHV2Ntuple execute()" );

  const LArHVManager *manager = NULL;
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
    const EMBHVManager* hvManager_EMB=manager->getEMBHVManager();
    for (unsigned int iSide=hvManager_EMB->beginSideIndex();iSide<hvManager_EMB->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMB->beginPhiIndex();iPhi<hvManager_EMB->endPhiIndex();iPhi++) {
        for (unsigned int iSector=hvManager_EMB->beginSectorIndex();iSector<hvManager_EMB->endSectorIndex();iSector++) {
          for (unsigned int iEta=hvManager_EMB->beginEtaIndex();iEta<hvManager_EMB->endEtaIndex();iEta++) { //0 to 7
            EMBHVModuleConstLink hvMod = hvManager_EMB->getHVModule(iSide,iEta,iPhi,iSector);
            float eta=0.5*(hvMod->getEtaMin()+hvMod->getEtaMax());
            for (unsigned int ielec=0;ielec<32;ielec++) { //use hvMod->getNumElectrodes when bug is corrected
              EMBHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);
              for (unsigned int iGap=0;iGap<2;iGap++) { // EMB : 2, TRY TO FIND AUTOMATICALLY NB OF GAPS
                float hv=electrode->voltage(iGap);
                float current = electrode->current(iGap);
                float phi = electrode->getPhi();

                m_bec=0;
                m_isPresampler=0;
                m_eta=eta;
                m_phi=phi;
                m_electrode = ielec;
                m_gap = iGap;
                m_hv = hv;
                m_current= current;
                m_hvline = electrode->hvLineNo(iGap);

                m_tree->Fill();

              } //end for iGap
            }
          }
        }
      }
    }

    const EMBPresamplerHVManager* hvManager_EMBPS=manager->getEMBPresamplerHVManager();
    for (unsigned int iSide=hvManager_EMBPS->beginSideIndex();iSide<hvManager_EMBPS->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMBPS->beginPhiIndex();iPhi<hvManager_EMBPS->endPhiIndex();iPhi++) {
         for (unsigned int iEta=hvManager_EMBPS->beginEtaIndex();iEta<hvManager_EMBPS->endEtaIndex();iEta++) { //0 to 7
            EMBPresamplerHVModuleConstLink hvMod = hvManager_EMBPS->getHVModule(iSide,iEta,iPhi);
            for (int iGap=0;iGap<2;iGap++) {
             float hv = hvMod->voltage(iGap);
             float current =hvMod->current(iGap);
             float eta = 0.5*(hvMod->getEtaMin()+hvMod->getEtaMax()); 
             float phi= 0.5*(hvMod->getPhiMin()+hvMod->getPhiMax());

             m_bec=0;
             m_isPresampler=1;
             m_eta=eta;
             m_phi=phi;
             m_electrode = 0;
             m_gap = iGap;
             m_hv = hv; 
             m_current= current;
             m_hvline = hvMod->hvLineNo(iGap);

             m_tree->Fill();

            } //end for iGap
         }
      }
    }

    const EMECPresamplerHVManager* hvManager_EMECPS=manager->getEMECPresamplerHVManager();
    for (unsigned int iSide=hvManager_EMECPS->beginSideIndex();iSide<hvManager_EMECPS->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMECPS->beginPhiIndex();iPhi<hvManager_EMECPS->endPhiIndex();iPhi++) {
            EMECPresamplerHVModuleConstLink hvMod = hvManager_EMECPS->getHVModule(iSide,iPhi);
            for (int iGap=0;iGap<2;iGap++) {
             float hv = hvMod->voltage(iGap);
             float current =hvMod->current(iGap);
             float eta = 0.5*(hvMod->getEtaMin()+hvMod->getEtaMax());
             float phi=0.5*(hvMod->getPhiMin()+hvMod->getPhiMax());

             m_bec=1;
             m_isPresampler=1;
             m_eta=eta;
             m_phi=phi;
             m_electrode = 0;
             m_gap = iGap;
             m_hv = hv;   
             m_current= current;
             m_hvline = hvMod->hvLineNo(iGap);

             m_tree->Fill();

            } //end for iGap
      }
    }



    const EMECHVManager* hvManager_EMEC=manager->getEMECHVManager(EMECHVModule::OUTER);
    for (unsigned int iSide=hvManager_EMEC->beginSideIndex();iSide<hvManager_EMEC->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC->beginPhiIndex();iPhi<hvManager_EMEC->endPhiIndex();iPhi++) {
        for (unsigned int iSector=hvManager_EMEC->beginSectorIndex();iSector<hvManager_EMEC->endSectorIndex();iSector++) {
          for (unsigned int iEta=hvManager_EMEC->beginEtaIndex();iEta<hvManager_EMEC->endEtaIndex();iEta++) {
            EMECHVModuleConstLink hvMod=hvManager_EMEC->getHVModule(iSide,iEta,iPhi,iSector);
            float eta=0.5*(hvMod->getEtaMin()+hvMod->getEtaMax());
            for (unsigned int ielec=0;ielec<hvMod->getNumElectrodes();ielec++) { //use hvMod->getNumElectrodes when bug is corrected
              EMECHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);
              for (unsigned int iGap=0;iGap<2;iGap++) { //EMEC : 2 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
                float hv=electrode->voltage(iGap);
                float current = electrode->current(iGap);
                float phi = electrode->getPhi();

                m_bec=1;
                m_isPresampler=0;
                m_eta=eta;
                m_phi=phi;
                m_electrode = ielec;
                m_gap = iGap;
                m_hv = hv;
                m_current= current;
               m_hvline = electrode->hvLineNo(iGap);

                m_tree->Fill();

              } //end for iGap
            }
          }
        }
      }
    }

    hvManager_EMEC=manager->getEMECHVManager(EMECHVModule::INNER);
    for (unsigned int iSide=hvManager_EMEC->beginSideIndex();iSide<hvManager_EMEC->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC->beginPhiIndex();iPhi<hvManager_EMEC->endPhiIndex();iPhi++) {
        for (unsigned int iSector=hvManager_EMEC->beginSectorIndex();iSector<hvManager_EMEC->endSectorIndex();iSector++) {
          for (unsigned int iEta=hvManager_EMEC->beginEtaIndex();iEta<hvManager_EMEC->endEtaIndex();iEta++) {
            EMECHVModuleConstLink hvMod=hvManager_EMEC->getHVModule(iSide,iEta,iPhi,iSector);
            float eta=0.5*(hvMod->getEtaMin()+hvMod->getEtaMax());
            for (unsigned int ielec=0;ielec<hvMod->getNumElectrodes();ielec++) { //use hvMod->getNumElectrodes when bug is corrected
              EMECHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);
              for (unsigned int iGap=0;iGap<2;iGap++) { //EMEC : 2 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
                float hv=electrode->voltage(iGap);
                float current = electrode->current(iGap);
                float phi = electrode->getPhi();
            
                m_bec=2;
                m_isPresampler=0;
                m_eta=eta;
                m_phi=phi;
                m_electrode = ielec;
                m_gap = iGap;
                m_hv = hv;
                m_current= current;
                m_hvline = electrode->hvLineNo(iGap);

                m_tree->Fill();

              } //end for iGap
            }
          }
        }
      }
    }

    const HECHVManager* hvManager_HEC=manager->getHECHVManager();
    float etamax_layer[4]={3.3,3.1,3.1,3.3};
    float etamin_layer[4]={1.5,1.5,1.6,1.7};

    
    for (unsigned int iSide=hvManager_HEC->beginSideIndex();iSide<hvManager_HEC->endSideIndex();iSide++) { // loop over HV modules      
      for (unsigned int iPhi=hvManager_HEC->beginPhiIndex();iPhi<hvManager_HEC->endPhiIndex();iPhi++) {
        for (unsigned int iSampling=hvManager_HEC->beginSamplingIndex();iSampling<hvManager_HEC->endSamplingIndex();iSampling++) {
          float eta_min,eta_max;
          if (iSide==1) {
           eta_min = etamin_layer[iSampling];
           eta_max = etamax_layer[iSampling];
          } else {
           eta_min = -1.*etamax_layer[iSampling];
           eta_max = -1.*etamin_layer[iSampling];
         }
         float eta = 0.5*(eta_min+eta_max);
         HECHVModuleConstLink hvMod = hvManager_HEC->getHVModule(iSide,iPhi,iSampling);
         float phi = 0.5*(hvMod->getPhiMin()+hvMod->getPhiMax());

         for (unsigned int iGap=0;iGap<hvMod->getNumSubgaps();iGap++) {//HEC : 4 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
            HECHVSubgapConstLink subgap=hvMod->getSubgap(iGap);
            float hv = subgap->voltage();
            float current = subgap->current();
            m_bec = 10+iSampling;
            m_isPresampler=0;
            m_eta=eta;
            m_phi=phi;
            m_electrode = 0;
            m_gap = iGap;
            m_hv=hv;
            m_current=current;
            m_hvline = subgap->hvLineNo();
            m_tree->Fill();
         }// end for iGap
       }
     }
   } 

  }

  return StatusCode::SUCCESS;
 }

