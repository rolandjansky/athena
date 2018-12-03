/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTaggingValidationPlots.h"
#include "ParticleJetTools/JetFlavourInfo.h" 
using CLHEP::GeV;

namespace JetTagDQA{

	BTaggingValidationPlots::BTaggingValidationPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
															 m_sParticleType(sParticleType),m_isJVT_defined(false)
	{
	  //std::cout << "m_sParticleType=" << m_sParticleType << std::endl;
	  if (m_sParticleType=="antiKt4EMTopoJets") { m_isJVT_defined = true; m_jvt_cut = 0.59;};
	  if (m_sParticleType=="antiKt4EMPFlowJets") {m_isJVT_defined = true; m_jvt_cut = 0.2;};
	}     
	 
	void BTaggingValidationPlots::initializePlots(){

		bookEffHistos();		

		m_truthLabel = Book1D("truthLabel", "truthLabel of "+ m_sParticleType +"; truthLabel ;Events", 17, -1., 16);
//		m_GAFinalHadronLabel = Book1D("GAFinalHadronLabel", "GAFinalHadronLabel of "+ m_sParticleType +"; GAFinalHadronLabel ;Events", 17, -1., 16);
//		m_GAInitialHadronLabel = Book1D("GAInitialHadronLabel", "GAInitialHadronLabel of "+ m_sParticleType +"; GAInitialHadronLabel ;Events", 17, -1., 16);
//		m_GAFinalPartonLabel = Book1D("GAFinalPartonLabel", "GAFinalPartonLabel of "+ m_sParticleType +"; GAFinalPartonLabel ;Events", 17, -1., 16);

//		m_GAFinalHadronC_dR = Book1D("GAFinalHadronC_dR", "DeltaR between final hadron C and "+ m_sParticleType +"; dR ;Events", 50, 0, 1);
//		m_GAInitialHadronC_dR = Book1D("GAInitialHadronC_dR", "DeltaR between initial hadron C and "+ m_sParticleType +"; dR ;Events", 50, 0, 1);
//		m_GAFinalPartonC_dR = Book1D("GAFinalPartonC_dR", "DeltaR between final parton C and "+ m_sParticleType +"; dR ;Events", 50, 0, 1);
//		m_GAFinalHadronTau_dR = Book1D("GAFinalHadronTau_dR", "DeltaR between final hadron tau and "+ m_sParticleType +"; dR ;Events", 50, 0, 1);

		m_truthPt_b  = Book1D("TruthBpt", "p_{T} of truth"+ m_sParticleType +"; p_{T} (GeV) ;Events", 100, 0., 1000);
		m_truthPt_u  = Book1D("TruthLpt", "p_{T} of truth"+ m_sParticleType +", light jets; p_{T} (GeV) ;Events", 100, 0., 1000);
		m_truthPt_c  = Book1D("TruthCpt", "p_{T} of truth"+ m_sParticleType +", c-jets; p_{T} (GeV) ;Events", 100, 0., 1000);
		m_truthPt_tau  = Book1D("TruthTauPt", "p_{T} of truth"+ m_sParticleType +", tau-jets; p_{T} (GeV) ;Events", 100, 0., 1000);

		m_IP3D_pb = Book1D("IP3Dpb", "IP3D pb of "+ m_sParticleType +"; IP3Dpb ;Events", 100, 0., 1.);
		m_IP3D_pc = Book1D("IP3Dpc", "IP3D pc of "+ m_sParticleType +"; IP3Dpc ;Events", 100, 0., 1.);
		m_IP3D_pu = Book1D("IP3Dpu", "IP3D pu of "+ m_sParticleType +"; IP3Dpu ;Events", 100, 0., 1.);

		m_SV1_pb = Book1D("SV1pb", "SV1 pb of "+ m_sParticleType +"; SV1pb ;Events", 100, 0., 1.);
		m_SV1_pc = Book1D("SV1pc", "SV1 pc of "+ m_sParticleType +"; SV1pc ;Events", 100, 0., 1.);
		m_SV1_pu = Book1D("SV1pu", "SV1 pu of "+ m_sParticleType +"; SV1pu ;Events", 100, 0., 1.);
		
		m_IP3D_nTracks = Book1D("NTracksIP3D", "number of IP3D tracks of "+ m_sParticleType +"; NTracks ;Events", 21, -1., 20.);
		m_IP2D_nTracks = Book1D("NTracksIP2D", "number of IP2D tracks of "+ m_sParticleType +"; NTracks ;Events", 21, -1., 20.);
		m_SV1_NGTinSvx = Book1D("NGTinSvx1", "number of good tracks in secondary vertex (SV1); NTracks ;Events", 21, -1., 20.);
		m_SV0_NGTinSvx = Book1D("NGTinSvx0", "number of good tracks in secondary vertex (SV0); NTracks ;Events", 21, -1., 20.);		

		m_e  = Book1D("e", "E of "+ m_sParticleType +"; E (GeV) ;Events", 100, 0., 1000);
		m_pt  = Book1D("pt", "p_{T} of "+ m_sParticleType +"; p_{T} (GeV) ;Events", 100, 0., 1000);
		m_eta = Book1D("eta", "#eta of " + m_sParticleType +";#eta; Events ", 200,-5,5.);
		m_phi = Book1D("phi", "#varphi of " + m_sParticleType +";#varphi;Events ",128 ,-3.2,3.2);

		//ANDREA --- Store tracking info
		m_track_d0       = Book1D("d0", "d_{0} of BTagTrackToJetAssociator -> "+ m_sParticleType +"; d_{0} (mm) ;Events", 200, -2, 2);
		m_track_z0       = Book1D("z0", "z_{0} of BTagTrackToJetAssociator -> "+ m_sParticleType +"; z_{0} (mm) ;Events", 200, -10, 10); 
		//m_track_sigd0	 = Book1D("d0Sig", "d_{0} Significance of BTagTrackToJetAssociator -> "+ m_sParticleType +"; d_{0} / #sigma_{d_{0}} ;Events", 60, -3, 3);
		//m_track_sigz0    = Book1D("z0Sig", "z_{0} Significance of BTagTrackToJetAssociator -> "+ m_sParticleType +"; z_{0} / #sigma_{z_{0}} ;Events", 100, -5, 5);

		//ANDREA --- Store IPTag track grading/category
		m_IP3D_trackGrading = Book1D("IP3DgradeOfTracks","IP3D grade of Tracks "+m_sParticleType    +"; IP3D grade of Tracks ;Events"  ,12,-0.5,11.5);
		m_IP2D_trackGrading = Book1D("IP2DgradeOfTracks","IP2D grade of Tracks "+m_sParticleType    +"; IP2D grade of Tracks ;Events"  ,12,-0.5,11.5);
		m_tmpD0 = Book1D("IP3D_valD0wrtPVofTracks","IP3D valD0 wrt PV of Tracks "+m_sParticleType   +"; IP3D valD0 wrt PV of Tracks ;Events"  ,200,-2,2);
		m_tmpZ0 = Book1D("IP3D_valZ0wrtPVofTracks","IP3D valZ0 wrt PV of Tracks "+m_sParticleType   +"; IP3D valZ0 wrt PV of Tracks ;Events"  ,100,-0.5,0.5);
		m_tmpD0sig = Book1D("IP3D_sigD0wrtPVofTracks","IP3D sigD0 wrt PV of Tracks "+m_sParticleType+"; IP3D sigD0 wrt PV of Tracks ;Events"  ,200,-2,2);
		m_tmpZ0sig = Book1D("IP3D_sigZ0wrtPVofTracks","IP3D sigZ0 wrt PV of Tracks "+m_sParticleType+"; IP3D sigZ0 wrt PV of Tracks ;Events"  ,200,-2,2);
		m_tmpIP3DBwgt = Book1D("IP3D_weightBofTracks","IP3D weight B of Tracks"+m_sParticleType	    +"; IP3D weight B of Tracks ;Events"  ,200,-2,2);
		m_tmpIP3DUwgt = Book1D("IP3D_weightUofTracks","IP3D weight U of Tracks"+m_sParticleType	    +"; IP3D weight U of Tracks ;Events"  ,200,-2,2);
		m_tmpIP2DBwgt = Book1D("IP2D_weightBofTracks","IP2D weight B of Tracks"+m_sParticleType	    +"; IP2D weight B of Tracks ;Events"  ,200,-2,2);
		m_tmpIP2DUwgt = Book1D("IP2D_weightUofTracks","IP2D weight U of Tracks"+m_sParticleType	    +"; IP2D weight U of Tracks ;Events"  ,200,-2,2);	

	}
	
	void BTaggingValidationPlots::fill(const xAOD::BTagging* btag){
		int nIP3DTracks, nGTinSvx1, nIP2DTracks, nGTinSvx0;
		try{
			nIP3DTracks = btag->nIP3D_TrackParticles(); 
		}
		catch(std::exception& exception){
                        nIP3DTracks = -1;
                }
		m_IP3D_nTracks->Fill(nIP3DTracks);
		try{
			nIP2DTracks = btag->nIP2D_TrackParticles(); 
		}
		catch(std::exception& exception){
                        nIP2DTracks = -1;
                }
		m_IP2D_nTracks->Fill(nIP2DTracks);
		try{
			btag->taggerInfo(nGTinSvx1, xAOD::SV1_NGTinSvx); 
		}
		catch(std::exception& exception){
                        nGTinSvx1 = -1;
                }
		m_SV1_NGTinSvx->Fill(nGTinSvx1);
		try{
			btag->taggerInfo(nGTinSvx0, xAOD::SV0_NGTinSvx); 
		}
		catch(std::exception& exception){
                        nGTinSvx0 = -1;
                }
		m_SV0_NGTinSvx->Fill(nGTinSvx0);

		m_IP3D_pb->Fill(btag->IP3D_pb());
		m_IP3D_pc->Fill(btag->IP3D_pc());
		m_IP3D_pu->Fill(btag->IP3D_pu());
		m_SV1_pb->Fill(btag->SV1_pb());
		m_SV1_pc->Fill(btag->SV1_pc());
		m_SV1_pu->Fill(btag->SV1_pu());
	}

	void BTaggingValidationPlots::fill(int truthLabel){
		 m_truthLabel->Fill(truthLabel);
	}

	void BTaggingValidationPlots::fill(const xAOD::Jet* jet){
		m_e->Fill(jet->e()/GeV);
		m_pt->Fill(jet->pt()/GeV);
		m_eta->Fill(jet->eta());
		m_phi->Fill(jet->phi());
//		m_GAFinalHadronLabel->Fill(xAOD::GAFinalHadronFlavourLabel(jet));	
//		m_GAInitialHadronLabel->Fill(xAOD::GAInitialHadronFlavourLabel(jet));	
//		m_GAFinalPartonLabel->Fill(xAOD::GAFinalPartonFlavourLabel(jet));	
//		m_GAFinalHadronC_dR->Fill(xAOD::deltaR(jet, "GhostCHadronsFinal"));
//		m_GAInitialHadronC_dR->Fill(xAOD::deltaR(jet, "GhostCHadronsInitial"));
//		m_GAFinalPartonC_dR->Fill(xAOD::deltaR(jet, "GhostCQuarksFinal"));
//		m_GAFinalHadronTau_dR->Fill(xAOD::deltaR(jet, "GhostTausFinal"));
	}

	void BTaggingValidationPlots::fill(const xAOD::Jet* jet, const xAOD::BTagging* btag, const xAOD::Vertex *myVertex){

		int label(1000);

		if(jet->isAvailable<int>("HadronConeExclTruthLabelID")) jet->getAttribute("HadronConeExclTruthLabelID", label);
		else jet->getAttribute("TruthLabelID",label);	

 		//ANDREA --- Store tracking quantities
		const xAOD::BTagging* bjet = jet->btagging();
 		std::vector< ElementLink< xAOD::TrackParticleContainer > > assocTracks = bjet->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >("BTagTrackToJetAssociator");
		for (unsigned int iT=0; iT<assocTracks.size(); iT++) {
      		  if (!assocTracks.at(iT).isValid()) continue;
      		  const xAOD::TrackParticle* tmpTrk= *(assocTracks.at(iT));	 	
		  double d0(1000), z0(1000); //, sigma_d0(1000), sigma_z0(1000);				
		  d0 	         = tmpTrk->d0();
		  z0 	         = tmpTrk->z0() + tmpTrk->vz() - myVertex->z(); //ANDREA: tmpTrk->z0() is defined wrt the beam spot-> Get it wrt the PV
		  //z0 	         = tmpTrk->z0(); //Naive z0 wrt the beam spot

		  //ANDREA: for IP significances we already have IP-categories
		  //sigma_d0         = sqrt(tmpTrk->definingParametersCovMatrixVec().at(0)); 
		  //sigma_z0         = sqrt(tmpTrk->definingParametersCovMatrixVec().at(2));

		  //std::cout<<"****************** BS z0 = "<< tmpTrk->z0() <<" and PV z0 = "<< z0 <<std::endl;
		  //std::cout<<"****************** d0 = "<< d0 <<" and sigma_d0 = "<< sigma_d0 <<std::endl;
		  //if(sigma_d0<=0 || sigma_z0<=0) std::cout<<"********************** IP error = or < 0 !!!"<<std::endl;

		  if(label == 5) {
		  	m_track_d0     ->Fill(d0);
	  	  	m_track_z0     ->Fill(z0);
		  	//m_track_sigd0  ->Fill(d0/sigma_d0);
		  	//m_track_sigz0  ->Fill(z0/sigma_z0);
		  }
		}	
		//ANDREA --- Store tracking quantities

		//ANDREA --- IPTag categories
		std::vector<int>      tmpGrading   = bjet->auxdata<std::vector<int> >("IP3D_gradeOfTracks");
		std::vector<int>      tmpGrading2  = bjet->auxdata<std::vector<int> >("IP2D_gradeOfTracks");
		int IP3D_trackSize    = tmpGrading .size();
		int IP2D_trackSize    = tmpGrading2.size();
		m_IP3D_trackGrading   ->Fill( IP3D_trackSize );
		m_IP2D_trackGrading   ->Fill( IP2D_trackSize );

		std::vector<float> tmpD0       = bjet->auxdata<std::vector<float> >("IP3D_valD0wrtPVofTracks");
       	        std::vector<float> tmpZ0       = bjet->auxdata<std::vector<float> >("IP3D_valZ0wrtPVofTracks");
	        std::vector<float> tmpD0sig    = bjet->auxdata<std::vector<float> >("IP3D_sigD0wrtPVofTracks");
	        std::vector<float> tmpZ0sig    = bjet->auxdata<std::vector<float> >("IP3D_sigZ0wrtPVofTracks");
		for( unsigned int i=0; i<tmpD0   .size(); i++) m_tmpD0   ->Fill(tmpD0.at(i));
		for( unsigned int i=0; i<tmpZ0   .size(); i++) m_tmpZ0   ->Fill(tmpZ0.at(i));
		for( unsigned int i=0; i<tmpD0sig.size(); i++) m_tmpD0sig->Fill(tmpD0sig.at(i));
		for( unsigned int i=0; i<tmpZ0sig.size(); i++) m_tmpZ0sig->Fill(tmpZ0sig.at(i));
	
	        std::vector<float> tmpIP3DBwgt = bjet->auxdata<std::vector<float> >("IP3D_weightBofTracks");
	        std::vector<float> tmpIP3DUwgt = bjet->auxdata<std::vector<float> >("IP3D_weightUofTracks");
	        std::vector<float> tmpIP2DBwgt = bjet->auxdata<std::vector<float> >("IP2D_weightBofTracks");
	        std::vector<float> tmpIP2DUwgt = bjet->auxdata<std::vector<float> >("IP2D_weightUofTracks");

		for( unsigned int i=0; i<tmpIP3DBwgt   .size(); i++) m_tmpIP3DBwgt  ->Fill(tmpIP3DBwgt.at(i));
		for( unsigned int i=0; i<tmpIP3DUwgt   .size(); i++) m_tmpIP3DUwgt  ->Fill(tmpIP3DUwgt.at(i));
		for( unsigned int i=0; i<tmpIP2DBwgt   .size(); i++) m_tmpIP2DBwgt  ->Fill(tmpIP2DBwgt.at(i));
		for( unsigned int i=0; i<tmpIP2DUwgt   .size(); i++) m_tmpIP2DUwgt  ->Fill(tmpIP2DUwgt.at(i));
		//ANDREA --- IPTag categories

		int nGTinSvx(1000);
		int nIP3DTracks(1000);
		int nGTinSvx0(1000);
		int nIP2DTracks(1000);
		try{
			nIP3DTracks = btag->nIP3D_TrackParticles(); 
		}
		catch(std::exception& exception){
                        nIP3DTracks = -1;
                }
		try{
			nIP2DTracks = btag->nIP2D_TrackParticles(); 
		}
		catch(std::exception& exception){
                        nIP2DTracks = -1;
                }
		try{
			btag->taggerInfo(nGTinSvx, xAOD::SV1_NGTinSvx); 
		}
		catch(std::exception& exception){
                        nGTinSvx = -1;
                }
		try{
			btag->taggerInfo(nGTinSvx0, xAOD::SV0_NGTinSvx); 
		}
		catch(std::exception& exception){
                        nGTinSvx0 = -1;
                }

                if(jet->pt() > 20000 && std::abs(jet->eta()) < 2.5){
			//std::cout << "IP3D weight of jet: " << btag->IP3D_loglikelihoodratio() << std::endl;
                  	if(label == 5) m_truthPt_b->Fill(jet->pt()/GeV);
                  	if(label == 0) m_truthPt_u->Fill(jet->pt()/GeV);
                	if(label == 4) m_truthPt_c->Fill(jet->pt()/GeV);
                	if(label == 15) m_truthPt_tau->Fill(jet->pt()/GeV);
			for(std::map<std::string, TH1*>::const_iterator hist_iter=m_weight_histos.begin(); hist_iter!=m_weight_histos.end(); ++hist_iter){
				for(std::map<std::string, int>::const_iterator label_iter = m_truthLabels.begin(); label_iter != 					m_truthLabels.end(); ++label_iter){
					if((hist_iter->first).find("IP3D_") < 1 && (hist_iter->first).find("_matched") < (hist_iter->first).length()){
						if( (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length() && label == label_iter->second && (hist_iter->first).find("_weight") < (hist_iter->first).length()){
							if((hist_iter->first).find("_trackCuts") < (hist_iter->first).length()){
								if(nIP3DTracks > 0)(hist_iter->second)->Fill(btag->IP3D_loglikelihoodratio());
							}
							else (hist_iter->second)->Fill(btag->IP3D_loglikelihoodratio());
						}
						else{
							 for(std::map<std::string, double>::const_iterator ip3d_iter = m_IP3D_workingPoints.begin(); ip3d_iter != m_IP3D_workingPoints.end(); ++ip3d_iter){
								if((hist_iter->first).find(ip3d_iter->first) < (hist_iter->first).length() && (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length()){
									if(label == label_iter->second && btag->IP3D_loglikelihoodratio()>ip3d_iter->second){
										(hist_iter->second)->Fill(jet->pt()/GeV);
									}
								}
							}
						}	
					}
					if((hist_iter->first).find("IP2D_") < 1 && (hist_iter->first).find("_matched") < (hist_iter->first).length()){
						if( (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length() && label == label_iter->second && (hist_iter->first).find("_weight") < (hist_iter->first).length()){
							if((hist_iter->first).find("_trackCuts") < (hist_iter->first).length()){
								if(nIP2DTracks > 0)(hist_iter->second)->Fill(btag->IP2D_loglikelihoodratio());
							}
							else (hist_iter->second)->Fill(btag->IP2D_loglikelihoodratio());
						}
						else{
							 for(std::map<std::string, double>::const_iterator ip2d_iter = m_IP2D_workingPoints.begin(); ip2d_iter != m_IP2D_workingPoints.end(); ++ip2d_iter){
								if((hist_iter->first).find(ip2d_iter->first) < (hist_iter->first).length() && (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length()){
									if(label == label_iter->second && btag->IP2D_loglikelihoodratio()>ip2d_iter->second){
										(hist_iter->second)->Fill(jet->pt()/GeV);
									}
								}
							}
						}	
					}
				
					if((hist_iter->first).find("SV1_") < 1 && (hist_iter->first).find("matched") < (hist_iter->first).length()){
						if( (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length() && label == label_iter->second && (hist_iter->first).find("_weight") < (hist_iter->first).length()){
							if((hist_iter->first).find("_trackCuts") < (hist_iter->first).length()){
								if(nGTinSvx > 0) (hist_iter->second)->Fill(btag->SV1_loglikelihoodratio());
							}
							else (hist_iter->second)->Fill(btag->SV1_loglikelihoodratio());
						}
						else{
							for(std::map<std::string, double>::const_iterator sv1_iter = m_SV1_workingPoints.begin(); sv1_iter != 					          	m_SV1_workingPoints.end(); ++sv1_iter){
								if((hist_iter->first).find(sv1_iter->first) < (hist_iter->first).length() && (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length()){
									if(label == label_iter->second && btag->SV1_loglikelihoodratio()>sv1_iter->second){
										(hist_iter->second)->Fill(jet->pt()/GeV);
									}
								}
							}
						}
					}
					if((hist_iter->first).find("SV0_") < 1 && (hist_iter->first).find("matched") < (hist_iter->first).length()){
					  float normdist=0;
					  btag->taggerInfo(normdist, xAOD::SV0_normdist);	
					  if( (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length() && label == label_iter->second && (hist_iter->first).find("_weight") < (hist_iter->first).length()){
						if((hist_iter->first).find("_trackCuts") < (hist_iter->first).length()){
								if(nGTinSvx0 > 0) (hist_iter->second)->Fill(normdist);
						}
						else (hist_iter->second)->Fill(normdist);
					  }
					  else{
					    for(std::map<std::string, double>::const_iterator sv0_iter = m_SV0_workingPoints.begin(); sv0_iter != m_SV0_workingPoints.end(); ++sv0_iter){
							  if((hist_iter->first).find(sv0_iter->first) < (hist_iter->first).length() && (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length()){
							    if(label == label_iter->second && normdist>sv0_iter->second){
	 									  (hist_iter->second)->Fill(jet->pt()/GeV);
							    }
							  }
					    }
					  }	
					}
					if((hist_iter->first).find("IP3DSV1_") < 1 && (hist_iter->first).find("matched") < (hist_iter->first).length()){						
						if( (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length() && label == label_iter->second && (hist_iter->first).find("_weight") < (hist_iter->first).length()){
							if((hist_iter->first).find("_trackCuts") < (hist_iter->first).length()){
								if(nGTinSvx > 0 && nIP3DTracks > 0) 							(hist_iter->second)->Fill(btag->SV1plusIP3D_discriminant());
							}
							else (hist_iter->second)->Fill(btag->SV1plusIP3D_discriminant());
						}
						else{
							for(std::map<std::string, double>::const_iterator ip3dsv1_iter = m_IP3DSV1_workingPoints.begin(); ip3dsv1_iter != 					          m_IP3DSV1_workingPoints.end(); ++ip3dsv1_iter){
								if((hist_iter->first).find(ip3dsv1_iter->first) < (hist_iter->first).length() && (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length()){
									if(label == label_iter->second && btag->SV1plusIP3D_discriminant()>ip3dsv1_iter->second){
	 									  (hist_iter->second)->Fill(jet->pt()/GeV);
									}
								}
							}
						}	
					}
					if((hist_iter->first).find("JetFitter_") < 1 && (hist_iter->first).find("matched") < (hist_iter->first).length()){						
						if( (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length() && label == label_iter->second && (hist_iter->first).find("_weight") < (hist_iter->first).length()){
							if((hist_iter->first).find("_trackCuts") < (hist_iter->first).length()){
							  if(nGTinSvx > 0 && nIP3DTracks > 0) 							(hist_iter->second)->Fill(btag->JetFitter_loglikelihoodratio());
							}
							else{
						 		(hist_iter->second)->Fill(btag->JetFitter_loglikelihoodratio());
							}
						}
						else{
							for(std::map<std::string, double>::const_iterator ip3dsv1_iter = m_IP3DSV1_workingPoints.begin(); ip3dsv1_iter != 					          m_IP3DSV1_workingPoints.end(); ++ip3dsv1_iter){
								if((hist_iter->first).find(ip3dsv1_iter->first) < (hist_iter->first).length() && (hist_iter->first).find("_"+label_iter->first+"_") < (hist_iter->first).length()){
									if(label == label_iter->second && btag->SV1plusIP3D_discriminant()>ip3dsv1_iter->second){
	 									  (hist_iter->second)->Fill(jet->pt()/GeV);
									}
								}
							}
						}	
					}
					if((hist_iter->first).find("MV") < 1 && (hist_iter->first).find("matched") < (hist_iter->first).length()){
						for(std::vector<std::string>::const_iterator tag_iter = m_taggers.begin(); tag_iter != m_taggers.end(); 
						  ++tag_iter){						
							if( (hist_iter->first).find(*tag_iter+"_"+label_iter->first+"_") < (hist_iter->first).length() && label == label_iter->second && (hist_iter->first).find("_weight") < (hist_iter->first).length()){
								if((hist_iter->first).find("_trackCuts") < (hist_iter->first).length()){
									if(nGTinSvx > 0 && nIP3DTracks > 0){ 
										double weight=-999;
										btag->variable<double>(*tag_iter,"discriminant",weight);		
										(hist_iter->second)->Fill(weight);
									}
								}
								else{
									double weight=-999;
									btag->variable<double>(*tag_iter,"discriminant",weight);
									(hist_iter->second)->Fill(weight);
								}
							}	
						}
					}
                		}
			}
		}
	}  


 	void BTaggingValidationPlots::finalizePlots(){

		
		std::string tmp_name_matched = "";
		std::string tmp_name_eff = "";
		std::string tmp_name_rej = "";
		
		for(std::map<std::string, TH1*>::const_iterator hist_iter=m_weight_histos.begin(); hist_iter!=m_weight_histos.end(); ++hist_iter){

			if((hist_iter->first).find("matched")< (hist_iter->first).length() && (hist_iter->first).find("trackCuts") > (hist_iter->first).length()){
				std::size_t found_matched = (hist_iter->first).find("matched");		
				tmp_name_matched = (hist_iter->first);
				tmp_name_eff = tmp_name_matched.replace(found_matched, 7, "eff");
				std::map<std::string, TProfile*>::const_iterator eff_profile_iter = m_eff_profiles.find(tmp_name_eff);

				if(eff_profile_iter != m_eff_profiles.end() && (eff_profile_iter->first).find("_pt") < (eff_profile_iter->first).length()){
			 		makeEfficiencyVsPtPlot(hist_iter->second, eff_profile_iter->second);
					(eff_profile_iter->second)->SetOption("E");
				}
				else if(eff_profile_iter != m_eff_profiles.end() && (eff_profile_iter->first).find("_weight") < (eff_profile_iter->first).length()){
					makeEfficiencyPlot(hist_iter->second, eff_profile_iter->second);
					std::size_t found_weight = tmp_name_eff.find("weight");
					tmp_name_rej = tmp_name_eff.replace(found_weight, 6, "rej");
					makeEfficiencyRejectionPlot(eff_profile_iter->second, m_eff_profiles.find(tmp_name_rej)->second);  
				}
			}
		}
	}


	void BTaggingValidationPlots::makeEfficiencyVsPtPlot(TH1* hReco, TProfile* pEff){
		TH1* hTruth=NULL;
		std::string recoName = hReco->GetName();
		if(recoName.find("_b_") < recoName.length()) hTruth = (TH1 *) m_truthPt_b->Clone();
		else if(recoName.find("_c_") < recoName.length()) hTruth = (TH1 *) m_truthPt_c->Clone();
		else if(recoName.find("_tau_") < recoName.length()) hTruth = (TH1 *) m_truthPt_tau->Clone();
		else if(recoName.find("_u_") < recoName.length()) hTruth = (TH1 *) m_truthPt_u->Clone();
		if (!hTruth || hTruth->GetNbinsX() != hReco->GetNbinsX() || hTruth->Integral() == 0) return;
		else{
			for (int bin_i=1; bin_i<= hTruth->GetNbinsX(); ++bin_i){ 
				if(hTruth->GetBinContent(bin_i) == 0) continue;
				double eff = hReco->GetBinContent(bin_i)/hTruth->GetBinContent(bin_i);
				//double eff_err = sqrt(hReco->GetBinContent(bin_i)*(1-eff))/hTruth->GetBinContent(bin_i);		
				double pt = hTruth->GetBinCenter(bin_i);	
				pEff->Fill(pt,eff);
			}
		}
  	}

	void BTaggingValidationPlots::makeEfficiencyPlot(TH1* hReco, TProfile* pEff){
//		double Ntrue = hReco->Integral();		
		double Ntrue = hReco->Integral(0,hReco->GetNbinsX()+1);
		std::string recoName =  hReco->GetName();
		if(Ntrue == 0) return;
		for (int bin_i=0; bin_i<= hReco->GetNbinsX()+1; ++bin_i){ 
			double eff = hReco->Integral(bin_i, hReco->GetNbinsX())/Ntrue;
			//double eff_err = sqrt(hReco->GetBinContent(bin_i)*(1-eff))/Ntrue;
			double weight = hReco->GetBinCenter(bin_i);	
			pEff->Fill(weight,eff);
		}		
	}

	void BTaggingValidationPlots::makeEfficiencyRejectionPlot(TProfile* pLEff, TProfile* pEffRej){	
		TProfile* pBEff=NULL;
		//std::cout << "HIER NAME " << pEffRej->GetName() << std::endl;
		std::string bEffName = pEffRej->GetName();
		//std::cout << "name of rej vs eff histo " << pEffRej->GetName() << std::endl;
		for(std::vector<std::string>::const_iterator tag_iter = m_taggers.begin(); tag_iter != m_taggers.end(); 
			  ++tag_iter){
		  if(bEffName.find(*tag_iter+"_") < 1) pBEff = (TProfile *) (m_eff_profiles.find(*tag_iter+"_b_eff_weight")->second)->Clone();
			//std::cout << "name of used b-eff histo " << pBEff->GetName() << "\t name of used l-eff histo " << pLEff->GetName() << std::endl; }
		}
		if (!pBEff || pBEff->GetNbinsX() != pLEff->GetNbinsX()) return;
		else{
			for (int bin_i=1; bin_i<= pBEff->GetNbinsX(); ++bin_i){ 
				double eff = pBEff->GetBinContent(bin_i);
				double rej = 1/(pLEff->GetBinContent(bin_i));
				//std::cout << "bin no.: " << bin_i << "\t b-bin: " << pBEff->GetBinCenter(bin_i) << "\t l-bin: " << pLEff->GetBinCenter(bin_i) << std::endl; 
				//std::cout << "b-eff: " << eff << "\t l-eff: " << 1/rej << "\t rej: " << rej << std::endl;
				pEffRej->Fill(eff,rej);
			}		
		}
	}


	void BTaggingValidationPlots::setTaggerInfos(){
		m_taggers.push_back("IP3D");
		m_taggers.push_back("IP2D");
		m_taggers.push_back("SV1");
		//m_taggers.push_back("SV0");
		m_taggers.push_back("IP3DSV1");
		m_taggers.push_back("JetFitter");
		//m_taggers.push_back("JetFitterCombNN");
		//m_taggers.push_back("MV1");
		//m_taggers.push_back("MVb");	
		//m_taggers.push_back("MV1c");
		//m_taggers.push_back("MV2c00");
		m_taggers.push_back("MV2c10");
		//m_taggers.push_back("MV2c20");
		m_taggers.push_back("MV2c10mu");
		m_taggers.push_back("MV2c10rnn");
		//To be added later
		//m_taggers.push_back("DL1");
		//m_taggers.push_back("DL1mu");
		//m_taggers.push_back("DL1rnn");

		m_truthLabels.insert(std::make_pair("b", 5));
		m_truthLabels.insert(std::make_pair("c", 4));
		m_truthLabels.insert(std::make_pair("u", 0));
		m_truthLabels.insert(std::make_pair("tau", 15));
		//rel. 16 MC10 Dec. 10
		m_IP3D_workingPoints.insert(std::make_pair("50", 3.75));
		m_IP3D_workingPoints.insert(std::make_pair("70", 1.2));
		m_IP3D_workingPoints.insert(std::make_pair("80", -0.3));
		//rel. 16 MC10 Dec. 10
		m_IP2D_workingPoints.insert(std::make_pair("50", 2.9));
		m_IP2D_workingPoints.insert(std::make_pair("70", 0.7));
		m_IP2D_workingPoints.insert(std::make_pair("80", -0.5));
		//rel 16 Mc10 Dec 10
		m_SV1_workingPoints.insert(std::make_pair("40", 5.5));
		m_SV1_workingPoints.insert(std::make_pair("50", 3.9));
		m_SV1_workingPoints.insert(std::make_pair("60", 1.6));
		//rel 17 MC12a Jul 12
		m_SV0_workingPoints.insert(std::make_pair("40", 9.55));
		m_SV0_workingPoints.insert(std::make_pair("50", 5.7));
		//rel 17 Mc11b Dec. 11
		m_IP3DSV1_workingPoints.insert(std::make_pair("50", 7.6));
		m_IP3DSV1_workingPoints.insert(std::make_pair("70", 1.85));
		m_IP3DSV1_workingPoints.insert(std::make_pair("80", -0.7));	
		//self determined
		m_JetFitter_workingPoints.insert(std::make_pair("50", 0.9));
		m_JetFitter_workingPoints.insert(std::make_pair("70", -1.7));
		m_JetFitter_workingPoints.insert(std::make_pair("80", -3.3));
		//rel 17 Mc12a jul.12
		//m_JetFitterCombNN_workingPoints.insert(std::make_pair("50", 2.1));
		//m_JetFitterCombNN_workingPoints.insert(std::make_pair("70", -0.95));
		//m_JetFitterCombNN_workingPoints.insert(std::make_pair("80", -2.6));	
		//rel 17 Mc12a Nov. 12
		//m_MV1_workingPoints.insert(std::make_pair("50", 0.992515446));
		//m_MV1_workingPoints.insert(std::make_pair("70", 0.8119));
		//m_MV1_workingPoints.insert(std::make_pair("80", 0.39));	
		
	}
	
	void BTaggingValidationPlots::bookEffHistos(){
		setTaggerInfos();
		for(std::vector<std::string>::const_iterator tag_iter = m_taggers.begin(); tag_iter != m_taggers.end(); 
			  ++tag_iter){

			for(std::map<std::string, int>::const_iterator label_iter = m_truthLabels.begin(); label_iter != m_truthLabels.end(); 
			  ++label_iter){

				std::string name_effRej = *tag_iter+"_"+label_iter->first+"_eff_rej";
				TProfile* profile_effRej = PlotBase::BookTProfile(name_effRej, "rejection of "+label_iter->first+"-jets vs. b-efficiency; efficiency; rejection", 100, 0., 1, 0, 1000000., false);
				m_eff_profiles.insert(std::make_pair(name_effRej, profile_effRej));

				if((*tag_iter).find("MV") < 1){
					std::string name_matched = *tag_iter+"_"+label_iter->first+"_matched_weight";
					TH1* histo_matched = Book1D(name_matched, *tag_iter+" loglikelihoodratio of matched "+ m_sParticleType + " "+label_iter->first + "-jets; "+*tag_iter+"_loglikelihoodRatio;Events", 250, -1, 1.);	
					m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
				}
				else{
					std::string name_matched = *tag_iter+"_"+label_iter->first+"_matched_weight";
					TH1* histo_matched = Book1D(name_matched, *tag_iter+" loglikelihoodratio of matched "+ m_sParticleType + " "+label_iter->first + "-jets; "+*tag_iter+"_loglikelihoodRatio;Events", 250, -10, 40.);	
					m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
				}

				std::string name_trackCuts = *tag_iter+"_"+label_iter->first+"_matched_weight_trackCuts";
				TH1* histo_trackCuts = Book1D(name_trackCuts, *tag_iter+" loglikelihoodratio of matched "+ m_sParticleType + " " +label_iter->first + "-jets with nTrack cuts; "+*tag_iter+"_loglikelihoodRatio;Events", 250, -10, 40.);	
				m_weight_histos.insert(std::make_pair(name_trackCuts, histo_trackCuts));
				
				if((*tag_iter).find("MV") < 1){
					std::string name_eff = *tag_iter+"_"+label_iter->first+"_eff_weight";
					TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. "+*tag_iter+" loglikelihoodratio of "+ m_sParticleType + " " + label_iter->first + "-jets; "+*tag_iter+"_loglikelihoodRatio;efficiency", 250, -1, 1, 0., 1., false);	
					m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
				}
				else{
					std::string name_eff = *tag_iter+"_"+label_iter->first+"_eff_weight";
					TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. "+*tag_iter+" loglikelihoodratio of "+ m_sParticleType + " " + label_iter->first + "-jets; "+*tag_iter+"_loglikelihoodRatio;efficiency", 250, -10, 40., 0., 1., false);
					m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
				}

				if(*tag_iter == "IP3D"){
					for(std::map<std::string, double>::const_iterator ip3d_iter = m_IP3D_workingPoints.begin(); ip3d_iter != 					   	m_IP3D_workingPoints.end(); ++ip3d_iter){

						std::ostringstream str_tmp("");
						str_tmp << ip3d_iter->second;
						std::string name_matched = "IP3D_"+label_iter->first+"_"+ip3d_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched "+ m_sParticleType +" for IP3D_loglikelihoodRatio > "+ str_tmp.str() + " " + label_iter->first + "-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "IP3D_"+label_iter->first+"_"+ip3d_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for IP3D_loglikelihoodRatio > " + str_tmp.str()+ " " + label_iter->first + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000., 0, 1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				}
				if(*tag_iter == "IP2D"){
					for(std::map<std::string, double>::const_iterator ip2d_iter = m_IP2D_workingPoints.begin(); ip2d_iter != 					   	m_IP2D_workingPoints.end(); ++ip2d_iter){

						std::ostringstream str_tmp("");
						str_tmp << ip2d_iter->second;
						std::string name_matched = "IP2D_"+label_iter->first+"_"+ip2d_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched "+ m_sParticleType +" for IP2D_loglikelihoodRatio > "+ str_tmp.str() + " " + label_iter->first + "-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "IP2D_"+label_iter->first+"_"+ip2d_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for IP2D_loglikelihoodRatio > " + str_tmp.str()+ " " + label_iter->first + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000., 0, 1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				}
				if(*tag_iter == "SV1"){
					for(std::map<std::string, double>::const_iterator sv1_iter = m_SV1_workingPoints.begin(); sv1_iter != 					   	m_SV1_workingPoints.end(); ++sv1_iter){
						std::ostringstream str_tmp("");
						str_tmp << sv1_iter->second;
						std::string name_matched = "SV1_"+label_iter->first+"_"+sv1_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched " + m_sParticleType + " for SV1_loglikelihoodRatio > " + str_tmp.str()+ " " + label_iter->first +"-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "SV1_"+label_iter->first+"_"+sv1_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for SV1_loglikelihoodRatio > " + str_tmp.str()+label_iter->first + " " + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000., 0.,1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				}
				/*
				if(*tag_iter == "SV0"){
					for(std::map<std::string, double>::const_iterator sv0_iter = m_SV0_workingPoints.begin(); sv0_iter != 					   	m_SV0_workingPoints.end(); ++sv0_iter){
						std::ostringstream str_tmp("");
						str_tmp << sv0_iter->second;
						std::string name_matched = "SV0_"+label_iter->first+"_"+sv0_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched " + m_sParticleType + " for SV0_loglikelihoodRatio > " + str_tmp.str()+ " " + label_iter->first +"-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "SV0_"+label_iter->first+"_"+sv0_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for SV0_loglikelihoodRatio > " + str_tmp.str()+label_iter->first + " " + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000., 0.,1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				} // SV0
				*/
				if(*tag_iter == "IP3DSV1"){
					for(std::map<std::string, double>::const_iterator ip3dsv1_iter = m_IP3DSV1_workingPoints.begin(); ip3dsv1_iter != 						m_IP3DSV1_workingPoints.end(); ++ip3dsv1_iter){
						std::ostringstream str_tmp("");
						str_tmp << ip3dsv1_iter->second;
						std::string name_matched = "IP3DSV1_"+label_iter->first+"_"+ip3dsv1_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched " + m_sParticleType + " for IP3D+SV1_loglikelihoodRatio > " + str_tmp.str() + " " + label_iter->first + "-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "IP3DSV1_"+label_iter->first+"_"+ip3dsv1_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for IP3D+SV1_loglikelihoodRatio > " + str_tmp.str() +" " + label_iter->first + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000.,0.,1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				} //IP3DSV
				if(*tag_iter == "JetFitter"){
					for(std::map<std::string, double>::const_iterator jetfitter_iter = m_JetFitter_workingPoints.begin(); jetfitter_iter != m_JetFitter_workingPoints.end(); ++jetfitter_iter){
						std::ostringstream str_tmp("");
						str_tmp << jetfitter_iter->second;
						std::string name_matched = "JetFitter_"+label_iter->first+"_"+jetfitter_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched " + m_sParticleType + " for JetFitter_loglikelihoodRatio > " + str_tmp.str() + " " + label_iter->first + "-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "JetFitter_"+label_iter->first+"_"+jetfitter_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for JetFitter_loglikelihoodRatio > " + str_tmp.str() +" " + label_iter->first + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000.,0.,1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				} // JetFitter
				/*
				if(*tag_iter == "JetFitterCombNN"){
					for(std::map<std::string, double>::const_iterator jetfitter_iter = m_JetFitterCombNN_workingPoints.begin(); jetfitter_iter != m_JetFitterCombNN_workingPoints.end(); ++jetfitter_iter){
						std::ostringstream str_tmp("");
						str_tmp << jetfitter_iter->second;
						std::string name_matched = "JetFitterCombNN_"+label_iter->first+"_"+jetfitter_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched " + m_sParticleType + " for JetFitterCombNN_loglikelihoodRatio > " + str_tmp.str() + " " + label_iter->first + "-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "JetFitterCombNN_"+label_iter->first+"_"+jetfitter_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for JetFitterCombNN_loglikelihoodRatio > " + str_tmp.str() +" " + label_iter->first + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000.,0.,1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				} // JetFitterCombNN
				*/
				/*
				if(*tag_iter == "MV1"){
					for(std::map<std::string, double>::const_iterator mv1_iter = m_MV1_workingPoints.begin(); mv1_iter != m_MV1_workingPoints.end(); ++mv1_iter){
						std::ostringstream str_tmp("");
						str_tmp << mv1_iter->second;
						std::string name_matched = "MV1_"+label_iter->first+"_"+mv1_iter->first+"_matched_pt";
						TH1* histo_matched = Book1D(name_matched, "p_{T} of matched " + m_sParticleType + " for MV1_weight > " + str_tmp.str() + " " + label_iter->first + "-jets; p_{T} (GeV) ;Events", 100, 0., 1000.);
						m_weight_histos.insert(std::make_pair(name_matched, histo_matched));
						std::string name_eff = "MV1_"+label_iter->first+"_"+mv1_iter->first+"_eff_pt";
						TProfile* profile_eff = BookTProfile(name_eff, "efficiency vs. p_{T} for MV1_weight > " + str_tmp.str() +" " + label_iter->first + "-jets; p_{T} (GeV) ;efficiency", 100, 0., 1000.,0.,1., false);
						m_eff_profiles.insert(std::make_pair(name_eff, profile_eff));
					}
				} // MV1
				*/
			}
		}
	}
		
}
