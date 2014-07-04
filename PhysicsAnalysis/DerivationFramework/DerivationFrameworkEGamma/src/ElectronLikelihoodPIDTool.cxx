/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ElectronLikelihoodPIDTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkEGamma/ElectronLikelihoodPIDTool.h"
#include "PathResolver/PathResolver.h"
#include<unistd.h>
#include <vector>
#include <string>


namespace DerivationFramework {
    
    ElectronLikelihoodPIDTool::ElectronLikelihoodPIDTool(const std::string& t,
                                                         const std::string& n,
                                                         const IInterface* p) :
    AthAlgTool(t,n,p),
    m_sgName(""),
    m_pdfRootFile("ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root")
    {
        declareInterface<DerivationFramework::IAugmentationTool>(this);
        declareProperty("StoreGateEntryName",m_sgName);
	declareProperty("PDFRootFile",m_pdfRootFile);
    }
    
    StatusCode ElectronLikelihoodPIDTool::initialize()
    {
	// Did the user provide an output SG name?
	if (m_sgName=="") {
		ATH_MSG_ERROR("No output SG name provided!");
		return StatusCode::FAILURE;
	}
	// Find the full path to the input file
	std::string file = PathResolver::find_file(m_pdfRootFile,"XMLPATH");
	ATH_MSG_INFO("Complete path to PDF input file: " << file);
	// Test whether the input PDF file is accessible
	if (access(file.c_str(),0)==-1) {
		ATH_MSG_ERROR("Input PDF file does not exist!");
		return StatusCode::FAILURE;
	}
        m_lhTool = new Root::TElectronLikelihoodTool();
        m_lhTool->setPDFFileName(file); 
        m_lhTool->setOperatingPoint(LikeEnum::Loose);
        m_lhTool->initialize();
        return StatusCode::SUCCESS;
    }
    
    StatusCode ElectronLikelihoodPIDTool::finalize()
    {
        return StatusCode::SUCCESS;
    }
   
    StatusCode ElectronLikelihoodPIDTool::addBranches() const
    {
        std::vector<int> *passLH = new std::vector<int>();
        
        // D3PD branches
        std::vector<float>* el_etas2;
        std::vector<float>* el_cl_E;
        std::vector<float>* el_f3;
        std::vector<float>* el_Ethad;
        std::vector<float>* el_Ethad1;
        std::vector<float>* el_reta;
        std::vector<float>* el_weta2;
        std::vector<float>* el_f1;
        std::vector<float>* el_wstot;
        std::vector<float>* el_emaxs1;
        std::vector<float>* el_Emax2;
        std::vector<float>* el_deltaeta1;
        std::vector<float>* el_trackd0pvunbiased;
        std::vector<float>* el_TRTHighTOutliersRatio;
        std::vector<float>* el_tracksigd0pvunbiased;
        std::vector<float>* el_rphi;
        std::vector<float>* el_deltaphiRescaled;
        std::vector<int>* el_nSiHits;
        std::vector<int>* el_nPixelDeadSensors;
        std::vector<int>* el_nSCTDeadSensors;
        std::vector<int>* el_nPixHits;
        std::vector<int>* el_nBLHits;
        std::vector<int>* el_nBLayerOutliers;
        std::vector<float>* el_expectHitInBLayer;
        std::vector<unsigned int>* el_isEM;
        std::vector<std::vector<float> >* el_refittedTrack_LMqoverp;
        std::vector<std::vector<int> >* el_refittedTrack_author;
        std::vector<float>* el_trackqoverp;
        
        int* vxp_n;
        std::vector<int>* vxp_trk_n;
        
        // Retrieve data
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_etas2,"el_etas2"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_cl_E,"el_cl_E"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_f3,"el_f3"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_Ethad,"el_Ethad"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_Ethad1,"el_Ethad1"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_reta,"el_reta"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_weta2,"el_weta2"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_f1,"el_f1"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_wstot,"el_wstot"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_emaxs1,"el_emaxs1"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_Emax2,"el_Emax2"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_deltaeta1,"el_deltaeta1"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_trackd0pvunbiased,"el_trackd0pvunbiased"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_TRTHighTOutliersRatio,"el_TRTHighTOutliersRatio"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_tracksigd0pvunbiased,"el_tracksigd0pvunbiased"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_rphi,"el_rphi"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_deltaphiRescaled,"el_deltaphiRescaled"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nSiHits,"el_nSiHits"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nPixelDeadSensors,"el_nPixelDeadSensors"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nSCTDeadSensors,"el_nSCTDeadSensors"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nPixHits,"el_nPixHits"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nBLHits,"el_nBLHits"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nBLayerOutliers,"el_nBLayerOutliers"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_expectHitInBLayer,"el_expectHitInBLayer"));
        CHECK(evtStore()->retrieve((const std::vector<unsigned int>*&)el_isEM,"el_isEM"));
        CHECK(evtStore()->retrieve((const std::vector<std::vector<float> >*&)el_refittedTrack_LMqoverp,"el_refittedTrack_LMqoverp"));
        CHECK(evtStore()->retrieve((const std::vector<std::vector<int> >*&)el_refittedTrack_author,"el_refittedTrack_author"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_trackqoverp,"el_trackqoverp"));
        
        CHECK(evtStore()->retrieve((const int*&)vxp_n,"vxp_n"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)vxp_trk_n,"vxp_trk_n"));
        
        // primary vertices with more than 2 tracks (not dependent on electrons)
        float ip(0.0);
        for (int pvItr=0; pvItr < *vxp_n; ++pvItr) {
            if ( (*vxp_trk_n)[pvItr] >= 2 ) ip += 1.0;
        }
        
        // LOOP OVER ELECTRONS
        unsigned int nElectrons = el_etas2->size();
        for (unsigned int eItr=0; eItr<nElectrons; ++eItr) {
            
            // Prepare variables
            float eta = (*el_etas2)[eItr];
            float eT = (*el_cl_E)[eItr]/cosh( (*el_etas2)[eItr]);
            float f3 = (*el_f3)[eItr];
            float rHad = (*el_Ethad)[eItr]/eT;
            float rHad1 = (*el_Ethad1)[eItr]/eT;
            float Reta = (*el_reta)[eItr];
            float w2 = (*el_weta2)[eItr];
            float f1 = (*el_f1)[eItr];
            //float wstot = (*el_wstot)[eItr];
            float eratio = ( (*el_emaxs1)[eItr]+(*el_Emax2)[eItr] == 0.0 ) ? 0.0 : ( (*el_emaxs1)[eItr]-(*el_Emax2)[eItr] )/( (*el_emaxs1)[eItr]+(*el_Emax2)[eItr] );
            float deltaEta = (*el_deltaeta1)[eItr];
            float d0 = (*el_trackd0pvunbiased)[eItr];
            float TRratio = (*el_TRTHighTOutliersRatio)[eItr];
            float d0sigma = (*el_tracksigd0pvunbiased)[eItr];
            float rphi = (*el_rphi)[eItr];
            float deltaPoverP(0.0);
            for (unsigned int i = 0; i<(*el_refittedTrack_LMqoverp)[eItr].size();++i) {
                if(((*el_refittedTrack_author)[eItr]).at(i)== 4) {
                    deltaPoverP= 1.-( (*el_trackqoverp)[eItr]/((*el_refittedTrack_LMqoverp)[eItr].at(i)));
                }
            }
            float deltaphires = (*el_deltaphiRescaled)[eItr];
            int nSi = (*el_nSiHits)[eItr];
            int nSiDeadSensors = (*el_nPixelDeadSensors)[eItr]+(*el_nSCTDeadSensors)[eItr];
            int nPix = (*el_nPixHits)[eItr];
            int nPixDeadSensors = (*el_nPixelDeadSensors)[eItr];
            int nBlayer = (*el_nBLHits)[eItr];
            int nBlayerOutliers = (*el_nBLayerOutliers)[eItr];
            bool expectBlayer(false);
            if ( (*el_expectHitInBLayer)[eItr]>-999.5 && (*el_expectHitInBLayer)[eItr]<-998.5 ) expectBlayer=true;
            if ( (*el_expectHitInBLayer)[eItr] > -0.5 && (*el_expectHitInBLayer)[eItr] < 0.5 ) expectBlayer=false;
            if ( (*el_expectHitInBLayer)[eItr] > 0.5 && (*el_expectHitInBLayer)[eItr] < 1.5 ) expectBlayer=true;
            int convBit = (*el_isEM)[eItr] & (0x1 << 1);
	    LLHInputs inputs; 
	    inputs.eta = (double)eta;
	    inputs.eT = (double)eT;
	    inputs.f3 = (double)f3;
	    inputs.rHad = (double)rHad;
	    inputs.rHad1 = (double)rHad1;
            inputs.Reta = (double)Reta;
            inputs.w2 = (double)w2;
	    inputs.f1 = (double)f1;
	    inputs.eratio = (double)eratio;
	    inputs.deltaEta = (double)deltaEta;
	    inputs.d0 = (double)d0;
	    inputs.TRratio = (double)TRratio;
            inputs.d0sigma = (double)d0sigma;
            inputs.rphi = (double)rphi;
	    inputs.deltaPoverP = (double)deltaPoverP;
	    inputs.deltaphires = (double)deltaphires;
            inputs.ip = (double)ip;
            inputs.nSi = (int)nSi;
	    inputs.nSiDeadSensors = (int)nSiDeadSensors;
            inputs.nPix = (int)nPix;
	    inputs.nPixDeadSensors = (int)nPixDeadSensors;
            inputs.nBlayer = (int)nBlayer;
            inputs.nBlayerOutliers = (int)nBlayerOutliers;
            inputs.expectBlayer = (bool)expectBlayer,
            inputs.convBit = (int)convBit;	
	    m_lhTool->setOperatingPoint(LikeEnum::VeryLoose);
	    bool veryloose = runLLHTool(inputs);
	    m_lhTool->setOperatingPoint(LikeEnum::Loose);
	    bool loose = runLLHTool(inputs);
	    m_lhTool->setOperatingPoint(LikeEnum::Medium);
	    bool medium = runLLHTool(inputs);
	    m_lhTool->setOperatingPoint(LikeEnum::Tight);
	    bool tight = runLLHTool(inputs);
	    m_lhTool->setOperatingPoint(LikeEnum::VeryTight); 
            bool verytight = runLLHTool(inputs);
 	    bool ePassesLH = veryloose || loose || medium || tight || verytight;
            if (ePassesLH) passLH->push_back(1);
            if (!ePassesLH) passLH->push_back(0);
            
        } // end of loop over electrons
        
        // Write masses to SG for access by downstream algs
        if (evtStore()->contains<std::vector<int> >(m_sgName)) {
            ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
            return StatusCode::FAILURE;
        }
        CHECK(evtStore()->record(passLH, m_sgName));
        return StatusCode::SUCCESS;
        
    }

    bool ElectronLikelihoodPIDTool::runLLHTool(	LLHInputs in) const {
	double discriminant = m_lhTool->calculate(	in.eta,in.eT,in.f3,in.rHad,in.rHad1,in.Reta,in.w2,in.f1,in.eratio,in.deltaEta,in.d0,in.TRratio,
							in.d0sigma,in.rphi,in.deltaPoverP,in.deltaphires,in.ip);
	bool ePassesLH = (bool)m_lhTool->accept(	discriminant,in.eta,in.eT,in.nSi,in.nSiDeadSensors,in.nPix,in.nPixDeadSensors,in.nBlayer,in.nBlayerOutliers,
							in.expectBlayer,in.convBit,in.ip);
 	return ePassesLH;
    }
    
} // end of namespace

