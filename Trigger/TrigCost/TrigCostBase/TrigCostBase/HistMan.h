/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_HISTMAN_H
#define ANP_HISTMAN_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : HistMan
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 *  HistMan manages creation of ROOT histograms. Histogram definitions are
 *  read from XML files.
 *  
 * 
 **********************************************************************************/

// C++
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Local
#include "TrigCostBase/Hist1d.h"

class TDirectory;
class TH1;
class TH2;

namespace Anp
{   
   class AxisInfo
   {
   public:
      
      AxisInfo();
      ~AxisInfo();

      const std::string& GetTitle() const;
      
      double GetMin() const;
      double GetMax() const;
      int GetNbins() const;

      bool Valid() const;

      const std::vector<double>& GetBins() const;

      void Print(std::ostream& o = std::cout) const;

   protected:

      friend class HistMan;
      
      int fNbins;
      double fMin;
      double fMax;
      
      bool fValid;

      std::vector<double> fBins;

      std::string fTitle;
   };

   class HistInfo
   {
   public:
      
      HistInfo();
      ~HistInfo();
      
      const std::string& GetName() const;
      const std::string& GetTitle() const;
      
      const AxisInfo& GetXaxis() const;
      const AxisInfo& GetYaxis() const;

      const std::string& GetKey() const;

      bool operator <(const HistInfo &rhs) const;
      bool operator==(const HistInfo &rhs) const;

      void Print(std::ostream& o = std::cout) const;

      friend class HistMan;

   protected:
      
      std::string fKey;

      std::string fName;
      std::string fTitle;

      AxisInfo fXaxis;
      AxisInfo fYaxis;
   };

   class HistMan
   {
   public:
      
      typedef std::map<std::string, HistInfo> InfoMap;
      typedef InfoMap::const_iterator InfoIter;
      typedef std::map<std::string, InfoMap> DirMap;
      typedef DirMap::const_iterator DirIter;

   public:

      static HistMan& Instance();
      
      void Config(const Registry &reg);

      void SetROOTDefaults();

      bool ReadFile(const std::string file);

      bool KeyExists(int key, const std::string &dir = ".") const;
      bool KeyExists(const std::string &key, const std::string &dir = ".") const;

      unsigned int NMiss() const;

      const Hist1d<double> CreateHist1d(int key, const std::string &dir = ".");
      const Hist1d<double> CreateHist1d(const std::string &key, const std::string &dir = ".");

      TH1* CreateTH1(int key, const std::string &dir = ".");
      TH2* CreateTH2(int key, const std::string &dir = ".");

      TH1* CreateTH1(const std::string &key, const std::string &dir = ".");
      TH2* CreateTH2(const std::string &key, const std::string &dir = ".");

      TH2* GetTH2(int xkey, int ykey, const std::string &dir = ".");
      TH2* GetTH2(const std::string &xkey, const std::string &ykey, const std::string &dir = ".");

      void Print(std::ostream& o = std::cout) const;

   private:     

      HistMan();
      ~HistMan();

      HistMan(const HistMan &);
      const HistMan& operator=(const HistMan &);
      
   private:
      
      const Hist1d<double> CreateHist1d(const HistInfo &info);

      TH1* CreateTH1(const HistInfo &info, const std::string &opt = "double");
      TH2* CreateTH2(const HistInfo &info, const std::string &opt = "float"); 

      bool ReadAxis(const Registry &reg, AxisInfo &info) const;

      bool AddHistogramBlock(InfoMap &hmap, const std::vector<int> &basekeys, 
			     std::vector<std::string> &dirvec);

      const std::string Convert(int key, int width = 0) const;
      unsigned short ComputeWidth(int key) const;

   private:
      
      unsigned fNMiss;
      unsigned fKeyWidth;
      bool     fDebug;
      DirMap   fDirMap;
   }; 

   TH1* MakeTH1(const std::string &dir, TDirectory *dir_, const std::string &key);
   TH2* MakeTH2(const std::string &dir, TDirectory *dir_, const std::string &key);

   std::ostream& operator<<(std::ostream& o, const HistInfo &self);
   std::ostream& operator<<(std::ostream& o, const AxisInfo &self);
}

#endif
