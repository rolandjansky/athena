/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "BSFilter.h"

#include "TrigConfData/HLTPrescalesSet.h"

BSFilter::BSFilter( const std::string& name,ISvcLocator* pSvcLocator ) :
  AthAlgorithm (name, pSvcLocator)
{
}

StatusCode BSFilter::initialize()
{
  ////////////////////////////
  if (!m_filterfile.empty() && m_l1ZB.empty()){
    FILE *vfile = fopen(m_filterfile.value().c_str(),"r");
    if (vfile){
      ATH_MSG_INFO("Opened filter file:" << m_filterfile);
      int vrun=0, vtrig=0, vnvtx=0; double vdt=0.0; int ne=0; uint64_t vevent=0;
      while (true){
        int r = fscanf(vfile, "%i %lu %i %i %lf\n", &vrun, &vevent, &vtrig, &vnvtx, &vdt);
        if (r>0){
          ATH_MSG_DEBUG("Read "<<r<<" filter values: "<<vrun<<"/"<<vevent<<" "<<vtrig<<","<<vnvtx<<","<<vdt);
          if (m_filtermap[vrun][vevent].magic==777){
            ATH_MSG_WARNING("Already filter info for run/event "<<vrun<<"/"<<vevent<<", magic="
                            <<m_filtermap[vrun][vevent].magic);
          }
          m_filtermap[vrun][vevent].trig=vtrig;
          m_filtermap[vrun][vevent].nvtx=vnvtx;
          m_filtermap[vrun][vevent].dt=vdt;
          m_filtermap[vrun][vevent].magic=777;
          ++ne;
        }
        else{
          ATH_MSG_INFO("Got "<<r<<" from fscanf, stopping");
          break;
        }
      }
      fclose(vfile);
      ATH_MSG_INFO("Read "<<ne<<" filter entries from file.");
    }
    else{
      ATH_MSG_ERROR("Could not open filter file: "<<m_filterfile);
    }
  }

  //If we have trigbit>0 and a filterfile, write the trigger info out to the filterfile
  if ( !m_filterfile.empty() ) {
    ATH_CHECK( m_trigConf.retrieve() );
    ATH_CHECK( m_trigDecTool.retrieve() );
    ATH_CHECK( (m_ffile = fopen(m_filterfile.value().c_str(),"w"))!=nullptr );
    ATH_MSG_INFO("Writing trigger decision for" << m_noalg << m_j40 << "to" << m_filterfile);
  }
  else {
    m_trigDecTool.disable();  // disable auto-retrieval
  }

  //////////////////////////
  if ( !m_EventIdFile.empty() ) {
    ATH_CHECK( (m_efile = fopen(m_EventIdFile.value().c_str(),"w"))!=nullptr );
    ATH_MSG_INFO("Opened" << m_EventIdFile);
  }

  return StatusCode::SUCCESS;
}

StatusCode BSFilter::finalize()
{
  if (m_efile) fclose(m_efile);
  if (m_ffile) fclose(m_ffile);

  ATH_MSG_INFO("BS Filter PASSED "<< m_pass<<"/"<< m_All << " events");
  return StatusCode::SUCCESS;
}

void BSFilter::writeEventIdFile(const EventIDBase& evt) const
{
  if (m_efile) {
    //"%ld" for evt_nbr since it's 64 bit, but need fix for https://its.cern.ch/jira/browse/ATEAM-286 first!
    fprintf(m_efile,
            "svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=%d, evt_nbr=%ld, time_stamp=%d, lbk_nbr=%d, nevts=1)\n",
            evt.run_number(), evt.event_number(), evt.time_stamp(), evt.lumi_block());
  }
}

StatusCode BSFilter::execute()
{
  m_All++;

  // Get event information from EventContext
  const EventContext ctx{Gaudi::Hive::currentContext()};
  const EventIDBase evt{ctx.eventID()};

  if (!m_l1ZB.empty()){
    const bool item_fired_after_veto = m_trigDecTool->isPassed(m_l1ZB);
    ATH_MSG_INFO(m_l1ZB<<" fired after veto? " << item_fired_after_veto);

    if (m_ffile) {
      const TrigConf::HLTPrescalesSet& hltps = m_trigConf->hltPrescalesSet(ctx);
      std::ostringstream oss;
      oss << "noalg=" << m_trigDecTool->isPassed(m_noalg)
          << ", j40=" << m_trigDecTool->isPassed(m_j40)
          << ", noalgps=" << hltps.prescale(m_noalg).prescale
          << ", j40ps=" << hltps.prescale(m_j40).prescale;

      ATH_MSG_INFO(oss.str());

      fprintf(m_ffile,"run_nbr=%d, evt_nbr=%ld, time_stamp=%d, lbk_nbr=%d, %s\n",
              evt.run_number(), evt.event_number(), evt.time_stamp(), evt.lumi_block(), oss.str().c_str());
    }

    if(item_fired_after_veto && m_filterfile.empty()) {//don't bother to write out RAW if we're only running to make the trigs.txt file
      ATH_MSG_INFO("Filter Passed");
      setFilterPassed(true);
      m_pass++;
      writeEventIdFile(evt);
    }
    else {
      ATH_MSG_INFO("Filter Failed");
      setFilterPassed(false);
    }

  }
  else {  // m_l1ZB not set -> filter based on filterfile
    const auto run = evt.run_number();
    const auto event = evt.event_number();
    bool passed = true;
    //Here is where you'd check for TAG info of some kind...
    if (!m_filterfile.empty()) {
      passed = false;
      if (m_filtermap[run][event].magic!=777) {
        ATH_MSG_WARNING("Dont have info in filtermap for "<<run<<" "<<event);
      }
      else {
        if (m_filtermap[run][event].nvtx==1 && m_filtermap[run][event].trig==1 &&
            fabs(m_filtermap[run][event].dt) <3.0) {
          passed = true;
        }
        ATH_MSG_INFO((passed ? "Passing" : "Not passing") << " filter event "<<
                     run<<" "<<event<<", trig run dt magic: "<<
                     m_filtermap[run][event].trig<<" "<<m_filtermap[run][event].nvtx<<" "<<
                     m_filtermap[run][event].dt<<" "<<m_filtermap[run][event].magic);
      }
    }

    if (passed) {
      ATH_MSG_DEBUG("Filter Passed");
      setFilterPassed(true);
      m_pass++;
      writeEventIdFile(evt);
    }
    else {
      ATH_MSG_DEBUG("Filter Failed");
      setFilterPassed(false);
    }
  }
  return StatusCode::SUCCESS;
}
