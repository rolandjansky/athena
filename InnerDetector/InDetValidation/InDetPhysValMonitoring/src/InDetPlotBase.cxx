/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPlotBase.cxx
 * @author shaun roe
**/

#include "InDetPlotBase.h"
//bring Gaudi utilities in scope
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
//#include <iostream>

//to retrieve HistogramDefinitionSvc
#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"


InDetPlotBase::InDetPlotBase(InDetPlotBase * pParent, const std::string & dirName):
	PlotBase(pParent, dirName),m_msg("InDetPlotBase"),m_histoDefSvc(nullptr){
	//nop
}

void
InDetPlotBase::book (TH1 * & pHisto, const SingleHistogramDefinition & hd){
  pHisto=Book1D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, false);
  return;
}

void
InDetPlotBase::book (TProfile * & pHisto, const SingleHistogramDefinition & hd){
  pHisto=BookTProfile(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, hd.yAxis.first,hd.yAxis.second,false);
  return;
}

void 
InDetPlotBase::book(TH1 * & pHisto, const std::string & histoIdentifier,const std::string & folder){
  const SingleHistogramDefinition hd=retrieveDefinition(histoIdentifier,folder);
  if (hd.empty() ) ATH_MSG_WARNING("Histogram definition is empty for identifier "<<histoIdentifier);
  book(pHisto,hd);
  return;
}

void 
InDetPlotBase::book(TProfile * & pHisto, const std::string & histoIdentifier,const std::string & folder){
  const SingleHistogramDefinition hd=retrieveDefinition(histoIdentifier,folder);
  if (hd.empty() ) ATH_MSG_WARNING("Histogram definition is empty for identifier "<<histoIdentifier);
  book(pHisto,hd);
  return;
}

void 
InDetPlotBase::book(TH2 * & pHisto, const std::string & histoIdentifier,const std::string & folder){
  const SingleHistogramDefinition hd=retrieveDefinition(histoIdentifier,folder);
  if (hd.empty() ) ATH_MSG_WARNING("Histogram definition is empty for identifier "<<histoIdentifier);
  book(pHisto,hd);
  return;
}

void
InDetPlotBase::book (TH2 * & pHisto, const SingleHistogramDefinition & hd){
  pHisto=Book2D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, hd.nBinsY, hd.yAxis.first,hd.yAxis.second,false);
  return;
}

SingleHistogramDefinition
InDetPlotBase::retrieveDefinition(const std::string & histoIdentifier, const std::string & folder){
	SingleHistogramDefinition s; //invalid result
	if (not m_histoDefSvc){
		ISvcLocator* svcLoc = Gaudi::svcLocator();
		StatusCode sc = svcLoc->service("HistogramDefinitionSvc",m_histoDefSvc);
		if (sc.isFailure()) {
		  ATH_MSG_WARNING("failed to retrieve HistogramDefinitionSvc in "<<__FILE__);
		  return s;
		}
	}
	bool folderDefault=(folder.empty() or folder=="default");
	s= m_histoDefSvc->definition(histoIdentifier, folder);
	//"default" and empty string should be equivalent
	if (folderDefault and s.empty()) {
		const std::string otherDefault=(folder.empty())?("default"):"";
	  s= m_histoDefSvc->definition(histoIdentifier, otherDefault);
	}
	if (s.empty()) ATH_MSG_WARNING("Histogram definition is empty for identifier "<<histoIdentifier);
	return s;
}