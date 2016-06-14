/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/CscSegmentMaker.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include <Math/Vector3D.h>

#include "xAODTrigMuon/TrigMuonDefs.h"


static const InterfaceID IID_CscSegmentMaker("IID_CscSegmentMaker", 1, 0);



namespace TrigL2MuonSA {



const InterfaceID& CscSegmentMaker::interfaceID() { return IID_CscSegmentMaker; }


CscSegmentMaker::CscSegmentMaker(const std::string& type, const std::string& name, const IInterface*  parent)
  : AthAlgTool(type, name, parent), m_util(0), m_cscregdict("TrigL2MuonSA::CscRegDict/CscRegDict")
{
  declareInterface<TrigL2MuonSA::CscSegmentMaker>(this);
  declareProperty("CscRegDict", m_cscregdict);
}


CscSegmentMaker :: ~CscSegmentMaker(){}


StatusCode CscSegmentMaker :: initialize(){

  ATH_MSG_DEBUG("Initializing TrigL2MuonSA::CscSegmentMaker - package version " << PACKAGE_VERSION);
  
  StatusCode sc = AthAlgTool::initialize();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
    return sc;
  }
  
  if (!m_util) m_util = new UtilTools();
  sc = m_cscregdict.retrieve();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not initialize CscRegDict");
    return sc;
  }
  
  
  return StatusCode::SUCCESS;
}


StatusCode CscSegmentMaker :: finalize(){
  ATH_MSG_DEBUG("Finalizing TgcRoadDefiner - package version " << PACKAGE_VERSION);
  
  delete m_util; m_util=0;
  
  StatusCode sc = AthAlgTool::finalize();
  
  return sc;
}


ReturnCode CscSegmentMaker :: initializeRegDict(ToolHandle<CscRegDict> cscregdict){
  
  m_cscregdict=cscregdict;
  
  return ReturnCode::SUCCESS;
}



ReturnCode CscSegmentMaker :: FindSuperPointCsc( const TrigL2MuonSA::CscHits &cscHits,
                                                std::vector<TrigL2MuonSA::TrackPattern> &v_trackPatterns,
                                                const TrigL2MuonSA::TgcFitResult &tgcFitResult,
                                                const TrigL2MuonSA::MuonRoad &muroad)
{
  ATH_MSG_DEBUG( "FindSuperPointCsc" );
  
  if( !cscHits.empty() ){
    
    std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
    for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) { // loop for track candidates
      
        //get module hash  to read
      int hash_clusters[32]={0};
      
      
      
      TrigL2MuonSA::CscHits clusters[32][8];
      for(unsigned int iclu=0; iclu<cscHits.size(); ++iclu){
        const TrigL2MuonSA::CscHitData &cscHit = cscHits[iclu];
        
          //outlier or not
        double width = (cscHit.MeasuresPhi == 0 ) ? 100. : 250.;
        if ( width < fabs(cscHit.Residual) )  continue;
        
        
        int cathodelayer = 2*(cscHit.WireLayer-1)+  cscHit.MeasuresPhi;//cathodelayer is in [0,7]
        int module_hash = m_cscregdict->get_hash(cscHit.StationName, cscHit.StationEta,  cscHit.StationPhi);
        
        clusters[module_hash][cathodelayer].push_back( cscHit );
        
        hash_clusters[module_hash]=1;//set modules to read
        
      }//for clusters
      
      
        //decide which module to read
      int hashSP=0;
      if( getModuleSP( hashSP, (*itTrack).phiBin, muroad.LargeSmall, hash_clusters)!=ReturnCode::FAILURE ){
        
          //making segment
        CscSegment cscsegment, cscsegment_noip;
        if (this->make_segment(hashSP, clusters[hashSP] , cscsegment, cscsegment_noip) != ReturnCode::FAILURE  ){
          
          
          xAOD::L2MuonParameters::Chamber chamber = xAOD::L2MuonParameters::Chamber::CSC;
          TrigL2MuonSA::SuperPoint* superPoint = &((*itTrack).superPoints[chamber]);
          
            //tgcfitresult
          double tgcmid1phi = tgcFitResult.tgcMid1[1];
          double tgcmid1z = tgcFitResult.tgcMid1[3];
          double tgcmid2phi = tgcFitResult.tgcMid2[1];
          double tgcmid2z = tgcFitResult.tgcMid2[3];
          double phimiddle = m_util->average_phi(tgcmid1phi,tgcmid2phi);
          double dPhidz = ( m_util->calc_dphi( tgcmid2phi, tgcmid1phi) )/( tgcmid2z-tgcmid1z );
          double tgcmidZ = (tgcmid1z+tgcmid2z)/2.;
          
          
            //calculate gamma
          CscSegment cscsegment_ext = segmentAtFirstLayer(hashSP, &cscsegment);
          double phiMod = m_cscregdict->phiMod(hashSP);
          double CSCR=cscsegment_ext.x()*cos(phiMod)+cscsegment_ext.y()*sin(phiMod);
          double CSCZ=cscsegment_ext.z();
          double CSCRcor = CSCR + m_cscregdict->posCorrectionR(hashSP, (*itTrack).charge);
          double CSCZcor = CSCZ + m_cscregdict->posCorrectionZ(hashSP, (*itTrack).charge);
          double PhiAtCsc = phimiddle + (CSCZcor-tgcmidZ)*dPhidz;
          double CSCSPR = CSCRcor/cos( m_util->calc_dphi(PhiAtCsc,phiMod) );
          
          
          superPoint->Z = CSCZcor;
          superPoint->R = CSCSPR;
          superPoint->Alin = cscsegment.slopeRZ();
          superPoint->Blin = cscsegment.interceptRZ();
          superPoint->Npoint = cscsegment.nHitEta();
        }//if there is a segment.
      }//if there is the module in which segment will be made.
    }//loop over track pattern candidate
  }//if there are clusters, then make a segment
  
  
  return  ReturnCode::SUCCESS;
}




ReturnCode  CscSegmentMaker :: make_segment(int mod_hash, TrigL2MuonSA::CscHits clusters[8],
                                            CscSegment &cscsegment,
                                            CscSegment &cscsegment_noip)
{
  ATH_MSG_DEBUG(" make_segment is called");
  int side= ((mod_hash % 16)>7)? 1:-1;
  int signz = side;
  
  std::vector<localCscHit> hits_loc_eta[4], hits_loc_phi[4],emp;
  emp.clear();
  for(int i =0;i<4;++i){
    hits_loc_eta[i]=emp;
    hits_loc_phi[i]=emp;
  }
  
  unsigned int l1id=9999999;
  
  
  ROOT::Math::RotationZ rotZ( (-1)*(m_cscregdict->phiMod(mod_hash)) );
  ROOT::Math::RotationY rotY( (-1)*(m_cscregdict->actualAtanNormal(mod_hash)) );
  double rotpi = (side>0) ? 0 : M_PI;
  ROOT::Math::RotationZ rotPI( rotpi );
  
  
  for (unsigned int clyr=0; clyr<8; ++clyr) {
    for (unsigned int ihit=0; ihit<clusters[clyr].size(); ++ihit) {
      
      TrigL2MuonSA::CscHitData &cschit = clusters[clyr][ihit];
      double r = cschit.r;
      double phi = cschit.phi;
      double z = cschit.z;
        //l1id=cschit.m_l1id;
      
        //move to local coordinate system
      ROOT::Math::XYZVector vect(r*cos(phi),r*sin(phi),z);
      ROOT::Math::XYZVector loc_vect = rotPI(rotY(rotZ(vect)));
      
        //building localCscHit object
      localCscHit loc_hit;
      loc_hit.loc_x = loc_vect.X();
      loc_hit.loc_y = loc_vect.Y();
      loc_hit.loc_z = loc_vect.Z();
      loc_hit.error = cschit.eta;
      loc_hit.residual = cschit.Residual;
      loc_hit.measphi=cschit.MeasuresPhi;
      loc_hit.index4=ihit;
      loc_hit.enabled=true;
      loc_hit.stationname=cschit.StationName;
      ATH_MSG_DEBUG( "measphi=" << cschit.MeasuresPhi << " wirelyr=" << cschit.WireLayer << " x=" << loc_hit.loc_x << " y=" << loc_hit.loc_y << " z=" << loc_hit.loc_z 
		     <<  " err=" << loc_hit.error << " res=" << loc_hit.residual);
      
      
      if(0==loc_hit.measphi) {
        if(100.>fabs(loc_hit.residual)) hits_loc_eta[cschit.WireLayer-1].push_back( loc_hit );
      }
      if(1==loc_hit.measphi) {
        if(250.>fabs(loc_hit.residual))  hits_loc_phi[cschit.WireLayer-1].push_back( loc_hit );
      }
    }//ihit
  }//clyr
  
  ROOT::Math::XYZVector seg_pos_loc, seg_dir_loc,seg_pos_loc_noip, seg_dir_loc_noip;
  int nhite=0,nhitp=0;
  local2dSegment seg2d_eta ,seg2d_phi, seg2d_eta_noip, seg2d_phi_noip;
  
  
  if( this->make_2dsegment(signz, 0/*=measphi*/,hits_loc_eta, seg2d_eta, seg2d_eta_noip, nhite)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  if( this->make_2dsegment(signz, 1/*=measphi*/,hits_loc_phi, seg2d_phi,seg2d_phi_noip, nhitp)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  
  
  ATH_MSG_DEBUG("==include IP==");
  if( this->make_4dsegment(signz,seg2d_eta,seg2d_phi,seg_pos_loc,seg_dir_loc)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  ATH_MSG_DEBUG("==exclude IP==");
  if( this->make_4dsegment(signz,seg2d_eta_noip,seg2d_phi_noip,seg_pos_loc_noip,seg_dir_loc_noip)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  
    //go back to global coordinate system
  ROOT::Math::RotationZ rotZinv( m_cscregdict->phiMod(mod_hash) );
  ROOT::Math::RotationY rotYinv( m_cscregdict->actualAtanNormal(mod_hash) );
  
  ROOT::Math::XYZVector seg_pos = rotZinv(rotYinv(rotPI(seg_pos_loc)));
  ROOT::Math::XYZVector seg_dir = rotZinv(rotYinv(rotPI(seg_dir_loc)));
  ROOT::Math::XYZVector seg_pos_noip = rotZinv(rotYinv(rotPI(seg_pos_loc_noip)));
  ROOT::Math::XYZVector seg_dir_noip = rotZinv(rotYinv(rotPI(seg_dir_loc_noip)));
  
  
    //building CscSegment objects
  cscsegment.set(seg_pos,seg_dir,seg2d_eta.a, seg2d_eta.b);
  cscsegment.setNHitEta(seg2d_eta.nhit);
  cscsegment.setNHitPhi(seg2d_phi.nhit);
  cscsegment.setL1id(l1id);
  cscsegment_noip.set(seg_pos_noip,seg_dir_noip,seg2d_eta_noip.a, seg2d_eta_noip.b);
  cscsegment_noip.setNHitEta(seg2d_eta_noip.nhit);
  cscsegment_noip.setNHitPhi(seg2d_phi_noip.nhit);
  cscsegment_noip.setL1id(l1id);
  
  
  return ReturnCode::SUCCESS;
}


ReturnCode CscSegmentMaker :: make_2dsegment(int signz,int measphi,const std::vector<localCscHit> hits_loc[4],
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
    
    if( this->make_2dseg4hit(signz, measphi,hits_loc_copy, seg2d_4hitCollection, nhit)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
    ATH_MSG_DEBUG("display hits left");
    display_hits(hits_loc_copy);
    if( this->make_2dseg3hit(signz, measphi,hits_loc_copy, seg2d_3hitCollection, nhit)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
    
  } else if(1==nohit) {
    
    if( this->make_2dseg3hit(signz, measphi,hits_loc_copy, seg2d_3hitCollection, nhit)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
    
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
    }
  }
  for(unsigned int iseg3=0; iseg3< seg2d_3hitCollection.size(); ++iseg3){
    
    if( fabs( seg2d_3hitCollection[iseg3].residual ) < fabs(seg2d.residual) ){
      seg2d=seg2d_3hitCollection[iseg3];
      exist=true;
    }
  }
  
  if(false==exist) return ReturnCode::FAILURE;
  
  if( this->fit_clusters(measphi,seg2d.localHits,seg2d_ipremoved)!=ReturnCode::SUCCESS ) return ReturnCode::FAILURE;
  
  return ReturnCode :: SUCCESS;
  
}

ReturnCode CscSegmentMaker :: make_2dseg4hit(int /*signz*/,int measphi,
                                             std::vector<localCscHit>  hits_loc[4],
                                             std::vector<local2dSegment> &seg2d_4hitCollection,
                                             int &nhit)
{
  
  std::vector<localCscHit> hit_fit;
  
    //IP
  localCscHit ip;
  ip.loc_x = ip.loc_y = ip.loc_z = 0.;
  ip.error = 250.;
  ip.residual = 0.;
  ip.measphi=-1;
  
  for (unsigned int ihit0=0; ihit0<hits_loc[0].size(); ++ihit0) {
    for (unsigned int ihit1=0; ihit1<hits_loc[1].size(); ++ihit1) {
      for (unsigned int ihit2=0; ihit2<hits_loc[2].size(); ++ihit2) {
        for (unsigned int ihit3=0; ihit3<hits_loc[3].size(); ++ihit3) {
          
            // set clusters
          hit_fit.clear();
          hit_fit.push_back(ip);
          hit_fit.push_back(hits_loc[0][ihit0]);
          hit_fit.push_back(hits_loc[1][ihit1]);
          hit_fit.push_back(hits_loc[2][ihit2]);
          hit_fit.push_back(hits_loc[3][ihit3]);
          
            //segment fit
          local2dSegment seg2d_tmp;
          if( this->fit_clusters(measphi,hit_fit,seg2d_tmp)!=ReturnCode::SUCCESS ) continue;
          
          
          if(seg2d_tmp.chi2<16.){
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


ReturnCode CscSegmentMaker :: make_2dseg3hit(int /*signz*/,int measphi,
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
  
  
    //IPconstraint
  localCscHit ip;
  ip.loc_x = ip.loc_y = ip.loc_z = 0.;
  ip.error = 250.;
  ip.residual = 0.;
  ip.measphi=-1;
  
  int hit_lyrA[4] = {1, 2, 3, 0};
  int hit_lyrB[4] = {2, 3, 0, 1};
  int hit_lyrC[4] = {3, 0, 1, 2};
  
  for(int lyrcomb=0; lyrcomb<4; ++lyrcomb){
    
    if(0<=empty_lyr && empty_lyr!=lyrcomb) continue;
    
    for (unsigned int ihitA=0; ihitA<hits_loc[hit_lyrA[lyrcomb]].size(); ++ihitA) {
      for (unsigned int ihitB=0; ihitB<hits_loc[hit_lyrB[lyrcomb]].size(); ++ihitB) {
        for (unsigned int ihitC=0; ihitC<hits_loc[hit_lyrC[lyrcomb]].size(); ++ihitC) {
          
          hit_fit.clear();
          hit_fit.push_back(ip);
          hit_fit.push_back(hits_loc[hit_lyrA[lyrcomb]][ihitA]);
          hit_fit.push_back(hits_loc[hit_lyrB[lyrcomb]][ihitB]);
          hit_fit.push_back(hits_loc[hit_lyrC[lyrcomb]][ihitC]);
          
            //fit clusters
          local2dSegment seg2d_tmp;
          
          if( this->fit_clusters(measphi,hit_fit,seg2d_tmp)!=ReturnCode::SUCCESS ) continue;//for eta
          
          if(seg2d_tmp.chi2<2.0*16.0/3.0){
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


ReturnCode CscSegmentMaker :: fit_clusters(int measphi, const std::vector<localCscHit> &hits_fit, local2dSegment &seg2d){
  
  
  double S=0.;
  double Sx=0.;
  double Sy=0.;
  double aStt=0.;
  int nhit_e=0;
  double Stt=0.;
  double Syy=0.;
  double Sxx=0.;
  double Sxy=0.;
  double a_eta = 0.;
  double b_eta = 0.;
  double aver_z=0.;
  double rp=0.;
  double rq=0.;
  double aver_res=0.;
  
  
  for (unsigned int ihit=0; ihit< hits_fit.size(); ++ihit) {
    
    seg2d.stationname=hits_fit[ihit].stationname;
    double x= hits_fit[ihit].loc_z;
    double y= (measphi==0) ? hits_fit[ihit].loc_x : hits_fit[ihit].loc_y;
    double err= hits_fit[ihit].error;
    double w = (ihit==0)?0.25:1./(err*err);
    ATH_MSG_DEBUG("local pos, z=" << hits_fit[ihit].loc_z << ", x,y=" << y << ", err=" << err << ", w=" << w << ", res=" << hits_fit[ihit].residual << ", measphi="<< hits_fit[ihit].measphi);
    S += w;
    Sx += w*x;
    Sy += w*y;
    aver_z += x;
    aver_res += w*hits_fit[ihit].residual;
    rq += w;
    rp += w*x;
    ++nhit_e;
  }//ihit
  seg2d.zshift=rp/rq;
  
  seg2d.residual=aver_res/S;
  int outlier=1;
  double displace=99999.;
  
  
  for (unsigned int ihit=0; ihit< hits_fit.size(); ++ihit) {
    
    double x= hits_fit[ihit].loc_z;
    double y= (measphi==0) ? hits_fit[ihit].loc_x : hits_fit[ihit].loc_y;
    double err = hits_fit[ihit].error;
    double w = (ihit==0)?0.25:1./(err*err);
    Stt += w*(x-Sx/S)*(x-Sx/S);
    aStt += w*y*(x-Sx/S);
    Sxx += w*x*x;
    Syy += w*y*y;
    Sxy += w*x*y;
    double displace_tmp = fabs( seg2d.residual - w*x);
    
    if( 0<ihit && displace>displace_tmp ){
      displace = displace_tmp;
      outlier = ihit;
    }
  }//ihit
  
  seg2d.outlier=outlier;
  
  int nhite=nhit_e-1;
  
  if(nhit_e>1){
    a_eta = seg2d.a = aStt/Stt;
    b_eta = seg2d.b = (Sy-Sx*a_eta)/S;
    seg2d.nhit = nhite;
    
  }else{
    return ReturnCode::FAILURE;
  }
  seg2d.chi2  = Syy + a_eta*a_eta*Sxx + 2*b_eta*a_eta*Sx + b_eta*b_eta*S - 2*b_eta*Sy - 2*a_eta*Sxy;
  
  ATH_MSG_DEBUG( "fit: a=" << a_eta << " b=" << b_eta << " chi2=" << seg2d.chi2 );
  
  return ReturnCode::SUCCESS;
}



ReturnCode CscSegmentMaker :: make_4dsegment(int /*signz*/, const local2dSegment &seg2d_eta,
                                             const local2dSegment &seg2d_phi,
                                             ROOT::Math::XYZVector &seg_pos,
                                             ROOT::Math::XYZVector &seg_dir)
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
  
  
  double Newb_phi = b_phi;  // - ( 0.5*M_PI - atan(a_phi) )*(rzshift-phizshift);
  ATH_MSG_DEBUG("rzshift=" << rzshift << " phizshift=" << phizshift << " diff=" << (rzshift-phizshift)
                << " angle=" << ( 0.5*M_PI - atan(a_phi) ) << " b_phi=" << b_phi << " Newb_phi=" << Newb_phi );
  
  
  double norm = sqrt( a_eta*a_eta + a_phi*a_phi + 1 );
  
  
  
  seg_dir.SetCoordinates(a_eta/norm, a_phi/norm, 1./norm);
  ATH_MSG_DEBUG("segdir_loc (px,py,pz)=" << "(" << seg_dir.X() << "," << seg_dir.Y() << "," << seg_dir.Z() << ") a_eta="
                << a_eta << " a_phi=" << a_phi << " chi2_eta=" << seg2d_eta.chi2 << " chi2_phi=" << seg2d_phi.chi2 );
  
  double aver_z=( seg2d_eta.stationname==50 )? m_cscregdict->displacement(0):m_cscregdict->displacement(16);
  
  
  ATH_MSG_DEBUG(" new aver_z=" << aver_z );
  seg_pos.SetCoordinates(seg_dir.X()*aver_z/seg_dir.Z() + b_eta,  seg_dir.Y()*aver_z/seg_dir.Z() + Newb_phi,  aver_z);
  
  ATH_MSG_DEBUG( "myseg_loc  (x,y,z,px,py,pz)=(" << seg_pos.X() << "," << seg_pos.Y() << "," << seg_pos.Z()
                << "," << seg_dir.X() << "," << seg_dir.Y() << "," << seg_dir.Z() << ")");
  
  return ReturnCode::SUCCESS;
  
}



ReturnCode CscSegmentMaker :: getModuleSP(int &mod_hash, int phibin, int LargeSmall, const int hash_clusters[32]){
  ATH_MSG_DEBUG("getModuleSP");
  
  
  for(unsigned int imod=0; imod<32;++imod){
    
    if( 0==hash_clusters[imod] ) continue;
    
    if(7!=phibin){
      if(imod<16 && phibin>7){//small
        mod_hash=imod;
        return ReturnCode::SUCCESS;
      }
      if( imod >=16  && phibin<7){//large
        mod_hash=imod;
        return ReturnCode::SUCCESS;
      }
    } else if (7==phibin) {
      if(imod<16 && 1==LargeSmall ){//small
        mod_hash=imod;
        return ReturnCode::SUCCESS;
      }
      if( imod >=16  && 0== LargeSmall){//large
        mod_hash=imod;
        return ReturnCode::SUCCESS;
      }
      
    }
  }//for imod
  
  
  return ReturnCode::FAILURE;
  
  
}


CscSegment CscSegmentMaker :: segmentAtFirstLayer(int mod_hash, TrigL2MuonSA::CscSegment *mu_seg){
  
  
  double alpha = m_cscregdict->displacement(mod_hash);
  Amg::Vector3D n = m_cscregdict->nomalVector(mod_hash);
  double a0=mu_seg->px(), a1=mu_seg->py(), a2=mu_seg->pz();
  double b0=mu_seg->x(), b1=mu_seg->y(),b2=mu_seg->z();
  double t = ( alpha-(n(0)*b0+n(1)*b1+n(2)*b2) )/( n(0)*a0+n(1)*a1+n(2)*a2 );
  double x0=a0*t+b0,x1=a1*t+b1,x2=a2*t+b2;
  double phiMod=m_cscregdict->phiMod(mod_hash);
  double slope=( (a0*cos(phiMod)+a1*sin(phiMod))/a2 );
  double intercept= x0*cos(phiMod)+x1*sin(phiMod) + slope*x2  ;
  
  CscSegment cscsegment;
  cscsegment.set(x0,x1,x2,a0,a1,a2,slope,intercept);
  
  return cscsegment;
}



ReturnCode CscSegmentMaker :: display_hits(const std::vector<localCscHit> localHits[4]){
  
  
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



CscSegment :: CscSegment(){}

CscSegment :: ~CscSegment(){}



ReturnCode CscSegment :: set(double x, double y, double z, double px, double py, double pz,
                             double slopeRZ, double interceptRZ){
  
  m_x=x;
  m_y=y;
  m_z=z;
  m_px=px;
  m_py=py;
  m_pz=pz;
  m_slopeRZ=slopeRZ;
  m_interceptRZ=interceptRZ;
  
  return ReturnCode::SUCCESS;
  
}

ReturnCode CscSegment :: set( ROOT::Math::XYZVector &seg_pos, ROOT::Math::XYZVector &seg_dir,
                             double slopeRZ, double interceptRZ)
{
  
  m_x=seg_pos.X();
  m_y=seg_pos.Y();
  m_z=seg_pos.Z();
  m_px=seg_dir.X();
  m_py=seg_dir.Y();
  m_pz=seg_dir.Z();
  m_slopeRZ=slopeRZ;
  m_interceptRZ=interceptRZ;
  
  
  return ReturnCode::SUCCESS;
  
}



bool CscSegment :: isClean(){
  
  bool eta_clean=false;
  bool phi_clean=false;
  
  if (m_nhit_eta>1&&m_nhit_eta<5) eta_clean = true;
  if (m_nhit_phi>1&&m_nhit_phi<5) phi_clean = true;
  
  return eta_clean && phi_clean;
  
}


}//namespace TrigL2MuonSA
