/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "NewVrtSecInclusiveTool/NewVrtSecInclusiveTool.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
//-------------------------------------------------
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
 // Other stuff
#include <cmath>


namespace Rec{  


  void NewVrtSecInclusiveTool::printWrkSet(const std::vector<WrkVrt> *, const std::string ) const {
/*  void NewVrtSecInclusiveTool::printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name) const {
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
    std::cout<<name<<" N="<<nGoodV<<'\n'; */
  }

               /*  Technicalities */
  double NewVrtSecInclusiveTool::ProjSV_PV(const Amg::Vector3D & SV, const xAOD::Vertex & PV, const TLorentzVector & Direction) const
  {  
     TVector3 SV_PV( SV.x()-PV.x(), SV.y()-PV.y(), SV.z()-PV.z() );
     return Direction.Vect().Unit()*SV_PV.Unit();
  }

  
  double NewVrtSecInclusiveTool::VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
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

  double NewVrtSecInclusiveTool::VrtVrtDist2D(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                          const std::vector<double> SecVrtErr, double& Signif)
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


  double NewVrtSecInclusiveTool::VrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>  & VrtErr1,
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
  double NewVrtSecInclusiveTool::PntPntDist(const Amg::Vector3D & Vrt1, const Amg::Vector3D & Vrt2) const
  { 
    double dx =  Vrt1.x()- Vrt2.x();
    double dy =  Vrt1.y()- Vrt2.y();
    double dz =  Vrt1.z()- Vrt2.z();
    return sqrt(dx*dx+dy*dy*dz*dz);
  }
//--------------------------------------------------
// Significance along some direction
//--------------------------------------------------
double NewVrtSecInclusiveTool::VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                           const std::vector<double> SecVrtErr, const TLorentzVector & Dir)
   const
   {
     Amg::Vector3D dir(Dir.Vect().Unit().X(), Dir.Vect().Unit().Y(), Dir.Vect().Unit().Z());
     double projDist=(SecVrt-PrimVrt.position()).dot(dir);
     double distx =  dir.x()*projDist;
     double disty =  dir.y()*projDist;
     double distz =  dir.z()*projDist;

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

//----------------------------
//   Vertex error along radius
//----------------------------
  double NewVrtSecInclusiveTool::VrtRadiusError(const Amg::Vector3D & SecVrt, const std::vector<double>  & VrtErr) const
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


    /* Invariant mass calculation for V0 decays*/
    /* Gives correct mass assignment in case of nonequal masses*/


   double NewVrtSecInclusiveTool::massV0(std::vector< std::vector<double> >& TrkAtVrt,
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



  TLorentzVector NewVrtSecInclusiveTool::MomAtVrt(const std::vector< double >& InpTrk) 
  const
  {
     double px = cos ( InpTrk[0]) * sin(InpTrk[1])/fabs(InpTrk[2]);
     double py = sin ( InpTrk[0]) * sin(InpTrk[1])/fabs(InpTrk[2]);
     double pz =                    cos(InpTrk[1])/fabs(InpTrk[2]);
     double ee = sqrt( px*px + py*py + pz*pz + m_massPi*m_massPi);
     return TLorentzVector(px,py,pz,ee); 
   }



  StatusCode NewVrtSecInclusiveTool::GetTrkFitWeights(std::vector<double> & wgt,
                                                    const Trk::IVKalState& istate) const
  {
    return m_fitSvc->VKalGetTrkWeights(wgt, istate);
  }

/*************************************************************************************************************/
  void   NewVrtSecInclusiveTool::getPixelLayers(const xAOD::TrackParticle* Part, int &blHit, int &l1Hit, int &l2Hit, int &nLays ) const
  {
    	blHit=l1Hit=l2Hit=nLays=0; 
        uint8_t IBLhit,BLhit,NPlay,IBLexp,BLexp;
        if(!Part->summaryValue( IBLhit,  xAOD::numberOfInnermostPixelLayerHits) )        IBLhit = 0;
        if(!Part->summaryValue(  BLhit,  xAOD::numberOfNextToInnermostPixelLayerHits) )   BLhit = 0;
        if(!Part->summaryValue(  NPlay,  xAOD::numberOfContribPixelLayers) )              NPlay = 0;
        if(!Part->summaryValue( IBLexp,  xAOD::expectInnermostPixelLayerHit) )           IBLexp = 0;
        if(!Part->summaryValue(  BLexp,  xAOD::expectNextToInnermostPixelLayerHit) )      BLexp = 0;
        blHit=IBLhit; if( IBLexp==0 ) blHit=-1;
        l1Hit= BLhit; if(  BLexp==0 ) l1Hit=-1;
        nLays=NPlay;
        uint32_t HitPattern=Part->hitPattern();
	l2Hit=0; if( HitPattern&((1<<Trk::pixelBarrel2)) ) l2Hit=1;
	//   bitH=HitPattern&((int)pow(2,Trk::pixelBarrel1));

  }
  void NewVrtSecInclusiveTool::getPixelProblems(const xAOD::TrackParticle* Part, int &splshIBL, int &splshBL ) const
  {
    	splshIBL=splshBL=0;
        uint8_t share,split;
	//if(!Part->summaryValue( IBLout,  xAOD::numberOfInnermostPixelLayerOutliers ) )        IBLout = 0;
	if(!Part->summaryValue( share,  xAOD::numberOfInnermostPixelLayerSharedHits ) )   share = 0;
	if(!Part->summaryValue( split,  xAOD::numberOfInnermostPixelLayerSplitHits ) )        split = 0;
        splshIBL=share+split;
	if(!Part->summaryValue( share,  xAOD::numberOfNextToInnermostPixelLayerSharedHits ) )   share = 0;
	if(!Part->summaryValue( split,  xAOD::numberOfNextToInnermostPixelLayerSplitHits ) )        split = 0;
        splshBL=share+split;
  }

  void   NewVrtSecInclusiveTool::getPixelDiscs(const xAOD::TrackParticle* Part, int &d0Hit, int &d1Hit, int &d2Hit) const
  {
        uint32_t HitPattern=Part->hitPattern();
	d0Hit=0; if( HitPattern&((1<<Trk::pixelEndCap0)) ) d0Hit=1;
	d1Hit=0; if( HitPattern&((1<<Trk::pixelEndCap1)) ) d1Hit=1;
	d2Hit=0; if( HitPattern&((1<<Trk::pixelEndCap2)) ) d2Hit=1;
  }
/*************************************************************************************************************/

  int NewVrtSecInclusiveTool::getIdHF(const xAOD::TrackParticle* TP ) const {
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

  int NewVrtSecInclusiveTool::notFromBC(int PDGID) const {
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
  const xAOD::TruthParticle * NewVrtSecInclusiveTool::getPreviousParent(const xAOD::TruthParticle * child, int & ParentPDG) const {
    ParentPDG=0;
    if( child->hasProdVtx() ){
       if( child->prodVtx()->nIncomingParticles()==1 ){
            ParentPDG = abs((*(child->prodVtx()->incomingParticleLinks())[0])->pdgId());
            return *(child->prodVtx()->incomingParticleLinks())[0];
       }
    }
    return 0;
  }


  int NewVrtSecInclusiveTool::getG4Inter(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( tplink.isValid() && (*tplink)->barcode()>200000) return 1;
      }
      return 0;
  }
  int NewVrtSecInclusiveTool::getMCPileup(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( !tplink.isValid() ) return 1;
      } else { return 1; }
      return 0;
  }

  double NewVrtSecInclusiveTool::distToMatLayerSignificance(Vrt2Tr & Vrt) const
  {
     if(Vrt.FitVertex.perp()<20.) return 1.e9;
     double normP=1./Vrt.Momentum.P();
     Amg::Vector3D momentumP(Vrt.Momentum.Px()*normP,Vrt.Momentum.Py()*normP,Vrt.Momentum.Pz()*normP);
     Amg::Vector3D momentumN=-momentumP;
     
     const Trk::Layer * someLayer  = 0;
     const Trk::Layer * nextLayerP = 0;
     const Trk::Layer * nextLayerN = 0;
     auto volume = m_extrapolator->trackingGeometry()->lowestTrackingVolume(Vrt.FitVertex);
     someLayer = volume->associatedLayer(Vrt.FitVertex);
     auto material =  someLayer->layerMaterialProperties();
     if(material){
       nextLayerP=someLayer;
     } else {
       nextLayerP = someLayer->nextLayer(Vrt.FitVertex,momentumP);
       if(nextLayerP){ if(!nextLayerP->layerMaterialProperties())nextLayerP=0; }
       nextLayerN = someLayer->nextLayer(Vrt.FitVertex,momentumN);
       if(nextLayerN){ if(!nextLayerN->layerMaterialProperties())nextLayerN=0; }
     }
     momentumP *= 1.e5;   //100GeV to have straight trajectory
     double charge = 1.;
     const Trk::Perigee pseudoVrtPart(Vrt.FitVertex, momentumP, charge, Vrt.FitVertex);

     const Trk::TrackParameters * extrapParP=0;
     const Trk::TrackParameters * extrapParN=0;
     if(nextLayerP){ extrapParP = m_extrapolator->extrapolate(pseudoVrtPart,
                     nextLayerP->surfaceRepresentation(), Trk::anyDirection, false, Trk::nonInteractingMuon) ;}
     if(nextLayerN){ extrapParN = m_extrapolator->extrapolate(pseudoVrtPart,
                     nextLayerN->surfaceRepresentation(), Trk::anyDirection, false, Trk::nonInteractingMuon) ;}

     float distanceP=1.e9, distanceN=1.e9;
     if(extrapParP)distanceP=PntPntDist(extrapParP->position(), Vrt.FitVertex);
     if(extrapParN)distanceN=PntPntDist(extrapParN->position(), Vrt.FitVertex);
     if(distanceP==1.e9 && distanceN==1.e9) return 1.e9;

     //std::pair<const Trk::TrackParameters*,const Trk::Layer*> next= 
     //         m_extrapolator->extrapolateToNextActiveLayer(pseudoVrtPart,Trk::anyDirection,true,Trk::pion) ;

     double Signif=1.e9;
     std::vector<double> pntCovar={1.e-2,0.,1.e-2,0.,0.,1.e-2};
     if(distanceP<distanceN)Signif=VrtVrtDist(Vrt.FitVertex, Vrt.ErrorMatrix, extrapParP->position(), pntCovar);
     else                   Signif=VrtVrtDist(Vrt.FitVertex, Vrt.ErrorMatrix, extrapParN->position(), pntCovar);
     delete extrapParP;
     delete extrapParN;
     return Signif;
  }

 

 

}  //end namespace
