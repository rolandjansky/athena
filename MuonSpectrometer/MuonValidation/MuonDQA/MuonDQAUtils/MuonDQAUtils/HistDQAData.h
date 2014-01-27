/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDQAHist_HistDQAData_H
#define MuonDQAHist_HistDQAData_H 

#include <string>

namespace MuonDQAHist {

  class HistDQAData {
  public:
    /** @brief constructor taking number of bins + hist range. Title will be initialized as "" */
    HistDQAData( unsigned nbins, double min, double max);

    /** @brief constructor taking number of bins + hist range + titles */
    HistDQAData( unsigned nbins, double min, double max, std::string title, std::string xAxisTitle, std::string yAxisTitle );
    
    /** @brief destructor */
    virtual ~HistDQAData() {}
    
    /** @brief returns the title of the histogram */
    std::string title() const { return m_title; }

    /** @brief returns the title of the x-axis of the histogram */
    std::string xAxisTitle() const { return m_xTitle; }

    /** @brief returns the title of the y-axis of the histogram */
    std::string yAxisTitle() const { return m_yTitle; }

    /** @brief returns number of bins of the x-axis of the histogram */
    unsigned int xBins() const { return m_xBins; }

    /** @brief returns lower range of the x-axis of the histogram */
    double xMin() const { return m_xMin; }

    /** @brief returns lower range of the x-axis of the histogram */
    double xMax() const { return m_xMax; }

    /** @brief returns the dimension of the histogram */
    virtual unsigned int dimension() const { return 1; }

    /** @brief clone HistDQAData */
    virtual HistDQAData* clone() const { return new HistDQAData(*this); }
    
  protected:
    unsigned int m_xBins;
    double m_xMin;
    double m_xMax;
    std::string m_title;
    std::string m_xTitle;
    std::string m_yTitle;
  };

  class HistDQAData2D : public HistDQAData {
  public:
    /** @brief constructor taking number of bins + hist range. Title will be initialized as "" */
    HistDQAData2D( unsigned int xbins, double xmin, double xmax, unsigned int ybins, double ymin, double ymax );

    /** @brief constructor taking number of bins + hist range + titles */
    HistDQAData2D( unsigned int xbins, double xmin, double xmax, unsigned int ybins, double ymin, double ymax, 
		   std::string title, std::string xAxisTitle, std::string yAxisTitle );

    /** @brief constructor taking a HistDQAData object + number of bins + hist range + title of the y-axis. */
    HistDQAData2D( const HistDQAData& xHistData, unsigned int ybins, double ymin, double ymax, std::string yAxisTitle  );

    /** @brief constructor taking two HistDQAData objects. */
    HistDQAData2D( const HistDQAData& xHistData, const HistDQAData& yHistData ); 

    /** @brief returns number of bins of the x-axis of the histogram */
    unsigned int yBins() const { return m_yBins; }

    /** @brief returns lower range of the x-axis of the histogram */
    double yMin() const { return m_yMin; }

    /** @brief returns lower range of the x-axis of the histogram */
    double yMax() const { return m_yMax; }

    /** @brief returns the dimension of the histogram */
    unsigned int dimension() const { return 2; }

    /** @brief clone HistDQAData */
    virtual HistDQAData2D* clone() const { return new HistDQAData2D(*this); }

  private:
    unsigned int m_yBins;
    double m_yMin;
    double m_yMax;
    
  };

}



#endif


