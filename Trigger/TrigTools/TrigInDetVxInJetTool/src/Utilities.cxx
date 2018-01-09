/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigInDetVxInJetTool/TrigInDetVxInJetTool.h"
#include "VxVertex/RecVertex.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
//-------------------------------------------------
// Other stuff
#include<iostream>



               /*  Technicalities */
bool TrigInDetVxInJetTool::sortbyPtPredicate(const TrigInDetTrack* t1, const TrigInDetTrack* t2)
{
  return fabs(t1->param()->pT()) < fabs(t2->param()->pT());
}
double TrigInDetVxInJetTool::ProjPos(const Amg::Vector3D & Vrt,const CLHEP::HepLorentzVector & JetDir)
  const
{
  double Denom = JetDir.px()*JetDir.px() + JetDir.py()*JetDir.py() + JetDir.pz()*JetDir.pz();
  return (Vrt.x()*JetDir.px() + Vrt.y()*JetDir.py() + Vrt.z()*JetDir.pz())/Denom;
}



double TrigInDetVxInJetTool::VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                        const std::vector<double> SecVrtErr, double& Signif)
  const
{
  double distx =  PrimVrt.position().x()- SecVrt.x();
  double disty =  PrimVrt.position().y()- SecVrt.y();
  double distz =  PrimVrt.position().z()- SecVrt.z();

  Amg::MatrixX  PrimCovMtx=PrimVrt.covariancePosition();
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
  
//--------------------------------------------------
// Significance along jet direction
//--------------------------------------------------
double TrigInDetVxInJetTool::VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                        const std::vector<double> SecVrtErr, const CLHEP::HepLorentzVector & JetDir)
  const
{
  Amg::Vector3D jetDir(JetDir.vect().unit().x(), JetDir.vect().unit().y(), JetDir.vect().unit().z());
  double projDist=(PrimVrt.position()-SecVrt).dot(jetDir);
  double distx =  jetDir.x()*projDist;
  double disty =  jetDir.y()*projDist;
  double distz =  jetDir.z()*projDist;
  
  Amg::MatrixX  PrimCovMtx=PrimVrt.covariancePosition();
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


double TrigInDetVxInJetTool::VrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>  & VrtErr1,
                                        const Amg::Vector3D & Vrt2, const std::vector<double>  & VrtErr2)
  const
{
   double Signif;
   double distx =  Vrt1.x()- Vrt2.x();
   double disty =  Vrt1.y()- Vrt2.y();
   double distz =  Vrt1.z()- Vrt2.z();
   
   AmgSymMatrix(3)  PrimCovMtx;
   PrimCovMtx(0,0) =                   VrtErr1[0]+VrtErr2[0];
   PrimCovMtx(0,1) = PrimCovMtx(1,0) = VrtErr1[1]+VrtErr2[1];
   PrimCovMtx(1,1) =                   VrtErr1[2]+VrtErr2[2];
   PrimCovMtx(0,2) = PrimCovMtx(2,0) = VrtErr1[3]+VrtErr2[3];
   PrimCovMtx(1,2) = PrimCovMtx(2,1) = VrtErr1[4]+VrtErr2[4];
   PrimCovMtx(2,2) =                   VrtErr1[5]+VrtErr2[5];
   
   AmgSymMatrix(3)  WgtMtx = PrimCovMtx.inverse();
 
   Signif = distx*WgtMtx(0,0)*distx
           +disty*WgtMtx(1,1)*disty
           +distz*WgtMtx(2,2)*distz
        +2.*distx*WgtMtx(0,1)*disty
        +2.*distx*WgtMtx(0,2)*distz
        +2.*disty*WgtMtx(1,2)*distz;

   Signif=sqrt(Signif);
   if(Signif != Signif)  Signif = 0.;
   
   return Signif;
}


/* Invariant mass calculation for V0 decays*/
/* Gives correct mass assignment in case of nonequal masses*/


double TrigInDetVxInJetTool::massV0(std::vector< std::vector<double> >& TrkAtVrt,
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



int TrigInDetVxInJetTool::FindMax( std::vector<double>& Chi2PerTrk)
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
  
 

//  Function returns a transverse momentum of track w/r some direction
//
double TrigInDetVxInJetTool::pTvsDir(const Amg::Vector3D &Dir, const std::vector< double >& InpTrk) 
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

CLHEP::HepLorentzVector TrigInDetVxInJetTool::TotalMom(const std::vector<const TrigInDetTrack*>& InpTrk) 
  const
{
  double px=0.,py=0.,pz=0.,ee=0.;
  double Pt=0;
  for (int i = 0; i < (int)InpTrk.size(); ++i) {
    if( InpTrk[i] == NULL ){ continue; } 
    const TrigInDetTrackFitPar* Per=InpTrk[i]->param();
    Pt= fabs(Per->pT());
    px += cos ( Per->phi0()) * Pt;
    py += sin ( Per->phi0()) * Pt;
    double Theta = 2.0*atan(exp(-Per->eta())); 
    pz += Pt/tan(Theta);
    ee += sqrt(  Pt*Pt + pz*pz + m_massPi*m_massPi);
  }
  return CLHEP::HepLorentzVector(px,py,pz,ee); 
}




void TrigInDetVxInJetTool::RemoveEntryInList(std::vector<const TrigInDetTrack*>& ListTracks, int Outlier)
  const
{
  if(Outlier < 0 ) return;
  if(Outlier >= (int)ListTracks.size() ) return;
  std::vector<const TrigInDetTrack*>::iterator   TransfEnd;
  TransfEnd = remove( ListTracks.begin(), ListTracks.end(), ListTracks[Outlier]);
  ListTracks.erase( TransfEnd,ListTracks.end());
}     





//
//  For possible templating....
//

StatusCode TrigInDetVxInJetTool::VKalVrtFitFastBase(const std::vector<const TrigInDetTrack*>& listPart,
                                                    Amg::Vector3D  & FitVertex)
{  
  return m_fitSvc->VKalVrtFitFast(listPart,FitVertex);    /* Fast crude estimation */
}

StatusCode TrigInDetVxInJetTool::VKalVrtFitBase(const std::vector<const TrigInDetTrack*> & listPart,
                                                Amg::Vector3D&                             Vertex,
                                                CLHEP::HepLorentzVector&                   Momentum,
                                                long int&                                  Charge,
                                                std::vector<double>&                       ErrorMatrix,
                                                std::vector<double>&                       Chi2PerTrk,
                                                std::vector< std::vector<double> >&        TrkAtVrt,
                                                double& Chi2 )
{
  return m_fitSvc->VKalVrtFit( listPart, Vertex, Momentum, Charge,
                               ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);

}

