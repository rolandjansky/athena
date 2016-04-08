/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGraphErrors.h"
#include "TProfile.h"


#include "CscCalibData/CscCalibReportSlope.h"
#include "DataModel/DataVector.h"
#include <string>
#include <map>
#include <set>

/* default constructor */
CscCalibReportSlope::CscCalibReportSlope() : 
  m_calGraphs(NULL),
    m_bitHists(NULL),
  //m_deadChanges(NULL),
  m_ampProfs(NULL),
  m_pulsedChambers(NULL),
  m_fitResults(NULL)
{ }

/* full constructor */
CscCalibReportSlope::CscCalibReportSlope(std::string label) :  
  CscCalibReportBase::CscCalibReportBase(label),
  m_calGraphs(NULL),
  //m_deadChanges(NULL),
    m_bitHists(NULL),
  m_ampProfs(NULL),
  m_pulsedChambers(NULL),
  m_fitResults(NULL)
{ }

CscCalibReportSlope::~CscCalibReportSlope()
{
  //Datavector, so it will delete its own contents
  delete m_calGraphs;

  delete m_bitHists;

  //Doesn't contain pointers, so this is also safe
  //delete m_deadChanges;

  delete m_ampProfs;
  delete m_pulsedChambers;
  delete m_fitResults;
  /*
  //ampProfs needs to have contents explicitly deleted.
  if(ampProfs)
  {
    std::map<int,TProfile*>::iterator profItr = m_ampProfs->begin();
    std::map<int,TProfile*>::iterator profEnd = m_ampProfs->end();
    for(;profItr != profEnd; profItr++)
      delete profItr->second;
    delete m_ampProfs;
    delete m_pulsedChambers;
  }
  */
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
  if(m_calGraphs)
    std::cout << "CscCalibReportSlope  WARNING      Writing over already existing calGraphs in report!" << std::endl;
    
  m_calGraphs = someCalGraphs;
}

const DataVector<TGraphErrors> * CscCalibReportSlope::getCalGraphs() const
{
  return m_calGraphs;
}


/*void CscCalibReportSlope::setDeadChanges( std::set<CscCalibReportSlope::bitChange> * someDeadChanges )
{
  if(m_deadChanges)
    std::cout << "CscCalibReportSlope  WARNING       Attempt was made to write over already existing dead changes in report" << std::endl;

  m_deadChanges = someDeadChanges;
}*/

void CscCalibReportSlope::setAmpProfs( std::map<int,TProfile *> * someAmpProfs )
{
  if(m_ampProfs)
    std::cout << "CscCalibReportSlope  WARNING        writing over already existing dead changes in report" << std::endl;
  m_ampProfs = someAmpProfs;
}

/*
const std::set<CscCalibReportSlope::bitChange> * CscCalibReportSlope::getDeadChanges() const
{
  return m_deadChanges;
}*/

void CscCalibReportSlope::setFitResults( std::vector<float> * someFitResults){
  m_fitResults = someFitResults;
}

const std::map<int,TProfile*> * CscCalibReportSlope::getAmpProfs() const
{
  return m_ampProfs;
}

void CscCalibReportSlope::setPulsedChambers( std::set<int> * somePulsedChambers)
{
  if(m_pulsedChambers)
    std::cout << "CscCalibReportSlope WARNING     Writing over previously existing pulsed chambers" << std::endl;
  m_pulsedChambers = somePulsedChambers;
}
    

const std::set<int> * CscCalibReportSlope::getPulsedChambers() const
{
  return m_pulsedChambers;
}

const std::vector<float> * CscCalibReportSlope::getFitResults() const{
  return m_fitResults;
}

