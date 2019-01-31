/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPTDSOFFLINETOOL_H
#define STRIPTDSOFFLINETOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/Property.h"

//local includes
#include "TrigT1NSWSimTools/IStripTdsTool.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"


//forward declarations
class IIncidentSvc;
class IAtRndmGenSvc;
class sTgcIdHelper;
class sTgcDigit;
class TTree;

namespace CLHEP {
  class HepRandomEngine;
}

namespace MuonGM {
  class MuonDetectorManager;
}


// namespace for the NSW LVL1 related classes
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
   *
   */

  class StripHits;

  class StripTdsOfflineTool: virtual public IStripTdsTool, 
                                   public AthAlgTool, 
                                   public IIncidentListener {

  public:
    enum cStatus {OK, FILL_ERROR, CLEARED};

    StripTdsOfflineTool(const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

    virtual ~StripTdsOfflineTool();

    virtual StatusCode initialize();

    virtual void handle (const Incident& inc);

    StatusCode gather_strip_data(std::vector<std::unique_ptr<StripData>>& strips,const std::vector<std::unique_ptr<PadTrigger>>& padTriggers);
 

  private:
    // methods implementing the internal data processing
    cStatus fill_strip_cache( const std::vector<std::unique_ptr<PadTrigger>>& padTriggers);   //!< loop over the digit container, apply the additional processing then fill the cache
    void clear_cache();                                     //!< clear the strip hit cache deleting the StripData pointers

    StatusCode book_branches();                             //!< book the branches to analyze the StripTds behavior
    void reset_ntuple_variables();                          //!< reset the variables used in the analysis ntuple
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple
    void fill_strip_validation_id();                        //!< fill the ntuple branch for the StripTdsOffline
    bool readStrip( StripData* ,const std::vector<std::unique_ptr<PadTrigger>>&);

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service
    ServiceHandle< IAtRndmGenSvc >     m_rndmSvc;           //!< Athena random number service
    CLHEP::HepRandomEngine*            m_rndmEngine;        //!< Random number engine
    const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
    const sTgcIdHelper*                m_sTgcIdHelper;      //!< sTgc offline Id helper

    // hidden variables
    std::vector<std::unique_ptr<StripData>>  m_strip_cache;                 //!< cache for the STRIP hit data in the event
    int     m_strip_cache_runNumber;                          //!< run number associated to the current STRIP cache
    int     m_strip_cache_eventNumber;                        //!< event number associated to the current STRIP cache
    cStatus m_strip_cache_status;                             //!< status of the current cache
    BooleanProperty  m_doNtuple;                            //!< property, see @link StripTdsOfflineTool::StripTdsOfflineTool @endlink  

    // properties: container and service names
    StringProperty   m_rndmEngineName;                      //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
    StringProperty   m_sTgcDigitContainer;                  //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
    StringProperty   m_sTgcSdoContainer;                    //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink


    // analysis ntuple
    TTree* m_tree;                                          //!< ntuple for analysis

    // analysis variable to be put into the ntuple
    int m_nStripHits;                                            //!< number of STRIP hit delivered
    int m_padTrigIndex;
    std::vector<float > *m_stripCharge=0;                           //!< charge of hit STRIPs 
    std::vector<float > *m_stripCharge_6bit=0;                           //!< charge of hit STRIPs 6 bit format
    std::vector<float > *m_stripCharge_10bit=0;                           //!< charge of hit STRIPs 10 bit format
    std::vector<float > *m_strip_global_X=0;                           //!< global X position
    std::vector<float > *m_strip_global_Y=0;                           //!< global Y position
    std::vector<float > *m_strip_global_Z=0;                           //!< global Z position
    std::vector<float > *m_strip_local_X=0;                           //!< local X position
    std::vector<float > *m_strip_local_Y=0;                           //!< local Y position
    std::vector<float > *m_strip_layer=0;                           //!< layer
    std::vector<float > *m_strip_isSmall=0;                           //!< sector number
    std::vector<float > *m_strip_eta=0;                           //!< sector eta
    std::vector<float > *m_strip_phi=0;                           //!< sector phi
    std::vector<float > *m_strip_readStrip=0;                           //!< sector phi
    std::vector<int > *m_strip_channel=0;                           //!< channel
    std::vector<int > *m_strip_BCID=0;                           //!< BCID
    std::vector<int > *m_strip_wedge=0;                           //!< multipletId
    std::vector<float > *m_strip_time=0;                           //!< multipletId


  };  // end of StripTdsOfflineTool class

} // namespace NSWL1

#endif
