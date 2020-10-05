/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalibData/CscCalibReportSlope.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TH1I.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include <string>
#include <map>
#include <set>

/* default constructor */
CscCalibReportSlope::CscCalibReportSlope() : 
  m_calGraphs(nullptr),
  m_bitHists(nullptr),
  m_ampProfs(nullptr),
  m_pulsedChambers(nullptr),
  m_fitResults(nullptr)
{ }

/* full constructor */
CscCalibReportSlope::CscCalibReportSlope(std::string label) :  
  CscCalibReportBase::CscCalibReportBase(label),
  m_calGraphs(nullptr),
  m_bitHists(nullptr),
  m_ampProfs(nullptr),
  m_pulsedChambers(nullptr),
  m_fitResults(nullptr)
{ }

CscCalibReportSlope::~CscCalibReportSlope()
{
  //Datavector, so it will delete its own contents
  delete m_calGraphs;
  delete m_bitHists;
  delete m_ampProfs;
  delete m_pulsedChambers;
  delete m_fitResults;
}


void CscCalibReportSlope::setBitHists(DataVector<TH1I> * someBitHists)
{
    m_bitHists = someBitHists;
}

const DataVector<TH1I> * CscCalibReportSlope::getBitHists() const
{
    return m_bitHists;
}

void CscCalibReportSlope::setCalGraphs(DataVector<TGraphErrors> * someCalGraphs)
{
  if(m_calGraphs) {
    MsgStream log(Athena::getMessageSvc(),"CscCalibReportSlope");
    log<<MSG::WARNING<<"Writing over already existing calGraphs in report!"<<endmsg;
  }
    
  m_calGraphs = someCalGraphs;
}

const DataVector<TGraphErrors> * CscCalibReportSlope::getCalGraphs() const
{
  return m_calGraphs;
}

void CscCalibReportSlope::setAmpProfs( std::map<int,TProfile *> * someAmpProfs )
{
  if(m_ampProfs) {
    MsgStream log(Athena::getMessageSvc(),"CscCalibReportSlope");
    log<<MSG::WARNING<<"writing over already existing dead changes in report!"<<endmsg;
  }
  m_ampProfs = someAmpProfs;
}

void CscCalibReportSlope::setFitResults( std::vector<float> * someFitResults){
  m_fitResults = someFitResults;
}

const std::map<int,TProfile*> * CscCalibReportSlope::getAmpProfs() const
{
  return m_ampProfs;
}

void CscCalibReportSlope::setPulsedChambers( std::set<int> * somePulsedChambers)
{
  if(m_pulsedChambers) {
    MsgStream log(Athena::getMessageSvc(),"CscCalibReportSlope");
    log<<MSG::WARNING<<"Writing over previously existing pulsed chambers!"<<endmsg;
  }
  m_pulsedChambers = somePulsedChambers;
}
    

const std::set<int> * CscCalibReportSlope::getPulsedChambers() const
{
  return m_pulsedChambers;
}

const std::vector<float> * CscCalibReportSlope::getFitResults() const{
  return m_fitResults;
}

