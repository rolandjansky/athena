/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPSEGMENTTOOL_H
#define STRIPSEGMENTTOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/Property.h"

//Muon RDOs for trigger EDM
#include "MuonRDO/NSW_TrigRawDataContainer.h"


//To access detector envelope
#include "RegSelLUT/IRegionIDLUT_Creator.h"

//local includes
#include "TrigT1NSWSimTools/IStripSegmentTool.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"

//forward declarations
class IIncidentSvc;
class TTree;

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

  class StripSegmentTool: virtual public IStripSegmentTool, 
                                   public AthAlgTool, 
                                   public IIncidentListener {

  public:
    enum cStatus {OK, FILL_ERROR, CLEARED};

    StripSegmentTool(const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
    virtual ~StripSegmentTool();
    virtual StatusCode initialize();
    virtual void handle (const Incident& inc);
    StatusCode find_segments(std::vector< std::unique_ptr<StripClusterData> >& clusters);

    
  private:
        // methods implementing the internal data processing

        StatusCode book_branches();                             //!< book the branches to analyze the StripTds behavior                                                                                           
        void reset_ntuple_variables();                          //!< reset the variables used in the analysis ntuple                                                                                              
        void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple                                                                                              
        
        // needed Services, Tools and Helpers
        ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service

        // analysis ntuple
        TTree* m_tree;                                          //!< ntuple for analysis
        BooleanProperty  m_doNtuple;                            //!< property, see @link StripTdsOfflineTool::StripTdsOfflineTool @endlink                         
        StringProperty   m_sTgcSdoContainer;                    //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink                          
        // analysis variable to be put into the ntuple
        int m_seg_n;                                            //!< number of Segments found
        std::vector<int> *m_seg_wedge1_size;                        //!< theta
        std::vector<int> *m_seg_wedge2_size;                        //!< theta
        std::vector<float> *m_seg_theta;                        //!< theta
        std::vector<float> *m_seg_dtheta;                      //!< delta theta
        std::vector<uint8_t> *m_seg_dtheta_int; 
        std::vector<float> *m_seg_eta;                          //!< m_seg_eta
        std::vector<float> *m_seg_eta_inf;  
        std::vector<float> *m_seg_phi;
        std::vector<int> *m_seg_bandId;
        std::vector<int> *m_seg_secId;
        std::vector<int> *m_seg_bcId;
        std::vector<int> *m_seg_phiId;
        std::vector<int> *m_seg_rIdx;
        std::vector<float> *m_seg_global_r;
        std::vector<float> *m_seg_global_x;
        std::vector<float> *m_seg_global_y;
        std::vector<float> *m_seg_global_z;
        std::vector<float> *m_seg_dir_r;
        std::vector<float> *m_seg_dir_y;
        std::vector<float> *m_seg_dir_z; 

        int m_rIndexBits;
        int m_dThetaBits;
        StatusCode FetchDetectorEnvelope();
        std::pair<float,float> m_zbounds;
        std::pair<float,float> m_etabounds;
        std::pair<float,float> m_rbounds;
        uint8_t findRIdx(const float&);
        uint8_t findDtheta(const float&);
        int m_ridxScheme;
        float m_dtheta_min;
        float m_dtheta_max;          
        
    protected:
        SG::WriteHandleKey<Muon::NSW_TrigRawDataContainer> m_trigRdoContainer;
        ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolsTGC;
            
         
     
    
  };  // end of StripSegmentTool class
    
} // namespace NSWL1

#endif
