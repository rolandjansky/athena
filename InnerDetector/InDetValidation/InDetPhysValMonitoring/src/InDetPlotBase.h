/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPLOTBASE
#define INDETPHYSVALMONITORING_INDETPLOTBASE
/**
 * @file InDetPlotBase.h
 * @author shaun roe
 * @date 11/9/2015
 **/
#include "TrkValHistUtils/PlotBase.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h" // to make available to children

#include <string>

#define IDPVM_BOOK(name) mBook(name, #name)

class IHistogramDefinitionSvc;
class TEfficiency;


/**
 * @brief Mixin class to give extra capabilities to plots
 * such as ATH_MSG and an easier booking interface, as well
 * as access to the HistogramDefinitionSvc which allows an
 * external configuration file (txt or xml) to define the plot
 * limits and titles.
 **/
class InDetPlotBase: public PlotBase, public AthMessaging {
public:
  /// Constructor taking parent node and directory name for plots
  InDetPlotBase(InDetPlotBase* pParent, const std::string& dirName);

  /// Retrieve a single histogram definition, given the unique string identifier
  SingleHistogramDefinition retrieveDefinition(const std::string& histoIdentifier,
                                               const std::string& folder = "default",
                                               const std::string& nameOverride="");

  /// Helper method to book histograms using an identifier string 
  /// @param pHisto: Pointer to the histogram to be booked (assumed to be initialized to nullptr)
  /// @param histoIdentifier: Identifier (looked up in the XML file) 
  /// @param nameOverride: Allows to override the histo name w.r.t the identifier. Leave empty for no override. This can be used to instantiate multiple, identically binned plots from a single entry in the XML. 
  /// @param folder: Folder to place the plot (if any)
  template <class Htype> void book (Htype* & pHisto, 
                                    const std::string& histoIdentifier, 
                                    const std::string & nameOverride="", 
                                    const std::string& folder = "default");

  /// Book a TH1 histogram
  void book(TH1*& pHisto, const SingleHistogramDefinition& hd);
  /// Book a TProfile histogram
  void book(TProfile*& pHisto, const SingleHistogramDefinition& hd);
  /// Book a TProfile2D histogram
  void book(TProfile2D*& pHisto, const SingleHistogramDefinition& hd);
  /// Book a 2D histogram (TH2)
  void book(TH2*& pHisto, const SingleHistogramDefinition& hd);
  /// Book a (1-D) TEfficiency
  void book(TEfficiency*& pHisto, const SingleHistogramDefinition& hd);

  // weight2 allows us to weight averaging in the profile
  static void fillHisto(TProfile* pTprofile, const float bin, const float weight, const float weight2=1.0);
  static void fillHisto(TProfile2D* pTprofile, const float xval, const float yval, const float weight, const float weight2=1.0);
  //
  static void fillHisto(TH1* pTh1, const float value);
  static void fillHisto(TH1* pTh1, const float value, const float weight);
  //
  static void fillHisto(TH2* pTh2, const float xval, const float yval);
  static void fillHisto(TH2* pTh2, const float xval, const float yval, const float weight);
  //
  static void fillHisto(TH3* pTh3, const float xval, const float yval, const float zval);
  //
  static void fillHisto(TEfficiency* pTeff,  const float value, const bool accepted, float weight);
  static void fillHisto(TEfficiency* eff2d,  const float xvalue, const float yvalue, const bool accepted, const float weight);
  //
protected:
  /// book, for use by macro
  template <class T>
  void
  mBook(T& pHisto, const std::string& histoIdentifier) {
    return book(pHisto, histoIdentifier.substr(2));
  }

private:
  IHistogramDefinitionSvc* m_histoDefSvc;
};


#include "InDetPlotBase.ixx"


#endif
