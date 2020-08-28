/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// Author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "VxVertex/RecVertex.h"
#include "CxxUtils/sincos.h"
//-------------------------------------------------
// Other stuff
#include <cmath>


namespace InDet{  

  double InDetVKalVxInJetTool::rankBTrk(double TrkPt, double JetPt, double Signif)  const
  {
    double p_prob=0.;
    double s_prob=0.;
    if(TrkPt > 0. &&  JetPt > 0.){
      double coeffPt=10.;
      double pfrac=(TrkPt-m_cutPt)/std::sqrt(JetPt);
      p_prob= pfrac/(coeffPt+pfrac);    // Old probability to be b-track
      if (Signif == 0.) return p_prob; //should be less than some epsilon?
    } 
    if( Signif != 0.) {
      double coeffSig=1.0;
      s_prob=(Signif-coeffSig)/Signif;   // Old probability to be b-track
      if(TrkPt + JetPt == 0.) return s_prob;
    }
    //----------------------------------Initial definition of selective variable
    double contrib=0.4;
    return (1.+contrib)*std::max(s_prob,0.)+(1.-contrib)*p_prob;
  }


  TLorentzVector  InDetVKalVxInJetTool::getBDir( const xAOD::TrackParticle* trk1,
                                                 const xAOD::TrackParticle* trk2,
                                                 const xAOD::Vertex    & PrimVrt,
						 Amg::Vector3D &V1, Amg::Vector3D &V2)
  const
  { // B hadron flight direction based on 2 separate tracks and PV. Calculated via plane-plane crossing
    Amg::Vector3D PVRT(PrimVrt.x(),PrimVrt.y(),PrimVrt.z());
//----------------------------------------------------------------------------
    Amg::Vector3D   pnt1=trk1->perigeeParameters().position()-PVRT;
    Amg::Vector3D   mom1((trk1->p4()).Px(),(trk1->p4()).Py(),(trk1->p4()).Pz());
    Amg::Vector3D   pnt2=trk2->perigeeParameters().position()-PVRT;
    Amg::Vector3D   mom2((trk2->p4()).Px(),(trk2->p4()).Py(),(trk2->p4()).Pz());
    pnt1.normalize(); pnt2.normalize(); mom1.normalize(); mom2.normalize();
//------------------------------------------------------------------------
    const float dRLim=0.4;
    Amg::Vector3D norm1=pnt1.cross(mom1);
    Amg::Vector3D norm2=pnt2.cross(mom2);
    Amg::Vector3D t=norm1.cross(norm2); t.normalize(); if(t.dot(mom1+mom2)<0.) t*=-1.;
    double aveP=(trk1->p4()+trk2->p4()).P()/2.;
    TLorentzVector tl;  tl.SetXYZM(t.x()*aveP,t.y()*aveP,t.z()*aveP,139.57); //Crossing line of 2 planes
    if( tl.DeltaR(trk1->p4()) >dRLim || tl.DeltaR(trk2->p4()) >dRLim ) {V1*=0.; V2*=0.; return tl;}//Too big dR between tracks and found "B line"
//------------------------------------------------------------------------
    double X;
    pnt1=trk1->perigeeParameters().position()-PVRT;
    pnt2=trk2->perigeeParameters().position()-PVRT;
    std::abs(mom1[1]*t[2]-mom1[2]*t[1])>std::abs(mom1[0]*t[2]-mom1[2]*t[0]) ? X=(t[1]*pnt1[2]-t[2]*pnt1[1])/(mom1[1]*t[2]-mom1[2]*t[1])
                                                                            : X=(t[0]*pnt1[2]-t[2]*pnt1[0])/(mom1[0]*t[2]-mom1[2]*t[0]);
    V1=pnt1+mom1*X;   // First particle vertex
    std::abs(mom2[1]*t[2]-mom2[2]*t[1])>std::abs(mom2[0]*t[2]-mom2[2]*t[0]) ? X=(t[1]*pnt2[2]-t[2]*pnt2[1])/(mom2[1]*t[2]-mom2[2]*t[1])
                                                                            : X=(t[0]*pnt2[2]-t[2]*pnt2[0])/(mom2[0]*t[2]-mom2[2]*t[0]);
    V2=pnt2+mom2*X;   // Second particle vertex
//------------------------------------------------------------------------
    if(V1.dot(t)<0. && V2.dot(t)<0.) {V1*=0.;V2*=0.;}        // Check correctness of topology
    else                             {V1+=PVRT; V2+=PVRT;}   // Transform to detector frame
//------------------------------------------------------------------------
    return tl;
  }

  void InDetVKalVxInJetTool::printWrkSet(const std::vector<WrkVrt> *, const std::string& ) const {
/*  void InDetVKalVxInJetTool::printWrkSet(const std::vector<WrkVrt> *wrkVrtSet, const std::string name) const {
    int nGoodV=0;
    for(int iv=0; iv<(int)wrkVrtSet->size(); iv++) {
      std::cout<<name
      <<"= "<<(*wrkVrtSet)[iv].vertex[0]
      <<", "<<(*wrkVrtSet)[iv].vertex[1]
      <<", "<<(*wrkVrtSet)[iv].vertex[2]
      <<" NTrk="<<(*wrkVrtSet)[iv].selTrk.size()
      <<" is good="<<std::boolalpha<<(*wrkVrtSet)[iv].Good<<std::noboolalpha
      <<"  Chi2="<<(*wrkVrtSet)[iv].chi2
      <<"  Mass="<<(*wrkVrtSet)[iv].vertexMom.M()
      <<"  detached="<<(*wrkVrtSet)[iv].detachedTrack
      <<"  proj.dist="<<(*wrkVrtSet)[iv].projectedVrt
      <<" trk=";
      for(int kk=0; kk<(int)(*wrkVrtSet)[iv].selTrk.size(); kk++) {
                std::cout<<", "<<(*wrkVrtSet)[iv].selTrk[kk];}
      //for(int kk=0; kk<(int)(*wrkVrtSet)[iv].selTrk.size(); kk++) {
      //          std::cout<<", "<<momAtVrt((*wrkVrtSet)[iv].TrkAtVrt[kk]).Perp();}
      std::cout<<'\n';
      if((*wrkVrtSet)[iv].Good)nGoodV++;
    }
    std::cout<<name<<" N="<<nGoodV<<'\n';*/
  }

               /*  Technicalities */
  double InDetVKalVxInJetTool::projSV_PV(const Amg::Vector3D & SV, const xAOD::Vertex & PV, const TLorentzVector & Jet) const
  {  
     TVector3 SV_PV( SV.x()-PV.x(), SV.y()-PV.y(), SV.z()-PV.z() );
     return Jet.Vect().Unit()*SV_PV.Unit();
  }

  bool InDetVKalVxInJetTool::insideMatLayer(float xvt,float yvt) const
  {
        float Dist2DBP=std::sqrt( (xvt-m_beampipeX)*(xvt-m_beampipeX) + (yvt-m_beampipeY)*(yvt-m_beampipeY) ); 
        float Dist2DBL=std::sqrt( (xvt-m_xLayerB)*(xvt-m_xLayerB) + (yvt-m_yLayerB)*(yvt-m_yLayerB) ); 
        float Dist2DL1=std::sqrt( (xvt-m_xLayer1)*(xvt-m_xLayer1) + (yvt-m_yLayer1)*(yvt-m_yLayer1) );
        float Dist2DL2=std::sqrt( (xvt-m_xLayer2)*(xvt-m_xLayer2) + (yvt-m_yLayer2)*(yvt-m_yLayer2) );
        if(m_existIBL){              // 4-layer pixel detector
               if( std::abs(Dist2DBP-m_beampipeR)< 1.0)  return true;           // Beam Pipe removal  
               if( std::abs(Dist2DBL-m_rLayerB)  < 2.5)     return true;
               if( std::abs(Dist2DL1-m_rLayer1)  < 3.0)      return true;
               if( std::abs(Dist2DL2-m_rLayer2)  < 3.0)      return true;
               //if( std::abs(Dist2DL2-m_rLayer3)  < 4.0)      return true;
        }else{                       // 3-layer pixel detector
               if( std::abs(Dist2DBP-m_beampipeR)< 1.5)  return true;           // Beam Pipe removal  
               if( std::abs(Dist2DBL-m_rLayerB)  < 3.5)     return true;
               if( std::abs(Dist2DL1-m_rLayer1)  < 4.0)      return true;
               if( std::abs(Dist2DL2-m_rLayer2)  < 5.0)      return true;
        }
        return false; 
  }

  double InDetVKalVxInJetTool::vrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double>& SecVrtErr, double& Signif)
  const
  {
    double distx =  PrimVrt.position().x()- SecVrt.x();
    double disty =  PrimVrt.position().y()- SecVrt.y();
    double distz =  PrimVrt.position().z()- SecVrt.z();

    Amg::MatrixX  PrimCovMtx=PrimVrt.covariancePosition();  //Create
    PrimCovMtx(0,0) += SecVrtErr[0];
    PrimCovMtx(0,1) += SecVrtErr[1];
    PrimCovMtx(1,0) += SecVrtErr[1];
    PrimCovMtx(1,1) += SecVrtErr[2];
    PrimCovMtx(0,2) += SecVrtErr[3];
    PrimCovMtx(2,0) += SecVrtErr[3];
    PrimCovMtx(1,2) += SecVrtErr[4];
    PrimCovMtx(2,1) += SecVrtErr[4];
    PrimCovMtx(2,2) += SecVrtErr[5];

    Amg::MatrixX  WgtMtx = PrimCovMtx.inverse();

    Signif = distx*WgtMtx(0,0)*distx
            +disty*WgtMtx(1,1)*disty
            +distz*WgtMtx(2,2)*distz
         +2.*distx*WgtMtx(0,1)*disty
         +2.*distx*WgtMtx(0,2)*distz
         +2.*disty*WgtMtx(1,2)*distz;
    Signif=std::sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    return std::sqrt(distx*distx+disty*disty+distz*distz);
  }
  
  double InDetVKalVxInJetTool::vrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double>& SecVrtErr, double& Signif)
  const
  {
    double distx =  PrimVrt.x()- SecVrt.x();
    double disty =  PrimVrt.y()- SecVrt.y();
    double distz =  PrimVrt.z()- SecVrt.z();


    AmgSymMatrix(3)  PrimCovMtx=PrimVrt.covariancePosition();  //Create
    PrimCovMtx(0,0) += SecVrtErr[0];
    PrimCovMtx(0,1) += SecVrtErr[1];
    PrimCovMtx(1,0) += SecVrtErr[1];
    PrimCovMtx(1,1) += SecVrtErr[2];
    PrimCovMtx(0,2) += SecVrtErr[3];
    PrimCovMtx(2,0) += SecVrtErr[3];
    PrimCovMtx(1,2) += SecVrtErr[4];
    PrimCovMtx(2,1) += SecVrtErr[4];
    PrimCovMtx(2,2) += SecVrtErr[5];

    AmgSymMatrix(3)  WgtMtx = PrimCovMtx.inverse();

    Signif = distx*WgtMtx(0,0)*distx
            +disty*WgtMtx(1,1)*disty
            +distz*WgtMtx(2,2)*distz
         +2.*distx*WgtMtx(0,1)*disty
         +2.*distx*WgtMtx(0,2)*distz
         +2.*disty*WgtMtx(1,2)*distz;
    Signif=std::sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    return std::sqrt(distx*distx+disty*disty+distz*distz);
  }

  double InDetVKalVxInJetTool::vrtVrtDist2D(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double>& SecVrtErr, double& Signif)
  const
  {
    double distx =  PrimVrt.x()- SecVrt.x();
    double disty =  PrimVrt.y()- SecVrt.y();


    AmgSymMatrix(3)  PrimCovMtx=PrimVrt.covariancePosition();  //Create
    AmgSymMatrix(2)  CovMtx;
    CovMtx(0,0) = PrimCovMtx(0,0) + SecVrtErr[0];
    CovMtx(0,1) = PrimCovMtx(0,1) + SecVrtErr[1];
    CovMtx(1,0) = PrimCovMtx(1,0) + SecVrtErr[1];
    CovMtx(1,1) = PrimCovMtx(1,1) + SecVrtErr[2];

    AmgSymMatrix(2)  WgtMtx = CovMtx.inverse();

    Signif = distx*WgtMtx(0,0)*distx
            +disty*WgtMtx(1,1)*disty
         +2.*distx*WgtMtx(0,1)*disty;
    Signif=std::sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    return std::sqrt(distx*distx+disty*disty);
  }

//--------------------------------------------------
// Significance along jet direction
//--------------------------------------------------
  double InDetVKalVxInJetTool::vrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double>& SecVrtErr, const TLorentzVector & JetDir)
  const
  {
    Amg::Vector3D jetDir(JetDir.Vect().Unit().X(), JetDir.Vect().Unit().Y(), JetDir.Vect().Unit().Z());
    double projDist=(SecVrt-PrimVrt.position()).dot(jetDir);
    double distx =  jetDir.x()*projDist;
    double disty =  jetDir.y()*projDist;
    double distz =  jetDir.z()*projDist;

    AmgSymMatrix(3)  PrimCovMtx=PrimVrt.covariancePosition();  //Create
    PrimCovMtx(0,0) += SecVrtErr[0];
    PrimCovMtx(0,1) += SecVrtErr[1];
    PrimCovMtx(1,0) += SecVrtErr[1];
    PrimCovMtx(1,1) += SecVrtErr[2];
    PrimCovMtx(0,2) += SecVrtErr[3];
    PrimCovMtx(2,0) += SecVrtErr[3];
    PrimCovMtx(1,2) += SecVrtErr[4];
    PrimCovMtx(2,1) += SecVrtErr[4];
    PrimCovMtx(2,2) += SecVrtErr[5];

    AmgSymMatrix(3)  WgtMtx = PrimCovMtx.inverse();

    double Signif = distx*WgtMtx(0,0)*distx
                   +disty*WgtMtx(1,1)*disty
                   +distz*WgtMtx(2,2)*distz
                +2.*distx*WgtMtx(0,1)*disty
                +2.*distx*WgtMtx(0,2)*distz
                +2.*disty*WgtMtx(1,2)*distz;
    Signif=std::sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    if(projDist<0)Signif=-Signif;
    return Signif;
  }

//--------------------------------------------------
// Significance along jet direction
//--------------------------------------------------
  double InDetVKalVxInJetTool::vrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double>& SecVrtErr, const TLorentzVector & JetDir)
  const
  {
    Amg::Vector3D jetDir(JetDir.Vect().Unit().X(), JetDir.Vect().Unit().Y(), JetDir.Vect().Unit().Z());
    double projDist=(SecVrt-PrimVrt.position()).dot(jetDir);
    double distx =  jetDir.x()*projDist;
    double disty =  jetDir.y()*projDist;
    double distz =  jetDir.z()*projDist;

    AmgSymMatrix(3)  PrimCovMtx=PrimVrt.covariancePosition();  //Create
    PrimCovMtx(0,0) += SecVrtErr[0];
    PrimCovMtx(0,1) += SecVrtErr[1];
    PrimCovMtx(1,0) += SecVrtErr[1];
    PrimCovMtx(1,1) += SecVrtErr[2];
    PrimCovMtx(0,2) += SecVrtErr[3];
    PrimCovMtx(2,0) += SecVrtErr[3];
    PrimCovMtx(1,2) += SecVrtErr[4];
    PrimCovMtx(2,1) += SecVrtErr[4];
    PrimCovMtx(2,2) += SecVrtErr[5];

    AmgSymMatrix(3)  WgtMtx = PrimCovMtx.inverse();

    double Signif = distx*WgtMtx(0,0)*distx
                   +disty*WgtMtx(1,1)*disty
                   +distz*WgtMtx(2,2)*distz
                +2.*distx*WgtMtx(0,1)*disty
                +2.*distx*WgtMtx(0,2)*distz
                +2.*disty*WgtMtx(1,2)*distz;
    Signif=std::sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    if(projDist<0)Signif=-Signif;
    return Signif;
  }

  double InDetVKalVxInJetTool::vrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>  & VrtErr1,
                                          const Amg::Vector3D & Vrt2, const std::vector<double>  & VrtErr2)
  const
  {
    double Signif;
    double distx =  Vrt1.x()- Vrt2.x();
    double disty =  Vrt1.y()- Vrt2.y();
    double distz =  Vrt1.z()- Vrt2.z();

    AmgSymMatrix(3)  PrimCovMtx;  //Create
    PrimCovMtx(0,0) =                   VrtErr1[0]+VrtErr2[0];
    PrimCovMtx(0,1) = PrimCovMtx(1,0) = VrtErr1[1]+VrtErr2[1];
    PrimCovMtx(1,1) =                   VrtErr1[2]+VrtErr2[2];
    PrimCovMtx(0,2) = PrimCovMtx(2,0) = VrtErr1[3]+VrtErr2[3];
    PrimCovMtx(1,2) = PrimCovMtx(2,1) = VrtErr1[4]+VrtErr2[4];
    PrimCovMtx(2,2) =                   VrtErr1[5]+VrtErr2[5];

    AmgSymMatrix(3)  WgtMtx = PrimCovMtx.inverse();


    Signif =   distx*WgtMtx(0,0)*distx
              +disty*WgtMtx(1,1)*disty
              +distz*WgtMtx(2,2)*distz
           +2.*distx*WgtMtx(0,1)*disty
           +2.*distx*WgtMtx(0,2)*distz
           +2.*disty*WgtMtx(1,2)*distz;
    Signif=std::sqrt(Signif);
    if(Signif != Signif)  Signif = 0.;
    return Signif;
  }
//


//----------------------------
//   Vertex error along radius
//----------------------------
  double InDetVKalVxInJetTool::vrtRadiusError(const Amg::Vector3D & SecVrt, const std::vector<double>  & VrtErr) const
  {
    double DirX=SecVrt.x(), DirY=SecVrt.y(); 
    double Covar =    DirX*VrtErr[0]*DirX
                  +2.*DirX*VrtErr[1]*DirY
                     +DirY*VrtErr[2]*DirY;
    Covar /= DirX*DirX + DirY*DirY;
    Covar=std::sqrt(Covar);
    if(Covar != Covar)  Covar = 0.;
    return Covar;
  }



  double InDetVKalVxInJetTool::coneDist(const AmgVector(5) & vectPerig, const TLorentzVector & jetDir)
  const
  {
  
  	  double  etaTr = -std::log(std::tan(vectPerig[3]/2.));
	  double  etaJet = jetDir.PseudoRapidity();
	  double  adphi = std::abs(jetDir.Phi()-vectPerig[2]);
	  while(adphi> M_PI)adphi-=2.*M_PI;
 	  return  std::sqrt(adphi*adphi + (etaJet-etaTr)*(etaJet-etaTr));
  }


    /* Invariant mass calculation for V0 decays*/
    /* Gives correct mass assignment in case of nonequal masses*/


   double InDetVKalVxInJetTool::massV0(std::vector< std::vector<double> >& trkAtVrt,
                               double massP, double massPi )
   const
   {
        double ap1=1./std::abs(trkAtVrt[0][2]);
	double ap2=1./std::abs(trkAtVrt[1][2]);
        CxxUtils::sincos phi1  (trkAtVrt[0][0]);
        CxxUtils::sincos theta1(trkAtVrt[0][1]);
        CxxUtils::sincos phi2  (trkAtVrt[1][0]);
        CxxUtils::sincos theta2(trkAtVrt[1][1]);
        double px = phi1.cs*theta1.sn*ap1 
                  + phi2.cs*theta2.sn*ap2;
        double py = phi1.sn*theta1.sn*ap1 
                  + phi2.sn*theta2.sn*ap2;
        double pz =         theta1.cs*ap1 
                  +         theta2.cs*ap2;
        double ee= (ap1 > ap2) ? 
            (std::sqrt(ap1*ap1+massP*massP)+std::sqrt(ap2*ap2+massPi*massPi)):
            (std::sqrt(ap2*ap2+massP*massP)+std::sqrt(ap1*ap1+massPi*massPi));
        double test=(ee-pz)*(ee+pz)-px*px-py*py;
        return test>0 ? std::sqrt(test) : 0.; 
    }


//
// Search for outliers using track Chi2 and track Ranking
   int InDetVKalVxInJetTool::findMax( std::vector<double>& chi2PerTrk, std::vector<float> & rank)
   const
   { 
      double chi2Ref=0.;
      int position=-1;
      if( chi2PerTrk.size() < 1 ) return position ;
      for (int i=0; i< (int)chi2PerTrk.size(); i++){
	if(chi2PerTrk[i]/std::max(rank[i],(float)0.1) > chi2Ref) { chi2Ref=chi2PerTrk[i]/std::max(rank[i],(float)0.1); position=i;}
      }
      return position;
   }      
  

//  Function returns a transverse momentum of track w/r some direction
//
  double InDetVKalVxInJetTool::pTvsDir(const Amg::Vector3D &Dir, const std::vector< double >& inpTrk) 
  const
  {
     double Norm=std::sqrt(Dir.x()*Dir.x() + Dir.y()*Dir.y() + Dir.z()*Dir.z());
     double sx=Dir.x()/Norm; double sy=Dir.y()/Norm; double sz=Dir.z()/Norm;

     double px=0.,py=0.,pz=0.; double scale;
     double api=1./std::abs(inpTrk[2]);
     CxxUtils::sincos phi  (inpTrk[0]);
     CxxUtils::sincos theta(inpTrk[1]);

     px = phi.cs * theta.sn*api;
     py = phi.sn * theta.sn*api;
     pz =          theta.cs*api;
       scale = px*sx + py*sy + pz*sz;
     px -= sx*scale;
     py -= sy*scale; 
     pz -= sz*scale;
     return std::sqrt( px*px +py*py + pz*pz );
   }

  TLorentzVector InDetVKalVxInJetTool::totalMom(const std::vector<const Trk::Perigee*>& InpTrk) 
  const
  {
     AmgVector(5) VectPerig; VectPerig.setZero();
     double px=0.,py=0.,pz=0.,ee=0.;
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       if( InpTrk[i] == NULL ){ continue; } 
       VectPerig = InpTrk[i]->parameters(); 
       double api=1./std::abs(VectPerig[4]);
       CxxUtils::sincos phi  (VectPerig[2]);
       CxxUtils::sincos theta(VectPerig[3]);
       px += phi.cs * theta.sn*api;
       py += phi.sn * theta.sn*api;
       pz +=          theta.cs*api;
       ee += std::sqrt( api*api + m_massPi*m_massPi);
     }
     return TLorentzVector(px,py,pz,ee); 
   }

  TLorentzVector InDetVKalVxInJetTool::totalMom(const std::vector<const xAOD::TrackParticle*>& InpTrk) 
  const
  {
     TLorentzVector sum(0.,0.,0.,0.); 
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       if( InpTrk[i] == NULL ) continue; 
       sum += InpTrk[i]->p4();
     }
     return sum; 
   }


  TLorentzVector InDetVKalVxInJetTool::momAtVrt(const std::vector< double >& inpTrk) 
  const
  {
     double api=1./std::abs(inpTrk[2]);
     CxxUtils::sincos phi  (inpTrk[0]);
     CxxUtils::sincos theta(inpTrk[1]);
     double px = phi.cs * theta.sn*api;
     double py = phi.sn * theta.sn*api;
     double pz =          theta.cs*api;
     double ee = std::sqrt( api*api + m_massPi*m_massPi);
     return TLorentzVector(px,py,pz,ee); 
   }
//
//-- Perigee in xAOD::TrackParticle
//

  const Trk::Perigee* InDetVKalVxInJetTool::GetPerigee( const xAOD::TrackParticle* i_ntrk) const
  {
       return &(i_ntrk->perigeeParameters());
  }



  StatusCode  InDetVKalVxInJetTool::VKalVrtFitFastBase(
    const std::vector<const xAOD::TrackParticle*>& listTrk,
    Amg::Vector3D& FitVertex,
    Trk::IVKalState& istate) const
  {
    return m_fitSvc->VKalVrtFitFast(listTrk, FitVertex, istate);
  }


  StatusCode InDetVKalVxInJetTool::VKalVrtFitBase(const std::vector<const xAOD::TrackParticle*> & listPart,
                                                  Amg::Vector3D&                   Vertex,
                                                  TLorentzVector&                  Momentum,
                                                  long int&                        Charge,
                                                  std::vector<double>&             ErrorMatrix,
                                                  std::vector<double>&             Chi2PerTrk,
                                                  std::vector< std::vector<double> >& TrkAtVrt,
                                                  double& Chi2,
                                                  Trk::IVKalState& istate,
                                                  bool ifCovV0) const
  {
     std::vector<const xAOD::NeutralParticle*> netralPartDummy(0);
     return m_fitSvc->VKalVrtFit( listPart, netralPartDummy,Vertex, Momentum, Charge,
                                  ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2,
                                  istate, ifCovV0 );

  }

  StatusCode InDetVKalVxInJetTool::GetTrkFitWeights(std::vector<double> & wgt,
                                                    const Trk::IVKalState& istate) const
  {
    return m_fitSvc->VKalGetTrkWeights(wgt, istate);
  }
/*************************************************************************************************************/
  void   InDetVKalVxInJetTool::getPixelLayers(const xAOD::TrackParticle* Part, int &blHit, int &l1Hit, int &l2Hit, int &nLays ) const
  {
    	blHit=l1Hit=l2Hit=nLays=0; 
        if(m_existIBL){              // 4-layer pixel detector
          uint8_t IBLhit,BLhit,NPlay,IBLexp,BLexp;
          if(!Part->summaryValue( IBLhit,  xAOD::numberOfInnermostPixelLayerHits) )        IBLhit = 0;
          if(!Part->summaryValue(  BLhit,  xAOD::numberOfNextToInnermostPixelLayerHits) )   BLhit = 0;
          if(!Part->summaryValue(  NPlay,  xAOD::numberOfContribPixelLayers) )              NPlay = 0;
          if(!Part->summaryValue( IBLexp,  xAOD::expectInnermostPixelLayerHit) )           IBLexp = 0;
          if(!Part->summaryValue(  BLexp,  xAOD::expectNextToInnermostPixelLayerHit) )      BLexp = 0;
          blHit=IBLhit; if( IBLexp==0 ) blHit=-1;
          l1Hit= BLhit; if(  BLexp==0 ) l1Hit=-1;
          nLays=NPlay;
          //if((IBLhit+BLhit) == 0){      //no hits in IBL and BL  VK OLD VERSION WITHOUT PATTERN AVAILABLE
	  //   if(NPlay>=1) { l2Hit=1; }  // at least one of remaining layers is fired
	  //   if(NPlay==0) { l2Hit=0; }
          //}else if( IBLhit*BLhit == 0){ // one hit in IBL and BL. Others are presumably also fired
	  //   if(NPlay>=2) { l2Hit=1; }
	  //   if(NPlay<=1) { l2Hit=0; }  // no fired layer except for IBL/BL
          //}
          uint32_t HitPattern=Part->hitPattern();
	  l2Hit=0; if( HitPattern&((1<<Trk::pixelBarrel2)) ) l2Hit=1;
	  //   bitH=HitPattern&((int)std::pow(2,Trk::pixelBarrel1));
        } else {                     // 3-layer pixel detector
          uint8_t BLhit,NPlay,NHoles,IBLhit;
          if(!Part->summaryValue( BLhit,  xAOD::numberOfBLayerHits) )          BLhit = 0;
          if(!Part->summaryValue(IBLhit,  xAOD::numberOfInnermostPixelLayerHits) )  IBLhit = 0; // Some safety
          BLhit=BLhit>IBLhit ? BLhit : IBLhit;                                                  // Some safety
          if(!Part->summaryValue( NPlay,  xAOD::numberOfContribPixelLayers) )  NPlay = 0;
          if(!Part->summaryValue(NHoles,  xAOD::numberOfPixelHoles) )         NHoles = 0;
          blHit=BLhit;  //B-layer hit is fired. Presumable all other layers are also fired.
          nLays=NPlay;
          //if (BLhit==0) {   //B-layer hit is absent. 
	  //   if(NPlay>=2) { l1Hit=l2Hit=1;}
	  //   if(NPlay==0) { l1Hit=l2Hit=0;}
	  //   if(NPlay==1) { 
	  //     if( NHoles==0) {l1Hit=0; l2Hit=1;}  
	  //     if( NHoles>=1) {l1Hit=1; l2Hit=0;}  
          //   }
          //}
          uint32_t HitPattern=Part->hitPattern();
	  l1Hit=0; if( HitPattern&((1<<Trk::pixelBarrel1)) ) l1Hit=1;
	  l2Hit=0; if( HitPattern&((1<<Trk::pixelBarrel2)) ) l2Hit=1;
        }

  }
  void InDetVKalVxInJetTool::getPixelProblems(const xAOD::TrackParticle* Part, int &splshIBL, int &splshBL ) const
  {
    	splshIBL=splshBL=0;
        if(m_existIBL){              // 4-layer pixel detector
          uint8_t share,split;
	  //if(!Part->summaryValue( IBLout,  xAOD::numberOfInnermostPixelLayerOutliers ) )        IBLout = 0;
	  if(!Part->summaryValue( share,   xAOD::numberOfInnermostPixelLayerSharedHits ) )   share = 0;
	  if(!Part->summaryValue( split,  xAOD::numberOfInnermostPixelLayerSplitHits ) )        split = 0;
          splshIBL=share+split;
	  if(!Part->summaryValue( share,   xAOD::numberOfNextToInnermostPixelLayerSharedHits ) )   share = 0;
	  if(!Part->summaryValue( split,  xAOD::numberOfNextToInnermostPixelLayerSplitHits ) )        split = 0;
          splshBL=share+split;
       }
  }
  void   InDetVKalVxInJetTool::getPixelDiscs(const xAOD::TrackParticle* Part, int &d0Hit, int &d1Hit, int &d2Hit) const
  {
        uint32_t HitPattern=Part->hitPattern();
	d0Hit=0; if( HitPattern&((1<<Trk::pixelEndCap0)) ) d0Hit=1;
	d1Hit=0; if( HitPattern&((1<<Trk::pixelEndCap1)) ) d1Hit=1;
	d2Hit=0; if( HitPattern&((1<<Trk::pixelEndCap2)) ) d2Hit=1;
  }

/*************************************************************************************************************/

  Amg::MatrixX InDetVKalVxInJetTool::makeVrtCovMatrix( std::vector<double> & errorMatrix )
  const
  {
      Amg::MatrixX vrtCovMtx(3,3);  
      vrtCovMtx(0,0)                  = errorMatrix[0];
      vrtCovMtx(0,1) = vrtCovMtx(1,0) = errorMatrix[1];
      vrtCovMtx(1,1)                  = errorMatrix[2];
      vrtCovMtx(0,2) = vrtCovMtx(2,0) = errorMatrix[3];
      vrtCovMtx(1,2) = vrtCovMtx(2,1) = errorMatrix[4];
      vrtCovMtx(2,2)                  = errorMatrix[5];
      return vrtCovMtx;
  }

  void InDetVKalVxInJetTool::fillVrtNTup( std::vector<Vrt2Tr>  & all2TrVrt)
  const
  {	 if(!m_curTup)return;
         int ipnt=0;
         for(auto & vrt : all2TrVrt) {
	   if(ipnt==DevTuple::maxNTrk)break;
	   m_curTup->VrtDist2D[ipnt]=vrt.fitVertex.perp();
	   m_curTup->VrtSig3D[ipnt]=vrt.signif3D;
	   m_curTup->VrtSig2D[ipnt]=vrt.signif2D;
	   m_curTup->itrk[ipnt]=vrt.i;
	   m_curTup->jtrk[ipnt]=vrt.j;
	   m_curTup->mass[ipnt]=vrt.momentum.M();
	   m_curTup->Chi2[ipnt]=vrt.chi2;
	   m_curTup->badVrt[ipnt]=vrt.badVrt;
	   m_curTup->VrtDR[ipnt]=vrt.dRSVPV;
           ipnt++; m_curTup->nVrt=ipnt;
        }
  } 

  void InDetVKalVxInJetTool::fillNVrtNTup(std::vector<WrkVrt> & VrtSet, std::vector< std::vector<float> > & trkScore,
                                          const xAOD::Vertex  & PV, const TLorentzVector & JetDir)
  const
  {	 if(!m_curTup)return;
         int ipnt=0;
         TLorentzVector VertexMom;
         for(auto & vrt : VrtSet) {
	   if(ipnt==DevTuple::maxNVrt)break;
	   m_curTup->NVrtDist2D[ipnt]=vrt.vertex.perp();
	   m_curTup->NVrtNT[ipnt]=vrt.selTrk.size();
           m_curTup->NVrtTrkI[ipnt]=vrt.selTrk[0];
	   m_curTup->NVrtM[ipnt]=vrt.vertexMom.M();
	   m_curTup->NVrtChi2[ipnt]=vrt.chi2;
           float maxW=0., sumW=0.;
           for(auto trk : vrt.selTrk){ sumW+=trkScore[trk][0]; maxW=std::max(trkScore[trk][0], maxW);}
	   m_curTup->NVrtMaxW[ipnt]=maxW;
	   m_curTup->NVrtAveW[ipnt]=sumW/vrt.selTrk.size();
           TLorentzVector SVPV(vrt.vertex.x()-PV.x(),vrt.vertex.y()-PV.y(),vrt.vertex.z()-PV.z(),1.);
           m_curTup->NVrtDR[ipnt]=JetDir.DeltaR(SVPV);
           VertexMom += vrt.vertexMom;
           ipnt++; m_curTup->nNVrt=ipnt;
        }
        m_curTup->TotM=VertexMom.M();
  } 


  int InDetVKalVxInJetTool::getIdHF(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( !tplink.isValid() ) return 0;
        if( TP->auxdata< float >( "truthMatchProbability" ) < 0.75 ) return 0;
        if( (*tplink)->barcode() > 200000) return 0;
        if( (*tplink)->hasProdVtx()){
          if( (*tplink)->prodVtx()->nIncomingParticles()==1){
             int PDGID1=0, PDGID2=0, PDGID3=0;
	     const xAOD::TruthParticle * parTP1=getPreviousParent(*tplink, PDGID1);
	     const xAOD::TruthParticle * parTP2=0;
	     int noBC1=notFromBC(PDGID1);
             if(noBC1)  parTP2 = getPreviousParent(parTP1, PDGID2);
	     int noBC2=notFromBC(PDGID2);
             if(noBC2 && parTP2) getPreviousParent(parTP2, PDGID3);
	     int noBC3=notFromBC(PDGID3);
             if(noBC1 && noBC2 && noBC3)return 0;
             return 1;  //This is a reconstructed track from B/C decays
      } } }
      return 0;
  }

  int InDetVKalVxInJetTool::notFromBC(int PDGID) const {
    int noBC=0;
    if(PDGID<=0)return 1;
    if(PDGID>600 && PDGID<4000)noBC=1;
    if(PDGID<400 || PDGID>5600)noBC=1;
    if(PDGID==513  || PDGID==523  || PDGID==533  || PDGID==543)noBC=1;  //Remove tracks from B* (they are in PV)
    if(PDGID==5114 || PDGID==5214 || PDGID==5224 || PDGID==5314 || PDGID==5324)noBC=1; //Remove tracks from B_Barions* (they are in PV)
  //if(PDGID==413  || PDGID==423  || PDGID==433 )continue;  //Keep tracks from D* (they are from B vertex)
  //if(PDGID==4114 || PDGID==4214 || PDGID==4224 || PDGID==4314 || PDGID==4324)continue;
    return noBC;
  }
  const xAOD::TruthParticle * InDetVKalVxInJetTool::getPreviousParent(const xAOD::TruthParticle * child, int & ParentPDG) const {
    ParentPDG=0;
    if( child->hasProdVtx() ){
       if( child->prodVtx()->nIncomingParticles()==1 ){
            ParentPDG = abs((*(child->prodVtx()->incomingParticleLinks())[0])->pdgId());
            return *(child->prodVtx()->incomingParticleLinks())[0];
       }
    }
    return 0;
  }


  int InDetVKalVxInJetTool::getG4Inter(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( tplink.isValid() && (*tplink)->barcode()>200000) return 1;
      }
      return 0;
  }
  int InDetVKalVxInJetTool::getMCPileup(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( !tplink.isValid() ) return 1;
      } else { return 1; }
      return 0;
  }

}  //end namespace
