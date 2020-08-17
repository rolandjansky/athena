/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

//#include "CLHEP/Units/PhysicalConstants.h"

#include "MissingETPerformance/ContainerComparatorTool.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

//------------------------------------------------------------------------------
ContainerComparatorTool::ContainerComparatorTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) : AthAlgTool( type, name, parent ) {

  declareInterface<ContainerComparatorTool>( this );
  declareProperty("containers_to_compare",   m_containers_to_compare);
  declareProperty("Data_BasicTool_pairs", m_data_basictool_pairs);
  declareProperty("Data_Resolution_pairs", m_data_resolution_pairs);
  declareProperty("Data_Linearity_pairs", m_data_linearity_pairs);

  declareProperty("MissingETData_array", m_the_data);

  declareProperty("BasicPlotsTools", m_basicplots_tools);
  declareProperty("ResolutionTools", m_resolution_tools);
  declareProperty("LinearityTools", m_linearity_tools);
}

//------------------------------------------------------------------------------

StatusCode ContainerComparatorTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  for (std::vector<std::vector<std::string> >::iterator container_it = m_containers_to_compare.begin(); container_it != m_containers_to_compare.end(); ++container_it) {
    for (std::vector<std::string>::iterator string_it = container_it->begin(); string_it != container_it->end(); ++string_it) {
      for (std::vector<std::string>::iterator string_it2 = string_it + 1; string_it2 != container_it->end(); ++string_it2) {
      ContainerComparatorTool::DiffContainers *df = new ContainerComparatorTool::DiffContainers( *string_it , *string_it2 ,m_thistSvc );
      m_diffContainers.push_back(df);

      ContainerComparatorTool::VsContainers *vs = new ContainerComparatorTool::VsContainers( *string_it, *string_it2, m_thistSvc );
      m_vsContainers.push_back(vs);
      }//loop over strings = container names to compare
    }//loop over strings = container names to compare
  }//loop over container lists

  for (std::vector<ToolHandle<BasicPlotsTool> >::iterator it = m_basicplots_tools.begin();it != m_basicplots_tools.end(); ++it) {
    sc = (*it).retrieve();
    sc = (*it)->CBNT_initialize();
  }
  for (std::vector<ToolHandle<ResolutionTool> >::iterator it = m_resolution_tools.begin();it != m_resolution_tools.end(); ++it) {
    sc = (*it).retrieve();
    sc = (*it)->CBNT_initialize();
  }
  for (std::vector<ToolHandle<LinearityTool> >::iterator it = m_linearity_tools.begin();it != m_linearity_tools.end(); ++it) {
    sc = (*it).retrieve();
    sc = (*it)->CBNT_initialize();
  }

  if (sc.isFailure()) { msg() << MSG::WARNING << "ContainerComparatorTool CBNT_initialize() fail" << endmsg; }

  return sc;
}

StatusCode ContainerComparatorTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode ContainerComparatorTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endmsg;

  for (std::vector<ToolHandle<MissingETData> >::iterator it = m_the_data.begin();it != m_the_data.end(); ++it) {
    sc = (*it)->retrieveContainers();
  }

  //loop over names and find the right MissingETData object and calling the right tool
  for (std::vector<std::vector<std::string> >::iterator it =  m_data_basictool_pairs.begin(); it != m_data_basictool_pairs.end(); ++it) {

    std::string dataname = (*it).at(0);
    std::string basictoolname = (*it).at(1);

    for (std::vector<ToolHandle<MissingETData> >::iterator itd = m_the_data.begin();itd != m_the_data.end(); ++itd) {
      if ( (*itd).name() == dataname) {

	for (std::vector<ToolHandle<BasicPlotsTool> >::iterator itb = m_basicplots_tools.begin();itb != m_basicplots_tools.end(); ++itb) {
	  if ( (*itb).name() == basictoolname) {
	    sc = (*itb)->execute(&(**itd));
	  }
	}
      }
    }
  }

  //loop over names and find the right MissingETData object and calling the right tool
  for (std::vector<std::vector<std::string> >::iterator it =  m_data_resolution_pairs.begin(); it != m_data_resolution_pairs.end(); ++it) {

    std::string dataname = (*it).at(0);
    std::string resolutionname = (*it).at(1);

    for (std::vector<ToolHandle<MissingETData> >::iterator itd = m_the_data.begin();itd != m_the_data.end(); ++itd) {
      if ( (*itd).name() == dataname) {

	for (std::vector<ToolHandle<ResolutionTool> >::iterator itb = m_resolution_tools.begin();itb != m_resolution_tools.end(); ++itb) {
	  if ( (*itb).name() == resolutionname) {
	    sc = (*itb)->execute(&(**itd));
	  }
	}
      }
    }
  }

  //loop over names and find the right MissingETData object and calling the right tool
  for (std::vector<std::vector<std::string> >::iterator it =  m_data_linearity_pairs.begin(); it != m_data_linearity_pairs.end(); ++it) {

    std::string dataname = (*it).at(0);
    std::string linearityname = (*it).at(1);

    for (std::vector<ToolHandle<MissingETData> >::iterator itd = m_the_data.begin();itd != m_the_data.end(); ++itd) {
      if ( (*itd).name() == dataname) {

	for (std::vector<ToolHandle<LinearityTool> >::iterator itb = m_linearity_tools.begin();itb != m_linearity_tools.end(); ++itb) {
	  if ( (*itb).name() == linearityname) {
	    sc = (*itb)->execute(&(**itd));
	  }
	}
      }
    }
  }

  sc = comparatorPlots(data);

  if (sc.isFailure()) { msg() << MSG::WARNING << "ContainerComparatorTool execute() fail" << endmsg; }

  return sc;
}

StatusCode ContainerComparatorTool::comparatorPlots(MissingETData *data) {

  msg() << MSG::DEBUG << "in comparatorPlots() " << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  std::vector<std::pair<const MissingET *, const MissingET *> > METpairs;

  for (std::vector<std::vector<std::string> >::iterator container_it = m_containers_to_compare.begin(); container_it != m_containers_to_compare.end(); ++container_it) {
    for (std::vector<std::string>::iterator string_it = container_it->begin(); string_it != container_it->end(); ++string_it) {
      for (std::vector<std::string>::iterator string_it2 = string_it + 1; string_it2 != container_it->end(); ++string_it2) {

	const MissingET *met  = data->getMissingETbyName( *string_it );
	const MissingET *met2  = data->getMissingETbyName( *string_it2 );
	METpairs.push_back(std::pair<const MissingET *, const MissingET *>(met, met2));
      }//loop over strings = container names to compare
    }//loop over strings = container names to compare
  }//loop over container lists

  int num = 0;
  for (std::vector<std::pair<const MissingET*, const MissingET *> >::iterator pair_it = METpairs.begin(); pair_it != METpairs.end(); ++pair_it, ++num) {
    m_diffContainers.at(num)->fillHistos( (*pair_it) );
    m_vsContainers.at(num)->fillHistos( (*pair_it) );
  }

  if (sc.isFailure()) { msg() << MSG::WARNING << "ContainerComparatorTool execute() fail" << endmsg; }

  return sc;
}

//------------------------------------------------------------------------------
StatusCode ContainerComparatorTool::finalize() {
  return StatusCode::SUCCESS;
}

ContainerComparatorTool::DiffContainers::DiffContainers(const std::string &s, const std::string &s2,ITHistSvc * thistSvc) 
{
  StatusCode sc = StatusCode::SUCCESS;

  m_met = new TH1D((s + "_minus_" + s2 + "_MET").c_str(),(s + "_minus_" + s2 + "_MET").c_str(),1000,-100.,100.);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_MET",m_met);

  m_metx = new TH1D((s + "_minus_" + s2 + "_METx").c_str(),(s + "_minus_" + s2 + "_METx").c_str(),1000,-100.,100.);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_METx",m_metx);

  m_mety = new TH1D((s + "_minus_" + s2 + "_METy").c_str(),(s + "_minus_" + s2 + "_METy").c_str(),1000,-100.,100.);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_METy",m_mety);

  m_metphi = new TH1D(("dphi" + s + "_" + s2).c_str(),("dphi_" + s + "_" + s2).c_str(),630,0.,6.3);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/dphi" + s + "_" + s2,m_metphi);

  m_set = new TH1D((s + "_minus_" + s2 + "_SET").c_str(),(s + "_minus_" + s2 + "_SET").c_str(),1000,-500.,500.);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_SET",m_set);

  if (sc.isFailure()) {}
}

ContainerComparatorTool::VsContainers::VsContainers(const std::string &s,const std::string &s2,ITHistSvc * thistSvc) 
{
  StatusCode sc = StatusCode::SUCCESS;

  m_met = new TH2D((s + "_vs_" + s2 + "_MET").c_str(),(s + "_vs_" + s2 + "_MET").c_str(),200,0.,1000., 200,0.,1000. );
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_MET",m_met);

  m_metx = new TH2D((s + "_vs_" + s2 + "_METx").c_str(),(s + "_vs_" + s2 + "_METx").c_str(),200,-500.,500.,200,-500.,500.);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_METx",m_metx);

  m_mety = new TH2D((s + "_vs_" + s2 + "_METy").c_str(),(s + "_vs_" + s2 + "_METy").c_str(),200,-500.,500.,200,-500.,500.);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_METy",m_mety);

  m_metphi = new TH2D((s + "_vs_" + s2 + "_METphi").c_str(),(s + "_vs_" + s2 + "_METphi").c_str(),63,-3.15,3.15,63,-3.15,3.15);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_METphi",m_metphi);

  m_set = new TH2D((s + "_vs_" + s2 + "_SET").c_str(),(s + "_vs_" + s2 + "_SET").c_str(),200,0.,2000.,200,0.,2000.);
  sc = thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_SET",m_set);

  if (sc.isFailure()) {}
}

ContainerComparatorTool::DiffContainers::~DiffContainers() {
  delete m_met;
  delete m_metx;
  delete m_mety;
  delete m_metphi;
  delete m_set;
}

ContainerComparatorTool::VsContainers::~VsContainers() {
  delete m_met;
  delete m_metx;
  delete m_mety;
  delete m_metphi;
  delete m_set;
}

void ContainerComparatorTool::DiffContainers::fillHistos(std::pair<const MissingET *, const MissingET *> pair_met) {
  float dphi = fabs(pair_met.first->phi() - pair_met.second->phi());
  if (dphi > M_PI) {dphi = fabs(dphi - 2*M_PI);}

  m_met->Fill((sqrt(pow(pair_met.first->etx(),2) + pow(pair_met.first->ety(),2)) - sqrt(pow(pair_met.second->etx(),2) + pow(pair_met.second->ety(),2)))/GeV);
  m_metx->Fill((pair_met.first->etx() - pair_met.second->etx())/GeV );
  m_mety->Fill((pair_met.first->ety() - pair_met.second->ety())/GeV );
  m_metphi->Fill(dphi);
  m_set->Fill((pair_met.first->sumet() - pair_met.second->sumet())/GeV);
}

void ContainerComparatorTool::VsContainers::fillHistos(std::pair<const MissingET *, const MissingET *> pair_met) {
  m_met->Fill(sqrt(pow(pair_met.first->etx(),2) + pow(pair_met.first->ety(),2))/GeV , sqrt(pow(pair_met.second->etx(),2) + pow(pair_met.second->ety(),2))/GeV);
  m_metx->Fill(pair_met.first->etx()/GeV , pair_met.second->etx()/GeV );
  m_mety->Fill(pair_met.first->ety()/GeV , pair_met.second->ety()/GeV );
  m_metphi->Fill( pair_met.first->phi() , pair_met.second->phi() );
  m_set->Fill(pair_met.first->sumet()/GeV , pair_met.second->sumet()/GeV);
}
