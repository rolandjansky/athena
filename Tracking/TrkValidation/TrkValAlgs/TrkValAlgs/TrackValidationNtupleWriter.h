/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackValidationNtupleWriter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRACKVALIDATIONNTUPLEWRITER_H
#define TRK_TRACKVALIDATIONNTUPLEWRITER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTruthData/TrackTruthCollection.h"

class TrackTruth;
class TTree;

#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {

class Track;
class ITruthToTrack;         // Produces perigee track parameters from generated parameters
class IGenParticleJetFinder;
class ITrackSelectorTool;
class ITrackValidationNtupleTool;
class IValidationNtupleTool;
class IGenParticleSelector;
class ITruthNtupleTool;
class IJetTruthNtupleTool;
class ITrackTruthClassifier;
class IEventPropertyNtupleTool;
class ValidationTrackTruthData;
class Vertex;

/** @class TrackValidationNtupleWriter
 
   Validation Algorithm to retrieve a Track Collection, loop over the tracks and write track information into ntuples using IValidationNtupleTool. For information about the ntuple variables filled, have a look into the IValidationNtupleTool implementation used.
 
   @author Andreas.Salzburger@cern.ch, Sebastian.Fleischmann@cern.ch, Wolfgang.Liebig@cern.ch
*/

class TrackValidationNtupleWriter : public AthAlgorithm {
 public:
  /** Standard Athena-Algorithm Constructor */
  TrackValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default Destructor */
  ~TrackValidationNtupleWriter();
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize();
  

protected:
  //!< following methods and atributes are used in derived class TrigTrackValidationNtupleWriter
  //!< Private method to associate truth to a track
  //const Trk::TrackParameters* associateTruthToTrack( const Track*, const TrackCollection*, const TrackTruth*& );
  StatusCode writeTrackData(unsigned int trackColIndex,
                            std::vector<Trk::ValidationTrackTruthData>&  truthData,
                            const Trk::Vertex* primaryVertex = NULL);

/** method to write track particle data to Ntuple. Rec::Track particles are retrived from the SG */
  StatusCode writeTrackParticleData(unsigned int trackParticleColIndex); 

  ToolHandleArray<Trk::ITrackValidationNtupleTool> m_ValidationNtupleTools;//!< set of tools for writing Trk::Track into the Ntuple
  ToolHandle<Trk::IValidationNtupleTool> m_ValTrkParticleNtupleTool;      //!< Tool for writting Rec::TrackParticle into the Ntuple -- OBSOLETE
  ToolHandle<Trk::ITruthNtupleTool>         m_truthNtupleTool;
  ToolHandle<Trk::IJetTruthNtupleTool>      m_jetTruthNtupleTool;
  ToolHandleArray<Trk::ITrackTruthClassifier> m_trackTruthClassifierHandles;    //!< jobO: list of truth classifiers
  std::vector<const Trk::ITrackTruthClassifier*> m_trackTruthClassifiers;     //!< the truth classifiers
  ToolHandleArray<Trk::IEventPropertyNtupleTool> m_eventPropertyNtupleHandles;//!< jobO: list of event property tools
  std::vector<const Trk::IEventPropertyNtupleTool*> m_eventPropertyNtupleTools;//!< list of event property tools
  
  std::vector<std::string>  m_inputTrackCollection;     //!< name of the TrackCollection
  std::vector<std::string>  m_inputTrackParticleCollection;     //!< name of the TrackParticleCollection
  std::vector<std::string>  m_trackTruthCollectionName; //!< name of the TrackTruthCollection
  std::string               m_McEventCollectionName;    //!< name of the McEventCollection
  std::string               m_inputPrimaryVertexCollection;//!< SG key of the input primary vertex collection (used by the track selector)
  std::string               m_ntupleFileName;           //!< jobOption: Ntuple file name
  std::string               m_ntupleDirName;               //!< jobOption: Ntuple directory name

  ToolHandle<Trk::ITruthToTrack>        m_truthToTrack;    //!< Tool handle to the Trk::TruthToTrack tool
  ToolHandle<Trk::ITrackSelectorTool>   m_trackSelector;   //!< Tool handle to the Trk::ITrackSelectorTool
  ToolHandle<Trk::IGenParticleSelector> m_genPartSelector; //!< Tool handle to the Trk::IGenParticleSelector
  ToolHandle<Trk::IGenParticleJetFinder> m_genJetFinder;   //!< Tool to find jets
  bool                      m_useExternalEventLinkTree;    //!< if TVNW should make event tree itself or assume CBNT does
  bool                      m_doTruth;                     //!< Switch to turn on / off truth
  bool                      m_doTrackParticle;             //!  Switch to turn on/pff recording track particle trees into Ntuple 

  const HepMC::GenParticle* m_visibleParticleWithoutTruth; //!< cludge to treat G4's "fake fakes"
  std::vector<unsigned int>           m_nTrackTreeRecords;
  std::vector<TTree*>       m_trees;                 //!< Pointer to the NTuple trees (one for each input track collection)
  TTree*                    m_eventLinkTree;         //!< pointer to event-wise ntuple trees (one for all input track collections + truth)
};
 
} // close of namespace

#endif // TRK_TRACKVALIDATIONNTUPLEWRITER_H

