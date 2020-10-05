/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBDATA_CSCCALIBREPORTSLOPE_H
#define CSCCALIBDATA_CSCCALIBREPORTSLOPE_H

/**************************************************************************
Package: MuonSpectrometer/MuonCalib/CscCalib/CscCalibData
Name: CscCalibReportSlope.h
Author: Caleb Parnell-Lampen
Date & Place: July 7, 2008, University of Arizona

Holds info corresponding to the calibration process, such as applicable plots
and graphs. The monitoring system can pick out what graphs it wants.
 ****************************************************************************/
#include "CscCalibData/CscCalibReportBase.h"
#include "AthContainers/DataVector.h"

#include <map>
#include <set>

class TProfile;
class TGraphErrors;
class TH1I;

class CscCalibReportSlope : public CscCalibReportBase
{

  public:

    /*class to record changed bit. As of 2008-09-08 is only used for changed 
      /dead chan status
      struct bitChange 
      {
      int hash;
      bool newVal;
      int pulserLevel;

      bitChange() {
      hash = -9999;
      newVal = true;
      pulserLevel = -9999;
      }

      };
     */

    /* default constructor */
    CscCalibReportSlope();

    /* full constructor */
    CscCalibReportSlope(std::string label); 

    ~CscCalibReportSlope();

    void setBitHists(DataVector<TH1I> * somePedAmpHists);
    /** setCalGraphs - Set the pedAmpHist vector.  
      @param someCalGraphs - Pointer to DataVector of calibration graphs. Takes ownership.
     */
    void setCalGraphs(DataVector<TGraphErrors> * someCalGraphs);

    /*setDeadChanges - set the changed dead channels
      @param someDeadChanges - Pointer to map of changed channels and their new value

      void setDeadChanges(std::set<bitChange> * someDeadChannels);
     */

    /**setAmpProfs - set the changed dead channels
      @param someAmpProfs - Pointer to map of changed channels and their new value
     */
    void setAmpProfs(std::map<int,TProfile*> * someAmpProfs);

    /**setPulsedChambers - set the pulsed chambers
      @param somePulsedChambers - Pointer to set of pulsed chambers. Takes ownership.
     */
    void setPulsedChambers(std::set<int> * somePulsedChambers);

    void setFitResults(std::vector<float> * fitResults);


    /**Retrive bit histogram vector*/
    const DataVector<TH1I> * getBitHists() const;        

    /**Retrieve pedestal amplitude histogram vector*/
    const DataVector<TGraphErrors> * getCalGraphs() const;

    /*get the set of changes in dead channels*
      const std::set<bitChange> * getDeadChanges() const;
     */

    /**getAmpProfs - get the map of ampProfs stored. These show the amplitudes each channel showed at a given
      pulserLevel*/
    const std::map<int,TProfile*> * getAmpProfs() const;


    /**getPulsedChambers - Contains the chambers that were pulsed. Stored as hash Ids*/
    const std::set<int> * getPulsedChambers() const;


    /**getFitReturns - Contains slope fit results.*/
    const std::vector<float> * getFitResults() const;


  private:

    //Pedestal amplitude histograms
    DataVector<TGraphErrors> * m_calGraphs;

    DataVector<TH1I> * m_bitHists;
    //status changes
    //std::set<bitChange> * m_deadChanges;

    //Interesting TProfiles grouped by attenuation level
    std::map<int,TProfile*> * m_ampProfs;

    std::set<int> * m_pulsedChambers;

    std::vector<float> * m_fitResults;


};

#endif
