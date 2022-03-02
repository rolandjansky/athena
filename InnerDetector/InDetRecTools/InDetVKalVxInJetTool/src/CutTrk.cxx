/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// Author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "TH1F.h"

//-------------------------------------------------
namespace InDet{



  StatusCode InDetVKalVxInJetTool::cutTrk(std::unordered_map<std::string,double> TrkVarDouble,
                                          std::unordered_map<std::string,int> TrkVarInt,
					  float evtWgt)
  const
  {

    double eta         = TrkVarDouble["eta"];
    double PInvVert    = TrkVarDouble["PInvVert"];
    double ThetaVert   = TrkVarDouble["ThetaVert"];
    double A0Vert      = TrkVarDouble["A0Vert"];
    double ZVert       = TrkVarDouble["ZVert"];
    double Chi2        = TrkVarDouble["Chi2"];
    double ConeDist    = TrkVarDouble["ConeDist"];
    double CovTrkMtx11 = TrkVarDouble["CovTrkMtx11"];
    double CovTrkMtx22 = TrkVarDouble["CovTrkMtx22"];
    double trkP        = TrkVarDouble["trkP"];
    double trkPErr     = TrkVarDouble["trkPErr"];

    int PixelHits  = TrkVarInt["PixelHits"];
    int SctHits    = TrkVarInt["SctHits"];
    int BLayHits   = TrkVarInt["BLayHits"];
    int SharedHits = TrkVarInt["SharedHits"];
    int badHits    = TrkVarInt["badHits"];

    if ( CovTrkMtx11 > m_a0TrkErrorCut*m_a0TrkErrorCut )  return StatusCode::FAILURE;
    if ( CovTrkMtx22 > m_zTrkErrorCut*m_zTrkErrorCut )    return StatusCode::FAILURE;
    if ( ConeDist > m_coneForTag )                        return StatusCode::FAILURE;
    if(trkP>10000.){
      if(m_fillHist)m_hb_trkPErr->Fill( trkPErr , evtWgt);
      if(trkPErr>0.5) return StatusCode::FAILURE;
    }

     double Pt = sin(ThetaVert)/std::abs(PInvVert);
     //- Track quality

     double pT_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinPtAtEta(eta) : m_cutPt;
     if(Pt               < pT_cut) 		return StatusCode::FAILURE;

     if(!m_multiWithPrimary){           //Must not be used for primary vertex search
       double z0_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxZImpactAtEta(eta) : m_cutZVrt;
       // Eta-dependent cuts with z0*sin(theta)
       if(m_useEtaDependentCuts && std::abs(ZVert*sin(ThetaVert)) > z0_cut) return StatusCode::FAILURE;
       //Otherwise cuts with z0
       else if(!m_useEtaDependentCuts && std::abs(ZVert) > z0_cut)	    return StatusCode::FAILURE;
     }

     double chi2_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxChi2AtEta(eta) : m_cutChi2;
     if(Chi2 	         > chi2_cut) 		return StatusCode::FAILURE;

     double d0_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxPrimaryImpactAtEta(eta) : m_cutA0;
     if(std::abs(A0Vert)     > d0_cut) 		return StatusCode::FAILURE;

     int pix_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinPixelHitsAtEta(eta) : m_cutPixelHits;
     if(!m_useEtaDependentCuts && std::abs(eta)>2.){
       if(badHits && PixelHits<=3) return StatusCode::FAILURE;
       PixelHits--;
     }
     if(PixelHits	    < pix_cut) 		return StatusCode::FAILURE;

     int strip_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinStripHitsAtEta(eta) : m_cutSctHits;
     if(!m_useEtaDependentCuts){
       if(SctHits<3) return StatusCode::FAILURE;
       if(std::abs(eta)>2. && m_existIBL) SctHits--;
       if(std::abs(eta)>1.65) SctHits--;
     }
     if(SctHits		    < strip_cut) 	return StatusCode::FAILURE;

     int si_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinSiHitsAtEta(eta) : m_cutSiHits;
     if((PixelHits+SctHits) < si_cut) 		return StatusCode::FAILURE;

     int inpix_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinInnermostPixelHitsAtEta(eta) : m_cutBLayHits;
     if(BLayHits	    < inpix_cut) 	return StatusCode::FAILURE;

     int shared_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxSharedAtEta(eta) : m_cutSharedHits;
     if(SharedHits          > shared_cut)       return StatusCode::FAILURE;

     return StatusCode::SUCCESS;

  }
 
//==============================================================================================================
//          xAOD based stuff
//
   int  InDetVKalVxInJetTool::selGoodTrkParticle( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                  const xAOD::Vertex                           & PrimVrt,
	                                          const TLorentzVector                         & JetDir,
                                                        std::vector<const xAOD::TrackParticle*>& SelectedTracks,
							float evtWgt)
   const
   {    

    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> Impact,ImpactError;
    std::multimap<double,const xAOD::TrackParticle*> orderedTrk;
    int nPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {

          if((*i_ntrk)->numberDoF() == 0) continue; //Protection
          if( (*i_ntrk)->pt() > JetDir.Pt() )                   continue;

          //
          //-- Perigee in TrackParticle
          //
          const Trk::Perigee mPer=(*i_ntrk)->perigeeParameters() ;
          AmgVector(5) VectPerig = mPer.parameters(); 


          double trkChi2 = (*i_ntrk)->chiSquared() / (*i_ntrk)->numberDoF();

          double CovTrkMtx11 = (*i_ntrk)->definingParametersCovMatrix()(0,0);
          double CovTrkMtx22 = (*i_ntrk)->definingParametersCovMatrix()(1,1);
          double CovTrkMtx55 = (*i_ntrk)->definingParametersCovMatrix()(4,4);
          double ConeDist = coneDist(VectPerig,JetDir);

          double trkP=1./std::abs(VectPerig[4]);
          double trkPErr=std::sqrt(CovTrkMtx55)*trkP ;

          uint8_t PixelHits,SctHits,BLayHits,sctSharedHits,pixSharedHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) )   continue; // Track is 
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   continue; // definitely bad
          if( !((*i_ntrk)->summaryValue(BLayHits,xAOD::numberOfInnermostPixelLayerHits)))  BLayHits=0;
	  if( !((*i_ntrk)->summaryValue(sctSharedHits,xAOD::numberOfSCTSharedHits)))  sctSharedHits=0;  //VK Bad for high Pt
          if( !((*i_ntrk)->summaryValue(pixSharedHits,xAOD::numberOfPixelSharedHits)))pixSharedHits=0;  //and many tracks
	  long int SharedHits = sctSharedHits+pixSharedHits;

          uint8_t splSCTHits,outSCTHits,splPixHits,outPixHits;
          if( !((*i_ntrk)->summaryValue(splSCTHits,xAOD::numberOfSCTSpoiltHits)))  splSCTHits=0;
          if( !((*i_ntrk)->summaryValue(outSCTHits,xAOD::numberOfSCTOutliers)))    outSCTHits=0;
          if( !((*i_ntrk)->summaryValue(splPixHits,xAOD::numberOfPixelSpoiltHits)))splPixHits=0;
          if( !((*i_ntrk)->summaryValue(outPixHits,xAOD::numberOfPixelOutliers)))  outPixHits=0;
          bool badHits = ( splSCTHits || outSCTHits || outPixHits || splPixHits );

          Amg::Vector3D perigeePos=mPer.position();
          double ImpactA0=std::sqrt( (perigeePos.x()-PrimVrt.x())*(perigeePos.x()-PrimVrt.x())
				     +(perigeePos.y()-PrimVrt.y())*(perigeePos.y()-PrimVrt.y()) );
          double ImpactZ=perigeePos.z()-PrimVrt.z();
          double ImpactSignif=std::sqrt(ImpactA0*ImpactA0/CovTrkMtx11+ImpactZ*ImpactZ/CovTrkMtx22);

          if(m_fillHist){  m_hb_trkD0->Fill( ImpactA0, evtWgt); }

	  double eta = (*i_ntrk)->eta();

	  std::unordered_map<std::string,double> TrkVarDouble;
	  std::unordered_map<std::string,int> TrkVarInt;

	  TrkVarDouble["eta"]         = eta;
	  TrkVarDouble["PInvVert"]    = VectPerig[4];
	  TrkVarDouble["ThetaVert"]   = VectPerig[3];
	  TrkVarDouble["A0Vert"]      = ImpactA0;
	  TrkVarDouble["ZVert"]       = ImpactZ;
	  TrkVarDouble["Chi2"]        = trkChi2;
	  TrkVarDouble["ConeDist"]    = ConeDist;
	  TrkVarDouble["CovTrkMtx11"] = CovTrkMtx11;
	  TrkVarDouble["CovTrkMtx22"] = CovTrkMtx22;
	  TrkVarDouble["trkP"]        = trkP;
	  TrkVarDouble["trkPErr"]     = trkPErr;

	  TrkVarInt["PixelHits"]  = PixelHits;
	  TrkVarInt["SctHits"]    = SctHits;
	  TrkVarInt["BLayHits"]   = BLayHits;
	  TrkVarInt["badHits"]    = badHits;
	  TrkVarInt["SharedHits"] = SharedHits;

          StatusCode sc = cutTrk(TrkVarDouble, TrkVarInt);
          if( sc.isFailure() )                 continue;

	  double rnkBTrk=rankBTrk(0.,0.,ImpactSignif);
	  if(rnkBTrk<0.7)nPrimTrk += 1;
	  orderedTrk.emplace(rnkBTrk,*i_ntrk);
    }

    //---- Order tracks according to ranks
    std::map<double,const xAOD::TrackParticle*>::reverse_iterator rt=orderedTrk.rbegin();
    SelectedTracks.resize(orderedTrk.size());
    for ( int cntt=0; rt!=orderedTrk.rend(); ++rt,++cntt) {SelectedTracks[cntt]=(*rt).second;}
    return nPrimTrk;
   }

}//end namespace
