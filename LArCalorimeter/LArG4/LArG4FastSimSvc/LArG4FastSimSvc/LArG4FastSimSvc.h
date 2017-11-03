#ifndef LARG4FASTSIMSVC_LARG4FASTSIMSVC_H
#define LARG4FASTSIMSVC_LARG4FASTSIMSVC_H

// local includes
#include "LArG4FastSimSvc/ILArG4FastSimSvc.h"
#include "AthenaBaseComps/AthService.h"

// Minimal service includes
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/Service.h"
#include "GaudiKernel/Property.h"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"

// STL includes
#include <string>
#include <map>

template <class TYPE> class SvcFactory;

namespace HepMC {
	class IO_GenEvent;
}


 /**
   *
   *   @short Class for fast simulation option service
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Zachary Marshal
   *
   * @version $Id: LArG4FastSimSvc.h 485620 2012-02-28 20:00:50Z gsedov $
   *
   */

class LArG4FastSimSvc : public AthService, virtual public ILArG4FastSimSvc {

 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILArG4FastSimSvc; }

  /// Query interface
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  inline virtual bool   flagToShowerLib ( const G4ParticleDefinition& particleType )  const;	      //!< get switch for frozen showers
  inline virtual double maxEneToShowerLib( const G4ParticleDefinition& particleType ) const;	      //!< get lower energy limit for frozen showers
  inline virtual double minEneToShowerLib( const G4ParticleDefinition& particleType ) const;	      //!< get upper energy limit for frozen showers

  inline virtual bool ContainLow()           const;           //!< Apply check containment at low eta
  inline virtual bool ContainHigh()          const;           //!< Apply check containment at high eta
  inline virtual bool ContainCrack()         const;           //!< Apply check containment in crack region

  inline virtual bool              generateFSStartingPoints() const;
  virtual bool                     generateFSStartingPoint( const HepMC::GenEvent * ge) const;
  inline virtual float             generateFSStartingPointsRatio() const;

  inline virtual int DetectorTag() const;

  friend class SvcFactory<LArG4FastSimSvc>;

  /// Standard Constructor
  LArG4FastSimSvc(const std::string& name, ISvcLocator* sl);

  /// Standard Destructor
  virtual ~LArG4FastSimSvc();

 private:

  void   print() const;                    //!< dump object content

  // data members
  bool   m_e_FlagShowerLib;			     //!< switch for electron frozen showers	      
  double m_e_MinEneShowerLib;			     //!< lower energy limit for electron frozen showers 
  double m_e_MaxEneShowerLib;			     //!< upper energy limit for electron frozen showers 

  bool   m_g_FlagShowerLib;			     //!< switch for photon frozen showers		      
  double m_g_MaxEneShowerLib;			     //!< lower energy limit for photon frozen showers   
  double m_g_MinEneShowerLib;			     //!< upper energy limit for photon frozen showers   

  bool   m_Neut_FlagShowerLib;                         //!< switch for neutron frozen showers
  double m_Neut_MinEneShowerLib;                       //!< lower energy limit for neutron frozen showers
  double m_Neut_MaxEneShowerLib;                       //!< upper energy limit for neutron frozen showers

  bool   m_Pion_FlagShowerLib;                         //!< switch for pion frozen showers
  double m_Pion_MinEneShowerLib;                       //!< lower energy limit for pion frozen showers
  double m_Pion_MaxEneShowerLib;                       //!< upper energy limit for pion frozen showers

  bool   m_containLow;                               //!< switch for containment check at low eta
  bool   m_containHigh;                              //!< switch for containment check at high eta
  bool   m_containCrack;                             //!< switch for containment check in crack region

  StringProperty   m_generated_starting_points_file;                 //!< switch for shower starting point record
  float            m_generated_starting_points_ratio;                //!< switch for shower starting point record
  bool             m_generate_starting_points;
  HepMC::IO_GenEvent * m_starting_points_file;
  int             m_detector_tag;                                   //!< name for the detector tag for the ShowerLibSvc
  std::string     m_detector_tag_str;
  std::map<std::string,int> m_detmap;


};

// Get methods
inline bool   LArG4FastSimSvc::flagToShowerLib( const G4ParticleDefinition& particleType )   const
{
  if ( &particleType == G4Electron::ElectronDefinition() ||
	   &particleType == G4Positron::PositronDefinition() ) {
         return m_e_FlagShowerLib;
  } else if ( &particleType == G4Gamma::GammaDefinition() ) {
         return m_g_FlagShowerLib;
  } else if ( &particleType == G4Neutron::NeutronDefinition() ) {
         return m_Neut_FlagShowerLib;
  } else if ( &particleType == G4PionPlus::PionPlusDefinition() ||
              &particleType == G4PionMinus::PionMinusDefinition() ) {
         return m_Pion_FlagShowerLib;
  } else return false;
}
inline double LArG4FastSimSvc::minEneToShowerLib( const G4ParticleDefinition& particleType ) const
{
  if ( &particleType == G4Electron::ElectronDefinition() ||
       &particleType == G4Positron::PositronDefinition() ) {
         return m_e_MinEneShowerLib;
  } else if ( &particleType == G4Gamma::GammaDefinition() ) {
         return m_g_MinEneShowerLib;
  } else if ( &particleType == G4Neutron::NeutronDefinition() ) {
         return m_Neut_MinEneShowerLib;
  } else if ( &particleType == G4PionPlus::PionPlusDefinition() ||
              &particleType == G4PionMinus::PionMinusDefinition() ) {
         return m_Pion_MinEneShowerLib;
  } else return 0.0;
}

inline double LArG4FastSimSvc::maxEneToShowerLib( const G4ParticleDefinition& particleType ) const
{
  if ( &particleType == G4Electron::ElectronDefinition() ||
       &particleType == G4Positron::PositronDefinition() ) {
         return m_e_MaxEneShowerLib;
  } else if ( &particleType == G4Gamma::GammaDefinition() ) {
         return m_g_MaxEneShowerLib;
  } else if ( &particleType == G4Neutron::NeutronDefinition() ) {
         return m_Neut_MaxEneShowerLib;
  } else if ( &particleType == G4PionPlus::PionPlusDefinition() ||
              &particleType == G4PionMinus::PionMinusDefinition() ) {
         return m_Pion_MaxEneShowerLib;
  } else return 0.0;
}

inline bool   LArG4FastSimSvc::ContainLow()         const { return m_containLow; }   //!< Apply check containment at low eta
inline bool   LArG4FastSimSvc::ContainHigh()        const { return m_containHigh; }  //!< Apply check containment at high eta
inline bool   LArG4FastSimSvc::ContainCrack()       const { return m_containCrack; } //!< Apply check containment in crack region

inline bool              LArG4FastSimSvc::generateFSStartingPoints() const { return m_generate_starting_points; }
inline float             LArG4FastSimSvc::generateFSStartingPointsRatio() const { return m_generated_starting_points_ratio; }
inline int LArG4FastSimSvc::DetectorTag() const { return m_detector_tag; }
#endif // LARG4FASTSIMSVC_LARG4FASTSIMSVC_H
