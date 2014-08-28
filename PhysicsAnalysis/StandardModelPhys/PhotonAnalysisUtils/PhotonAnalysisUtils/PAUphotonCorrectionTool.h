//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// PAUEvent.h, (c) ATLAS Detector software 2007
// author: P.A. Delsart
///////////////////////////////////////////////////////////////////
// \class PAUphotonCorrectionTool
// A tool designed to group all correction (angle, energy, ... other?) to 
// be applied to photons in the Higgs->gamma gamma analysis.
// It combines other tools (PAUhggFitter, PAUprimaryVertexFinder) to compute different 
// sets of corrections.
///////////////////////////////////////////////////////////////////////////////


#ifndef HIGGSANALYSYSUTILS_PHOTONCORRECTIONTOOL_H
#define HIGGSANALYSYSUTILS_PHOTONCORRECTIONTOOL_H

// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "PhotonAnalysisUtils/IPAUphotonCorrectionTool.h"
#include "PhotonAnalysisUtils/PAUegamma.h"

#include "PhotonAnalysisUtils/IPAUprimaryVertexFinder.h"
#include "PhotonAnalysisUtils/IPAUhggFitter.h"
#include "PhotonAnalysisUtils/IPAUconversionFlagTool.h"
#include "ParticleEvent/CompositeParticle.h"
#include "egammaEvent/Photon.h"

class I4Momentum;
class IPAUprimaryVertexFinder;
class IPAUhggFitter;
class IPAUconversionFlagTool;


#include <map>
#include <string>

//MF static const InterfaceID IID_PAUphotonCorrectionTool("PAUphotonCorrectionTool", 1, 0);

class PAUphotonCorrectionTool : public AthAlgTool, virtual public IPAUphotonCorrectionTool {
  
public:

//   typedef PAU::gamma photon_t;


//   /// A list of diferrent angle correction for phontons
//   enum angle_correction_t {
//     angle_none=0,
//     PAUhggFitter_Calo = 1,
//     PAUhggFitter_Conv,
//     PAUhggFitter_Track,
//     Track,
//     PAUhggFitter_ConvTrack,

//     // code > 100 let to user
//     userdefined0 = 100
//   };

//   /// A list different energy corrections for converted photons
//   enum energy_correction_t {
//     e_none =0
//   };

//   /// Identifies all possible corrections
//   /// For performance studies we want to test lots of different corrections : this class is used to identify a set of correction
//   /// applied to photons.
//   class correction_id {
//   public:
//     correction_id() : angle_correction(0), energy_correction(0), do_pvf(0), do_eta_correct_convphoton(0) {} ;
//     correction_id(int angle, int energy, bool pvf = true , bool eta_correct_convphoton = false):
//       angle_correction(angle), energy_correction(energy), do_pvf(pvf), do_eta_correct_convphoton(eta_correct_convphoton){};
//     inline bool operator==(const correction_id& id2) const {
//       return (angle_correction==id2.angle_correction) && (energy_correction==id2.energy_correction) 
// 	&& (do_pvf==id2.do_pvf) && (do_eta_correct_convphoton==id2.do_eta_correct_convphoton) ;
//     }
    
//     int angle_correction;
//     int  energy_correction;
//     bool do_pvf;
//     bool do_eta_correct_convphoton;
//   };
//   /// A comparison fct to allow the use of correction_id in a std::map
//   struct comp_correction_id
//   {
//     bool operator()(const correction_id &id1, const correction_id &id2) const
//     {
//       if( id1.angle_correction != id2.angle_correction) return id1.angle_correction < id2.angle_correction;
//       else if( id1.energy_correction != id2.energy_correction) return id1.energy_correction < id2.energy_correction;
//       else if( id1.do_pvf != id1.do_pvf) return id1.do_pvf < id2.do_pvf;
//       else  return id1.do_eta_correct_convphoton < id2.do_eta_correct_convphoton;
//     }
//   };


//   /// This subclass store all corrected quantities for a given set of corrections.
//   class corrected_set_t {
//   public:
//     corrected_set_t();
//     Analysis::Photon *Ph1, *Ph2;
//     CompositeParticle * Higgs;
//     // vertex information. Useful for perf studies :
//     float vtx_beforePVF_z, vtx_beforePVF_zres;
//     float vtx_afterPVF_z,  vtx_afterPVF_zres,  vtx_afterPVF_zlike ;
//     // correction status
//     bool isCorrected;
//     // make sure that Ph1->pt > Ph2->pt, return true if needed to swap
//     bool sort_ph(){Analysis::Photon* tmp; if(Ph1->pt()>Ph2->pt()) return false; tmp=Ph2;Ph2=Ph1;Ph1=tmp;return true;}
//   };


  typedef std::map<correction_id, corrected_set_t, comp_correction_id> corrections_store_t;


  

  /** AlgTool like constructor */
  PAUphotonCorrectionTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUphotonCorrectionTool(){};
  
  //MF static const InterfaceID& interfaceID() { return IID_PAUphotonCorrectionTool; }
  
  /** AlgTool initialize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  /// Initialize the tool with 2 photons. 
  /// ph1 and ph2 are expected to be correctly initialized (with tracks and conv if any)
  void set_photons(photon_t *ph1, photon_t *ph2);

  /// Return the id of the preferred set of corrections
  correction_id preferred_corrections();
  
  /// Computes corrections defined by id and return a set of corrected quantities
  corrected_set_t compute_single_correction(correction_id id);

  /// Compute and store a list of different corrections
  void compute_default_corrections(){};

  /// Access the corrected quantities for correction id 
  /// return false if correction id has not been computed
  bool retrieve_corrections(correction_id id, corrected_set_t &corrSet);

  /// Direct access to corrected particles
  Analysis::Photon * retrieve_corrected_ph1(correction_id id);
  Analysis::Photon * retrieve_corrected_ph2(correction_id id);
  CompositeParticle * retrieve_corrected_higgs(correction_id id);



  // Following functions are used internally.
  // left public for experts.
  void init_correction(corrected_set_t &corrSet);
  /// Perform corrections
  bool do_angle_corrections(correction_id id, corrected_set_t &corrSet);
  bool do_energy_corrections(correction_id id, corrected_set_t &corrSet);

  bool correct_hggFitterCaloOnly(corrected_set_t &corrSet , bool correct_eta_conv = false);
  bool correct_hggFitterConv(corrected_set_t &corrSet , bool correct_eta_conv = false);
  bool correct_hggFitterTrack(corrected_set_t &corrSet , bool correct_eta_conv = false);
  bool correct_hggFitterConvTrack(corrected_set_t &corrSet , bool correct_eta_conv = false);
  bool correct_withPrimaryVertex(corrected_set_t &corrSet, bool correct_eta_conv = false);
  bool correct_withGivenVertex(float z, float zres, corrected_set_t &corrSet, bool correct_eta_conv = false);

  /// Combine corrected photons to form a corrected Higgs
  void finalize_correction(corrected_set_t &corrSet);


protected:
  
  photon_t *m_inPh1, *m_inPh2; /// the input, uncorrected photons
  bool m_isConv1, m_isConv2;   /// flag conversions

  corrections_store_t m_corrections; /// map of corrected quantities


  // List of used tools :
  ToolHandle<IPAUprimaryVertexFinder>  m_PrimaryVertex;
  ToolHandle<IPAUhggFitter>            m_PAUhggFitterTool;
  ToolHandle<IPAUconversionFlagTool>   m_ConvTool ;

protected:
  // internal functions :
  void clear();
  bool correct_hggFitter_generic(corrected_set_t &corrSet , bool correct_eta_conv = false);


};


#endif // HIGGSANALYSYSUTILS_PHOTONCORRECTIONTOOL_H
