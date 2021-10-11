/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetSVWithMuonTool/InDetSVWithMuonTool.h"
//-------------------------------------------------
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include  "xAODTracking/Vertex.h"
#include <cmath>
#include <vector>

namespace InDet{  



               /*  Technicalities */

  
  double InDetSVWithMuonTool::VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
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
    Signif=(Signif<0) ? 0.:std::sqrt(Signif);
    return std::sqrt(distx*distx+disty*disty+distz*distz);
  }



  double InDetSVWithMuonTool::ConeDist(const AmgVector(5) & VectPerig, const TLorentzVector & Dir)
  const
  {
  	double  etaTr = -std::log(std::tan(VectPerig[3]*0.5));
	  double  etaJet = Dir.PseudoRapidity();
	  double  adphi = std::abs(Dir.Phi()-VectPerig[2]);
	  while(adphi> M_PI)adphi-=2.*M_PI;
 	  return  std::sqrt(adphi*adphi + (etaJet-etaTr)*(etaJet-etaTr));
  }




   int InDetSVWithMuonTool::FindMaxAfterFirst( std::vector<double>& Chi2PerTrk)
   const
   { 
      double Chi2Ref=0.;
      int Position=1;
      if( Chi2PerTrk.size() < 2 ) return Position ;
      for (int i=1; i<(int)Chi2PerTrk.size(); i++){
         if( Chi2PerTrk[i] > Chi2Ref) { Chi2Ref=Chi2PerTrk[i]; Position=i;}
      }
      return Position;
   }      
  
 

//  Function returns a transverse momentum of track w/r some direction
//
  double InDetSVWithMuonTool::pTvsDir(const Amg::Vector3D &Dir, const std::vector< double >& InpTrk) 
  const
  {
     double Norm=std::sqrt(Dir.x()*Dir.x() + Dir.y()*Dir.y() + Dir.z()*Dir.z());
     double sx=Dir.x()/Norm; 
     double sy=Dir.y()/Norm; 
     double sz=Dir.z()/Norm;

     double px=0.,py=0.,pz=0.; 
     double scale{};
     const auto invDenom{1./std::abs(InpTrk[2])};
     px = std::cos ( InpTrk[0]) * std::sin(InpTrk[1])*invDenom;
     py = std::sin ( InpTrk[0]) * std::sin(InpTrk[1])*invDenom;
     pz = std::cos(InpTrk[1])*invDenom;
     scale = px*sx + py*sy + pz*sz;
     px -= sx*scale;
     py -= sy*scale; 
     pz -= sz*scale;
     return std::sqrt( px*px +py*py + pz*pz );
   }



  StatusCode InDetSVWithMuonTool::VKalVrtFitFastBase(const std::vector<const xAOD::TrackParticle*>& listTrk,
                                                     Amg::Vector3D  & FitVertex,
                                                     Trk::IVKalState& istate)
  const
  {  return m_fitSvc->VKalVrtFitFast(listTrk,FitVertex,istate);  }




  StatusCode InDetSVWithMuonTool::VKalVrtFitBase(const std::vector<const xAOD::TrackParticle*> & listPart,
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
                                  istate, ifCovV0);

  }

  TLorentzVector InDetSVWithMuonTool::TotalMom(const std::vector<const xAOD::TrackParticle*>& InpTrk) 
  const
  {
     TLorentzVector sum(0.,0.,0.,0.); 
     for (int i = 0; i < (int)InpTrk.size(); ++i) {
       if( InpTrk[i] == nullptr ) continue; 
       sum += InpTrk[i]->p4();
     }
     return sum; 
   }


}  //end namespace
