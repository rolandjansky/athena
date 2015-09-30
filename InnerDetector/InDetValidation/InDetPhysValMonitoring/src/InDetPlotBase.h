/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPLOTBASE
#define INDETPHYSVALMONITORING_INDETPLOTBASE
/**
 * @file InDetPlotBase.h
 * @author shaun roe
 * @date 11/9/2015
**/
#include "TrkValHistUtils/PlotBase.h"
#include "AthenaKernel/MsgStreamMember.h" 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h" //to make available to children

#include <string>

#define IDPVM_BOOK(name) mBook(name, #name)

class IHistogramDefinitionSvc;


//mixin class to give extra capabilities to plots
class InDetPlotBase:public PlotBase{
public:
  void book (TH1 * & pHisto, const SingleHistogramDefinition & hd);
  void book (TProfile * & pHisto, const SingleHistogramDefinition & hd);
	InDetPlotBase(InDetPlotBase * pParent, const std::string & dirName);
	SingleHistogramDefinition retrieveDefinition(const std::string & histoIdentifier, const std::string & folder = "default");
	void book(TH1 * & pHisto, const std::string & histoIdentifier, const std::string & folder = "default");
	void book(TProfile * & pHisto, const std::string & histoIdentifier, const std::string & folder = "default");
protected:
  /// book, for use by macro
  template <class T>
  void mBook(T & pHisto, const std::string & histoIdentifier) {
    return book(pHisto,histoIdentifier.substr(2));
  }
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg.get() << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

private:
	IHistogramDefinitionSvc * m_histoDefSvc;
};



#endif
