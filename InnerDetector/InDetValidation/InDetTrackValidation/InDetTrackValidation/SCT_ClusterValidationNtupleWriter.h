/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// SCT_ClusterValidationNtupleWriter.h
///////////////////////////////////////////////////////////////////

#ifndef INDET_SCT_CLUSTERVALIDATIONNTUPLEWRITER_H
#define INDET_SCT_CLUSTERVALIDATIONNTUPLEWRITER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"  // typedef
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODEventInfo/EventInfo.h"

#include <set>
#include <string>
#include <vector>

class SCT_ID;
class TTree;

namespace InDet {

  /** @class SCT_ClusterValidationNtupleWriter
 
      Validation Algorithm to retrieve a SCT_ClusterContainer, loop over the entries and fill
      the validation ntuple with RIO specific entries.
   
      @author Sebastian.Fleischmann@cern.ch 
      @author Peter.Vankov@cern.ch
  */

  class SCT_ClusterValidationNtupleWriter : public AthAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    SCT_ClusterValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~SCT_ClusterValidationNtupleWriter() = default;

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode execute() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;

  private:

    const SCT_ID* m_sctid; //!< SCT ID helper
    const InDet::SCT_ClusterContainer* m_riocontainer; //!< container of RIOs
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "key for xAOD::EventInfo"};
    SG::ReadHandleKey<SCT_ClusterContainer> m_jo_riocontainername{this, "SCT_ClusterContainer", "SCT_Clusters", "jobOption: name of container with RIOs"};
    SG::ReadHandleKey<SCT_RDO_Container> m_dataObjectName{this, "SCT_RDOContainer", "SCT_RDOs", "Data object name: for the SCT this is SCT_RDOs"};
    SG::ReadHandleKey<SpacePointContainer> m_spacePointContainerName{this, "SCT_SpacePointContainer", "SCT_SpacePoints", "SpacePoint container name: for the SCT this is SCT_SpacePoints"};
    SG::ReadHandleKey<TrackCollection> m_inputTrackCollection{this, "SCT_InputTrackCollection", "CombinedInDetTracks", "TrackCollection name, needed for hits-on-tracks, default is CombinedInDetTracks"};

    // For P->T converter of SCT_Clusters
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

    ToolHandle<ISCT_ByteStreamErrorsTool> m_byteStreamErrTool{this, "ByteStreamErrTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
    ToolHandle<ISCT_CablingTool> m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
    StringProperty m_ntupleFileName{this, "NtupleFileName", "/NTUPLES/FILE1"};     //!< jobOption: Ntuple file name
    StringProperty m_ntupleDirName{this, "NtupleDirectoryName", "FitterValidation"};      //!< jobOption: Ntuple directory name
    StringProperty m_ntupleTreeName{this, "NtupleTreeName", "RIOs"};     //!< jobOption: Ntuple tree name
    BooleanProperty m_fillCluster{this, "FillCluster", false};        //!< flag to book, access and fill Cluster or not
    BooleanProperty m_fillRDO{this, "FillRDO", true};            //!< flag to book, access and fill RDO or not
    BooleanProperty m_fillSpacePoint{this, "FillSpacePoint", false};     //!< flag to book, access and fill SpacePoints or not
    BooleanProperty m_fillBSErrs{this, "FillBSErrs", true};         //!< flag to book, access and fill ByteStream errors or not
    BooleanProperty m_doHitsOnTracks{this, "DoHitsOnTracks", false};     //!< flag to book, access and fill RDO isOnTrack or not

    //! pointer to the ntuple tree.
    TTree* m_nt;

    // --- ntuple items. Pls describe here to allow copying to wiki+doxygen ---
    int         m_runNumber;       //!< run number
    int         m_eventNumber;     //!< event number for current PRD vector
    int         m_lumiBlock;       //!< event lumi block information
    int         m_timeStamp;       //!< event time stamp
    int         m_bunchCrossing;   //!< event bunch crossing ID

    // ntuple items for SCT clusters
    int         m_nRIOs;              //!< number of RIOs, i.e. SCT clusters, in the event. This variable set the size of the vectors with cluster information.
    std::vector<float>* m_rioLoc1;    //!< local x coordinate of strip UNIT:mm
    std::vector<float>* m_rioSurfaceX; //!< global x coordinate of SCT wafer center UNIT:mm
    std::vector<float>* m_rioSurfaceY; //!< global x coordinate of SCT wafer center UNIT:mm
    std::vector<float>* m_rioSurfaceZ; //!< global x coordinate of SCT wafer center UNIT:mm
    std::vector<int>*   m_SctBarrelEndcap; //!< from IdHelper: pos/neg barrel (+/-1) and pos/neg endcap (+/-2) UNIT:1:bar, 2:ec
    std::vector<int>*   m_SctLayerDisk; //!< from IdHelper: layer/disk index
    std::vector<int>*   m_SctEtaModule; //!< from IdHelper: module index in eta
    std::vector<int>*   m_SctPhiModule; //!< from IdHelper: module index in phi
    std::vector<int>*   m_SctSide;      //!< from IdHelper: side index (0/1)
    std::vector<float>* m_SctDeltaPhi;  //!< please provide description!
    std::vector<float>* m_SctHitErr;    //!< please provide description!

    //SpacePoints
    int m_nSP;
    std::vector<int>* m_sp_bec;          //!< Barrel or endcap index  (barrel: 0, endcap: +/-2) for SpacePoint
    std::vector<int>* m_sp_layer;        //!< index of the SCT-layer in which the SCT wafer is positioned for SpacePoint
    std::vector<int>* m_sp_eta;          //!< eta index of the SCT-layer in which the SCT wafer is positioned for SpacePoint
    std::vector<int>* m_sp_phi;          //!< phi index of the SCT-layer in which the SCT wafer is positioned for SpacePoint
    std::vector<float>* m_sp_x;          //!< x position of SpacePoint in ATLAS coordinate system (mm)
    std::vector<float>* m_sp_y;          //!< y position of SpacePoint in ATLAS coordinate system (mm)
    std::vector<float>* m_sp_z;          //!< z position of SpacePoint in ATLAS coordinate system (mm)
    
    //  RDO
    int  m_nRDOs;                         //!< Number of RDOs
    std::vector<int>* m_sct_rdoGroupSize; //!< RDO group size 
    std::vector<int>* m_sct_rdoIsOnTrack; //!< RDO on track 
    std::vector<int>* m_sct_layer;        //!< index of the SCT-layer in which the SCT wafer is positioned
    std::vector<int>* m_sct_eta;          //!< eta of the SCT wafer
    std::vector<int>* m_sct_phi;          //!< phi of the SCT wafer
    std::vector<int>* m_sct_side;         //!< side of the SCT wafer (0 or 1) within the SCT module
    std::vector<int>* m_sct_barrelec;     //!< barrel/endcap index (barrel: 0, endcap: +/-2) 
    std::vector<int>* m_sct_firstStrip;   //!< index (could be from 0 to 767) of the fired strip within the SCT wafer
    std::vector<int>* m_sct_numHitsInWafer; //!< number of hits in the SCT wafer
    std::vector<int>* m_sct_waferHash;    //!< index (could be from 0 to 8175) of the fired SCT wafer 
    std::vector<int>* m_sct_tbin;         //!< timebin of the RDO
    std::vector<int>* m_sct_rodid;        //!< ROD ID of the RDO
    std::vector<int>* m_sct_channel;      //!< channel within ROD of the RDO

    /* ByteStream errors: these variables represent a bit set in the header or trailer for a
       link (i.e. SCT module side) in the Bytestream, indicating that the ROD found
       something wrong for that event for that link. In general, the ROD expects to
       get from each link for each event: [link header] [zero or more hits] [link trailer].
       These errors can either be a bit in the header or the trailer, or they can take the place
       of a hit (for the ABCD or Raw) errors. */
    int               m_totalNumErrors;   //!< total number of BS errors.
    std::vector<int>* m_scterr_bec;       //!< barrel/endcap index (barrel: 0, endcap: +/-2) 
    std::vector<int>* m_scterr_layer;     //!< layer (barrel) or disk (endcap) of wafer with the error
    std::vector<int>* m_scterr_eta;       //!< eta of the SCT wafer
    std::vector<int>* m_scterr_phi;       //!< phi of the SCT wafer 
    std::vector<int>* m_scterr_side;      //!< side of the SCT wafer (0 or 1) within the module
    std::vector<int>* m_scterr_rodid;     //!< online ID of ROD containing link   
    std::vector<int>* m_scterr_channel;   //!< online channel no. of link.
    std::vector<int>* m_scterr_type;      //!< type of BS error (as defined in SCT_ByteStreamErrs enum in ISCT_ByteStreamErrorsSvc.h)

  };

} // close of namespace

#endif // INDET_SCT_CLUSTERVALIDATIONNTUPLEWRITER_H
