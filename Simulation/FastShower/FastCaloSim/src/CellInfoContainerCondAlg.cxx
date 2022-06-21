/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CellInfoContainerCondAlg.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CLHEP/Geometry/Vector3D.h"
#include <memory>

StatusCode CellInfoContainerCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());
  ATH_CHECK(m_caloMgrKey.initialize());
  ATH_CHECK(m_cellInfoContKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode CellInfoContainerCondAlg::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<CellInfoContainer> cellInfoContHandle{m_cellInfoContKey,ctx};
  if(cellInfoContHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }

  // ____________ Get the input condition object ____________
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
  ATH_CHECK(caloMgrHandle.isValid());
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;
  ATH_MSG_DEBUG("Retrieved CaloDetDescrManager object form the Condition Store");
  cellInfoContHandle.addDependency(caloMgrHandle);

  // ____________ Build the output condition object ____________
  std::unique_ptr<CellInfoContainer> cellInfoCont = std::make_unique<CellInfoContainer>();

  find_phi0(caloDDMgr,cellInfoCont.get());

  ATH_MSG_INFO("========================= Init EM map =============================");
  cellInfoCont->getEmMap().init(-5,+5,-M_PI+cellInfoCont->getPhi0Em() ,+M_PI+cellInfoCont->getPhi0Em() ,100,64);
  cellInfoCont->getEmMap().setname("EM");

  ATH_MSG_INFO("========================= Init EM fine map ========================");
  cellInfoCont->getEmFineMap().init(-2.8,+2.8,-M_PI+cellInfoCont->getPhi0Em() ,+M_PI+cellInfoCont->getPhi0Em() ,224,256);
  cellInfoCont->getEmFineMap().setname("EM fine");

  ATH_MSG_INFO("========================= Init HAD map ============================");
  cellInfoCont->getHadMap().init(-5,+5,-M_PI+cellInfoCont->getPhi0Had(),+M_PI+cellInfoCont->getPhi0Had(),100,64);
  cellInfoCont->getHadMap().setname("HAD");

  ATH_MSG_INFO("========================= Init EM celllist map =============================");
  cellInfoCont->getEmCellistMap().init(-5,+5,-M_PI+cellInfoCont->getPhi0Em() ,+M_PI+cellInfoCont->getPhi0Em() ,100,64,2,2);
  cellInfoCont->getEmCellistMap().setname("EMlist");

  ATH_MSG_INFO("========================= Init celllist maps sample 0 ... "<< CaloCell_ID_FCS::LastSample);
  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    cellInfoCont->getCellistMap(sample).init(-5,+5,-M_PI+cellInfoCont->getPhi0Em() ,+M_PI+cellInfoCont->getPhi0Em() ,100,64,3,3);
    cellInfoCont->getCellistMap(sample).setname("samplecelllist");
  }

  init_all_maps(caloDDMgr,cellInfoCont.get());
  ATH_MSG_INFO("========================= Init volume all maps =========================");
  init_volume(cellInfoCont->getEmMap());
  init_volume(cellInfoCont->getEmFineMap());
  init_volume(cellInfoCont->getHadMap());

  // ____________ Record the output into condition store ____________

  ATH_CHECK(cellInfoContHandle.record(std::move(cellInfoCont)));
  ATH_MSG_DEBUG("recorded new CaloDetDescr Manager condition object with key " << cellInfoContHandle.key()
		<< " and range " << cellInfoContHandle.getRange());

  return StatusCode::SUCCESS;
}

void CellInfoContainerCondAlg::init_all_maps(const CaloDetDescrManager* caloDDM
					     , CellInfoContainer* cont) const
{
  ATH_MSG_INFO("Building all maps");

  cellinfo_map* curmap{nullptr};
  cellinfo_map* curmap2{nullptr};

  ATH_MSG_DEBUG("size all cells : " <<caloDDM->element_end()-caloDDM->element_begin()
                <<  "; LAREM="          <<caloDDM->element_end(CaloCell_ID::LAREM)-caloDDM->element_begin(CaloCell_ID::LAREM)
                <<  "; TILE="           <<caloDDM->element_end(CaloCell_ID::TILE)-caloDDM->element_begin(CaloCell_ID::TILE)
                <<  "; LARHEC="         <<caloDDM->element_end(CaloCell_ID::LARHEC)-caloDDM->element_begin(CaloCell_ID::LARHEC)
                <<  "; LARFCAL="        <<caloDDM->element_end(CaloCell_ID::LARFCAL)-caloDDM->element_begin(CaloCell_ID::LARFCAL) );

  int nok=0;
  int nvolOK=0;
  int naccept=0;
  bool doem_celllist_map=false;
  bool do_celllist_map=false;
  int emcl_neta=cont->getEmCellistMap().leta()-1;
  int emcl_nphi=cont->getEmCellistMap().lphi()-1;
  if(cont->getEmCellistMap().leta()>1 && cont->getEmCellistMap().lphi()>1) doem_celllist_map=true;
  if(cont->getCellistMap(0).leta()>1 && cont->getCellistMap(0).lphi()>1) do_celllist_map=true;

  FSmap< double , double > rz_map_eta [2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_rmid[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_zmid[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_rent[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , double > rz_map_zent[2][CaloCell_ID_FCS::MaxSample];
  FSmap< double , int    > rz_map_n   [2][CaloCell_ID_FCS::MaxSample];


  for(int side=0;side<=1;++side) for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    cont->getMinEtaSample(side,sample)=+1000;
    cont->getMaxEtaSample(side,sample)=-1000;
  }


  for(CaloDetDescrManager::calo_element_const_iterator calo_iter=caloDDM->element_begin();calo_iter<caloDDM->element_end();++calo_iter) {
    const CaloDetDescrElement* theDDE=*calo_iter;
    if(theDDE) {
      ++nok;
      
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
      if(min_eta<cont->getMinEtaSample(side,sample)) cont->getMinEtaSample(side,sample)=min_eta;
      if(max_eta>cont->getMaxEtaSample(side,sample)) cont->getMaxEtaSample(side,sample)=max_eta;
      
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
        int cl_neta=cont->getCellistMap(sample).leta()-1;
        int cl_nphi=cont->getCellistMap(sample).lphi()-1;
        double ceta_min=theDDE->eta()-cont->getCellistMap(sample).deta()*cl_neta;
        double ceta_max=theDDE->eta()+cont->getCellistMap(sample).deta()*cl_neta;
        double cphi_min=theDDE->phi()-cont->getCellistMap(sample).dphi()*cl_nphi;
        double cphi_max=theDDE->phi()+cont->getCellistMap(sample).dphi()*cl_nphi;

        int iphi1=cont->getCellistMap(sample).phi_to_index_cont(cphi_min);
        int iphi2=cont->getCellistMap(sample).phi_to_index_cont(cphi_max);
        int ieta1=cont->getCellistMap(sample).eta_to_index(ceta_min);
        int ieta2=cont->getCellistMap(sample).eta_to_index(ceta_max);

        for(int ieta=ieta1;ieta<=ieta2;++ieta) {
          if(ieta<0)          continue;
          if((unsigned int)ieta>=cont->getCellistMap(sample).neta()) continue;
	  
          for(int s_iphi=iphi1;s_iphi<=iphi2;++s_iphi) {
            int iphi=s_iphi;
            if(iphi<0)           iphi+=cont->getCellistMap(sample).nphi();
            if((unsigned int)iphi>=cont->getCellistMap(sample).nphi()) iphi-=cont->getCellistMap(sample).nphi();

            cont->getCellistMap(sample).vec(ieta,iphi).push_back( cellinfo( theDDE,1),2);
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
          double ceta_min=theDDE->eta()-cont->getEmCellistMap().deta()*emcl_neta;
          double ceta_max=theDDE->eta()+cont->getEmCellistMap().deta()*emcl_neta;
          double cphi_min=theDDE->phi()-cont->getEmCellistMap().dphi()*emcl_nphi;
          double cphi_max=theDDE->phi()+cont->getEmCellistMap().dphi()*emcl_nphi;
	  
          int iphi1=cont->getEmCellistMap().phi_to_index_cont(cphi_min);
          int iphi2=cont->getEmCellistMap().phi_to_index_cont(cphi_max);
          int ieta1=cont->getEmCellistMap().eta_to_index(ceta_min);
          int ieta2=cont->getEmCellistMap().eta_to_index(ceta_max);

          for(int ieta=ieta1;ieta<=ieta2;++ieta) {
            if(ieta<0)          continue;
            if((unsigned int)ieta>=cont->getEmCellistMap().neta()) continue;
	    
            for(int s_iphi=iphi1;s_iphi<=iphi2;++s_iphi) {
              int iphi=s_iphi;
              if(iphi<0)           iphi+=cont->getEmCellistMap().nphi();
              if((unsigned int)iphi>=cont->getEmCellistMap().nphi()) iphi-=cont->getEmCellistMap().nphi();
	      
              cont->getEmCellistMap().vec(ieta,iphi).push_back( cellinfo( theDDE,1),2);
            }
          }
	  
        }

        curmap=nullptr;
        curmap2=nullptr;
        if(calo==CaloCell_ID::LAREM)   {
          curmap=&(cont->getEmMap());
          if(fabs(theDDE->eta())<2.9 && cont->getEmFineMap().neta()>0) curmap2=&(cont->getEmFineMap());
        }
        if(sample==CaloCell_ID_FCS::FCAL0) curmap=&(cont->getEmMap());
	
        if(calo==CaloCell_ID::LARHEC)  curmap=&(cont->getHadMap());
	if(calo==CaloCell_ID::TILE)    curmap=&(cont->getHadMap());
        if(sample==CaloCell_ID_FCS::FCAL1 || sample==CaloCell_ID_FCS::FCAL2) curmap=&(cont->getHadMap());
	
        if(curmap2) {
          init_cell(*curmap2,theDDE);
        }
        if(curmap) {
          if(sample!=CaloCell_ID_FCS::EMB1 && sample!=CaloCell_ID_FCS::EME1) init_cell(*curmap,theDDE);
        }
      } 
    } 
  }
  for(int side=0;side<=1;++side) for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
      if(!rz_map_n[side][sample].empty()) {
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

            cont->getRmidMap(side,sample)[eta]=rmid;
            cont->getZmidMap(side,sample)[eta]=zmid;
            cont->getRentMap(side,sample)[eta]=rent;
            cont->getZentMap(side,sample)[eta]=zent;
          }
        }
        ATH_MSG_DEBUG("rz-map for side="<<side<<" sample="<<sample<<" #etas="<<cont->getRmidMap(side,sample).size());
      } else {
        ATH_MSG_WARNING("rz-map for side="<<side<<" sample="<<sample<<" is empty!!!");
      }
    }
  
  ATH_MSG_INFO("Building all maps finished : n(total)="<<nok<< " n(volume>0)="<<nvolOK<<" n(used)="<<naccept);
}

void CellInfoContainerCondAlg::init_cell(cellinfo_map& map
					 ,const CaloDetDescrElement* theDDE) const
{

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

    }
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

      // First test if cell is completely in volume, most likely after cellsInZone
      if(eta_min<=c_eta_min && eta_max>=c_eta_max && phi_min<=c_phi_min && phi_max>=c_phi_max) {
        map.vec(ieta,iphi).push_back( cellinfo( theDDE,(float)theDDE->volume() ),layernr );
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
        continue;
      }
    }
  }
}

void CellInfoContainerCondAlg::init_volume(cellinfo_map& map) const
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

void CellInfoContainerCondAlg::find_phi0(const CaloDetDescrManager* caloDDM
					 , CellInfoContainer* cont) const
{
  if(cont->getPhi0Em()==-1000) {
    ATH_MSG_DEBUG("Trying to find best phi0 of EM calo :");
    cont->getPhi0Em()=0;
    cellinfo_vec test_vec;
    const double dphi=2*M_PI/64;
    findCells(caloDDM,test_vec,0.1,0.2,-dphi,dphi,1);
    double bestphi0=10000;

    for(unsigned int i=0;i<test_vec.size();++i) {
      const CaloDetDescrElement* theDDE=test_vec[i].first;
      double phi0;

      phi0=theDDE->phi()-theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
      phi0=theDDE->phi()+theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
    }

    if(bestphi0!=10000) cont->getPhi0Em()=bestphi0;

    ATH_MSG_DEBUG("Best phi0 EM=" << bestphi0);
  }

  if(cont->getPhi0Had()==-1000) {
    ATH_MSG_DEBUG("Trying to find best phi0 of Tile calo :");
    cont->getPhi0Had()=0;
    cellinfo_vec test_vec;
    const double dphi=2*M_PI/64;
    findCells(caloDDM,test_vec,0.1,0.2,-dphi,dphi,3);
    double bestphi0=10000;

    for(unsigned int i=0;i<test_vec.size();++i) {
      const CaloDetDescrElement* theDDE=test_vec[i].first;
      double phi0;

      phi0=theDDE->phi()-theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
      phi0=theDDE->phi()+theDDE->dphi()/2;
      if(fabs(phi0)<fabs(bestphi0)) bestphi0=phi0;
    }

    if(bestphi0!=10000) cont->getPhi0Had()=bestphi0;

    ATH_MSG_DEBUG("Best phi0 HAD=" << bestphi0);
  }
}
 
void CellInfoContainerCondAlg::findCells(const CaloDetDescrManager* caloDDM
					 , cellinfo_vec & cell_vec
					 , double eta_min
					 , double eta_max
					 , double phi_min
					 , double phi_max
					 , int layer) const
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
    caloDDM->cellsInZone(eta_min,eta_max,phi_min,phi_max,subcalos[sc],idhash_list);
    ATH_MSG_VERBOSE("sc="<<sc<<" : idhash_list.size()=" <<idhash_list.size());
    caloDDE_list.reserve(caloDDE_list.size()+idhash_list.size());
    vol_list.reserve(vol_list.size()+idhash_list.size());

    double c_phi_min;
    double c_phi_max;
    double c_eta_min;
    double c_eta_max;

    std::vector<IdentifierHash>::iterator idhash_list_end(idhash_list.end());
    for(std::vector<IdentifierHash>::iterator it_idhash=idhash_list.begin();it_idhash<idhash_list_end;++it_idhash) {
      theDDE=caloDDM->get_element(*it_idhash);
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
