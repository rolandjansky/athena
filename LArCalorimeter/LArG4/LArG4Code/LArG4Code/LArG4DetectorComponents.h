/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4DetectorComponents
// Maintain a list of detector components.
// 05-Jun-2003 Bill Seligman

#ifndef __LArG4DetectorComponents_H__
#define __LArG4DetectorComponents_H__

#include "LArG4Code/LArVolumeInterface.h"

#include "globals.hh"

#include <map>


class LArG4DetectorComponents
{
public:

  enum EDetectorComponent {
    kBarrelCryostat ,      // The entire barrel cryostat
    kEndcapCryostat ,      // The entire endcap cryostat
    kEMB ,                 // The electromagnetic barrel calorimeter in the barrel cryostat
    kEMBps ,               // The electromagnetic barrel pre-sampler in the barrel cryostat
    kEMEC ,                // The electromagnetic endcap calorimeter in the endcap cryostat
    kEMECps ,              // The electromagnetic endcap pre-sampler in the endcap cryostat
    kFCAL ,                // The forward calorimeter in the endcap cryostat
    kHEC                   // The hadrnic calorimeter in the endcap cryostat
  };

  virtual ~LArG4DetectorComponents();

  // Accessor for pointer to the single instance of this class.
  static LArG4DetectorComponents* GetInstance();

  // If the user wishes to override the default detector components,
  // they do so by editing the LArG4DetectorComponents.cc file, or by
  // using the following method from the main program.  These methods
  // are "static" because I don't anticipate that there'll be multiple
  // ATLAS detectors in one G4 simulation.
  void SetDetectorComponent ( const EDetectorComponent, LArVolumeInterface* );
  LArVolumeInterface* GetDetectorComponent ( const EDetectorComponent ) const;

  // The positive (z>0) and negative (z<0) portions of the detector
  // can be built separately.  These access methods provides a common
  // reference point for sub-detectors to know which half is being
  // built.
  G4bool zNeg() const { return m_zNeg; }
  void SetzNeg( G4bool z ) { m_zNeg = z; }

protected:
  // The constructor is protected according to the standard singleton
  // design pattern.
  LArG4DetectorComponents();

private: 
  // Are we building the positive or the negative half of the
  // detector?  m_Zneg == true means that we're building the negative
  // half.
  static G4bool m_zNeg;

  // The list of volumes that will be assembled into the final
  // detector description.
  typedef std::map< EDetectorComponent, LArVolumeInterface* > m_detectorMap_t;
  typedef m_detectorMap_t::const_iterator                     m_detectorMap_ptr;
  m_detectorMap_t                                             m_detectorMap;

};

#endif // __LArG4DetectorComponents_H__
