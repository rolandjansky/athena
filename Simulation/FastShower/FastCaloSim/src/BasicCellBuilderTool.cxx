/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/BasicCellBuilderTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
 
#include "GaudiKernel/INTupleSvc.h"

#include "StoreGate/StoreGateSvc.h" 

#include <iostream> 
#include <cmath>

BasicCellBuilderTool::BasicCellBuilderTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AthAlgTool(type, name, parent),
   m_nEvent(0),
   m_caloDDM(0),
   m_caloCID(0),
   m_atlas_id(0)
{ 
  declareProperty("phi0_em",m_phi0_em);
  declareProperty("phi0_had",m_phi0_had);

  m_phi0_em=-1000;
  m_phi0_had=-1000;

  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) m_isCaloBarrel[i]=true;
  for(int i=CaloCell_ID_FCS::PreSamplerE;i<=CaloCell_ID_FCS::EME3;++i) m_isCaloBarrel[i]=false;
  m_isCaloBarrel[CaloCell_ID_FCS::TileGap3]=false;
  for(int i=CaloCell_ID_FCS::HEC0;i<=CaloCell_ID_FCS::HEC3;++i) m_isCaloBarrel[i]=false;
  for(int i=CaloCell_ID_FCS::FCAL0;i<=CaloCell_ID_FCS::FCAL2;++i) m_isCaloBarrel[i]=false;
}


BasicCellBuilderTool::~BasicCellBuilderTool()
{
  ATH_MSG_DEBUG("in destructor ");
}

void BasicCellBuilderTool::find_phi0()
{
  if(m_phi0_em==-1000) {
    ATH_MSG_DEBUG("Trying to find best phi0 of EM calo :");
    m_phi0_em=0;
    cellinfo_vec test_vec;
    const double dphi=2*M_PI/64;
    findCells(test_vec,0.1,0.2,-dphi,dphi,1);
    double bestphi0=10000;

    for(unsigned int i=0;i<test_vec.size();++i) {
      const CaloDetDescrElement* theDDE=test_vec[i].first;
      double phi0;

      phi0=theDDE->phi()-theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
      phi0=theDDE->phi()+theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
    }

    if(bestphi0!=10000) m_phi0_em=bestphi0;

    ATH_MSG_DEBUG("Best phi0 EM=" << bestphi0);
  }

  if(m_phi0_had==-1000) {
    ATH_MSG_DEBUG("Trying to find best phi0 of Tile calo :");
    m_phi0_had=0;
    cellinfo_vec test_vec;
    const double dphi=2*M_PI/64;
    findCells(test_vec,0.1,0.2,-dphi,dphi,3);
    double bestphi0=10000;

    for(unsigned int i=0;i<test_vec.size();++i) {
      const CaloDetDescrElement* theDDE=test_vec[i].first;
      double phi0;

      phi0=theDDE->phi()-theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
      phi0=theDDE->phi()+theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
    }

    if(bestphi0!=10000) m_phi0_had=bestphi0;

    ATH_MSG_DEBUG("Best phi0 HAD=" << bestphi0);
  }
}


StatusCode BasicCellBuilderTool::initialize()
{
  ATH_MSG_DEBUG("Accesssing CaloDetDescrManager");
  m_caloDDM = CaloDetDescrManager::instance() ;

  ATH_MSG_DEBUG("Accesssing CaloCellID");
  m_caloCID = m_caloDDM->getCaloCell_ID();

  // pointer to detector manager:
  m_atlas_id = (m_caloDDM->getCalo_Mgr())->getEM_ID();

  //StatusCode sc;
  /*
  //initialize the StoreGateSvc ptr
  sc = service("StoreGateSvc", m_storeGate); 
  if (StatusCode::SUCCESS!= sc )
  {
    return sc; 
  } 
  */

  // need tool service to get LArCablingService
  IToolSvc* toolSvc;
  if (service("ToolSvc", toolSvc).isFailure()) {
    ATH_MSG_ERROR("Tool Service not found "); 
    return StatusCode::FAILURE;
  }

/*
// retrive onlineID helper from detStore
  StoreGateSvc* detStore;
  if (service("DetectorStore", detStore).isFailure()) {
    log << MSG::ERROR   << "Unable to access DetectoreStore" << endmsg ;
    return StatusCode::FAILURE;
  }
*/  

  m_nEvent=0;

  return StatusCode::SUCCESS; 

}

/*
void BasicCellBuilderTool::init_map(cellinfo_map& map, int layer)
{
  ATH_MSG_DEBUG("Building map layer=" << layer);

  double eta_min=0;
  double eta_max=0;
  double phi_min=0;
  double phi_max=0;
  for(unsigned int i=0;i<map.neta();++i) {
    map.eta_range(i,eta_min,eta_max);
    for(unsigned int j=0;j<map.nphi();++j) {
      map.phi_range(j,phi_min,phi_max);
      findCells(map.vec(i,j),eta_min,eta_max,phi_min,phi_max,layer);
    }   
  }
}
*/

double BasicCellBuilderTool::deta(CaloCell_ID_FCS::CaloSample sample,double eta) const
{
  int side=0;
  if(eta>0) side=1;
  
  double mineta=m_min_eta_sample[side][sample];
  double maxeta=m_max_eta_sample[side][sample];
  
  if(eta<mineta) {
    return fabs(eta-mineta);
  } else if(eta>maxeta) {
    return fabs(eta-maxeta);
  } else {
    double d1=fabs(eta-mineta);
    double d2=fabs(eta-maxeta);
    if(d1<d2) return -d1;
     else return -d2;
  }
}

void BasicCellBuilderTool::minmaxeta(CaloCell_ID_FCS::CaloSample sample,double eta,double& mineta,double& maxeta) const 
{
  int side=0;
  if(eta>0) side=1;
  
  mineta=m_min_eta_sample[side][sample];
  maxeta=m_max_eta_sample[side][sample];
}

double BasicCellBuilderTool::rmid(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return m_rmid_map[side][sample].find_closest(eta)->second;
}

double BasicCellBuilderTool::zmid(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return m_zmid_map[side][sample].find_closest(eta)->second;
}

double BasicCellBuilderTool::rzmid(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  if(isCaloBarrel(sample)) return m_rmid_map[side][sample].find_closest(eta)->second;
   else                    return m_zmid_map[side][sample].find_closest(eta)->second;
}

double BasicCellBuilderTool::rent(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return m_rent_map[side][sample].find_closest(eta)->second;
}

double BasicCellBuilderTool::zent(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return m_zent_map[side][sample].find_closest(eta)->second;
}

double BasicCellBuilderTool::rzent(CaloCell_ID_FCS::CaloSample sample,double eta) const
{
  int side=0;
  if(eta>0) side=1;

  if(isCaloBarrel(sample)) return m_rent_map[side][sample].find_closest(eta)->second;
   else                    return m_zent_map[side][sample].find_closest(eta)->second;
}


void BasicCellBuilderTool::init_all_maps()
{
  ATH_MSG_INFO("Building all maps");

  cellinfo_map* curmap;
  cellinfo_map* curmap2;

  ATH_MSG_DEBUG("size all cells : " <<m_caloDDM->element_end()-m_caloDDM->element_begin() 
            <<  "; LAREM="          <<m_caloDDM->element_end(CaloCell_ID::LAREM)-m_caloDDM->element_begin(CaloCell_ID::LAREM)
            <<  "; TILE="           <<m_caloDDM->element_end(CaloCell_ID::TILE)-m_caloDDM->element_begin(CaloCell_ID::TILE)
            <<  "; LARHEC="         <<m_caloDDM->element_end(CaloCell_ID::LARHEC)-m_caloDDM->element_begin(CaloCell_ID::LARHEC)
            <<  "; LARFCAL="        <<m_caloDDM->element_end(CaloCell_ID::LARFCAL)-m_caloDDM->element_begin(CaloCell_ID::LARFCAL) );

/*
  int nokfcal=0;
  log << MSG::INFO <<  "Loop only fcal" << endmsg ;
  for(CaloDetDescrManager::calo_element_const_iterator calo_iter=m_caloDDM->element_begin(CaloCell_ID::LARFCAL);calo_iter<m_caloDDM->element_end(CaloCell_ID::LARFCAL);++calo_iter) {
    const CaloDetDescrElement* theDDE=*calo_iter;
    if(theDDE) {
      ++nokfcal;
      if(fabs(theDDE->eta()+4.95)<0.1) log << MSG::DEBUG <<  "init cell eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling() <<" calo="<<theDDE->getSubCalo()<< endmsg ;
    }  
  }
  log << MSG::INFO <<  "Loop only fcal finished nok="<<nokfcal << endmsg ;
*/
  int nok=0;
  int nvolOK=0;
  int naccept=0;
  bool doem_celllist_map=false;
  bool do_celllist_map=false;
  int emcl_neta=m_em_celllist_map.leta()-1;
  int emcl_nphi=m_em_celllist_map.lphi()-1;
  if(m_em_celllist_map.leta()>1 && m_em_celllist_map.lphi()>1) doem_celllist_map=true;
  if(m_celllist_maps[0].leta()>1 && m_celllist_maps[0].lphi()>1) do_celllist_map=true;

  FSmap< double , double > rz_map_eta [2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_rmid[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_zmid[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_rent[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_zent[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , int    > rz_map_n   [2][CaloCell_ID_FCS::MaxSample];


  for(int side=0;side<=1;++side) for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    m_min_eta_sample[side][sample]=+1000;
    m_max_eta_sample[side][sample]=-1000;
  }  


  for(CaloDetDescrManager::calo_element_const_iterator calo_iter=m_caloDDM->element_begin();calo_iter<m_caloDDM->element_end();++calo_iter) {
    const CaloDetDescrElement* theDDE=*calo_iter;
    if(theDDE) {
      ++nok;
/*
      CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();
      if( calo==CaloCell_ID::LARFCAL ) {
        log << MSG::DEBUG <<  "init cell eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling() <<" calo="<<theDDE->getSubCalo();
        log<<" deta="<<theDDE->deta()<<" dphi="<<theDDE->dphi()<<" dr="<<theDDE->dr()<<" vol="<<theDDE->volume() ;
        log<<" dx="<<theDDE->dx()<<" dy="<<theDDE->dy()<<" dz="<<theDDE->dz()<< endmsg ;
      }  
*/      
      CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();
      CaloCell_ID_FCS::CaloSample sample=(CaloCell_ID_FCS::CaloSample)theDDE->getSampling();

      int side=0;
      int sign_side=-1;
      double eta_raw=theDDE->eta_raw();
      if(eta_raw>0) {
        side=1;
        sign_side=+1;
      }
      double min_eta=theDDE->eta()-theDDE->deta()/2;
      double max_eta=theDDE->eta()+theDDE->deta()/2;
      if(min_eta<m_min_eta_sample[side][sample]) m_min_eta_sample[side][sample]=min_eta;
      if(max_eta>m_max_eta_sample[side][sample]) m_max_eta_sample[side][sample]=max_eta;
      
      if(rz_map_eta[side][sample].find(eta_raw)==rz_map_eta[side][sample].end()) {
        rz_map_eta [side][sample][eta_raw]=0;
        rz_map_rmid[side][sample][eta_raw]=0;
        rz_map_zmid[side][sample][eta_raw]=0;
        rz_map_rent[side][sample][eta_raw]=0;
        rz_map_zent[side][sample][eta_raw]=0;
        rz_map_n   [side][sample][eta_raw]=0;
      }
      rz_map_eta [side][sample][eta_raw]+=theDDE->eta();
      rz_map_rmid[side][sample][eta_raw]+=theDDE->r();
      rz_map_zmid[side][sample][eta_raw]+=theDDE->z();
      rz_map_rent[side][sample][eta_raw]+=theDDE->r()-theDDE->dr();
      rz_map_zent[side][sample][eta_raw]+=theDDE->z()-theDDE->dz()*sign_side;
      rz_map_n   [side][sample][eta_raw]++;
      
      if(do_celllist_map) {
        int cl_neta=m_celllist_maps[sample].leta()-1;
        int cl_nphi=m_celllist_maps[sample].lphi()-1;
        double ceta_min=theDDE->eta()-m_celllist_maps[sample].deta()*cl_neta;
        double ceta_max=theDDE->eta()+m_celllist_maps[sample].deta()*cl_neta;
        double cphi_min=theDDE->phi()-m_celllist_maps[sample].dphi()*cl_nphi;
        double cphi_max=theDDE->phi()+m_celllist_maps[sample].dphi()*cl_nphi;

        int iphi1=m_celllist_maps[sample].phi_to_index_cont(cphi_min);
        int iphi2=m_celllist_maps[sample].phi_to_index_cont(cphi_max);
        int ieta1=m_celllist_maps[sample].eta_to_index(ceta_min);
        int ieta2=m_celllist_maps[sample].eta_to_index(ceta_max);

        for(int ieta=ieta1;ieta<=ieta2;++ieta) {
          if(ieta<0)          continue;
          if((unsigned int)ieta>=m_celllist_maps[sample].neta()) continue;

          for(int s_iphi=iphi1;s_iphi<=iphi2;++s_iphi) {
            int iphi=s_iphi;
            if(iphi<0)           iphi+=m_celllist_maps[sample].nphi();
            if((unsigned int)iphi>=m_celllist_maps[sample].nphi()) iphi-=m_celllist_maps[sample].nphi(); 

            m_celllist_maps[sample].vec(ieta,iphi).push_back( cellinfo( theDDE,1),2);
          }
        }    

      }  

      if(theDDE->volume()>0) {
        ++nvolOK;
        //dont' fill PreSampler, EM1 and TileGap

        if(sample==CaloCell_ID_FCS::PreSamplerB) continue;
        if(sample==CaloCell_ID_FCS::PreSamplerE) continue;
        if(sample==CaloCell_ID_FCS::TileGap1) continue;
        if(sample==CaloCell_ID_FCS::TileGap2) continue;
        if(sample==CaloCell_ID_FCS::TileGap3) continue;

        ++naccept;
        
        if(doem_celllist_map && calo==CaloCell_ID::LAREM) {
          double ceta_min=theDDE->eta()-m_em_celllist_map.deta()*emcl_neta;
          double ceta_max=theDDE->eta()+m_em_celllist_map.deta()*emcl_neta;
          double cphi_min=theDDE->phi()-m_em_celllist_map.dphi()*emcl_nphi;
          double cphi_max=theDDE->phi()+m_em_celllist_map.dphi()*emcl_nphi;
          
          int iphi1=m_em_celllist_map.phi_to_index_cont(cphi_min);
          int iphi2=m_em_celllist_map.phi_to_index_cont(cphi_max);
          int ieta1=m_em_celllist_map.eta_to_index(ceta_min);
          int ieta2=m_em_celllist_map.eta_to_index(ceta_max);

          for(int ieta=ieta1;ieta<=ieta2;++ieta) {
            if(ieta<0)          continue;
            if((unsigned int)ieta>=m_em_celllist_map.neta()) continue;

            for(int s_iphi=iphi1;s_iphi<=iphi2;++s_iphi) {
              int iphi=s_iphi;
              if(iphi<0)           iphi+=m_em_celllist_map.nphi();
              if((unsigned int)iphi>=m_em_celllist_map.nphi()) iphi-=m_em_celllist_map.nphi(); 

              m_em_celllist_map.vec(ieta,iphi).push_back( cellinfo( theDDE,1),2);
            }
          }    
          
        }  

        curmap=0;
        curmap2=0;
        if(calo==CaloCell_ID::LAREM)   {
          curmap=&m_em_map;
          if(fabs(theDDE->eta())<2.9 && m_em_fine_map.neta()>0) curmap2=&m_em_fine_map;
/*          
          if(log.level()<=MSG::DEBUG) {
            if(theDDE->phi()>0.0 && theDDE->phi()<0.1 && theDDE->eta()>0) {
              log<<"init cell eta="<<theDDE->eta()<<" phi="<<theDDE->phi()<<" r="<<theDDE->r()<<" z="<<theDDE->z()<<" sample="<<theDDE->getSampling() <<" calo="<<theDDE->getSubCalo();
              log<<endmsg;
            }
          }  
*/          
        }  
        if(sample==CaloCell_ID_FCS::FCAL0) curmap=&m_em_map;

        if(calo==CaloCell_ID::LARHEC)  curmap=&m_had_map;
        if(calo==CaloCell_ID::TILE)    curmap=&m_had_map;
        if(sample==CaloCell_ID_FCS::FCAL1 || sample==CaloCell_ID_FCS::FCAL2) curmap=&m_had_map;

        if(curmap2) {
          init_cell(*curmap2,theDDE);
        }
        if(curmap) {
          if(sample!=CaloCell_ID_FCS::EMB1 && sample!=CaloCell_ID_FCS::EME1) init_cell(*curmap,theDDE);
        }
      } else {
//        log << MSG::WARNING <<  "volume=0 cell eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling() <<" calo="<<theDDE->getSubCalo()<< endmsg ;
      }
    } else {
//      log << MSG::ERROR <<  "empty caloDDM element"<< endmsg ;
    }  
  }
  for(int side=0;side<=1;++side) for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    if(rz_map_n[side][sample].size()>0) {
      for(FSmap< double , int    >::iterator iter=rz_map_n[side][sample].begin();iter!=rz_map_n[side][sample].end();++iter) {
        double eta_raw=iter->first;
        if(iter->second<1) {
          ATH_MSG_WARNING("rz-map for side="<<side<<" sample="<<sample<<" eta_raw="<<eta_raw<<" : #cells="<<iter->second<<" !!!");
        } else {
          double eta =rz_map_eta[side][sample][eta_raw]/iter->second;
          double rmid=rz_map_rmid[side][sample][eta_raw]/iter->second;
          double zmid=rz_map_zmid[side][sample][eta_raw]/iter->second;
          double rent=rz_map_rent[side][sample][eta_raw]/iter->second;
          double zent=rz_map_zent[side][sample][eta_raw]/iter->second;
          
          m_rmid_map[side][sample][eta]=rmid;
          m_zmid_map[side][sample][eta]=zmid;
          m_rent_map[side][sample][eta]=rent;
          m_zent_map[side][sample][eta]=zent;
        }
      }
      ATH_MSG_DEBUG("rz-map for side="<<side<<" sample="<<sample<<" #etas="<<m_rmid_map[side][sample].size());
    } else {
      ATH_MSG_WARNING("rz-map for side="<<side<<" sample="<<sample<<" is empty!!!");
    }
  }

  ATH_MSG_INFO("Building all maps finished : n(total)="<<nok<< " n(volume>0)="<<nvolOK<<" n(used)="<<naccept);
}

void BasicCellBuilderTool::init_volume(cellinfo_map& map)
{
  ATH_MSG_DEBUG("Normalizing volumes");
  for(unsigned int i=0;i<map.neta();++i) {
    for(unsigned int j=0;j<map.nphi();++j) {
      double_t vol=0;
      if( map.vec(i,j).size()==0 && fabs(map.index_to_eta_center(i))<4.9 ) {
        ATH_MSG_WARNING("empty map cell "<<map.name()<<": eta="<<map.index_to_eta_center(i)<<" phi="<<map.index_to_phi_center(j));
      }
      for(unsigned int k=0;k<map.vec(i,j).size();++k) {
        vol+=map.vec(i,j)[k].second;
      }
      if(vol!=map.vec(i,j).vol()) ATH_MSG_WARNING("internal volume not consistent");
      if(vol>0) {
        for(unsigned int k=0;k<map.vec(i,j).size();++k) {
          map.vec(i,j)[k].second/=vol;
        }
      }  
    }   
  }
  ATH_MSG_DEBUG("Normalizing volumes finished");
}


void BasicCellBuilderTool::init_cell(cellinfo_map& map,const CaloDetDescrElement* theDDE)
{
//  log << MSG::VERBOSE <<  "init cell "<<map.name()<<" eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling() << endmsg ;

  double c_phi_min_org=theDDE->phi()-theDDE->dphi()/2;
  double c_phi_max_org=theDDE->phi()+theDDE->dphi()/2;
  double c_eta_min=theDDE->eta()-theDDE->deta()/2;
  double c_eta_max=theDDE->eta()+theDDE->deta()/2;

  CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();
  if( calo==CaloCell_ID::LARFCAL ) {
    CLHEP::Hep3Vector pcorner[4]={ CLHEP::Hep3Vector(theDDE->x()+theDDE->dx()/2,theDDE->y()-theDDE->dy()/2,theDDE->z()),
                                   CLHEP::Hep3Vector(theDDE->x()-theDDE->dx()/2,theDDE->y()-theDDE->dy()/2,theDDE->z()),
                                   CLHEP::Hep3Vector(theDDE->x()+theDDE->dx()/2,theDDE->y()+theDDE->dy()/2,theDDE->z()),
                                   CLHEP::Hep3Vector(theDDE->x()-theDDE->dx()/2,theDDE->y()+theDDE->dy()/2,theDDE->z()) };
    for(int i=0;i<4;++i) {
      double phi=pcorner[i].phi();
      double eta=pcorner[i].eta();
      if(theDDE->phi()>+2 && phi<-2) phi+=2*M_PI;
      if(theDDE->phi()<-2 && phi>+2) phi-=2*M_PI;
      
      if(phi<c_phi_min_org) c_phi_min_org=phi;
      if(phi>c_phi_max_org) c_phi_max_org=phi;
      if(eta<c_eta_min) c_eta_min=eta;
      if(eta>c_eta_max) c_eta_max=eta;
  
//      log << MSG::DEBUG<<"corner "<<i<<" eta="<<eta<<" phi="<<phi;
//      log<<" ndeta="<<c_eta_max-c_eta_min<<" ndphi="<<c_phi_max_org-c_phi_min_org<< endmsg ;
    }
/*    
    if( (fabs(c_eta_max)>4.9 || fabs(c_eta_min)>4.9) && fabs(theDDE->phi())<0.5 ) log<<MSG::DEBUG;
     else log << MSG::VERBOSE;
    log<<  "init FCAL cell "<<map.name()<<" eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling() <<" calo="<<theDDE->getSubCalo();
    log<<" deta="<<theDDE->deta()<<" dphi="<<theDDE->dphi()<<" dr="<<theDDE->dr()<<" vol="<<theDDE->volume() ;
    log<<" dx="<<theDDE->dx()<<" dy="<<theDDE->dy()<<" dz="<<theDDE->dz();
    log<<" ndeta="<<c_eta_max-c_eta_min<<" ndphi="<<c_phi_max_org-c_phi_min_org<< endmsg ;
*/    
  }  



  CaloCell_ID_FCS::CaloSample calosample = (CaloCell_ID_FCS::CaloSample)theDDE->getSampling();
  int layernr=0;

  switch ( calosample ) {
// Cell in ECal
    case CaloCell_ID_FCS::PreSamplerB:
    case CaloCell_ID_FCS::PreSamplerE:
      layernr=0;
      break;
    case CaloCell_ID_FCS::EMB1:
    case CaloCell_ID_FCS::EME1:
      layernr=1;
      break;
    case CaloCell_ID_FCS::EMB2:
    case CaloCell_ID_FCS::EME2:
      layernr=2;
      break;
    case CaloCell_ID_FCS::EMB3:
    case CaloCell_ID_FCS::EME3:
      layernr=3;
      break;
// Cell in HCal
    case CaloCell_ID_FCS::TileBar0:
    case CaloCell_ID_FCS::TileExt0:
    case CaloCell_ID_FCS::TileGap1:
    case CaloCell_ID_FCS::HEC0:
      layernr=0;
      break;
    case CaloCell_ID_FCS::TileBar1:
    case CaloCell_ID_FCS::TileExt1:
    case CaloCell_ID_FCS::TileGap2:
    case CaloCell_ID_FCS::HEC1:
      layernr=1;
      break;
    case CaloCell_ID_FCS::TileBar2:
    case CaloCell_ID_FCS::TileExt2:
    case CaloCell_ID_FCS::TileGap3:
    case CaloCell_ID_FCS::HEC2:
      layernr=2;
      break;
    case CaloCell_ID_FCS::HEC3:
      layernr=3;
      break;
// Cell in FCal
    case CaloCell_ID_FCS::FCAL0:
      layernr=0;
      break;
    case CaloCell_ID_FCS::FCAL1:
      layernr=1;
      break;
    case CaloCell_ID_FCS::FCAL2:
      layernr=2;
      break;
    default :
      ATH_MSG_ERROR(" unknown CELL type " << calosample);
      break;
  }

  int iphi1=map.phi_to_index_cont(c_phi_min_org);
  int iphi2=map.phi_to_index_cont(c_phi_max_org);
  int ieta1=map.eta_to_index(c_eta_min);
  int ieta2=map.eta_to_index(c_eta_max);
  
  double eta_min;
  double eta_max;
  double phi_min;
  double phi_max;
  
  for(int ieta=ieta1;ieta<=ieta2;++ieta) {
    if(ieta<0)          continue;
    if((unsigned int)ieta>=map.neta()) continue;
    map.eta_range(ieta,eta_min,eta_max);
    
    for(int s_iphi=iphi1;s_iphi<=iphi2;++s_iphi) {
      int iphi=s_iphi;
      if(iphi<0)           iphi+=map.nphi();
      if((unsigned int)iphi>=map.nphi()) iphi-=map.nphi(); 
      map.phi_range(iphi,phi_min,phi_max);
      
      double phi_0=0.5*(phi_min+phi_max);
      double phi_add=0;
      if(phi_0<-2) phi_add=-2*M_PI; // project all phi's to phi<0 if inputcell phi<-2
      if(phi_0>+2) phi_add=+2*M_PI; // project all phi's to phi>0 if inputcell phi>+2

      double c_phi_min=c_phi_min_org;
      double c_phi_max=c_phi_max_org;

      // map phi in continues range
      if(c_phi_min>2 && phi_0<-2) {
        c_phi_min+=phi_add;
        c_phi_max+=phi_add;
      } else if(c_phi_max<-2 && phi_0>+2) {
        c_phi_min+=phi_add;
        c_phi_max+=phi_add;
      }

//      log << MSG::VERBOSE << "eta_min=" <<eta_min<<" eta_max=" <<eta_max<<" c_eta_min=" <<c_eta_min<<" c_eta_max=" <<c_eta_max<<std::endl;
//      log << MSG::VERBOSE << "phi_min=" <<phi_min<<" phi_max=" <<phi_max<<" c_phi_min=" <<c_phi_min<<" c_phi_max=" <<c_phi_max<< endmsg;

      // First test if cell is completely in volume, most likely after cellsInZone
      if(eta_min<=c_eta_min && eta_max>=c_eta_max && phi_min<=c_phi_min && phi_max>=c_phi_max) {
        map.vec(ieta,iphi).push_back( cellinfo( theDDE,(float)theDDE->volume() ),layernr );
//        log << MSG::VERBOSE << " -> cell eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling()<<" overlap="<<1<<std::endl;
        continue;
      }

      // test for no overlap at all
      if(c_eta_max<=eta_min) continue;
      if(c_eta_min>=eta_max) continue;
      if(c_phi_max<=phi_min) continue;
      if(c_phi_min>=phi_max) continue;


      // there must be some overlap, test independetly for eta and phi
      float overlap_eta=0;
      float overlap_phi=0;

      if(eta_min<=c_eta_min && eta_max>=c_eta_max) {
        // cell completely in eta range
        overlap_eta=1;
      } else  if(eta_min>=c_eta_min && eta_max<=c_eta_max) {
        // eta range completely in cell
        overlap_eta=(eta_max-eta_min)/(c_eta_max-c_eta_min);
      } else if(eta_min<=c_eta_min) {
        overlap_eta=(eta_max-c_eta_min)/(c_eta_max-c_eta_min);
      } else if(eta_max>=c_eta_max) {
        overlap_eta=(c_eta_max-eta_min)/(c_eta_max-c_eta_min);
      } else {
        ATH_MSG_WARNING("strange cell boundary eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" ; eta_min=" <<eta_min<<" eta_max=" <<eta_max<<" c_eta_min=" <<c_eta_min<<" c_eta_max=" <<c_eta_max);
      }


      if(phi_min<=c_phi_min && phi_max>=c_phi_max) {
        // cell completely in eta range
        overlap_phi=1;
      } else  if(phi_min>=c_phi_min && phi_max<=c_phi_max) {
        // eta range completely in cell
        overlap_phi=(phi_max-phi_min)/(c_phi_max-c_phi_min);
      } else if(phi_min<=c_phi_min) {
        overlap_phi=(phi_max-c_phi_min)/(c_phi_max-c_phi_min);
      } else if(phi_max>=c_phi_max) {
        overlap_phi=(c_phi_max-phi_min)/(c_phi_max-c_phi_min);
      } else {
        ATH_MSG_WARNING("strange cell boundary eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" ; phi_min=" <<phi_min<<" phi_max=" <<phi_max<<" c_phi_min=" <<c_phi_min<<" c_phi_max=" <<c_phi_max);
      }

      if(overlap_eta>1 || overlap_phi>1) {
        ATH_MSG_WARNING("strange overlap eta="<<overlap_eta<<" overlap_phi="<<overlap_phi<<" boundary eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()
                      <<" ; eta_min=" <<eta_min<<" eta_max=" <<eta_max<<" c_eta_min=" <<c_eta_min<<" c_eta_max=" <<c_eta_max
                      <<" ; phi_min=" <<phi_min<<" phi_max=" <<phi_max<<" c_phi_min=" <<c_phi_min<<" c_phi_max=" <<c_phi_max );
      }

      float overlap=overlap_eta*overlap_phi;
      if(overlap<0.02) continue;
      float overlapvol=overlap*theDDE->volume();

      if(overlapvol>0) {
        map.vec(ieta,iphi).push_back( cellinfo(theDDE,overlapvol),layernr );
//        log << MSG::VERBOSE << " -> cell eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling()<<" overlap="<<overlap<<endmsg;
        continue;
      }  

    }
  }

//  log << MSG::VERBOSE <<  "init cell finished" <<std::endl<< endmsg ;
}





void BasicCellBuilderTool::findCells(cellinfo_vec & cell_vec, double eta_min, double eta_max, double phi_min, double phi_max, int layer)
{
  ATH_MSG_VERBOSE("REGION : eta_min=" <<eta_min<<" eta_max=" <<eta_max << " phi_min=" <<phi_min<<" phi_max=" <<phi_max);

  std::vector<IdentifierHash> idhash_list;

  std::vector<const CaloDetDescrElement*> caloDDE_list;
  std::vector<double> vol_list;
  std::vector<bool> emhadflag_list;
  double vol=0;
  
  // Now do some tricks to calculate geometric overlaps, map phi in continues range
  double phi_0=0.5*(phi_min+phi_max);
  double phi_add=0;
  if(phi_0<-2) phi_add=-2*M_PI; // project all phi's to phi<0 if inputcell phi<-2
  if(phi_0>+2) phi_add=+2*M_PI; // project all phi's to phi>0 if inputcell phi>+2
  
  std::vector<CaloCell_ID::SUBCALO> subcalos;
  if(layer==0) {
    subcalos.push_back(CaloCell_ID::LAREM);
  }
  if(layer==1) {
    subcalos.push_back(CaloCell_ID::LAREM);
    subcalos.push_back(CaloCell_ID::LARFCAL);
  }
  if(layer==3) {
    subcalos.push_back(CaloCell_ID::TILE);
    subcalos.push_back(CaloCell_ID::LARHEC);
    subcalos.push_back(CaloCell_ID::LARFCAL);
  }
  
  const CaloDetDescrElement* theDDE;

  for(unsigned int sc=0;sc<subcalos.size();++sc) {
    m_caloDDM->cellsInZone(eta_min,eta_max,phi_min,phi_max,subcalos[sc],idhash_list);
    ATH_MSG_VERBOSE("sc="<<sc<<" : idhash_list.size()=" <<idhash_list.size());
    caloDDE_list.reserve(caloDDE_list.size()+idhash_list.size());
    vol_list.reserve(vol_list.size()+idhash_list.size());
  
    double c_phi_min;
    double c_phi_max;
    double c_eta_min;
    double c_eta_max;

    std::vector<IdentifierHash>::iterator idhash_list_end(idhash_list.end());
    for(std::vector<IdentifierHash>::iterator it_idhash=idhash_list.begin();it_idhash<idhash_list_end;++it_idhash) {
      theDDE=m_caloDDM->get_element(*it_idhash);
      if(theDDE) if(theDDE->volume()>0) {

        //dont' fill PreSampler and TileGap
        CaloCell_ID_FCS::CaloSample sample=(CaloCell_ID_FCS::CaloSample)theDDE->getSampling();
        CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();

        if(sample==CaloCell_ID_FCS::PreSamplerB) continue;
        if(sample==CaloCell_ID_FCS::PreSamplerE) continue;
        if(sample==CaloCell_ID_FCS::TileGap1) continue;
        if(sample==CaloCell_ID_FCS::TileGap2) continue;
        if(sample==CaloCell_ID_FCS::TileGap3) continue;

        if(layer==0) {
          if(! (sample==CaloCell_ID_FCS::EMB1 || sample==CaloCell_ID_FCS::EME1)) continue;
        }
        if(layer==1) {
          if(sample==CaloCell_ID_FCS::EMB1) continue;
          if(sample==CaloCell_ID_FCS::EME1) continue;
          if(! (calo==CaloCell_ID::LAREM || sample==CaloCell_ID_FCS::FCAL0)) continue;
        }
        if(layer==3) {
          if(! (calo==CaloCell_ID::LARHEC || calo==CaloCell_ID::TILE || sample==CaloCell_ID_FCS::FCAL1 || sample==CaloCell_ID_FCS::FCAL2)) continue;
        }

        //get cell boundaries
        c_phi_min=theDDE->phi()-theDDE->dphi()/2;
        c_phi_max=theDDE->phi()+theDDE->dphi()/2;
        c_eta_min=theDDE->eta()-theDDE->deta()/2;
        c_eta_max=theDDE->eta()+theDDE->deta()/2;

        // map phi in continues range
        if(c_phi_min>2 && phi_0<-2) {
          c_phi_min+=phi_add;
          c_phi_max+=phi_add;
        } else if(c_phi_max<-2 && phi_0>+2) {
          c_phi_min+=phi_add;
          c_phi_max+=phi_add;
        }

  //      log << MSG::DEBUG << "fill cell boundary eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling()<<std::endl;
  //      log << MSG::DEBUG << "eta_min=" <<eta_min<<" eta_max=" <<eta_max<<" c_eta_min=" <<c_eta_min<<" c_eta_max=" <<c_eta_max<<std::endl;
  //      log << MSG::DEBUG << "phi_min=" <<phi_min<<" phi_max=" <<phi_max<<" c_phi_min=" <<c_phi_min<<" c_phi_max=" <<c_phi_max<< endmsg;

        // First test if cell is completely in volume, most likely after cellsInZone
        if(eta_min<=c_eta_min && eta_max>=c_eta_max && phi_min<=c_phi_min && phi_max>=c_phi_max) {
          caloDDE_list.push_back(theDDE);
          vol_list.push_back(theDDE->volume());
          vol+=theDDE->volume();
          continue;
        }

        // test for no overlap at all
        if(c_eta_max<=eta_min) continue;
        if(c_eta_min>=eta_max) continue;
        if(c_phi_max<=phi_min) continue;
        if(c_phi_min>=phi_max) continue;


        // there must be some overlap, test independetly for eta and phi
        double overlap_eta=0;
        double overlap_phi=0;

        if(eta_min<=c_eta_min && eta_max>=c_eta_max) {
          // cell completely in eta range
          overlap_eta=1;
        } else  if(eta_min>=c_eta_min && eta_max<=c_eta_max) {
          // eta range completely in cell
          overlap_eta=(eta_max-eta_min)/(c_eta_max-c_eta_min);
        } else if(eta_min<=c_eta_min) {
          overlap_eta=(eta_max-c_eta_min)/(c_eta_max-c_eta_min);
        } else if(eta_max>=c_eta_max) {
          overlap_eta=(c_eta_max-eta_min)/(c_eta_max-c_eta_min);
        } else {
          ATH_MSG_WARNING("strange cell boundary eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" ; eta_min=" <<eta_min<<" eta_max=" <<eta_max<<" c_eta_min=" <<c_eta_min<<" c_eta_max=" <<c_eta_max);
        }


        if(phi_min<=c_phi_min && phi_max>=c_phi_max) {
          // cell completely in eta range
          overlap_phi=1;
        } else  if(phi_min>=c_phi_min && phi_max<=c_phi_max) {
          // eta range completely in cell
          overlap_phi=(phi_max-phi_min)/(c_phi_max-c_phi_min);
        } else if(phi_min<=c_phi_min) {
          overlap_phi=(phi_max-c_phi_min)/(c_phi_max-c_phi_min);
        } else if(phi_max>=c_phi_max) {
          overlap_phi=(c_phi_max-phi_min)/(c_phi_max-c_phi_min);
        } else {
          ATH_MSG_WARNING("strange cell boundary eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" ; phi_min=" <<phi_min<<" phi_max=" <<phi_max<<" c_phi_min=" <<c_phi_min<<" c_phi_max=" <<c_phi_max);
        }

        if(overlap_eta>1 || overlap_phi>1) {
          ATH_MSG_WARNING("strange overlap eta="<<overlap_eta<<" overlap_phi="<<overlap_phi<<" boundary eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()
                        <<" ; eta_min=" <<eta_min<<" eta_max=" <<eta_max<<" c_eta_min=" <<c_eta_min<<" c_eta_max=" <<c_eta_max
                        <<" ; phi_min=" <<phi_min<<" phi_max=" <<phi_max<<" c_phi_min=" <<c_phi_min<<" c_phi_max=" <<c_phi_max );
        }

        double overlap=overlap_eta*overlap_phi;
        if(overlap<0.02) continue;
        double overlapvol=overlap*theDDE->volume();

        if(overlapvol>0) {
          caloDDE_list.push_back(theDDE);
          vol_list.push_back(overlapvol);
          vol+=overlapvol;
          continue;
        }  


      }
    }
  }
  ATH_MSG_VERBOSE("caloDDE_list.size()=" <<caloDDE_list.size() << " ; vol_list.size()=" <<vol_list.size() );
  

//  log << MSG::DEBUG << "REGION : eta_min=" <<eta_min<<" eta_max=" << " phi_min=" <<phi_min<<" phi_max=" <<phi_max<< endmsg;
  for(unsigned int i=0;i<caloDDE_list.size();++i) {
    theDDE=caloDDE_list[i];
    ATH_MSG_VERBOSE( i <<" : cell eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" sample="<<theDDE->getSampling()<<" deta="<<theDDE->deta()<<" dphi="<<theDDE->dphi()<<" overlap="<<vol_list[i]/theDDE->volume() );
  }
  
  double test=0;
  for(unsigned int i=0;i<caloDDE_list.size();++i) {
    test+=vol_list[i];
    vol_list[i]/=vol;
    cell_vec.push_back(cellinfo(caloDDE_list[i],vol_list[i]),0);
  }

  if(vol!=test) {
    ATH_MSG_WARNING("Volumne sum not correct : vol=" <<vol<<" test="<<test);
  } else {  
    ATH_MSG_VERBOSE("Volume sum vol=" <<vol);
  }
}












StatusCode BasicCellBuilderTool::process(CaloCellContainer * theCellContainer)
{
  ATH_MSG_INFO("Executing start size=" <<theCellContainer->size());
  ++m_nEvent;
  
//  addCell0101(theCellContainer,48,10,300000,m_em_map);
//  addCell0101(theCellContainer,52,10,100000,m_em_map);
//  addCell0101(theCellContainer,52,20,300000,m_had_map);

//  addCell0101(theCellContainer,54,30,300000,m_em_map);
//  addCell0101(theCellContainer,54,30,300000,m_had_map);


  ATH_MSG_INFO("Executing finished : end size=" <<theCellContainer->size());
  return StatusCode::SUCCESS;
}



void BasicCellBuilderTool::addCell(CaloCellContainer * theCellContainer, int etabin, int phibin, double energy, cellinfo_map& map )
{
//  log << MSG::DEBUG << "theCellContainer->hasTotalSize()="<<theCellContainer->hasTotalSize()<< endmsg;
//  log << MSG::DEBUG << "theCellContainer->isOrderedAndComplete()="<<theCellContainer->isOrderedAndComplete()<< endmsg;
//  log << MSG::DEBUG << "theCellContainer->isOrdered()="<<theCellContainer->isOrdered()<< endmsg;
//  log << MSG::DEBUG << "theCellContainer->hasTotalSize()="<<theCellContainer->hasTotalSize()<< endmsg;

  if(etabin<0 || etabin>=(int)map.neta()) {
    ATH_MSG_WARNING("wrong etabin or phibin in BasicCellBuilderTool::addCell etabin="<<etabin<<" phibin="<<phibin<<" neta="<<map.neta()<<" nphi="<<map.nphi());
    return;
  }

  if(phibin<0 || phibin>=(int)map.nphi()) {
    ATH_MSG_WARNING("wrong etabin or phibin in BasicCellBuilderTool::addCell etabin="<<etabin<<" phibin="<<phibin<<" neta="<<map.neta()<<" nphi="<<map.nphi());
    return;
  }

  cellinfo_vec::vec::iterator iterbeg(map.vec(etabin,phibin).m_cellinfo_vec.begin());
  cellinfo_vec::vec::iterator iterend(map.vec(etabin,phibin).m_cellinfo_vec.end());
  
  bool new_cell_created=false;
  
  double etest=0;
  int nadd=0;
  
  //double towerphi=map.index_to_phi_center(phibin);
  double towereta=map.index_to_eta_center(etabin);

  for(cellinfo_vec::vec::iterator iter=iterbeg;iter<iterend;++iter) {
    const CaloDetDescrElement* theDDE=(iter->first);
    if (theDDE!=0) 	
    {
      CaloCell* theCaloCell;

      // is there a other way to get a non const pointer?
      theCaloCell=(CaloCell*)(theCellContainer->findCell(theDDE->calo_hash()));
      if(theCaloCell) {
        ATH_MSG_VERBOSE("found calo cell : eta=" <<theCaloCell->caloDDE()->eta()<<" phi="<<theCaloCell->caloDDE()->phi()<<" overlap="<<iter->second<<"old e=" <<theCaloCell->energy()<< " ; new e=" <<theCaloCell->energy()+energy*iter->second);
        theCaloCell->setEnergy(theCaloCell->energy()+energy*iter->second);
/*
        if(fabs(towereta-theCaloCell->caloDDE()->eta())>0.1 || fabs(towerphi-theCaloCell->caloDDE()->phi())>0.1) {
          log<<MSG::WARNING<<"    -> teta="<<towereta<<" tphi="<<towerphi <<" ceta="<<theCaloCell->caloDDE()->eta()<<" cphi="<<theCaloCell->caloDDE()->phi()<<endmsg;
        }  
*/
      } else {
        ATH_MSG_WARNING("det_elm found eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" overlap="<<iter->second<<" hash="<<theDDE->calo_hash()
                     << " : new   calo cell : e=" <<energy*iter->second);
        theCaloCell=new CaloCell(theDDE,energy*iter->second,0,1,CaloGain::UNKNOWNGAIN);
        theCellContainer->push_back(theCaloCell);
        new_cell_created=true;
      }
      etest+=energy*iter->second;
      ++nadd;
    } else {
      ATH_MSG_ERROR("no det_elm found etabin=" <<etabin<<" phibin="<<phibin);
    }
  } 
  if(fabs(etest-energy)>0.1 && fabs(towereta)<4.9) {
    ATH_MSG_ERROR("energy missmatch "<<map.name()<<" eta="<<map.index_to_eta_center(etabin)<<" phi="<<map.index_to_phi_center(phibin)
                <<": Ein=" <<energy<<" Eout="<<etest<<" : "<<nadd<<" cells for filling");
  }
  if(new_cell_created) theCellContainer->initializeLookUpTable();
}	

