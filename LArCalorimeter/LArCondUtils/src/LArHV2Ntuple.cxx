/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV2Ntuple.h"
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
#include "LArHV/FCALHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingService.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

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
    m_caloId(0), m_onlId(0),
    m_cabling("LArCablingService")
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
     ATH_CHECK( m_cabling.retrieve() );
     ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );
     std::vector<Identifier>::const_iterator cell_b=m_caloId->cell_begin();
     std::vector<Identifier>::const_iterator cell_e=m_caloId->cell_end();
     for(;cell_b!=cell_e; ++cell_b) {
         if(m_caloId->is_tile(*cell_b)) continue;
         HWIdentifier onlid = m_cabling->createSignalChannelID(*cell_b);
         std::vector<int> hvlines = GetHVLines(*cell_b);
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

   const FCALHVManager *hvManager_FCAL=manager->getFCALHVManager();
   for (unsigned int iSide=hvManager_FCAL->beginSideIndex();iSide<hvManager_FCAL->endSideIndex();iSide++) { // loop over HV modules
       float eta_min=3.1,eta_max=4.9;
       if (iSide==0) { eta_min=-4.9; eta_max=-3.1; }

       float eta = 0.5*(eta_min+eta_max);
       for (unsigned int iSampling=hvManager_FCAL->beginSamplingIndex();iSampling<hvManager_FCAL->endSamplingIndex();iSampling++) {
            for (unsigned int iSector=hvManager_FCAL->beginSectorIndex(iSampling);iSector<hvManager_FCAL->endSectorIndex(iSampling);iSector++) {
 
                 FCALHVModuleConstLink hvMod = hvManager_FCAL->getHVModule(iSide,iSector,iSampling);
                 //std::cout << " FCAL HVModule side,sampling,sector " << iSide << " " << iSampling << " " << iSector << std::endl;
                 //std::cout << "   HV nominal " << HVnominal << std::endl;
  
                 float dphi=CaloPhiRange::twopi()/16;
                 if (iSampling==1) dphi=CaloPhiRange::twopi()/8.;
                 if (iSampling==2) dphi=CaloPhiRange::twopi()/4.;
                 float phi_min = ((float)(iSector))*dphi;
                 phi_min =   CaloPhiRange::fix(phi_min);
                 float phi_max = CaloPhiRange::fix(dphi+phi_min);
                 float phi = 0.5*(phi_min+phi_max);
         
                 for (unsigned int iLine=0;iLine<hvMod->getNumHVLines();iLine++) {
                     FCALHVLineConstLink hvline = hvMod->getHVLine(iLine);
                     float hv = hvline->voltage();
                     float current = hvline->current();
                     m_bec = 14+iSampling;
                     m_isPresampler=0;
                     m_eta=eta;
                     m_phi=phi;
                     m_electrode = iSector;
                     m_gap = iLine;
                     m_hv=hv;
                     m_current=current;
                     m_hvline = hvline->hvLineNo();
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

  }//LArHVManager

  return StatusCode::SUCCESS;
 }

std::vector<int> LArHV2Ntuple::GetHVLines(const Identifier& id) {

   std::set<int> hv;
 
   // LAr EMB
   if (m_caloId->is_em(id) && m_caloId->sampling(id)>0) {
     if (abs(m_caloId->em_idHelper()->barrel_ec(id))==1) {
       const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
       if (!embElement) std::abort();
       const EMBCellConstLink cell = embElement->getEMBCell();
       unsigned int nelec = cell->getNumElectrodes();
       for (unsigned int i=0;i<nelec;i++) {
         const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
         for (unsigned int igap=0;igap<2;igap++) hv.insert(electrode->hvLineNo(igap));
       }
     } else { // LAr EMEC
       const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
       if (!emecElement) std::abort();
       const EMECCellConstLink cell = emecElement->getEMECCell();
       unsigned int nelec = cell->getNumElectrodes();
       for (unsigned int i=0;i<nelec;i++) {
         const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
         for (unsigned int igap=0;igap<2;igap++) hv.insert(electrode->hvLineNo(igap));
       }
     }
   } else if (m_caloId->is_hec(id)) { // LAr HEC
     const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id));
     if (!hecElement) std::abort();
     const HECCellConstLink cell = hecElement->getHECCell();
     unsigned int nsubgaps = cell->getNumSubgaps();
     for (unsigned int igap=0;igap<nsubgaps;igap++) {
       const HECHVSubgapConstLink subgap = cell->getSubgap(igap);
       hv.insert(subgap->hvLineNo());
     }
   } else if (m_caloId->is_fcal(id)) { // LAr FCAL
     const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id));
     if (!fcalElement) std::abort();
     const FCALTile* tile = fcalElement->getFCALTile();
     unsigned int nlines = tile->getNumHVLines();
     unsigned int nlines_found=0;
     for (unsigned int i=0;i<nlines;i++) {
       const FCALHVLineConstLink line = tile->getHVLine(i);
       if (line) nlines_found++;
     }
     if ( nlines_found>0 ) {
       for (unsigned int i=0;i<nlines;i++) {
         const FCALHVLineConstLink line = tile->getHVLine(i);
         if (!line) continue;
         hv.insert(line->hvLineNo());
       }
     }
   } else if (m_caloId->is_em(id) && m_caloId->sampling(id)==0) { // Presamplers
     if (abs(m_caloId->em_idHelper()->barrel_ec(id))==1) {
       const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
       if (!embElement) std::abort();
       const EMBCellConstLink cell = embElement->getEMBCell();
       const EMBPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule();
       for (unsigned int igap=0;igap<2;igap++) hv.insert(hvmodule->hvLineNo(igap));
     } else {
       const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
       if (!emecElement) std::abort();
       const EMECCellConstLink cell = emecElement->getEMECCell();
       const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();
       for (unsigned int igap=0;igap<2;igap++) hv.insert(hvmodule->hvLineNo(igap));
     }
   }
 
   std::vector<int> hvlines;
   for (std::set<int>::iterator i=hv.begin();i!=hv.end();++i) hvlines.push_back(*i);
   return hvlines;
 }
