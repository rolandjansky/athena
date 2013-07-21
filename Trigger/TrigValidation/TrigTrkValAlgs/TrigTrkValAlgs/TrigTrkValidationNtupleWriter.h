/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrigTrkValidationNtupleWriter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRIGTRKVALIDATIONNTUPLEWRITER_H
#define TRIGTRKVALIDATIONNTUPLEWRITER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

class TTree;
class ITrigL2ValidationNtupleTool; 

namespace Trk {
class IValidationNtupleTool;
class IRoIPropertyNtupleTool;
class IEventPropertyNtupleTool;

/** @class TrigTrkValidationNtupleWriter
    Class Trk::TrigTrkValidationNtupleWriter is the main algorithm which creates TrackValidation Ntuple. 
    It uses tools from TrkValTools to store Rec:TrackParticles, Trk::Tracks information and event information.
    The algorithm provides also information about RoI and link the information with tracks and is able to write TrigInDetTracks. 
    It uses helper tools in TrigTrkValTools. 
    
   @author Pavel.Ruzicka@cern.ch
*/

class TrigTrkValidationNtupleWriter : public AthAlgorithm {
 public:
  /** Standard Athena-Algorithm Constructor */
  TrigTrkValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default Destructor */
  ~TrigTrkValidationNtupleWriter();
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize();
  
 private:
  /** method to write track particle data to Ntuple. The method also add the RoI information. To do this Trig::TrigDecisionTool is used. 
      Rec::TrackParticles are retrieved as features.  */
  StatusCode writeTrackParticleEFData(unsigned int trackParticleColIndex); 

  /** method to write track particle data to Ntuple. Rec::Track particles are retrived from the SG */
  StatusCode writeTrackParticleData(unsigned int trackParticleColIndex); 

  /** method to write track particle data to Ntuple. InDetTracks are retrived from the SG */
  StatusCode writeInDetTrackTrigData(unsigned int trackParticleColIndex); 


  ToolHandle<Trk::IValidationNtupleTool>    m_ValTrkParticleNtupleTool;      //!< Tool for writting Rec::TrackParticle into the Ntuple
  ToolHandle<ITrigL2ValidationNtupleTool>   m_TrigL2ValidationNtupleTool;    //!< Tool for writting TrigInDetTrack into the Ntuple

  ToolHandle<Trig::TrigDecisionTool> m_trigDec;                              //!< Tool to recieve trigger decision information
  
  ToolHandleArray<Trk::IEventPropertyNtupleTool>    m_eventPropertyNtupleHandles;  //!< jobO: list of event property tools
  std::vector<const Trk::IEventPropertyNtupleTool*> m_eventPropertyNtupleTools;    //!< list of event property tools

  ToolHandleArray<Trk::IRoIPropertyNtupleTool>    m_roiPropertyNtupleHandles;   //!< list of RoI property handles
  std::vector<const Trk::IRoIPropertyNtupleTool*> m_roiPropertyNtupleTools;     //!< list of RoI property tools
  
  std::vector<std::string>  m_inputTrackParticleCollection;       //!< name of the TrackParticleCollections
  std::vector<std::string>  m_inputEFTrackParticleCollection;     //!< name of the EFTrackParticleCollections
  std::vector<std::string>  m_inputInDetTrackTrigCollection;      //!< name of the InDetTrackCollecions
  

  std::vector<TTree*>       m_trees;                    //!< Pointer to the NTuple trees (one for each input track collection)
  TTree*                    m_eventLinkTree;            //!< pointer to event-wise ntuple trees
  TTree*                    m_roiLinkTree;              //!< pointer to roi-wise ntuple trees (one for all input track collections)

  std::string               m_chainName;                //!< name of the trigger chain used for recieved feature collections
  std::string               m_roiName;                  //!< name of the EFTrackParticleCollection
  std::string               m_ntupleFileName;           //!< jobOption: Ntuple file name
  std::string               m_ntupleDirName;            //!< jobOption: Ntuple directory name
  bool                      m_doRoI;                    //!< turn on/off recording RoI information
};
 
} // close of namespace

#endif // TRIGTRKKVALIDATIONNTUPLEWRITER_H

