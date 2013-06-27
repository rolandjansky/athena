/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHist                              //
//                                                            //
//  Description: Base class for the light weight histogram    //
//               classes.                                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHIST_H
#define LWHIST_H

class TH1;
class TAxis;

class LWHist {
public:

  static void safeDelete(LWHist*);//Use this rather than a simple delete.

  //Use methods with names similar to the ones found in TH1F (although
  //not exactly the same signature):
  void SetName(const char*);
  void SetTitle(const char*);
  void SetNameTitle(const char*name,const char*title);
  const char* GetName() const;
  const char* GetTitle() const;

  virtual unsigned GetEntries() const = 0;//returns double in ROOT

  virtual void SetEntries(unsigned) = 0;

  virtual void Reset() = 0;//Clears accumulated contents and statistics

  //We even support some of the most popular decoration-style things,
  //for easier migration from ROOT classes:
  class LWHistAxis;
  LWHistAxis * GetXaxis() const;
  LWHistAxis * GetYaxis() const;
  LWHistAxis * GetZaxis() const;
  short GetMarkerColor() const;
  short GetMarkerStyle() const;
  float GetMarkerSize() const;
  void SetMarkerColor( short c = 1);
  void SetMarkerStyle( short s = 1 );
  void SetMarkerSize( float sz = 1);
  void SetXTitle(const char *);
  void SetYTitle(const char *);
  void SetZTitle(const char *);
  void SetMinimum(const double& minimum = -1111 );
  void SetMaximum(const double& maximum = -1111 );
  void SetOption(const char* option=" ");//Simply passes the options on to the ROOT hist. YMMV.
  virtual double Integral() const = 0;

  //Methods for the monitoring framework:

  //For convenience it is possible to assign custom data to an LWHist
  //[NB: used by the AthenaMonitoring framework]:
  void setCustomData(void*data) { m_customData = data; }
  void * getCustomData() const  { return m_customData; }
  virtual TH1* getROOTHistBase() = 0;
  bool usingROOTBackend() const { return m_usingROOTBackend; }
  bool ownsROOTHisto() const { return m_ownsRootHisto; }
  void setOwnsROOTHisto(bool b) { m_ownsRootHisto = b; }
protected:
  LWHist(const char* name, const char* title, bool rootbackend);
  virtual ~LWHist();
  virtual bool apply(TH1*) const;
  virtual void clearKeptROOTHist() = 0;
  virtual TH1* getROOTHistBaseNoAlloc() const = 0;

  virtual void clear();//releases all held memory, apart from any held ROOT histograms.
  virtual double actualGetBinCenterX(int bin) const = 0;
  virtual double actualGetBinCenterY(int bin) const = 0;
  virtual unsigned actualFindBinX(const double&) const = 0;
  virtual unsigned actualFindBinY(const double&) const = 0;
  virtual unsigned actualGetNBinsX() const = 0;
  virtual unsigned actualGetNBinsY() const = 0;
  unsigned short m_nBytesFromPool;//0 when histogram new'ed normally,
                                  //otherwise set to number of bytes.
private:
  const bool m_usingROOTBackend;
  bool m_ownsRootHisto;
  // It is illegal to copy/assign these histograms:
  LWHist( const LWHist & );
  LWHist & operator= ( const LWHist & );
  char* m_name;
  char* m_title;
  class LWHistDecorations;
  LWHistDecorations * m_decorations;
  void * m_customData;
  void ensureInitDecorations();

};

class LWBinLabels;
class LWHist::LWHistAxis {
public:
  void SetLabelSize(float size=0.04);
  void SetTitle(const char*);
  float GetLabelSize() const { return m_labelSize; }
  const char* GetTitle() const { return m_title; }
  double GetBinCenter(int bin) const;//int rather than unsigned for this one
  const char * GetBinLabel(unsigned bin) const;//0 if not set
  void SetBinLabel(unsigned bin, const char* label);
  unsigned GetNbins() const;
  unsigned FindBin(const double& x);
private:
  LWHistAxis( const LWHistAxis & );
  LWHistAxis & operator= ( const LWHistAxis & );
  friend class LWHist;
  friend class LWPools;
  LWHistAxis(const LWHist *);
  ~LWHistAxis();
  TAxis * rootAxis() const;
  bool isXAxis() const;
  bool isYAxis() const;
  const LWHist * m_hist;
  float m_labelSize;
  char * m_title;
  LWBinLabels * m_binLabels;
};


#endif
