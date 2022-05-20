/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPTDSOFFLINETOOL_H
#define STRIPTDSOFFLINETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "TrigT1NSWSimTools/IStripTdsTool.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/StripOfflineData.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"

#include "TTree.h"
#include "CxxUtils/checker_macros.h"
#include <functional>
#include <algorithm>
#include <map>
#include <utility>

namespace MuonGM {
  class MuonDetectorManager;
}

namespace NSWL1 {

  /**
   *
   *   @short interface for the StripTDS tools
   *
   * This class implements the Strip TDS offline simulation. It loops over the input digits,
   * determines the BC tag and applies the additional processing of the VMM chip which is
   * not yet implemented in the digitization. The loop is executed over the full digit pool
   * once upon the first data request for an event and the STRIP data are internally cached
   * and collected per trigger sectors. The run ID and event ID are cached; the processing
   * status is also cached to be propagated via a StatusCode at each data request.
   *
   * Supported processing:
   *  Currently None;
   *
   * It returns a vector of StripData to input the StripTrigger simulation.
   *
   *  @author Jacob Searcy <jsearcy@cern.ch>
   *
   * ----------------------------------------------------------------------------------------
   * 2022 Update: the internal cache has been removed for the code to deal with parallel
   * processing (athenaMT) in Release 22. It has been replaced by an event-by-event cache,
   * passed by reference throughout the workflow.
   *
   *  @modified by Francesco Giuseppe Gravili <francesco.giuseppe.gravili@cern.ch>
   *
   */

  class StripHits;

  class StripTdsOfflineTool: virtual public IStripTdsTool,
                                   public AthAlgTool,
                                   public IIncidentListener {

  public:
    StripTdsOfflineTool(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

    virtual ~StripTdsOfflineTool()=default;

    virtual StatusCode initialize() override;

    virtual void handle (const Incident& inc) override;

    virtual StatusCode gather_strip_data(std::vector<std::unique_ptr<StripData>>& strips,const std::vector<std::unique_ptr<PadTrigger>>& padTriggers) const override;


  private:
    // methods implementing the internal data processing
    StatusCode fill_strip_cache(const std::vector<std::unique_ptr<PadTrigger>>& padTriggers, std::vector<std::unique_ptr<StripData>> &strip_cache) const;

    StatusCode book_branches();                             //!< book the branches to analyze the StripTds behavior
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple
    void fill_strip_validation_id(std::vector<std::unique_ptr<StripData>> &strip_cache) const;  //!< fill the ntuple branch for the StripTdsOffline
    bool readStrip( StripData* ,const std::vector<std::unique_ptr<PadTrigger>>&) const;

    // needed Servives, Tools and Helpers
    ServiceHandle<IIncidentSvc> m_incidentSvc{this, "IncidentSvc", "IncidentSvc"};  //!< Athena/Gaudi incident Service
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    // hidden variables
    Gaudi::Property<bool> m_doNtuple{this, "DoNtuple", false, "Input StripTds branches into the analysis ntuple"};

    // activate MC-only processes
    Gaudi::Property<bool>         m_isMC            {this, "IsMC",                  true,       "This is MC"};


    /* None of the TTree filling is thread-safe and should really be refactored.
     * But we check in initialize() that this is only used in single-threaded mode.
     */
    TTree* m_tree ATLAS_THREAD_SAFE{nullptr};                                          //!< ntuple for analysis
    mutable int m_nStripHits ATLAS_THREAD_SAFE;             //!< number of STRIP hit delivered
    std::vector<float > *m_stripCharge ATLAS_THREAD_SAFE{nullptr};                   //!< charge of hit STRIPs
    std::vector<float > *m_stripCharge_6bit ATLAS_THREAD_SAFE{nullptr};              //!< charge of hit STRIPs 6 bit format
    std::vector<float > *m_stripCharge_10bit ATLAS_THREAD_SAFE{nullptr};             //!< charge of hit STRIPs 10 bit format
    std::vector<float > *m_strip_global_X ATLAS_THREAD_SAFE{nullptr};                //!< global X position
    std::vector<float > *m_strip_global_Y ATLAS_THREAD_SAFE{nullptr};                //!< global Y position
    std::vector<float > *m_strip_global_Z ATLAS_THREAD_SAFE{nullptr};                //!< global Z position
    std::vector<float > *m_strip_local_X ATLAS_THREAD_SAFE{nullptr};                 //!< local X position
    std::vector<float > *m_strip_local_Y ATLAS_THREAD_SAFE{nullptr};                 //!< local Y position
    std::vector<float > *m_strip_layer ATLAS_THREAD_SAFE{nullptr};                   //!< layer
    std::vector<bool > *m_strip_isSmall ATLAS_THREAD_SAFE{nullptr};                  //!< sector type
    std::vector<float > *m_strip_eta ATLAS_THREAD_SAFE{nullptr};                     //!< sector eta
    std::vector<float > *m_strip_phi ATLAS_THREAD_SAFE{nullptr};                     //!< sector phi
    std::vector<bool > *m_strip_readStrip ATLAS_THREAD_SAFE{nullptr};                //!< read strip status
    std::vector<int > *m_strip_channel ATLAS_THREAD_SAFE{nullptr};                   //!< channel
    std::vector<int > *m_strip_BCID ATLAS_THREAD_SAFE{nullptr};                      //!< BCID
    std::vector<int > *m_strip_wedge ATLAS_THREAD_SAFE{nullptr};                     //!< multipletId
    std::vector<float > *m_strip_time ATLAS_THREAD_SAFE{nullptr};                    //!< time

    SG::ReadHandleKey<sTgcDigitContainer> m_sTgcDigitContainer = {this,"sTGC_DigitContainerName","sTGC_DIGITS","the name of the sTGC digit container"};
    SG::ReadHandleKey<MuonSimDataCollection> m_sTgcSdoContainer = {this,"sTGC_SdoContainerName","sTGC_SDO","the name of the sTGC SDO container"};

  };  // end of StripTdsOfflineTool class

} // namespace NSWL1

#endif
