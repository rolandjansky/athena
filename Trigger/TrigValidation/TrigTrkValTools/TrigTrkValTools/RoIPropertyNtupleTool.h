/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//   RoIPropertyNtupleTool.h
//   Header file for RoIPropertyNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka -at- cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_ROIPROPERTYNTUPLETOOL_H
#define TRK_ROIPROPERTYNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrigTrkValInterfaces/IRoIPropertyNtupleTool.h"
#include <vector>

/** @class RoIPropertyNtupleTool
    @brief ntuple helper tool to encapsulate filling of general RoI property e.g. eta, phi, Id.
    @Principle of this tool is the same as in case of the EventPropertyNtupleTool
    @author Pavel.Ruzicka -at- cern.ch
 */
class TrigRoiDescriptor;

namespace Trk {

  class RoIPropertyNtupleTool : virtual public IRoIPropertyNtupleTool, public AthAlgTool {
  public:

    // standard AlgTool constructor
    RoIPropertyNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~RoIPropertyNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    //! method not active in this tool
    virtual void registerTrackCollections( std::vector<std::string>) const;
    //! method not active in this tool
    virtual void setTrackTreeIndices( unsigned int, int, int) const;
    
    /** @brief add branches to the tree
      Should be called once dunring the initialisation phase by the calling algorithm
      (usually Trk::TrigTrkValidationNtupleWriter) */
    virtual StatusCode addNtupleItems ( TTree*, const std::string ) const;

    //! calculate event-wide data and copy into TTree branches, but don't write the record yet.
    virtual StatusCode fillRoIData ( const TrigRoiDescriptor* roi, bool isPassedTrigChain = false) const;

    //! reset ntuple variables (mainly for vectors which need to be cleared)
    virtual StatusCode  resetVariables ( ) const;

    //! method is true if instance of the tool links Rec::TrackParticles with RoI
    virtual  bool isTrkParticleLinkRoI() const;
    
    //! method is true if instance of the tool links TrigInDetTrack with RoI
    virtual  bool isTrigInDetTrackLinkRoI () const;
    
   private:
    // --- ntuple items --- 
    mutable int               m_runNumber;   //!< run number 
    mutable int               m_eventNumber; //!< event number
    mutable float             m_eta;         //!< eta of the RoI
    mutable float             m_phi;         //!< phi of the RoI
    mutable float             m_roiID;       //!< ID of the RoI  
    mutable bool              m_isPassedTrigChain; //!< set true if RoI is in a feature which passed trigger chain
  };
}

inline void Trk::RoIPropertyNtupleTool::registerTrackCollections
( std::vector<std::string>) const {}

inline void Trk::RoIPropertyNtupleTool::setTrackTreeIndices( unsigned int, int, int) const {}

inline bool Trk::RoIPropertyNtupleTool::isTrkParticleLinkRoI() const{ return false; }

inline bool Trk::RoIPropertyNtupleTool::isTrigInDetTrackLinkRoI() const{ return false; }

#endif // TRK_EVENTPROPERTYNTUPLETOOL_H
