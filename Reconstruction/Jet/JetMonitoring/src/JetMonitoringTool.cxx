/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoringTool.h"
#include "xAODJet/JetContainer.h"

JetMonitoringTool::JetMonitoringTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent ):
  ManagedMonitorToolBase( type, name, parent )
  ,  m_htools(this)
  //  , m_interval(HistoGroupBase::all)
  , m_interval(HistoGroupBase::lumiBlock)
{
   
  declareProperty( "HistoTools", m_htools );
  declareProperty( "HistoDir", m_histoDir="Jets/" ); 
  declareProperty( "IntervalType", m_interval);
}
 
// Destructor
///////////////
JetMonitoringTool::~JetMonitoringTool()
{}
 
// Athena algtool's Hooks
////////////////////////////
StatusCode JetMonitoringTool::initialize()
{
  //  ATH_MSG_INFO ("Initializing " << name() << "...");   
  ATH_CHECK(ManagedMonitorToolBase::initialize());   
  CHECK( m_htools.retrieve() );

  // All subtools in JetMonitoringTool must act on the same container.
  // Force it so the histo naming done by the subtool is correct.
  std::string prefix = m_histoDir;
  for( auto jtool : m_htools){
    jtool->prefixHistoDir( prefix );
  }
  return StatusCode::SUCCESS;
}
 
StatusCode JetMonitoringTool::bookHistograms()
{
  /* ATH_MSG_INFO ("Booking hists " << name() << "...");
     ATH_MSG_INFO (" Tools size = " << m_htools.size() << "...");*/

  // loop over tools : for each tool, register its histos
  for( auto jtool : m_htools){
    ATH_MSG_DEBUG(" Building histos for tool "<< jtool->name() );
    jtool->setInterval((HistoGroupBase::Interval_t) m_interval );
    if( jtool->buildHistos() != 0 ) continue;
    
    // retrieve histos and their paths
    const auto & hdata = jtool->bookedHistograms();
    // loop over histos
    for( const auto& hd : hdata){
      ATH_MSG_DEBUG ("Initializing " << hd.hist << " name= "<< hd.hist->GetName() << " path=" << hd.path << "..." << hd.interval << " dir="<<hd.hist->GetDirectory());
      ATH_CHECK(regHist(hd.hist,hd.path,  (Interval_t) hd.interval));
      ATH_MSG_DEBUG ("    -->> "<< hd.hist->GetName()  << " dir="<<hd.hist->GetDirectory() << "  "<< hd.hist->kCanDelete );
    } // loop over histos
  } // loop over tools

  return StatusCode::SUCCESS;     
}

StatusCode JetMonitoringTool::fillHistograms()
{
  //ATH_MSG_INFO ("Filling hists " << name() << "..." );

  /// simply call fillHistosFromContainer() for each tool...
  int count = 0;
  for( auto jtool : m_htools){
    count += jtool->fillHistos();
  }

  return count==0 ? StatusCode::SUCCESS : StatusCode::FAILURE ;  
}
 
StatusCode JetMonitoringTool::procHistograms()
{
  // ATH_MSG_INFO ("Finalising hists " << name() << "...");
  // m_oMuonValidationPlots.finalize();
  return StatusCode::SUCCESS;
}
