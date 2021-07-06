/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloG4_EscapedEnergyRegistry_H
#define CaloG4_EscapedEnergyRegistry_H

#include "CaloG4Sim/VEscapedEnergyProcessing.h"
#include "CxxUtils/checker_macros.h"
#include "globals.hh"

#include <map>

#include <thread>
#include "G4Types.hh"
#ifdef G4MULTITHREADED
#  include "tbb/concurrent_unordered_map.h"
#endif
namespace CaloG4
{

  /// @class EscapedEnergyRegistry
  /// @brief Keeps track of which types of volumes use which
  /// VEscapedEnergyProcessing objects.
  ///
  /// 15-Jul-2004 William Seligman
  ///
  /// The types volumes are categorized by volume name.  I anticipate that
  /// there will be only two entries in this registry, one for "LAr::"
  /// and one for "Tile::", but there may be others in the future.
  ///
  /// In other words, LAr volumes will use one VEscapedEnergyProcessing
  /// object, and Tile volumes will another VEscapedEnergyProcessing
  /// object, and this class keeps track of which is which.
  ///
  /// Since there's only one registry, this class uses the singleton
  /// pattern.
  ///
  /// UPDATE:
  ///
  ///12-Apr-2019 Marilena Bandieramonte
  ///
  /// This singleton class was not thread-safe.
  /// Added the #ifdef G4MULTITHREADED directive to handle
  /// the multithreaded case. One instance of the class will be created
  /// per each thread and stored in a tbb::concurrent_unordered_map that
  /// is hashed with the threadID number.
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
#ifdef G4MULTITHREADED
     // Thread-to-EscapeEnergyRegistry concurrent map type
    using EERThreadMap_t = tbb::concurrent_unordered_map< std::thread::id, EscapedEnergyRegistry*, std::hash<std::thread::id> >;
    // Concurrent map of EERs, one for each thread
    static EERThreadMap_t m_EERThreadMap ATLAS_THREAD_SAFE;
    // @brief Search inside m_EERThreadMap the element with the current threadID 
    // and return it or return a null pointer if the element is not found
    static EscapedEnergyRegistry* getEER();
    // @brief Insert the current EER in m_EERThreadMap and 
    // associate it with the current threadID
    static EscapedEnergyRegistry* setEER();
#endif
  };

} // namespace CaloG4

#endif // CaloG4_EscapedEnergyRegistry_H
