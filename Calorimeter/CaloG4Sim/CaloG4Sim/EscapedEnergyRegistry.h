/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EscapedEnergyRegistry
// 15-Jul-2004 William Seligman

#ifndef CaloG4_EscapedEnergyRegistry_H
#define CaloG4_EscapedEnergyRegistry_H

// This class keeps track of which types of volumes use which
// VEscapedEnergyProcessing objects.

// The types volumes are categorized by volume name.  I anticipate that
// there will be only two entries in this registry, one for "LAr::"
// and one for "Tile::", but there may be others in the future.

// In other words, LAr volumes will use one VEscapedEnergyProcessing
// object, and Tile volumes will another VEscapedEnergyProcessing
// object, and this class keeps track of which is which.

// Since there's only one registry, this class uses the singleton
// pattern.

#include "CaloG4Sim/VEscapedEnergyProcessing.h"
#include "globals.hh"

#include <map>

namespace CaloG4 {

  class EscapedEnergyRegistry
  {
  public:

    static EscapedEnergyRegistry* GetInstance();

    ~EscapedEnergyRegistry();

    // Add a VEscapedEnergyProcessing object to the registry.  The
    // name include "Adopt" because we assume responsibility for
    // deleting the pointer.
    void AddAndAdoptProcessing( const G4String& name, VEscapedEnergyProcessing* process );

    // Given a volume name, 
    VEscapedEnergyProcessing* GetProcessing( const G4String& volumeName ) const;

  protected:

    // Constructor protected according to singleton pattern.
    EscapedEnergyRegistry();

  private:

    typedef std::map< const G4String, VEscapedEnergyProcessing* > m_processingMap_t;
    typedef m_processingMap_t::iterator                           m_processingMap_ptr_t;
    typedef m_processingMap_t::const_iterator                     m_processingMap_const_ptr_t;
    m_processingMap_t                                             m_processingMap;

  };

} // namespace CaloG4

#endif // CaloG4_EscapedEnergyRegistry_H
