///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METSystematicsTool.h
// Header file for class METSystematicsTool
// Author: Russell Smith <rsmith@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef METUTILITIES_METSYSTEMATICSTOOL_H
#define METUTILITIES_METSYSTEMATICSTOOL_H

class TH3D;
class TH2D;
class TH1D;

#include "METInterface/IMETSystematicsTool.h"
#include "AsgTools/AsgTool.h"

#include "PATInterfaces/SystematicsTool.h"

#include "TRandom3.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"

#include "xAODJet/Jet.h"

namespace met {

  namespace softCaloAffSyst {
    const static CP::SystematicVariation MET_SoftCalo_ScaleUp  ("MET_SoftCalo_ScaleUp"  );
    const static CP::SystematicVariation MET_SoftCalo_ScaleDown("MET_SoftCalo_ScaleDown");
    const static CP::SystematicVariation MET_SoftCalo_Reso     ("MET_SoftCalo_Reso");
  }

  namespace softTrkAffSyst {
    const static CP::SystematicVariation MET_SoftTrk_ScaleUp  ("MET_SoftTrk_ScaleUp"  );
    const static CP::SystematicVariation MET_SoftTrk_ScaleDown("MET_SoftTrk_ScaleDown");
    const static CP::SystematicVariation MET_SoftTrk_ResoPara ("MET_SoftTrk_ResoPara" );
    const static CP::SystematicVariation MET_SoftTrk_ResoPerp ("MET_SoftTrk_ResoPerp" );
    const static CP::SystematicVariation MET_SoftTrk_ResoCorr ("MET_SoftTrk_ResoCorr" );
  }

  namespace jetTrkAffSyst {
    const static CP::SystematicVariation MET_JetTrk_ScaleUp   ("MET_JetTrk_ScaleUp"  );
    const static CP::SystematicVariation MET_JetTrk_ScaleDown ("MET_JetTrk_ScaleDown");
  }

  enum SystType {
    SOFTCALO,
    SOFTTRK,
    JETTRK
  };

  //this is to speed up applyCorrection lookup.
  //we still store the full list in appliedSystematics()/m_appliedSystematics
  enum SystApplied {      
    NONE              ,
    MET_SOFTTRK_SCALEUP   , 
    MET_SOFTTRK_SCALEDOWN ,
    MET_SOFTTRK_RESOPARA  ,
    MET_SOFTTRK_RESOPERP  ,
    MET_SOFTTRK_RESOCORR  ,
    MET_SOFTCALO_SCALEUP  , 
    MET_SOFTCALO_SCALEDOWN,
    MET_SOFTCALO_RESO     ,
    MET_JETTRK_SCALEUP    ,
    MET_JETTRK_SCALEDOWN  
  };

  class METSystematicsTool : public virtual IMETSystematicsTool, 
			     public virtual CP::SystematicsTool,
			     public virtual asg::AsgTool 
  {
    // This macro defines the constructor with the interface declaration                                                                              
    ASG_TOOL_CLASS(METSystematicsTool, IMETSystematicsTool)

    public:

    xAOD::EventInfo const * getDefaultEventInfo() const;

    //Constructor
    METSystematicsTool(const std::string& name);

    //Destructor 
    virtual ~METSystematicsTool(){}


    StatusCode softTrkSystInitialize(); //initialize softTrk scale/reo histos from config file
    StatusCode softCaloSystInitialize();//initialize softCalo scale/reso histos from config file
    StatusCode jetTrkSystInitialize();  //initialize jetTrk scale/reso histos from config file

    //required  asg tool interface functions
    StatusCode  initialize();
    StatusCode  finalize();

    //required correction tool functions
    //we don't inherit from CorrectionTool directly, since we don't want to implement applyContainerCorrection
    CP::CorrectionCode applyCorrection(xAOD::MissingET& inputMet, xAOD::MissingETAssociationMap * map = nullptr ) const;
    CP::CorrectionCode correctedCopy(const xAOD::MissingET& met, xAOD::MissingET*& outputmet,xAOD::MissingETAssociationMap * map = nullptr) const;
    //We don't want these for MET since we only apply systematics to the soft term, and this may be unclear
    //virtual CP::CorrectionCode applyContainerCorrection(xAOD::MissingETContainer& inputs, const CP::xAOD::EventInfo& eInfo) const;
    //virtual CP::CorrectionCode applyContainerCorrection(xAOD::MissingETContainer& inputs, const CP::xAOD::EventInfo& eInfo) const;


    //required systematic tool functions
    bool               isAffectedBySystematic  (const CP::SystematicVariation& var) const{return CP::SystematicsTool::isAffectedBySystematic(var)   ;}
    CP::SystematicSet  affectingSystematics    () const{		       		  return CP::SystematicsTool::affectingSystematics  ()      ;}
    CP::SystematicSet  recommendedSystematics  () const{		       		  return CP::SystematicsTool::recommendedSystematics()      ;}
    CP::SystematicCode applySystematicVariation(const CP::SystematicSet& set){		  return CP::SystematicsTool::applySystematicVariation(set) ;}
    CP::SystematicCode sysApplySystematicVariation(const CP::SystematicSet&); //when inheriting from SystematicsTool, we should only have to implement this one

    void setRandomSeed(int seed);

  private:
    SystApplied m_appliedSystEnum;

    CP::CorrectionCode internalSoftTermApplyCorrection(xAOD::MissingET& softMet, xAOD::MissingETContainer const * METcont , xAOD::EventInfo const& eInfo
) const;
    CP::CorrectionCode calcJetTrackMETWithSyst(xAOD::MissingET& jettrkmet, const xAOD::MissingETAssociationMap* map, const xAOD::Jet* jet) const;
    CP::CorrectionCode getCorrectedJetTrackMET(xAOD::MissingET& jettrkmet, const xAOD::MissingETAssociationMap* map, const xAOD::JetContainer* vecJets) const;



    //declared properties 
    std::string m_jetColl;
    std::string m_configPrefix;
    std::string m_configSoftTrkFile;
    std::string m_configJetTrkFile;
    std::string m_configSoftCaloFile;
    std::string m_truthCont; 
    std::string m_truthObj;
    std::string m_vertexCont;
    std::string m_eventInfo;
    int         m_randSeed ;

    TH3D* shiftpara_pthard_njet_mu;
    TH3D* resopara_pthard_njet_mu;
    TH3D* resoperp_pthard_njet_mu;
    TH2D* jet_systRpt_pt_eta;
    TH1D* h_calosyst_scale;
    TH1D* h_calosyst_reso;

    mutable TRandom3 m_rand;//so that we can call this from applyCorrection

    int getNPV() const;

    StatusCode addMETAffectingSystematics();
    StatusCode extractHistoPath(std::string & histfile, std::string & systpath, std::string & configdir, std::string & suffix, SystType const & type);
    
    xAOD::MissingET calcPtHard(xAOD::MissingETContainer const * const cont) const;
    xAOD::MissingET caloSyst_scale(xAOD::MissingET const &softTerms , double const scale) const;
    xAOD::MissingET caloSyst_reso (xAOD::MissingET const &softTerms) const;
    xAOD::MissingET softTrkSyst_scale (xAOD::MissingET const & softTerms, xAOD::MissingET const & ptHard, double const shift) const;
    xAOD::MissingET softTrkSyst_reso  (xAOD::MissingET const & softTerms, xAOD::MissingET const & ptHard, double const shift, double const smearpara,
				       double const smearperp) const;
    xAOD::MissingET projectST     (xAOD::MissingET const & softTerms, xAOD::MissingET const & ptHard) const;

  };//METSystematicsTool

}

#endif //METUTILIES_METSYSTEMATICSTOOL_H

//  LocalWords:  METSystematicsTool
