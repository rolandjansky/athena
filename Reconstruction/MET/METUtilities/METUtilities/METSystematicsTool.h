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
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AsgTool.h"

#include "PATInterfaces/SystematicsTool.h"

#include "TRandom3.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAssociationHelper.h"

#include <gtest/gtest_prod.h>

namespace met {

  typedef ElementLink<xAOD::IParticleContainer> obj_link_t;

  //we create this to avoid calling the xAOD::MissingET constructor
  //since it creates a private store which is slow
  //please forgive the code duplication :)
  struct missingEt {
    missingEt() : mpx(0), mpy(0), sumet(0), name(""), source(MissingETBase::Source::UnknownType){}
    missingEt(double mpx_in, double mpy_in, double sumet_in) :
      mpx(mpx_in),
      mpy(mpy_in),
      sumet(sumet_in),
      name(""),
      source(MissingETBase::Source::UnknownType)
    {}
    missingEt(double mpx_in, double mpy_in, double sumet_in, std::string const & iname, MissingETBase::Types::bitmask_t const & isource ) :
      mpx(mpx_in),
      mpy(mpy_in),
      sumet(sumet_in),
      name(iname),
      source(isource)
    {}

    double mpx;
    double mpy;
    double sumet;
    std::string name;
    MissingETBase::Types::bitmask_t  source;
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
			     public asg::AsgTool,
			     public CP::SystematicsTool
  {
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METSystematicsTool, IMETSystematicsTool)

    // FRIEND_TEST( METSystTest, TestGetEventInfo );
    FRIEND_TEST( METSystTest, TestDefaultHistosFilled );
    FRIEND_TEST( METSystTest, TestJetTrkHistosFilled );
    FRIEND_TEST( METSystTest, TestAddAffectingSystematic );
    FRIEND_TEST( METSystTest, TestProjectST );
    FRIEND_TEST( METSystTest, TestProjectST2 );

    public:

    //Constructor
    METSystematicsTool(const std::string& name);

    //Destructor
    // virtual ~METSystematicsTool();

    StatusCode softTrkSystInitialize(); //initialize softTrk scale/reo histos from config file
    StatusCode softCaloSystInitialize();//initialize softCalo scale/reso histos from config file
    StatusCode jetTrkSystInitialize();  //initialize jetTrk scale/reso histos from config file

    //required  asg tool interface functions
    StatusCode  initialize();
    StatusCode  finalize();

    //required correction tool functions
    //we don't inherit from CorrectionTool directly, since we don't want to implement applyContainerCorrection
    CP::CorrectionCode applyCorrection(xAOD::MissingET& inputMet,
				       const xAOD::MissingETAssociationHelper * helper = nullptr ) const;
    CP::CorrectionCode correctedCopy(const xAOD::MissingET& met, xAOD::MissingET*& outputmet,
				     const xAOD::MissingETAssociationHelper * helper = nullptr) const;
    //We don't want these for MET since we only apply systematics to the soft term, and this may be unclear
    //virtual CP::CorrectionCode applyContainerCorrection(xAOD::MissingETContainer& inputs, const CP::xAOD::EventInfo& eInfo) const;
    //virtual CP::CorrectionCode applyContainerCorrection(xAOD::MissingETContainer& inputs, const CP::xAOD::EventInfo& eInfo) const;

    //required ISystematicTool functions
    bool               isAffectedBySystematic  (const CP::SystematicVariation& var) const{return CP::SystematicsTool::isAffectedBySystematic(var)   ;}
    CP::SystematicSet  affectingSystematics    () const{		       		  return CP::SystematicsTool::affectingSystematics  ()      ;}
    CP::SystematicSet  recommendedSystematics  () const{		       		  return CP::SystematicsTool::recommendedSystematics()      ;}
    CP::SystematicCode applySystematicVariation(const CP::SystematicSet& set){		  return CP::SystematicsTool::applySystematicVariation(set) ;}
    CP::SystematicCode sysApplySystematicVariation(const CP::SystematicSet&); //when inheriting from SystematicsTool, we should only have to implement this one

    void setRandomSeed(int seed) const;

  private:

    //default constructor
    METSystematicsTool();

    //this saves the the currently applied systematic as an enum for faster lookup
    SystApplied m_appliedSystEnum;

    //these are the internal computation functions
    CP::CorrectionCode internalSoftTermApplyCorrection(xAOD::MissingET& softMet,
						       xAOD::MissingETContainer const * METcont,
						       xAOD::EventInfo          const & eInfo
						       ) const;
    CP::CorrectionCode calcJetTrackMETWithSyst(xAOD::MissingET& jettrkmet, const xAOD::MissingETAssociationHelper* helper) const;
    CP::CorrectionCode calcJetTrackMETWithSyst(xAOD::MissingET& jettrkmet, const xAOD::MissingETAssociationHelper* helper, const xAOD::Jet* jet) const;
    CP::CorrectionCode getCorrectedJetTrackMET(xAOD::MissingET& jettrkmet, const xAOD::MissingETAssociationHelper* helper) const;

    //declared properties
    std::string m_configPrefix;
    std::string m_configSoftTrkFile;
    std::string m_configJetTrkFile;
    std::string m_configSoftCaloFile;
    std::string m_truthCont;
    std::string m_truthObj;
    std::string m_vertexCont;
    std::string m_eventInfo;
    int         m_randSeed ;
    bool        m_useDevArea;

    TH3D* m_shiftpara_pthard_njet_mu;
    TH3D* m_resopara_pthard_njet_mu;
    TH3D* m_resoperp_pthard_njet_mu;
    TH2D* m_jet_systRpt_pt_eta;
    TH1D* m_h_calosyst_scale;
    TH1D* m_h_calosyst_reso;

    mutable TRandom3 m_rand;//so that we can call this from applyCorrection

    //internal property
    int m_units;//by default = -1, not set.  Set to 1 (MeV) if not value found in config file

    int getNPV() const;
    xAOD::EventInfo const * getDefaultEventInfo() const;


    //Read/write handles
    SG::ReadHandleKey<xAOD::VertexContainer>  m_VertexContKey;
    SG::ReadHandleKey<xAOD::MissingETContainer>  m_TruthContKey;
    SG::ReadHandleKey<xAOD::EventInfo>  m_EventInfoKey;


    StatusCode addMETAffectingSystematics();
    StatusCode extractHistoPath(std::string & histfile, std::string & systpath, std::string & configdir, std::string & suffix, SystType const & type);

    missingEt calcPtHard(xAOD::MissingETContainer const * const cont) const;
    missingEt caloSyst_scale(missingEt const &softTerms , double const scale) const;
    missingEt caloSyst_reso (missingEt const &softTerms) const;
    missingEt softTrkSyst_scale (missingEt const & softTerms, missingEt const & ptHard, double const shift) const;
    missingEt softTrkSyst_reso  (missingEt const & softTerms, missingEt const & ptHard, double const shift, double const smearpara,
				       double const smearperp) const;
    missingEt projectST     (missingEt const & softTerms, missingEt const & ptHard) const;



  };//METSystematicsTool

}

#endif //METUTILIES_METSYSTEMATICSTOOL_H

//  LocalWords:  METSystematicsTool
