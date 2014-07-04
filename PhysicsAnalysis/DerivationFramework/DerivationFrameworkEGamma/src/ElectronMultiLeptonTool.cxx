/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ElectronMultiLeptonTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkEGamma/ElectronMultiLeptonTool.h"
#include "PathResolver/PathResolver.h"
#include<unistd.h>
#include <vector>
#include <string>


namespace DerivationFramework {
    
    ElectronMultiLeptonTool::ElectronMultiLeptonTool(const std::string& t,
                                                         const std::string& n,
                                                         const IInterface* p) :
    AthAlgTool(t,n,p),
    m_sgName("")
    {
        declareInterface<DerivationFramework::IAugmentationTool>(this);
        declareProperty("StoreGateEntryName",m_sgName);
    }
    
    StatusCode ElectronMultiLeptonTool::initialize()
    {
	// Did the user provide an output SG name?
	if (m_sgName=="") {
		ATH_MSG_ERROR("No output SG name provided!");
		return StatusCode::FAILURE;
	}
        m_mlTool = new Root::TElectronMultiLeptonSelector();
        m_mlTool->initialize();
        return StatusCode::SUCCESS;
    }
    
    StatusCode ElectronMultiLeptonTool::finalize()
    {
        return StatusCode::SUCCESS;
    }
    
    StatusCode ElectronMultiLeptonTool::addBranches() const
    {
        std::vector<int> *passML = new std::vector<int>();
        
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
        std::vector<float>* el_TRTHighTOutliersRatio;
        std::vector<float>* el_deltaphiRescaled;
        std::vector<int>* el_nSiHits;
        std::vector<int>* el_nPixelDeadSensors;
        std::vector<int>* el_nSCTDeadSensors;
        std::vector<int>* el_nPixHits;
        std::vector<int>* el_nBLHits;
        std::vector<float>* el_expectHitInBLayer;
        std::vector<int>* el_nTRTHits;
	std::vector<int>* el_nTRTOutliers;
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
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_TRTHighTOutliersRatio,"el_TRTHighTOutliersRatio"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_deltaphiRescaled,"el_deltaphiRescaled"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nSiHits,"el_nSiHits"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nPixelDeadSensors,"el_nPixelDeadSensors"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nSCTDeadSensors,"el_nSCTDeadSensors"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nPixHits,"el_nPixHits"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nBLHits,"el_nBLHits"));
        CHECK(evtStore()->retrieve((const std::vector<float>*&)el_expectHitInBLayer,"el_expectHitInBLayer"));
        CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nTRTHits,"el_nTRTHits"));
	CHECK(evtStore()->retrieve((const std::vector<int>*&)el_nTRTOutliers,"el_nTRTOutliers"));         
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
            float wstot = (*el_wstot)[eItr];
            float eratio = ( (*el_emaxs1)[eItr]+(*el_Emax2)[eItr] == 0.0 ) ? 0.0 : ( (*el_emaxs1)[eItr]-(*el_Emax2)[eItr] )/( (*el_emaxs1)[eItr]+(*el_Emax2)[eItr] );
            float deltaEta = (*el_deltaeta1)[eItr];
            float TRratio = (*el_TRTHighTOutliersRatio)[eItr];
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
            bool expectBlayer(false);
            if ( (*el_expectHitInBLayer)[eItr]>-999.5 && (*el_expectHitInBLayer)[eItr]<-998.5 ) expectBlayer=true;
            if ( (*el_expectHitInBLayer)[eItr] > -0.5 && (*el_expectHitInBLayer)[eItr] < 0.5 ) expectBlayer=false;
            if ( (*el_expectHitInBLayer)[eItr] > 0.5 && (*el_expectHitInBLayer)[eItr] < 1.5 ) expectBlayer=true;
	    int nTRTTotal = (*el_nTRTHits)[eItr] + (*el_nTRTOutliers)[eItr];
            bool ePassesML = (bool)m_mlTool->accept( 	(double)eta, (double)eT, (double)rHad, (double)rHad1, (double)Reta,
						     	(double)w2, (double)f1, (double)f3, (double)wstot, (double)eratio,
							(double)deltaEta, (int)nSi, (int)nSiDeadSensors, (int)nPix, 
							(int)nPixDeadSensors, (double)deltaphires, (double)deltaPoverP, 
							(double)TRratio, (int)nTRTTotal, (int)nBlayer, (bool)expectBlayer, false);
            
            if (ePassesML) passML->push_back(1);
            if (!ePassesML) passML->push_back(0);
            
        } // end of loop over electrons
        
        // Write masses to SG for access by downstream algs
        if (evtStore()->contains<std::vector<int> >(m_sgName)) {
            ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
            return StatusCode::FAILURE;
        }
        CHECK(evtStore()->record(passML, m_sgName));
        return StatusCode::SUCCESS;
        
    }
    
} // end of namespace

