/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IEventPropertyNtupleTool.h
//   Header file for interface of EventPropertyNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig -ad- cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IEVENTPROPERTYNTUPLETOOL_H
#define TRK_IEVENTPROPERTYNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"

class TTree;

namespace Trk {

static const InterfaceID IID_IEventPropertyNtupleTool("IEventPropertyNtupleTool",1,0);

/** @class IEventPropertyNtupleTool
    provides the interface for validation tools which write special information
    about the whole event into ntuples. Such info is for instance timing and trigger.
    It also serves as the hybrid link between either the Trk::TrackValidationNtupleWriter
    or CBNT as the TTree-managing entity.

    @author Wolfgang.Liebig -at- cern.ch
*/

class IEventPropertyNtupleTool : virtual public IAlgTool {
 public:  
  //! Interface ID, declared here, and defined below*/
  static const InterfaceID& interfaceID();

  //! make active track collections known to tool, such that a delayed addition to tree is possible
  virtual void registerTrackCollections ( std::vector<std::string>,
                                                bool ) const = 0;

  /*
    virtual void registerTrackParticleCollections ( std::vector<std::string>,
    bool ) const = 0; */

  /** define tree entries on external tree
      @param[in] tree is the TTree from the Trk::TrackValidationNtupleWriter or from CBNT
      @param[in] prefix is usually none, but allows a tracking-specific prefix when added to CBNT
    */
  virtual StatusCode addNtupleItems( TTree* tree, const std::string prefix="" ) const = 0;

  //! transfer of track-tree indices into event tree variables, to allow delayed writing by TV or CBNT
  virtual void setTrackTreeIndices( unsigned int, int, int) const = 0;

  //! transfer of truth-tree indices into event tree variables
  virtual void setGenParticleTreeIndices( int, int) const = 0;

  //! fill ntuple data from calculations, storegate etc but DO NOT WRITE Tree record
  virtual StatusCode fillEventData ( ) const = 0;

  //! reset ntuple variables (mainly for vectors which need to be cleared)
  virtual StatusCode  resetVariables ( ) const = 0;

  //! is True if instance is Tool which links events property to Trk::Tracks
  virtual bool isTrackLinkTool( ) const = 0;
  //! is True if instance is Tool which links events property to Rec::TrkParticle
  virtual bool isTrkParticleLinkTool( ) const = 0;
  //! is True if instance is Tool which links events property to Rec::TrkParticle recieved using TrigDecTool
  virtual bool isTrkParticleTrigLinkTool( ) const = 0;
  //! is True if instance is Tool which links events property to InDetTrack recieved using TrigDecTool
  virtual bool isInDetTrackTrigLinkTool( ) const = 0;
  //! is True if instance is EventPropertyTool
  virtual bool isEvtPropertyTool( ) const = 0;
};

inline const InterfaceID& Trk::IEventPropertyNtupleTool::interfaceID() {
    return IID_IEventPropertyNtupleTool;
}

} // end of namespace

#endif // TRK_IEVENTPROPERTYNTUPLETOOL_H
