/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETBEAMSPOTFINDER_H
#define INDET_INDETBEAMSPOTFINDER_H
//////////////////////////////
// Author: Brian Amadio
// Contact: btamadio@lbl.gov
// Main Algorithm: Determine the Beamspot position
//////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetBeamSpotFinder/IInDetBeamSpotTool.h" //for BeamSpot::Event
// #include "InDetBeamSpotVertex.h"
// #include "InDetBeamSpotRooFit.h"
#include "InDetBeamSpotFinder/BeamSpotStatusCode.h"
#include "xAODEventInfo/EventInfo.h" //typedef, can't fwd declare
#include "xAODTracking/VertexContainer.h" //typedef, can't fwd declare
#include "xAODTracking/TrackingPrimitives.h" //for xAOD::VxType
// #include "xAODTracking/Vertex.h"
#include "LumiBlockData/BunchCrossingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "BeamSpotID.h"
#include <string>
#include <vector>
#include <map>

class ITHistSvc;
class IToolSvc;
class IInDetBeamSpotTool;
class TTree;

namespace InDet {
  class InDetBeamSpotFinder : public AthAlgorithm {

  public:
    //Standard constructor and methods
    InDetBeamSpotFinder  (const std::string& name, ISvcLocator* pSvcLocator);
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    //Reorganize and clean up this section
    ServiceHandle<IToolSvc> m_toolSvc;
    ToolHandleArray<IInDetBeamSpotTool> m_beamSpotToolList;

    SG::ReadCondHandleKey<BunchCrossingCondData> m_bcDataKey
      {this, "BunchCrossingCondDataKey", "BunchCrossingData" ,"SG Key of BunchCrossing CDO"};

    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo
      {this, "EvtInfo", "EventInfo", "EventInfo name"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer
      {this, "VertexContainer", "PrimaryVertices", "Vertex container name"};

    //Beamspot sorting options
    unsigned int m_maxRunsPerFit;
    unsigned int m_maxEventsPerFit;
    unsigned int m_maxLBsPerFit;
    //Event Selection
    std::vector<unsigned int> m_BCIDsToAccept;
    //Job Options
    bool m_writeVertexNtuple;
    std::string m_beamSpotNtupleName;
    //Initialize some pointers
    ITHistSvc * m_thistSvc=0;
    TTree * m_root_bs=0;
    TTree * m_root_vrt=0;

    struct beamSpotNtuple_struct{
      int pileup{}, bcid{}, defectWord{}, fill{}, lbEnd{}, lbStart{};
      int nEvents{}, nValid{}, nVtxAll{}, nVtxPrim{};
      int run{}, separation{}, status{}, timeEnd{}, timeStart{}, runEnd{};
      std::map<std::string,double>paramMap{};
      std::map<std::string,double>covMap{};
    };

    struct vertexNtuple_struct{
      double x{},y{},z{},vxx{},vxy{},vyy{},vzz{};
      xAOD::VxType::VertexType vType{};
      unsigned int run{}, lb{}, bcid{}, pileup{}, nTracks{};
      unsigned long long eventNumber{}, eventTime{}, eventTime_NS{};
      bool passed{}, valid{};
    };

    beamSpotNtuple_struct m_beamSpotNtuple;
    vertexNtuple_struct m_root_vtx;
    BeamSpotStatusCode m_BeamStatusCode;
    
    std::string m_vertexTreeName; 
    std::vector<BeamSpot::Event> m_eventList;
    std::map< BeamSpot::ID, std::vector<BeamSpot::Event> > m_eventMap;
    std::vector<std::vector<BeamSpot::Event>> m_sortedEventList;
    std::vector<std::vector<BeamSpot::VrtHolder>> m_sortedVertices;

    bool passEventSelection(const xAOD::EventInfo &);
    BeamSpot::Event readEvent(const xAOD::EventInfo & , const xAOD::VertexContainer &);
    void writeToVertexTree(BeamSpot::Event &, BeamSpot::VrtHolder &);
    void writeToBeamSpotTree(const IInDetBeamSpotTool *bs, std::vector<BeamSpot::Event> &, std::vector<BeamSpot::VrtHolder> &);
    void sortEvents();
    StatusCode setupVertexTree();
    StatusCode setupBeamSpotTree();
    StatusCode performFits();
    bool iequals(const std::string &, const std::string &);
    int min_lb( std::vector<BeamSpot::Event> & );
    int max_lb( std::vector<BeamSpot::Event> & );
    int min_run( std::vector<BeamSpot::Event> & );
    int max_run( std::vector<BeamSpot::Event> & );

    void convertVtxTypeNames();

    // Parameters for vertex selection criteria
    int m_minTrackNum{};         //min Tracks in a vertex to be added
    int m_maxTrackNum{};         //max Tracks in a vertex to be added
    double m_maxChi2Vertex{};    //maxChi2 of vertex
    double m_maxTransverseError{}; // max transverse vertex resolution
    double m_minVtxProb{}; // probability cut on chi2/ndf
    unsigned int m_minVertexNum{};        //min vertex count for solution

    std::vector<std::string> m_vertexTypeNames{}; //names of vertexTypes
    std::vector<xAOD::VxType::VertexType> m_vertexTypes{};

    IInDetBeamSpotTool * cloneTool( int );
    bool passVertexSelection(const xAOD::Vertex * );    
    //For forcing a specific run number

    //    bool m_groupFitsByBCID; // each bcid is fitted separately
    bool m_useFilledBCIDsOnly{}; // Only use filled BCIDs not empty ones (data)
    std::string m_fitSortingKey{};

    bool m_writeAllVertices{};
    //std::map<unsigned int,long> m_nEvents;
    //std::map<BeamSpot::ID, long> m_nEvents;
   
    unsigned long m_secondsPerFit{}; 
   };
}//end namespace 

#endif

