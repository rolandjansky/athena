/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
//-------------------------------------------------
// Other stuff
#include <cmath>


namespace InDet{  

  double InDetVKalVxInJetTool::RankBTrk(double TrkPt, double JetPt, double Signif)  const
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


  TLorentzVector  InDetVKalVxInJetTool::GetBDir( const xAOD::TrackParticle* trk1,
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
    fabs(mom1[1]*t[2]-mom1[2]*t[1])>fabs(mom1[0]*t[2]-mom1[2]*t[0]) ? X=(t[1]*pnt1[2]-t[2]*pnt1[1])/(mom1[1]*t[2]-mom1[2]*t[1])
                                                                    : X=(t[0]*pnt1[2]-t[2]*pnt1[0])/(mom1[0]*t[2]-mom1[2]*t[0]);
    V1=pnt1+mom1*X;   // First particle vertex
    fabs(mom2[1]*t[2]-mom2[2]*t[1])>fabs(mom2[0]*t[2]-mom2[2]*t[0]) ? X=(t[1]*pnt2[2]-t[2]*pnt2[1])/(mom2[1]*t[2]-mom2[2]*t[1])
                                                                    : X=(t[0]*pnt2[2]-t[2]*pnt2[0])/(mom2[0]*t[2]-mom2[2]*t[0]);
    V2=pnt2+mom2*X;   // Second particle vertex
//------------------------------------------------------------------------
    if(V1.dot(t)<0. && V2.dot(t)<0.) {V1*=0.;V2*=0.;}        // Check correctness of topology
    else                             {V1+=PVRT; V2+=PVRT;}   // Transform to detector frame
//------------------------------------------------------------------------
    return tl;
  }

  void InDetVKalVxInJetTool::printWrkSet(const std::vector<WrkVrt> *, const std::string& ) const {
/*  void InDetVKalVxInJetTool::printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name) const {
    int nGoodV=0;
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
      std::cout<<name
      <<"= "<<(*WrkVrtSet)[iv].vertex[0]
      <<", "<<(*WrkVrtSet)[iv].vertex[1]
      <<", "<<(*WrkVrtSet)[iv].vertex[2]
      <<" NTrk="<<(*WrkVrtSet)[iv].SelTrk.size()
      <<" is good="<<std::boolalpha<<(*WrkVrtSet)[iv].Good<<std::noboolalpha
      <<"  Chi2="<<(*WrkVrtSet)[iv].Chi2
      <<"  Mass="<<(*WrkVrtSet)[iv].vertexMom.M()
      <<"  detached="<<(*WrkVrtSet)[iv].detachedTrack
      <<"  proj.dist="<<(*WrkVrtSet)[iv].ProjectedVrt
      <<" trk=";
      for(int kk=0; kk<(int)(*WrkVrtSet)[iv].SelTrk.size(); kk++) {
                std::cout<<", "<<(*WrkVrtSet)[iv].SelTrk[kk];}
      //for(int kk=0; kk<(int)(*WrkVrtSet)[iv].SelTrk.size(); kk++) {
      //          std::cout<<", "<<MomAtVrt((*WrkVrtSet)[iv].TrkAtVrt[kk]).Perp();}
      std::cout<<'\n';
      if((*WrkVrtSet)[iv].Good)nGoodV++;
    }
    std::cout<<name<<" N="<<nGoodV<<'\n';*/
  }

               /*  Technicalities */
  double InDetVKalVxInJetTool::ProjSV_PV(const Amg::Vector3D & SV, const xAOD::Vertex & PV, const TLorentzVector & Jet) const
  {  
     TVector3 SV_PV( SV.x()-PV.x(), SV.y()-PV.y(), SV.z()-PV.z() );
     return Jet.Vect().Unit()*SV_PV.Unit();
  }

  bool InDetVKalVxInJetTool::insideMatLayer(float xvt,float yvt) const
  {
        float Dist2DBP=sqrt( (xvt-m_beampipeX)*(xvt-m_beampipeX) + (yvt-m_beampipeY)*(yvt-m_beampipeY) ); 
        float Dist2DBL=sqrt( (xvt-m_xLayerB)*(xvt-m_xLayerB) + (yvt-m_yLayerB)*(yvt-m_yLayerB) ); 
        float Dist2DL1=sqrt( (xvt-m_xLayer1)*(xvt-m_xLayer1) + (yvt-m_yLayer1)*(yvt-m_yLayer1) );
        float Dist2DL2=sqrt( (xvt-m_xLayer2)*(xvt-m_xLayer2) + (yvt-m_yLayer2)*(yvt-m_yLayer2) );
        if(m_existIBL){              // 4-layer pixel detector
               if( fabs(Dist2DBP-m_beampipeR)< 1.0)  return true;           // Beam Pipe removal  
               if( fabs(Dist2DBL-m_rLayerB)  < 2.5)     return true;
               if( fabs(Dist2DL1-m_rLayer1)  < 3.0)      return true;
               if( fabs(Dist2DL2-m_rLayer2)  < 3.0)      return true;
               //if( fabs(Dist2DL2-m_rLayer3)  < 4.0)      return true;
        }else{                       // 3-layer pixel detector
               if( fabs(Dist2DBP-m_beampipeR)< 1.5)  return true;           // Beam Pipe removal  
               if( fabs(Dist2DBL-m_rLayerB)  < 3.5)     return true;
               if( fabs(Dist2DL1-m_rLayer1)  < 4.0)      return true;
               if( fabs(Dist2DL2-m_rLayer2)  < 5.0)      return true;
        }
        return false; 
  }

  double InDetVKalVxInJetTool::VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
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
    Signif=sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    return sqrt(distx*distx+disty*disty+distz*distz);
  }
  
  double InDetVKalVxInJetTool::VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
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
    Signif=sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    return sqrt(distx*distx+disty*disty+distz*distz);
  }

  double InDetVKalVxInJetTool::VrtVrtDist2D(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
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
    Signif=sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    return sqrt(distx*distx+disty*disty);
  }

//--------------------------------------------------
// Significance along jet direction
//--------------------------------------------------
  double InDetVKalVxInJetTool::VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
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
    Signif=sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    if(projDist<0)Signif=-Signif;
    return Signif;
  }

//--------------------------------------------------
// Significance along jet direction
//--------------------------------------------------
  double InDetVKalVxInJetTool::VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
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
    Signif=sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
    if(projDist<0)Signif=-Signif;
    return Signif;
  }

  double InDetVKalVxInJetTool::VrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>  & VrtErr1,
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
    Signif=sqrt(Signif);
    if(Signif != Signif)  Signif = 0.;
    return Signif;
  }
//


//----------------------------
//   Vertex error along radius
//----------------------------
  double InDetVKalVxInJetTool::VrtRadiusError(const Amg::Vector3D & SecVrt, const std::vector<double>  & VrtErr) const
  {
    double DirX=SecVrt.x(), DirY=SecVrt.y(); 
    double Covar =    DirX*VrtErr[0]*DirX
                  +2.*DirX*VrtErr[1]*DirY
                     +DirY*VrtErr[2]*DirY;
    Covar /= DirX*DirX + DirY*DirY;
    Covar=sqrt(Covar);
    if(Covar != Covar)  Covar = 0.;
    return Covar;
  }



  double InDetVKalVxInJetTool::ConeDist(const AmgVector(5) & VectPerig, const TLorentzVector & JetDir)
  const
  {
  
  	  double  etaTr = -log(tan(VectPerig[3]/2.));
	  double  etaJet = JetDir.PseudoRapidity();
	  double  adphi = fabs(JetDir.Phi()-VectPerig[2]);
	  while(adphi> M_PI)adphi-=2.*M_PI;
 	  return  sqrt(adphi*adphi + (etaJet-etaTr)*(etaJet-etaTr));
  }


    /* Invariant mass calculation for V0 decays*/
    /* Gives correct mass assignment in case of nonequal masses*/


   double InDetVKalVxInJetTool::massV0(std::vector< std::vector<double> >& TrkAtVrt,
                               double massP, double massPi )
   const
   {
        double p1=fabs(TrkAtVrt[0][2]); double p2=fabs(TrkAtVrt[1][2]);
        double px = cos(TrkAtVrt[0][0])*sin(TrkAtVrt[0][1])/p1 
                  + cos(TrkAtVrt[1][0])*sin(TrkAtVrt[1][1])/p2;
        double py = sin(TrkAtVrt[0][0])*sin(TrkAtVrt[0][1])/p1 
                  + sin(TrkAtVrt[1][0])*sin(TrkAtVrt[1][1])/p2;
        double pz =                     cos(TrkAtVrt[0][1])/p1 
                  +                     cos(TrkAtVrt[1][1])/p2;
        double ee= (1./p1 > 1./p2) ? 
            (sqrt(1./p1/p1+massP*massP)+sqrt(1./p2/p2+massPi*massPi)):
            (sqrt(1./p2/p2+massP*massP)+sqrt(1./p1/p1+massPi*massPi));
        double test=(ee-pz)*(ee+pz)-px*px-py*py;
        return test>0 ? sqrt(test) : 0.; 
    }



   int InDetVKalVxInJetTool::FindMax( std::vector<double>& Chi2PerTrk, std::vector<float> & Rank)
   const
   { 
      double Chi2Ref=0.;
      int Position=-1;
      if( Chi2PerTrk.size() < 1 ) return Position ;
      for (int i=0; i< (int)Chi2PerTrk.size(); i++){
	if(Chi2PerTrk[i]/std::max(Rank[i],(float)0.1) > Chi2Ref) { Chi2Ref=Chi2PerTrk[i]/std::max(Rank[i],(float)0.1); Position=i;}
      }
      return Position;
   }      
  
 
//  Function returns a transverse mass of the track system
//
  double InDetVKalVxInJetTool::TotalTMom(const std::vector<const Trk::Perigee*>& InpTrk) 
  const
  {
     TLorentzVector SumP = TotalMom(InpTrk); 
     double Norm=sqrt(SumP.Px()*SumP.Px() + SumP.Py()*SumP.Py() + SumP.Pz()*SumP.Pz());
     double sx=SumP.Px()/Norm; double sy=SumP.Py()/Norm; double sz=SumP.Pz()/Norm;

     double px=0.,py=0.,pz=0.,ee=0.; double scale;
     AmgVector(5) VectPerig; VectPerig.setZero();
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       if( InpTrk[i]== NULL ){ continue; } 
       VectPerig = InpTrk[i]->parameters(); 
       px = cos ( VectPerig[2]) * sin(VectPerig[3])/fabs(VectPerig[4]);
       py = sin ( VectPerig[2]) * sin(VectPerig[3])/fabs(VectPerig[4]);
       pz =                       cos(VectPerig[3])/fabs(VectPerig[4]);
       scale = px*sx + py*sy + pz*sz;
       px -= sx*scale;
       py -= sy*scale; 
       pz -= sz*scale;
       ee += sqrt( px*px +py*py + pz*pz + m_massPi*m_massPi);
     }
     return ee; 
   }


//  Function returns a transverse mass of the track system
//
  double InDetVKalVxInJetTool::TotalTVMom(const Amg::Vector3D &Dir, const std::vector<const Trk::Perigee*>& InpTrk) 
  const
  {
   //double Norm=sqrt(pow(Dir.x(),2.) + pow(Dir.y(),2.) + pow(Dir.z(),2.) );
     double Norm=sqrt(Dir.x()*Dir.x() + Dir.y()*Dir.y() + Dir.z()*Dir.z());
     double sx=Dir.x()/Norm; double sy=Dir.y()/Norm; double sz=Dir.z()/Norm;

     double px=0.,py=0.,pz=0.,ee=0.; double scale;
     AmgVector(5) VectPerig; VectPerig.setZero();
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       if( InpTrk[i] == NULL ){ continue; } 
       VectPerig = InpTrk[i]->parameters(); 
       px = cos ( VectPerig[2]) * sin(VectPerig[3])/fabs(VectPerig[4]);
       py = sin ( VectPerig[2]) * sin(VectPerig[3])/fabs(VectPerig[4]);
       pz =                       cos(VectPerig[3])/fabs(VectPerig[4]);
       scale = px*sx + py*sy + pz*sz;
       px -= sx*scale;
       py -= sy*scale; 
       pz -= sz*scale;
       ee += sqrt( px*px +py*py + pz*pz + m_massPi*m_massPi);
     }
     return ee; 
   }

//  Function returns a transverse momentum of track w/r some direction
//
  double InDetVKalVxInJetTool::pTvsDir(const Amg::Vector3D &Dir, const std::vector< double >& InpTrk) 
  const
  {
     double Norm=sqrt(Dir.x()*Dir.x() + Dir.y()*Dir.y() + Dir.z()*Dir.z());
     double sx=Dir.x()/Norm; double sy=Dir.y()/Norm; double sz=Dir.z()/Norm;

     double px=0.,py=0.,pz=0.; double scale;
     px = cos ( InpTrk[0]) * sin(InpTrk[1])/fabs(InpTrk[2]);
     py = sin ( InpTrk[0]) * sin(InpTrk[1])/fabs(InpTrk[2]);
     pz =                    cos(InpTrk[1])/fabs(InpTrk[2]);
       scale = px*sx + py*sy + pz*sz;
     px -= sx*scale;
     py -= sy*scale; 
     pz -= sz*scale;
     return sqrt( px*px +py*py + pz*pz );
   }

  TLorentzVector InDetVKalVxInJetTool::TotalMom(const std::vector<const Trk::Perigee*>& InpTrk) 
  const
  {
     AmgVector(5) VectPerig; VectPerig.setZero();
     double px=0.,py=0.,pz=0.,ee=0.;
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       if( InpTrk[i] == NULL ){ continue; } 
       VectPerig = InpTrk[i]->parameters(); 
       px += cos ( VectPerig[2]) * sin(VectPerig[3])/fabs(VectPerig[4]);
       py += sin ( VectPerig[2]) * sin(VectPerig[3])/fabs(VectPerig[4]);
       pz +=                       cos(VectPerig[3])/fabs(VectPerig[4]);
       ee += sqrt( 1./(VectPerig[4]*VectPerig[4]) + m_massPi*m_massPi);
     }
     return TLorentzVector(px,py,pz,ee); 
   }

  TLorentzVector InDetVKalVxInJetTool::TotalMom(const std::vector<const xAOD::TrackParticle*>& InpTrk) 
  const
  {
     TLorentzVector sum(0.,0.,0.,0.); 
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       if( InpTrk[i] == NULL ) continue; 
       sum += InpTrk[i]->p4();
     }
     return sum; 
   }


  TLorentzVector InDetVKalVxInJetTool::MomAtVrt(const std::vector< double >& InpTrk) 
  const
  {
     double px = cos ( InpTrk[0]) * sin(InpTrk[1])/fabs(InpTrk[2]);
     double py = sin ( InpTrk[0]) * sin(InpTrk[1])/fabs(InpTrk[2]);
     double pz =                    cos(InpTrk[1])/fabs(InpTrk[2]);
     double ee = sqrt( px*px + py*py + pz*pz + m_massPi*m_massPi);
     return TLorentzVector(px,py,pz,ee); 
   }
//
//-- Perigee in Rec::TrackParticle, Trk::Track and xAOD::TrackParticle
//

  const Trk::Perigee* InDetVKalVxInJetTool::GetPerigee( const xAOD::TrackParticle* i_ntrk)
  const
  {
       return &(i_ntrk->perigeeParameters());
  }



//
//  Needed translation from Rec::TrackParticle to Trk::TrackParticleBase
//
  StatusCode
  InDetVKalVxInJetTool::VKalVrtFitFastBase(
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
	  //   bitH=HitPattern&((int)pow(2,Trk::pixelBarrel1));
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

  Amg::MatrixX InDetVKalVxInJetTool::SetFullMatrix(int NTrk, std::vector<double> & Matrix) const
  {
    Amg::MatrixX mtx(3+3*NTrk,3+3*NTrk);   // Create identity matrix of needed size
    long int ij=0;
    for(int i=0; i<(3+3*NTrk); i++){
      for(int j=0; j<i; j++){
                  mtx(i,j)=Matrix[ij];
         if(i!=j) mtx(j,i)=Matrix[ij];
         ij++;
      }
    }
    return mtx;
  }

  Amg::MatrixX InDetVKalVxInJetTool::makeVrtCovMatrix( std::vector<double> & ErrorMatrix )
  const
  {
      Amg::MatrixX VrtCovMtx(3,3);  
      VrtCovMtx(0,0)                  = ErrorMatrix[0];
      VrtCovMtx(0,1) = VrtCovMtx(1,0) = ErrorMatrix[1];
      VrtCovMtx(1,1)                  = ErrorMatrix[2];
      VrtCovMtx(0,2) = VrtCovMtx(2,0) = ErrorMatrix[3];
      VrtCovMtx(1,2) = VrtCovMtx(2,1) = ErrorMatrix[4];
      VrtCovMtx(2,2)                  = ErrorMatrix[5];
      return VrtCovMtx;
  }

  void InDetVKalVxInJetTool::fillVrtNTup( std::vector<Vrt2Tr>  & all2TrVrt)
  const
  {	 if(!m_curTup)return;
         int ipnt=0;
         for(auto & vrt : all2TrVrt) {
	   if(ipnt==DevTuple::maxNTrk)break;
	   m_curTup->VrtDist2D[ipnt]=vrt.FitVertex.perp();
	   m_curTup->VrtSig3D[ipnt]=vrt.Signif3D;
	   m_curTup->VrtSig2D[ipnt]=vrt.Signif2D;
	   m_curTup->itrk[ipnt]=vrt.i;
	   m_curTup->jtrk[ipnt]=vrt.j;
	   m_curTup->mass[ipnt]=vrt.Momentum.M();
	   m_curTup->Chi2[ipnt]=vrt.Chi2;
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
	   m_curTup->NVrtNT[ipnt]=vrt.SelTrk.size();
           m_curTup->NVrtTrkI[ipnt]=vrt.SelTrk[0];
	   m_curTup->NVrtM[ipnt]=vrt.vertexMom.M();
	   m_curTup->NVrtChi2[ipnt]=vrt.Chi2;
           float maxW=0., sumW=0.;
           for(auto trk : vrt.SelTrk){ sumW+=trkScore[trk][0]; maxW=std::max(trkScore[trk][0], maxW);}
	   m_curTup->NVrtMaxW[ipnt]=maxW;
	   m_curTup->NVrtAveW[ipnt]=sumW/vrt.SelTrk.size();
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
  int InDetVKalVxInJetTool::getIdHF(const Rec::TrackParticle*) const { return 0; }
  int InDetVKalVxInJetTool::getG4Inter(const Rec::TrackParticle* ) const { return 0; }
  int InDetVKalVxInJetTool::getMCPileup(const Rec::TrackParticle* ) const { return 0; }

}  //end namespace
