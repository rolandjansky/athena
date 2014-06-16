/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ITHistSvc.h"

#include "VxVertex/VxContainer.h"

#include "EventShapeRec/EventEtDensityTester.h"

EventEtDensityTester::EventEtDensityTester(const std::string& name,
					   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name,pSvcLocator)
  , m_tools(0)
  , m_histPath("EventEtDensity")
  , m_vtxContainerKey("VxPrimaryCandidate")
  , h_densities()
  , p_densities()
  , m_fillEvtStore(false)
  , m_fillHistos(false)
{
  declareProperty("EventEtDensityTools",m_tools);
  declareProperty("HistogramPath",m_histPath);
  declareProperty("VertexContainerKey",m_vtxContainerKey);
  declareProperty("FillEventStore",m_fillEvtStore);
  declareProperty("FillHistos",m_fillHistos);
}

EventEtDensityTester::~EventEtDensityTester()
{ }

StatusCode EventEtDensityTester::initialize()
{
  // allocate tools
  if ( m_tools.retrieve().isFailure() || m_tools.empty() )
    {
      ATH_MSG_ERROR("No tools allocated for algorithm");
      return StatusCode::FAILURE;
    }

  if(m_fillHistos){
  // book histograms for tools
  ITHistSvc* pHistSvc = 0;
  if ( service("THistSvc",pHistSvc).isFailure() )
    {
      ATH_MSG_ERROR("Cannot allocate HistSvc");
      return StatusCode::FAILURE;
    }
  //
  ToolHandleArray<IEventEtDensityProvider>::iterator fTool(m_tools.begin());
  ToolHandleArray<IEventEtDensityProvider>::iterator lTool(m_tools.end());
  for ( ; fTool != lTool; ++fTool )
    {
      // rho distributions
      std::string hName = m_histPath+"/Rho_"+(*fTool)->name();
      TH1D* h = new TH1D(hName.c_str(),hName.c_str(),80,0.,20.);
      if ( pHistSvc->regHist(hName,h).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot register histogram <" << hName << ">");
	  h_densities.push_back((TH1D*)0);
	}
      else
	{
	  ATH_MSG_INFO("Registered histogram <" << hName << ">");
	  h_densities.push_back(h);
	}
      // rho versus NPV
      std::string pName = m_histPath+"/RhoVsNpv_"+(*fTool)->name();
      TProfile* p = new TProfile(pName.c_str(),pName.c_str(),
				 50,0.,50.);
      if ( pHistSvc->regHist(pName,p).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot register profile <" << pName << ">");
	  p_densities.push_back((TProfile*)0);
	}
      else
	{
	  ATH_MSG_INFO("Registered profile <" << pName << ">");
	  p_densities.push_back(p);
	}
    } 
  }

  ATH_CHECK( userStore().retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode EventEtDensityTester::execute()
{
  // get event characteristics
  double npv;
  if(m_fillHistos) {
    npv=this->getNPV();                     
    if (npv < 0. ) {
      ATH_MSG_WARNING("Invalid Npv " << npv);
      return StatusCode::SUCCESS;
    }
  }
  // loop tools
  ToolHandleArray<IEventEtDensityProvider>::iterator fTool(m_tools.begin());
  ToolHandleArray<IEventEtDensityProvider>::iterator lTool(m_tools.end());
  std::vector<TH1D*>::iterator     fHist(h_densities.begin());
  std::vector<TProfile*>::iterator fProf(p_densities.begin());
  for ( ; fTool != lTool; ++fTool, ++fHist, ++fProf )
    {
      double rho((*fTool)->rho());
      if(m_fillHistos && (npv>=0 )){
        (*fHist)->Fill(rho);
        (*fProf)->Fill(npv,rho);
      }
      if(m_fillEvtStore){
        std::string n = (*fTool)->name();
        n = n.substr(n.find_last_of(".")+1);
        ATH_CHECK ( userStore()->record( n, rho) );
        ATH_MSG_DEBUG( " stored "<< n << "  "<<rho) ;
      }
    }
  return StatusCode::SUCCESS;
}

double EventEtDensityTester::getNPV() 
{
  // picking up the vertex container for all methods
  const VxContainer* pVxColl = 0;
  if ( evtStore()->retrieve(pVxColl,m_vtxContainerKey).isFailure() )
    {
      ATH_MSG_WARNING("Cannot allocate VxContainer with key <"
		      << m_vtxContainerKey << ">");
      return StatusCode::SUCCESS;
    }
  // count primary vertices
  VxContainer::const_iterator fVtx(pVxColl->begin());
  VxContainer::const_iterator lVtx(pVxColl->end());
  size_t npvCtr(0);
  for ( ; fVtx != lVtx; ++fVtx )
    { if ( (*fVtx)->vertexType() == Trk::PriVtx ) ++npvCtr; }
  if ( npvCtr == 0 )
    { 
      ATH_MSG_WARNING("No primary vertex in event");
      return double(-1.0); 
    }
  return (double)npvCtr;
}
