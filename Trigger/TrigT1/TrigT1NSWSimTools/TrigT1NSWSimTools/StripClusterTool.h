/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPCLUSTERTOOL_H
#define STRIPCLUSTERTOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/Property.h"

//local includes
#include "TrigT1NSWSimTools/IStripClusterTool.h"
#include "StripClusterOfflineData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/StripTdsOfflineTool.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"


//forward declarations
class IIncidentSvc;
class sTgcIdHelper;
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
    enum cStatus {OK, FILL_ERROR, CLEARED};

    StripClusterTool(const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
    virtual ~StripClusterTool();
    virtual StatusCode initialize();
    virtual void handle (const Incident& inc);
    StatusCode cluster_strip_data( std::vector<std::unique_ptr<StripData>>& strips,std::vector<std::unique_ptr<StripClusterData>>& clusters);
  private:
    // methods implementing the internal data processing

    StatusCode book_branches();                             //!< book the branches to analyze the StripTds behavior                                                                                           
    void reset_ntuple_variables();                          //!< reset the variables used in the analysis ntuple                                                                                              
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple                                                                                              
    void fill_strip_validation_id(std::vector<std::unique_ptr<StripClusterData>>& clusters);  //!< fill the ntuple branch for the StripTdsOffline   
    bool MatchModule(const std::unique_ptr<StripData>& one, const StripData* two);
    

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    const sTgcIdHelper*                m_sTgcIdHelper;      //!< sTgc offline Id helper

    // analysis ntuple
    TTree* m_tree;                                          //!< ntuple for analysis
    BooleanProperty  m_doNtuple;                            //!< property, see @link StripTdsOfflineTool::StripTdsOfflineTool @endlink                         
    StringProperty   m_sTgcSdoContainer;                    //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink                          
    // analysis variable to be put into the ntuple
    int m_cl_n;                                            //!< number of STRIP hit delivered
    std::vector<int> *m_cl_charge;                           //!< charge of hit STRIPs 
    std::vector<int> *m_cl_size;                           //!< charge of hit STRIPs 
    std::vector<float> *m_cl_x;                           //!<global x  position of cluster
    std::vector<float> *m_cl_y;                           //!<global y  position of cluster
    std::vector<float> *m_cl_z;                           //!<global z  position of cluster
    std::vector<float> *m_cl_lx;                           //!<global x  position of cluster
    std::vector<float> *m_cl_ly;                           //!<global y  position of cluster
    std::vector<float> *m_cl_lz;                           //!<global z  position of cluster
    std::vector<float> *m_cl_ltgx;                           //!<global x  position of cluster
    std::vector<float> *m_cl_ltgy;                           //!<global y  position of cluster
    std::vector<float> *m_cl_ltgz;                           //!<global z  position of cluster

    std::vector<float> *m_cl_truth_x;                           //!<global x  of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_y;                           //!<global y  of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_z;                           //!<global z  of first truth hit for strip in cluster

    std::vector<float> *m_cl_truth_lx;                           //!<global x  of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_ly;                           //!<global y  of first truth hit for strip in cluster
    std::vector<float> *m_cl_truth_lz;                           //!<global z  of first truth hit for strip in cluster


    std::vector<float> *m_cl_truth_E;                           //!<global energy of truth strip deposits
    std::vector<int> *m_cl_truth_n;                           //!<Cluster index for truth strip hits

    std::vector<int> *m_cl_side;
    std::vector<int> *m_cl_isSmall;
    std::vector<int> *m_cl_wedge;
    std::vector<int> *m_cl_sector;
    std::vector<int> *m_cl_module;
    std::vector<int> *m_cl_layer;
    std::vector<int> *m_cl_bandId;

    std::vector< std::shared_ptr<std::vector<std::unique_ptr<StripData> >>  > m_clusters;
  };  // end of StripClusterTool class

} // namespace NSWL1

#endif
