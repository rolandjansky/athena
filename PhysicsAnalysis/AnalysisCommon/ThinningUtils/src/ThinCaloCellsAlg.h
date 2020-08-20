///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_THINCALOCELLSALG_H
#define THINNINGUTILS_THINCALOCELLSALG_H 1

/**
   @class ThinCaloCellsAlg
   @brief Tool to thin (remove) individual xAOD::CaloClusters from a container.
          This algorithm is not doing anything itself. It is rather holding a
          private instance of the ThinCaloCellsTool and it is forwarding
          its Properties to this tool and finally using this tool for every event.

   @author Karsten Koeneke
*/


// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class IThinningTool;
}



class ThinCaloCellsAlg
  : public ::AthAlgorithm
{
 public:

  /// Constructor with parameters:
  ThinCaloCellsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~ThinCaloCellsAlg();

  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'CaloCellsToThin' property
  void setupCaloCellsToThin( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'InputContainerList' property
  void setupInputContainerList( Gaudi::Details::PropertyBase& /*prop*/ );


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream being thinned" };

  /// The ToolHandle to the ThinningTool
  ToolHandle<DerivationFramework::IThinningTool> m_thinTool;


  /// Name of the CaloCellContainer to thin
  StringProperty m_caloCellKey;

  /// This boolean is true if the user sets the 'CaloCellsToThin' property
  bool m_setCaloCellKey;


  /// List of names of the object collections
  StringArrayProperty m_inCollKeyList;

  /// This boolean is true if the user sets the 'InputContainerList' property
  bool m_setInCollKey;


  /// Internal event counter
  unsigned long m_nEventsProcessed;

};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/// This internal method will realize if a user sets the 'CaloCellsToThin' property
inline void ThinCaloCellsAlg::setupCaloCellsToThin( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setCaloCellKey = true;
  return;
}

/// This internal method will realize if a user sets the 'InputContainerList' property
inline void ThinCaloCellsAlg::setupInputContainerList( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setInCollKey = true;
  return;
}


#endif //> !THINNINGUTILS_THINCALOCELLSALG_H
