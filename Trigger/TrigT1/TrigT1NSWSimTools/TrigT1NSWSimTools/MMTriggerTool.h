/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMTRIGGERTOOL_H
#define MMTRIGGERTOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "MuonIdHelpers/MmIdHelper.h"

//local includes
#include "TrigT1NSWSimTools/IMMTriggerTool.h"
#include "TrigT1NSWSimTools/MMLoadVariables.h"
#include "TrigT1NSWSimTools/MMT_Finder.h"
#include "TrigT1NSWSimTools/MMT_Fitter.h"
#include "TrigT1NSWSimTools/MMT_Diamond.h"
#include "TTree.h"

#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonDigitContainer/MmDigit.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecordCollection.h"

namespace MuonGM {
  class MuonDetectorManager;
}


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  class MMTriggerTool: virtual public IMMTriggerTool,
    public AthAlgTool,
    public IIncidentListener {

  public:

    //load event stuff
    std::vector<hitData_entry> event_hitDatas(int find_event, std::map<hitData_key,hitData_entry>& Hits_Data_Set_Time) const;
    std::vector<hitData_key> event_hitData_keys(int find_event, std::map<hitData_key,hitData_entry>& Hits_Data_Set_Time) const;

    //MMT_Loader stuff end

    MMTriggerTool(const std::string& type, const std::string& name, const IInterface* parent);

    virtual ~MMTriggerTool()=default;

    virtual StatusCode initialize();

    virtual void handle (const Incident& inc);

    StatusCode runTrigger(Muon::NSW_TrigRawDataContainer* rdo, const bool do_MMDiamonds) const;

  private:

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc > m_incidentSvc{this, "IncidentSvc", "IncidentSvc"};       //!< Athena/Gaudi incident Service

    // read data handle
    SG::ReadHandleKey<McEventCollection> m_keyMcEventCollection{this,"McEventCollection","TruthEvent","Location of TruthEvent"};
    SG::ReadHandleKey<TrackRecordCollection> m_keyMuonEntryLayer{this,"MuonEntryLayer","MuonEntryLayer","Location of MuonEntryLayer"};
    SG::ReadHandleKey<MmDigitContainer> m_keyMmDigitContainer{this,"MmDigitContainer","MM_DIGITS","Location of MmDigitContainer"};
    Gaudi::Property<bool>  m_isMC            {this, "IsMC",         true, "This is MC"};

    // Parameters for Diamond Road algorithms
    Gaudi::Property<bool>  m_trapShape            {this, "TrapezoidalShape",         true, "Consider the quadruplet as a trapezoid"};
    Gaudi::Property<int>   m_diamRoadSize         {this, "DiamondRoadSize",          8,    "Number of strips to create a road"};
    Gaudi::Property<bool>  m_uv                   {this, "DiamondUV",                true, "Include Stereo planes for tracking"};
    Gaudi::Property<int>   m_diamXthreshold       {this, "DiamondEtaThreshold",      2,    "Number of Eta planes for coincidences"};
    Gaudi::Property<int>   m_diamUVthreshold      {this, "DiamondStereoThreshold",   2,    "Number of Stereo planes for coincidences"};
    Gaudi::Property<int>   m_diamOverlapEtaUp     {this, "DiamondEtaUpOverlap",      4,    "Number of Eta strips for upper road overlap"};
    Gaudi::Property<int>   m_diamOverlapEtaDown   {this, "DiamondEtaDownOverlap",    0,    "Number of Eta strips for lower road overlap"};
    Gaudi::Property<int>   m_diamOverlapStereoUp  {this, "DiamondStereoUpOverlap",   4,    "Number of Stereo strips for upper road overlap"};
    Gaudi::Property<int>   m_diamOverlapStereoDown{this, "DiamondStereoDownOverlap", 0,    "Number of Stereo strips for lower road overlap"};

    // Parameters for RDO encoding
    Gaudi::Property<std::string>  m_mmDigitContainer{this, "MM_DigitContainerName", "MM_DIGITS", "Name of the MM digit container"};
    Gaudi::Property<bool>         m_doNtuple        {this, "DoNtuple",   false,          "Input the MMStrip branches into the analysis ntuple"};
    Gaudi::Property<float>        m_phiMin          {this, "PhiMin",    -16.*M_PI/180.0, "Minimum Phi"};
    Gaudi::Property<float>        m_phiMax          {this, "PhiMax",     16.*M_PI/180.0, "Maximum Phi"};
    Gaudi::Property<int>          m_phiBits         {this, "PhiBits",    6,              "Number of Phi bits"};
    Gaudi::Property<float>        m_rMin            {this, "RMin",       900.0,          "Minimum R [mm]"};
    Gaudi::Property<float>        m_rMax            {this, "RMax",       5000.0,         "Maximum R [mm]"};
    Gaudi::Property<int>          m_rBits           {this, "RBits",      8,              "Number of R bits"};
    Gaudi::Property<float>        m_dThetaMin       {this, "DThetaMin", -0.015,          "Minimum dTheta [rad]"};
    Gaudi::Property<float>        m_dThetaMax       {this, "DThetaMax",  0.015,          "Maximum dTheta [rad]"};
    Gaudi::Property<int>          m_dThetaBits      {this, "DThetaBits", 5,              "Number of dTheta bits"};

    std::shared_ptr<MMT_Parameters> m_par_large;
    std::shared_ptr<MMT_Parameters> m_par_small;
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    const MmIdHelper*                  m_MmIdHelper;        //!< MM offline Id helper

    //Histogram
    StatusCode book_branches();                       //!< book the branches
    void clear_ntuple_variables();                    //!< clear the variables used in the analysis ntuple
    void fillNtuple(const histogramDigitVariables& histDigVars) const;

    /* None of the TTree filling is thread-safe and should really be refactored.
     * But we check in initialize() that this is only used in single-threaded mode.
     */
    TTree* m_tree ATLAS_THREAD_SAFE;                                          //!< ntuple for analysis
    std::vector<unsigned int>* m_trigger_diamond_ntrig ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_diamond_bc ATLAS_THREAD_SAFE;
    std::vector<char>* m_trigger_diamond_sector ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_diamond_stationPhi ATLAS_THREAD_SAFE;
    std::vector<unsigned int>* m_trigger_diamond_totalCount ATLAS_THREAD_SAFE;
    std::vector<unsigned int>* m_trigger_diamond_realCount ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_diamond_iX ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_diamond_iU ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_diamond_iV ATLAS_THREAD_SAFE;
    std::vector<unsigned int>* m_trigger_diamond_XbkgCount ATLAS_THREAD_SAFE;
    std::vector<unsigned int>* m_trigger_diamond_UVbkgCount ATLAS_THREAD_SAFE;
    std::vector<unsigned int>* m_trigger_diamond_XmuonCount ATLAS_THREAD_SAFE;
    std::vector<unsigned int>* m_trigger_diamond_UVmuonCount ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_age ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_mx ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_my ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_Uavg ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_Vavg ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_mxl ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_theta ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_eta ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_dtheta ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_phi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_diamond_phiShf ATLAS_THREAD_SAFE;
    std::vector<uint8_t>* m_trigger_diamond_TP_phi_id ATLAS_THREAD_SAFE;
    std::vector<uint8_t>* m_trigger_diamond_TP_R_id ATLAS_THREAD_SAFE;
    std::vector<uint8_t>* m_trigger_diamond_TP_dTheta_id ATLAS_THREAD_SAFE;

    std::vector<double>* m_trigger_RZslopes ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_fitThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_fitPhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_fitDth ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_trueEtaRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_truePtRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_fitEtaRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_fitPtRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_resThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_resPhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_resDth ATLAS_THREAD_SAFE;

    std::vector<double>* m_trigger_large_fitThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_fitPhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_fitDth ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_trueEtaRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_truePtRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_trueThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_truePhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_trueDth ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_fitEtaRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_fitPtRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_resThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_resPhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_large_resDth ATLAS_THREAD_SAFE;

    std::vector<double>* m_trigger_small_fitThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_fitPhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_fitDth ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_trueEtaRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_truePtRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_trueThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_truePhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_trueDth ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_fitEtaRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_fitPtRange ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_resThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_resPhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_small_resDth ATLAS_THREAD_SAFE;

    std::vector<int>* m_trigger_VMM ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_plane ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_station ATLAS_THREAD_SAFE;
    std::vector<int>* m_trigger_strip ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_slope ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_trueThe ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_truePhi ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_trueDth ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_mxl ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_mx ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_my ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_mu ATLAS_THREAD_SAFE;
    std::vector<double>* m_trigger_mv ATLAS_THREAD_SAFE;

    std::vector<std::string> *m_NSWMM_dig_stationName ATLAS_THREAD_SAFE;
    std::vector<int> *m_NSWMM_dig_stationEta ATLAS_THREAD_SAFE;
    std::vector<int> *m_NSWMM_dig_stationPhi ATLAS_THREAD_SAFE;
    std::vector<int> *m_NSWMM_dig_multiplet ATLAS_THREAD_SAFE;
    std::vector<int> *m_NSWMM_dig_gas_gap ATLAS_THREAD_SAFE;
    std::vector<int> *m_NSWMM_dig_channel ATLAS_THREAD_SAFE;

    std::vector< std::vector<float> >  *m_NSWMM_dig_time ATLAS_THREAD_SAFE;
    std::vector< std::vector<float> >  *m_NSWMM_dig_charge ATLAS_THREAD_SAFE;
    std::vector< std::vector<int> >    *m_NSWMM_dig_stripPosition ATLAS_THREAD_SAFE;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripLposX ATLAS_THREAD_SAFE;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripLposY ATLAS_THREAD_SAFE;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposX ATLAS_THREAD_SAFE;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposY ATLAS_THREAD_SAFE;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposZ ATLAS_THREAD_SAFE;
  };  // end of MMTriggerTool class
} // namespace NSWL1
#endif
