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
  calGraphs(NULL),
    bitHists(NULL),
  //deadChanges(NULL),
  ampProfs(NULL),
  pulsedChambers(NULL),
  fitResults(NULL)
{ }

/* full constructor */
CscCalibReportSlope::CscCalibReportSlope(std::string label) :  
  CscCalibReportBase::CscCalibReportBase(label),
  calGraphs(NULL),
  //deadChanges(NULL),
    bitHists(NULL),
  ampProfs(NULL),
  pulsedChambers(NULL),
  fitResults(NULL)
{ }

CscCalibReportSlope::~CscCalibReportSlope()
{
  //Datavector, so it will delete its own contents
  delete calGraphs;

  delete bitHists;

  //Doesn't contain pointers, so this is also safe
  //delete deadChanges;

  delete ampProfs;
  delete pulsedChambers;
  delete fitResults;
  /*
  //ampProfs needs to have contents explicitly deleted.
  if(ampProfs)
  {
    std::map<int,TProfile*>::iterator profItr = ampProfs->begin();
    std::map<int,TProfile*>::iterator profEnd = ampProfs->end();
    for(;profItr != profEnd; profItr++)
      delete profItr->second;
    delete ampProfs;
    delete pulsedChambers;
  }
  */
}


void CscCalibReportSlope::setBitHists(DataVector<TH1I> * someBitHists)
{
    bitHists = someBitHists;
}

const DataVector<TH1I> * CscCalibReportSlope::getBitHists() const
{
    return bitHists;
}

void CscCalibReportSlope::setCalGraphs(DataVector<TGraphErrors> * someCalGraphs)
{
  if(calGraphs)
    std::cout << "CscCalibReportSlope  WARNING      Writing over already existing calGraphs in report!" << std::endl;
    
  calGraphs = someCalGraphs;
}

const DataVector<TGraphErrors> * CscCalibReportSlope::getCalGraphs() const
{
  return calGraphs;
}


/*void CscCalibReportSlope::setDeadChanges( std::set<CscCalibReportSlope::bitChange> * someDeadChanges )
{
  if(deadChanges)
    std::cout << "CscCalibReportSlope  WARNING       Attempt was made to write over already existing dead changes in report" << std::endl;

  deadChanges = someDeadChanges;
}*/

void CscCalibReportSlope::setAmpProfs( std::map<int,TProfile *> * someAmpProfs )
{
  if(ampProfs)
    std::cout << "CscCalibReportSlope  WARNING        writing over already existing dead changes in report" << std::endl;
  ampProfs = someAmpProfs;
}

/*
const std::set<CscCalibReportSlope::bitChange> * CscCalibReportSlope::getDeadChanges() const
{
  return deadChanges;
}*/

void CscCalibReportSlope::setFitResults( std::vector<float> * someFitResults){
  fitResults = someFitResults;
}

const std::map<int,TProfile*> * CscCalibReportSlope::getAmpProfs() const
{
  return ampProfs;
}

void CscCalibReportSlope::setPulsedChambers( std::set<int> * somePulsedChambers)
{
  if(pulsedChambers)
    std::cout << "CscCalibReportSlope WARNING     Writing over previously existing pulsed chambers" << std::endl;
  pulsedChambers = somePulsedChambers;
}
    

const std::set<int> * CscCalibReportSlope::getPulsedChambers() const
{
  return pulsedChambers;
}

const std::vector<float> * CscCalibReportSlope::getFitResults() const{
  return fitResults;
}

