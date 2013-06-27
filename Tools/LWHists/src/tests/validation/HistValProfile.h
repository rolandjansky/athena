/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class HistValProfile                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef HISTVALPROFILE_H
#define HISTVALPROFILE_H

#include "HistValBase.h"
#include "TProfile.h"
#include "LWHists/TProfile_LW.h"

class HistValProfile : public HistValBase {
public:

  HistValProfile( const std::string& name, const std::string& title,
                  int nbins, const double& xmin, const double& xmax,
                  const double& profparmin=0, const double& profparmax=0 );
  template <class TFloat>
  HistValProfile( const std::string& name, const std::string& title,
                  int nbins, const TFloat* xbins );

  virtual ~HistValProfile();

  unsigned getXAxis_NBins() const;
  unsigned getYAxis_NBins() const;

  void setXAxis_BinLabel(unsigned bin, const char* label);
  void setYAxis_BinLabel(unsigned bin, const char* label);

  void setXAxis_LabelSize(float);
  void setYAxis_LabelSize(float);
  void setXAxis_Title(const std::string&);
  void setYAxis_Title(const std::string&);
  void setZAxis_Title(const std::string&);

  void SetMarkerColor( short c);
  void SetMarkerStyle( short s );
  void SetMarkerSize( float sz);

  void setName(const std::string& name);
  void setTitle(const std::string& title);
  void setNameTitle(const std::string& name,
                    const std::string& title);

  void setMinimum(const double& m = -1111);
  void setMaximum(const double& m = -1111);

  //Test various way to fill the histogram:
  void fill(const double& x, const double& y);
  void fill(const double& x, const double& y, const double& w);
  void setBinEntries(unsigned, const double& entries);
  void setBinContent(unsigned, const double& content);
  void setBinError(unsigned, const double& error);
  void setBinInfo(unsigned, const double& entries, const double& content,const double& error);

  void compareBinContents() const;
  void compareMetaData() const;
  void compareTH1s() const;
  void compareFastLoop() const;

  void triggerConversionToROOTHist()
  {
    if (!m_triggeredConversionToROOTHist) {
      m_triggeredConversionToROOTHist=true;
      m_h1->SetName(convertedRootName(m_h1->GetName()).c_str());
      m_h2->getROOTHist();
    }
  }

private:
  TProfile * m_h1;
  TProfile_LW * m_h2;
};

#include "HistValProfile.icc"

#endif
