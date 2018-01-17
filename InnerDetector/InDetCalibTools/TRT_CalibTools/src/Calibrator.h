/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

   NAME: Calibrator.h   
PACKAGE: TRTCalibTools 

AUTHORS: Johan Lundquist  
CREATED: 27-03-2009 

PURPOSE: Class for calibrating a TRT sub-level
          
********************************************************************/

#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <TNtuple.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TText.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <map>
#include <set>
using namespace std;

/**
A class for generating a r-t and t-r graphs by binning the 2D histograms in Calibrator::rtHists in r ot t bins. 
*/
class RtGraph{
public:
  /** the constructor*/ RtGraph(TH2F*, int, const char*, bool, TDirectory*);
  /** the destructor */ ~RtGraph();
  /** array of the histograms for all bins*/ TH1D** hslizes;
  /** the r values*/ vector<double> rval;
  /** the r values*/ vector<double> tval;
  /** the t(r) graph*/ TGraphErrors* trgr;
  /** the r(t) graph*/ TGraphErrors* rtgr;
  /** the number of graph points */ int npoints;
  /** the minimum t-value*/ float mintime;
private:
  /** the histogram name*/ string m_chname;
  /** the histogram title*/ string m_chtit;
  /***/ enum  bintype {LOW, GOOD, HIGH, EMPTY};
  /***/ bintype* m_btype;
  /***/ float *m_rightsig;
  /***/ float *m_leftsig;
  /***/ float *m_maxval;
  /***/ float *m_leftval;
  /***/ float *m_rightval;
  /***/ float m_mean;  
  /***/ float m_mindistance;
  /***/ int *m_maxbin;
  /***/ int m_ipoint;
  /***/ float m_t;
  /***/ float m_d;
  /***/ float m_et;
  /***/ float m_ed;
  /***/ float *m_tv;
  /***/ float *m_dv;
  /***/ float *m_etv;
  /***/ float *m_edv;
//  /***/ float width;
  /***/ TF1* m_ff;
};

/**
A structure to contain hit data
*/
class databundle{
 public:
  /**detector (barrel -1 or 1, or end-cap -2 or 2)*/ int det;
  /**layer*/ int lay;
  /**phi module*/ int mod;
  /**board*/ int brd;
  /**chip*/ int chp;
  /**straw-layer*/ int stl;
  /**straw number (within the strawlayer)*/ int stw;
  /**straw ID*/ int sid;
  /**event number*/ int ievt;
  /**time residual*/ float tres;
  /**histogram weight (currently set to 1)*/ float weight;
  /**space residual*/ float res;
  /**raw time*/ float t;
  /**drift radius from r(t) relation*/ float r;
  /**t0 value used in reconstruction*/ float t0;
  /**reference t0 (offset from board mean)*/ float rt0;
  /**rt-parameters used in the reconstruction*/ float rtpar[4];
  /**straw x position*/ float x;
  /**straw y position*/ float y;
  /**straw z position*/ float z;
};

/**
A structure to contain data associated with the calibration of a certain sub-module
*/
class caldata{
 public:
  caldata();
  caldata(bool,int,int);
  ~caldata();
  /**detector (barrel or end-cap)*/ int det;
  /**layer*/ int lay;
  /**phi module*/ int mod;
  /**board*/ int brd;
  /**chip*/ int chp;
  /**straw-layer*/ int stl;
  /**straw number (within the strawlayer)*/ int stw;
  /**straw ID*/ int sid;
  /**number of time residual histogram entries*/ int ntres;
  /**number of rt histogram entries*/ int nrt;
  /**number of residual histogram entries */ int nres;
  /**the type of time residual fit that was made*/ int t0fittype;
  /**the residual*/ float res;
  /**the residual mean*/ float resMean;
  /**the residual error*/ float reserr;
  /**the time residual*/ float tres;
  /**the time residual mean*/ float tresMean;
  /**the new t0*/ float t0;
  /**the new to error*/ float t0err;
  /**the reference t0 (offset from board mean)*/ float reft0;
  /**the t0 offset from the level below*/ float t0off;
  /**the t0 fron the R-t fit*/ float rtt0;
  /**the rt-parameters used in the track reconstruction*/ float oldrtpar[4];
  /**the new rt-parameters*/ float rtpar[4];
  /**the number of straws in the sub-module*/ float nhits;
  /**sub-module x position (average of all straws in the module)*/ float x;
  /**sub-module y position (average of all straws in the module)*/ float y;
  /**sub-module z position (average of all straws in the module)*/ float z;
  /**the old sub-module t0 (average of t0 for all straws in the module)*/ double oldt02;
  /**...*/ double sumt0;
  /**...*/ double sumx;
  /**...*/ double sumy;
  /**...*/ double sumz;
  /**flag indicating if any calibration has been made*/ bool calflag;
  /**flag indicating if an R-t calibration has been made*/ bool rtflag;
  /**flag indicating if a t0 calibration has been made*/ bool t0flag;
  /**the 1D time residual histogram (100 bins)*/ float* m_treshist;
  /**the 1D residual histogram (100 bins)*/ float* reshist;
  /**the 2D rt histogram (20x32 bins)*/ float* rthist;
  /**the rt graph*/ RtGraph* rtgraph;
};

/**
A structure to contain data associated with the calibration of a certain sub-module
*/
//class caldata : public caldata{
// public:
//  /**the 1D time residual histogram (100 bins)*/ float treshist[100];
//  /**the 1D residual histogram (100 bins)*/ float reshist[100];
//  /**the 2D rt histogram (20x32 bins)*/ float rthist[640];
//};

/**
An instance of this class is created for each sub-level of the %TRT by the TRTCalibrator. It contains three histograms for each
sub-module in the level:

<ul>
<li> a 2D R-t histogram (caldata::rthist)
<li> a 1D time residual histogram (caldata::treshist)
<li> a 1D residual histogram (caldata::reshist)
</ul>

The histograms are inside a caldata structure which also contains other variables with information about the
calibration of a sub-module. There is one caldata instance for each sub-module in the level. They are contained in
a map (Calibrator::data) with a key string which identifies the sub-module.

Methods exists for adding hits to the histograms (Calibrator::AddHits) and for fitting them (Calibrator::Calibrate).

*/
class Calibrator{
public:
  Calibrator();

  /**
     Constructor
     @param[in] lev sub level number 0=%TRT 1=barrel/ec 2=layer 3=phi-module 4=board 5=chip 6=straw
     @param[in] nme the name of the calibrator
     @param[in] mint0 minimum number of hits in a sub-module required to do a t0 calibration
     @param[in] minrt minimum number of hits in a sub-module required to do an rt calibration
     @param[in] rtr which rt-relation to use
     @param[in] rtb which rt binning to use
  */
  Calibrator(int,string,int,int,string,string,float);

  /**
     Destructor
  */
   ~Calibrator();

   /**
     A 1D histograming function
     @param[in] min histogram lower limit
     @param[in] max histogram upper limit
     @param[in] nbins nymber of bins
     @param[in] value the value
     @return the histogram bin
   */
  int Simple1dHist(float, float, int, float);

  /**
     A 2D histograming function
     @param[in] minx histogram lower x-limit
     @param[in] maxx histogram upper x-limit
     @param[in] nbinsx nymber of x bins
     @param[in] valuex the x value
     @param[in] miny histogram lower y-limit
     @param[in] maxy histogram upper y-limit
     @param[in] nbinsy nymber of y bins
     @param[in] valuey the y value
     @return the histogram bin in the form (biny*nbinsx+binx)
  */
  int Simple2dHist(float, float, int, float, float, int, float, float);

  /**
     Increments a single bin in all three histograms in a sub-module
     @param[in] key the identifier of the histogram
     @param[in] d the databundle containing single straw hit data
     @return 1
  */
  float AccumulativeMean(float, float, float);

  /**
     Adds hits to a sub-module either in the form of a single straw hit or a histogram containing all the hits in a single straw
     @param[in] key the identifier of the sub-module
     @param[in] d the databundle containing single straw hit data
     @param[in] binhist an integer arry containing the histogram data for a single straw
     @return 1 if the sub-module is seen for the first time and 0 if it has been seen before
  */
  int AddHit(string,databundle,int*,bool);

  /**
     Creates root histograms, performs the t0 and R-t calibration for a given sub-module and writes the resulting histograms to a root directory tree (if not told otherwise). The new t0 values are calculated here.
     @param[in] dir the root directory of the sub level above
     @param[in] key the identifier of the sub-module to calibrate
     @param[in] caldata_above the caldata object from the sub-module above the one to be calibrated
     @return the root directory where the histgrams were written
  */
  TDirectory* Calibrate(TDirectory*, string, string, caldata*);

  /**
     Makes the R-t fit
     @param[in] key the identifier of the sub-module to calibrate
     @param[in] rtHist the 2D root histogram with the R-t data
     @return the t0 from the R-t fit
  */
  float FitRt(string,string,TH2F*,TDirectory*);

  /**
     Makes the time residual fit
     @param[in] key the identifier of the sub-module to calibrate
     @param[in] tresHist the 1D root histogram with the time residuals
     @return the mean value of the time residual fit
  */
  float FitTimeResidual(string,TH1F*);

  /**
     Makes the residual fit
     @param[in] key the identifier of the sub-module to calibrate
     @param[in] resHist the 1D root histogram with the residuals
     @return the mean value of the residual fit
  */
  float FitResidual(string,TH1F*);

  /**
     Creates an ntuple with entries containing data associated with the sub-modules in a sub level
     @param[in] dir the root directory where to write the ntuple
  */
  void WriteStat(TDirectory*);

  void DumpConstants();

  /**
  */
  string GetBinnedRt(string);

  /**
     Prints some sub-level statistics
  */
  string PrintStat();

  string PrintInfo();

  /**
     ...
  */
  bool HasKey(string);

  /**
     ...
  */
  int UpdateOldConstants();

  /**
     Checks if a given sub-module is selected for calibration
     @param[in] level the number of the sub-module to check
     @return returns true if the module is found
  */
  bool CheckSelection(int);

  /**
     ...
  */
  bool Skip();

  /**
     Creates a string summarizing what is being done at this sub-level
     @return the string
  */
  string GetOptString();

  /**
     ...
  */
  int GetNDirs();

  /**
     Creates a string summarizing which modules are being calibrated at this sub-level
     @return the string
  */
  string GetSelString();

  float oldt0(string key) {return data[key].sumt0/float(data[key].ntres);}
  float xmean(string key) {return data[key].sumx/float(data.size());}
  float ymean(string key) {return data[key].sumy/float(data.size());}
  float zmean(string key) {return data[key].sumz/float(data.size());}

  /**A map between the sub-module identifier string and the calibration data structure (caldata)*/
  std::map<std::string,caldata> data;

  /**if true an rt fit is made, if false the value from the level above is used*/  bool dort;
  /**if true a time residual fit is made, if false the value from the level above is used*/  bool dot0;
  /**if true a residual fit is made*/  bool dores;
  /**if true the old rt parameters are copied to the new ones*/  bool nort;
  /**if true the old t0 valus is copied to the new one*/  bool not0;
  /**if true chip reference t0 values (offset from board mean) are used*/  bool usebref;
  /**if true no histograms are written to the root file*/  bool bequiet;
  /**if true a log entry is prined for each sub-modile in this sub-level*/  bool printlog;
  /**if true a t0 entry in the calibration output file is prined for each sub-module in this sub-level*/  bool printt0;
  /**if true an rt entry in the calibration output file is prined for each sub-module in this sub-level*/  bool printrt;
  /**if true the 0th order coeficcient of the rt fit function is not set to 0 in the calibration output file*/  bool usep0;
  /**if true the 3rd order coeficcient of the rt fit function is not fixed to 0*/  bool floatp3;
  /**if true a shift of -0.75 ns is applied for straws in layer ==0, strawlayer < 9, and when doing calibration in a granuralyty different from chip or straw*/  bool useshortstraws;
  /**a set containing the sub-modules to be calibrated*/ set<int> selection;
  /**the sub-level of the Calibrator instance*/ int level;

private:

  /**The name of the Calibrator instance*/ string m_name;
  /**The direction to do the R-t binning*/ string m_rtbinning;
  /**minimum number of hits in a sub-module required to do an R-t calibration*/ int m_minrtstat;
  /**minimum number of hits in a sub-module required to do a t0 calibration*/ int m_mint0stat;
  /**the t0 shift*/ float m_t0shift;
  /**number of r-bins in the 2D rt histogram*/ int m_nbinsr;
  /**number of t-bins in the 2D rt histogram*/ int m_nbinst;
  /**number of bins in the 1D time residual histogram*/ int m_nbinstres;
  /**number of bins in the 1D residual histogram*/ int m_nbinsres;
  /**lower limit of r in 2D rt histogram*/ float m_minr;
  /**upper limit of r in 2D rt histogram*/ float m_maxr;
  /**lower limit of t in 2D rt histogram*/ float m_mint;
  /**upper limit of t in 2D rt histogram*/ float m_maxt;
  /**lower limit of 1D time residual histogram*/ float m_mintres;
  /**upper limit of 1D time residual histogram*/ float m_maxtres;
  /**lower limit of 1D residual histogram*/ float m_minres;
  /**upper limit of 1D residual histogram*/ float m_maxres;
  /**...*/ std::map<std::string,TDirectory*> m_hdirs;
  /**...*/ std::map<std::string,TH1F*> m_resHists;
  /**...*/ std::map<std::string,TH1F*> m_tresHists;
  /**...*/ std::map<std::string,TH2F*> m_rtHists;
  //  /**...*/ std::map<std::string,RtGraph*> m_rtgraphs;
  /**...*/ int m_ntreshits;
  /**...*/ int m_nreshits;
  /**...*/ int m_nrthits;
  /**...*/ int m_nhits;



  /**true if the rt relation is Dines'*/ bool m_isdines;

};


