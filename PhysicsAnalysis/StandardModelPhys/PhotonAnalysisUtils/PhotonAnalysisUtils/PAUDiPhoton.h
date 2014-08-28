//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHOTONANALYSISUTILS_PAUDIPHOTON_H
#define PHOTONANALYSISUTILS_PAUDIPHOTON_H

#include "Navigation/IAthenaBarCode.h"
#include "PhotonAnalysisUtils/PAUegamma.h"

namespace PAU {

  //-------------------------------------------------------------------------------------
  // class PAU::egamma: a class to treat photons and electrons at the same level
  class DiPhoton : public IAthenaBarCode  {

  private:
    std::vector<PAU::egamma*>     m_photons;
    std::vector<int> m_index;


  public:
    DiPhoton() : IAthenaBarCode() {};
      
    DiPhoton(PAU::egamma* g1, PAU::egamma* g2) : IAthenaBarCode() {
      setPhotons(g1,g2);
    };
	
    DiPhoton(std::vector<PAU::egamma*> photons) : IAthenaBarCode() {
      setPhotons(photons);
    };

    DiPhoton(std::vector<PAU::egamma*>* photons) : IAthenaBarCode() {
      setPhotons(*photons);
    };

    DiPhoton(std::vector<PAU::egamma> photons) : IAthenaBarCode() {
      setPhotons(photons);
    };

    DiPhoton(std::vector<PAU::egamma>* photons) : IAthenaBarCode() {
      setPhotons(*photons);
    };

    inline void clearPhotons() {
      for(std::vector<PAU::egamma*>::iterator g_iter = m_photons.begin(); g_iter != m_photons.end(); ++g_iter){
	delete *g_iter;
	m_photons.erase(g_iter);
      }
      m_photons.clear();
    }

    inline void clearIndex() {
      m_index.clear();
    }

    inline void setIndex(int idx1, int idx2) {
      clearIndex();
      m_index.push_back(idx1);
      m_index.push_back(idx2);      
    }

    inline const std::vector<int>* getIndex() {
      return &m_index;
    }

    inline const std::vector<PAU::egamma*>* getPhotons() const {
      return &m_photons;
    }

    inline bool                      setPhotons(PAU::egamma* g1, PAU::egamma* g2){
      clearPhotons();
      if(!g1 || !g2){
	return false;
      }

      if(g1->pt() < g2->pt()){
	m_photons.push_back(g2);
	m_photons.push_back(g1);
      } else {
	m_photons.push_back(g1);
	m_photons.push_back(g2);
      }
      return true;
    }
    inline bool                      setPhotons(PAU::egamma g1, PAU::egamma g2){
      return setPhotons(&g1, &g2);
    }
    inline bool                      setPhotons(std::vector<PAU::egamma*> photons){
      if(photons.size() != 2){
	return false;
      }
      return setPhotons(photons[0], photons[1]);
    }
    inline bool                      setPhotons(std::vector<PAU::egamma> photons){
      if(photons.size() != 2){
	return false;
      }
      return setPhotons(&(photons[0]), &(photons[1]));
    }
    

  public:
    // Some properties of the diphoton system that we want to record

    std::vector<float>                 etaLead               ;
    std::vector<float>                 etaSublead            ;
    std::vector<float>                 z                     ;
    std::vector<float>                 zres                  ;
    std::vector<float>                 vtxLikelihood         ;

    std::vector<float>   invmass               ;
    std::vector<float>   invmassTrueDir        ;
    std::vector<float>   invmassTrueE          ;
    std::vector<int>     invmass_flag          ;
    std::vector<float>   ptgg                  ;
    std::vector<float>   detagg                ;
    std::vector<float>   cosTh                 ;

    //float                dphigg              ;
    //float                cosThCS             ;

    
    int                  onejet_cutStatus      ;
    int                  onejet_ok             ;
    float                onejet_mgg            ;
    float                onejet_mggj           ;

    int                  twojet_cutStatus      ;
    int                  twojet_ok             ;
    float                twojet_mgg            ;
    float                twojet_mjj            ;
    

    float hggZWjj_SumTracksPT   ;
    float hggZWjj_mjjZW_tracks  ;
    float hggZWjj_mjjZW         ;
    float hggZWjj_ZW7Njet       ;
    float hggZWjj_PTjjZW        ;
    float hggZWjj_PTjjZW_tracks ;
    float hggZWjj_hggZWjj_ok    ;
    float hggZWjj_hggZWdeltaRjj ;
    std::vector<float> hggZWjj_PTjZW         ;
    std::vector<float> hggZWjj_PTjZWTracks   ;
    std::vector<float> hggZWjj_EtajZW        ;

    float hggZWjj_SumTracksPT4  ;
    float hggZWjj_mjjZW4_tracks ;
    float hggZWjj_mjjZW4        ;
    float hggZWjj_ZW4Njet       ;
    float hggZWjj_PTjjZW4       ;
    float hggZWjj_PTjjZW4_tracks;
    float hggZWjj_DeltaRphotons ;
    std::vector<float> hggZWjj_PTjZW4        ;
    std::vector<float> hggZWjj_PTjZW4Tracks  ;
    std::vector<float> hggZWjj_EtajZW4       ;
    
    float hggZWjj_hggetmiss_ok;
    float hggZWjj_hgglepton_ok;
    float hggZWjj_DeltaRmujet ;

    std::vector<float> hggZWjj_LeptonJets_pt;
    float hggZWjj_LeptonJetsSumpt;

    std::vector<int> hgg_indexGoodMuon;
    std::vector<int> hgg_indexGoodElectron;

    const Trk::VxCandidate* hgg_HPV_vertex;
    float hgg_HPV_zcommon;
    float hgg_HPV_sigma_zcommon;
    float hgg_HPV_zcommon_primVtxLH;
    float hgg_HPV_zcommon_primVtxNN;
    float hgg_HPV_vtxLikelihood;
    float hgg_HPV_etaLead_zcommon;
    float hgg_HPV_etaSublead_zcommon;
    float hgg_HPV_invmass;
    float hgg_HPV_etaLead_zcommon_primVtxLH;
    float hgg_HPV_etaSublead_zcommon_primVtxLH;
    float hgg_HPV_invmass_primVtxLH;
    float hgg_HPV_invmass_Ztruth;
    float hgg_HPV_ptgg    ;
    float hgg_HPV_detagg  ;
    float hgg_HPV_dphigg  ;
    float hgg_HPV_cosTh   ;
    float hgg_HPV_cosThCS ;

  } ;

}

#endif // PHOTONANALYSISUTILS_PAUDIPHOTON_H
