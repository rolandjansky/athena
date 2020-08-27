#include <list>
#include "AthContainers/ConstDataVector.h"
#include "JetMonitoring/JetHistoSelectSort.h"
#include "JetMonitoring/JetMonitoringAlg.h" 


JetHistoSelectSort::JetHistoSelectSort( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
  , m_jetFillerTools(this)
  , m_selectTool(this)
  , m_eventSelTool(this)
  , m_sortVar(this)

{
  declareInterface<IJetHistoFiller>(this);

  declareProperty("FillerTools",m_jetFillerTools);
  declareProperty("Selector",m_selectTool);
  declareProperty("EventSelector",m_eventSelTool);
  declareProperty("SortVariable",m_sortVar);
}


StatusCode JetHistoSelectSort::initialize() {
  ATH_MSG_INFO("  initialize "    );

  ATH_CHECK(m_jetFillerTools.retrieve() );

  ATH_MSG_INFO( " Scheduled Histo fillers/selectors : ");
  for(const auto& t: m_jetFillerTools){
    ATH_MSG_INFO( "--> "<< t->name() );
  }

  
  if(m_selectTool.isEnabled()){
    ATH_CHECK(m_selectTool.retrieve());
    ATH_MSG_INFO( " Selecting with "<< m_selectTool->name() );
  }
  if(m_inverseJetSel){
    ATH_MSG_DEBUG( "   inverse Jet Selection will be applied" );
  }

  if(m_eventSelTool.isEnabled()){
    ATH_CHECK(m_eventSelTool.retrieve());
    ATH_MSG_INFO( " Selecting with "<< m_eventSelTool->name() );
  }
  
  if( m_sortVar.isEnabled() ){
    ATH_CHECK(m_sortVar.retrieve());
    ATH_MSG_INFO( "Sorting on var =("<< m_sortVar->describe() << ")");    
  }  
    
  return StatusCode::SUCCESS;
}


StatusCode JetHistoSelectSort::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const {


  // fill a temporary list of pointer
  std::list<const xAOD::Jet*> tmpList;

  // do sort if needed
  if(m_sortVar.isEnabled() ){
    auto sort = [this] (const xAOD::Jet * j1, const xAOD::Jet * j2) {return m_sortVar->value(*j1) > m_sortVar->value(*j2); } ;
    tmpList.sort( sort );
  }

  if(m_selIndex>-1){
    if( (int) jets.size( ) > m_selIndex) { tmpList.push_back( jets[m_selIndex]); }
  }else {
    for(const xAOD::Jet* jet : jets ) tmpList.push_back(jet);
  }

  // select if needed
  if(m_selectTool.isEnabled()){
    if (m_inverseJetSel) {
      auto sel = [this] (const xAOD::Jet * j) {return  m_selectTool->keep(*j) ; } ;
      tmpList.remove_if( sel );
    }
    else {
      auto sel = [this] (const xAOD::Jet * j) {return ! m_selectTool->keep(*j) ; } ;
      tmpList.remove_if( sel );
    }
  }

  if(m_eventSelTool.isEnabled()){
    auto eventInfo = parentAlg.GetEventInfo(ctx);
    if ( ! m_eventSelTool->keep(*eventInfo, jets) ) tmpList.clear();
  }


  ConstDataVector< xAOD::JetContainer > tmpCont(SG::VIEW_ELEMENTS);
  for(const xAOD::Jet* jet : tmpList ) tmpCont.push_back(jet);
  const xAOD::JetContainer * filteredCont = tmpCont.asDataVector();

  for(const auto& t: m_jetFillerTools){
    ATH_CHECK(t->processJetContainer(parentAlg, *filteredCont, ctx));
  }

  
  return StatusCode::SUCCESS;
}

