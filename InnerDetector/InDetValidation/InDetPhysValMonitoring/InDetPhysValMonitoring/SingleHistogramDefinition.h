/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  SingleHistogramDefinition.h
//  HDef
//
//  Created by sroe on 07/07/2015.
//

#ifndef HDef_SingleHistogramDefinition_h
#define HDef_SingleHistogramDefinition_h
#include "InDetPhysValMonitoring/IHistogramDefinitionSvc.h"
#include <string>
///Almost-a-struct for holding the single histogram definition
class SingleHistogramDefinition{
public:
    typedef const std::string & Titles_t;
    typedef const unsigned int NBins_t;
    typedef const float Var_t;
    SingleHistogramDefinition();
    
    ///C'tor for 1D histograms (no Y bins or limits)
    SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, 
			      NBins_t nbinsX, Var_t xLo, Var_t xHi, 
			      Titles_t xName,Titles_t yName,Titles_t theFolder="");
    
    ///C'tor for 2D histograms (Y bins and limits)
    SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, 
			      NBins_t nbinsX,  NBins_t nbinsY, 
			      Var_t xLo, Var_t xHi, 
			      Var_t yLo, Var_t yHi, 
			      Titles_t xName, Titles_t yName, Titles_t theFolder="");

    ///C'tor for 3D histograms (Y,Z bins and limits)
    SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, 
			      NBins_t nbinsX,  NBins_t nbinsY, NBins_t nbinsZ, 
			      Var_t xLo, Var_t xHi, 
			      Var_t yLo, Var_t yHi, 
			      Var_t zLo, Var_t zHi, 
			      Titles_t xName, Titles_t yName, Titles_t zName, Titles_t theFolder="");
    
    std::string name;
    std::string histoType;
    std::string title;
    unsigned int nBinsX;
    unsigned int nBinsY;
    unsigned int nBinsZ;
    IHistogramDefinitionSvc::axesLimits_t xAxis;
    IHistogramDefinitionSvc::axesLimits_t yAxis;
    IHistogramDefinitionSvc::axesLimits_t zAxis;
    std::string xTitle;
    std::string yTitle;
    std::string zTitle;
    std::string allTitles;
    std::string folder;
    /// Is the histogram definition empty?
    bool empty() const;
    /// String representation of the histogram definition
    std::string str() const;
    /// Does the histogram definition have a valid type (TH1, TH2 etc)?
    bool validType() const;
    /// Is the histogram definition valid
    bool isValid() const;
    /// add the folder path to produce the full histogram name e.g. myFolder/myHisto
    std::string stringIndex() const;
    /// concatenate a sanitised folderName with the histogram name
    static std::string stringIndex(const std::string & thisname, const std::string & thisfolder);
    /// produce single-line representation of the titles (titles separated by the ';' delimiter)
    std::string titleDigest() const;

    bool m_empty;
};


#endif
