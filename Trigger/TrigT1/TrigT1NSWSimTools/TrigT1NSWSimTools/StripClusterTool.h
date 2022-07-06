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
    virtual StatusCode cluster_strip_data( std::vector<std::unique_ptr<StripData>>& strips,std::vector<std::unique_ptr<StripClusterData>>& clusters) const override;

  private:
    // methods implementing the internal data processing
    StatusCode init_branches();                             //!< init the branches
    StatusCode book_branches();                             //!< book the branches to analyze the StripTds
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis
    StatusCode fill_strip_validation_id(std::vector<std::unique_ptr<StripClusterData>>& clusters,
                                        std::vector< std::shared_ptr<std::vector<std::unique_ptr<StripData> >>  > &cluster_cache) const;

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc > m_incidentSvc{this, "IncidentSvc", "IncidentSvc"};       //!< Athena/Gaudi incident Service
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    Gaudi::Property<bool>         m_isMC            {this, "IsMC",                  true,       "This is MC"};
    Gaudi::Property<bool>         m_doNtuple        {this, "DoNtuple",              false,      "Input the sTGC strip cluster branches into the analysis ntuple"};
    SG::ReadHandleKey<MuonSimDataCollection> m_sTgcSdoContainerKey{this,"sTGC_SdoContainerName", "sTGC_SDO", "the name of the sTGC SDO container"};

    /* None of the TTree filling is thread-safe and should really be refactored.
     * But we check in initialize() that this is only used in single-threaded mode.
     */
    TTree* m_tree{nullptr};                                 //!< ntuple for analysis
    mutable int m_cl_n ATLAS_THREAD_SAFE;                   //!< number of STRIP hit delivered
    std::vector<int> *m_cl_charge ATLAS_THREAD_SAFE{nullptr};                          //!< charge of hit STRIPs
    std::vector<int> *m_cl_size ATLAS_THREAD_SAFE{nullptr};                            //!< charge of hit STRIPs
    std::vector<float> *m_cl_x ATLAS_THREAD_SAFE{nullptr};                             //!<global x position of cluster
    std::vector<float> *m_cl_y ATLAS_THREAD_SAFE{nullptr};                             //!<global y position of cluster
    std::vector<float> *m_cl_z ATLAS_THREAD_SAFE{nullptr};                             //!<global z position of cluster
    std::vector<float> *m_cl_lx ATLAS_THREAD_SAFE{nullptr};                            //!<global x position of cluster
    std::vector<float> *m_cl_ly ATLAS_THREAD_SAFE{nullptr};                            //!<global y position of cluster
    std::vector<float> *m_cl_lz ATLAS_THREAD_SAFE{nullptr};                            //!<global z position of cluster
    std::vector<float> *m_cl_ltgx ATLAS_THREAD_SAFE{nullptr};                          //!<global x position of cluster
    std::vector<float> *m_cl_ltgy ATLAS_THREAD_SAFE{nullptr};                          //!<global y position of cluster
    std::vector<float> *m_cl_ltgz ATLAS_THREAD_SAFE{nullptr};                          //!<global z position of cluster

    std::vector<float> *m_cl_truth_x ATLAS_THREAD_SAFE{nullptr};                       //!<global x of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_y ATLAS_THREAD_SAFE{nullptr};                       //!<global y of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_z ATLAS_THREAD_SAFE{nullptr};                       //!<global z of first truth hit for strip in cluster

    std::vector<float> *m_cl_truth_lx ATLAS_THREAD_SAFE{nullptr};                      //!<local x of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_ly ATLAS_THREAD_SAFE{nullptr};                      //!<local y of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_lz ATLAS_THREAD_SAFE{nullptr};                      //!<local z of first truth hit for strip in cluster

    std::vector<float> *m_cl_truth_E ATLAS_THREAD_SAFE{nullptr};                       //!<global energy of truth strip deposits
    std::vector<int> *m_cl_truth_n ATLAS_THREAD_SAFE{nullptr};                         //!<Cluster index for truth strip hits

    std::vector<int> *m_cl_side ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_cl_isSmall ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_cl_wedge ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_cl_sector ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_cl_module ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_cl_layer ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_cl_bandId ATLAS_THREAD_SAFE{nullptr};

    std::vector<int> *m_cl_phiId ATLAS_THREAD_SAFE{nullptr};
  };  // end of StripClusterTool class
} // namespace NSWL1
#endif
