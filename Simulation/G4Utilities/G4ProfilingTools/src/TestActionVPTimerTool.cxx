/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TestActionVPTimerTool.h"
#include "VolumeTreeNavigator.h"
#include "boost/io/ios_state.hpp"
#include <algorithm>
#include <iomanip>
#include <ios>

using std::map;         using std::max;         using std::string;
using std::setw;        using std::ios;         using std::make_pair;
using std::pair;        using std::find;        using std::vector;

namespace G4UA{ 
  
 // Formats print string for timer output
  
  inline std::string vPrFmt (double time, double nEv, double tRun, int depth, std::string id)
  {
    std::string dprnt = stringify(depth);
    if (depth == -999) dprnt = "      ";
    std::ostringstream oss;
    oss << std::setw(12) << time << std::setw(12) << time/nEv << std::setw(12) << time/tRun
	<< std::setw(6) << dprnt << "  " << id;
    return oss.str();
  }
  

  typedef std::map<VolTree, TestActionVPTimer::volumeData> VolMap;
  typedef VolMap::const_iterator VolIt;

  TestActionVPTimerTool::TestActionVPTimerTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBaseReport<TestActionVPTimer>(type, name, parent), m_config(){    
    declareProperty("CaloDepth",m_config.dCALO);
    declareProperty("BeamPipeDepth",m_config.dBeam);
    declareProperty("InDetDepth",m_config.dIDET);
    declareProperty("MuonDepth",m_config.dMUON);
    declareProperty("DetailDepth",m_config.dDetail);
    
  }
  std::unique_ptr<TestActionVPTimer>  TestActionVPTimerTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<TestActionVPTimer>(m_config);
    return std::move(action);
  }
  StatusCode TestActionVPTimerTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<TestActionVPTimer>::queryInterface(riid, ppvIf);
  }


  StatusCode TestActionVPTimerTool::finalize(){

    for(auto tidAction : this->actions()) {
      ((G4UserRunAction*)tidAction.second)->EndOfRunAction(0);
    }
 
    mergeReports();
    
    if(m_report.time_index.size()){

      VolTree topPV; // vector< pair<physvol*,int> >
      // time_index is map<VolTree, TestActionVPTimer::volumeData>

      topPV.push_back(m_report.time_index.begin()->first.front());
      TestActionVPTimer::volumeData atlasData = m_report.time_index.find(topPV)->second;
      G4double DeadTime  = m_report.runTime - atlasData.tTotal;
      
      ATH_MSG_INFO("Runtime " << m_report.runTime
		   <<  " Dead Time " << DeadTime );
      
      if (m_report.nev != 0){
	ATH_MSG_INFO("******* Beginning timer dump *******" );
	
	ATH_MSG_INFO(setw(12) << "Time" << setw(12) << "per Event" << setw(12)
		     << "per Run" << setw(6) << "Depth" << "  Location/Particle" );
	ATH_MSG_INFO(vPrFmt(m_report.runTime, m_report.nev, atlasData.tTotal, -999, "  Event Average") );
	ATH_MSG_INFO(vPrFmt(DeadTime, m_report.nev, atlasData.tTotal, -999, "  Dead/Initialization") );
	
	ATH_MSG_INFO("\n******* Data by volume/select particles *******");
	ATH_MSG_INFO("Data printed in reverse tree order (mother following daughter)");
	
	TreeOut(topPV, atlasData.tTotal);
	//      for (VolIt c = v_time_index.begin(); c != v_time_index.end(); c++) {
	//          TimerPrint(*c, atlasData.tTotal, c->first.size());
	//      }
	
	ATH_MSG_INFO( "\n******* Data by particle/category *******");
	
	ATH_MSG_INFO(vPrFmt(atlasData.tElectron, m_report.nev, atlasData.tTotal, -999, "e+/-"   ) );
	ATH_MSG_INFO(vPrFmt(atlasData.tPhoton,   m_report.nev, atlasData.tTotal, -999, "gamma"  ) );
	ATH_MSG_INFO(vPrFmt(atlasData.tNeutron,  m_report.nev, atlasData.tTotal, -999, "neutron") );
	ATH_MSG_INFO(vPrFmt(atlasData.tPion,     m_report.nev, atlasData.tTotal, -999, "pi+/-/0") );
	ATH_MSG_INFO(vPrFmt(atlasData.tBaryon,   m_report.nev, atlasData.tTotal, -999, "baryon" ) );
	ATH_MSG_INFO(vPrFmt(atlasData.tLepton,   m_report.nev, atlasData.tTotal, -999, "lepton" ) );
	ATH_MSG_INFO(vPrFmt(atlasData.tMeson,    m_report.nev, atlasData.tTotal, -999, "meson"  ) );
	ATH_MSG_INFO(vPrFmt(atlasData.tOther,    m_report.nev, atlasData.tTotal, -999, "other"  ) );
	
      } else { ATH_MSG_WARNING("******* No events timed! *******"); }
    } else { ATH_MSG_WARNING("******* No Timing information recorded! *******"); }
      return StatusCode::SUCCESS;
  }
  
  void TestActionVPTimerTool::TreeOut(VolTree id, const double tAtlas, int depth)
  {
    VolIt v = m_report.time_index.find(id);
    if (v != m_report.time_index.end()) {
      depth++;
      for (VolIt c = m_report.time_index.begin(); c != m_report.time_index.end(); c++) {
	VolTree check = c->first;
	check.pop_back();
	if (id == check)  TreeOut(c->first, tAtlas, depth);
      }
      TimerPrint(*v, tAtlas, depth);
    }
    return;
  }


  // Prints information about time by particle type and volume (modify to suit)
  
  inline void TestActionVPTimerTool::TimerPrint(std::pair<VolTree, TestActionVPTimer::volumeData> vp, const double tTotal, const int depth) const
  {
    G4VPhysicalVolume* pv = vp.first.back().first;
    std::string physical = stringify(pv->GetName());
    std::string copyNo = stringify(vp.first.back().second);
    std::string mother = "";
    if (vp.first.size() != 1) mother = stringify( (vp.first.end()-2)->first->GetName() );
    ATH_MSG_INFO(vPrFmt(vp.second.tTotal, m_report.nev, tTotal, depth-1, physical+"_"+copyNo+"  "+mother) );
    ATH_MSG_INFO(vPrFmt(vp.second.tElectron, m_report.nev, tTotal, depth-1, " - electrons & positrons") );
    ATH_MSG_INFO(vPrFmt(vp.second.tPhoton, m_report.nev, tTotal, depth-1, " - photons") );
    ATH_MSG_INFO(vPrFmt(vp.second.tNeutron, m_report.nev, tTotal, depth-1, " - neutrons") );
    ATH_MSG_INFO(vPrFmt(vp.second.tPion, m_report.nev, tTotal, depth-1, " - pions") );
    
    double tOther = vp.second.tTotal - vp.second.tElectron - vp.second.tPhoton - vp.second.tNeutron - vp.second.tPion;
    ATH_MSG_INFO(vPrFmt(tOther, m_report.nev, m_report.runTime, depth-1, " - other particles") );
    return;
  }
  


} // namespace G4UA 
