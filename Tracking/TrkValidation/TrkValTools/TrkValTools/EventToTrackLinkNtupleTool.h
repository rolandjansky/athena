/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// EventToTrackLinkNtupleTool.h
//   Header file for EventToTrackLinkNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_EVENTTRACKLINKNTUPLETOOL_H
#define TRK_EVENTTRACKLINKNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkValInterfaces/IEventPropertyNtupleTool.h"
#include <vector>
class TTree;

/** @class EventToTrackLinkNtupleTool
    @brief document!

    @author Wolfgang.Liebig -at- cern.ch
 */

namespace Trk {

  class EventToTrackLinkNtupleTool : virtual public IEventPropertyNtupleTool, public AthAlgTool {
  public:

    // standard AlgToolmethods
    EventToTrackLinkNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~EventToTrackLinkNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    //! see interface for documentation
    virtual void registerTrackCollections( std::vector<std::string>, bool) const;
    //! see interface for documentation
    virtual void setTrackTreeIndices( unsigned int, int, int) const;
    //! see interface for documentation
    virtual void setGenParticleTreeIndices( int, int) const;

    /** @brief add branches to the tree
        Should be called once dunring the initialisation phase by the calling algorithm
        (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems ( TTree*, const std::string ) const;

    //! calculate event-wide data and copy into TTree branches, but don't write the record yet.
    virtual StatusCode fillEventData ( ) const;

    //! reset ntuple variables (mainly for vectors which need to be cleared)
    virtual StatusCode  resetVariables ( ) const;
       
    //! is True if instance is Tool which links events property to Trk::Tracks
   inline  virtual bool isTrackLinkTool( ) const;
    //! is True if instance is Tool which links events property to Rec::TrkParticle
   inline  virtual bool isTrkParticleLinkTool( ) const;
    //! is True if instance is Tool which links events property to Rec::TrkParticle recieved useing TrigDecTool
   inline  virtual bool isTrkParticleTrigLinkTool( ) const;
    //! is True if instance is Tool which links events property to InDetTrack recieved useing TrigDecTool
   inline  virtual bool isInDetTrackTrigLinkTool( ) const;
    //! is True if instance is EventPropertyTool
   inline  virtual bool isEvtPropertyTool( ) const;

  private:
    //! checks if recieved string collectionType corresponds to some collection type names [Trk::Track, Rec::TrackParticle, Rec::TrackParticle_Trig]
    StatusCode checkCollectionType() const;

    std::string                       m_collectionType;
    mutable TTree*                    m_eventLinkTree;         //!< pointer to event-wise ntuple trees (one for all input track collections + truth)
    mutable bool                      m_doTruth;               //!< Switch to turn on / off truth
    mutable std::vector<std::string>  m_trackCollections;      //!< name of the TrackCollections to form tree branch names later

    // --- ntuple items --- 
    mutable std::vector<int>  m_trackIndexBegin;       //!< index-based link from event tree to track tree entry.
    mutable std::vector<int>  m_nTracksPerEvent;       //!< # of tracks per event, to form track loop.
    mutable int               m_genParticleIndexBegin; //!< index-based link from event tree to truth tree entry.
    mutable int               m_nGenParticlesPerEvent; //!< # of true tracks per event, to form truth loop.
    
    static const std::string trackTypeName;            //!< denotes instance which deals with Trk::Track
    static const std::string trkParticleTypeName;      //!< denotes instance which deals with Rec::TrackParticle
    static const std::string trkParticleTrigTypeName;  //!< denotes instance which deals with Rec::TrackParticle obtained as feature using TriDecTool
    static const std::string inDetTrackTrigTypeName;   //!< denotes instance which deals with InDetTrack obtained as feature using TriDecTool
  };

}

 
inline bool Trk::EventToTrackLinkNtupleTool::isTrackLinkTool( ) const {
                                                                     if( m_collectionType == trackTypeName) return true;
	                					     return false;
                                                                            }
   
inline bool Trk::EventToTrackLinkNtupleTool::isTrkParticleLinkTool( ) const {
                                                                       if( m_collectionType == trkParticleTypeName) return true;
								       return false;
                                                                            }
    
inline bool Trk::EventToTrackLinkNtupleTool::isTrkParticleTrigLinkTool( ) const{
                                                                         if( m_collectionType == trkParticleTrigTypeName) return true;
									 return false;
                                                                               }

inline bool Trk::EventToTrackLinkNtupleTool::isInDetTrackTrigLinkTool( ) const{
                                                                         if( m_collectionType == inDetTrackTrigTypeName) return true;
									 return false;
                                                                               }

inline bool Trk::EventToTrackLinkNtupleTool::isEvtPropertyTool( ) const {
                                                                      return false;
                                                                        } 


#endif // TRK_EVENTTRACKLINKNTUPLETOOL_H
