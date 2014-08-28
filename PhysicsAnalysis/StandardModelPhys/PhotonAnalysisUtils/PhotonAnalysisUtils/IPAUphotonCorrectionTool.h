//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// IPAUphotonCorrectionTool.h, (c) ATLAS Detector software 2007
// author: P.A. Delsart
///////////////////////////////////////////////////////////////////
// Interface for IPhotonCorrection tool
///////////////////////////////////////////////////////////////////

#ifndef HIGGSANALYSYSUTILS_IPHOTONCORRECTIONTOOL_H
#define HIGGSANALYSYSUTILS_IPHOTONCORRECTIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "egammaEvent/Photon.h"
#include "PhotonAnalysisUtils/PAUegamma.h"
#include "ParticleEvent/CompositeParticle.h"

static const InterfaceID IID_IPAUphotonCorrectionTool("IPAUphotonCorrectionTool", 1, 0);

class IPAUphotonCorrectionTool : virtual public IAlgTool {
  
public:

  /** Virtual destructor */
  virtual ~IPAUphotonCorrectionTool(){};

  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUphotonCorrectionTool; };
 
  // public methods:


  /**** new ****/
  typedef PAU::egamma photon_t;


  /// A list of diferrent angle correction for phontons
  enum angle_correction_t {
    angle_none=0,
    PAUhggFitter_Calo = 1,
    PAUhggFitter_Conv,
    PAUhggFitter_Track,
    Track,
    PAUhggFitter_ConvTrack,

    // code > 100 let to user
    userdefined0 = 100
  };

  /// A list different energy corrections for converted photons
  enum energy_correction_t {
    e_none =0
  };

  /// Identifies all possible corrections
  /// For performance studies we want to test lots of different corrections : this class is used to identify a set of correction
  /// applied to photons.
  class correction_id {
  public:
    correction_id() : angle_correction(0), energy_correction(0), do_pvf(0), do_eta_correct_convphoton(0) {} ;
    correction_id(int angle, int energy, bool pvf = true , bool eta_correct_convphoton = false):
      angle_correction(angle), energy_correction(energy), do_pvf(pvf), do_eta_correct_convphoton(eta_correct_convphoton){};
    inline bool operator==(const correction_id& id2) const {
      return (angle_correction==id2.angle_correction) && (energy_correction==id2.energy_correction) 
	&& (do_pvf==id2.do_pvf) && (do_eta_correct_convphoton==id2.do_eta_correct_convphoton) ;
    }
    
    int angle_correction;
    int  energy_correction;
    bool do_pvf;
    bool do_eta_correct_convphoton;
  };
  /// A comparison fct to allow the use of correction_id in a std::map
  struct comp_correction_id
  {
    bool operator()(const correction_id &id1, const correction_id &id2) const
    {
      if( id1.angle_correction != id2.angle_correction) return id1.angle_correction < id2.angle_correction;
      else if( id1.energy_correction != id2.energy_correction) return id1.energy_correction < id2.energy_correction;
      else if( id1.do_pvf != id1.do_pvf) return id1.do_pvf < id2.do_pvf;
      else  return id1.do_eta_correct_convphoton < id2.do_eta_correct_convphoton;
    }
  };


  /// This subclass store all corrected quantities for a given set of corrections.
  class corrected_set_t {
  public:
    corrected_set_t() :  Ph1(0), Ph2(0), Higgs(0),
			 vtx_beforePVF_z(-9999.), vtx_beforePVF_zres(-9999.),
			 vtx_afterPVF_z(-9999.), vtx_afterPVF_zres(-9999.), vtx_afterPVF_zlike(-9999.),
			 isCorrected(false) {};

    Analysis::Photon *Ph1, *Ph2;
    CompositeParticle * Higgs;
    // vertex information. Useful for perf studies :
    float vtx_beforePVF_z, vtx_beforePVF_zres;
    float vtx_afterPVF_z,  vtx_afterPVF_zres,  vtx_afterPVF_zlike ;
    // correction status
    bool isCorrected;
    // make sure that Ph1->pt > Ph2->pt, return true if needed to swap
    bool sort_ph(){Analysis::Photon* tmp; if(Ph1->pt()>Ph2->pt()) return false; tmp=Ph2;Ph2=Ph1;Ph1=tmp;return true;}
  };


  typedef std::map<correction_id, corrected_set_t, comp_correction_id> corrections_store_t;



  /// Initialize the tool with 2 photons. 
  /// ph1 and ph2 are expected to be correctly initialized (with tracks and conv if any)
  virtual void set_photons(photon_t *ph1, photon_t *ph2) = 0;

  /// Return the id of the preferred set of corrections
  virtual correction_id preferred_corrections() = 0;
  
  /// Computes corrections defined by id and return a set of corrected quantities
  virtual corrected_set_t compute_single_correction(correction_id id) = 0;

  /// Compute and store a list of different corrections
  virtual void compute_default_corrections() = 0;

  /// Access the corrected quantities for correction id 
  /// return false if correction id has not been computed
  virtual bool retrieve_corrections(correction_id id, corrected_set_t &corrSet) = 0;

  /// Direct access to corrected particles
  virtual Analysis::Photon * retrieve_corrected_ph1(correction_id id) = 0;
  virtual Analysis::Photon * retrieve_corrected_ph2(correction_id id) = 0;
  virtual CompositeParticle * retrieve_corrected_higgs(correction_id id) = 0;



  // Following functions are used internally.
  // left public for experts.
  virtual void init_correction(corrected_set_t &corrSet) = 0;
  /// Perform corrections
  virtual bool do_angle_corrections(correction_id id, corrected_set_t &corrSet) = 0;
  virtual bool do_energy_corrections(correction_id id, corrected_set_t &corrSet) = 0;

  virtual bool correct_hggFitterCaloOnly(corrected_set_t &corrSet , bool correct_eta_conv = false) = 0;
  virtual bool correct_hggFitterConv(corrected_set_t &corrSet , bool correct_eta_conv = false) = 0;
  virtual bool correct_hggFitterTrack(corrected_set_t &corrSet , bool correct_eta_conv = false) = 0;
  virtual bool correct_hggFitterConvTrack(corrected_set_t &corrSet , bool correct_eta_conv = false) = 0;
  virtual bool correct_withPrimaryVertex(corrected_set_t &corrSet, bool correct_eta_conv = false) = 0;
  virtual bool correct_withGivenVertex(float z, float zres, corrected_set_t &corrSet, bool correct_eta_conv = false) = 0;

  /// Combine corrected photons to form a corrected Higgs
  virtual void finalize_correction(corrected_set_t &corrSet) = 0;


  /**** end new ****/


private:
  /// a handle on Store Gate 
  StoreGateSvc* m_storeGate;



  
};

#endif // HIGGSANALYSYSUTILS_IPHOTONCORRECTIONTOOL_H
