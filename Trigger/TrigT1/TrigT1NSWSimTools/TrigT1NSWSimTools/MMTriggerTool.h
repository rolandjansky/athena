/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMTRIGGERTOOL_H
#define MMTRIGGERTOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IIncidentListener.h"

//local includes
#include "TrigT1NSWSimTools/IMMTriggerTool.h"
#include "MMT_Diamond.h"

//forward declarations
class IIncidentSvc;
class MmIdHelper;
class MmDigit;
class TTree;

#include "MMLoadVariables.h"
#include "MuonDigitContainer/MmDigitContainer.h"

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

    virtual ~MMTriggerTool();

    virtual StatusCode initialize();

    virtual void handle (const Incident& inc);

    StatusCode runTrigger(Muon::NSW_TrigRawDataContainer* rdo, const bool do_MMDiamonds);

  private:

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc > m_incidentSvc{this, "IncidentSvc", "IncidentSvc"};       //!< Athena/Gaudi incident Service

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
    StatusCode book_branches();                             //!< book the branches
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple
    void fillNtuple(const MMLoadVariables& loadedVariables);

    TTree* m_tree;                                          //!< ntuple for analysis
    std::vector<unsigned int>* m_trigger_diamond_ntrig;
    std::vector<int>* m_trigger_diamond_bc;
    std::vector<char>* m_trigger_diamond_sector;
    std::vector<int>* m_trigger_diamond_stationPhi;
    std::vector<unsigned int>* m_trigger_diamond_totalCount;
    std::vector<unsigned int>* m_trigger_diamond_realCount;
    std::vector<int>* m_trigger_diamond_iX;
    std::vector<int>* m_trigger_diamond_iU;
    std::vector<int>* m_trigger_diamond_iV;
    std::vector<unsigned int>* m_trigger_diamond_XbkgCount;
    std::vector<unsigned int>* m_trigger_diamond_UVbkgCount;
    std::vector<unsigned int>* m_trigger_diamond_XmuonCount;
    std::vector<unsigned int>* m_trigger_diamond_UVmuonCount;
    std::vector<double>* m_trigger_diamond_age;
    std::vector<double>* m_trigger_diamond_mx;
    std::vector<double>* m_trigger_diamond_my;
    std::vector<double>* m_trigger_diamond_Uavg;
    std::vector<double>* m_trigger_diamond_Vavg;
    std::vector<double>* m_trigger_diamond_mxl;
    std::vector<double>* m_trigger_diamond_theta;
    std::vector<double>* m_trigger_diamond_eta;
    std::vector<double>* m_trigger_diamond_dtheta;
    std::vector<double>* m_trigger_diamond_phi;
    std::vector<double>* m_trigger_diamond_phiShf;
    std::vector<uint8_t>* m_trigger_diamond_TP_phi_id;
    std::vector<uint8_t>* m_trigger_diamond_TP_R_id;
    std::vector<uint8_t>* m_trigger_diamond_TP_dTheta_id;

    std::vector<double>* m_trigger_RZslopes;
    std::vector<double>* m_trigger_fitThe;
    std::vector<double>* m_trigger_fitPhi;
    std::vector<double>* m_trigger_fitDth;
    std::vector<double>* m_trigger_trueEtaRange;
    std::vector<double>* m_trigger_truePtRange;
    std::vector<double>* m_trigger_fitEtaRange;
    std::vector<double>* m_trigger_fitPtRange;
    std::vector<double>* m_trigger_resThe;
    std::vector<double>* m_trigger_resPhi;
    std::vector<double>* m_trigger_resDth;

    std::vector<double>* m_trigger_large_fitThe;
    std::vector<double>* m_trigger_large_fitPhi;
    std::vector<double>* m_trigger_large_fitDth;
    std::vector<double>* m_trigger_large_trueEtaRange;
    std::vector<double>* m_trigger_large_truePtRange;
    std::vector<double>* m_trigger_large_trueThe;
    std::vector<double>* m_trigger_large_truePhi;
    std::vector<double>* m_trigger_large_trueDth;
    std::vector<double>* m_trigger_large_fitEtaRange;
    std::vector<double>* m_trigger_large_fitPtRange;
    std::vector<double>* m_trigger_large_resThe;
    std::vector<double>* m_trigger_large_resPhi;
    std::vector<double>* m_trigger_large_resDth;

    std::vector<double>* m_trigger_small_fitThe;
    std::vector<double>* m_trigger_small_fitPhi;
    std::vector<double>* m_trigger_small_fitDth;
    std::vector<double>* m_trigger_small_trueEtaRange;
    std::vector<double>* m_trigger_small_truePtRange;
    std::vector<double>* m_trigger_small_trueThe;
    std::vector<double>* m_trigger_small_truePhi;
    std::vector<double>* m_trigger_small_trueDth;
    std::vector<double>* m_trigger_small_fitEtaRange;
    std::vector<double>* m_trigger_small_fitPtRange;
    std::vector<double>* m_trigger_small_resThe;
    std::vector<double>* m_trigger_small_resPhi;
    std::vector<double>* m_trigger_small_resDth;

    std::vector<int>* m_trigger_VMM;
    std::vector<int>* m_trigger_plane;
    std::vector<int>* m_trigger_station;
    std::vector<int>* m_trigger_strip;
    std::vector<double>* m_trigger_slope;
    std::vector<double>* m_trigger_trueThe;
    std::vector<double>* m_trigger_truePhi;
    std::vector<double>* m_trigger_trueDth;
    std::vector<double>* m_trigger_mxl;
    std::vector<double>* m_trigger_mx;
    std::vector<double>* m_trigger_my;
    std::vector<double>* m_trigger_mu;
    std::vector<double>* m_trigger_mv;

    std::vector<std::string> *m_NSWMM_dig_stationName;
    std::vector<int> *m_NSWMM_dig_stationEta;
    std::vector<int> *m_NSWMM_dig_stationPhi;
    std::vector<int> *m_NSWMM_dig_multiplet;
    std::vector<int> *m_NSWMM_dig_gas_gap;
    std::vector<int> *m_NSWMM_dig_channel;

    std::vector< std::vector<float> >  *m_NSWMM_dig_time;
    std::vector< std::vector<float> >  *m_NSWMM_dig_charge;
    std::vector< std::vector<int> >    *m_NSWMM_dig_stripPosition;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripLposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripLposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_stripGposZ;
    std::vector< std::vector<float> >  *m_NSWMM_dig_sr_time;
    std::vector< std::vector<float> >  *m_NSWMM_dig_sr_charge;
    std::vector< std::vector<int> >    *m_NSWMM_dig_sr_stripPosition;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposX;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposY;
    std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposZ;

    std::vector< int    > *m_NSWMM_dig_truth_barcode;
    std::vector< double > *m_NSWMM_dig_truth_localPosX;
    std::vector< double > *m_NSWMM_dig_truth_localPosY;
    std::vector< double > *m_NSWMM_dig_truth_globalPosX;
    std::vector< double > *m_NSWMM_dig_truth_globalPosY;
    std::vector< double > *m_NSWMM_dig_truth_globalPosZ;
    std::vector< float  > *m_NSWMM_dig_truth_XZ_angle;

    std::vector<int>   *m_NSWMM_dig_stripForTrigger;
    std::vector<float> *m_NSWMM_dig_stripTimeForTrigger;


    std::vector<int>    *m_NSWMM_trackId;
    std::vector<int>    *m_NSWMM_truthEl;
    std::vector<double> *m_NSWMM_globalTime;
    std::vector<double> *m_NSWMM_hitGlobalPositionX;
    std::vector<double> *m_NSWMM_hitGlobalPositionY;
    std::vector<double> *m_NSWMM_hitGlobalPositionZ;
    std::vector<double> *m_NSWMM_hitGlobalPositionR;
    std::vector<double> *m_NSWMM_hitGlobalPositionP;
    std::vector<double> *m_NSWMM_hitGlobalDirectionX;
    std::vector<double> *m_NSWMM_hitGlobalDirectionY;
    std::vector<double> *m_NSWMM_hitGlobalDirectionZ;


    std::vector<double> *m_NSWMM_detector_globalPositionX;
    std::vector<double> *m_NSWMM_detector_globalPositionY;
    std::vector<double> *m_NSWMM_detector_globalPositionZ;
    std::vector<double> *m_NSWMM_detector_globalPositionR;
    std::vector<double> *m_NSWMM_detector_globalPositionP;

    std::vector<double> *m_NSWMM_hitToDsurfacePositionX;
    std::vector<double> *m_NSWMM_hitToDsurfacePositionY;
    std::vector<double> *m_NSWMM_hitToDsurfacePositionZ;

    std::vector<double> *m_NSWMM_hitToRsurfacePositionX;
    std::vector<double> *m_NSWMM_hitToRsurfacePositionY;
    std::vector<double> *m_NSWMM_hitToRsurfacePositionZ;


    std::vector<int> *m_NSWMM_particleEncoding;
    std::vector<double> *m_NSWMM_kineticEnergy;
    std::vector<double> *m_NSWMM_depositEnergy;

    std::vector<std::string> *m_NSWMM_sim_stationName;
    std::vector<int> *m_NSWMM_sim_stationEta;
    std::vector<int> *m_NSWMM_sim_stationPhi;
    std::vector<int> *m_NSWMM_sim_multilayer;
    std::vector<int> *m_NSWMM_sim_layer;
    std::vector<int> *m_NSWMM_sim_side;

    std::vector<std::string> *m_NSWMM_off_stationName;
    std::vector<int> *m_NSWMM_off_stationEta;
    std::vector<int> *m_NSWMM_off_stationPhi;
    std::vector<int> *m_NSWMM_off_multiplet;
    std::vector<int> *m_NSWMM_off_gas_gap;
    std::vector<int> *m_NSWMM_off_channel;

  };  // end of MMTriggerTool class

} // namespace NSWL1

#endif
