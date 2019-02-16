/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
// -*- c++ -*-
#ifndef INDETSECVTXTRACKSELECTIONTOOL_H
#define INDETSECVTXTRACKSELECTIONTOOL_H

// Local include(s):
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

// Framework include(s):
#include "AsgTools/AsgTool.h"
#ifndef XAOD_ANALYSIS
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#endif

#include <map>
#include <limits>

namespace InDet {

  // forward declaration of helper classes
  class SecVtxTrackAccessor;
  class SecVtxTrackCut;

   /// Implementation of the track selector tool
   ///
  class InDetSecVtxTrackSelectionTool :
    public virtual IInDetTrackSelectionTool, 
    public asg::AsgTool {
    
    friend class SecVtxTrackCut;

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( InDetSecVtxTrackSelectionTool,
		     CP::ISelectionTool,
		     InDet::IInDetTrackSelectionTool )
    
  public:
    /// Constructor for standalone usage
    InDetSecVtxTrackSelectionTool( const std::string& name );

    // The default destructor is OK but it must be defined in the
    // implementation file in order to forward declare with unique_ptr
    ~InDetSecVtxTrackSelectionTool();

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{
    
    /// Function initialising the tool
    virtual StatusCode initialize() override;
    /// Function finalizing the tool
    virtual StatusCode finalize() override;
    
    /// @}
    
    /// @name Function(s) implementing the IAsgSelectionTool interface
    /// @{
    
    /// Get an object describing the "selection steps" of the tool
    virtual const asg::AcceptInfo& getAcceptInfo() const override;
    
    /// Get the decision using a generic IParticle pointer
    virtual  asg::AcceptData accept( const xAOD::IParticle* ) const override;
    
    /// @}
    
    /// @name Function(s) implementing the IInDetSecVtxTrackSelectionTool interface
    /// @{
    
    /// Get the decision for a specific track object
    virtual  asg::AcceptData accept( const xAOD::TrackParticle& track,
					 const xAOD::Vertex* vertex = nullptr ) const override;

#ifndef XAOD_ANALYSIS
    virtual  asg::AcceptData accept( const Trk::Track& track,
					   const Trk::Vertex* vertex = nullptr) const override;
#endif
    
    /// @}
    virtual void setCutLevel( InDet::CutLevel level, Bool_t overwrite = true ) override
       __attribute__ ((deprecated("For consistency with the athena interface, the cut level is best set through the \"CutLevel\" property.")));

  private:
    bool m_isInitialized = false; // flag whether or not the tool has been initialized, to check erroneous use cases.
    mutable bool m_warnInit = false; // flag to keep track of whether we have warned about a lack of initialization

    std::unordered_map< std::string, std::shared_ptr<SecVtxTrackAccessor> > m_trackAccessors; //!< list of the accessors that need to be run for each track

    // first element is cut family, second is the set of cuts
    std::map< std::string, std::vector< std::unique_ptr<SecVtxTrackCut> > > m_trackCuts; //!< First element is the name of the cut family, second element is the set of cuts

    mutable ULong64_t m_numTracksProcessed = 0; //!< a counter of the number of tracks proccessed
    mutable ULong64_t m_numTracksPassed = 0; //!< a counter of the number of tracks that passed all cuts
    mutable std::vector<ULong64_t> m_numTracksPassedCuts; //!< tracks the number of tracks that passed each cut family

    constexpr static Double_t LOCAL_MAX_DOUBLE = 1.0e16;
    constexpr static Int_t LOCAL_MAX_INT = std::numeric_limits<Int_t>::max();

    /// Object used to store the last decision
    asg::AcceptInfo m_acceptInfo; //!< Object that stores detailed selection information
    
    Double_t m_minD0 = -1.; //!< Minimum |d0| of tracks

    Int_t m_NPixel0TRT = -1 ; //! < Minimum number of Pixel hits when TRT has no hit
    Int_t m_minInDetHits = -1 ; //  Minimum number of total InDet hits

//    ToolHandle< InDet::IInDetTrackSelectionTool > m_trkFilter ;
#ifndef XAOD_ANALYSIS
    Bool_t m_initTrkTools = false; //!< Whether to initialize the Trk::Track tools
    Bool_t m_trackSumToolAvailable = false; //!< Whether the summary tool is available    
#endif // XAOD_ANALYSIS

  }; // class InDetSecVtxTrackSelectionTool

} // namespace InDet

#endif // INDETSECVTXTRACKSELECTIONTOOL_INDETSECVTXTRACKSELECTIONTOOL_H
