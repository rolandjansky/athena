/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// IMcVtxFilterTool.h 
// Header file for class IMcVtxFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEKERNEL_IMCVTXFILTERTOOL_H 
#define MCPARTICLEKERNEL_IMCVTXFILTERTOOL_H 

// STL includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

// Forward declaration
class McEventCollection;
#include "AtlasHepMC/GenVertex_fwd.h"

static const InterfaceID IID_IMcVtxFilterTool("IMcVtxFilterTool", 1, 0);

class IMcVtxFilterTool : virtual public IAlgTool,
                         virtual public IProperty
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IMcVtxFilterTool();

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() = 0;
  virtual StatusCode  execute()    = 0;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /** Display the decay pattern of each McVtxFilter this Tool is embedding
   */
  virtual void displayOptions() const = 0;

  /** Display the statistics for each McVtxFilter this Tool is embedding
   */
  virtual void stats() const = 0;

  /** Return true if one of the McVtxFilters accept this GenVertex
   */
  virtual bool isAccepted( HepMC::ConstGenVertexPtr vtx ) const = 0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Filter the McEventCollection according to the list of McVtxFilter
   *  embedded in this AlgTool, and fill the output McEventCollection
   */
  virtual void filterMcEventCollection( const McEventCollection* mcCollection,
					McEventCollection* filterColl ) = 0;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IMcVtxFilterTool::interfaceID() 
{ 
   return IID_IMcVtxFilterTool; 
}

#endif //> MCPARTICLEKERNEL_IMCVTXFILTERTOOL_H
