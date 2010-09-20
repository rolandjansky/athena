/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IROIPropertyNtupleTool.h
//   Header file for interface of ROIPropertyNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka -ad- cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IROIPROPERTYNTUPLETOOL_H
#define TRK_IROIPROPERTYNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"

class TTree;
class TrigRoiDescriptor;

namespace Trk {

static const InterfaceID IID_IRoIPropertyNtupleTool("IRoIPropertyNtupleTool",1,0);

/** @class IRoIPropertyNtupleTool
    provides the interface for validation tools which write special information
    about the RoI. Such info is for instance eta and phi.
*/

class IRoIPropertyNtupleTool : virtual public IAlgTool {
public:
  //! Interface ID, declared here, and defined below*/
  static const InterfaceID& interfaceID();

  //! make active track collections known to tool, such that a delayed addition to tree is possible
  virtual void registerTrackCollections ( std::vector<std::string>) const = 0;

  virtual StatusCode addNtupleItems( TTree* tree, const std::string prefix="" ) const = 0;

  //! transfer of track-tree indices into RoI tree variables, to allow delayed writing by TV or CBNT
  virtual void setTrackTreeIndices( unsigned int, int, int) const = 0;

  //! fill ntuple data from calculations, storegate etc but DO NOT WRITE Tree record
  virtual StatusCode fillRoIData ( const TrigRoiDescriptor* roi , bool isPassedTrigChain = false) const = 0;

  //! reset ntuple variables (mainly for vectors which need to be cleared)
  virtual StatusCode  resetVariables ( ) const = 0;
  
  //! method is true if instance of the tool links Rec::TrackParticles with RoI
  virtual  bool isTrkParticleLinkRoI () const = 0;
  
  //! method is true if instance of the tool links TrigInDetTrack with RoI
  virtual  bool isTrigInDetTrackLinkRoI () const = 0;
};

inline const InterfaceID& Trk::IRoIPropertyNtupleTool::interfaceID() {
    return IID_IRoIPropertyNtupleTool;
}

} // end of namespace

#endif // TRK_IEVENTPROPERTYNTUPLETOOL_H
