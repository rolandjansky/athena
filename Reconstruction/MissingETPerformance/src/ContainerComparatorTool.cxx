/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

//#include "CLHEP/Units/PhysicalConstants.h"

#include "MissingETPerformance/ContainerComparatorTool.h"

//------------------------------------------------------------------------------
ContainerComparatorTool::ContainerComparatorTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) : AthAlgTool( type, name, parent ) {

  declareInterface<ContainerComparatorTool>( this );
  declareProperty("containers_to_compare",   containers_to_compare);
  declareProperty("Data_BasicTool_pairs", data_basictool_pairs);
  declareProperty("Data_Resolution_pairs", data_resolution_pairs);
  declareProperty("Data_Linearity_pairs", data_linearity_pairs);

  declareProperty("MissingETData_array", the_data);

  declareProperty("BasicPlotsTools", basicplots_tools);
  declareProperty("ResolutionTools", resolution_tools);
  declareProperty("LinearityTools", linearity_tools);
}

//------------------------------------------------------------------------------

StatusCode ContainerComparatorTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  for (std::vector<std::vector<std::string> >::iterator container_it = containers_to_compare.begin(); container_it != containers_to_compare.end(); ++container_it) {
    for (std::vector<std::string>::iterator string_it = container_it->begin(); string_it != container_it->end(); ++string_it) {
      for (std::vector<std::string>::iterator string_it2 = string_it + 1; string_it2 != container_it->end(); ++string_it2) {
      ContainerComparatorTool::_diffContainers *df = new ContainerComparatorTool::_diffContainers( *string_it , *string_it2 ,m_thistSvc );
      diffContainers.push_back(df);

      ContainerComparatorTool::_vsContainers *vs = new ContainerComparatorTool::_vsContainers( *string_it, *string_it2, m_thistSvc );
      vsContainers.push_back(vs);
      }//loop over strings = container names to compare
    }//loop over strings = container names to compare
  }//loop over container lists

  for (std::vector<ToolHandle<BasicPlotsTool> >::iterator it = basicplots_tools.begin();it != basicplots_tools.end(); ++it) {
    sc = (*it).retrieve();
    sc = (*it)->CBNT_initialize();
  }
  for (std::vector<ToolHandle<ResolutionTool> >::iterator it = resolution_tools.begin();it != resolution_tools.end(); ++it) {
    sc = (*it).retrieve();
    sc = (*it)->CBNT_initialize();
  }
  for (std::vector<ToolHandle<LinearityTool> >::iterator it = linearity_tools.begin();it != linearity_tools.end(); ++it) {
    sc = (*it).retrieve();
    sc = (*it)->CBNT_initialize();
  }

  if (sc.isFailure()) { msg() << MSG::WARNING << "ContainerComparatorTool CBNT_initialize() fail" << endreq; }

  return sc;
}

StatusCode ContainerComparatorTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode ContainerComparatorTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  for (std::vector<ToolHandle<MissingETData> >::iterator it = the_data.begin();it != the_data.end(); ++it) {
    sc = (*it)->retrieveContainers();
  }

  //loop over names and find the right MissingETData object and calling the right tool
  for (std::vector<std::vector<std::string> >::iterator it =  data_basictool_pairs.begin(); it != data_basictool_pairs.end(); ++it) {

    std::string dataname = (*it).at(0);
    std::string basictoolname = (*it).at(1);

    for (std::vector<ToolHandle<MissingETData> >::iterator itd = the_data.begin();itd != the_data.end(); ++itd) {
      if ( (*itd).name() == dataname) {

	for (std::vector<ToolHandle<BasicPlotsTool> >::iterator itb = basicplots_tools.begin();itb != basicplots_tools.end(); ++itb) {
	  if ( (*itb).name() == basictoolname) {
	    sc = (*itb)->execute(&(**itd));
	  }
	}
      }
    }
  }

  //loop over names and find the right MissingETData object and calling the right tool
  for (std::vector<std::vector<std::string> >::iterator it =  data_resolution_pairs.begin(); it != data_resolution_pairs.end(); ++it) {

    std::string dataname = (*it).at(0);
    std::string resolutionname = (*it).at(1);

    for (std::vector<ToolHandle<MissingETData> >::iterator itd = the_data.begin();itd != the_data.end(); ++itd) {
      if ( (*itd).name() == dataname) {

	for (std::vector<ToolHandle<ResolutionTool> >::iterator itb = resolution_tools.begin();itb != resolution_tools.end(); ++itb) {
	  if ( (*itb).name() == resolutionname) {
	    sc = (*itb)->execute(&(**itd));
	  }
	}
      }
    }
  }

  //loop over names and find the right MissingETData object and calling the right tool
  for (std::vector<std::vector<std::string> >::iterator it =  data_linearity_pairs.begin(); it != data_linearity_pairs.end(); ++it) {

    std::string dataname = (*it).at(0);
    std::string linearityname = (*it).at(1);

    for (std::vector<ToolHandle<MissingETData> >::iterator itd = the_data.begin();itd != the_data.end(); ++itd) {
      if ( (*itd).name() == dataname) {

	for (std::vector<ToolHandle<LinearityTool> >::iterator itb = linearity_tools.begin();itb != linearity_tools.end(); ++itb) {
	  if ( (*itb).name() == linearityname) {
	    sc = (*itb)->execute(&(**itd));
	  }
	}
      }
    }
  }

  sc = comparatorPlots(data);

  if (sc.isFailure()) { msg() << MSG::WARNING << "ContainerComparatorTool execute() fail" << endreq; }

  return sc;
}

StatusCode ContainerComparatorTool::comparatorPlots(MissingETData *data) {

  msg() << MSG::DEBUG << "in comparatorPlots() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  std::vector<std::pair<const MissingET *, const MissingET *> > METpairs;

  for (std::vector<std::vector<std::string> >::iterator container_it = containers_to_compare.begin(); container_it != containers_to_compare.end(); ++container_it) {
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
    diffContainers.at(num)->fillHistos( (*pair_it) );
    vsContainers.at(num)->fillHistos( (*pair_it) );
  }

  if (sc.isFailure()) { msg() << MSG::WARNING << "ContainerComparatorTool execute() fail" << endreq; }

  return sc;
}

//------------------------------------------------------------------------------
StatusCode ContainerComparatorTool::finalize() {
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
ContainerComparatorTool::~ContainerComparatorTool() {}

ContainerComparatorTool::_diffContainers::_diffContainers(const std::string &s, const std::string &s2,ITHistSvc * m_thistSvc) 
{
  StatusCode sc = StatusCode::SUCCESS;

  met = new TH1D((s + "_minus_" + s2 + "_MET").c_str(),(s + "_minus_" + s2 + "_MET").c_str(),1000,-100.,100.);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_MET",met);

  metx = new TH1D((s + "_minus_" + s2 + "_METx").c_str(),(s + "_minus_" + s2 + "_METx").c_str(),1000,-100.,100.);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_METx",metx);

  mety = new TH1D((s + "_minus_" + s2 + "_METy").c_str(),(s + "_minus_" + s2 + "_METy").c_str(),1000,-100.,100.);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_METy",mety);

  metphi = new TH1D(("dphi" + s + "_" + s2).c_str(),("dphi_" + s + "_" + s2).c_str(),630,0.,6.3);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/dphi" + s + "_" + s2,metphi);

  set = new TH1D((s + "_minus_" + s2 + "_SET").c_str(),(s + "_minus_" + s2 + "_SET").c_str(),1000,-500.,500.);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_minus_" + s2 + "_SET",set);

  if (sc.isFailure()) {}
}

ContainerComparatorTool::_vsContainers::_vsContainers(const std::string &s,const std::string &s2,ITHistSvc * m_thistSvc) 
{
  StatusCode sc = StatusCode::SUCCESS;

  met = new TH2D((s + "_vs_" + s2 + "_MET").c_str(),(s + "_vs_" + s2 + "_MET").c_str(),200,0.,1000., 200,0.,1000. );
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_MET",met);

  metx = new TH2D((s + "_vs_" + s2 + "_METx").c_str(),(s + "_vs_" + s2 + "_METx").c_str(),200,-500.,500.,200,-500.,500.);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_METx",metx);

  mety = new TH2D((s + "_vs_" + s2 + "_METy").c_str(),(s + "_vs_" + s2 + "_METy").c_str(),200,-500.,500.,200,-500.,500.);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_METy",mety);

  metphi = new TH2D((s + "_vs_" + s2 + "_METphi").c_str(),(s + "_vs_" + s2 + "_METphi").c_str(),63,-3.15,3.15,63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_METphi",metphi);

  set = new TH2D((s + "_vs_" + s2 + "_SET").c_str(),(s + "_vs_" + s2 + "_SET").c_str(),200,0.,2000.,200,0.,2000.);
  sc = m_thistSvc->regHist("/AANT/ContainerComparator/"+ s + "_and_" + s2 + "/" + s + "_vs_" + s2 + "_SET",set);

  if (sc.isFailure()) {}
}

ContainerComparatorTool::_diffContainers::~_diffContainers() {
  if (met) delete met;
  if (metx) delete metx;
  if (mety) delete mety;
  if (metphi) delete metphi;
  if (set) delete set;
}

ContainerComparatorTool::_vsContainers::~_vsContainers() {
  if (met) delete met;
  if (metx) delete metx;
  if (mety) delete mety;
  if (metphi) delete metphi;
  if (set) delete set;
}

void ContainerComparatorTool::_diffContainers::fillHistos(std::pair<const MissingET *, const MissingET *> pair_met) {
  float dphi = fabs(pair_met.first->phi() - pair_met.second->phi());
  if (dphi > M_PI) {dphi = fabs(dphi - 2*M_PI);}

  met->Fill((sqrt(pow(pair_met.first->etx(),2) + pow(pair_met.first->ety(),2)) - sqrt(pow(pair_met.second->etx(),2) + pow(pair_met.second->ety(),2)))/1000.);
  metx->Fill((pair_met.first->etx() - pair_met.second->etx())/1000. );
  mety->Fill((pair_met.first->ety() - pair_met.second->ety())/1000. );
  metphi->Fill(dphi);
  set->Fill((pair_met.first->sumet() - pair_met.second->sumet())/1000.);
}

void ContainerComparatorTool::_vsContainers::fillHistos(std::pair<const MissingET *, const MissingET *> pair_met) {
  met->Fill(sqrt(pow(pair_met.first->etx(),2) + pow(pair_met.first->ety(),2))/1000. , sqrt(pow(pair_met.second->etx(),2) + pow(pair_met.second->ety(),2))/1000.);
  metx->Fill(pair_met.first->etx()/1000. , pair_met.second->etx()/1000. );
  mety->Fill(pair_met.first->ety()/1000. , pair_met.second->ety()/1000. );
  metphi->Fill( pair_met.first->phi() , pair_met.second->phi() );
  set->Fill(pair_met.first->sumet()/1000. , pair_met.second->sumet()/1000.);
}
