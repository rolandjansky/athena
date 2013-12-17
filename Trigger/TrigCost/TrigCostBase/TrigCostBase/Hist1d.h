/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_HIST1D_H
#define ANP_HIST1D_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : Hist1d
 * @Author : Rustem Ospanov
 *
 * @Brief  : Template for 1d histogram class
 * 
 **********************************************************************************/

//
// Bin<> and Hist<> templates that implement histogram template
//
// Bin numbering conventions is as in TH1: 
//    bin = 0 is overflow bin
//    bin = nbin + 1 is overflow bin
//

// C++
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// ROOT
#include "TH1.h"

// Local
#include "UtilCore.h"

namespace Anp
{
  template<typename T>
      class Bin
   {
   public:
      
      Bin();
      Bin(unsigned int bin, const T edge);
      Bin(const Bin &lhs, const Bin &rhs);
      ~Bin();
      
      void add();
      void add(const T &weight);
      
      void set_bin(unsigned int bin);
      void set_sum(const T &sum, const T &sum2);
      void set_ent(unsigned int entries);
      void reset();

      unsigned int bin() const;
      unsigned int entries() const;

      T edge() const;
      T sum()  const;
      T sum2() const;

      void print(std::ostream& o = std::cout, short width = 0, short precision = 0) const;

   private:
      
      unsigned int fBin;
      unsigned int fNEntry;

      T fEdge;
      T fSum;
      T fSum2;
   };

   // 
   // Bin global operators and functions
   // 
   template <typename T> bool operator<(const Bin<T> &lhs, const Bin<T> &rhs);

   template <typename T> bool operator<(const Bin<T> &lhs, const T &rhs);
   template <typename T> bool operator<(const T &lhs, const Bin<T> &rhs);

   template <typename T> bool operator<(const Bin<T> &lhs, unsigned int bin);
   template <typename T> bool operator<(unsigned int bin, const Bin<T> &rhs);

   template <typename T> bool operator==(const Bin<T> &lhs, unsigned int bin);
   template <typename T> bool operator==(unsigned int bin, const Bin<T> &rhs);   

   template <typename T> Bin<T> Merge(const Bin<T> &lhs, const Bin<T> &rhs);


   template <typename T>
      class Hist1d
   {      
   public:
      
#ifndef __CINT__
      typedef typename std::vector<Anp::Bin<T> >::const_iterator BinIter;
      typedef typename std::vector<Anp::Bin<T> >::iterator BinIterator;

      typedef typename std::vector<T>::const_iterator Iter;
      typedef typename std::vector<T>::iterator Iterator;
#endif

   public:

      Hist1d();
      Hist1d(unsigned int nbin, const T low, const T high);
      explicit Hist1d(const std::vector<T> &vec);
      explicit Hist1d(const TH1 &h);
      ~Hist1d();
      
      unsigned int Fill(const T value);
      unsigned int Fill(const T value, const T weight);
      unsigned int Fill(const T value, const T weight, const T width);
      unsigned int Fill(const TH1 &h);

      unsigned int Merge(const T minvalue);
      unsigned int Rebin(unsigned int ngroup);

      Bin<T>& operator[](unsigned int bin);
      Bin<T>& GetBin(const T value);

      const Bin<T>& operator[](unsigned int bin) const;
      const Bin<T>& GetBin(const T value) const;

      T GetIntegral(const std::string &option = "") const;
      unsigned int GetEntries(const std::string &option = "") const;
      unsigned int GetNbins() const { return fNBin; }

      const std::vector<Anp::Bin<T> >& GetBins() const { return fData; }

      void Reset();
      void Print(std::ostream& o = std::cout) const;
      
   private:

      const Bin<T>& get_bin_const(const T value) const;
      Bin<T>& get_bin(const T value);

      void init_bins();
      void init_bins(std::vector<T> vec);
      void init_trivial();

   private:

      std::vector<Anp::Bin<T> > fData;

      unsigned int fNBin;

      T fLowEdge;
      T fHighEdge;

      bool fSearchBin;
   };

   template<typename T> TH1* CreateTH1(const Hist1d<T> &h, const std::string &name = "h");

   //--------------------------------------------------------------------------------------
   // inlined member functions for Bin<T>
   //--------------------------------------------------------------------------------------

   template<typename T> 
      Bin<T>::Bin()
	 : fBin(), fNEntry(0), fEdge(), fSum(0), fSum2(0) {}


   template<typename T> 
      Bin<T>::Bin(const unsigned int bin, const T edge)
	 : fBin(bin), fNEntry(0), fEdge(edge), fSum(0), fSum2(0) {}

   template <typename T> 
      Bin<T>::Bin(const Bin &lhs, const Bin &rhs) :
      fBin(lhs.bin()),
      fNEntry(lhs.entries() + rhs.entries()),
      fEdge(lhs.edge()),
      fSum(lhs.sum() + rhs.sum()),
      fSum2(lhs.sum2() + rhs.sum2())
   {
      if(rhs.bin() < lhs.bin())
      {
	 fBin = rhs.bin();
      }
      if(rhs.edge() < lhs.edge())
      {
	 fEdge = rhs.edge();
      }      
   }

   template<typename T> 
      Bin<T>::~Bin() {}

   template<typename T> 
      inline unsigned int Bin<T>::bin() const { return fBin; }

   template<typename T> 
      inline unsigned int Bin<T>::entries() const { return fNEntry; }

   template<typename T>    
      inline T Bin<T>::edge() const { return fEdge; }

   template<typename T> 
      inline T Bin<T>::sum()  const { return fSum; }

   template<typename T> 
      inline T Bin<T>::sum2() const { return fSum2; }

   template<typename T> 
      inline void Bin<T>::add()
   {
      ++fNEntry;
      ++fSum;
      ++fSum2;
   }

   template<typename T> 
      inline void Bin<T>::add(const T &weight)
   {
      ++fNEntry;
      fSum  += weight;
      fSum2 += weight*weight;
   }

   template<typename T> 
      inline void Bin<T>::set_bin(const unsigned int bin)
   {
      fBin = bin;
   }

   template<typename T> 
      inline void Bin<T>::set_sum(const T &sum, const T &sum2)
   {
      fSum  = sum;
      fSum2 = sum2;
   }

   template<typename T> 
      inline void Bin<T>::set_ent(const unsigned int entries)
   {
      fNEntry = entries;
   }

   template<typename T> 
      inline void Bin<T>::reset()
   {
      fNEntry = 0;
      fSum    = 0;
      fSum2   = 0;
   }

   template<typename T> 
      inline void Bin<T>::print(std::ostream& o, const short width, const short precision) const
   {
      if(width == 0 || precision == 0)
      {
	 o << "Bin: (bin, edge, sum) = (" 
	   << bin() << ", " << edge()  << ", "<< sum() << ")" << std::endl;
      }
      else
      {
	 o << "Bin: (bin, edge, sum) = " 
	   << std::setw(width) << std::setprecision(precision) << bin()
	   << ", " 
	   << std::setw(width) << std::setprecision(precision) << edge() 
	   << ", " 
	   << std::setw(width) << std::setprecision(precision) << sum() 
	   << ")" <<std::endl;
      }
   }

   //--------------------------------------------------------------------------------------
   // non member Bin<T> functions
   //--------------------------------------------------------------------------------------
   template<typename T>
      inline bool operator<(const Bin<T> &lhs, const Bin<T> &rhs) { return lhs.bin() < rhs.bin(); }
   
   template<typename T>
      inline bool operator<(const Bin<T> &lhs, const T &rhs) { return lhs.edge() < rhs; }
   
   template<typename T> 
      inline bool operator<(const T &lhs, const Bin<T> &rhs) { return lhs < rhs.edge(); }  

   template<typename T>
      inline bool operator<(const Bin<T> &lhs, const unsigned int rhs) { return lhs.bin() < rhs; }
   
   template<typename T> 
      inline bool operator<(const unsigned int lhs, const Bin<T> &rhs) { return lhs < rhs.bin(); } 

   template<typename T>
      inline bool operator==(const Bin<T> &lhs, const unsigned int rhs) { return lhs.bin() == rhs; }
   
   template<typename T> 
      inline bool operator==(const unsigned int lhs, const Bin<T> &rhs) { return lhs == rhs.bin(); } 
   
   template <typename T>
      inline std::ostream& operator<<(std::ostream& o, const Bin<T> &self)
   {
      self.print(o);
      return o;
   }

   template <typename T>
      inline T GetKernelOverlap(const T lpos, const T rpos, const T cpos, const T width)
   {
      // Compute fraction of kernel intergral for kernel centered at x0 = cpos 
      // and between lpos and rpos boundaries. Kernel function is (1-((x-x0)/w)^2)^2
      // for x < 1.0 and 0 otherwise. Kerner integral I(x) = (x - 2*x^3/3 + x^5/5)/C,
      // where C = 16/15.
      
      if(!(lpos < rpos) || !(width > 0.0))
      {
	 std::cerr << "GetKernelOverlap - invalid input parameters (" 
		   << lpos << ", " << rpos << ", " << cpos << ", " << width << ")" << std::endl;
	 return 0;
      }

      T ledge = cpos - width;
      T redge = cpos + width;

      if(ledge < lpos) ledge = lpos;
      if(redge > rpos) redge = rpos;
      
      const T x1 = (ledge - cpos)/width;
      const T x2 = (redge - cpos)/width;

      const T I1 = x1 - 2.0*x1*x1*x1/3.0 + x1*x1*x1*x1*x1/5.0;
      const T I2 = x2 - 2.0*x2*x2*x2/3.0 + x2*x2*x2*x2*x2/5.0;

      const T overlap = 15.0*(I2 - I1)/16.0;

      //std::cout << "(lpos, rpos, cpos) = (" 
      //<< lpos << ", " << rpos << ", " << cpos << ")" << std::endl
      //<< "(ledg, redg, width, overlap) = (" 
      //<< ledge << ", " << redge << ", " << width << ", " << overlap << ")" << std::endl;

      return overlap;
   }

   template <typename T> 
      inline Bin<T> Merge(const Bin<T> &lhs, const Bin<T> &rhs)
   {
      Bin<T> bin(std::min<unsigned int>(lhs.bin(), rhs.bin()),
		 std::min<T>(lhs.edge(), rhs.edge()));

      bin.set_sum(lhs.sum() + rhs.sum(), lhs.sum2() + rhs.sum2());
      bin.set_ent(lhs.entries() + rhs.entries());

      return bin;
   }

   //--------------------------------------------------------------------------------------
   // Hist1d<T> member functions
   //--------------------------------------------------------------------------------------
   template <typename T>
      Hist1d<T>::Hist1d()
     : fData(), fNBin(0), fLowEdge(), fHighEdge(), fSearchBin(false)
   {    
      init_trivial();
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      Hist1d<T>::Hist1d(const unsigned int nbin, const T low, const T high)
     : fData(), fNBin(nbin), fLowEdge(low), fHighEdge(high), fSearchBin(false)
   {
      if(fNBin > 0)
      {
	 init_bins();
      }
      else
      {
	 init_trivial();
      }
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      Hist1d<T>::Hist1d(const std::vector<T> &vec)
     : fData(), fNBin(0), fLowEdge(), fHighEdge(), fSearchBin(true)
   {
      if(vec.size() > 1)
      {
	 init_bins(vec);
      }
      else
      {
	 init_trivial();
      }
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
     Hist1d<T>::Hist1d(const TH1 &h)
     :fData(),
      fNBin(h.GetNbinsX()),
      fLowEdge(h.GetXaxis() -> GetXmin()),
      fHighEdge(h.GetXaxis() -> GetXmax()),
      fSearchBin(true)
   {
      const int nbin = h.GetNbinsX();
      
      if(nbin < 2)
      { 
	 std::cerr << "Hist1d<T>::Hist1d encountered TH1 without bins" << std::endl;
	 return;
      }

      for(int ibin = 1; ibin <= nbin + 1; ++ibin)
      {
	 const double value = h.GetBinContent(ibin);
	 const double edge  = h.GetBinLowEdge(ibin);
	 
	 if(ibin == 1)
	 {
	    fData.push_back(Bin<T>(0, edge));
	 }

	 Bin<T> bin(ibin, edge);
	 bin.add(value);

	 fData.push_back(bin);
      }      

      std::sort(fData.begin(), fData.end());
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      Hist1d<T>::~Hist1d() {}

   //-----------------------------------------------------------------------------------------------
   template <typename T> unsigned int Hist1d<T>::Fill(const T value)
   {
      Bin<T> &bin = get_bin(value);
      bin.add();
      return bin.bin();
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T> unsigned int Hist1d<T>::Fill(const T value, const T weight)
   {
      Bin<T> &bin = get_bin(value);
      bin.add(weight);
      return bin.bin();
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T> unsigned int Hist1d<T>::Fill(const T value, const T weight, const T width)
   {
       if(!(width > 0.0) || fNBin < 1 || fData.empty()) {
	 return Fill(value, weight);
       }

      // compute boundaries for non zero kernel coontributions
      const T lpos = value - width;
      const T rpos = value + width;

      // Only overlap with underflow or overflow bins
      if(rpos < fData.front())
      {
	 // rpos < underflow bin edge - no contribution to other bins
	 Bin<T> &bin = fData.front();
	 bin.add(weight);
	 return bin.bin();
      }
      else if(!(lpos < fData.back()))
      {
	 // lpos >= last overflow edge - no contribution to other bins
	 Bin<T> &bin = fData.back();
	 bin.add(weight);
	 return bin.bin();
      }

      BinIterator lit = std::lower_bound(fData.begin(), fData.end(), lpos);
      if(lit == fData.end())
      {
	 std::cerr << "Hist1d<T>::Fill - lower_bound has failed to find correct bin" << std::endl;
	 return 0;
      }       

      // make copy of beg and end iterators
      BinIterator ibeg = fData.begin();
      BinIterator iend = fData.end();

      //
      // special case: compute overlap with underflow bin
      //
      if(lit == ibeg)
      {
	 if(lpos < ibeg -> edge())
	 {
	    ibeg -> add(weight*GetKernelOverlap<T>(lpos, ibeg -> edge(), value, width));
	 }

	 lit++;
      }
      else
      {
	 lit--;
	 assert(lit != ibeg && "logic error finding underflow bin");
      }

      for(; lit != iend; ++lit)
      {	 
	 //
	 // No more non overlapping bins
	 //
	 if(!(lit -> edge() < rpos))
	 {
	    break;
	 }

	 //
	 // Overlap with this bin
	 //
	 T overlap = 0;
	 
	 const BinIterator nit = lit + 1;
	 if(nit == iend)
	 {
	    overlap = Anp::GetKernelOverlap<T>(lit -> edge(), rpos, value, width);
	 }
	 else
	 {
	    //
	    // kernel is entirely contain in a single bin
	    //
	    if(lit -> edge() < lpos && rpos < nit -> edge())
	    {
	       lit -> add(weight);
	       break;
	    }
	    
	    overlap = Anp::GetKernelOverlap<T>(std::max<T>(lit -> edge(), lpos),
					       std::min<T>(nit -> edge(), rpos),
					       value, width);
	 }

	 lit -> add(weight*overlap); // compute overlap fraction with this bin
      }
      
      return 0;
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T> unsigned int Hist1d<T>::Fill(const TH1 &h)
   {
     //
     // Copy content from h, using bin center and bin content as single fill entry
     //
     const int nbin = h.GetNbinsX();
     
     assert(nbin > 1 && "Hist1d<T>::Fill encountered TH1 without bins");
     
     for(int ibin = 0; ibin <= nbin + 1; ++ibin) {
       const double value  = h.GetBinContent(ibin);
       const double center = h.GetBinCenter(ibin);
       
       if(ibin == 0) {
	 fData.front().add(value);
       }
       else if(ibin == nbin + 1) {
	 fData.back().add(value);
       }
       else {
	 Fill(center, value);
       }
     }
     
     return 0;
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T> unsigned int Hist1d<T>::Merge(const T minvalue) {
     //
     // Merge histogram bins which have content less than minvalue
     //
     unsigned int nmerge = 0;
     
     typename std::vector<Bin<T> >::iterator bit = fData.begin();
     while(bit != fData.end()) {
       if(bit -> sum() > minvalue) {
	 ++bit;
	 continue;
       }

       if(bit == fData.begin() || bit - 1 == fData.begin()) {
	 ++bit;
	 continue;
       }

       if(bit + 1 == fData.end() || bit + 2 == fData.end()) {
	 ++bit;
	 continue;
       }
	 
       typename std::vector<Bin<T> >::iterator lit = bit - 1;
       typename std::vector<Bin<T> >::iterator hit = bit + 1;
       
       if(lit -> sum() < hit -> sum()) {
	 const Bin<T> newbin(*lit, *bit);
	 
	 typename std::vector<Bin<T> >::iterator pit = fData.erase(lit, bit + 1);
	 
	 fData.insert(pit, newbin);
       }
       else {
	 const Bin<T> newbin(*bit, *hit);
	 
	 typename std::vector<Bin<T> >::iterator pit = fData.erase(bit, hit + 1);
	 
	 fData.insert(pit, newbin);
       }
       
       bit = fData.begin();       
       ++nmerge;
     }
     
     BinIterator pit = fData.begin();
     for(BinIterator cit = fData.begin(); cit != fData.end(); ++cit) {	 
       if(cit == fData.begin() || cit - 1 == fData.begin()) {
	 pit = cit;
	 continue;
       }
       
       if(!(pit -> edge() < cit -> edge()) || !(pit -> bin() < cit -> bin())) {
	 std::cerr << "Hist1d<T>::Merge - logic error for bin " << cit -> bin() << std::endl;
       }
       
       pit = cit;       
       cit -> set_bin(std::distance(fData.begin(), cit));	 
     }
     
     fNBin = fData.size() - 2;
     return nmerge;
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T> unsigned int Hist1d<T>::Rebin(const unsigned int ngroup) {
     //
     // rebin histogram by adding ngroup bins together
     //
     if(ngroup < 1 || ngroup >= fNBin || fData.size() < 3) {
       return 0;
     }

      std::vector<Anp::Bin<T> > data;
      bool newbin = true;
      Bin<T> bin;

      assert(int(fNBin) == int(fData.size()) - 2 && "logic error counting bins");

      for(unsigned int ibin = 1; ibin <= fNBin; ++ibin)
      {
	 assert(ibin < fData.size() && "out of bounds access");

	 if(newbin)
	 {
	    bin = fData[ibin];
	    newbin = false;
	    continue;
	 }

	 bin = Anp::Merge<T>(bin, fData[ibin]);

	 if(ibin % ngroup == 0)
	 {
	    data.push_back(bin);
	    newbin = true;
	 }
      }

      //
      // Add last bin
      //
      if(!newbin) data.push_back(bin);

      //
      // sort bins
      //
      std::sort(data.begin(), data.end());
      
      //
      // Add overflow and underflow bins
      //
      data.insert(data.begin(), fData.front());
      data.insert(data.end(),   fData.back());
      
      for(unsigned int ibin = 0; ibin < data.size(); ibin++)
      {
	 data[ibin].set_bin(ibin);
      }
      
      fData = data;
      fNBin = data.size() - 2;
      fSearchBin = true;

      return fNBin;
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>      
      Bin<T>& Hist1d<T>::operator[](const unsigned int bin)
   {
      assert(!fData.empty() && "empty bin vector in Hist1d<T>::operator[]");

      if(bin >= fData.size())
      {
	 std::cerr << "Hist1d<T>::operator[" << bin << "] is out of range" << std::endl;
	 return fData.back();
      }
      
      return fData[bin];
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      Bin<T>& Hist1d<T>::GetBin(const T value)
   {
      return get_bin(value);
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>      
      const Bin<T>& Hist1d<T>::operator[](const unsigned int bin) const
   {
      assert(!fData.empty() && "empty bin vector in Hist1d<T>::operator[]");

      if(bin >= fData.size())
      {
	 std::cerr << "Hist1d<T>::operator[" << bin << "] is out of range" << std::endl;
	 return fData.back();
      }
      
      return fData[bin];
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      const Bin<T>& Hist1d<T>::GetBin(const T value) const
   {
      return get_bin_const(value);
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      inline unsigned int Hist1d<T>::GetEntries(const std::string &option) const
   {
      if(fData.size() < 2)
      {
	 return 0;
      }

      unsigned int entries = 0;
      
      BinIter beg = fData.begin();
      BinIter end = fData.end();
      
      if(option.find("U") == std::string::npos)
      {
	 beg++;
      }
      if(option.find("O") == std::string::npos)
      {
	 end--;
      }
      
      for(BinIter cit = beg; cit != end; ++cit)
      {	 
	 entries += cit -> entries();
      }
      
      return entries;
   }
   
   //-----------------------------------------------------------------------------------------------
   template <typename T>
      inline T Hist1d<T>::GetIntegral(const std::string &option) const
   {
      if(fData.size() < 2)
      {
	 return 0;
      }

      T integral = 0;

      BinIter beg = fData.begin();
      BinIter end = fData.end();
      
      if(option.find("U") == std::string::npos)
      {
	 beg++;
      }
      if(option.find("O") == std::string::npos)
      {
	 end--;
      }

      for(BinIter cit = beg; cit != end; ++cit)
      {	 
	 integral += cit -> sum();
      }
      
      return integral;
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      inline void Hist1d<T>::Reset()
   {
      for(BinIterator cit = fData.begin(); cit != fData.end(); ++cit)
      {	 
	 cit -> reset();
      }
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      const Bin<T>& Hist1d<T>::get_bin_const(const T value) const
   {
      assert(!fData.empty() && "Hist1d<T>::get_bin_const() - vector is empty");
	 
      if(fData.size() == 1)
      {
	 return fData.front();
      }
      else if(value < fData.front())
      {
	 // value < underflow bin edge so return underflow bin
	 return fData.front();
      }
      else if(!(value < fData.back()))
      {
	 // value >= last overflow edge so return overflow bin
	 return fData.back();
      }

      // this point is reached if value >= low edge AND value < high edge
      // use stl binary_search to find proper bin, exclude underflow bin
      
      if(fSearchBin)
      {
	 BinIter bit = std::lower_bound(fData.begin() + 1, fData.end(), value);
	 if(bit == fData.end())
	 {
	    std::cerr << "Hist1d<T>::get_bin_const() - lower_bound has failed to find bin" << std::endl;
	    return fData.back();
	 }
	 else if(!(value < *bit) && !(*bit < value))
	 {
	    return *bit;
	 }
	 
	 return *(bit - 1);
      }
      else
      {
	 const unsigned int bin = 
	    1 + static_cast<unsigned int>((fNBin*(value - fLowEdge))/(fHighEdge - fLowEdge));

	 if(bin > fNBin)
	 {
	    std::cerr << "Hist1d<T>::get_bin_const() - logic error #1" << std::endl;
	    return fData.back();
	 }

	 return fData[bin];
      }      
      
      std::cerr << "Hist1d<T>::get_bin_const() - logic error #2" << std::endl;
      return fData.front();
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      Bin<T>& Hist1d<T>::get_bin(const T value)
   {
      return const_cast<Bin<T> &>(get_bin_const(value));
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      void Hist1d<T>::init_trivial()
   {
      fNBin = 0;
      fData.push_back(Bin<T>(0, T()));
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      void Hist1d<T>::init_bins()
   {
      if(!(fLowEdge < fHighEdge))
      {
	 std::cerr << "Hist1d<T>::init_bins() - minimum is not larger than maximum" << std::endl;
	 init_trivial();
	 return;
      }

      fData.push_back(Bin<T>(0, fLowEdge));

      for(unsigned int ibin = 0; ibin < fNBin + 1; ++ibin)
      {
	 const T edge = fLowEdge + static_cast<T>(((fHighEdge - fLowEdge)*ibin)/fNBin);
	 fData.push_back(Bin<T>(ibin + 1, edge));
      }

      // sort all but first overflow bin
      std::sort(fData.begin() + 1, fData.end());
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      void Hist1d<T>::init_bins(std::vector<T> vec)
   {
      assert(vec.size() > 1 && "Hist1d<T>::init_bins() encountered small vector");

      // sort bin edges
      std::sort(vec.begin(), vec.end());
      
      fNBin = vec.size() - 1;
      fData.push_back(Bin<T>(0, vec.front()));

      for(unsigned int ibin = 0; ibin < vec.size(); ++ibin)
      {
	 fData.push_back(Bin<T>(ibin + 1, vec[ibin]));
      }

      // sort all but first overflow bin
      std::sort(fData.begin() + 1, fData.end());
   }

   //-----------------------------------------------------------------------------------------------
   template <typename T>
      void Hist1d<T>::Print(std::ostream& o) const
   {
      o << "Printing 1d histogram containing " << fNBin << " bins" << std::endl;

      short width = 1;      
      for(short i = 1; i < 10; ++i)
      {
	 if(fNBin < 10^(i))
	 {
	    width = i;
	    break;
	 }
      }
      if(width < 4)
      {
	 width = 4;
      }

      for(BinIter it = fData.begin(); it != fData.end(); ++it)
      {
	 it -> print(o, width, width);
      }
   }

   //--------------------------------------------------------------------------------------
   // non member Hist1d<T> functions below this line
   //--------------------------------------------------------------------------------------
   template<typename T>    
      TH1* CreateTH1(const Hist1d<T> &h, const std::string &name)
   {
      const std::vector<Bin<T> > &bvec = h.GetBins();

      if(bvec.empty())
      {
	 return 0;
      }

      double *array = new double[bvec.size() - 1];

      for(int ibin = 1; ibin < int(bvec.size()); ++ibin)
      {
	 array[ibin - 1] = bvec[ibin].edge();
      }

      TH1 *th1 = new TH1D(name.c_str(), name.c_str(), bvec.size() - 2, array);

      delete [] array;

      for(int ibin = 0; ibin < int(bvec.size()); ++ibin)
      {
	 th1 -> SetBinContent(ibin, bvec[ibin].sum());
      }

      Anp::SetDir(th1, 0);
      th1 -> SetEntries(h.GetIntegral("UO"));

      return th1;
   }

#ifndef __CINT__

   template <typename T>
      std::ostream& operator<<(std::ostream& o, const Hist1d<T> &self)
   {
      self.Print(o);
      return o;
   }

#endif

}
#endif
