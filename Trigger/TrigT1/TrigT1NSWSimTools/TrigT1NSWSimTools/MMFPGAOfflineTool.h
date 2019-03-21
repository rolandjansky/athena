/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMFPGAOFFLINETOOL_H
#define MMFPGAOFFLINETOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/Property.h"

//local includes
#include "TrigT1NSWSimTools/IMMFPGATool.h"


//forward declarations
class IIncidentSvc;
class TTree;

// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short interface for the trigger FPGA tools
   *
   * This class implements the trigger FPGA offline simulation. It loops over the input MM strips, 
   * identifies cluster of hits coming from tracks, determines the BC tag and measures the delta
   * theta for applying the threshold for prompt tracks. It finally delivers the track candidates
   * to be matched with the Big Wheel RoIs. 
   * The trigger logic is processed for each trigger sector and the resulting candidates are cached
   * until the next event gets processed. Any attempt to process data from the same trigger sector 
   * twice in the event will trigger a failure of the tool. 
   * The input interface always works in the assumption that the input vector of MM strips contains
   * the data of the full NSW detector. The parameters "side" and "sector" only select the output
   * trigger candidates from a specific trigger sector. 
   *
   * It returns a vector of MMCandidateData to input the Trigger Sector Logic simulation.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>, Ourania Sidiropoulou <osidirop@cern.ch>
   *
   *
   */

  class MMFPGAOfflineTool: virtual public IMMFPGATool, 
                                   public AthAlgTool, 
                                   public IIncidentListener {

  public:
    enum cStatus {OK, FILL_ERROR, CLEARED};

    MMFPGAOfflineTool(const std::string& type, 
	              const std::string& name,
	              const IInterface* parent);

    virtual ~MMFPGAOfflineTool();

    virtual StatusCode initialize();

    virtual void handle (const Incident& inc);

    StatusCode gather_mmcandidate_data(std::vector<MMStripData*>& mmstrips, std::vector<MMCandidateData*>& mmcandidates, int side=-1, int sector=-1);


  private:
    // methods implementing the internal data processing
    cStatus apply_trigger_logic(std::vector<MMStripData*>& mmstrips); //!< process the mmstrip of a specifi trigger sector (0 - 31)
    void clear_cache();                                     //!< clear the MM candidate cache deleting the MMCandidateData pointers

    StatusCode book_branches();                             //!< book the branches to analyze the MMFPGA behavior
    void reset_ntuple_variables();                          //!< reset the variables used in the analysis ntuple
    void clear_ntuple_variables();                          //!< clear the variables used in the analysis ntuple
 

    // needed Servives, Tools and Helpers
    ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service

    // hidden variables
    std::vector< std::vector<MMCandidateData*> > 
            m_trigger_candidates;                           //!< cache for the MM Strip hit data in the event
    int     m_mmcandidate_cache_runNumber;                  //!< run number associated to the current MM Strip cache
    int     m_mmcandidate_cache_eventNumber;                //!< event number associated to the current MM Strip cache
    cStatus m_mmcandidate_cache_status[32];                 //!< status of the current cache

    //properties
    BooleanProperty  m_doNtuple;

    // analysis ntuple
    TTree* m_tree;                                          //!< ntuple for analysis

    // analysis variable to be put into the ntuple
    int  m_nMMCandidates;                                   //!< number of Candidates delivered


  };  // end of MMFPGAOfflineTool class

} // namespace NSWL1

#endif
