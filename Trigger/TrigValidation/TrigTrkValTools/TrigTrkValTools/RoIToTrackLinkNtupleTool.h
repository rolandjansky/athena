/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// RoIToTrackLinkNtupleTool.h
//   Header file for RoIToTrackLinkNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka -at- cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_ROITRACKLINKNTUPLETOOL_H
#define TRK_ROITRACKLINKNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrigTrkValInterfaces/IRoIPropertyNtupleTool.h"
#include <vector>
#include <string>
class TTree;

/** @class RoIToTrackLinkNtupleTool
    @brief document!
    @ This helper tool links information between Rec::TrackParticle information and RoI. The principle is the same as in the case of event information. 
    @ Thee tool is usualy called by the TrigTrkNtupleWriter tool.
    @author Pavel.Ruzicka -at- cern.ch
 */

namespace Trk {

  class RoIToTrackLinkNtupleTool : virtual public IRoIPropertyNtupleTool, public AthAlgTool {
  public:

    // standard AlgToolmethods
    RoIToTrackLinkNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~RoIToTrackLinkNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    //! see interface for documentation
    virtual void registerTrackCollections( std::vector<std::string>) const;
    //! see interface for documentation
    virtual void setTrackTreeIndices( unsigned int, int, int) const;

    /** @brief add branches to the tree
        Should be called once dunring the initialisation phase by the calling algorithm
        (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems ( TTree*, const std::string ) const;

    //! calculate event-wide data and copy into TTree branches, but don't write the record yet.
    virtual StatusCode fillRoIData ( const TrigRoiDescriptor* roi,  bool isPassedTrigChain = false) const;

    //! reset ntuple variables (mainly for vectors which need to be cleared)
    virtual StatusCode  resetVariables ( ) const;

    //! method is true if instance of the tool links Rec::TrackParticles with RoI
    virtual  bool isTrkParticleLinkRoI () const;

    //! method is true if instance of the tool links TrigInDetTrack with RoI
    virtual  bool isTrigInDetTrackLinkRoI () const;
  private:
    /** checks if recieved strings using declare_property are defined among constants: trkParticleTool, trigInDetTrackTool*/
    StatusCode checkToolType() const;
    
    std::string                       m_trackToolType;	       //!< string recieved from python configuration script telling which collection belongs to the class instance  
    mutable TTree*                    m_eventLinkTree;         //!< pointer to event-wise ntuple trees (one for all input track collections + truth)
    mutable std::vector<std::string>  m_trackCollections;      //!< name of the TrackCollections to form tree branch names later

    // --- ntuple items --- 
    mutable std::vector<int>  m_trackIndexBegin;     //!< index-based link from RoI tree to track tree entry.
    mutable std::vector<int>  m_nTracksPerRoI;       //!< Number of tracks per RoI, to form track loop.
    
    static const std::string trkParticleTool; 	     //!< defined string which denotes that instanstance works with Rec::TrackParticles
    static const std::string trigInDetTrackTool;     //!< defined string which denotes that instanstance works with TrigInDetTrack	
    };


}
#endif // TRK_ROITRACKLINKNTUPLETOOL_H
