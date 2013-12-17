/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TStyle.h"

// Local
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"

//------------------------------------------------------------------------------------------
// return (create for a first call) factory instance
//------------------------------------------------------------------------------------------
Anp::HistMan& Anp::HistMan::Instance()
{
  static Anp::HistMan instance;
  return instance;
}

using namespace std;

//-----------------------------------------------------------------------------------------
Anp::HistMan::HistMan()
   :fNMiss(0),
    fKeyWidth(4),
    fDebug(false)
{
}

//-----------------------------------------------------------------------------------------
Anp::HistMan::~HistMan()
{
}

//-----------------------------------------------------------------------------------------
void Anp::HistMan::Config(const Registry &reg)
{
  reg.Get("HistMan", "Debug", fDebug);
}

//-----------------------------------------------------------------------------------------
bool Anp::HistMan::ReadAxis(const Registry &reg, AxisInfo &axis) const
{
  //
  // Read axis info from Registry - require binning definition
  //
  reg.Get("title", axis.fTitle);

  if(!reg.Get("bin", axis.fNbins) || 
     !reg.Get("min", axis.fMin) || 
     !reg.Get("max", axis.fMax)) {
    //
    // List comma separated list of bins
    //
    if(!reg.Get("bins", axis.fBins)) {
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------------------
bool Anp::HistMan::ReadFile(const string path)
{
  //
  // Parse xml file with histogram definitions. These are top level keys
  // which must be located within "histograms" object:
  //    keybase - creates multiple keys for one histogram
  //    dirbase - stores keys in "sub-directory"
  //    h1d or hist - histogram definition
  //

  Anp::Registry xml_reg = Anp::ParseXml(path);

  string root_element;
  xml_reg.Get("RootElement", root_element);
  
  if(root_element != "histograms") {
    cerr << "HistMan::ReadFile - wrong root element=\"" << root_element << "\"" << endl;
    return false;
  }

  //
  // Read "directory" name and list of keys
  //
  Registry dir_reg, key_reg;
  xml_reg.Get("dirbase", dir_reg);
  xml_reg.Get("keybase", key_reg);
  
  vector<int> keyvec;
  vector<string> dirvec;

  dir_reg.Get("dirs", dirvec);
  key_reg.Get("dirs", keyvec);  

  InfoMap hmap;

  while(xml_reg.KeyExists("hist")) {
    //
    // Read and then erase one histogram key
    //
    Anp::Registry hist_reg;
    xml_reg.Get("hist", hist_reg);
    xml_reg.RemoveKey("hist");

    HistInfo hist;

    if(!hist_reg.Get("key", hist.fKey)) {
      continue;
    }
    else {
      hist_reg.Get("name",  hist.fName);
      hist_reg.Get("title", hist.fTitle);
    }

    //
    // Read axis definitions - require valid Xaxis data
    //
    Anp::Registry xreg, yreg;
    
    hist_reg.Get("Xaxis", xreg);
    hist_reg.Get("Yaxis", yreg);

    if(!ReadAxis(xreg, hist.fXaxis)) {
      continue;
    }
    else {
      ReadAxis(yreg, hist.fYaxis);
    }
    
    if(!hmap.insert(InfoMap::value_type(hist.GetKey(), hist)).second) {
      cerr << "HistMan::ReadFile - histogram key " << hist.GetKey() << " already exists" << endl;
    }
  }
  
  AddHistogramBlock(hmap, keyvec, dirvec);

  return true;
}

//-----------------------------------------------------------------------------------------
bool Anp::HistMan::AddHistogramBlock(InfoMap &hmap,
				     const vector<int> &basekeys,
				     vector<string> &dirvec)
{
   vector<vector<int> > basevec;
   basevec.push_back(basekeys);

   vector<int> allvec;
   for(vector<vector<int> >::const_iterator bit = basevec.begin(); bit != basevec.end(); ++bit)
   {
      const vector<int> prevvec = allvec;
      const vector<int> &currvec = *bit;
      
      for(vector<int>::const_iterator cit = currvec.begin(); cit != currvec.end(); ++cit)
      {
	 if(prevvec.empty())
	 {
	    allvec.push_back(*cit);
	 }
	 else
	 {
	    for(vector<int>::const_iterator pit = prevvec.begin(); pit != prevvec.end(); ++pit)
	    {
	       allvec.push_back(*cit + *pit);
	    }
	 } 
      }
   }

   InfoMap nmap = hmap;

   for(vector<int>::const_iterator it = allvec.begin(); it != allvec.end(); ++it)
   {
      const int base = *it;

      for(InfoMap::const_iterator hit = hmap.begin(); hit != hmap.end(); ++hit)
      {
	 const int oldkey = std::atoi((hit -> first).c_str());
	 if(oldkey == 0)
	 {
	    continue;
	 }

	 const int newkey = base + oldkey;
	 HistInfo info = hit -> second;

	 stringstream strkey;
	 strkey << newkey;
	 info.fKey = strkey.str();

	 if(!nmap.insert(InfoMap::value_type(info.GetKey(), info)).second)
	 {
	    cerr << "HistMan::ReadFile - histogram key " << info.GetKey() << " already exists" << endl;
	 }

	 const unsigned int width = ComputeWidth(newkey);
	 fKeyWidth = max<int>(width, fKeyWidth);

	 if(width != info.GetKey().size())
	 {
	    cerr << "HistMan::ReadFile - integer width and string size are not equal" << endl;
	 }
      }
   }

   if(dirvec.empty())
   {
      dirvec.push_back(".");
   }

   for(vector<string>::const_iterator dit = dirvec.begin(); dit != dirvec.end(); ++dit)
   {
      InfoMap &imap = fDirMap[*dit];

      for(InfoMap::const_iterator nit = nmap.begin(); nit != nmap.end(); ++nit)
      {
	 if(!imap.insert(InfoMap::value_type(*nit)).second)
	 {
	    cerr << "HistMan::ReadFile - failed to add " << nit -> first << " to " << *dit << endl;
	 }
      }
   }

   return true;
}

//-----------------------------------------------------------------------------------------      
bool Anp::HistMan::KeyExists(const int key, const string &dir) const
{
   return HistMan::KeyExists(HistMan::Convert(key), dir);
}

//-----------------------------------------------------------------------------------------      
bool Anp::HistMan::KeyExists(const string &key, const string &dir) const
{
   DirIter dit = fDirMap.find(dir);
   if(dit == fDirMap.end())
   {
      return false;
   }

   const InfoMap &imap = dit -> second;

   return (imap.find(key) != imap.end());
}

//-----------------------------------------------------------------------------------------      
const Anp::Hist1d<double> Anp::HistMan::CreateHist1d(const int key, const string &dir)
{
   return HistMan::CreateHist1d(HistMan::Convert(key), dir);
}

//-----------------------------------------------------------------------------------------      
const Anp::Hist1d<double> Anp::HistMan::CreateHist1d(const string &key, const string &dir)
{
   DirIter dit = fDirMap.find(dir);
   if(dit == fDirMap.end())
   {
      ++fNMiss;
      return Hist1d<double>(0, 0.0, 0.0);
   }

   const InfoMap &imap = dit -> second;

   InfoIter hit = imap.find(key);
   if(hit == imap.end())
   {
      ++fNMiss;
      return Hist1d<double>(0, 0.0, 0.0);
   }

   return HistMan::CreateHist1d(hit -> second);
}

//-----------------------------------------------------------------------------------------      
TH1* Anp::HistMan::CreateTH1(const int key, const string &dir)
{
   return HistMan::CreateTH1(HistMan::Convert(key), dir);
}

//-----------------------------------------------------------------------------------------      
TH1* Anp::HistMan::CreateTH1(const string &key, const string &dir_)
{
   //
   // Extract option
   //
   string opt;
   string dir = dir_;
   if     (dir.find("profile") != string::npos) opt += "profile";
   else if(dir.find("float")   != string::npos) opt += "float";
   
   if(!opt.empty() && dir.find(opt.c_str()) != string::npos)
   {
      dir.erase(dir.find(opt.c_str()), opt.size());
   }

   if(dir.empty())
   {
      dir = ".";
   }
   DirIter dit = fDirMap.find(dir);
   if(dit == fDirMap.end())
   {
      ++fNMiss;
      cerr << "HistMan::CreateTH1 - Directory \"" << dir << "\" not found" << endl;
      return 0;
   }

   const InfoMap &imap = dit -> second;

   InfoIter hit = imap.find(key);
   if(hit == imap.end())
   {
      ++fNMiss;
      if(fDebug) {
	cerr << "HistMan::CreateTH1 - Histogram key \"" << key << "\" not found" << endl; 
      }
      return 0;
   }

   return HistMan::CreateTH1(hit -> second, opt);
}

//-----------------------------------------------------------------------------------------      
TH2* Anp::HistMan::CreateTH2(const int key, const string &dir)
{
   return HistMan::CreateTH2(Convert(key), dir);
}

//-----------------------------------------------------------------------------------------      
TH2* Anp::HistMan::CreateTH2(const string &key, const string &dir_)
{
   //
   // Extract option
   //
   string opt;
   string dir = dir_;
   if     (dir.find("double") != string::npos) opt += "double";
   
   if(!opt.empty() && dir.find(opt.c_str()) != string::npos)
   {
      dir.erase(dir.find(opt.c_str()), opt.size());
   }

   if(dir.empty())
   {
      dir = ".";
   }


   DirIter dit = fDirMap.find(dir);
   if(dit == fDirMap.end())
   {
      ++fNMiss;
      return 0;
   }

   const InfoMap &imap = dit -> second;

   InfoIter hit = imap.find(key);
   if(hit == imap.end())
   {
      ++fNMiss;
      return 0;
   }

   return HistMan::CreateTH2(hit -> second, opt);
}

//-----------------------------------------------------------------------------------------      
TH2* Anp::HistMan::GetTH2(const int xkey, const int ykey, const string &dir)
{
   return HistMan::GetTH2(Convert(xkey), HistMan::Convert(ykey), dir);
}

//-----------------------------------------------------------------------------------------      
TH2* Anp::HistMan::GetTH2(const string &xkey, const string &ykey, const string &dir)
{
   DirIter dit = fDirMap.find(dir);
   if(dit == fDirMap.end())
   {
      ++fNMiss;
      return 0;
   }

   const InfoMap &imap = dit -> second;

   InfoIter xhit = imap.find(xkey);
   if(xhit == imap.end())
   {
      ++fNMiss;
      return 0;
   }

   InfoIter yhit = imap.find(ykey);
   if(yhit == imap.end())
   {
      ++fNMiss;
      return 0;
   }

   const HistInfo &xinfo = xhit -> second;
   const HistInfo &yinfo = yhit -> second;

   HistInfo info;

   info.fKey = "keys_" + xinfo.GetKey() + "_and_" + yinfo.GetKey();
   info.fXaxis = xinfo.GetXaxis();
   info.fYaxis = yinfo.GetXaxis();
   info.fName  = info.GetKey();
   info.fTitle = info.GetKey();

   if(xinfo.GetYaxis().Valid())
   {
      info.fXaxis = xinfo.GetYaxis();
   }
   if(yinfo.GetYaxis().Valid())
   {
      info.fYaxis = yinfo.GetYaxis();
   }

   return HistMan::CreateTH2(info);
}

//-----------------------------------------------------------------------------------------      
const Anp::Hist1d<double> Anp::HistMan::CreateHist1d(const HistInfo &info)
{
   const AxisInfo &xaxis = info.GetXaxis();

   if(!xaxis.GetBins().empty())
   {
      return Hist1d<double>(xaxis.GetBins());
   }

   return Hist1d<double>(xaxis.GetNbins(), xaxis.GetMin(), xaxis.GetMax());      
}

//-----------------------------------------------------------------------------------------      
TH1* Anp::HistMan::CreateTH1(const HistInfo &info, const string &opt)
{
   string strkey = info.GetKey();
   string name   = info.GetName();
   string title  = info.GetTitle();

   const int intkey = std::atoi(strkey.c_str());   
   if(intkey != 0)
   {
      strkey = "key_" + Convert(intkey, fKeyWidth);
   }
   if(name.empty())
   {
      name = strkey;
   }
   if(title.empty())
   {
      title = info.GetName();
   }

   const AxisInfo &xaxis = info.GetXaxis();
   const AxisInfo &yaxis = info.GetYaxis();

   TH1 *h = 0;
   if(xaxis.GetBins().empty())
   {
      if(opt.find("profile") != string::npos)
      {
	 h = new TProfile(name.c_str(), title.c_str(), 
			  xaxis.GetNbins(), xaxis.GetMin(), xaxis.GetMax());  
      }
      else if(opt.find("double") != string::npos)
      {
	 h = new TH1D(name.c_str(), title.c_str(), 
		      xaxis.GetNbins(), xaxis.GetMin(), xaxis.GetMax());      
      }
      else
      {
	 h = new TH1F(name.c_str(), title.c_str(),
		      xaxis.GetNbins(), xaxis.GetMin(), xaxis.GetMax());      
      }
   }
   else
   {
      const vector<double> &bvec = xaxis.GetBins();

      double *darray = new double[bvec.size()];
      
      for(unsigned int ibin = 0; ibin < bvec.size(); ++ibin)
      {
	 darray[ibin] = bvec[ibin];
      }
      
      if(opt.find("profile") != string::npos)
      {
	 h = new TProfile(name.c_str(), title.c_str(), bvec.size() - 1, darray);
      }
      else if(opt.find("double") != string::npos)
      {
	 h = new TH1D(name.c_str(), title.c_str(),  bvec.size() - 1, darray);
      }
      else
      {
	 h = new TH1F(name.c_str(), title.c_str(), bvec.size() - 1, darray);
      }
      
      delete [] darray;      
   }
   
   //
   // Set NULL TDirectory
   //
   Anp::SetDir(h, 0);

   if(xaxis.GetTitle().size() > 0)
   {
      h -> GetXaxis() -> SetTitle(xaxis.GetTitle().c_str());
      h -> GetXaxis() -> CenterTitle();
   }
   if(yaxis.GetTitle().size() > 0)
   {
      h -> GetYaxis() -> SetTitle(yaxis.GetTitle().c_str());
      h -> GetYaxis() -> CenterTitle();
   }

   return h;
}

//-----------------------------------------------------------------------------------------      
TH2* Anp::HistMan::CreateTH2(const HistInfo &info, const string &opt)
{
   string strkey = info.GetKey();
   string name   = info.GetName();
   string title  = info.GetTitle();

   const int intkey = std::atoi(strkey.c_str());   
   if(intkey != 0)
   {
      strkey = Convert(intkey, fKeyWidth);
   }
   if(name.empty())
   {
      name = strkey;
   }
   if(title.empty())
   {
      title = info.GetName();
   }

   const AxisInfo &xaxis = info.GetXaxis();
   const AxisInfo &yaxis = info.GetYaxis();

   TH2 *h = 0;
   if(!name.empty())
   {
      if(opt.find("double") != string::npos)
	{
	  h = new TH2D(name.c_str(), title.c_str(),
		   xaxis.GetNbins(), xaxis.GetMin(), xaxis.GetMax(),
		   yaxis.GetNbins(), yaxis.GetMin(), yaxis.GetMax());   
	  
	}
      else
	{
	  h = new TH2F(name.c_str(), title.c_str(),
		   xaxis.GetNbins(), xaxis.GetMin(), xaxis.GetMax(),
		   yaxis.GetNbins(), yaxis.GetMin(), yaxis.GetMax());      
	}

      if(xaxis.GetTitle().size() > 0)
      {
	 h -> GetXaxis() -> SetTitle(xaxis.GetTitle().c_str());
	 h -> GetXaxis() -> CenterTitle();
      }
      if(yaxis.GetTitle().size() > 0)
      {
	 h -> GetYaxis() -> SetTitle(yaxis.GetTitle().c_str());
	 h -> GetYaxis() -> CenterTitle();
      }
   }

   //
   // Set NULL TDirectory
   //
   Anp::SetDir(h, 0);

   return h;
}

//-----------------------------------------------------------------------------------------      
const string Anp::HistMan::Convert(const int key, const int width) const
{
   stringstream strkey;
   if(width == 0)
   {
      strkey << setw(ComputeWidth(key)) << key;
   }
   else
   {
      strkey << setfill('0') << setw(width) << key;
   }

   return strkey.str();
}

//-----------------------------------------------------------------------------------------      
unsigned short Anp::HistMan::ComputeWidth(int key) const
{
   //
   // Compute key width
   //
   return static_cast<unsigned int>(std::floor(std::log10(double(std::abs(key) + 1)))) + 1;
}

//-----------------------------------------------------------------------------------------      
unsigned int Anp::HistMan::NMiss() const
{
   return fNMiss;
}

//-----------------------------------------------------------------------------------------      
void Anp::HistMan::Print(ostream& o) const
{
   o << "HistMan::Print" << endl;
   for(DirIter dit = fDirMap.begin(); dit != fDirMap.end(); ++dit)
   {
      o << "Directory = " << dit -> first << endl; 
      const InfoMap &imap = dit -> second;

      for(InfoIter it = imap.begin(); it != imap.end(); ++it)
      {
	 it -> second.Print(o);
      }
   }
}

//-----------------------------------------------------------------------------------------      
Anp::HistInfo::HistInfo()
   :fKey(),
    fName(),
    fTitle(),
    fXaxis(),
    fYaxis()
{
}

//-----------------------------------------------------------------------------------------      
Anp::HistInfo::~HistInfo()
{
}

//-----------------------------------------------------------------------------------------      
const std::string& Anp::HistInfo::GetName() const 
{
   return fName;
}

//-----------------------------------------------------------------------------------------      
const std::string& Anp::HistInfo::GetTitle() const
{
   return fTitle;
}

//-----------------------------------------------------------------------------------------      
const Anp::AxisInfo& Anp::HistInfo::GetXaxis() const
{
   return fXaxis;
}

//-----------------------------------------------------------------------------------------      
const Anp::AxisInfo& Anp::HistInfo::GetYaxis() const
{
   return fYaxis;
}

//-----------------------------------------------------------------------------------------      
const std::string& Anp::HistInfo::GetKey() const
{
   return fKey;
}

//-----------------------------------------------------------------------------------------      
bool Anp::HistInfo::operator<(const HistInfo &rhs) const
{
   if(fKey < rhs.fKey)
   {
      return true;
   }

   return false;
}

//-----------------------------------------------------------------------------------------
bool Anp::HistInfo::operator==(const HistInfo &rhs) const
{
   if(fKey == rhs.fKey)
   {
      return true;
   }

   return false;
}

//----------------------------------------------------------------------
void Anp::HistInfo::Print(ostream& o) const
{
   o << "Printing HistInfo: key = " << GetKey() << std::endl
     << "   name = " << GetName()
     << ", title = " << GetTitle()
     << "  " << fXaxis
     << "  " << fYaxis;
}

//----------------------------------------------------------------------
std::ostream& Anp::operator<<(ostream& o, const Anp::HistInfo &self)
{
   self.Print(o);
   return o;
}

//-----------------------------------------------------------------------------------------      
Anp::AxisInfo::AxisInfo()
   :fNbins(0),
    fMin(0),
    fMax(0),
    fValid(false),
    fTitle()
{
}

//-----------------------------------------------------------------------------------------      
Anp::AxisInfo::~AxisInfo()
{
}

//-----------------------------------------------------------------------------------------      
const std::string& Anp::AxisInfo::GetTitle() const
{
   return fTitle;
}

//-----------------------------------------------------------------------------------------      
double Anp::AxisInfo::GetMin() const
{
   return fMin;
}

//-----------------------------------------------------------------------------------------      
double Anp::AxisInfo::GetMax() const
{
   return fMax;
}

//-----------------------------------------------------------------------------------------      
int Anp::AxisInfo::GetNbins() const
{
   return fNbins;
}

//-----------------------------------------------------------------------------------------      
const std::vector<double>& Anp::AxisInfo::GetBins() const
{
   return fBins;
}

//----------------------------------------------------------------------
bool Anp::AxisInfo::Valid() const
{
   return fValid;
}

//----------------------------------------------------------------------
void Anp::AxisInfo::Print(std::ostream& o) const
{
   o << "AxisInfo (" << fTitle << "): (nbin, min, max) = "
     << "(" << GetNbins() << ", " << GetMin() << ", " << GetMax() << ")" << std::endl;
}

//----------------------------------------------------------------------
ostream& Anp::operator<<(std::ostream& o, const Anp::AxisInfo &self)
{
   self.Print(o);
   return o;
}

//----------------------------------------------------------------------
TH1* Anp::MakeTH1(const string &dir, TDirectory *dir_, const string &key)
{
   TH1 *h = Anp::HistMan::Instance().CreateTH1(key, dir);
   if(h)
   {
      Anp::SetDir(h, dir_);
   }
   
   return h;
}

//----------------------------------------------------------------------
TH2* Anp::MakeTH2(const string &dir, TDirectory *dir_, const string &key)
{
   TH2 *h = Anp::HistMan::Instance().CreateTH2(key, dir);
   if(h)
   {
      Anp::SetDir(h, dir_);
   }
   
   return h;
}

//----------------------------------------------------------------------------
void Anp::HistMan::SetROOTDefaults()
{
  gROOT->SetStyle("Plain");
  
  // set axis ticks on top and right
  gStyle -> SetPadTickX(1);         
  gStyle -> SetPadTickY(1);         
  
  // Set the background color to white
  gStyle->SetFillColor(10);           
  gStyle->SetFrameFillColor(10);      
  gStyle->SetCanvasColor(10);         
  gStyle->SetPadColor(10);            
  gStyle->SetTitleFillColor(0);       
  gStyle->SetStatColor(10);           
  
  
  // Turn off all borders
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0); 
  gStyle->SetPadBorderMode(0);   
  gStyle->SetDrawBorder(0);      
  gStyle->SetTitleBorderSize(0); 
  
  // Set the size of the default canvas
  gStyle->SetCanvasDefH(400);          
  gStyle->SetCanvasDefW(650);          
  //gStyle->SetCanvasDefX(10);         
  //gStyle->SetCanvasDefY(10);         
  
  
  // Set fonts
  gStyle->SetLabelFont(132,"xyz");
  gStyle->SetStatFont(132);       
  gStyle->SetTitleFont(132);      
  gStyle->SetTitleFont(132,"xyz");
  gStyle->SetTextFont(132);       
  gStyle->SetTitleX(0.3);        
  gStyle->SetTitleW(0.4);        
  
  // Set Line Widths
  //gStyle->SetFrameLineWidth(0);
  gStyle->SetFuncWidth(2);
  gStyle->SetHistLineWidth(2);
  gStyle->SetFuncColor(2);
  
  // Set margins
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadRightMargin(0.12);
  
  // Set tick marks and turn off grids
  gStyle->SetNdivisions(505,"xyz");
  
  // Set Data/Stat/... and other options
  gStyle->SetOptDate(0);
  gStyle->SetDateX(0.1);
  gStyle->SetDateY(0.1);
  //gStyle->SetOptFile(0);
  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(111);
  gStyle->SetStatFormat("6.3f");
  gStyle->SetFitFormat("6.3f");
  //gStyle->SetStatTextColor(1);
  //gStyle->SetStatColor(1);
  //gStyle->SetOptFit(1);
  //gStyle->SetStatH(0.20);
  //gStyle->SetStatStyle(0);
  //gStyle->SetStatW(0.30);
  //gStyle -SetStatLineColor(0);
  gStyle->SetStatX(0.919);
  gStyle->SetStatY(0.919);
  gStyle->SetOptTitle(0);
  //gStyle->SetStatStyle(0000);   // transparent mode of Stats PaveLabel
  gStyle->SetStatBorderSize(0);
  
  // Adjust size and placement of axis labels
  gStyle -> SetLabelSize(0.065,"xyz");
  //gStyle -> SetLabelOffset(0.005,"xyz");
  //gStyle -> SetTitleY(.98);
  gStyle -> SetTitleOffset(1.0,"xz");
  gStyle -> SetTitleOffset(1.15,"y");
  gStyle -> SetTitleSize(0.065, "xyz");
  gStyle -> SetTextAlign(22);
  gStyle -> SetTextSize(0.12);
  
  // Set paper size for life in the US
  gStyle -> SetPaperSize(TStyle::kA4);
  
  // Set color pallete
  gStyle -> SetPalette(1,0);
  
  //gStyle -> SetOptFit(0);
  gStyle -> SetOptStat("reimo");
  //gStyle -> SetStatStyle(0);

  gROOT -> ForceStyle();
}
