/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "HistValBase.h"

#include "validatefunctions.h"
#include "HistVal1D.h"
#include "HistVal2D.h"
#include "HistValProfile.h"
#include "HistValProfile2D.h"

#include "LWHists/LWHistControls.h"

#include "TH1F.h"
#include "LWHists/TH1F_LW.h"
#include "TH1D.h"
#include "LWHists/TH1D_LW.h"
#include "TH1I.h"
#include "LWHists/TH1I_LW.h"
#include "TH2F.h"
#include "LWHists/TH2F_LW.h"
#include "TH2D.h"
#include "LWHists/TH2D_LW.h"
#include "TH2I.h"
#include "LWHists/TH2I_LW.h"
#include "TProfile.h"
#include "LWHists/TProfile_LW.h"
#include "TProfile2D.h"
#include "LWHists/TProfile2D_LW.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <climits>
#include <limits>
#include <type_traits>

#define LW_OLD_WEIGHTED_PROFILE_ERRORS 1

std::vector<double> getXValues()//by value... because it doesn't matter
{
  std::vector<double> x;
  x.push_back(5.4);//inside
  x.push_back(0.0);//= hist edge
  x.push_back(100.0);//= hist edge
  x.push_back(-20.0);//underflow
  x.push_back(10.0);//inside
  x.push_back(1123.0);//overflow
  return x;
}

std::vector<double> getWeights(bool is_int)
{
  std::vector<double> w;
  w.push_back(1.0);//unit
  w.push_back(0.0);//zero
  w.push_back(8.0);//positive integer
  w.push_back(-1.0);//negative unit
  w.push_back(-7.0);//negative integer
  w.push_back(4.655);//positive
  w.push_back(-4.252);//negative
  w.push_back(2.655);//positive
  w.push_back(4.251);//positive (different trunc vs. round from above)
  w.push_back(-4.257);//negativ (different trunc vs. round from above)
  w.push_back(2.251);//positive (different trunc vs. round from above)
  if (!is_int) {
    w.push_back(5.0e9);//Out of int range
    w.push_back(-8.0e9);//Out of int range
  }
  return w;
}

template <class HistVal>
void setNamesAndTitles(HistVal&hist) {
  hist.setName("name1");
  hist.setName("name2");
  hist.setName("title1");//not typo
  hist.setTitle("");
  hist.setTitle("title2");
  hist.setName("name3");
  hist.setNameTitle("someothername","someothertitle");

  hist.setXAxis_LabelSize(0.04);
  hist.setXAxis_LabelSize(2.0);
  hist.setYAxis_LabelSize(0.04);
  hist.setYAxis_LabelSize(0.01);
  hist.setXAxis_Title("");
  hist.setXAxis_Title("some-x-title");
  hist.setYAxis_Title("some-y-title");
  hist.setZAxis_Title("some-z-title");

  unsigned nx(hist.getXAxis_NBins());
  hist.setXAxis_BinLabel(1,"hejsa");
  hist.setXAxis_BinLabel(17%nx+1,"hejsa");
  hist.setXAxis_BinLabel(19%nx+1,"hejsa");
  hist.setXAxis_BinLabel(74%nx+1,"hejsa");
  hist.setXAxis_BinLabel(nx,"hejsa");
  hist.setXAxis_BinLabel(19%nx+1,"hejsa");
  hist.setXAxis_BinLabel(1%nx+1,"hejsa");
  hist.setXAxis_BinLabel(1%nx+1,"hejsa");
  hist.setXAxis_BinLabel(100%nx+1,"hejsa");
  unsigned ny(hist.getYAxis_NBins());
  hist.setYAxis_BinLabel(2%ny+1,"hejsa");
  hist.setYAxis_BinLabel(1%ny+1,"hejsa");
  hist.setYAxis_BinLabel(ny,"hejsa");

  hist.compareAll();
  hist.SetMarkerStyle( 3 );
  hist.compareAll();
  hist.SetMarkerColor( 4 );
  hist.compareAll();
  hist.SetMarkerSize( 1.02 );
  hist.compareAll();
  static unsigned i(0);
  switch (i++%5) {
  case 0: hist.setMinimum(12312.0); break;
  case 1: hist.setMinimum(); break;
  case 2: hist.setMinimum(0); break;
  case 3: hist.setMinimum(-123.0); break;
  case 4: hist.setMinimum(-123.0); hist.setMinimum(); break;
  default: assert(false); break;
  };
  hist.compareAll();
  static unsigned i2(0);
  switch (i2++%5) {
  case 0: hist.setMaximum(12312.0); break;
  case 1: hist.setMaximum(); break;
  case 2: hist.setMaximum(0); break;
  case 3: hist.setMaximum(-123.0); break;
  case 4: hist.setMaximum(-123.0); hist.setMaximum(); break;
  default: assert(false); break;
  };
}

template <class T1, class T2>
void fillInVariousWays(int nbinsx, double xmin, double xmax,
                       int nbinsy, double ymin, double ymax)
{
  //Either Fill(x) or Fill(x,w):
  HistVal2D<T1,T2> h1("name1a","title1",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  fillInVariousWays_int2d(h1,true,false);
  HistVal2D<T1,T2> h2("name1b","title1",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  fillInVariousWays_int2d(h2,false,true);

  //First Fill(x), then Fill(x,w):
  HistVal2D<T1,T2> h3("name1c","title1",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  fillInVariousWays_int2d(h3,true,false);
  fillInVariousWays_int2d(h3,false,true);

  //First Fill(x,w), then Fill(x):
  HistVal2D<T1,T2> h4("name1d","title1",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  fillInVariousWays_int2d(h4,false,true);
  fillInVariousWays_int2d(h4,true,false);

  //Fill slowly to pass through char, short and full stages:
  HistVal2D<T1,T2> h5("name1e","title1",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  for (unsigned i=0; i<UCHAR_MAX+10;++i)
    h5.fill(17,18);
  h5.setCompareBinContentsOnEachFill(false);//Disable since we will have maaany calls.
  for (unsigned i=0; i<USHRT_MAX;++i)
    h5.fill(17,18);
  h5.compareAll();

}

template <class T1, class T2>
void fillInVariousWays(int nbins, double xmin, double xmax)
{
  //Either Fill(x) or Fill(x,w):
  HistVal1D<T1,T2> h1("name1a","title1",nbins,xmin,xmax);
  fillInVariousWays_int(h1,true,false);
  HistVal1D<T1,T2> h2("name1b","title1",nbins,xmin,xmax);
  fillInVariousWays_int(h2,false,true);

  //First Fill(x), then Fill(x,w):
  HistVal1D<T1,T2> h3("name1c","title1",nbins,xmin,xmax);
  fillInVariousWays_int(h3,true,false);
  fillInVariousWays_int(h3,false,true);

  //First Fill(x,w), then Fill(x):
  HistVal1D<T1,T2> h4("name1d","title1",nbins,xmin,xmax);
  fillInVariousWays_int(h4,false,true);
  fillInVariousWays_int(h4,true,false);

  //Fill slowly to pass through char, short and full stages:
  HistVal1D<T1,T2> h5("name1e","title1",nbins,xmin,xmax);
  for (unsigned i=0; i<UCHAR_MAX+10;++i)
    h5.fill(17);
  h5.setCompareBinContentsOnEachFill(false);//Disable since we will have maaany calls.
  for (unsigned i=0; i<USHRT_MAX;++i)
    h5.fill(17);
  h5.compareAll();

  //Fill - with weights - in a way which allows us pass through char,
  //short and full stages:

  //Char->Short->Full:
  HistVal1D<T1,T2> h6("name1f","title1",nbins,xmin,xmax);
  h6.fill(17,1.0);
  h6.fill(17,UCHAR_MAX+1.0);
  h6.fill(17,USHRT_MAX+1.0);
  //Char->Full:
  HistVal1D<T1,T2> h7("name1g","title1",nbins,xmin,xmax);
  h6.fill(17,1.0);
  h6.fill(17,UCHAR_MAX+USHRT_MAX+1.0);
}

template <class HistVal>
void fillInVariousWays_int(HistVal&h,bool doUnitWeight, bool doFloatWeight)
{
  std::vector<double> w(getWeights(std::is_integral<typename HistVal::bin_type_t>::value));
  std::vector<double> x(getXValues());

  //Fill(x):
  if (doUnitWeight)
    for (unsigned ix=0; ix<x.size();++ix)
      h.fill(x.at(ix));

  h.compareAll();
  //Fill(x,w):
  if (doFloatWeight)
    for (unsigned ix=0; ix<x.size();++ix)
      for (unsigned iw=0; iw<w.size();++iw)
        h.fill(x.at(ix),w.at(iw));

  h.compareAll();
}

template <class HistVal>
void fillInVariousWays_int2d(HistVal&h,bool doUnitWeight, bool doFloatWeight)
{
  std::vector<double> w(getWeights(std::is_integral<typename HistVal::bin_type_t>::value));
  std::vector<double> x(getXValues());

  //Fill(x):
  if (doUnitWeight) {
    h.setCompareBinContentsOnEachFill(false);
    for (unsigned ix=0; ix<x.size();++ix) {
      for (unsigned iy=0; iy<x.size();++iy)
        h.fill(x.at(ix),x.at(iy));
    }
    h.setCompareBinContentsOnEachFill(true);
  }
  h.compareAll();
  //Fill(x,w):
  if (doFloatWeight) {
    h.setCompareBinContentsOnEachFill(false);
    for (unsigned ix=0; ix<x.size();++ix) {
      h.setCompareBinContentsOnEachFill(false);
      for (unsigned iy=0; iy<x.size();++iy)
        for (unsigned iw=0; iw<w.size();++iw)
          h.fill(x.at(ix),x.at(iy),w.at(iw));
    }
    h.setCompareBinContentsOnEachFill(true);
  }
  h.compareAll();
}

template <class T1, class T2>
void basicValidation(bool do_systematic_tests =  true) {
  //Test name/title:
  HistVal1D<T1,T2> h("name1","title1",150,-10.0,80.0);
  setNamesAndTitles(h);
  HistVal1D<T1,T2> h2("name1","",150,0.0,17.0);
  setNamesAndTitles(h2);
  HistVal1D<T1,T2> h3("","title3",150,0.0,17.0);
  setNamesAndTitles(h3);
  HistVal1D<T1,T2> h4("","",150,0.0,17.0);
  setNamesAndTitles(h4);
  //Special basic test:
  HistVal1D<T1,T2> h5("name1test","title1test",2,0.0,100.0);
  h5.fill(80);
  h5.compareAll();
  //  std::cout<<"basic validation of h1="<<typeid(T1).name()<<" vs. h2="<<typeid(T2).name()<<std::endl;
  h5.fill(30,4.545);
  h5.compareAll();
  h5.fill(-1,4.545);
  h5.compareAll();
  h5.fill(101,4.545);
  h5.compareAll();

  //Another special test for savanna bug #73561
  HistVal1D<T1,T2> h_errbug("nameerrtest","title1test",2,0.0,100.0);
  h_errbug.setBinContentAndError(1,10.0,10.0);//err is now 10
  h_errbug.setBinContentAndError(1,4.0,2.0);//err is now sqrt(content)
  h_errbug.compareAll();

  std::cout << " ---> Quick basic test completed"<<std::endl;

  //Very simple test of our scaleContentsAndErrors:
  unsigned nbinsscaled(100);
  T2 * hscale = T2::create("namescale","titlescale",nbinsscaled,0.0,100.0);
  hscale->Fill(0.5);//ibin = 1
  hscale->Fill(50.5,17.0);//ibin = 51
  double scalefact=15.0;//We choose integer values 17.0 and 15.0 here to let the test pass for TH1I also.
  std::vector<double> unscaled_cont(nbinsscaled+2),unscaled_err(nbinsscaled+2);
  for (unsigned ibin=0;ibin<nbinsscaled;++ibin)
    hscale->GetBinContentAndError(ibin,unscaled_cont.at(ibin),unscaled_err.at(ibin));
  hscale->scaleContentsAndErrors(scalefact);
  for (unsigned ibin=0;ibin<nbinsscaled;++ibin) {
    double c,e;
    hscale->GetBinContentAndError(ibin,c,e);
    HistValFunctions::test("Scaled bin content",c,unscaled_cont.at(ibin)*scalefact);
    HistValFunctions::test("Scaled bin error",e,unscaled_err.at(ibin)*scalefact);
  }
  LWHist::safeDelete(hscale);

  std::cout << " ---> Basic scaling test completed"<<std::endl;

  if (do_systematic_tests) {
    //Systematic validation of special cases:
    std::vector<unsigned> sys_ibin;
    std::vector<double> sys_x;
    const unsigned nbins=40;
    const double xmin(0.0);
    const double xmax(40.0);
    sys_ibin.push_back(0);sys_x.push_back(xmin-10.0);//underflow bin
    sys_ibin.push_back(1);sys_x.push_back(0.5);//internal bin 1
    sys_ibin.push_back(6);sys_x.push_back(5.5);//internal bin 2 (<16 bins from bin1)
    sys_ibin.push_back(36);sys_x.push_back(35.5);//internal bin 3 (>16 bins from bin1 and bin2)
#ifdef LW_STRICT_ROOT_BEHAVIOUR
    sys_ibin.push_back(nbins+1);sys_x.push_back(xmax+10.0);//overflow bin
    sys_ibin.push_back(nbins+USHRT_MAX+100);sys_x.push_back(std::numeric_limits<float>::quiet_NaN());//bin out of range and x NaN
#endif
    unsigned npoints=sys_ibin.size();
    assert(npoints==sys_x.size());
    std::vector<double> sys_args;
    sys_args.push_back(0.0);
    //FIXME: sys_args.push_back(std::numeric_limits<float>::quiet_NaN());
    sys_args.push_back(-4.0);
    sys_args.push_back(-2.0);
    sys_args.push_back(2.0);
    sys_args.push_back(4.0);
    sys_args.push_back(20.0);

    const bool expecterror(false);
#define SYSTEST(x) { if (HistValFunctions::verbose()) { std::cout<<"Systematic filling: "<<x<<std::endl;} }
    double x,a;unsigned i, itest(0);
    for (unsigned ipoint1 = 0;ipoint1<npoints;++ipoint1)
    for (unsigned ipoint2 = 0;ipoint2<npoints;++ipoint2)
    for (unsigned iarg1 = 0;iarg1<sys_args.size();++iarg1)
    for (unsigned iarg2 = 0;iarg2<sys_args.size();++iarg2)
    for (unsigned doFillX_1 = 0;doFillX_1<=1;++doFillX_1)
    for (unsigned doFillXW_1 = 0;doFillXW_1<=1;++doFillXW_1)
    for (unsigned doSetBinContent_1 = 0;doSetBinContent_1<=1;++doSetBinContent_1)
    for (unsigned doSetBinError_1 = 0;doSetBinError_1<=1;++doSetBinError_1)
    for (unsigned doFillX_2 = 0;doFillX_2<=1;++doFillX_2)
    for (unsigned doFillXW_2 = 0;doFillXW_2<=1;++doFillXW_2)
    for (unsigned doSetBinContent_2 = 0;doSetBinContent_2<=1;++doSetBinContent_2)
    for (unsigned doSetBinError_2 = 0;doSetBinError_2<=1;++doSetBinError_2)
    for (unsigned doSetBinContentAndError_2 = 0;doSetBinContentAndError_2<=1;++doSetBinContentAndError_2)
      {
        ++itest;
        SYSTEST("------------> 1D Test number "<<itest);
        HistVal1D<T1,T2> h("systname","systtitle",nbins,xmin,xmax);
        h.setCompareBinContentsOnEachFill(expecterror);

        i=sys_ibin.at(ipoint1);x=sys_x.at(ipoint1); a=sys_args.at(iarg1);

        if (doFillX_1) { SYSTEST("Fill("<<x<<")"); h.fill(x); }
        if (doFillXW_1) { SYSTEST("Fill("<<x<<", "<<a<<")"); h.fill(x,a); }
        if (doSetBinContent_1) { SYSTEST("setBinContent("<<i<<", "<<a<<")"); h.setBinContent(i,a); }
#ifdef LW_STRICT_ROOT_BEHAVIOUR
        //only compatible when SetBinError is last when not in strict mode
        if (doSetBinError_1) { SYSTEST("setBinError("<<i<<", "<<a<<")"); h.setBinError(i,a); }
#endif

        i=sys_ibin.at(ipoint2);x=sys_x.at(ipoint2); a=sys_args.at(iarg2);
        if (doFillX_2) { SYSTEST("Fill("<<x<<")"); h.fill(x); }
        if (doFillXW_2) { SYSTEST("Fill("<<x<<", "<<a<<")"); h.fill(x,a); }
        if (doSetBinContent_2) { SYSTEST("setBinContent("<<i<<", "<<a<<")"); h.setBinContent(i,a); }
#ifndef LW_STRICT_ROOT_BEHAVIOUR
        //only compatible when SetBinError is last when not in strict mode
        if (doSetBinError_1) { SYSTEST("setBinError("<<i<<", "<<a<<")"); h.setBinError(i,a); }
#endif
        if (doSetBinError_2) { SYSTEST("setBinError("<<i<<", "<<a<<")"); h.setBinError(i,a); }
        if (doSetBinContentAndError_2)
          { SYSTEST("setBinContentAndError("<<i<<", "<<a<<", 17.0)"); h.setBinContentAndError(i,a,17.0); }
      }
    std::cout << " ---> Systematic tests ("<<itest<<") completed"<<std::endl;
  }

  float * xbins = new float[5];
  xbins[0] = 0.0;
  xbins[1] = 1.0;
  xbins[2] = 2.7;
  xbins[3] = 2.701;
  xbins[4] = 10.0;
  HistVal1D<T1,T2> h6("name6","title6",4,xbins);
  delete[] xbins;
  h6.fill(2.6);h6.compareAll();
  h6.fill(2.7);h6.compareAll();
  h6.fill(0.0);h6.compareAll();
  h6.fill(10.0,1.2);h6.compareAll();
  h6.fill(-20.0);h6.compareAll();
  h6.fill(12.0);h6.compareAll();
  h6.fill(5.0);h6.compareAll();
  h6.fill(1.2);h6.compareAll();
  std::cout << " ---> Variable binning tests completed"<<std::endl;

  fillInVariousWays_int(h,true,true);
  fillInVariousWays<T1,T2>(100,0.0,100.0);
  fillInVariousWays<T1,T2>(150,0.0,100.0);
  fillInVariousWays<T1,T2>(15000,0.0,100.0);//not small()!
  std::cout << " ---> Various filling tests completed"<<std::endl;

}

template <class T1, class T2>
void basicValidation2D() {
  //Test name/title:
  HistVal2D<T1,T2> h("name1","title1",100,-10.0,80.0,50,10.0,1230.0);
  setNamesAndTitles(h);
  HistVal2D<T1,T2> h2("name1","",100,0.0,17.0,50,10.0,1230.0);
  setNamesAndTitles(h2);
  HistVal2D<T1,T2> h3("","title3",100,0.0,17.0,50,10.0,1230.0);
  setNamesAndTitles(h3);
  HistVal2D<T1,T2> h4("","",100,0.0,17.0,50,10.0,1230.0);
  setNamesAndTitles(h4);
  std::cout << " ---> Quick basic test completed"<<std::endl;
  fillInVariousWays_int2d(h,true,true);
  fillInVariousWays<T1,T2>(100,0.0,100.0,100,0.0,100.0);
  std::cout << " ---> Various filling tests completed"<<std::endl;

  const bool do_systematic_tests = true;
  if (do_systematic_tests) {
    std::vector<unsigned> sys_ibin;
    std::vector<double> sys_x;
    const unsigned nbins=3;
    const double xmin(0.0);
    const double xmax(3.0);
    sys_ibin.push_back(0);sys_x.push_back(xmin-10.0);//underflow bin
    sys_ibin.push_back(2);sys_x.push_back(1.5);//internal bin
#ifdef LW_STRICT_ROOT_BEHAVIOUR
    sys_ibin.push_back(nbins+1);sys_x.push_back(xmax+10.0);//overflow bin
    sys_ibin.push_back(nbins+USHRT_MAX+100);sys_x.push_back(std::numeric_limits<float>::quiet_NaN());//bin out of range and x NaN
#endif
    unsigned npoints=sys_ibin.size();
    assert(npoints==sys_x.size());
    std::vector<double> sys_args;
    sys_args.push_back(0.0);
    //FIXMEsys_args.push_back(std::numeric_limits<float>::quiet_NaN());
    sys_args.push_back(-17.0);
    sys_args.push_back(17.0);

    const bool expecterror(false);
    double x,y,a;unsigned ibinx, ibiny, itest(0);
    for (unsigned ix = 0;ix<npoints;++ix)
    for (unsigned iy = 0;iy<npoints;++iy)
    for (unsigned iarg = 0;iarg<sys_args.size();++iarg)
    for (unsigned doFillX_1 = 0;doFillX_1<=1;++doFillX_1)
    for (unsigned doFillXW_1 = 0;doFillXW_1<=1;++doFillXW_1)
    for (unsigned doSetBinContent_1 = 0;doSetBinContent_1<=1;++doSetBinContent_1)
    for (unsigned doSetBinError_1 = 0;doSetBinError_1<=1;++doSetBinError_1)
      {
        ++itest;
        SYSTEST("------------> 2D Test number "<<itest)
        HistVal2D<T1,T2> h("systname","systtitle",nbins,xmin,xmax,nbins,xmin,xmax);
        h.setCompareBinContentsOnEachFill(expecterror);

        ibinx=sys_ibin.at(ix);x=sys_x.at(ix);
        ibiny=sys_ibin.at(ix);y=sys_x.at(ix);
        a=sys_args.at(iarg);
        if (doFillX_1) { SYSTEST("Fill("<<x<<", "<<y<<")"); h.fill(x,y); }
        if (doFillXW_1) { SYSTEST("Fill("<<x<<", "<<y<<", "<<a<<")"); h.fill(x,y,a); }
        if (doSetBinContent_1) { SYSTEST("setBinContent("<<ibinx<<", "<<ibiny<<", "<<a<<")"); h.setBinContent(ibinx,ibiny,a); }
        if (doSetBinError_1) { SYSTEST("setBinError("<<ibinx<<", "<<ibiny<<", "<<a<<")"); h.setBinError(ibinx,ibiny,a); }
      }
    std::cout << " ---> Systematic tests ("<<itest<<") completed"<<std::endl;
  }
  float * xbins = new float[5];
  xbins[0] = 0.0;
  xbins[1] = 1.0;
  xbins[2] = 2.7;
  xbins[3] = 2.701;
  xbins[4] = 10.0;
  double * xbinsd = new double[5];
  xbinsd[0] = 0.0;
  xbinsd[1] = 1.0;
  xbinsd[2] = 2.7;
  xbinsd[3] = 2.701;
  xbinsd[4] = 10.0;
  float * ybins = new float[3];
  ybins[0] = -20.0;
  ybins[1] = 0.0;
  ybins[2] = 100.0;
  double * ybinsd = new double[3];
  ybinsd[0] = -20.0;
  ybinsd[1] = 0.0;
  ybinsd[2] = 100.0;
  HistVal2D<T1,T2> h6("name6","title6",4,xbins,2,ybins);
  HistVal2D<T1,T2> h7("name7","title7",4,xbinsd,2,ybinsd);
  HistVal2D<T1,T2> h8("name8","title8",4,xbinsd,2,-20.0,100.0);
  HistVal2D<T1,T2> h9("name9","title9",4,0.0,10.0,2,ybinsd);
  delete[] xbins;
  delete[] ybins;
  delete[] xbinsd;
  delete[] ybinsd;

  const double eps=1e-5;

  h6.fill(2.6,1.0);h6.compareAll();
  h6.fill(2.7,1.0);h6.compareAll();
  h6.fill(0.0,1.0);h6.compareAll();
  h6.fill(10.0,1.2,1.0);h6.compareAll();
  h6.fill(-20.0,1.0);h6.compareAll();
  h6.fill(12.0,1.0);h6.compareAll();
  h6.fill(5.0,1.0);h6.compareAll();
  h6.fill(1.2,1.0);h6.compareAll();

  h7.fill(2.6,1.0);h7.compareAll();
  h7.fill(2.7+eps,1.0);h7.compareAll();
  h7.fill(0.0,1.0);h7.compareAll();
  h7.fill(10.0,1.2,1.0);h7.compareAll();
  h7.fill(-20.0,1.0);h7.compareAll();
  h7.fill(12.0,1.0);h7.compareAll();
  h7.fill(5.0,1.0);h7.compareAll();
  h7.fill(1.2,1.0);h7.compareAll();

  h8.fill(2.6,1.0);h8.compareAll();
  h8.fill(2.7+eps,1.0);h8.compareAll();
  h8.fill(0.0,1.0);h8.compareAll();
  h8.fill(10.0,1.2,1.0);h8.compareAll();
  h8.fill(-20.0,1.0);h8.compareAll();
  h8.fill(12.0,1.0);h8.compareAll();
  h8.fill(5.0,1.0);h8.compareAll();
  h8.fill(1.2,1.0);h8.compareAll();

  h9.fill(2.6,1.0);h9.compareAll();
  h9.fill(2.7,1.0);h9.compareAll();
  h9.fill(0.0,1.0);h9.compareAll();
  h9.fill(10.0,1.2,1.0);h9.compareAll();
  h9.fill(-20.0,1.0);h9.compareAll();
  h9.fill(12.0,1.0);h9.compareAll();
  h9.fill(5.0,1.0);h9.compareAll();
  h9.fill(1.2,1.0);h9.compareAll();

  std::cout << " ---> Variable binning tests completed"<<std::endl;
}

template <class T1, class T2>
void stressTestValidation() {
  const unsigned maxbins(300);
  double * xbins = new double[maxbins+1];
  for (int ih = 1; ih <= 15; ++ih) {
    double l(rand()%300000+50.232-150000);
    double u(l+rand()%1000+0.1);
    assert(u>l);
    std::ostringstream s;
    s<<"n"<<ih;
    HistVal1D<T1,T2> htest1(s.str(),"t"+s.str(),rand()%maxbins+1,l,u);
    s<<"b";
    HistVal1D<T1,T2> htest2(s.str(),"t"+s.str(),rand()%maxbins+1,l,u);
    s<<"c";
    double v = l;
    unsigned ibin=0;
    double avstep((u-l)/(rand()%maxbins+1));
    while(true) {
      xbins[ibin++] = v;
      v+=avstep*((rand()*1.0*(1./RAND_MAX))+0.001)*2;
      if (v>=u||ibin==maxbins) {
        xbins[ibin++] = u;
        break;
      }
    }
    assert(ibin>0);
    unsigned nvarbins = ibin-1;
    if (nvarbins==0) {
      nvarbins = 1;
      xbins[0] = l;
      xbins[1] = u;
    }
    assert(xbins[0]==l);
    assert(xbins[nvarbins]==u);
    assert(nvarbins>=1);
    HistVal1D<T1,T2> htest3(s.str(),"t"+s.str(),nvarbins,xbins);
    htest1.setCompareBinContentsOnEachFill(false);
    htest2.setCompareBinContentsOnEachFill(false);
    htest3.setCompareBinContentsOnEachFill(false);
    const int n(ih*ih*ih*ih);
    for (int i = 0; i < n; ++i) {
      double x(((u-l)*1.2*(rand()%1000000*1.0e-6-0.1)));
      htest1.fill(x);
      htest2.fill(x,double(rand()%10-5+0.345));
      htest3.fill(x);
      if (!(i%1000)) {
        htest1.compareBinContents();
        htest2.compareBinContents();
        htest3.compareBinContents();
      }
    }
    htest1.compareAll();
    htest2.compareAll();
    htest3.compareAll();
  }
  delete[] xbins;
}

void basicValidation_Profile(bool do_systematic_tests =  true)
{
  //Basic usage of methods:
  HistValProfile a("aprofilehist","a title for my profile hist",100, 0.0, 100.0 );
  setNamesAndTitles(a);

  a.compareAll();
  a.fill(7.0, 10.0);
  a.compareAll();
  a.fill(91.5,20.0,5.0);
  a.compareAll();
  a.fill(81.5,-20.0,-5.0);
  a.compareAll();
  a.setBinEntries(20, 5.5);
  a.compareAll();
  a.setBinContent(30, 20.2);
  a.compareAll();
  a.setBinError(40, 23.4);
  a.compareAll();
  a.setBinInfo(50, 1.0,2.0,3.0);
  a.compareAll();

#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
  double weight = 1;
#else
  double weight = 3;
#endif

  //test ranges on the profile par
  HistValProfile b("anotherprofilehist","another title for my profile hist",100, 0.0, 100.0 );
  b.fill(30.0,-1.0);
  b.fill(30.0,0.0);
  b.fill(30.0,0.5);
  b.fill(30.0,1.0);
  b.fill(30.0,2.0);
  b.fill(30.0,-1.0,weight);
  b.fill(30.0,0.0,weight);
  b.fill(30.0,0.5,weight);
  b.fill(30.0,1.0,weight);
  b.fill(30.0,2.0,weight);
  b.compareAll();

  HistValProfile b2("yetanotherprofilehist","bla",100, 0.0, 100.0, -1.0,2.0 );
  b2.fill(30.0,-1.0);
  b2.fill(30.0,0.0);
  b2.fill(30.0,0.5);
  b2.fill(30.0,1.0);
  b2.fill(30.0,2.0);
  b2.fill(30.0,-1.0,weight);
  b2.fill(30.0,0.0,weight);
  b2.fill(30.0,0.5,weight);
  b2.fill(30.0,1.0,weight);
  b2.fill(30.0,2.0,weight);
  b2.compareAll();

  std::cout << " ---> Quick basic test completed"<<std::endl;
  //Systematic:
  if (do_systematic_tests) {
    //Systematic validation of special cases:
    std::vector<unsigned> sys_ibin;
    std::vector<double> sys_x;
    const unsigned nbins=40;
    const double xmin(0.0);
    const double xmax(40.0);
    //NB: Important that these match up (x vs. ibin)!:
    sys_ibin.push_back(0);sys_x.push_back(xmin-10.0);//underflow bin
    sys_ibin.push_back(1);sys_x.push_back(0.5);//internal bin 1
    sys_ibin.push_back(6);sys_x.push_back(5.5);//internal bin 2 (<16 bins from bin1)
    sys_ibin.push_back(36);sys_x.push_back(35.5);//internal bin 3 (>16 bins from bin1 and bin2)
#ifdef LW_STRICT_ROOT_BEHAVIOUR
    sys_ibin.push_back(nbins+1);sys_x.push_back(xmax+10.0);//overflow bin
    //sys_ibin.push_back(nbins+USHRT_MAX+100);sys_x.push_back(std::numeric_limits<float>::quiet_NaN());//bin out of range and x NaN
#endif
    unsigned npoints=sys_ibin.size();
    assert(npoints==sys_x.size());
    std::vector<double> sys_args;
    sys_args.push_back(0.0);
    //FIXME: sys_args.push_back(std::numeric_limits<float>::quiet_NaN());
    sys_args.push_back(-4.0);
    sys_args.push_back(-2.0);
    sys_args.push_back(2.0);
    sys_args.push_back(4.0);
    sys_args.push_back(20.0);

    const bool expecterror(false);
    //#define SYSTEST(x) { if (HistValFunctions::verbose()) { std::cout<<"Systematic filling: "<<x<<std::endl;} }
    double x,a,a2;unsigned i, itest(0);
    for (unsigned ipoint1 = 0;ipoint1<npoints;++ipoint1)
    for (unsigned ipoint2 = 0;ipoint2<npoints;++ipoint2)
    for (unsigned iarg1 = 0;iarg1<sys_args.size();++iarg1)
    for (unsigned iarg2 = 0;iarg2<sys_args.size();++iarg2)
    for (unsigned doFillX_1 = 0;doFillX_1<=1;++doFillX_1)
#ifdef LW_EXTRATESTS
    for (unsigned doFillXW_1 = 0;doFillXW_1<=1;++doFillXW_1)
#endif
    for (unsigned doSetBinEntries_1 = 0;doSetBinEntries_1<=1;++doSetBinEntries_1)
    for (unsigned doSetBinContent_1 = 0;doSetBinContent_1<=1;++doSetBinContent_1)
    for (unsigned doSetBinError_1 = 0;doSetBinError_1<=1;++doSetBinError_1)
#ifdef LW_EXTRATESTS
    for (unsigned doFillX_2 = 0;doFillX_2<=1;++doFillX_2)
#endif
    for (unsigned doFillXW_2 = 0;doFillXW_2<=1;++doFillXW_2)
    for (unsigned doSetBinEntries_2 = 0;doSetBinEntries_2<=1;++doSetBinEntries_2)
    for (unsigned doSetBinContent_2 = 0;doSetBinContent_2<=1;++doSetBinContent_2)
    for (unsigned doSetBinError_2 = 0;doSetBinError_2<=1;++doSetBinError_2)
    for (unsigned doSetBinInfo_2 = 0;doSetBinInfo_2<=1;++doSetBinInfo_2)
      {
        ++itest;
        SYSTEST("------------> 1D-profile Test number "<<itest);
        HistValProfile h("systname","systtitle",nbins,xmin,xmax);
        h.setCompareBinContentsOnEachFill(expecterror);

        i=sys_ibin.at(ipoint1);x=sys_x.at(ipoint1); a=sys_args.at(iarg1),a2=sys_args.at(iarg2);

        bool cansetdirectly_1(false);
        if (doFillX_1) { SYSTEST("Fill("<<x<<","<<a<<")"); h.fill(x,a); cansetdirectly_1=true;}
#ifdef LW_EXTRATESTS
        if (doFillXW_1) { SYSTEST("Fill("<<x<<", "<<a<<", "<<a2<<")"); h.fill(x,a,a2); cansetdirectly_1=true;}
#endif
        if (doSetBinEntries_1&&cansetdirectly_1) { SYSTEST("setBinEntries("<<i<<", "<<a<<")"); h.setBinEntries(i,a); }
        if (doSetBinContent_1&&cansetdirectly_1) { SYSTEST("setBinContent("<<i<<", "<<a<<")"); h.setBinContent(i,a); }
        if (doSetBinError_1&&cansetdirectly_1) { SYSTEST("setBinError("<<i<<", "<<a<<")"); h.setBinError(i,a); }

        bool cansetdirectly_2 = false;
        i=sys_ibin.at(ipoint2);x=sys_x.at(ipoint2); std::swap(a,a2);
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
        a2 = 1;
#endif
#ifdef LW_EXTRATESTS
        if (doFillX_2) { SYSTEST("Fill("<<x<<", "<<a<<")"); h.fill(x,a); cansetdirectly_2 = true; }
#endif
        if (doFillXW_2) { SYSTEST("Fill("<<x<<", "<<a<<", "<<a2<<")"); h.fill(x,a,a2); cansetdirectly_2 = true; }
        if (doSetBinContent_2&&cansetdirectly_2) { SYSTEST("setBinContent("<<i<<", "<<a<<")"); h.setBinContent(i,a); }
        if (doSetBinError_2&&cansetdirectly_2) { SYSTEST("setBinError("<<i<<", "<<a<<")"); h.setBinError(i,a); }
        if (doSetBinInfo_2)
          { SYSTEST("setBinInfo("<<i<<", "<<a<<", "<<a2<<", 17.0)"); h.setBinInfo(i,a,a2,17.0); }
      }
    std::cout << " ---> Systematic tests ("<<itest<<") completed"<<std::endl;
  }

  //Floating point precision tests (since we use floats internally
  //rather than doubles):

  //First a quickie with many fills in one bin (well, we keep it at O(10^6) due to floats).
  unsigned ifptests(1);
  HistValProfile h_fp("profhist_fptest0","sometitle",1, 0.0, 1.0 );
  h_fp.setCompareBinContentsOnEachFill(false);
  for (long long unsigned i=0;i<1000000;++i)
    //h_fp.fill(0.5,1.0);
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
    h_fp.fill(0.5,1.2*(i%10));
#else
    h_fp.fill(0.5,1.2*(i%10),0.5+i%3);
#endif
  h_fp.compareAll();
  //Then a bit more systematic:
  std::vector<double> fptest_profpar, fptest_weight;
  fptest_profpar.push_back(-5.0);
  fptest_profpar.push_back(0.0);
  fptest_profpar.push_back(0.001);
  fptest_profpar.push_back(1.0);
  fptest_profpar.push_back(1.2);
  //  fptest_profpar.push_back(2.5);
  fptest_profpar.push_back(5.0);
  //  fptest_profpar.push_back(100.0);
  fptest_profpar.push_back(10000.0);

  fptest_weight.push_back(0.0);
  fptest_weight.push_back(-2.0);
  fptest_weight.push_back(0.1);
  fptest_weight.push_back(1.0);//== use unweighted fill.
  fptest_weight.push_back(1.2);
  fptest_weight.push_back(100.0);
  fptest_weight.push_back(1000.0);

  //fptest_profpar.size()*fptest_weight.size() combinations (~40-50)
  //Use 50**3 times, i.e. fill three times.

  const bool expecterror(false);
  double pp, w;
  for (unsigned ipp1 = 0; ipp1<fptest_profpar.size();++ipp1)
  for (unsigned iw1 = 0; iw1<fptest_weight.size();++iw1)
  for (unsigned ipp2 = 0; ipp2<fptest_profpar.size();++ipp2)
  for (unsigned iw2 = 0; iw2<fptest_weight.size();++iw2)
  for (unsigned ipp3 = 0; ipp3<fptest_profpar.size();++ipp3)
  for (unsigned iw3 = 0; iw3<fptest_weight.size();++iw3)
    {
      ++ifptests;
      HistValProfile h("profhist_fptest","sometitle",1, 0.0, 1.0 );
      h.setCompareBinContentsOnEachFill(expecterror);

      if (HistValFunctions::verbose())
        std::cout<<"-----> FP Test #"<<ifptests<<std::endl;
      pp = fptest_profpar.at(ipp1); w = fptest_weight.at(iw1);
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
      w = 1;
#endif
      if (HistValFunctions::verbose())
        std::cout<<"    --> Fill profpar = "<<pp<<", weight = "<<w<<std::endl;
      if (w!=1.0) h.fill(0.5,pp,w); else h.fill(0.5,pp);

      pp = fptest_profpar.at(ipp2); w = fptest_weight.at(iw2);
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
      w = 1;
#endif
      if (HistValFunctions::verbose())
        std::cout<<"    --> Fill profpar = "<<pp<<", weight = "<<w<<std::endl;
      if (w!=1.0) h.fill(0.5,pp,w); else h.fill(0.5,pp);

      pp = fptest_profpar.at(ipp3); w = fptest_weight.at(iw3);
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
      w = 1;
#endif
      if (HistValFunctions::verbose())
        std::cout<<"    --> Fill profpar = "<<pp<<", weight = "<<w<<std::endl;
      if (w!=1.0) h.fill(0.5,pp,w); else h.fill(0.5,pp);

    }
  std::cout << " ---> Floating point precision tests ("<<ifptests<<") completed"<<std::endl;

  float * xbins = new float[5];
  xbins[0] = 0.0;
  xbins[1] = 1.0;
  xbins[2] = 2.7;
  xbins[3] = 2.701;
  xbins[4] = 10.0;
  HistValProfile c("profvarbins","profile with varbins",4, xbins );
  delete[] xbins;
  c.fill(0.1,1.20);//,2.0);
  c.fill(2.6,2.0);c.compareAll();
  c.fill(2.7,2.0);c.compareAll();
  c.fill(0.0,2.0);c.compareAll();
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
  double weight2 = 1;
#else
  double weight2 = 2;
#endif
  c.fill(0.0,2.0,weight2);c.compareAll();
  c.fill(10.0,1.2,weight2);c.compareAll();
  c.fill(-20.0,2.0);c.compareAll();
  c.fill(12.0,2.0);c.compareAll();
  c.fill(5.0,2.0);c.compareAll();
  c.fill(1.2,2.0);c.compareAll();
  std::cout << " ---> Variable binning tests completed"<<std::endl;
}

void basicValidation_Profile2D(bool do_systematic_tests =  true)
{
  //Basic usage of methods:
  HistValProfile2D a("a2dprofilehist","a title for my 2d profile hist",100, 0.0, 100.0,100, 0.0, 100.0 );
  setNamesAndTitles(a);


  a.compareAll();
  a.fill(7.0, 17.0,10.0);
  a.compareAll();
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
  a.fill(91.5,17.0, 20.0);
#else
  a.fill(91.5,17.0, 20.0,5.0);
#endif
  a.compareAll();
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
  a.fill(81.5,17.0,-20.0);
#else
  a.fill(81.5,17.0,-20.0,-5.0);
#endif
  a.compareAll();
  a.setBinEntries(20, 17,5.5);
  a.compareAll();
  a.setBinContent(30, 17, 20.2);
  a.compareAll();
  a.setBinError(40, 17, 23.4);
  a.compareAll();
  a.setBinInfo(50, 17, 1.0,2.0,3.0);
  a.compareAll();

#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
  double weight = 1;
#else
  double weight = 3;
#endif

  //test ranges on the profile par
  HistValProfile2D b("another2dprofilehist","another title for my 2dprofile hist",
                     100, 0.0, 100.0, 100, 0.0, 100.0, 0.0,1.0 );
  b.fill(30.0,10.0,-1.0);
  b.fill(30.0,10.0,0.0);
  b.fill(30.0,10.0,0.5);
  b.fill(30.0,10.0,1.0);
  b.fill(30.0,10.0,2.0);
  b.fill(30.0,10.0,-1.0,weight);
  b.fill(30.0,10.0,0.0,weight);
  b.fill(30.0,10.0,0.5,weight);
  b.fill(30.0,10.0,1.0,weight);
  b.fill(30.0,10.0,2.0,weight);

  std::cout << " ---> Quick basic test completed"<<std::endl;
  //Systematic:
  if (do_systematic_tests) {
    std::vector<unsigned> sys_ibin;
    std::vector<double> sys_x;
    const unsigned nbins=3;
    const double xmin(0.0);
    const double xmax(3.0);
    sys_ibin.push_back(0);sys_x.push_back(xmin-10.0);//underflow bin
    sys_ibin.push_back(2);sys_x.push_back(1.5);//internal bin
#ifdef LW_STRICT_ROOT_BEHAVIOUR
    sys_ibin.push_back(nbins+1);sys_x.push_back(xmax+10.0);//overflow bin
    //sys_ibin.push_back(nbins+USHRT_MAX+100);sys_x.push_back(std::numeric_limits<float>::quiet_NaN());//bin out of range and x NaN
#endif
    unsigned npoints=sys_ibin.size();
    assert(npoints==sys_x.size());
    std::vector<double> sys_args;
    sys_args.push_back(0.0);
    //FIXMEsys_args.push_back(std::numeric_limits<float>::quiet_NaN());
    sys_args.push_back(-17.0);
    sys_args.push_back(17.0);

    const bool expecterror(false);
    double x,y,a,a2;unsigned ibinx, ibiny, itest(0);
    for (unsigned ix = 0;ix<npoints;++ix)
    for (unsigned iy = 0;iy<npoints;++iy)
    for (unsigned iarg = 0;iarg<sys_args.size();++iarg)
    for (unsigned iarg2 = 0;iarg2<sys_args.size();++iarg2)
    for (unsigned doFillX_1 = 0;doFillX_1<=1;++doFillX_1)
    for (unsigned doFillXW_1 = 0;doFillXW_1<=1;++doFillXW_1)
    for (unsigned doSetBinEntries_1 = 0;doSetBinEntries_1<=1;++doSetBinEntries_1)
    for (unsigned doSetBinContent_1 = 0;doSetBinContent_1<=1;++doSetBinContent_1)
    for (unsigned doSetBinError_1 = 0;doSetBinError_1<=1;++doSetBinError_1)
    for (unsigned doSetBinInfo_1 = 0;doSetBinInfo_1<=1;++doSetBinInfo_1)
      {
        if (doSetBinError_1&&!doSetBinEntries_1)
          continue;//We can't call setBinError unless we are sure there are some entries.
        ++itest;
        SYSTEST("------------> 2D-profile Test number "<<itest)
        HistValProfile2D h("systname","systtitle",nbins,xmin,xmax,nbins,xmin,xmax);
        h.setCompareBinContentsOnEachFill(expecterror);

        ibinx=sys_ibin.at(ix);x=sys_x.at(ix);
        ibiny=sys_ibin.at(ix);y=sys_x.at(ix);
        a=sys_args.at(iarg);
        a2=sys_args.at(iarg2);
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
        a = 1;
        a2 = 1;
#endif
        if (doFillX_1) { SYSTEST("Fill("<<x<<", "<<y<<", "<<a2<<")"); h.fill(x,y,a2); }
        if (doFillXW_1) { SYSTEST("Fill("<<x<<", "<<y<<", "<<a2<<", "<<a<<")"); h.fill(x,y,a2,a); }
        //Fixme: setbinentries? setbininfo
        if (doSetBinEntries_1) { SYSTEST("setBinEntries("<<ibinx<<", "<<ibiny<<", "<<a<<")"); h.setBinEntries(ibinx,ibiny,a); }
        if (doSetBinContent_1) { SYSTEST("setBinContent("<<ibinx<<", "<<ibiny<<", "<<a<<")"); h.setBinContent(ibinx,ibiny,a); }
        if (doSetBinError_1) { SYSTEST("setBinError("<<ibinx<<", "<<ibiny<<", "<<a<<")"); h.setBinError(ibinx,ibiny,a); }
         if (doSetBinInfo_1)
           { SYSTEST("setBinInfo("<<ibinx<<", "<<ibiny<<", "<<a<<", "<<a2<<", 17.0)"); h.setBinInfo(ibinx,ibiny,a,a2,17.0); }

      }
    std::cout << " ---> Systematic tests ("<<itest<<") completed"<<std::endl;
  }

  double * xbins = new double[5];
  xbins[0] = 0.0;
  xbins[1] = 1.0;
  xbins[2] = 2.7;
  xbins[3] = 2.701;
  xbins[4] = 10.0;
  double * ybins = new double[3];
  ybins[0] = -20.0;
  ybins[1] = 0.0;
  ybins[2] = 100.0;
  HistValProfile2D hvb("hvb","sometitle",4,xbins,2,ybins);
  delete[] xbins;
  delete[] ybins;
  const double eps(1.0e-4);//we use epsilon to avoid edges due to
                           //double vs. float used for binning in root
                           //vs. lwhists.
  hvb.compareAll();
  hvb.fill(2.6,1.0,2.0,2.0);  hvb.compareAll();
  hvb.fill(2.7+eps,1.0, 2.0);hvb.compareAll();
  hvb.fill(0.0+eps,1.0, 2.0);hvb.compareAll();
#ifdef LW_OLD_WEIGHTED_PROFILE_ERRORS
  hvb.fill(10.0-eps,1.0,  1.0);hvb.compareAll();
#else
  hvb.fill(10.0-eps,1.0,  1.0,2.0);hvb.compareAll();
#endif
  hvb.fill(-20.0,1.0, 2.0);hvb.compareAll();
  hvb.fill(12.0,1.0, 2.0);hvb.compareAll();
  hvb.fill(5.0,1.0, 2.0);hvb.compareAll();
  hvb.fill(1.2,1.0, 2.0);hvb.compareAll();
}

void usage(char**argv,int exitcode) {
  std::cout << "Usage:"<<std::endl;
  std::cout << ""<<std::endl;
  std::cout << argv[0]<<" [-h|--help] [-v|--verbose] [--no1d] [--no2d] [--noprofile] [--quick|--noquick] [--rootbackend] [--triggerconv] [--seed=XXX]"<<std::endl;
  std::cout <<""<<std::endl;
  exit(exitcode);
}
int main (int argc, char** argv) {

  bool cfg_rootbackend(false);
  bool cfg_triggerrootconversion(false);
  unsigned cfg_seed(UINT_MAX);
  bool cfg_do1d(true);
  bool cfg_do2d(true);
  bool cfg_doprofile(true);
  bool cfg_quick(false);
#ifndef NDEBUG
  //Debug builds have quick by default, to even out run times a bit.
  cfg_quick = true;
#endif
  bool sawquickopt(false);
  bool cfg_verbose(false);
  for (int i=1; i<argc;++i) {
     std::string arg(argv[i]);
     if (arg=="-h"||arg=="--help")
       usage(argv,0);
     if (arg=="-v"||arg=="--verbose") {
       if (cfg_verbose)
         usage(argv,1);
       cfg_verbose = true;
       continue;
     }
     if (arg=="--no1d") {
       if (!cfg_do1d)
         usage(argv,1);
       cfg_do1d = false;
       continue;
     }
     if (arg=="--no2d") {
       if (!cfg_do2d)
         usage(argv,1);
       cfg_do2d = false;
       continue;
     }
     if (arg=="--noprofile") {
       if (!cfg_doprofile)
         usage(argv,1);
       cfg_doprofile = false;
       continue;
     }
     if (arg=="--quick") {
       if (sawquickopt)
         usage(argv,1);
       sawquickopt = true;
       cfg_quick = true;
       continue;
     }
     if (arg=="--noquick") {
       if (sawquickopt)
         usage(argv,1);
       sawquickopt = true;
       cfg_quick = false;
       continue;
     }
     if (arg=="--rootbackend") {
       if (cfg_rootbackend)
         usage(argv,1);
       cfg_rootbackend = true;
       continue;
     }
     if (arg=="--triggerconv") {
       if (cfg_triggerrootconversion)
         usage(argv,1);
       cfg_triggerrootconversion = true;
       continue;
     }
     if (arg.rfind("--seed=")==0&&arg.size()>=8) {
       if (cfg_seed!=UINT_MAX)
         usage(argv,1);
       unsigned iseed = atoi(&(arg.c_str()[7]));
       std::ostringstream ss; ss<<iseed;
       if (arg!="--seed="+ss.str())
         usage(argv,1);
       cfg_seed = iseed;
       continue;
     }
     usage(argv,1);
  }

  HistValFunctions::setVerbose(cfg_verbose);
  LWHistControls::setROOTBackend(cfg_rootbackend);
  HistVal_trigger_conversion_all = cfg_triggerrootconversion;

  if (cfg_rootbackend)
    HistValFunctions::TH1_FieldsAccess::set_fgAddDirectory(false);//get rid of "possible memory leak" warnings

  std::cout<<"Starting tests"<<std::endl;

  std::cout<<"Testing types..."<<std::endl;
  HistValFunctions::testTypes();

  //seed random numbers;
  unsigned actual_seed = (cfg_seed==0||cfg_seed==UINT_MAX) ? (unsigned)time(0) : cfg_seed;
  std::cout<<"NB: Stress-tests will use random seed = "<<actual_seed<<std::endl;
  srand(actual_seed);

  if (cfg_do1d) {
    std::cout<<"Performing basic validation of TH1F vs. TH1F_LW..."<<std::endl;
    basicValidation<TH1F,TH1F_LW>();
    std::cout<<"Performing basic validation of TH1D vs. TH1D_LW..."<<std::endl;
    basicValidation<TH1D,TH1D_LW>(!cfg_quick);
    std::cout<<"Performing basic validation of TH1I vs. TH1I_LW..."<<std::endl;
    basicValidation<TH1I,TH1I_LW>(!cfg_quick);

    std::cout<<"Performing stress-test level validations of TH1F vs. TH1F_LW..."<<std::endl;
    stressTestValidation<TH1F,TH1F_LW>();
    std::cout<<"Performing stress-test level validations of TH1D vs. TH1D_LW..."<<std::endl;
    stressTestValidation<TH1D,TH1D_LW>();
    std::cout<<"Performing stress-test level validations of TH1I vs. TH1I_LW..."<<std::endl;
    stressTestValidation<TH1I,TH1I_LW>();
  }

  if (cfg_do2d) {
    std::cout<<"Performing basic validation of TH2F vs. TH2F_LW..."<<std::endl;
    basicValidation2D<TH2F,TH2F_LW>();
    std::cout<<"Performing basic validation of TH2D vs. TH2D_LW..."<<std::endl;
    basicValidation2D<TH2D,TH2D_LW>();
    std::cout<<"Performing basic validation of TH2I vs. TH2I_LW..."<<std::endl;
    basicValidation2D<TH2I,TH2I_LW>();
  }

  if (cfg_doprofile) {
    std::cout<<"Performing basic validation of TProfile vs. TProfile_LW..."<<std::endl;
    basicValidation_Profile(!cfg_quick);
    std::cout<<"Performing basic validation of TProfile2D vs. TProfile2D_LW..."<<std::endl;
    basicValidation_Profile2D();
  }



  LWHistControls::releaseAllHeldMemory();

  std::cout <<"All enabled tests succesfully completed"<<std::endl;

  //Magic report triggering lines in case librootspy.so is preloaded:
  TH1F * hreport = new TH1F("rootspy","",1,0,1);
  hreport->Fill("rootspy_producereport",0.123456);
  delete hreport;
}
