// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HistogramTool.h,v 1.0 2014-05-25 16:54 cburgard Exp $
/**
 * @file  HistogramTool.h
 * @author carsten burgard <cburgarc@cern.ch>
 * @date July, 2014
 * @brief tool for booking histograms

 */

#ifndef HISTOGRAMUTILS_HISTOGRAMTOOL_H
#define HISTOGRAMUTILS_HISTOGRAMTOOL_H 1

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include <string>

#include "GaudiKernel/StatusCode.h"

#include "AthenaBaseComps/AthHistogramTool.h"
#include "PATCore/IAthHistogramTool.h"

#include "ExpressionEvaluation/ExpressionParser.h"
#ifndef XAOD_ANALYSIS
#include "TrigDecisionTool/TrigDecisionTool.h"
#endif

namespace HistogramUtils {

  struct AxisDetails {
    AxisDetails();
    std::string m_title;
    std::string m_variable;
    unsigned int m_nBins;
    std::vector<double> m_binBoundaries;
    double m_min;
    double m_max;
    bool hasVariableBinning() const;
    bool isValid() const;
    bool isEmpty() const;
    void print() const;
  };


  class HistogramToolTH1: public AthHistogramTool, virtual public IAthHistogramTool {
  public:
    TH1* m_histogram;
    std::string m_histTitle;
    std::string m_histClassName;
    std::vector<std::string> m_cutStages;
    AxisDetails m_axisX;
    
    ExpressionParsing::ExpressionParser *m_parserX;
#ifndef XAOD_ANALYSIS
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
#endif

  public:
    HistogramToolTH1(const std::string& type, const std::string& name, const IInterface* parent);
    virtual StatusCode fill( const double weight = 1.0) override final;
    virtual StatusCode initialize() final override;
    virtual StatusCode finalize() final override;
  };

  class HistogramToolTH2: public AthHistogramTool, virtual public IAthHistogramTool {
  protected:
    TH2* m_histogram;
    std::string m_histTitle;
    std::string m_histClassName;
    std::vector<std::string> m_cutStages;
    AxisDetails m_axisX;
    AxisDetails m_axisY;

    ExpressionParsing::ExpressionParser *m_parserX;
    ExpressionParsing::ExpressionParser *m_parserY;
#ifndef XAOD_ANALYSIS
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
#endif

  public:
    HistogramToolTH2(const std::string& type, const std::string& name, const IInterface* parent);
    virtual StatusCode fill( const double weight = 1.0) override final;
    virtual StatusCode initialize() final override;
    virtual StatusCode finalize() final override;
  };

  class HistogramToolTH3: public AthHistogramTool, virtual public IAthHistogramTool {
  protected:
    TH3* m_histogram;
    std::string m_histTitle;
    std::string m_histClassName;
    std::vector<std::string> m_cutStages;
    AxisDetails m_axisX;
    AxisDetails m_axisY;
    AxisDetails m_axisZ;

    ExpressionParsing::ExpressionParser *m_parserX;
    ExpressionParsing::ExpressionParser *m_parserY;
    ExpressionParsing::ExpressionParser *m_parserZ;
#ifndef XAOD_ANALYSIS
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
#endif

  public:
    HistogramToolTH3(const std::string& type, const std::string& name, const IInterface* parent);
    virtual StatusCode fill( const double weight = 1.0) override final;
    virtual StatusCode initialize() final override;
    virtual StatusCode finalize() final override;
  };

}

#endif // HISTOGRAMUTILS_HISTOGRAMTOOL_H
