/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
//-------------------------------------------------
// Other stuff
#include <cmath>
#include<iostream>


namespace InDet{  

  TLorentzVector  InDetVKalVxInJetTool::GetBDir( const xAOD::TrackParticle* trk1,
                                                 const xAOD::TrackParticle* trk2,
                                                 const xAOD::Vertex    & PrimVrt,
						 Amg::Vector3D &V1, Amg::Vector3D &V2)
  const
  { // B hadron flight direction based on 2 separate tracks and PV. Calculated via plane-plane crossing
    Amg::Vector3D PVRT(PrimVrt.x(),PrimVrt.y(),PrimVrt.z());
    ////double d01=trk1->d0();    double d02=trk2->d0();
    ////double z01=trk1->z0();    double z02=trk2->z0();
    ////double phi1=trk1->phi0(); double phi2=trk2->phi0();
    ////Amg::Vector3D new1(trk1->vx()-d01*sin(phi1),trk1->vy()+d01*cos(phi1),trk1->vz()+z01); // Wrong sign!!!!!
    ////Amg::Vector3D new2(trk2->vx()-d02*sin(phi2),trk2->vy()+d02*cos(phi2),trk2->vz()+z02);
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

  void InDetVKalVxInJetTool::printWrkSet(const std::vector<WrkVrt> *, const std::string ) const {
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
  double InDetVKalVxInJetTool::ProjPos(const Amg::Vector3D & Vrt, const TLorentzVector & JetDir)
  const
  {
    //Amg::Vector3D Vrt=SV-PV;
    return (Vrt.x()*JetDir.Px() + Vrt.y()*JetDir.Py() + Vrt.z()*JetDir.Pz())/JetDir.P();
  }

  double InDetVKalVxInJetTool::ProjPosT(const Amg::Vector3D & Vrt, const TLorentzVector & JetDir)
  const
  {
    return (Vrt.x()*JetDir.Px() + Vrt.y()*JetDir.Py())/JetDir.Pt();
  }

  bool InDetVKalVxInJetTool::insideMatLayer(float xvt,float yvt) const
  {
        float Dist2DBP=sqrt( (xvt-m_Xbeampipe)*(xvt-m_Xbeampipe) + (yvt-m_Ybeampipe)*(yvt-m_Ybeampipe) ); 
        float Dist2DBL=sqrt( (xvt-m_XlayerB)*(xvt-m_XlayerB) + (yvt-m_YlayerB)*(yvt-m_YlayerB) ); 
        float Dist2DL1=sqrt( (xvt-m_Xlayer1)*(xvt-m_Xlayer1) + (yvt-m_Ylayer1)*(yvt-m_Ylayer1) );
        float Dist2DL2=sqrt( (xvt-m_Xlayer2)*(xvt-m_Xlayer2) + (yvt-m_Ylayer2)*(yvt-m_Ylayer2) );
        if(m_existIBL){              // 4-layer pixel detector
               if( fabs(Dist2DBP-m_Rbeampipe)< 1.0)  return true;           // Beam Pipe removal  
               if( fabs(Dist2DBL-m_RlayerB)  < 2.5)     return true;
               if( fabs(Dist2DL1-m_Rlayer1)  < 3.0)      return true;
               if( fabs(Dist2DL2-m_Rlayer2)  < 3.0)      return true;
               //if( fabs(Dist2DL2-m_Rlayer3)  < 4.0)      return true;
        }else{                       // 3-layer pixel detector
               if( fabs(Dist2DBP-m_Rbeampipe)< 1.5)  return true;           // Beam Pipe removal  
               if( fabs(Dist2DBL-m_RlayerB)  < 3.5)     return true;
               if( fabs(Dist2DL1-m_Rlayer1)  < 4.0)      return true;
               if( fabs(Dist2DL2-m_Rlayer2)  < 5.0)      return true;
        }
        return false; 
  }

  double InDetVKalVxInJetTool::VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double> SecVrtErr, double& Signif)
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
                                          const std::vector<double> SecVrtErr, double& Signif)
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

//--------------------------------------------------
// Significance along jet direction
//--------------------------------------------------
  double InDetVKalVxInJetTool::VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double> SecVrtErr, const TLorentzVector & JetDir)
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
                                          const std::vector<double> SecVrtErr, const TLorentzVector & JetDir)
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

//    std::cout << " Inversion ="<<iFail<<'\n';

    Signif =   distx*WgtMtx(0,0)*distx
              +disty*WgtMtx(1,1)*disty
              +distz*WgtMtx(2,2)*distz
           +2.*distx*WgtMtx(0,1)*disty
           +2.*distx*WgtMtx(0,2)*distz
           +2.*disty*WgtMtx(1,2)*distz;
    Signif=sqrt(Signif);
    if(Signif != Signif)  Signif = 0.;
//    std::cout << " Inversion signif="<<Signif<<", "<<sqrt(distx*distx+disty*disty+distz*distz)<<'\n';
//    return sqrt(distx*distx+disty*disty+distz*distz);
    return Signif;
  }
//



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



   int InDetVKalVxInJetTool::FindMax( std::vector<double>& Chi2PerTrk, std::vector<int> & cntTrk)
   const
   { 
      double Chi2Ref=0.;
      int Position=0;
      if( Chi2PerTrk.size() < 1 ) return Position ;
      for (int i=0; i< (int)Chi2PerTrk.size(); i++){
         if( Chi2PerTrk[i]/cntTrk[i] > Chi2Ref) { Chi2Ref=Chi2PerTrk[i]/cntTrk[i]; Position=i;}
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
     AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
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
     AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
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
     AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
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
//  Get vector of Perigee for kinematical calculations
//
  std::vector<const Trk::Perigee*> InDetVKalVxInJetTool::GetPerigeeVector( const std::vector<const Rec::TrackParticle*>& InpTrk)
  const
  {  
     std::vector<const Trk::Perigee*> tmp;
     for (int i = 0; i < (int)InpTrk.size(); ++i) tmp.push_back( GetPerigee( InpTrk[i] ) ) ;
     return tmp;       
  }
  std::vector<const Trk::Perigee*> InDetVKalVxInJetTool::GetPerigeeVector( const std::vector<const Trk::TrackParticleBase*>& InpTrk)
  const
  {  
     std::vector<const Trk::Perigee*> tmp;
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       const Rec::TrackParticle* tmpP = dynamic_cast<const Rec::TrackParticle*> (InpTrk[i]);
       if(tmpP)tmp.push_back( GetPerigee( tmpP ) ) ;
     }
     return tmp;       
  }





//
//-- Perigee in Rec::TrackParticle, Trk::Track and xAOD::TrackParticle
//
  const Trk::Perigee* InDetVKalVxInJetTool::GetPerigee( const Rec::TrackParticle* i_ntrk)
  const
  {
       return i_ntrk->perigee();
  }


  const Trk::Perigee* InDetVKalVxInJetTool::GetPerigee( const xAOD::TrackParticle* i_ntrk)
  const
  {
       return &(i_ntrk->perigeeParameters());
  }



//
//  Needed translation from Rec::TrackParticle to Trk::TrackParticleBase
//
  StatusCode InDetVKalVxInJetTool::VKalVrtFitFastBase(const std::vector<const xAOD::TrackParticle*>& listTrk,
                                                      Amg::Vector3D  & FitVertex)
  const
  {  return m_fitSvc->VKalVrtFitFast(listTrk,FitVertex);  }

  StatusCode InDetVKalVxInJetTool::VKalVrtFitFastBase(const std::vector<const Rec::TrackParticle*>& listPart,
                                                      Amg::Vector3D  & FitVertex)
  const
  {  
     std::vector <const Trk::TrackParticleBase*> listBase;
     for(int i=0; i<(int)listPart.size(); i++) {
       listBase.push_back( (const Trk::TrackParticleBase*)listPart[i]); 
     }
     return m_fitSvc->VKalVrtFitFast(listBase,FitVertex);    /* Fast crude estimation */
  }


  StatusCode InDetVKalVxInJetTool::VKalVrtFitBase(const std::vector<const Rec::TrackParticle*> & listPart,
                                                  Amg::Vector3D&                   Vertex,
                                                  TLorentzVector&                  Momentum,
                                                  long int&                        Charge,
                                                  std::vector<double>&             ErrorMatrix,
                                                  std::vector<double>&             Chi2PerTrk,
                                                  std::vector< std::vector<double> >& TrkAtVrt,
                                                  double& Chi2 ) const
  {
     return m_fitSvc->VKalVrtFit( PartToBase(listPart), Vertex, Momentum, Charge,
                                  ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);

  }

  StatusCode InDetVKalVxInJetTool::VKalVrtFitBase(const std::vector<const xAOD::TrackParticle*> & listPart,
                                                  Amg::Vector3D&                   Vertex,
                                                  TLorentzVector&                  Momentum,
                                                  long int&                        Charge,
                                                  std::vector<double>&             ErrorMatrix,
                                                  std::vector<double>&             Chi2PerTrk,
                                                  std::vector< std::vector<double> >& TrkAtVrt,
                                                  double& Chi2 ) const
  {
     std::vector<const xAOD::NeutralParticle*> netralPartDummy(0);
     return m_fitSvc->VKalVrtFit( listPart, netralPartDummy,Vertex, Momentum, Charge,
                                  ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);

  }



/*************************************************************************************************************/
  void   InDetVKalVxInJetTool::getPixelLayers(const Rec::TrackParticle* Part, int &blHit, int &l1Hit, int &l2Hit, int &nLays  ) const
  {
     	blHit=l1Hit=l2Hit=nLays=0; 
        const Trk::TrackSummary* testSum = Part->trackSummary();
        if(testSum){ 
	       if(testSum->isHit(Trk::pixelBarrel0))blHit=1;
	       if(testSum->isHit(Trk::pixelBarrel1))l1Hit=1;
	       if(testSum->isHit(Trk::pixelBarrel2))l2Hit=1;
        }
	nLays=blHit+l1Hit+l2Hit;
  }
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
          if(!Part->summaryValue( BLhit,  xAOD::numberOfInnermostPixelLayerHits) )          BLhit = 0;
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
  void InDetVKalVxInJetTool::getPixelProblems(const Rec::TrackParticle* , int &splshIBL, int &splshBL ) const
  {
    	splshIBL=splshBL=0;  // Temporary implementation
  }
  void   InDetVKalVxInJetTool::getPixelDiscs(const xAOD::TrackParticle* Part, int &d0Hit, int &d1Hit, int &d2Hit) const
  {
        uint32_t HitPattern=Part->hitPattern();
	d0Hit=0; if( HitPattern&((1<<Trk::pixelEndCap0)) ) d0Hit=1;
	d1Hit=0; if( HitPattern&((1<<Trk::pixelEndCap1)) ) d1Hit=1;
	d2Hit=0; if( HitPattern&((1<<Trk::pixelEndCap2)) ) d2Hit=1;
  }
  void   InDetVKalVxInJetTool::getPixelDiscs(const  Rec::TrackParticle* Part, int &d0Hit, int &d1Hit, int &d2Hit) const
  {
        const Trk::TrackSummary* testSum = Part->trackSummary();
        if(testSum){ 
	       if(testSum->isHit(Trk::pixelEndCap0))d0Hit=1;
	       if(testSum->isHit(Trk::pixelEndCap1))d1Hit=1;
	       if(testSum->isHit(Trk::pixelEndCap2))d2Hit=1;
        } else d0Hit=d1Hit=d2Hit=0; 
  }

/*************************************************************************************************************/
   const std::vector<const Trk::TrackParticleBase*> 
       InDetVKalVxInJetTool::PartToBase(const std::vector<const Rec::TrackParticle*> & listPart)
   const
   {
     std::vector <const Trk::TrackParticleBase*> listBase;
     for(int i=0; i<(int)listPart.size(); i++) {
        listBase.push_back( (const Trk::TrackParticleBase*)listPart[i]); 
     }
     return listBase;
   }

   const std::vector<const Rec::TrackParticle*> 
       InDetVKalVxInJetTool::BaseToPart(const std::vector<const Trk::TrackParticleBase*> & listBase)
   const
   {
     std::vector <const Rec::TrackParticle*> listPart;
     for(int i=0; i<(int)listBase.size(); i++) {
        listPart.push_back( dynamic_cast<const Rec::TrackParticle*> (listBase[i])); 
     }
     return listPart;
   }



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

  double InDetVKalVxInJetTool::trkPtCorr(double pT) const
  {
     double adp=pT/64000.; if(adp<0.)adp=0; if(adp>1.)adp=1.; adp=sqrt(adp)/2.;
     return adp;
  }

}  //end namespace
