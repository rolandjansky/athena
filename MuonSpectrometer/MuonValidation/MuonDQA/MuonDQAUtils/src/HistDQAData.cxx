/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/HistDQAData.h"

#include "MuonDQAUtils/HistDQAData.h"


namespace MuonDQAHist {

  HistDQAData::HistDQAData( unsigned bins, double min, double max) : 
    m_xBins(bins),m_xMin(min), m_xMax(max), m_title(""),m_xTitle(""),m_yTitle("")
  {

  }

  HistDQAData::HistDQAData( unsigned bins, double min, double max, std::string title, std::string xAxisTitle, std::string yAxisTitle ): 
    m_xBins(bins),m_xMin(min), m_xMax(max), m_title(title),m_xTitle(xAxisTitle),m_yTitle(yAxisTitle)
  {

  }

  HistDQAData2D::HistDQAData2D( unsigned int xbins, double xmin, double xmax, unsigned int ybins, double ymin, double ymax )  : 
    HistDQAData(xbins,xmin,xmax), m_yBins(ybins), m_yMin(ymin), m_yMax(ymax)
  {

  }

  HistDQAData2D::HistDQAData2D( unsigned int xbins, double xmin, double xmax, unsigned int ybins, double ymin, double ymax, 
				std::string title, std::string xAxisTitle, std::string yAxisTitle ) : 
    HistDQAData(xbins,xmin,xmax,title,xAxisTitle,yAxisTitle), m_yBins(ybins), m_yMin(ymin), m_yMax(ymax)
  {

  }

  HistDQAData2D::HistDQAData2D( const HistDQAData& xHistData, unsigned int ybins, double ymin, double ymax, std::string yAxisTitle ) :
    HistDQAData(xHistData), m_yBins(ybins), m_yMin(ymin), m_yMax(ymax)
  {
    m_yTitle = yAxisTitle;
  }

  HistDQAData2D::HistDQAData2D( const HistDQAData& xHistData, const HistDQAData& yHistData ) :
    HistDQAData(xHistData), m_yBins(yHistData.xBins()), m_yMin(yHistData.xMin()), m_yMax(yHistData.xMax())
  {
    m_title += yHistData.title();
    m_yTitle = yHistData.xAxisTitle();
  }

}
 
