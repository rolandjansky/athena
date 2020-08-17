///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AssocBuilder.h 
// Header file for class AssocBuilder
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONCOMPS_ASSOCBUILDER_H 
#define ASSOCIATIONCOMPS_ASSOCBUILDER_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class IAssocBuilderTool;

class AssocBuilder : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AssocBuilder( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AssocBuilder(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  AssocBuilder();

  /** Callback method to setup the "ObjectContainers" property
   */
  void setupInputContainers( Gaudi::Details::PropertyBase& inputNames );

  /** Callback method to setup the "AssocContainers" property
   */
  void setupInputAssocContainers( Gaudi::Details::PropertyBase& inputAssocNames );

  /** Callback method to setup the "AssocsOutput" property
   */
  void setupAssocsOutput( Gaudi::Details::PropertyBase& assocsOutputName );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  // Containers

  /** List of containers to check for overlap: each element of each of
   *  these containers will be analysed to create association with the
   *  association containers.
   */
  StringArrayProperty m_inputNames;

  /** List of containers against which we'll check for some overlapping
   *  object
   */
  StringArrayProperty m_inputAssocNames;
  
  /** Output location for the map of associations this algorithm is
   *  building.
   */
  StringProperty m_assocsOutputName;

  typedef ToolHandleArray<IAssocBuilderTool> IAssocBuilderTools_t;

  /** List of association builder tools. These tools will be dynamically
   *  plugged-in from the jobOption file.
   */
  IAssocBuilderTools_t m_assocTools;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ASSOCIATIONCOMPS_ASSOCBUILDER_H
