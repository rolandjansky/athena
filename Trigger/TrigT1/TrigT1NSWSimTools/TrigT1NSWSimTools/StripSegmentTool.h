/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPSEGMENTTOOL_H
#define STRIPSEGMENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonRDO/NSW_TrigRawDataContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

#include "RegSelLUT/RegSelSiLUT.h"
#include "RegSelLUT/IRegionIDLUT_Creator.h"
#include "IRegionSelector/IRegSelLUTCondData.h"

#include "TrigT1NSWSimTools/IStripSegmentTool.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/StripOfflineData.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"

#include "TTree.h"
#include <Math/Vector3D.h>
#include <functional>
#include <algorithm>
#include <map>
#include <utility>
#include <cmath>

struct Envelope_t{
  float lower_r{FLT_MIN};
  float upper_r{FLT_MAX};
  float lower_eta{FLT_MIN};
  float upper_eta{FLT_MAX};
  float lower_z{FLT_MIN};
  float upper_z{FLT_MAX};
};

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
    StripSegmentTool(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
    virtual ~StripSegmentTool()=default;
    virtual StatusCode initialize() override;
    virtual void handle (const Incident& inc) override;
    virtual StatusCode find_segments( std::vector< std::unique_ptr<StripClusterData> >& ,const std::unique_ptr<Muon::NSW_TrigRawDataContainer>& ) const override;
    StatusCode FetchDetectorEnvelope(Envelope_t &env) const;

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    SG::ReadCondHandleKey<IRegSelLUTCondData> m_regSelTableKey{this, "RegSelLUT", "RegSelLUTCondData_sTGC", "sTGC Region Selector lookup table"};

    // methods implementing the internal data processing
    StatusCode book_branches();                             //!< book the branches to analyze the StripTds behavior
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple

    // needed Services, Tools and Helpers
    ServiceHandle<IIncidentSvc>  m_incidentSvc     {this, "IncidentSvc", "IncidentSvc"};  //!< Athena/Gaudi incident Service
    Gaudi::Property<std::string> m_sTgcSdoContainer{this, "sTGC_SdoContainerName",  "sTGC_SDO", "Name of the sTGC SDO digit container"};
    Gaudi::Property<bool>        m_doNtuple        {this, "DoNtuple",               false,      "Input StripTds branches into the analysis ntuple"};
    Gaudi::Property<int>         m_rIndexBits      {this, "rIndexBits",              8,         "Number bits in R-index calculation"};
    Gaudi::Property<int>         m_dThetaBits      {this, "dthetaBits",              5,         "Number bits in dTheta calculation"};
    Gaudi::Property<float>       m_dtheta_min      {this, "dthetaMin",             -15.,        "Minimum allowed value for dtheta in mrad"};
    Gaudi::Property<float>       m_dtheta_max      {this, "dthetaMax",              15.,        "Maximum allowed value for dtheta in mrad"};
    Gaudi::Property<int>         m_ridxScheme      {this, "rIndexScheme",            1,         "rIndex slicing scheme/ 0-->R / 1-->eta"};

    /* None of the TTree filling is thread-safe and should really be refactored.
     * But we check in initialize() that this is only used in single-threaded mode.
     */
    TTree* m_tree{nullptr};                                  //!< ntuple for analysis
    std::vector<int> *m_seg_wedge1_size ATLAS_THREAD_SAFE{nullptr};                     //!< theta
    std::vector<int> *m_seg_wedge2_size ATLAS_THREAD_SAFE{nullptr};                     //!< theta
    std::vector<float> *m_seg_theta ATLAS_THREAD_SAFE{nullptr};                         //!< theta
    std::vector<float> *m_seg_dtheta ATLAS_THREAD_SAFE{nullptr};                        //!< delta theta
    std::vector<uint8_t> *m_seg_dtheta_int ATLAS_THREAD_SAFE{nullptr};
    std::vector<float> *m_seg_eta ATLAS_THREAD_SAFE{nullptr};                           //!< m_seg_eta
    std::vector<float> *m_seg_eta_inf ATLAS_THREAD_SAFE{nullptr};
    std::vector<float> *m_seg_phi ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_seg_bandId ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_seg_phiId ATLAS_THREAD_SAFE{nullptr};
    std::vector<int> *m_seg_rIdx ATLAS_THREAD_SAFE{nullptr};
    std::vector<float> *m_seg_global_x ATLAS_THREAD_SAFE{nullptr};
    std::vector<float> *m_seg_global_y ATLAS_THREAD_SAFE{nullptr};
    std::vector<float> *m_seg_global_z ATLAS_THREAD_SAFE{nullptr};

    uint8_t findRIdx(const float& val, const Envelope_t &env) const;
    uint8_t findDtheta(const float&) const;
  };  // end of StripSegmentTool class
} // namespace NSWL1
#endif
