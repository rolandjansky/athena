/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETBEAMSPOTFINDER_H
#define INDET_INDETBEAMSPOTFINDER_H
//////////////////////////////
// Author jwalder@cern.ch
// Main Algorithm: Determine the Beamspot position
//////////////////////////////


#include <string>
#include <vector>
#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "InDetBeamSpotVertex.h"
#include "InDetBeamSpotRooFit.h"
#include "InDetBeamSpotFinder/BeamSpotStatusCode.h" 
#include "VxVertex/VxContainer.h"
#include "TTree.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/Vertex.h"

class ITHistSvc;
class ToolSvc;

namespace BeamSpot {
  class ID;
}

namespace InDet {
  /** Main algorithm to run the specified beamspot determination algorithm, and 
      output results to DB. This class is the main class to determine the beamspot
      using a particular algorithm and to pass the results to be stored in a database (default and sqlite file).
   */
  class InDetBeamSpotFinder : public AthAlgorithm {
    
  public:
    //Standard constructor and methods
    InDetBeamSpotFinder  (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
    /** From the status-word of the beamspot return a string corresponding to the algorithm's name. */
    static std::string getAlgorithmName( int status);
    /** From the status-word of the beamspot return a string corresponding to the algorithm's fitStatus. */
    static std::string getFitStatusName(int status); 
    /** From the status-word of the beamspot return a string corresponding to the algorithm's
	method of determining the beamspot width. */
    static std::string getWidthName(int status);

    BeamSpotStatusCode m_BeamStatusCode; //!< Simple struct containing information about the fit-status.

    /** Print information for a particular beamspot. */
    void printBeamspot(const BeamSpot::ID*, const IInDetBeamSpotTool*);
  private:
    ServiceHandle<IToolSvc> m_toolSvc;

    ToolHandleArray<IInDetBeamSpotTool> m_beamSpotToolList;
    //ToolHandle<IInDetBeamSpotWriterTool> m_beamSpotWriterTool;
    ToolHandle<Trig::IBunchCrossingTool> m_bcTool; ///< Handle to the BC tool

    std::map<BeamSpot::ID, IInDetBeamSpotTool *> m_beamspots;
    int m_maxCount; //max events per beamspot calculation
    int m_lumiRange; //luminosity range
    int m_runRange;  // runRange
    int m_eventRange; //eventRange
    int m_skipNevents; // skip every N number of events
    int m_eventCount;
    //std::map<unsigned int,long> m_vertexCountPrim;
    //std::map<unsigned int,long> m_vertexCountAll;

    std::map<BeamSpot::ID,long> m_vertexCountPrim;
    std::map<BeamSpot::ID,long> m_vertexCountAll;

    /*static*/ BeamSpot::VrtHolder m_root_vtx;
    /** Use of specific luminosity blocks */
    bool m_useLumiBlocks; 
    /** Range of pairs of [start,end) lumi ranges */
    std::vector<std::pair< int, int> > m_lumiBlockRanges;

    /** List of accepted BCID values */
    std::vector<unsigned int> m_bcid_val;

    bool m_writeDb; //write to database
    bool m_useNearestBS; //if failed beamspot, use nearest neighbour instead?
    // ----- Histogramming ----- //
    bool m_doHists; //do histogramming?
    bool m_VertexNtuple;//Write Vertex Ntuple?
    ITHistSvc * m_thistSvc;
    std::string m_root_beamspotName;
    TTree * m_root_bs;

    /** Interal storage of IOV and fitstatus for ROOT plots
     */
    struct beamSpotNtuple_struct{
      int bcid;
      int defectWord;
      int fill;
      int lbEnd;
      int lbStart;
      int nEvents;
      int nValid;
      int nVtxAll;
      int nVtxPrim;
      int run;
      int separation;
      int status;
      int timeEnd;
      int timeStart;
      int runEnd;
      std::map<std::string,double>paramMap;
      std::map<std::string,double>covMap;
    };

    beamSpotNtuple_struct m_beamSpotNtuple;
    void setupBeamspotTree();
    void setupVertexTree();
    void setRootValues(const IInDetBeamSpotTool * );
    void addExtraBranches(const IInDetBeamSpotTool *);
    TTree * m_root_vrt;
    bool addVertex(const xAOD::Vertex * , BeamSpot::ID  );
    void addToVertexTree(const xAOD::Vertex *, const xAOD::EventInfo &);
    void convertVtxTypeNames();

    std::string m_root_vtxName; // vertex tree name in root
    unsigned int m_bcid; 
    //unsigned int m_root_event[4];
    struct vrtPar {
      int nTracks, passed;
    };
    /*static*/ vrtPar m_root_par;
    std::vector<BeamSpot::VrtHolder> m_vertexDataVec;
    
    std::map<BeamSpot::ID, std::vector<BeamSpot::VrtHolder> > m_vertexDataMap;

    // Parameters for vertex selection criteria
    int m_minTrackNum;         //min Tracks in a vertex to be added
    int m_maxTrackNum;         //max Tracks in a vertex to be added
    double m_maxChi2Vertex;    //maxChi2 of vertex
    double m_maxTransverseErr; // max transverse vertex resolution
    double m_minVtxProb; // probability cut on chi2/ndf
    unsigned int m_minVertexNum;        //min vertex count for solution

    std::string m_containerName;

    std::vector<std::string> m_vertexTypeNames; //names of vertexTypes
    std::vector<xAOD::VxType::VertexType> m_vertexTypes;

    bool m_useDefaults; //use default values if fit returns a 0 
    double m_def_sigmaX, m_def_sigmaY,m_def_sigmaZ;
    double m_def_posX, m_def_posY,m_def_posZ;
    double m_def_tiltX, m_def_tiltY;
    double m_def_sigmaXY;

    IInDetBeamSpotTool * cloneTool( int );
    bool applySelection(const xAOD::Vertex * );    
    //For forcing a specific run number
    bool m_useForcedRun;
    unsigned int m_forcedRunNumber, m_forcedRunRange; 
    int m_lbRangeOffset;

    bool m_setLBwithViewedEvents;
    bool m_setLBwithAcceptedEvents;

    bool m_separateByBCID; // each bcid is fitted separately
    bool m_useFilledBCIDsOnly; // Only use filled BCIDs not empty ones (data)
    bool m_writeAllVertices;
    //std::map<unsigned int,long> m_nEvents;
    std::map<BeamSpot::ID, long> m_nEvents;
   };
    
}//end namespace 

#endif

