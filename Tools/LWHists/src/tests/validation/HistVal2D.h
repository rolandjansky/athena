/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class HistVal2D                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef HISTVAL2D_H
#define HISTVAL2D_H

#include "HistValBase.h"

template <class TH_2D, class THLW_2D>
class HistVal2D : public HistValBase {
public:
  typedef typename THLW_2D::bin_type_t bin_type_t;

  HistVal2D( const std::string& name, const std::string& title,
             int nbinsx, const double& xmin, const double& xmax,
             int nbinsy, const double& ymin, const double& ymax );

  template <class TFloat>
  HistVal2D( const std::string& name, const std::string& title,
             int nbinsx, const TFloat* xbins,
             int nbinsy, const TFloat* ybins );

  template <class TFloat>
  HistVal2D( const std::string& name, const std::string& title,
             int nbinsx, const TFloat* xbins,
             int nbinsy, const double& ymin, const double& ymax );

  template <class TFloat>
  HistVal2D( const std::string& name, const std::string& title,
             int nbinsx, const double& xmin, const double& xmax,
             int nbinsy, const TFloat* ybins );

  virtual ~HistVal2D();

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
  void setMinimum( const double& m = -1111 );
  void setMaximum( const double& m = -1111 );

  void setName(const std::string& name);
  void setTitle(const std::string& title);
  void setNameTitle(const std::string& name,
                    const std::string& title);

  //Test various way to fill the histogram:
  void fill(const double& x, const double& y);
  void fill(const double& x, const double& y, const double& w);
  void setBinContent(unsigned, unsigned, const double& content);
  void setBinError(unsigned, unsigned, const double& error);
  void setBinContentAndError(unsigned, unsigned, const double& content,const double& error);

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
  TH_2D * m_h1;
  THLW_2D * m_h2;
};

#include "HistVal2D.icc"

#endif
