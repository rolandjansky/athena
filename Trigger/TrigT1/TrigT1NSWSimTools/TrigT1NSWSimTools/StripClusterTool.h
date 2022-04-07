/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPCLUSTERTOOL_H
#define STRIPCLUSTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "TrigT1NSWSimTools/StripOfflineData.h"
#include "TrigT1NSWSimTools/IStripClusterTool.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/StripTdsOfflineTool.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "StripClusterOfflineData.h"

#include "TTree.h"
#include <functional>
#include <algorithm>
#include <map>
#include <utility>

//forward declarations
class IIncidentSvc;
class sTgcDigit;
class TTree;

namespace MuonGM {
  class MuonDetectorManager;
}


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short interface for the StripTDS tools
   *
   * This class implements the Strip Clustering offline simulation. It loops over the hits,
   * readout from the StripTDSOffLineTool
   *
   *  @author Jacob Searcy <jsearcy@cern.ch>
   *
   *
   */

  class StripClusterTool: virtual public IStripClusterTool,
                                  public AthAlgTool,
                                  public IIncidentListener {

  public:
    StripClusterTool(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
    virtual ~StripClusterTool()=default;
    virtual StatusCode initialize() override;
    virtual void handle (const Incident& inc) override;
    virtual StatusCode cluster_strip_data( std::vector<std::unique_ptr<StripData>>& strips,std::vector<std::unique_ptr<StripClusterData>>& clusters) override;

  private:
    // methods implementing the internal data processing
    StatusCode init_branches();                             //!< init the branches
    StatusCode book_branches();                             //!< book the branches to analyze the StripTds
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis
    StatusCode fill_strip_validation_id(std::vector<std::unique_ptr<StripClusterData>>& clusters,
                                        std::vector< std::shared_ptr<std::vector<std::unique_ptr<StripData> >>  > &cluster_cache);

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc > m_incidentSvc{this, "IncidentSvc", "IncidentSvc"};       //!< Athena/Gaudi incident Service
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    // analysis ntuple
    TTree* m_tree;                                          //!< ntuple for analysis
    Gaudi::Property<bool>         m_isMC            {this, "IsMC",                  true,       "This is MC"};
    Gaudi::Property<bool>         m_doNtuple        {this, "DoNtuple",              false,      "Input the sTGC strip cluster branches into the analysis ntuple"};
    SG::ReadHandleKey<MuonSimDataCollection> m_sTgcSdoContainerKey{this,"sTGC_SdoContainerName", "sTGC_SDO", "the name of the sTGC SDO container"};

    // analysis variable to be put into the ntuple
    int m_cl_n;                                             //!< number of STRIP hit delivered
    std::vector<int> *m_cl_charge;                          //!< charge of hit STRIPs
    std::vector<int> *m_cl_size;                            //!< charge of hit STRIPs
    std::vector<float> *m_cl_x;                             //!<global x position of cluster
    std::vector<float> *m_cl_y;                             //!<global y position of cluster
    std::vector<float> *m_cl_z;                             //!<global z position of cluster
    std::vector<float> *m_cl_lx;                            //!<global x position of cluster
    std::vector<float> *m_cl_ly;                            //!<global y position of cluster
    std::vector<float> *m_cl_lz;                            //!<global z position of cluster
    std::vector<float> *m_cl_ltgx;                          //!<global x position of cluster
    std::vector<float> *m_cl_ltgy;                          //!<global y position of cluster
    std::vector<float> *m_cl_ltgz;                          //!<global z position of cluster

    std::vector<float> *m_cl_truth_x;                       //!<global x of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_y;                       //!<global y of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_z;                       //!<global z of first truth hit for strip in cluster

    std::vector<float> *m_cl_truth_lx;                      //!<local x of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_ly;                      //!<local y of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_lz;                      //!<local z of first truth hit for strip in cluster

    std::vector<float> *m_cl_truth_E;                       //!<global energy of truth strip deposits
    std::vector<int> *m_cl_truth_n;                         //!<Cluster index for truth strip hits

    std::vector<int> *m_cl_side;
    std::vector<int> *m_cl_isSmall;
    std::vector<int> *m_cl_wedge;
    std::vector<int> *m_cl_sector;
    std::vector<int> *m_cl_module;
    std::vector<int> *m_cl_layer;
    std::vector<int> *m_cl_bandId;

    std::vector<int> *m_cl_phiId;
  };  // end of StripClusterTool class
} // namespace NSWL1
#endif
