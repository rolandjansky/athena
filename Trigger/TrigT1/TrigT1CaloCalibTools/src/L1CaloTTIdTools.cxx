/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloTTIdTools.h"
#include <cmath>
namespace LVL1{

  const double L1CaloTTIdTools::BASEDETA = 0.1;
  const double L1CaloTTIdTools::BASEDPHI = 0.098175;
  const double L1CaloTTIdTools::FCALDETA = 0.425;

  const double L1CaloTTIdTools::ETAMAXREGION0 = 2.5;
  const double L1CaloTTIdTools::ETAMAXREGION1 = 3.1;
  const double L1CaloTTIdTools::ETAMAXREGION2 = 3.2;
  const double L1CaloTTIdTools::ETAMAXREGION3 = 4.9;

  const double L1CaloTTIdTools::ETAMIN = -4.9;
  const double L1CaloTTIdTools::PHIMIN = 0.;

  L1CaloTTIdTools::L1CaloTTIdTools( const std::string& name) : 
    asg::AsgTool( name ),
    m_l1ttonlineHelper(0),
    m_l1ttofflineHelper(0),
    m_bInitialized(false)
  {
  }

  StatusCode L1CaloTTIdTools::initialize() {
      
      ATH_MSG_INFO("Initializing LVL1::L1CaloTTIdTools");

      StatusCode sc ;

      const TTOnlineID* l1ttonline_id;
      sc = detStore()->retrieve(l1ttonline_id, "TTOnlineID");
      if(sc.isFailure()){
        ATH_MSG_ERROR("Could not get TTOnlineID helper !");
        return sc;
      } else {
          m_l1ttonlineHelper = l1ttonline_id;
          ATH_MSG_DEBUG(" Found the TTOnlineID helper. ");
      }

      const CaloLVL1_ID* l1ttoffline_id;
      sc = detStore()->retrieve(l1ttoffline_id, "CaloLVL1_ID");
      if(sc.isFailure()) {
        ATH_MSG_DEBUG("Could not get CaloLVL1_ID helper !");
        return sc;
      } else {
          m_l1ttofflineHelper = l1ttoffline_id;
          ATH_MSG_DEBUG(" Found the CaloLVL1_ID helper. ");
      }

      m_bInitialized = true;
      return sc;
  }

  StatusCode L1CaloTTIdTools::finalize() {
      return StatusCode::SUCCESS;
  }


  // return 1 for z>0 and 0 for z<0
  int L1CaloTTIdTools::side(const HWIdentifier& id) {
      int crate = m_l1ttonlineHelper->crate(id);
      if(crate==0 || crate==2 || crate==4 || crate==6) return 1;
      else if(crate==1 || crate==3 || crate==5 || crate==7) return 0;
      else return 999;
  }

  int L1CaloTTIdTools::sampling(const HWIdentifier& id) {
      int crate = m_l1ttonlineHelper->crate(id);
      if(crate==0 || crate==1 || crate==2 || crate==3) return 0; //EM -> 0
      else if(crate==6 || crate ==7 ) return 1; //HAd -> 1
      else if (crate==4 || crate ==5) {
          int module = m_l1ttonlineHelper->module(id);
          if (module==5) return 0; //EM
          else return 1; //HAD
      } else return 999;
  }

  int L1CaloTTIdTools::region(const HWIdentifier& id) {
      int ppm_type = PPM_type(id);

      if ( ppm_type<=6 ) {
          return 0;
      } else if ( ppm_type==7 && this->side(id)==1 ) {

          int submodule = m_l1ttonlineHelper->submodule(id);
          int normalized_submodule_number = submodule%4;

          // Module 1 & 2 -> region 1
          if( normalized_submodule_number==1 || normalized_submodule_number ==2) return 1;

          // Module 0 & 3
          else {
              int channel = m_l1ttonlineHelper->channel(id);
              // channel 2 of modules 0&3 ->region 1
              if( channel==2 ) return 1;
              // channel 1&2 of modules 1&4 ->region 0
              else if ( channel==0 || channel ==1 ) return 0;
              else return -1;
          }
      } else if ( ppm_type==7 && this->side(id)==0 ) {

          int submodule = m_l1ttonlineHelper->submodule(id);
          int normalized_submodule_number = submodule%4;

          // Module 0 & 3 -> region 1
          if( normalized_submodule_number==0 || normalized_submodule_number ==3) return 1;

          // Module 1 & 2
          else {
              int channel = m_l1ttonlineHelper->channel(id);
              // channel 0 of modules 1&2 ->region 1
              if( channel==0 ) return 1;
              // channel 2&3 of modules 1&2 ->region 0
              else if ( channel==2 || channel ==3 ) return 0;
              else return -1;
          }
      } else if ( ppm_type==8 && this->side(id)==1 ) {
          int channel = m_l1ttonlineHelper->channel(id);
          // channel 0 -> region 1
          if(channel==0) return 1;
          // channel 3 ->region 2
          else if(channel==2) return 2;
          else return -1;
      } else if ( ppm_type==8 && this->side(id)==0 ) {
          int channel = m_l1ttonlineHelper->channel(id);
          // channel 2 -> region 1
          if(channel==2) return 1;
          // channel 3 ->region 2
          else if(channel==0) return 2;
          else return -1;
      } else if ( ppm_type==9 ) {
          return 3;
      } else return -1;
  }

  // return PPM type from 1 to 9
  // PPM 1 to 6 are "standard"
  // The cabling of PPMs 7, 8 & 9 is different
  int L1CaloTTIdTools::PPM_type(const HWIdentifier& id) {
      int crate = m_l1ttonlineHelper->crate(id);
      int module = m_l1ttonlineHelper->module(id);
      if(crate==0 || crate==6 ) return (module-5)%4+1;
      if(crate==1 || crate==7 ) return 4-(module-5)%4;
      if(crate==2 || crate==4 ) {
          if(module==5 || module==13) return 9;
          else return (module-6)%4+5;
      }
      if(crate==3 || crate==5 ) {
          if(module==5 || module==13) return 9;
          else return 8-(module-5)%4;
      }
      return -1;
  }

  int L1CaloTTIdTools::eta(const HWIdentifier& id) {
      int region = this->region(id);

      //int pos_neg   = m_l1ttonlineHelper->pos_neg(id);
      int pos_neg = this->side(id);
      //int module    = m_l1ttonlineHelper->module(id);
      int submodule = m_l1ttonlineHelper->submodule(id);
      int channel   = m_l1ttonlineHelper->channel(id);

      int ppm_type = PPM_type(id);

      //-- REGION 0 --//
      if(region==0) {
          if(ppm_type==7) return 24;
          else {
              int module_eta_offset = 0;
              int submodule_eta_offset = 0;
              int channel_eta_offset = 0;

              module_eta_offset = (ppm_type-1)*4;

              if(pos_neg==1) {
                  if( (submodule%4)==0 || (submodule%4)==3 ) submodule_eta_offset = 0;
                  else submodule_eta_offset = 2;
                  channel_eta_offset = channel/2;
              } else {
                  if( (submodule%4)==0 || (submodule%4)==3 ) submodule_eta_offset = 2;
                  else submodule_eta_offset = 0;
                  channel_eta_offset = 1-channel/2;
              }
              return module_eta_offset + submodule_eta_offset + channel_eta_offset;
          }

      //-- REGION 1 --//
      } else if (region==1) {
          if(ppm_type==8) return 2;
          else {
              if(pos_neg==1) {
                  if(submodule%4==1 || submodule%4 ==2) return 1;
                  else return 0;
              } else {
                  if(submodule%4==0 || submodule%4 ==3) return 1;
                  else return 0;
              }
          }

      //-- REGION 2 --//
      } else if (region==2) {
          return 0;

      //-- REGION 3 --//
      } else if (region==3) {
          //int em_had = m_l1ttonlineHelper->em_had(id);
          int em_had = this->sampling(id);
          if(pos_neg==1) {
              if(em_had==0) { //Em
                      if(channel%4==0) return 0;
                  else if(channel%4==1) return 3;
                  else if(channel%4==2) return 1;
                  else if(channel%4==3) return 2;
                  else return -1;
              } else { //Had
                      // these fcal had channels have the same eta position, but belong to different layers (FCAL_2&FCAL_3)
  //                     if(channel%4==0 || channel%4==2) return 0;
  //                else if(channel%4==1 || channel%4==3) return 1;
  //                else return -1;

                  //the following assignation to eta indexes is FALSE for FCAL3 channels (3&1),
                  //but necessary to match with the way offline Ids are designed

          // this mapping is not correct in regard of the one used in the bytestream decoder
  //                     if(channel%4==0) return 0;
  //                else if(channel%4==1) return 3;
  //                else if(channel%4==2) return 2;
  //                else if(channel%4==3) return 1;
  //                else return -1;

          // This mapping follow the BS decoder channel ordering for FCal
                      if(channel%4==0) return 0;
                  else if(channel%4==1) return 3;
                  else if(channel%4==2) return 1;
                  else if(channel%4==3) return 2;
                  else return -1;

              }
          } else {
              if(em_had==0) { //Em
                      if(channel%4==0) return 3;
                  else if(channel%4==1) return 0;
                  else if(channel%4==2) return 2;
                  else if(channel%4==3) return 1;
                  else return -1;
              } else { //Had
                      // these fcal had channels have the same eta position, but belong to different layers (FCAL_2&FCAL_3)
  //                     if(channel%4==0 || channel%4==2) return 1;
  //                else if(channel%4==1 || channel%4==3) return 0;
  //                else return -1;

                  //the following assignation to eta indexes is FALSE for FCAL3 channels (3&1),
                  //but necessary to match with the way offline Ids are designed

                  // this mapping is not correct in regard of the one used in the bytestream decoder
  //                     if(channel%4==0) return 1;
  //                else if(channel%4==1) return 2;
  //                else if(channel%4==2) return 3;
  //                else if(channel%4==3) return 0;
  //                else return -1;

          // This mapping follow the BS decoder channel ordering for FCal
                      if(channel%4==0) return 3;
                  else if(channel%4==1) return 0;
                  else if(channel%4==2) return 2;
                  else if(channel%4==3) return 1;
                  else return -1;


              }
          }
      }
      return -1;
  }

  int L1CaloTTIdTools::phi(const HWIdentifier& id) {
      int region = this->region(id);

      //int crate = m_l1ttonlineHelper->crate(id);
      int module = m_l1ttonlineHelper->module(id);
      int submodule = m_l1ttonlineHelper->submodule(id);
      int channel = m_l1ttonlineHelper->channel(id);

      int ppm_type = PPM_type(id);

      int module_phi_offset = 0;
      int submodule_phi_offset = 0;
      int channel_phi_offset = 0;


      //-- REGION 0 --//
      if(region==0) {
          module_phi_offset = int((module-5)/4)*16;
          submodule_phi_offset = int(submodule/2)*2;
          channel_phi_offset = channel%2;

          return module_phi_offset + submodule_phi_offset + channel_phi_offset;

      //-- REGION 1 --//
      } else if (region==1) {
          if(ppm_type==7) {
              module_phi_offset = int((module-5)/4)*8;
              submodule_phi_offset = int(submodule/2)*1;
              channel_phi_offset = 0;

          } else if(ppm_type==8) {
              if(module==9)  module_phi_offset = 0;
              if(module==17) module_phi_offset = 16;
              submodule_phi_offset = submodule;
              channel_phi_offset = 0;
          } else return -1;

          return module_phi_offset + submodule_phi_offset + channel_phi_offset;

      //-- REGION 2 --//
      } else if (region==2) {
          if(module==9)  module_phi_offset = 0;
          if(module==17) module_phi_offset = 16;
          submodule_phi_offset = submodule;
          channel_phi_offset = 0;

          return module_phi_offset + submodule_phi_offset + channel_phi_offset;

      //-- REGION 3 --//
      } else if (region==3) {
          module_phi_offset = 0;
          submodule_phi_offset = submodule;
          channel_phi_offset = 0;

          return module_phi_offset + submodule_phi_offset + channel_phi_offset;
      }
      return -1;
  }

  // for had fcal (PPM9), 0== FCAL2, 1== FCAL3
  int L1CaloTTIdTools::layer(const HWIdentifier& id) {
      int region = this->region(id);
      int channel = m_l1ttonlineHelper->channel(id);

      if(region==3 && this->sampling(id)==1 ) {
              if(channel%4== 0 || channel%4==3) return 0; // FCAL_2
          else if(channel%4== 2 || channel%4==1) return 1; // FCAL_3
          else return -1;
      } else return -1;
  }

  int L1CaloTTIdTools::barrel_endcap_fcal(const HWIdentifier& id) {
      int crate = m_l1ttonlineHelper->crate(id);

      if(crate==0 || crate==1 || crate==6 || crate==7) return 0; // barrel
      else if(crate==2 || crate==3) return 1; //endcap
      else if(crate==4 || crate==5) {
          int ppm_type = PPM_type(id);
          if(ppm_type == 9) return 2; // fcal
          else if (ppm_type>=5 && ppm_type<=8) return 1; //endcap
          else return 999;
      } else return 999;
  }

  int L1CaloTTIdTools::module_type(const HWIdentifier& id) {
      int crate  = m_l1ttonlineHelper->crate(id);
      int module = m_l1ttonlineHelper->module(id);

      if(crate==0) {
          if(module>=5 && module<=20) return 1; // PPM
          else return 999;

      } else if(crate==1) {
          if(module>=5 && module<=20) return 1; // PPM
          else return 999;

      } else if(crate==2) {
          if( (module>=6 && module<=20) && module!=13) return 1; // PPM
          else return 999;

      } else if(crate==3) {
          if( (module>=6 && module<=20) && module!=13) return 1; // PPM
          else return 999;

      } else if(crate==4) {
          if(module>=5 && module<=20) return 1; // PPM
          else return 999;

      } else if(crate==5) {
          if(module>=5 && module<=20) return 1; // PPM
          else return 999;

      } else if(crate==6) {
          if(module>=5 && module<=20) return 1; // PPM
          else return 999;

      } else if(crate==7) {
          if(module>=5 && module<=20) return 1; // PPM
          else return 999;

      } else if(crate==8) {
          if(module>=5 && module<=18) return 2; // CPM
          else return 999;

      } else if(crate==9) {
          if(module>=5 && module<=18) return 2; // CPM
          else return 999;

      } else if(crate==10) {
          if(module>=5 && module<=18) return 2; // CPM
          else return 999;

      } else if(crate==11) {
          if(module>=5 && module<=18) return 2; // CPM
          else return 999;

      } else if(crate==12) {
          if(module>=4 && module<=19) return 3; // JEM
          else return 999;

      } else if(crate==13) {
          if(module>=4 && module<=19) return 3; // JEM
          else return 999;

      } else return 999;
  }

  double L1CaloTTIdTools::IDeta(const Identifier& id) const {
    int region = m_l1ttofflineHelper->region(id);
    int ieta = m_l1ttofflineHelper->eta(id);
    int sign = m_l1ttofflineHelper->pos_neg_z(id);

    double gran[4] = {0.1, 0.2, 0.1, 0.425};
    double offset[4] = {0., 2.5, 3.1, 3.2};
    double eta;

    if (region>=0 && region<=3) {
      eta = sign* ( ( (ieta+0.5) * gran[region] ) + offset[region] );
    }
    else {
      eta = 0.;
    }
    return eta;
  }

  double L1CaloTTIdTools::IDphi(const Identifier& id) const {
    Identifier regId = m_l1ttofflineHelper->region_id(id);

    double phiMax = m_l1ttofflineHelper->phi_max(regId);
    int iphi = m_l1ttofflineHelper->phi(id);
    double phi = (iphi+0.5)*2.*M_PI/(phiMax+1.);
    return phi;
  }

  LVL1::InternalTriggerTower* L1CaloTTIdTools::findInternalTriggerTower(std::map<unsigned int, LVL1::InternalTriggerTower*>* pIntTTContainer, const Identifier& towerId, bool bCreate) {
      // Create tower key from towerId
      double tower_eta = this->IDeta(towerId);
      double tower_phi = this->IDphi(towerId);
      LVL1::TriggerTowerKey towerKey;
      unsigned int key = towerKey.ttKey(tower_phi,tower_eta);
      double tt_phi = towerKey.phi();
      double tt_eta = towerKey.eta();

      return this->findInternalTriggerTower(pIntTTContainer, tt_phi, tt_eta, key, bCreate);
  }

  LVL1::InternalTriggerTower* L1CaloTTIdTools::findInternalTriggerTower(std::map<unsigned int, LVL1::InternalTriggerTower*>* pIntTTContainer, double tt_phi, double tt_eta, unsigned int key, bool bCreate) {
      std::map<unsigned int, LVL1::InternalTriggerTower*>::iterator it = pIntTTContainer->find( key );
      LVL1::InternalTriggerTower* TT = 0;
      if (it == pIntTTContainer->end()){
          if(bCreate) {
              // no TT yet. Create it!
              TT = new LVL1::InternalTriggerTower(tt_phi,tt_eta, key);
              pIntTTContainer->insert(std::map<unsigned int, LVL1::InternalTriggerTower*>::value_type(key,TT)); //and put it in the map.
          }
      }else{
          TT = (it->second);
      } // end else
      return TT;
  }

  double L1CaloTTIdTools::etaWidth(const double eta) const {
    double abseta = fabs(eta);
      if(abseta<ETAMAXREGION0) {
          return BASEDETA;

      } else if(abseta<ETAMAXREGION1) {
          return BASEDETA*2.;

      } else if (abseta<ETAMAXREGION2) {
          return BASEDETA;

      } else {
          return FCALDETA;
      }
  }

  double L1CaloTTIdTools::phiWidth(const double eta) const {
    double abseta = fabs(eta);
      if(abseta<ETAMAXREGION0) {
          return BASEDPHI;

      } else if(abseta<ETAMAXREGION1) {
          return BASEDPHI*2.;

      } else if (abseta<ETAMAXREGION2) {
          return BASEDPHI*2.;

      } else {
          return BASEDPHI*4.;
      }
  }

  int L1CaloTTIdTools::pos_neg_z(const double eta) const {
    return (eta>=0) ? +1 : -1;
  }

  int L1CaloTTIdTools::etaIndex(const double eta) const {
      int etacenter = -1;
      double deta = this->etaWidth(eta);
      double abseta = fabs(eta);

      if(abseta<ETAMAXREGION0) {
          etacenter = (int) floor(abseta/deta);

      } else if(abseta<ETAMAXREGION1) {
          etacenter = (int) (floor((abseta-ETAMAXREGION0)/deta));

      } else if (abseta<ETAMAXREGION2) {
          etacenter = (int) (floor((abseta-ETAMAXREGION1)/deta));

      } else if (abseta<ETAMAXREGION3){
          etacenter = (int) (floor((abseta-ETAMAXREGION2)/deta));
      }
      return etacenter;
  }

  int L1CaloTTIdTools::phiIndex(const double eta, const double phi) const {
      double dphi = this->phiWidth(eta);
      int phicenter = (int) floor(phi/dphi);
      return phicenter;
  }

  int L1CaloTTIdTools::regionIndex(const double eta) const {
      int region = -1;
    double abseta = fabs(eta);

      if(abseta<ETAMAXREGION0) {
          region = 0;

      } else if(abseta<ETAMAXREGION1) {
          region = 1;

      } else if (abseta<ETAMAXREGION2) {
          region = 2;

      } else if (abseta<ETAMAXREGION3) {
          region = 3;

      }
      return region;
  }
} // end of namespace

