/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include <iostream>
//-------------------------------------------------

namespace Trk {
 extern void dsinv(long int * , double *, long int , long int *);             
}
namespace VKalVrtAthena{

  void VrtSecInclusive::printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name)
  {
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
      std::cout<<name
      <<"= "<<(*WrkVrtSet)[iv].vertex[0]
      <<", "<<(*WrkVrtSet)[iv].vertex[1]
      <<", "<<(*WrkVrtSet)[iv].vertex[2]
      <<" NTrk="<<(*WrkVrtSet)[iv].SelTrk.size()
      <<" trk=";
      for(int kk=0; kk<(int)(*WrkVrtSet)[iv].SelTrk.size(); kk++) {
                std::cout<<", "<<(*WrkVrtSet)[iv].SelTrk[kk];}
      std::cout<<'\n';
    }
  }


  StatusCode VrtSecInclusive::CutTrk(double PInvVert,double ThetaVert, 
         double A0Vert, double ZVert, double Chi2, 
         long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits)
  {
     double Pt = sin(ThetaVert)/fabs(PInvVert);
//- Track quality
     m_trkSelCuts->Fill(20.); 

     if(Pt               < m_TrkPtCut) 			return StatusCode::FAILURE;
     m_trkSelCuts->Fill(21.); 
     if(Chi2 	         > m_TrkChi2Cut) 		return StatusCode::FAILURE;
     m_trkSelCuts->Fill(22.); 

     if(fabs(A0Vert)     < m_a0TrkPVDstMinCut) 	        return StatusCode::FAILURE;
     if(fabs(A0Vert)     > m_a0TrkPVDstMaxCut) 	        return StatusCode::FAILURE;
     m_trkSelCuts->Fill(23.); 

     if(fabs(ZVert)     < m_zTrkPVDstMinCut) 	        return StatusCode::FAILURE;
     if(fabs(ZVert)     > m_zTrkPVDstMaxCut) 	        return StatusCode::FAILURE;
     m_trkSelCuts->Fill(24.); 

    // do Pixel/SCT/SiHits only if we exclude StandAlone TRT hits
     //
     if(!m_SAloneTRT) {
      if(PixelHits	    < m_CutPixelHits) 		return StatusCode::FAILURE;
      m_trkSelCuts->Fill(25.); 

      if(SctHits		    < m_CutSctHits) 		return StatusCode::FAILURE;
      m_trkSelCuts->Fill(26.); 

      if((PixelHits+SctHits) < m_CutSiHits) 		return StatusCode::FAILURE;
      m_trkSelCuts->Fill(27.); 
      if(BLayHits	    < m_CutBLayHits) 		return StatusCode::FAILURE;
      m_trkSelCuts->Fill(28.); 
      if(SharedHits	    > m_CutSharedHits) 		return StatusCode::FAILURE;
      m_trkSelCuts->Fill(29.); 
     }
     else {return StatusCode::SUCCESS;}
     //
     return StatusCode::SUCCESS;
  }
 




   void  VrtSecInclusive::SelGoodTrkParticle( const vector<const Trk::TrackParticleBase*>& InpTrk,
                                              const Trk::RecVertex                        & PrimVrt,
                                              vector<const Trk::TrackParticleBase*>& SelectedTracks)
  {    

    MsgStream mlog( messageService(), name() );
    mlog << MSG::DEBUG << "VrtSecInclusive SelGoodTrkParticle()" << endreq;

    std::vector<const Trk::TrackParticleBase*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee* m_mPer;
    const Trk::FitQuality*     TrkQual;
    vector<double> Impact,ImpactError;
    // PV that was obtained in VrtSecInclusive.cxx
    vector<Amg::Vector3D> vxPositions;
    vxPositions.push_back(Amg::Vector3D(m_PVX,m_PVY,m_PVZ));
    mlog<<MSG::DEBUG<<"vxPositions "<<vxPositions[0].x()<<","<<vxPositions[0].y()<<","<<vxPositions[0].z()<<endreq;
    //
    m_trkSelCuts->Fill(1.); 
    ATH_MSG_DEBUG("here:0 ");
    //
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
      m_trkSelCuts->Fill(2.); 
      m_mPer=GetPerigee( (*i_ntrk) ) ;
      if( m_mPer == NULL ){ continue; } 
      m_trkSelCuts->Fill(3.); 
      VectPerig = m_mPer->parameters(); 
      TrkQual   = (*i_ntrk)->fitQuality();
      double CovTrkMtx11 = (*(m_mPer->covariance()))(Trk::d0,Trk::d0);
      double CovTrkMtx22 = (*(m_mPer->covariance()))(Trk::z0,Trk::z0);
      //
      long int PixelHits     = 3;
      long int SctHits       = 9; 
      long int SharedHits    = 0;
      long int BLayHits      = 1;
      //----------------------------------- Summary tools
      ATH_MSG_DEBUG("here:0a ");
      const Trk::TrackSummary* testSum = (*i_ntrk)->trackSummary();
      ATH_MSG_DEBUG("here:0b ");
      PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
      SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
      BLayHits  = (long int) testSum->get(Trk::numberOfBLayerHits);
      long int TRTHits = (long int) testSum->get(Trk::numberOfTRTHits);          
      long int PixShare = (long int) testSum->get(Trk::numberOfPixelSharedHits);
      long int SctShare = (long int) testSum->get(Trk::numberOfSCTSharedHits);
      SharedHits = PixShare + SctShare;
      if(PixelHits < 0 ) PixelHits=0; 
      if(SctHits   < 0 ) SctHits=0; 
      if(BLayHits  < 0 ) BLayHits=0; 
      ATH_MSG_DEBUG("here:0c ");
      //std::cout<<"NwTrkSummary="<<PixelHits<<", "<<SctHits<<", "<<BLayHits<<'\n';
      //double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
      m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);  // ImpactSignif not needed
      ATH_MSG_DEBUG("here:0c0 ");
      m_trkSelCuts->Fill(4.); 
      ATH_MSG_DEBUG("here:0c1 ");
      double ImpactA0=VectPerig[0];                         // relative to (0,0,0)
      ATH_MSG_DEBUG("here:0c2 ");
      double ImpactZ=VectPerig[1]-PrimVrt.position().z();   // (0,0,0)
      ATH_MSG_DEBUG("here:0c3 ");
      ImpactA0=Impact[0];  // these are relative to PV
      ATH_MSG_DEBUG("here:0c4 ");
      ImpactZ=Impact[1];   

      ATH_MSG_DEBUG("here:0d ");
      double theta = m_mPer->parameters()[Trk::theta];
      double qOverP = m_mPer->parameters()[Trk::qOverP];
      double ptrk = (1./qOverP);
      double pT = ptrk*sin(theta);
      double trketa = -1.*log( tan(theta/2));
      double chi2 = (TrkQual->chiSquared() / TrkQual->numberDoF());
      //
      // relative to (0,0,0)
      ATH_MSG_DEBUG("here:1 ");
      m_RecoTrk_pT->push_back(pT);
      m_RecoTrk_eta->push_back(trketa);
      m_RecoTrk_phi->push_back(m_mPer->parameters()[Trk::phi]);
      m_RecoTrk_chi2->push_back(chi2);
      //
      m_RecoTrk_2dIP->push_back(m_mPer->parameters()[Trk::d0]); 
      m_RecoTrk_zIP->push_back(m_mPer->parameters()[Trk::z0]); 
      m_RecoTrk_A0Error->push_back(CovTrkMtx11);
      m_RecoTrk_ZError->push_back(CovTrkMtx22);
      // also these two
      m_RecoTrk_2dIPPV->push_back(ImpactA0); // relative to PV
      m_RecoTrk_zIPPV->push_back(ImpactZ); 
      //
      m_RecoTrk_numBLay->push_back(BLayHits);
      m_RecoTrk_numPix->push_back(PixelHits);
      m_RecoTrk_numSCT->push_back(SctHits);
      m_RecoTrk_numTRT->push_back(TRTHits);
      m_RecoTrk_PixShare->push_back(PixShare);
      m_RecoTrk_SctShare->push_back(SctShare);
      m_RecoTrk_PixBar1->push_back((int) testSum->isHit(Trk::pixelBarrel1)); 
      m_RecoTrk_PixBar2->push_back((int) testSum->isHit(Trk::pixelBarrel2)); 
      ATH_MSG_DEBUG("here:2 ");
      /*
      m_RecoTrk_SctBar0->push_back((int) testSum->isHit(Trk::sctBarrel0)); 
      m_RecoTrk_SctBar1->push_back((int) testSum->isHit(Trk::sctBarrel1)); 
      m_RecoTrk_SctBar2->push_back((int) testSum->isHit(Trk::sctBarrel2)); 
      m_RecoTrk_SctBar3->push_back((int) testSum->isHit(Trk::sctBarrel3)); 
      int TRTHoles = testSum->get(Trk::numberOfTRTHoles);
      m_RecoTrk_numTRTholes->push_back(TRTHoles);
      int TRTOut = testSum->get(Trk::numberOfTRTOutliers);
      m_RecoTrk_numTRTout->push_back(TRTOut);
      */
      //
      // some B layer stuff
      /*
      m_RecoTrk_BLayOut->push_back( testSum->get(Trk::numberOfBLayerOutliers));
      
      double blx=0; double bly=0; double blz=0;
      const std::vector<const Trk::ParametersBase*> params = (*i_ntrk)->trackParameters();
        //Just a sanity check:
      if (params.size() ==0 ) {
          mlog<<MSG::WARNING<<"Error: no track parameters"<<endreq;
        }else{
	std::vector<const Trk::ParametersBase*>::const_iterator itp=params.begin();
        std::vector<const Trk::ParametersBase*>::const_iterator itpE = params.end();
        bool foundBL=false;
        for (;itp!=itpE;++itp) {

	  if(foundBL) continue;
	  double lx = (*itp)->position().x(); double ly = (*itp)->position().y(); 
	  double lz = (*itp)->position().z(); double lr = (*itp)->position().perp();
	  mlog<<MSG::DEBUG<<"lx/ly/lr/lz "<<lx<<","<<ly<<","<<lr<<","<<lz<<endreq;
	  //
	  if(lr>48 && lr<54) { // within active sensor of B layer
	    blx = lx; bly = ly; blz = lz;
	    foundBL=true;
	  }
	  else {blx=0; bly=0; blz=0;}

	} // loop over parameters
      } // TrackParameters exist
      
      m_RecoTrk_BLayX->push_back(blx);m_RecoTrk_BLayY->push_back(bly);m_RecoTrk_BLayZ->push_back(blz);
      */
      //
      //
      ATH_MSG_DEBUG("here:3 ");
      double matchProb=-1;
      int barcode=0;
      if(m_doTruth) { // get the matching GenParticle
	HepMC::GenParticle* aTemp_truth = getTrkGenParticle((*i_ntrk), matchProb);
	if(aTemp_truth) {// found it
	  barcode = aTemp_truth->barcode();
	}
      }
      m_RecoTrk_barcode->push_back(barcode);
      m_RecoTrk_matchPr->push_back(matchProb);

      mlog<<MSG::DEBUG<<" Reco Trk d0PV/z0PV/Perigee(pT/eta/phi)/Pixel/SCT/SharedHits/chi2/match bc/pr/TRTHits/Holes/Out "<<ImpactA0<<","<<ImpactZ<<","<<pT<<","<<trketa<<","<<m_mPer->parameters()[Trk::phi]<<","<<PixelHits<<","<<SctHits<<","<<SharedHits<<","<<chi2<<","<<barcode<<","<<matchProb<<endreq;
      //
      // get track's d0/z0 parameters at PV
      // only for testing
      /*
	const Rec::TrackParticle* TrkPart = dynamic_cast<const Rec::TrackParticle*>(*i_ntrk);
	const Trk::Perigee* PV_Peri=NULL;
	if(TrkPart) {
	const Trk::Perigee* test_per = GetPerigee(TrkPart) ;
	//
	double temp1P = test_per->parameters()[Trk::qOverP];
	double tempth = test_per->parameters()[Trk::theta];
	double temppt = (1./temp1P)*sin(tempth);
	double tempeta = -1.*log( tan(tempth/2));

	mlog<<MSG::DEBUG<<"(rel to 0,0,0): d0/z0/pT/eta/phi/theta "<<test_per->parameters()[Trk::d0]<<","<<test_per->parameters()[Trk::z0]<<","<<temppt<<","<<tempeta<<","<<test_per->parameters()[Trk::phi]<<","<<test_per->parameters()[Trk::theta]<<endreq;

	// get new perigee at PV           
	PV_Peri = m_trackToVertexTool->perigeeAtVertex(*TrkPart, vxPositions[0]);
	}
	if(PV_Peri == NULL) {
	mlog<<MSG::DEBUG<<"extrap to PV didn't work "<<endreq;
	}
	else{
	double temp1P = PV_Peri->parameters()[Trk::qOverP];
	double tempth = PV_Peri->parameters()[Trk::theta];
	double temppt = (1./temp1P)*sin(tempth);
	double tempeta = -1.*log( tan(tempth/2));

	mlog<<MSG::DEBUG<<"rel to PV: d0/z0/pT/eta/phi/theta "<<PV_Peri->parameters()[Trk::d0]<<","<<PV_Peri->parameters()[Trk::z0]<<","<<temppt<<","<<tempeta<<","<<PV_Peri->parameters()[Trk::phi]<<","<<PV_Peri->parameters()[Trk::theta]<<endreq;
	}
      */
      // extrapolate track to given surfaces
      //
      getIntersection(m_mPer); // get intersection and store values in ntuple

      //
      const Trk::TrackInfo& trackInfo = (*i_ntrk)->info();
      int sitrk = trackInfo.patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder);
      int sitrkWTRT = trackInfo.patternRecoInfo(Trk::TrackInfo::InDetExtensionProcessor);
      int BackTrk = trackInfo.patternRecoInfo(Trk::TrackInfo::TRTSeededTrackFinder);
      int SAlone = trackInfo.patternRecoInfo(Trk::TrackInfo::TRTStandalone);
      //

      int trkauth=0;
      if(sitrk==1 && sitrkWTRT==1) trkauth=1;
      if(sitrk==1 && sitrkWTRT==0) trkauth=2;
      if(BackTrk==1) trkauth=3;
      if(SAlone==1) trkauth=4;
      if((sitrk==1 || sitrkWTRT==1) && (BackTrk==1 || SAlone==1)) trkauth=-1;
      //
      m_RecoTrk_TrkAuth->push_back(trkauth);
      //          int lowpttrk = trackInfo.trackProperties(Trk::TrackInfo::LowPtTrack); 
      int lowpttrk =  trackInfo.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum);
      m_RecoTrk_TrkLowPt->push_back(lowpttrk);


      //
      //	  if(fabs(pT)>500) mlog<<MSG::INFO<<" Reco Trk Perigee(pT/eta)/Pixel/SCT/match pr/sitrk/siWTRT/BackTrk/SAlone "<<pT<<","<<trketa<<","<<PixelHits<<","<<SctHits<<","<<matchProb<<" ---- "<<sitrk<<","<<sitrkWTRT<<","<<BackTrk<<","<<SAlone<<" trkauth = "<<trkauth<<" --lowpt trk -- "<<lowpttrk<<" TRT "<<TRTHits<<","<<TRTHoles<<","<<TRTOut<<endreq;


      if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
      m_trkSelCuts->Fill(5.); 
      if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
      m_trkSelCuts->Fill(6.); 

      if ( fabs(ImpactA0)/sqrt(ImpactError[0]) < m_a0TrkPVSignifCut ) continue;
      if ( fabs(ImpactZ) /sqrt(ImpactError[2]) < m_zTrkPVSignifCut  ) continue;
      m_trkSelCuts->Fill(7.); 


      StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3], ImpactA0 , ImpactZ ,
			      chi2, PixelHits, SctHits, SharedHits, BLayHits);
      m_trkSelCuts->Fill(8.); 
					  
      if( sc.isFailure() )                 continue;
      m_trkSelCuts->Fill(9.); 
      SelectedTracks.push_back(*i_ntrk);
      mlog<<MSG::DEBUG<<" Track has been selected "<<endreq;
      m_SelTrk_barcode->push_back(barcode); // will need this later          
    } // end of track loop
    mlog<<MSG::DEBUG<<"Size of Inp Reco / w/barcode = "<<InpTrk.size()<<","<<m_RecoTrk_barcode->size()<<endreq; 
  }

  //
  // get intersection point of track with various surfaces
  //
  void VrtSecInclusive::getIntersection(const Trk::Perigee* per) {
    MsgStream mlog( messageService(), name() );
    //
    const Trk::TrackParameters* Output =0;
    Amg::Transform3D *trnsf = new Amg::Transform3D(); // remember to delete at the end
    (*trnsf).setIdentity();
    Trk::CylinderSurface surfBorder(trnsf, m_avRad_bp, 300000.); // 
    Output = m_extrapolator->extrapolateDirectly(*per,surfBorder,Trk::anyDirection,true,Trk::pion);
    double intz = -10000;
    Amg::Vector3D IntPoint;
    if(Output) {
      IntPoint = Output->position();
      intz = IntPoint.z();
    }
    mlog<<MSG::DEBUG<<"z at bp "<<intz<<endreq;
    m_RecoTrk_Int_bpZ->push_back(intz);
    
    // now pixel 1
    if(Output) delete Output;
    Amg::Transform3D *trnsf1 = new Amg::Transform3D(); // remember to delete at the end
    (*trnsf1).setIdentity();
    Trk::CylinderSurface surfBorder1(trnsf1, m_avRad_pix1, 300000.); 
    Output = m_extrapolator->extrapolateDirectly(*per,surfBorder1,Trk::anyDirection,true,Trk::pion);
    intz = -10000;
    if(Output) {
      IntPoint = Output->position();
      intz = IntPoint.z();
    }
    mlog<<MSG::DEBUG<<"z at pix1 "<<intz<<endreq;
    m_RecoTrk_Int_pix1Z->push_back(intz);
    // now pixel 2
    if(Output) delete Output;
    Amg::Transform3D *trnsf2 = new Amg::Transform3D(); // remember to delete at the end
    (*trnsf2).setIdentity();
    Trk::CylinderSurface surfBorder2(trnsf2, m_avRad_pix2, 300000.); 
    Output = m_extrapolator->extrapolateDirectly(*per,surfBorder2,Trk::anyDirection,true,Trk::pion);
    intz = -10000;
    if(Output) {
      IntPoint = Output->position();
      intz = IntPoint.z();
    }
    mlog<<MSG::DEBUG<<"z at pix2 "<<intz<<endreq;
    m_RecoTrk_Int_pix2Z->push_back(intz);
    // now pixel 3
    if(Output) delete Output;
    Amg::Transform3D *trnsf3 = new Amg::Transform3D(); // remember to delete at the end
    (*trnsf3).setIdentity();
    Trk::CylinderSurface surfBorder3(trnsf3, m_avRad_pix3, 300000.); 
    Output = m_extrapolator->extrapolateDirectly(*per,surfBorder3,Trk::anyDirection,true,Trk::pion);
    intz = -10000;
    if(Output) {
      IntPoint = Output->position();
      intz = IntPoint.z();
    }
    mlog<<MSG::DEBUG<<"z at pix3 "<<intz<<endreq;
    m_RecoTrk_Int_pix3Z->push_back(intz);
    // now sct 1
    if(Output) delete Output;
    Amg::Transform3D *trnsf4 = new Amg::Transform3D(); // remember to delete at the end
    (*trnsf4).setIdentity();
    Trk::CylinderSurface surfBorder4(trnsf4, m_avRad_sct1, 300000.); 
    Output = m_extrapolator->extrapolateDirectly(*per,surfBorder4,Trk::anyDirection,true,Trk::pion);
    intz = -10000;
    if(Output) {
      IntPoint = Output->position();
      intz = IntPoint.z();
    }
    mlog<<MSG::DEBUG<<"z at sct1 "<<intz<<endreq;
    m_RecoTrk_Int_sct1Z->push_back(intz);
    // now sct 2
    if(Output) delete Output;
    Amg::Transform3D *trnsf5 = new Amg::Transform3D(); // remember to delete at the end
    (*trnsf5).setIdentity();
    Trk::CylinderSurface surfBorder5(trnsf5, m_avRad_sct2, 300000.); 
    Output = m_extrapolator->extrapolateDirectly(*per,surfBorder5,Trk::anyDirection,true,Trk::pion);
    intz = -10000;
    if(Output) {
      IntPoint = Output->position();
      intz = IntPoint.z();
    }
    mlog<<MSG::DEBUG<<"z at sct2 "<<intz<<endreq;
    m_RecoTrk_Int_sct2Z->push_back(intz);
    //
    if(Output) delete Output;
    mlog<<MSG::DEBUG<<"End of getIntersection"<<endreq;
  }
  //
  //
  const Trk::Perigee* VrtSecInclusive::GetPerigee( const Trk::TrackParticleBase* i_ntrk) 
  {
       //const Rec::TrackParticle* tmpP = dynamic_cast<const Rec::TrackParticle*> (i_ntrk);
       return i_ntrk->perigee() ;
  }

  
  double VrtSecInclusive::VrtVrtDist(const Amg::Vector3D & Vrt1, const vector<double>  & VrtErr1,
                                     const Amg::Vector3D & Vrt2, const vector<double>  & VrtErr2)
  {
    double Signif=0.;
    double distx =  Vrt1.x()- Vrt2.x();
    double disty =  Vrt1.y()- Vrt2.y();
    double distz =  Vrt1.z()- Vrt2.z();

    double mmm[3][3];
 
    mmm[0][0] =             VrtErr1[0]+VrtErr2[0];
    mmm[0][1] = mmm[1][0] = VrtErr1[1]+VrtErr2[1];
    mmm[1][1] =             VrtErr1[2]+VrtErr2[2];
    mmm[0][2] = mmm[2][0] = VrtErr1[3]+VrtErr2[3];
    mmm[1][2] = mmm[2][1] = VrtErr1[4]+VrtErr2[4];
    mmm[2][2] =             VrtErr1[5]+VrtErr2[5];

    long int jfail,NN=3;
    Trk::dsinv( &NN, &mmm[0][0], NN, &jfail);

    if(jfail==0){
        Signif = distx*mmm[0][0]*distx
                +disty*mmm[1][1]*disty
                +distz*mmm[2][2]*distz
             +2.*distx*mmm[0][1]*disty
             +2.*distx*mmm[0][2]*distz
             +2.*disty*mmm[1][2]*distz;
        Signif=sqrt(Signif);
    }

    return Signif;
  }

} // end of namespace
