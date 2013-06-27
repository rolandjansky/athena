/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class HistValBase                         //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef HISTVALBASE_H
#define HISTVALBASE_H

#include "validatefunctions.h"//Not the nicest way to do it (but its just for the tests)
#include "LWHistVal.h"
#include <string>
#include <cassert>
#include "LWHists/LWHistControls.h"

static bool HistVal_trigger_conversion_all = false;

class HistValBase {
public:

  HistValBase();
  virtual ~HistValBase();

  virtual unsigned getXAxis_NBins() const = 0;
  virtual unsigned getYAxis_NBins() const = 0;

  virtual void setXAxis_BinLabel(unsigned bin, const char* label) = 0;
  virtual void setYAxis_BinLabel(unsigned bin, const char* label) = 0;

  virtual void setXAxis_LabelSize(float) = 0;
  virtual void setYAxis_LabelSize(float) = 0;
  virtual void setXAxis_Title(const std::string&) = 0;
  virtual void setYAxis_Title(const std::string&) = 0;

  virtual void SetMarkerColor( short c) = 0;
  virtual void SetMarkerStyle( short s ) = 0;
  virtual void SetMarkerSize( float sz) = 0;

  virtual void setName(const std::string& name) = 0;
  virtual void setTitle(const std::string& title) = 0;
  virtual void setNameTitle(const std::string& name,
                            const std::string& title) = 0;

  virtual void setMinimum(const double& m = -1111) = 0;
  virtual void setMaximum(const double& m = -1111) = 0;

  void setCompareBinContentsOnEachFill(bool);
  bool compareBinContentsOnEachFill() const
  { return m_compareBinContentsOnEachFill; }

  virtual void compareBinContents() const = 0;//bin contents (incl. over/underflow)
  virtual void compareMetaData() const = 0;
  void compareAllContents() const;//both of the above
  virtual void compareTH1s() const = 0;//compare the TH1 and the TH1
                                       //created from the light weight instance
                                       //Extend in derived.

  virtual void compareFastLoop() const = 0;
  void compareAll() const;//compareAllContents(), compareTH1Fs() and compareFastLoop()

  virtual void triggerConversionToROOTHist() = 0;

protected:
  //Call in Fill(..), SetBinXXX(..) methods:
  void binContentsChanged();
  //Call in SetName/SetTitle/... methods:
  void titleOrNameWasChanged();

  static const char* profile_test_erroroption() { return " "; }//" ", "s", "i", "g" (fails for "i" due to numerical problems in ROOT, "g" fails for 2D due to a bug in .22 version of ROOT)

private:

  // Illegal to copy/assign a HistValBase:
  HistValBase( const HistValBase & );
  HistValBase & operator= ( const HistValBase & );

  bool m_compareBinContentsOnEachFill;
protected:
  bool m_triggeredConversionToROOTHist;
  bool converted() const { return m_triggeredConversionToROOTHist; }
  std::string convertedRootName(const std::string& n)
  {
    static std::string prefix="::trigger::";
    //To avoid name clashes
    if (m_triggeredConversionToROOTHist)
      return prefix+n;
    return n;
  }

};

#endif
