/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "CscSegmentMaker.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include <cmath>


namespace TrigL2MuonSA {


CscSegmentMaker::CscSegmentMaker(const std::string& type, const std::string& name, const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_util()
{
}


StatusCode CscSegmentMaker::initialize(){
  ATH_CHECK(m_cscregdict.retrieve());
  ATH_CHECK(m_muDetMgrKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}


ReturnCode CscSegmentMaker::FindSuperPointCsc( const TrigL2MuonSA::CscHits &cscHits,
                                                std::vector<TrigL2MuonSA::TrackPattern> &v_trackPatterns,
                                                const TrigL2MuonSA::TgcFitResult &tgcFitResult,
                                                const TrigL2MuonSA::MuonRoad &muroad)
{
  ATH_MSG_DEBUG( "FindSuperPointCsc" );
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  
  if( !cscHits.empty() ){
    
    std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
    for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) { // loop for track candidates
      
      //get module hash  to read
      int hash_clusters[32]={0};      
      
      TrigL2MuonSA::CscHits clusters[32][8];
      for(unsigned int iclu=0; iclu<cscHits.size(); ++iclu){
        const TrigL2MuonSA::CscHitData &cscHit = cscHits[iclu];
        
	//outlier or not
        double width = (cscHit.MeasuresPhi == 0 ) ? m_max_residual_eta : m_max_residual_phi;
        if ( width < fabs(cscHit.Residual) )  continue;
        
        
        int cathodelayer = 2*(cscHit.WireLayer-1)+  cscHit.MeasuresPhi;//cathodelayer is in [0,7]
        int module_hash = m_cscregdict->get_hash(cscHit.StationName, cscHit.StationEta,  cscHit.StationPhi);
        
	if(0<=module_hash && 32>module_hash){
	  clusters[module_hash][cathodelayer].push_back( cscHit );        
	  hash_clusters[module_hash]=1;//set modules to read
        }else{
	  continue;
	}
      }//for clusters
      
      
      //decide which module to read
      int hashSPs[2]={999,999};
      if( getModuleSP( hashSPs, tgcFitResult, (*itTrack).phiBin, muroad, hash_clusters)!=ReturnCode::FAILURE ){
        
	bool found_segment=false;
	for(int ihash=0; ihash<2; ++ihash){
	  if(found_segment) break;//once a segment is found, then finish searching more
	  
	  int hashSP=hashSPs[ihash];
	  if(hashSP>32) continue;
          
	  //making segment
	  CscSegment cscsegment, cscsegment_noip;
	  if (this->make_segment(hashSP, clusters[hashSP] , cscsegment, cscsegment_noip, muDetMgr) != ReturnCode::FAILURE  ){
	    found_segment=true;
	    (*itTrack).hashID_CSC = hashSP;

	    xAOD::L2MuonParameters::Chamber csc = xAOD::L2MuonParameters::Chamber::CSC;
	    xAOD::L2MuonParameters::Chamber outer = xAOD::L2MuonParameters::Chamber::EndcapOuter;
	    TrigL2MuonSA::SuperPoint* superPoint = &((*itTrack).superPoints[csc]);
	    const TrigL2MuonSA::SuperPoint* outerSP = &((*itTrack).superPoints[outer]);
	    bool &large_dphidz = (*itTrack).large_dPhidZ;
	    double &outerCorFactor =  (*itTrack).outerCorFactor;
	    
            //tgcfitresult
	    double tgcmid1z = tgcFitResult.tgcMid1[3];
	    double tgcmid2z = tgcFitResult.tgcMid2[3];
	    double phimiddle = tgcFitResult.phi;
	    double dPhidz = tgcFitResult.dPhidZ;
	    if( 0.75e-6<fabs(dPhidz) ) large_dphidz=true;
	    double tgcmidZ = (tgcmid1z+tgcmid2z)/2.;
	    double outerz = outerSP->Z;
	    
            //calculate SP
	    CscSegment cscsegment_ext = segmentAtFirstLayer(hashSP, &cscsegment);
	    double phiMod = m_cscregdict->phiMod(hashSP);
	    double CSCR=cscsegment_ext.x()*cos(phiMod)+cscsegment_ext.y()*sin(phiMod);
	    double CSCZ=cscsegment_ext.z();
	    double PhiAtCsc = phimiddle/* - fabs(CSCZ-tgcmidZ)*dPhidz*/;
	    double CSCSPR = CSCR/cos( m_util.calc_dphi(PhiAtCsc,phiMod) );
	    
	    
	    superPoint->Z = CSCZ;
	    superPoint->R = CSCSPR;
	    superPoint->Alin = cscsegment_ext.slopeRZ();
	    superPoint->Blin = cscsegment_ext.interceptRZ();
	    superPoint->Npoint = cscsegment_ext.nHitEta();
	    superPoint->Chi2 = cscsegment_ext.chiSquare();
	    ATH_MSG_DEBUG("CSC super point:hashID=" << hashSP
			  << ",CSCR=" << CSCSPR << ",CSCZ=" << CSCZ 
			  << ",Slope=" << cscsegment_ext.slopeRZ() << ",Intercept=" << cscsegment_ext.interceptRZ()
			  << ",nHitsEta=" << cscsegment_ext.nHitEta() << ",Chi2="<< cscsegment_ext.chiSquare());
	    
	    //calculate outerSP's correction (dphidz of tgcFitResult)
	    double phiouter = phimiddle+fabs(outerz-tgcmidZ)*dPhidz;
	    outerCorFactor = cos( m_util.calc_dphi(phiouter,phiMod) )/cos( m_util.calc_dphi(phimiddle,phiMod) );
	    ATH_MSG_DEBUG("outerCorFactor=" << outerCorFactor);
	    
	  }//if there is a segment.
	}//ihash loop over modules in which segment will be made
      }//if there is the module in which segment will be made.
    }//loop over track pattern candidate
  }//if there are clusters, then make a segment
  
  
  return  ReturnCode::SUCCESS;
}




ReturnCode  CscSegmentMaker::make_segment(int mod_hash, TrigL2MuonSA::CscHits clusters[8],
                                            CscSegment &cscsegment,
                                            CscSegment &cscsegment_noip, const MuonGM::MuonDetectorManager* muDetMgr)
{
  ATH_MSG_DEBUG("################################## make_segment #####################################");
  
  std::vector<localCscHit> hits_loc_eta[4], hits_loc_phi[4],emp;
  emp.clear();
  for(int i =0;i<4;++i){
    hits_loc_eta[i]=emp;
    hits_loc_phi[i]=emp;
  }
  

 
  Amg::Transform3D gToLocal;
  if(m_use_geometry){
    Identifier Id = m_idHelperSvc->cscIdHelper().channelID(m_cscregdict->stationName(mod_hash), m_cscregdict->stationEta(mod_hash),m_cscregdict->stationPhi(mod_hash),2/*chamberLayer*/, 1, 0, 1);
    const MuonGM::CscReadoutElement *csc = muDetMgr->getCscReadoutElement(Id);
    if (csc == NULL){
      ATH_MSG_DEBUG( "Csc Readout Element not found ---- skip");
      return ReturnCode::FAILURE;
    }
    ATH_MSG_DEBUG("CscReadoutElement");
    gToLocal = csc->GlobalToAmdbLRSTransform();
  }else{
  double rotpi = (m_cscregdict->stationEta(mod_hash)>0) ? -M_PI/2. : M_PI/2.;
  Amg::AngleAxis3D rotZamg( (-1)*(m_cscregdict->phiMod(mod_hash)), Amg::Vector3D(0,0,1));
  Amg::AngleAxis3D rotYamg( (-1)*(m_cscregdict->actualAtanNormal(mod_hash)), Amg::Vector3D(0,1,0) );
  Amg::AngleAxis3D rotPIamg( rotpi, Amg::Vector3D(0,0,1));
  Amg::Translation3D translation( 0.0, 0.0, (-1)*(m_cscregdict->displacement(mod_hash)) );
  gToLocal=translation*rotPIamg*rotYamg*rotZamg;
  }


  localCscHit ip_loc;  
  Amg::Vector3D ip_vec = gToLocal*Amg::Vector3D(0.0, 0.0, 0.0);
  ip_loc.loc_x = ip_vec(Amg::x);  
  ip_loc.loc_y = ip_vec(Amg::y);  
  ip_loc.loc_z = ip_vec(Amg::z);  
  ip_loc.measphi = -1;
  ip_loc.error = m_err_ip;
  ip_loc.residual = 0.;
  ip_loc.isIP = true;
  ip_loc.enabled = false;
  ip_loc.stationname = 0;

  for (unsigned int clyr=0; clyr<8; ++clyr) {
    for (unsigned int ihit=0; ihit<clusters[clyr].size(); ++ihit) {
      
      TrigL2MuonSA::CscHitData &cschit = clusters[clyr][ihit];
      double r = cschit.r;
      double phi = cschit.phi;
      double z = cschit.z;
      
        //move to local coordinate system
      Amg::Vector3D vect(r*cos(phi),r*sin(phi),z);
      Amg::Vector3D loc_vect = gToLocal*vect;      

        //building localCscHit object
      localCscHit loc_hit;
      loc_hit.loc_x = loc_vect(Amg::x);
      loc_hit.loc_y = loc_vect(Amg::y);
      loc_hit.loc_z = loc_vect(Amg::z);
      loc_hit.measphi=cschit.MeasuresPhi;
      loc_hit.error = (loc_hit.measphi==0) ? m_err_eta : m_err_phi;
      loc_hit.residual = cschit.Residual;
      loc_hit.enabled=true;
      loc_hit.isIP=false;
      loc_hit.stationname=cschit.StationName;
      ATH_MSG_DEBUG( "measphi=" << cschit.MeasuresPhi << " wirelyr=" << cschit.WireLayer << " x=" << loc_hit.loc_x << " y=" << loc_hit.loc_y << " z=" << loc_hit.loc_z <<  " err=" << loc_hit.error << " res=" << loc_hit.residual);
      
      
      if(0==loc_hit.measphi) {
        if(m_max_residual_eta>fabs(loc_hit.residual)) hits_loc_eta[cschit.WireLayer-1].push_back( loc_hit );
      }
      if(1==loc_hit.measphi) {
        if(m_max_residual_phi>fabs(loc_hit.residual))  hits_loc_phi[cschit.WireLayer-1].push_back( loc_hit );
      }
    }//ihit
  }//clyr
  
  Amg::Vector3D seg_pos_loc, seg_dir_loc,seg_pos_loc_noip, seg_dir_loc_noip;
  int nhite=0,nhitp=0;
  local2dSegment seg2d_eta ,seg2d_phi, seg2d_eta_noip, seg2d_phi_noip;
  
  
  if( this->make_2dsegment( 0/*=measphi*/, ip_loc, hits_loc_eta, seg2d_eta, seg2d_eta_noip, nhite)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  if( this->make_2dsegment( 1/*=measphi*/, ip_loc, hits_loc_phi, seg2d_phi,seg2d_phi_noip, nhitp)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  
  
  ATH_MSG_DEBUG("==include IP==");
  if( this->make_4dsegment(seg2d_eta,seg2d_phi,seg_pos_loc,seg_dir_loc)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  ATH_MSG_DEBUG("==exclude IP==");
  if( this->make_4dsegment(seg2d_eta_noip,seg2d_phi_noip,seg_pos_loc_noip,seg_dir_loc_noip)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  
  
  Amg::Transform3D lToGlobal = gToLocal.inverse();
  Amg::Vector3D seg_pos = lToGlobal*seg_pos_loc;
  Amg::Vector3D seg_dir = lToGlobal.rotation()*seg_dir_loc;
  Amg::Vector3D seg_pos_noip = lToGlobal*seg_pos_loc_noip;
  Amg::Vector3D seg_dir_noip = lToGlobal.rotation()*seg_dir_loc_noip;

  
    //building CscSegment objects
  cscsegment.set(seg_pos,seg_dir, seg2d_eta.chi2, seg2d_phi.chi2);
  cscsegment.setNHitEta(seg2d_eta.nhit);
  cscsegment.setNHitPhi(seg2d_phi.nhit);
  cscsegment_noip.set(seg_pos_noip,seg_dir_noip, seg2d_eta_noip.chi2, seg2d_phi_noip.chi2);
  cscsegment_noip.setNHitEta(seg2d_eta_noip.nhit);
  cscsegment_noip.setNHitPhi(seg2d_phi_noip.nhit);
  
  
  return ReturnCode::SUCCESS;
}


  ReturnCode CscSegmentMaker::make_2dsegment(int measphi, const localCscHit &ip_loc, const std::vector<localCscHit> hits_loc[4],
                                             local2dSegment &seg2d,
                                             local2dSegment &seg2d_ipremoved,
                                             int &nhit)
{
  
  int nohit=0;
  std::vector<int> hit_lyr;
  hit_lyr.clear();
  
    //counting layers without clusters
  for (int i=0; i<4; ++i ){
    if( 0==hits_loc[i].size() ) ++nohit;
  }
  
  std::vector<local2dSegment> seg2d_4hitCollection,seg2d_3hitCollection;
  seg2d_4hitCollection.clear(); seg2d_3hitCollection.clear();
  std::vector<localCscHit> hits_loc_copy[4];
  for (int i=0;i<4;++i) hits_loc_copy[i] = hits_loc[i];
  
  
  if(0==nohit){
    
    if( this->make_2dseg4hit(measphi, ip_loc, hits_loc_copy, seg2d_4hitCollection, nhit)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
    //hits already used are removed in hits_loc_copy
    ATH_MSG_DEBUG("display hits left");
    display_hits(hits_loc_copy);
    if( this->make_2dseg3hit(measphi, ip_loc, hits_loc_copy, seg2d_3hitCollection, nhit)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
    
  } else if(1==nohit) {
    
    if( this->make_2dseg3hit(measphi, ip_loc, hits_loc_copy, seg2d_3hitCollection, nhit)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
    
  } else {
    return ReturnCode::FAILURE;
  }
  
    //select the segment nearest to tgcroad.
  bool exist=false;
  seg2d.residual=99999.;
  for(unsigned int iseg4=0; iseg4< seg2d_4hitCollection.size(); ++iseg4){
    
    if( fabs( seg2d_4hitCollection[iseg4].residual ) < fabs(seg2d.residual) ){
      seg2d=seg2d_4hitCollection[iseg4];
      exist=true;
      ATH_MSG_DEBUG("seg2d.residual=" << seg2d.residual);
    }
  }
  for(unsigned int iseg3=0; iseg3< seg2d_3hitCollection.size(); ++iseg3){
    
    if( fabs( seg2d_3hitCollection[iseg3].residual ) < fabs(seg2d.residual) ){
      seg2d=seg2d_3hitCollection[iseg3];
      exist=true;
      ATH_MSG_DEBUG("seg2d.residual=" << seg2d.residual);
    }
  }
  
  if(false==exist) return ReturnCode::FAILURE;
  
  if( this->fit_clusters(measphi,seg2d.localHits,seg2d_ipremoved)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  
  return ReturnCode::SUCCESS;
  
}

ReturnCode CscSegmentMaker::make_2dseg4hit(int measphi, const localCscHit &ip_loc,
                                             std::vector<localCscHit>  hits_loc[4], //removing hits used in fit with 4 hits
                                             std::vector<local2dSegment> &seg2d_4hitCollection,
                                             int &nhit)
{
  
  std::vector<localCscHit> hit_fit;
  
  for (unsigned int ihit0=0; ihit0<hits_loc[0].size(); ++ihit0) {
    for (unsigned int ihit1=0; ihit1<hits_loc[1].size(); ++ihit1) {
      for (unsigned int ihit2=0; ihit2<hits_loc[2].size(); ++ihit2) {
        for (unsigned int ihit3=0; ihit3<hits_loc[3].size(); ++ihit3) {
          
            // set clusters
          hit_fit.clear();
          hit_fit.push_back(ip_loc);
          hit_fit.push_back(hits_loc[0][ihit0]);
          hit_fit.push_back(hits_loc[1][ihit1]);
          hit_fit.push_back(hits_loc[2][ihit2]);
          hit_fit.push_back(hits_loc[3][ihit3]);
          
            //segment fit
          local2dSegment seg2d_tmp;
          if( this->fit_clusters(measphi,hit_fit,seg2d_tmp)!=ReturnCode::SUCCESS ) continue;
          
          
          if(seg2d_tmp.chi2<m_max_chisquare){
            seg2d_tmp.localHits.push_back(hits_loc[0][ihit0]);
            seg2d_tmp.localHits.push_back(hits_loc[1][ihit1]);
            seg2d_tmp.localHits.push_back(hits_loc[2][ihit2]);
            seg2d_tmp.localHits.push_back(hits_loc[3][ihit3]);
              //disable hits
            hits_loc[0][ihit0].enabled=false;
            hits_loc[1][ihit1].enabled=false;
            hits_loc[2][ihit2].enabled=false;
            hits_loc[3][ihit3].enabled=false;
            
            seg2d_4hitCollection.push_back(seg2d_tmp);
            ATH_MSG_DEBUG("4hitsegment was made");
          }
          
        }
      }
    }
  }//loop over all combination of 4hits
  
  
    //make vector of hits left unused
  std::vector<localCscHit> hit_loc_left[4];
  for (unsigned int ilyr=0;ilyr<4; ++ilyr){
    for(unsigned int ihit=0; ihit < hits_loc[ilyr].size(); ++ihit){
      
      if ( true==hits_loc[ilyr][ihit].enabled ){
        hit_loc_left[ilyr].push_back(hits_loc[ilyr][ihit]);
      }
    }//loop over hits in layer
  }//loop over wire layer
  
  for(int ilyr=0; ilyr<4; ++ilyr) hits_loc[ilyr]=hit_loc_left[ilyr];//copying "objects" of vector
  
  nhit=4;
  
  return ReturnCode::SUCCESS;
  
}


ReturnCode CscSegmentMaker::make_2dseg3hit(int measphi, const localCscHit &ip_loc,
                                             const std::vector<localCscHit> hits_loc[4],
                                             std::vector<local2dSegment> &seg2d_3hitCollection,
                                             int &nhit)
{
  
  int empty_lyr=-1;
  int nohit=0;
  for (int ilyr=0; ilyr<4; ++ilyr ){
    if( 0==hits_loc[ilyr].size() ){
      ++nohit;
      empty_lyr=ilyr;
    }
  }
  
  if ( 1<nohit ) return ReturnCode::SUCCESS;
  
  std::vector<localCscHit> hit_fit;
  
  
  int hit_lyrA[4] = {1, 2, 3, 0};
  int hit_lyrB[4] = {2, 3, 0, 1};
  int hit_lyrC[4] = {3, 0, 1, 2};
  
  for(int lyrcomb=0; lyrcomb<4; ++lyrcomb){
    
    if(0<=empty_lyr && empty_lyr!=lyrcomb) continue;
    
    for (unsigned int ihitA=0; ihitA<hits_loc[hit_lyrA[lyrcomb]].size(); ++ihitA) {
      for (unsigned int ihitB=0; ihitB<hits_loc[hit_lyrB[lyrcomb]].size(); ++ihitB) {
        for (unsigned int ihitC=0; ihitC<hits_loc[hit_lyrC[lyrcomb]].size(); ++ihitC) {
          
          hit_fit.clear();
          hit_fit.push_back(ip_loc);
          hit_fit.push_back(hits_loc[hit_lyrA[lyrcomb]][ihitA]);
          hit_fit.push_back(hits_loc[hit_lyrB[lyrcomb]][ihitB]);
          hit_fit.push_back(hits_loc[hit_lyrC[lyrcomb]][ihitC]);
          
            //fit clusters
          local2dSegment seg2d_tmp;
          
          if( this->fit_clusters(measphi,hit_fit,seg2d_tmp)!=ReturnCode::SUCCESS ) continue;//for eta
          
          if(seg2d_tmp.chi2<2.0*m_max_chisquare/3.0){
            seg2d_tmp.localHits.push_back(hits_loc[hit_lyrA[lyrcomb]][ihitA]);
            seg2d_tmp.localHits.push_back(hits_loc[hit_lyrB[lyrcomb]][ihitB]);
            seg2d_tmp.localHits.push_back(hits_loc[hit_lyrC[lyrcomb]][ihitC]);
            seg2d_3hitCollection.push_back(seg2d_tmp);
            ATH_MSG_DEBUG("3hitsegment was made");
          }
        }
      }
    }
  }//loop over layer combination
  nhit=3;
  
  
  return ReturnCode::SUCCESS;
}


ReturnCode CscSegmentMaker::fit_clusters(int measphi, const std::vector<localCscHit> &hits_fit, local2dSegment &seg2d){
  
  
  double S=0.;
  double Sx=0.;
  double Sy=0.;
  double aStt=0.;
  int nhit=0;
  int nhit_with_ip=0;
  double Stt=0.;
  double Syy=0.;
  double Sxx=0.;
  double Sxy=0.;
  double a = 0.;
  double b = 0.;
  double aver_z=0.;
  double rp=0.;
  double rq=0.;
  double aver_res=0.;
  
  
  for (unsigned int ihit=0; ihit< hits_fit.size(); ++ihit) {
    

    double x= hits_fit[ihit].loc_z;
    double y= (measphi==0) ? hits_fit[ihit].loc_y : hits_fit[ihit].loc_x;
    double err= hits_fit[ihit].error;
    double w = 1./(err*err);
    //    ATH_MSG_DEBUG("local pos, z=" << hits_fit[ihit].loc_z << ", x,y=" << y << ", err=" << err << ", w=" << w << ", res=" << hits_fit[ihit].residual << ", measphi="<< hits_fit[ihit].measphi);
    S += w;
    Sx += w*x;
    Sy += w*y;
    aver_z += x;
    if( !hits_fit[ihit].isIP ){
        seg2d.stationname=hits_fit[ihit].stationname;
	aver_res += w*hits_fit[ihit].residual;
	rq += w;
	rp += w*x;
	++nhit;
    }
    ++nhit_with_ip;
  }//ihit
  seg2d.zshift=rp/rq;
  
  seg2d.residual=aver_res/rq;
  
  
  for (unsigned int ihit=0; ihit< hits_fit.size(); ++ihit) {
    
    double x= hits_fit[ihit].loc_z;
    double y= (measphi==0) ? hits_fit[ihit].loc_y : hits_fit[ihit].loc_x;
    double err = hits_fit[ihit].error;
    double w = 1./(err*err);// (ihit==0)?0.25:100.;//
    Stt += w*(x-Sx/S)*(x-Sx/S);
    aStt += w*y*(x-Sx/S);
    Sxx += w*x*x;
    Syy += w*y*y;
    Sxy += w*x*y;
    
  }//ihit
    
    
  
  if(nhit_with_ip>1){
    a = seg2d.a = aStt/Stt;
    b = seg2d.b = (Sy-Sx*a)/S;
    seg2d.nhit = nhit;
    
  }else{
    return ReturnCode::FAILURE;
  }
  seg2d.chi2  = Syy + a*a*Sxx + 2*b*a*Sx + b*b*S - 2*b*Sy - 2*a*Sxy;
  
  ATH_MSG_DEBUG( "fit: a=" << a << " b=" << b << " chi2=" << seg2d.chi2 );
  
  return ReturnCode::SUCCESS;
}



ReturnCode CscSegmentMaker::make_4dsegment(const local2dSegment &seg2d_eta,
                                             const local2dSegment &seg2d_phi,
                                             Amg::Vector3D &seg_pos,
                                             Amg::Vector3D &seg_dir)
{
  
  int nhit_e=seg2d_eta.nhit;
  int nhit_p=seg2d_phi.nhit;
  double a_eta=seg2d_eta.a;
  double b_eta=seg2d_eta.b;
  double a_phi=seg2d_phi.a;
  double b_phi=seg2d_phi.b;
  double rzshift=seg2d_eta.zshift;
  double phizshift=seg2d_phi.zshift;
  
  
  if (nhit_e<2   || nhit_p<2 ) return ReturnCode::FAILURE;
  
  

  ATH_MSG_DEBUG("rzshift=" << rzshift << " phizshift=" << phizshift << " diff=" << (rzshift-phizshift)
                << " angle=" << ( 0.5*M_PI - atan(a_phi) ) << " b_phi=" << b_phi /*<< " Newb_phi=" << Newb_phi*/ );
  
  
  double norm = sqrt( a_phi*a_phi + a_eta*a_eta + 1 );
  
  
  
  seg_dir = Amg::Vector3D(a_phi/norm, a_eta/norm, 1./norm);
  ATH_MSG_DEBUG("segdir_loc (px,py,pz)=" << "(" << seg_dir(Amg::px) << "," << seg_dir(Amg::py) << "," << seg_dir(Amg::pz) << ") a_eta="
                << a_eta << " a_phi=" << a_phi << " chi2_eta=" << seg2d_eta.chi2 << " chi2_phi=" << seg2d_phi.chi2 );
  
  double aver_z=0.5*(rzshift+phizshift);
  
  
  ATH_MSG_DEBUG(" new aver_z=" << aver_z );
  seg_pos = Amg::Vector3D(seg_dir(Amg::px)*aver_z/seg_dir(Amg::pz) + b_phi,  seg_dir(Amg::py)*aver_z/seg_dir(Amg::z) + b_eta,  aver_z);
  
  ATH_MSG_DEBUG( "myseg_loc  (x,y,z,px,py,pz)=(" << seg_pos(Amg::x) << "," << seg_pos(Amg::y) << "," << seg_pos(Amg::z)
		 << "," << seg_dir(Amg::px) << "," << seg_dir(Amg::py) << "," << seg_dir(Amg::pz) << ")");
  
  return ReturnCode::SUCCESS;
  
}



ReturnCode CscSegmentMaker::getModuleSP(int mod_hashes[2], const TrigL2MuonSA::TgcFitResult &tgcFitResult, int phibin, const MuonRoad &muroad, const int hash_clusters[32]){
  ATH_MSG_DEBUG("getModuleSP()");
  
  
  for(unsigned int imod=0; imod<32;++imod){
    
    if( 0==hash_clusters[imod] ) continue;
    int stationeta = m_cscregdict->stationEta(imod);
    int side = (muroad.side) ? 1 : -1;
    double phiMod = m_cscregdict->phiMod(imod);
    double dphi = m_util.calc_dphi(phiMod, tgcFitResult.phi);
    ATH_MSG_DEBUG("getModuleSP()::(phi,side) modlue:(" << phiMod << "," << stationeta << ") tgcroad:(" << tgcFitResult.phi << "," << side << ")");
    if( fabs(dphi)>M_PI/8. || side != stationeta) continue;

    
    if(7!=phibin){

      if(imod<16 && phibin>7){//small normal
        mod_hashes[0]=imod;
      } else if( imod >=16  && phibin<7){//large normal
        mod_hashes[0]=imod;
      } else {//overlap
        mod_hashes[1]=imod;
      }

    } else if (7==phibin) {

      if(imod<16 && 1==muroad.LargeSmall ){//small normal
        mod_hashes[0]=imod;
      } else if( imod >=16  && 0== muroad.LargeSmall){//large normal
        mod_hashes[0]=imod;
      } else {//overlap
        mod_hashes[1]=imod;
      }

    }
  }//for imod
  ATH_MSG_DEBUG("getModuleSP::Normal sector=" << mod_hashes[0] << " overlap sector=" << mod_hashes[1]);
  
  return ReturnCode::SUCCESS;
  
  
}


CscSegment CscSegmentMaker::segmentAtFirstLayer(int mod_hash, TrigL2MuonSA::CscSegment *mu_seg){
  
  
  double alpha = m_cscregdict->displacement(mod_hash);
  Amg::Vector3D n = m_cscregdict->nomalVector(mod_hash);
  double a0=mu_seg->px(), a1=mu_seg->py(), a2=mu_seg->pz();
  double b0=mu_seg->x(), b1=mu_seg->y(),b2=mu_seg->z();
  double t = ( alpha-(n(0)*b0+n(1)*b1+n(2)*b2) )/( n(0)*a0+n(1)*a1+n(2)*a2 );
  double x0=a0*t+b0,x1=a1*t+b1,x2=a2*t+b2;
  double chisquare=mu_seg->chiSquare();  
  double chisquare_phi=mu_seg->chiSquarePhi();  

  CscSegment cscsegment;
  cscsegment.set(x0,x1,x2,a0,a1,a2,chisquare, chisquare_phi);
  cscsegment.setNHitEta(mu_seg->nHitEta());  
  cscsegment.setNHitPhi(mu_seg->nHitPhi());  

  return cscsegment;
}



ReturnCode CscSegmentMaker::display_hits(const std::vector<localCscHit> localHits[4]){
  
  
  for(unsigned int ilyr=0; ilyr<4; ++ilyr){
    for (unsigned int ihit=0; ihit<localHits[ilyr].size(); ++ihit) {
      const localCscHit &locHit = localHits[ilyr][ihit];
      ATH_MSG_DEBUG("measphi=" << locHit.measphi << " wirelyr=" << ilyr+1
                    << " x=" << locHit.loc_x << " y=" << locHit.loc_y << " z=" << locHit.loc_z
                    << " err=" << locHit.error << " res=" << locHit.residual);
    }
  }
  
  
  return ReturnCode::SUCCESS;
}



CscSegment::CscSegment(){

  m_x=0.;
  m_y=0.;
  m_z=0.;
  m_px=0.;
  m_py=0.;
  m_pz=0.;
  m_slopeRZ=0.;
  m_interceptRZ=0.;
  m_nhit_eta=0;
  m_nhit_phi=0;
  m_chisquare=0.;
  m_chisquare_phi=0.;
}

ReturnCode CscSegment::set(double x, double y, double z, double px, double py, double pz, double chisquare, double chisquare_phi)
{
  
  m_x=x;
  m_y=y;
  m_z=z;
  m_px=px;
  m_py=py;
  m_pz=pz;
  double r = sqrt( x*x + y*y );
  m_slopeRZ = ( px*x+py*y )/( r*pz );
  m_interceptRZ = r - slopeRZ()*z;
  m_chisquare = chisquare;
  m_chisquare_phi = chisquare_phi;
  
  return ReturnCode::SUCCESS;
  
}

ReturnCode CscSegment::set( Amg::Vector3D &seg_pos, Amg::Vector3D &seg_dir, double chisquare, double chisquare_phi)
{
  
  m_x = seg_pos(Amg::x);
  m_y = seg_pos(Amg::y);
  m_z = seg_pos(Amg::z);
  m_px = seg_dir(Amg::px);
  m_py = seg_dir(Amg::py);
  m_pz = seg_dir(Amg::pz);
  double r = sqrt( x()*x() + y()*y() );
  m_slopeRZ = ( px()*x()+py()*y() )/( r*pz() );
  m_interceptRZ = r - slopeRZ()*z(); 
  m_chisquare = chisquare;
  m_chisquare_phi = chisquare_phi;
  
  return ReturnCode::SUCCESS;
  
}



bool CscSegment::isClean(){
  
  bool eta_clean=false;
  bool phi_clean=false;
  
  if (m_nhit_eta>1&&m_nhit_eta<5) eta_clean = true;
  if (m_nhit_phi>1&&m_nhit_phi<5) phi_clean = true;
  
  return eta_clean && phi_clean;
  
}


}//namespace TrigL2MuonSA
