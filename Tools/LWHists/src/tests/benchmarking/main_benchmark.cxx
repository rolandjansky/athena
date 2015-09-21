/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LWHists/LWHistControls.h"

#include "TH1I.h"
#include "LWHists/TH1I_LW.h"
#include "TH1F.h"
#include "LWHists/TH1F_LW.h"
#include "TH1D.h"
#include "LWHists/TH1D_LW.h"
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
#include "TRandom3.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <climits>
#include <cstdlib>

inline long long thisProcess_VirtualMemUsed_kB()
{
  FILE* file = fopen("/proc/self/status", "r");
  if (!file) {
    std::cerr << "ERROR: Could not open /proc/self/stat"<<std::endl;
    return -1;
  };
  long long result = -1;
  char line[128];
  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmSize:", 7) == 0) {
      std::stringstream s(&(line[7]));
      s >> result;
      break;
    }
  }
  fclose(file);
  return result;
}

inline double vmemUsedMB()
{
  return thisProcess_VirtualMemUsed_kB()/1024.0;
}

template <class T> std::string histClassName() { return "unknown"; }
template <> std::string histClassName<TH1I>() { return "TH1I"; }
template <> std::string histClassName<TH1I_LW>() { return "TH1I_LW"; }
template <> std::string histClassName<TH1F>() { return "TH1F"; }
template <> std::string histClassName<TH1F_LW>() { return "TH1F_LW"; }
template <> std::string histClassName<TH1D>() { return "TH1D"; }
template <> std::string histClassName<TH1D_LW>() { return "TH1D_LW"; }
template <> std::string histClassName<TH2I>() { return "TH2I"; }
template <> std::string histClassName<TH2I_LW>() { return "TH2I_LW"; }
template <> std::string histClassName<TH2F>() { return "TH2F"; }
template <> std::string histClassName<TH2F_LW>() { return "TH2F_LW"; }
template <> std::string histClassName<TH2D>() { return "TH2D"; }
template <> std::string histClassName<TH2D_LW>() { return "TH2D_LW"; }
template <> std::string histClassName<TProfile>() { return "TProfile"; }
template <> std::string histClassName<TProfile_LW>() { return "TProfile_LW"; }
template <> std::string histClassName<TProfile2D>() { return "TProfile2D"; }
template <> std::string histClassName<TProfile2D_LW>() { return "TProfile2D_LW"; }

long time_clock;
void timerStart() { time_clock = clock(); }
double timerEnd(const std::string& text, const unsigned& n, const unsigned estimatedOverheadToDiscount = 0)
{
  double t((clock()-time_clock-estimatedOverheadToDiscount)*(1./double(CLOCKS_PER_SEC))*1.0e6/n);
  std::cout<<"  Timed ["<<text<<"]: "<<t<<" microsecond"<<std::endl;
  return t;
}

long long lastMemNKB;
void memCheckStart() { lastMemNKB = thisProcess_VirtualMemUsed_kB(); }
double memCheckEnd(const std::string& text, const unsigned& n, const double& extra=0.0)
{
  double m(extra+double(thisProcess_VirtualMemUsed_kB()-lastMemNKB)*double(1024.0)/n);
  std::cout<<"  Mem-usage ["<<text<<"]: "<<m<<" bytes"<<std::endl;
  return m;
}

TRandom3 theRandGen(117);
double getRandX() { return theRandGen.Gaus(50.0,12.5); }

// double getRandX() { return 102.0*(rand()*1.0/RAND_MAX)-1.0; }

template <class T> void fillX(T*t) { t->Fill(getRandX()); }
template <class T> void fillXW(T*t) { t->Fill(getRandX(),1.2); }
template <> void fillX(TH2F*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TH2D*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TH2I*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TH2F_LW*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TH2D_LW*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TH2I_LW*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TProfile*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TProfile_LW*t) { t->Fill(getRandX(),15.0); }
template <> void fillX(TProfile2D*t) { t->Fill(getRandX(),getRandX(),15.0); }
template <> void fillX(TProfile2D_LW*t) { t->Fill(getRandX(),getRandX(),15.0); }
template <> void fillXW(TH2F*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TH2D*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TH2I*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TH2F_LW*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TH2D_LW*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TH2I_LW*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TProfile*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TProfile_LW*t) { t->Fill(getRandX(),getRandX(),1.2); }
template <> void fillXW(TProfile2D*t) { t->Fill(getRandX(),getRandX(),15.0,1.2); }
template <> void fillXW(TProfile2D_LW*t) { t->Fill(getRandX(),getRandX(),15.0,1.2); }

template <class T> void fakeFillRandGen() { getRandX(); }
template <> void fakeFillRandGen<TH2F>() { getRandX();getRandX(); }
template <> void fakeFillRandGen<TH2D>() { getRandX();getRandX(); }
template <> void fakeFillRandGen<TH2I>() { getRandX();getRandX(); }
template <> void fakeFillRandGen<TH2F_LW>() { getRandX();getRandX(); }
template <> void fakeFillRandGen<TH2D_LW>() { getRandX();getRandX(); }
template <> void fakeFillRandGen<TH2I_LW>() { getRandX();getRandX(); }

template <class T> T* book(const std::string& n, const std::string& t, unsigned nbins)
{T*h=new T(n.c_str(),t.c_str(),nbins,0.0,100.0);
  h->Sumw2();//TODO monitor sumw2!!
 return h; }


template <> TH1F_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TH1F_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0); }
template <> TH1D_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TH1D_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0); }
template <> TH1I_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TH1I_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0); }
template <> TProfile_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TProfile_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0); }
template <> TH2F_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TH2F_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }
template <> TH2D_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TH2D_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }
template <> TH2I_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TH2I_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }
template <> TH2F* book(const std::string& n, const std::string& t, unsigned nbins)
{ return new TH2F(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }
template <> TH2D* book(const std::string& n, const std::string& t, unsigned nbins)
{ return new TH2D(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }
template <> TH2I* book(const std::string& n, const std::string& t, unsigned nbins)
{ return new TH2I(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }
template <> TProfile2D* book(const std::string& n, const std::string& t, unsigned nbins)
{ return new TProfile2D(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }
template <> TProfile2D_LW* book(const std::string& n, const std::string& t, unsigned nbins)
{ return TProfile2D_LW::create(n.c_str(),t.c_str(),nbins,0.0,100.0,nbins,0.0,100.0); }

template <class T> void triggerConversion(T*) { /*ROOT classes*/ }
template <> void triggerConversion(TH1F_LW* h) { h->getROOTHist(); }
template <> void triggerConversion(TH1D_LW* h) { h->getROOTHist(); }
template <> void triggerConversion(TH1I_LW* h) { h->getROOTHist(); }
template <> void triggerConversion(TH2F_LW* h) { h->getROOTHist(); }
template <> void triggerConversion(TH2D_LW* h) { h->getROOTHist(); }
template <> void triggerConversion(TH2I_LW* h) { h->getROOTHist(); }
template <> void triggerConversion(TProfile_LW* h) { h->getROOTHist(); }
template <> void triggerConversion(TProfile2D_LW* h) { h->getROOTHist(); }

template <class T> bool isROOT() { return true; }
template <> bool isROOT<TH1F_LW>() { return false; }
template <> bool isROOT<TH1D_LW>() { return false; }
template <> bool isROOT<TH1I_LW>() { return false; }
template <> bool isROOT<TH2F_LW>() { return false; }
template <> bool isROOT<TH2D_LW>() { return false; }
template <> bool isROOT<TH2I_LW>() { return false; }
template <> bool isROOT<TProfile_LW>() { return false; }
template <> bool isROOT<TProfile2D_LW>() { return false; }

template <class T> void safeDelete(T*h)
{
  if (isROOT<T>()) {
    delete dynamic_cast<TH1*>(h);
  } else {
    LWHist::safeDelete(reinterpret_cast<LWHist*>(h));
  }
}

template <class T>
int performBenchmark( const unsigned nhists,
		      const unsigned nbins,
		      const int nfills /*<0 for fill(x,w)*/)
{

  std::vector<std::pair<std::string,T*> > hists(nhists);
  typename std::vector<std::pair<std::string,T*> >::iterator it,itE (hists.end());
  for (unsigned i = 0; i < nhists; ++i) {
    std::ostringstream s;
    s << "some_hist_"<<i;
    hists.at(i)=std::pair<std::string,T*>(s.str(),0);
  }

  //First we book:
  std::string title("This is some title");
  it=hists.begin();
  memCheckStart();
  timerStart();
  for (;it!=itE;++it)
    it->second = book<T>(it->first,title.c_str(),nbins);
  timerEnd(histClassName<T>()+" booking",nhists);
  double mem_booked = memCheckEnd("Booked size ["+histClassName<T>()+"]",hists.size());

  if (!isROOT<T>())
    std::cout<<"Pool wastage: "<<LWHistControls::poolWasteFraction()*100.0<<" %"<<std::endl;

  const unsigned nrandfills = (nfills<0?-nfills:nfills);

  //Estimate overhead of random filling (looping & random number generation):
  const long clock1 = clock();
  it=hists.begin();
  for (;it!=itE;++it)
    for (unsigned i = 0; i < nrandfills; ++i)
      fakeFillRandGen<T>();
  const long loopandrandgenoverhead(clock()-clock1);

  if (nfills!=0) {
    memCheckStart();


    //Time random fills:
    it=hists.begin();
    timerStart();
    if (nfills<0) {
      for (;it!=itE;++it)
	for (unsigned i = 0; i < nrandfills; ++i)
	  fillXW(it->second);
      timerEnd(histClassName<T>()+" average time of random fills (Fill(x,w))",nhists*nrandfills,loopandrandgenoverhead);
    } else {
      for (;it!=itE;++it)
	for (unsigned i = 0; i < nrandfills; ++i)
	  fillX(it->second);
      timerEnd(histClassName<T>()+" average time of random fills (Fill(x))",nhists*nrandfills,loopandrandgenoverhead);
    }

    memCheckEnd("Size after random fills ["+histClassName<T>()+"]",hists.size(),mem_booked);

    if (!isROOT<T>())
      std::cout<<"Pool wastage: "<<LWHistControls::poolWasteFraction()*100.0<<" %"<<std::endl;
  }
  if (!isROOT<T>()) {
    //Fixme: Perform conversion benchmarks!

    it=hists.begin();
    long base_t0(clock());
    for (;it!=itE;++it)
      it->second->GetName();
    long base_t1(clock());

    it = hists.begin();
    long actual_t0(clock());
    for (;it!=itE;++it)
      triggerConversion(it->second);
    long actual_t1(clock());
    std::cout<<"Conversion took "<<((actual_t1-actual_t0)-(base_t1-base_t0))/(double(CLOCKS_PER_SEC)*hists.size())*1.0e3<<" ms/hist"<<std::endl;
  }

  //For valgrind we also delete the histograms again:
  it=hists.begin();
  for (;it!=itE;++it) {
    safeDelete(it->second);
  }

  LWHistControls::releaseAllHeldMemory();
  //Magic report triggering lines in case librootspy.so is preloaded:
  TH1F * hreport = new TH1F("rootspy","",1,0,1);
  hreport->Fill("rootspy_producereport",0.123456);
  delete hreport;
  return 0;
}

std::string progname;
int usage()
{
  std::cout << "Usage:\n\n  "
	    << progname << " [float|double|int|profile] [1d|2d] [root|lw|lwrb] [nbins] [nfills] [nhists]\n\n"
	    << "    float|double|int|profile : type of histogram (e.g. TH1F vs. TH1D vs. TH1I vs. TProfile)\n\n"
	    << "    1d|2d : Dimension of histogram (e.g. TH1F vs. TH2F)\n\n"
	    << "    'root': benchmark cpu/vmem usage of booking & filling root hists\n"
	    << "    'lw': benchmark cpu/vmem usage of booking & filling lw hists + conversion lw->root\n\n"
	    << "    'lwrb': benchmark cpu/vmem usage of booking & filling lw hists using a root backend\n\n"
	    << "    nbins : Number of histogram bins (in both dimensions for 2D hists)\n\n"
	    << "    nfills : Number of fills per histogram (put <0 for fill(x,w) rather than fill(x))\n\n"
	    << "    nhists : Number of histograms to use for test"
	    << std::endl;
  return 1;
}
int main (int argc, char** argv) {
  progname=argv[0];
  if (argc!=7)
    return usage();
  const int nbins = atoi(argv[4]);
  const int nfills = atoi(argv[5]);
  const int nhists = atoi(argv[6]);
  if (nbins<1||nbins>USHRT_MAX-1||nhists<1||nhists>1000000)
    return usage();

  std::string valtype(argv[1]),dim(argv[2]),histimpl(argv[3]);
  if (histimpl=="lwrb") {
    histimpl = "lw";
    LWHistControls::setROOTBackend(true);
    std::cout<<"Using ROOT backend for LW histograms."<<std::endl;
  }

  if (valtype=="float"&&dim=="1d"&&histimpl=="lw")
    return performBenchmark<TH1F_LW> (nhists,nbins,nfills);
  if (valtype=="double"&&dim=="1d"&&histimpl=="lw")
    return performBenchmark<TH1D_LW> (nhists,nbins,nfills);
  if (valtype=="int"&&dim=="1d"&&histimpl=="lw")
    return performBenchmark<TH1I_LW> (nhists,nbins,nfills);
  if (valtype=="profile"&&dim=="1d"&&histimpl=="lw")
    return performBenchmark<TProfile_LW> (nhists,nbins,nfills);
  if (valtype=="float"&&dim=="1d"&&histimpl=="root")
    return performBenchmark<TH1F> (nhists,nbins,nfills);
  if (valtype=="double"&&dim=="1d"&&histimpl=="root")
    return performBenchmark<TH1D> (nhists,nbins,nfills);
  if (valtype=="int"&&dim=="1d"&&histimpl=="root")
    return performBenchmark<TH1I> (nhists,nbins,nfills);
  if (valtype=="profile"&&dim=="1d"&&histimpl=="root")
    return performBenchmark<TProfile> (nhists,nbins,nfills);

  if (valtype=="float"&&dim=="2d"&&histimpl=="lw")
    return performBenchmark<TH2F_LW> (nhists,nbins,nfills);
  if (valtype=="double"&&dim=="2d"&&histimpl=="lw")
    return performBenchmark<TH2D_LW> (nhists,nbins,nfills);
  if (valtype=="int"&&dim=="2d"&&histimpl=="lw")
    return performBenchmark<TH2I_LW> (nhists,nbins,nfills);
  if (valtype=="profile"&&dim=="2d"&&histimpl=="lw")
    return performBenchmark<TProfile2D_LW> (nhists,nbins,nfills);
  if (valtype=="float"&&dim=="2d"&&histimpl=="root")
    return performBenchmark<TH2F> (nhists,nbins,nfills);
  if (valtype=="double"&&dim=="2d"&&histimpl=="root")
    return performBenchmark<TH2D> (nhists,nbins,nfills);
  if (valtype=="int"&&dim=="2d"&&histimpl=="root")
    return performBenchmark<TH2I> (nhists,nbins,nfills);
  if (valtype=="profile"&&dim=="2d"&&histimpl=="root")
    return performBenchmark<TProfile2D> (nhists,nbins,nfills);

  return usage();
}
