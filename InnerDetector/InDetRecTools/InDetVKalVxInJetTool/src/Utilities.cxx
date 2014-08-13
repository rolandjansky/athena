/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include  "VxVertex/VxTrackAtVertex.h"
//-------------------------------------------------
// Other stuff
#include <cmath>
#include<iostream>


namespace InDet{  


  void InDetVKalVxInJetTool::printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name)
  const
  {
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
      std::cout<<name
      <<"= "<<(*WrkVrtSet)[iv].vertex[0]
      <<", "<<(*WrkVrtSet)[iv].vertex[1]
      <<", "<<(*WrkVrtSet)[iv].vertex[2]
      <<" NTrk="<<(*WrkVrtSet)[iv].SelTrk.size()
      <<" is good="<<std::boolalpha<<(*WrkVrtSet)[iv].Good
      <<" trk=";
      for(int kk=0; kk<(int)(*WrkVrtSet)[iv].SelTrk.size(); kk++) {
                std::cout<<", "<<(*WrkVrtSet)[iv].SelTrk[kk];}
      std::cout<<'\n';
    }
  }


               /*  Technicalities */
  double InDetVKalVxInJetTool::ProjPos(const Amg::Vector3D & Vrt,const TLorentzVector & JetDir)
  const
  {
    double Denom = JetDir.Px()*JetDir.Px() + JetDir.Py()*JetDir.Py() + JetDir.Pz()*JetDir.Pz();
    return (Vrt.x()*JetDir.Px() + Vrt.y()*JetDir.Py() + Vrt.z()*JetDir.Pz())/Denom;
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
    Amg::Vector3D jetDir(JetDir.Vect().Unit().X(), JetDir.Vect().Unit().X(), JetDir.Vect().Unit().Z());
    double projDist=(PrimVrt.position()-SecVrt).dot(jetDir);
    double distx =  jetDir.x()*projDist;
    double disty =  jetDir.y()*projDist;
    double distz =  jetDir.z()*projDist;

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

    double Signif = distx*WgtMtx(0,0)*distx
                   +disty*WgtMtx(1,1)*disty
                   +distz*WgtMtx(2,2)*distz
                +2.*distx*WgtMtx(0,1)*disty
                +2.*distx*WgtMtx(0,2)*distz
                +2.*disty*WgtMtx(1,2)*distz;
    Signif=sqrt(Signif);
    if( Signif!=Signif ) Signif = 0.;
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



   int InDetVKalVxInJetTool::FindMax( std::vector<double>& Chi2PerTrk)
   const
   { 
      double Chi2Ref=0.;
      int Position=0;
      if( Chi2PerTrk.size() < 1 ) return Position ;
      for (int i=0; i< (int)Chi2PerTrk.size(); i++){
         if( Chi2PerTrk[i] > Chi2Ref) { Chi2Ref=Chi2PerTrk[i]; Position=i;}
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
     return m_fitSvc->VKalVrtFit( listPart, Vertex, Momentum, Charge,
                                  ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);

  }



/*************************************************************************************************************/
  void   InDetVKalVxInJetTool::getPixelLayers(const Rec::TrackParticle* Part, int &blHit, int &l1Hit, int &l2Hit ) const
  {
     	blHit=l1Hit=l2Hit=0; 
        const Trk::TrackSummary* testSum = Part->trackSummary();
        if(testSum){ 
	       if(testSum->isHit(Trk::pixelBarrel0))blHit=1;
	       if(testSum->isHit(Trk::pixelBarrel1))l1Hit=1;
	       if(testSum->isHit(Trk::pixelBarrel2))l2Hit=1;
        }
  }
  void   InDetVKalVxInJetTool::getPixelLayers(const xAOD::TrackParticle* Part, int &blHit, int &l1Hit, int &l2Hit ) const
  {
    	blHit=l1Hit=l2Hit=0; 
        uint8_t BLhit,NPlay;
        if(!Part->summaryValue( BLhit,  xAOD::numberOfBLayerHits) )                BLhit = 0;
        if(!Part->summaryValue( NPlay,  xAOD::numberOfContribPixelLayers) )  NPlay = 0;
         if           (BLhit)       { blHit=l1Hit=l2Hit=1;}  // B-layer hit is present. Presumably all pixel layers are fired
         else if (NPlay>1) { l1Hit=l2Hit=1;}                       // B-layer hit is absent. Presumably layers 1+2  are fired 
	 else if (NPlay>0) { l2Hit=1;}
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

//  Mass error in vertex from ExtendedVxCandidate.
//   test code itself is in BTagVrtSec.cxx
//-------------------------------------------------------------------------------------------------------
  double InDetVKalVxInJetTool::massError(const Trk::ExtendedVxCandidate * vxCandidate, double mass) const
  {
    int NTrk= vxCandidate->vxTrackAtVertex()->size();
    std::vector<TLorentzVector> particleMom(NTrk);                 // Momenta of particles
    std::vector<AmgMatrix(3,3)> particleDeriv(NTrk);               // Derivatives of  particle momentum
    TLorentzVector totalMom;                                       // summary momentum
    AmgMatrix(3,3) tmpDeriv;

    double phi,theta,invP;
    for( int it=0; it<NTrk; it++){
      const Trk::TrackParameters  *bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      const Trk::Perigee* mPer = dynamic_cast<const Trk::Perigee*>(bPer);
      phi   =  mPer->parameters()[Trk::phi];
      theta =  mPer->parameters()[Trk::theta];
      invP  =  mPer->parameters()[Trk::qOverP];
      TLorentzVector    tmp( cos(phi)*sin(theta)/fabs(invP),
                             sin(phi)*sin(theta)/fabs(invP),
                                      cos(theta)/fabs(invP),
				                       0.   );
      tmp.SetE( sqrt(tmp.Vect().Mag2()+mass*mass) );
      particleMom[it]=tmp;
      totalMom += tmp;
//  d(Px,Py,Pz)/d(Phi,Theta,InvP)
      tmpDeriv(0,0) = - tmp.Py();
      tmpDeriv(1,0) =   tmp.Px();
      tmpDeriv(2,0) =   0.;
      tmpDeriv(0,1) =   cos(phi) * tmp.Pz();
      tmpDeriv(1,1) =   sin(phi) * tmp.Pz();
      tmpDeriv(2,1) = - sin(theta)/fabs(invP);
      tmpDeriv(0,2) = - tmp.Px()/invP;
      tmpDeriv(1,2) = - tmp.Py()/invP;
      tmpDeriv(2,2) = - tmp.Pz()/invP;
      particleDeriv[it] = tmpDeriv;
    }
    double dMdPx,dMdPy,dMdPz,dMdPhi,dMdTheta,dMdInvP;
    std::vector<double> Deriv(3*NTrk+3, 0.);  
    for( int it=0; it<NTrk; it++){
// dM/d(Px_it,Py_it,Pz_it)
      dMdPx = ( totalMom.E() * particleMom[it].Px()/particleMom[it].E() - totalMom.Px() ) / totalMom.M();
      dMdPy = ( totalMom.E() * particleMom[it].Py()/particleMom[it].E() - totalMom.Py() ) / totalMom.M();
      dMdPz = ( totalMom.E() * particleMom[it].Pz()/particleMom[it].E() - totalMom.Pz() ) / totalMom.M();
      
      dMdPhi   = dMdPx*particleDeriv[it](0,0) + dMdPy*particleDeriv[it](1,0) + dMdPz*particleDeriv[it](2,0);
      dMdTheta = dMdPx*particleDeriv[it](0,1) + dMdPy*particleDeriv[it](1,1) + dMdPz*particleDeriv[it](2,1);
      dMdInvP  = dMdPx*particleDeriv[it](0,2) + dMdPy*particleDeriv[it](1,2) + dMdPz*particleDeriv[it](2,2);

      Deriv[3*it +3 +0] = dMdPhi;    Deriv[3*it +3 +1] = dMdTheta; Deriv[3*it +3 +2] = dMdInvP;
    }


    const Amg::MatrixX * fullCov = vxCandidate->fullCovariance();
    double err=0;
    for(int i=0; i<3*NTrk+3; i++){
      for(int j=0; j<3*NTrk+3; j++){
        err += Deriv[i]*(*fullCov)(i,j)*Deriv[j];
      }
    }
    return sqrt(err);
  }

//  Summary track from ExtendedVxCandidate.
//   test code itself is in BTagVrtSec.cxx
//-------------------------------------------------------------------------------------------------------
  Trk::TrackParticleBase * InDetVKalVxInJetTool::CreateParticle(const Trk::ExtendedVxCandidate * vxCandidate) const
  {
    const int NTrk = vxCandidate->vxTrackAtVertex()->size();
    std::vector<TLorentzVector> particleMom(NTrk);        // Momenta of particles
    std::vector<AmgMatrix(3,3)> particleDeriv(NTrk);      // Derivatives of  particle momentum
    TLorentzVector totalMom;                              // summary momentum
    AmgMatrix(3,3) tmpDeriv;
    int Charge=0;
    double phi,theta,invP;
    for( int it=0; it<NTrk; it++){
      const Trk::TrackParameters  *bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      const Trk::Perigee* mPer = dynamic_cast<const Trk::Perigee*>(bPer);
      phi   =  mPer->parameters()[Trk::phi];
      theta =  mPer->parameters()[Trk::theta];
      invP  =  mPer->parameters()[Trk::qOverP];
      TLorentzVector    tmp( cos(phi)*sin(theta)/fabs(invP),
                             sin(phi)*sin(theta)/fabs(invP),
                                      cos(theta)/fabs(invP),
                                                   0.       );
      tmp.SetE( tmp.Vect().Mag() );
      particleMom[it]=tmp;
      totalMom += tmp;
      if(invP>0){ Charge++;} else {Charge--;}

//  d(Px,Py,Pz)/d(Phi,Theta,InvP)
      tmpDeriv(0,0) = - tmp.Py();
      tmpDeriv(1,0) =   tmp.Px();
      tmpDeriv(2,0) =   0.;
      tmpDeriv(0,1) =   cos(phi) * tmp.Pz();
      tmpDeriv(1,1) =   sin(phi) * tmp.Pz();
      tmpDeriv(2,1) = - sin(theta)/fabs(invP);
      tmpDeriv(0,2) = - tmp.Px()/invP;
      tmpDeriv(1,2) = - tmp.Py()/invP;
      tmpDeriv(2,2) = - tmp.Pz()/invP;
      particleDeriv[it] = tmpDeriv;
    }
//-----------------------------------------------
//  Summary particle parameters
//
    double Px=totalMom.Px(); double Py=totalMom.Py(); double Pz=totalMom.Pz();
    double Pt= sqrt(Px*Px + Py*Py) ;
    double mom= sqrt(Pz*Pz + Pt*Pt) ;
    phi=atan2(Py,Px);
    while ( phi > M_PI) phi -= 2.*M_PI;    
    while ( phi <-M_PI) phi += 2.*M_PI;    
    theta=acos( Pz/mom );
    while ( theta > M_PI) theta -= 2.*M_PI;    
    while ( theta <-M_PI) theta += 2.*M_PI;    
    if    ( theta < 0.) { 
        theta = fabs(theta); phi += M_PI; 
        while ( phi > M_PI) phi -= 2.*M_PI;    
    } 
    invP=Charge/mom;
//---------------------------------------------------------------
//  d(Phi,Theta,InvP)/d(Px,Py,Pz)  -  Perigee vs summary momentum
    tmpDeriv(1,1) = -Py/Pt/Pt;             //dPhi/dPx
    tmpDeriv(1,2) =  Px/Pt/Pt;             //dPhi/dPy
    tmpDeriv(1,3) =  0;                    //dPhi/dPz
    tmpDeriv(2,1) =  Px*Pz/(Pt*mom*mom);   //dTheta/dPx
    tmpDeriv(2,2) =  Py*Pz/(Pt*mom*mom);   //dTheta/dPy
    tmpDeriv(2,3) = -Pt/(mom*mom);         //dTheta/dPz
    tmpDeriv(3,1) = -Px/(mom*mom) * invP;  //dInvP/dPx
    tmpDeriv(3,2) = -Py/(mom*mom) * invP;  //dInvP/dPy
    tmpDeriv(3,3) = -Pz/(mom*mom) * invP;  //dInvP/dPz
//-------------------------------------------------------------------------------------------
    Amg::MatrixX Deriv(5,3*NTrk+3);  Deriv.setZero();    // d(d0,Z,phi,theta,invP)/d(.....) - final derivatives
    for( int it=0; it<NTrk; it++){
      for( int temp=0; temp<3; temp++){
        Deriv(2,3*it+3+0) += tmpDeriv(0,temp)*particleDeriv[it](temp,1) ;
        Deriv(3,3*it+3+0) += tmpDeriv(1,temp)*particleDeriv[it](temp,1) ;
        Deriv(4,3*it+3+0) += tmpDeriv(2,temp)*particleDeriv[it](temp,1) ;

        Deriv(2,3*it+3+1) += tmpDeriv(0,temp)*particleDeriv[it](temp,2) ;
        Deriv(3,3*it+3+1) += tmpDeriv(1,temp)*particleDeriv[it](temp,2) ;
        Deriv(4,3*it+3+1) += tmpDeriv(2,temp)*particleDeriv[it](temp,2) ;

        Deriv(2,3*it+3+2) += tmpDeriv(0,temp)*particleDeriv[it](temp,3) ;
        Deriv(3,3*it+3+2) += tmpDeriv(1,temp)*particleDeriv[it](temp,3) ;
        Deriv(4,3*it+3+2) += tmpDeriv(2,temp)*particleDeriv[it](temp,3) ;
      } 
    }
    Deriv(0,0) = -sin(phi);               // Space derivatives
    Deriv(0,1) =  cos(phi);
    Deriv(1,0) = -cos(phi)/tan(theta);
    Deriv(1,1) = -sin(phi)/tan(theta);
    Deriv(1,2) =  1.;
//----------------------------------------------
// Covariance matrix at fitted vertex
    AmgSymMatrix(5) *SumCov = new  AmgSymMatrix(5);
    SumCov->setZero();
    const Amg::MatrixX * fullCov = vxCandidate->fullCovariance();
    for(int ik=0; ik<5; ik++){
      for(int jk=ik; jk<5; jk++){
        for(int i=0; i<3*NTrk+3; i++){
          for(int j=0; j<3*NTrk+3; j++){
             (*SumCov)(ik,jk) += Deriv(ik,i)*(*fullCov)(i,j)*Deriv(jk,j);
          }
        }
      }
    }
    
//----------------------------------------------
    Amg::Vector3D  baseGlobalVertex( vxCandidate->recVertex().position().x(),
                                     vxCandidate->recVertex().position().y(),
                                     vxCandidate->recVertex().position().z() );
    const Trk::FitQuality* fitQuality = new Trk::FitQuality(10.,1); 
    Trk::TrackParticleBase *nTrkPrt;
    if(Charge){
      const Trk::TrackParameters * perigee;
      std::vector<const Trk::TrackParameters*>     tmpPar;
      perigee = new Trk::Perigee( 0.,0.,phi,theta,Charge/mom, Trk::PerigeeSurface( baseGlobalVertex ), SumCov  );
      tmpPar.push_back(perigee);
      nTrkPrt = new Trk::TrackParticleBase(0, Trk::SecVtx, vxCandidate, new Trk::TrackSummary(),
                                              tmpPar, perigee, fitQuality);
    }else{     
      const Trk::NeutralParameters * perigeeN;
      perigeeN = new Trk::NeutralPerigee( 0.,0.,phi,theta,1./mom, Trk::PerigeeSurface( baseGlobalVertex ), SumCov  );
      std::vector<const Trk::NeutralParameters*>   tmpPar;
      tmpPar.push_back(perigeeN);              //Perigee at fitted vertex
      double a0 = sin(phi) * (0.- vxCandidate->recVertex().position().x()) -     //Perigee with respect to (0,0,0)
                  cos(phi) * (0.- vxCandidate->recVertex().position().y());
      double t  = (0. - vxCandidate->recVertex().position().x())*cos(phi) +
                  (0. - vxCandidate->recVertex().position().y())*sin(phi);
      double z0 = vxCandidate->recVertex().position().z() + t/tan(theta);
      double dA0dPhi = t;                            // Additional derivatives due to transportation
      double dZ0dPhi = -a0/tan(theta);
      double dZ0dTheta = - t /sin(theta)/sin(theta);
      Deriv.setZero();
      for( int it=0; it<NTrk; it++){
        for( int temp=0; temp<3; temp++){
          Deriv(0,3*it+3+0) += dA0dPhi*tmpDeriv(0,temp)*particleDeriv[it](temp,0) ;
          Deriv(0,3*it+3+1) += dA0dPhi*tmpDeriv(0,temp)*particleDeriv[it](temp,1) ;
          Deriv(0,3*it+3+2) += dA0dPhi*tmpDeriv(0,temp)*particleDeriv[it](temp,2) ;

          Deriv(1,3*it+3+0) += dZ0dPhi*tmpDeriv(0,temp)*particleDeriv[it](temp,0) ;
          Deriv(1,3*it+3+1) += dZ0dPhi*tmpDeriv(0,temp)*particleDeriv[it](temp,1) ;
          Deriv(1,3*it+3+2) += dZ0dPhi*tmpDeriv(0,temp)*particleDeriv[it](temp,2) ;

          Deriv(1,3*it+3+0) += dZ0dTheta*tmpDeriv(1,temp)*particleDeriv[it](temp,0) ;
          Deriv(1,3*it+3+1) += dZ0dTheta*tmpDeriv(1,temp)*particleDeriv[it](temp,1) ;
          Deriv(1,3*it+3+2) += dZ0dTheta*tmpDeriv(1,temp)*particleDeriv[it](temp,2) ;
        } 
      }
//---------------------------------------------------------------
// Covariance at (0,0,0) perigee
      AmgSymMatrix(5) * SumCovN=new  AmgSymMatrix(5);
      SumCovN->setZero();
      for(int ik=0; ik<5; ik++){
        for(int jk=ik; jk<5; jk++){
          for(int i=0; i<3*NTrk+3; i++){
            for(int j=0; j<3*NTrk+3; j++){
              (*SumCovN)(ik,jk) += Deriv(ik,i)*(*fullCov)(i,j)*Deriv(jk,j);
            }
          }
        }
      }
      Amg::Vector3D zeroGlobalVertex(0.,0.,0.);
      perigeeN = new Trk::NeutralPerigee( a0,z0,phi,theta,1./mom, Trk::PerigeeSurface( zeroGlobalVertex ),SumCovN  );
//std::cout<<" NEWPART="<<(*perigee)<<'\n';
      //nTrkPrt = new Trk::TrackParticleBase(0, Trk::SecVtx, vxCandidate, new Trk::TrackSummary(),
      //                             tmpPar, perigee, fitQuality);
      nTrkPrt = 0; //VK TrackParticle doesn't accept NeutralPerigee yet.
    }

    return nTrkPrt;

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

}  //end namespace
