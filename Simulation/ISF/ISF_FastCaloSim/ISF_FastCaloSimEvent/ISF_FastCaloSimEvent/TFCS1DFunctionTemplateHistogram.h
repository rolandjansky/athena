/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS1DFunctionTemplateHistogram_h
#define ISF_FASTCALOSIMEVENT_TFCS1DFunctionTemplateHistogram_h

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include <vector>
#include "TH1.h"
#include "TBuffer.h"
#include <cstring>
#include <algorithm>
#include <cmath>


#include <iostream>

template<typename T,typename Tfloat=float> class TFCS1DFunction_Numeric {
public:
  static constexpr T MaxValue = 1;
  static constexpr Tfloat MaxValueFloat = MaxValue;

  static T MaxCeilOnlyForInt(const Tfloat value) {return value;};
  static T ExpandToMaxRange(const Tfloat value) {return value;};
  static Tfloat ToNormalizedRange(const T value) {return value;};
};  

template<typename Tfloat> class TFCS1DFunction_Numeric<uint8_t,Tfloat> {
public:
  static constexpr uint8_t MaxValue = UINT8_MAX;
  static constexpr Tfloat MaxValueFloat = MaxValue;

  static uint8_t MaxCeilOnlyForInt(const Tfloat value) {return std::ceil(MaxValueFloat*value);};
  static uint8_t ExpandToMaxRange(const Tfloat value) {return value*(1+MaxValueFloat);};
  static Tfloat ToNormalizedRange(const uint8_t value) {return value/(1+MaxValueFloat);};
};  

template<typename Tfloat> class TFCS1DFunction_Numeric<uint16_t,Tfloat> {
public:
  static constexpr uint16_t MaxValue = UINT16_MAX;
  static constexpr Tfloat MaxValueFloat = MaxValue;

  static uint16_t MaxCeilOnlyForInt(const Tfloat value) {return std::ceil(MaxValueFloat*value);};
  static uint16_t ExpandToMaxRange(const Tfloat value) {return value*(1+MaxValueFloat);};
  static Tfloat ToNormalizedRange(const uint16_t value) {return value/(1+MaxValueFloat);};
};  

template<typename Tfloat> class TFCS1DFunction_Numeric<uint32_t,Tfloat> {
public:
  static constexpr uint32_t MaxValue = UINT32_MAX;
  static constexpr Tfloat MaxValueFloat = MaxValue;

  static uint32_t MaxCeilOnlyForInt(const Tfloat value) {return std::ceil(MaxValueFloat*value);};
  static uint32_t ExpandToMaxRange(const Tfloat value) {return value*(1+MaxValueFloat);};
  static Tfloat ToNormalizedRange(const uint32_t value) {return value/(1+MaxValueFloat);};
};  

template<typename T> class TFCS1DFunction_Array
{
  public:
    TFCS1DFunction_Array() {};
    TFCS1DFunction_Array(std::size_t count) {resize(count);};
    virtual ~TFCS1DFunction_Array() {
      if(m_content) delete[] m_content;
    };

    std::size_t MemorySizeArray() const {return sizeof(T[size()]);};
    std::size_t MemorySize() const {return sizeof(*this)+MemorySizeArray();};

    std::size_t size() const {return m_size;};
    
    ///resize to given count, copying old content
    void resize(std::size_t count) {
      T* new_cont=nullptr;
      if(count>0) new_cont=new T[count];
      if(m_content && new_cont) {
        std::size_t ncopy=count;
        if(size()<ncopy) ncopy=size();
        ncopy*=sizeof(T);
        std::memcpy(new_cont,m_content,ncopy);
      }
      if(m_content) delete[] m_content;
      m_size=count;
      m_content=new_cont;
    };
    
    ///Direct data pointer
    T* data() {return m_content;};
    const T* data() const {return m_content;};
    
    ///Direct access operators. Values are in the range [0,TFCS1DFunction_Numeric<T>::MaxValue]
    T&       operator[]( std::size_t pos ) {return m_content[pos];};
    const T& operator[]( std::size_t pos ) const {return m_content[pos];};
    
    ///begin() and end() iterators
    T* begin() {return m_content;};
    const T* begin() const {return m_content;};

    T* end() {return m_content+size();};
    const T* end() const {return m_content+size();};

  private:
    T* m_content{nullptr};
    std::size_t m_size{0};

  ClassDef(TFCS1DFunction_Array,1)  //TFCS1DFunction_Array
};

//Class to represent histogram content. Trandom should be a type with a floating point range [0,1]
template<typename T,typename Trandom=float> class TFCS1DFunction_HistogramContent: private TFCS1DFunction_Array<T>
{
  private:
    using TFCS1DFunction_Array<T>::size;
    using TFCS1DFunction_Array<T>::operator[];
    using TFCS1DFunction_Array<T>::resize;
    using TFCS1DFunction_Array<T>::begin;
    using TFCS1DFunction_Array<T>::end;
  
  public:
    typedef T value_type;
    typedef Trandom random_type;

    TFCS1DFunction_HistogramContent():TFCS1DFunction_Array<T>() {};
    TFCS1DFunction_HistogramContent(std::size_t nbins):TFCS1DFunction_Array<T>(nbins >= 1 ? nbins-1 : 0) {};
    ~TFCS1DFunction_HistogramContent() {};

    std::size_t MemorySizeArray() const {return TFCS1DFunction_Array<T>::MemorySizeArray();};
    std::size_t MemorySize() const {return sizeof(*this)+MemorySizeArray();};

    ///Set the content of bin pos to a given value, where value is in the range [0,1]
    void set_fraction( std::size_t pos, Trandom value ) {
      if(pos>=size()) return;
      operator[](pos)=TFCS1DFunction_Numeric<T,Trandom>::MaxCeilOnlyForInt(value);
    };
    
    ///Get the content of bin pos as fraction in the range [0,1]
    Trandom get_fraction( std::size_t pos ) {
      if(pos>=size()) return 1;
      return operator[](pos)/TFCS1DFunction_Numeric<T,Trandom>::MaxValueFloat;
    };
    
    ///set number of bins. 
    ///The actualy alocated size is one smaller than count, as the last bin is fixed with the range [get_fraction(size()-1,1]
    void set_nbins(std::size_t nbins) {resize(nbins >= 1 ? nbins-1 : 0);};

    ///return number of bins. 
    ///This is one larger than size, as the last bin is fixed with the range [get_fraction(size()-1,1]
    std::size_t get_nbins() const {return size()+1;};

    ///Get the bin for a given random value in the range [0,1)
    std::size_t get_bin(const Trandom& drnd,Trandom& residual_rnd) {
      if(size()==0) {
        residual_rnd=drnd;
        return 0;
      }  
      T rnd=TFCS1DFunction_Numeric<T,Trandom>::MaxValueFloat*drnd;
      auto it = std::upper_bound(begin(),end(),rnd);

      T basecont=0;
      if(it!=begin()) basecont=*(it-1);

      T fullcont=TFCS1DFunction_Numeric<T,Trandom>::MaxValue;
      if(it!=end()) fullcont=*it;

      T dcont=fullcont-basecont;
      if(dcont>0) {
        residual_rnd=(TFCS1DFunction_Numeric<T,Trandom>::MaxValueFloat*drnd-basecont) / dcont;
      } else {
        residual_rnd=0.5;
      }

      if(it==end()) return size();
      return std::distance(begin(),it);
    }; 

  ClassDef(TFCS1DFunction_HistogramContent,1)  //TFCS1DFunction_HistogramContent
};

//Class to represent histogram bin edges. Trandom should be a type with a floating point range [0,1]
template<typename T,typename Trandom=float> class TFCS1DFunction_HistogramBinEdges: private TFCS1DFunction_Array<T>
{
  private:
    using TFCS1DFunction_Array<T>::size;
    using TFCS1DFunction_Array<T>::operator[];
    using TFCS1DFunction_Array<T>::resize;

  public:
    typedef T value_type;
    typedef Trandom random_type;

    TFCS1DFunction_HistogramBinEdges():TFCS1DFunction_Array<T>() {};
    TFCS1DFunction_HistogramBinEdges(std::size_t nbins):TFCS1DFunction_Array<T>(nbins+1) {};
    ~TFCS1DFunction_HistogramBinEdges() {};

    std::size_t MemorySizeArray() const {return TFCS1DFunction_Array<T>::MemorySizeArray();};
    std::size_t MemorySize() const {return sizeof(*this)+MemorySizeArray();};

    ///set number of bins
    void set_nbins(std::size_t nbins) {resize(nbins+1);};

    ///return number of bins
    std::size_t get_nbins() const {return size()-1;};
    
    ///set position of lower edge of bins
    void SetBinLowEdge(std::size_t pos,const T& value) {operator[](pos)=value;};
    
    ///get position of lower edge of bins
    const T& GetBinLowEdge(std::size_t pos) const {return operator[](pos);};
    
    ///set and get minimum
    void SetMin(const T& value) {operator[](0)=value;};
    const T& GetMin() const {return operator[](0);};

    ///set and get maximum
    void SetMax(const T& value) {operator[](get_nbins())=value;};
    const T& GetMax() const {return operator[](get_nbins());};

    ///set minimum and maximum
    void SetMinMax(const T& valuemin,const T& valuemax) {SetMin(valuemin);SetMax(valuemax);};
    
    ///Get length of interval of all bins
    const T Length() const {return GetMax()-GetMin();};
    
    ///return linear interpolated position for bin pos. Interpolation is done with a random value in the range [0,1]
    T position(std::size_t pos,const Trandom& drnd) {
      return (1-drnd)*operator[](pos) + drnd*operator[](pos+1);
    };

    ///return interpolated position for bin pos, such that histograming the position gives a linear slope m,
    ///where m is in units of the bin width for bin pos. Interpolation is done with a random value in the range [0,1]
    T position(std::size_t pos,Trandom m,const Trandom& drnd) {
      if(m>2) m=2;
      if(m<-2) m=-2;
      Trandom x=fabs(m)>0.001 ? (0.5*std::sqrt(m*(m+8*drnd-4)+4)-1)/m+0.5 : drnd;
      return (1-x)*operator[](pos) + x*operator[](pos+1);
    };

  ClassDef(TFCS1DFunction_HistogramBinEdges,1)  //TFCS1DFunction_HistogramBinEdges
};

class TFCS1DFunction_HistogramFloatBinEdges: public TFCS1DFunction_HistogramBinEdges<float,float>
{
  public:
    TFCS1DFunction_HistogramFloatBinEdges():TFCS1DFunction_HistogramBinEdges<float,float>() {};
    TFCS1DFunction_HistogramFloatBinEdges(std::size_t nbins):TFCS1DFunction_HistogramBinEdges<float,float>(nbins) {};

  ClassDef(TFCS1DFunction_HistogramFloatBinEdges,1)  //TFCS1DFunction_HistogramFloatBinEdges
};

class TFCS1DFunction_HistogramDoubleBinEdges: public TFCS1DFunction_HistogramBinEdges<double,double>
{
  public:
    TFCS1DFunction_HistogramDoubleBinEdges():TFCS1DFunction_HistogramBinEdges<double,double>() {};
    TFCS1DFunction_HistogramDoubleBinEdges(std::size_t nbins):TFCS1DFunction_HistogramBinEdges<double,double>(nbins) {};

  ClassDef(TFCS1DFunction_HistogramDoubleBinEdges,1)  //TFCS1DFunction_HistogramDoubleBinEdges
};



//Class to represent histogram bin edges, where the interval between GetMin() and GetMax() 
//can be stored as a different (smaller) data type Tint
//Trandom should be a type with a floating point range [0,1]
template<typename T,typename Tint,typename Trandom=float> class TFCS1DFunction_HistogramCompactBinEdges: private TFCS1DFunction_Array<Tint>
{
  private:
    using TFCS1DFunction_Array<Tint>::size;
    using TFCS1DFunction_Array<Tint>::operator[];
    using TFCS1DFunction_Array<Tint>::resize;

  public:
    typedef T value_type;
    typedef Trandom random_type;
    typedef Tint internal_storage_type;
    
    TFCS1DFunction_HistogramCompactBinEdges():TFCS1DFunction_Array<Tint>() {};
    TFCS1DFunction_HistogramCompactBinEdges(std::size_t nbins):TFCS1DFunction_Array<Tint>(nbins >= 1 ? nbins-1 : 0) {};
    ~TFCS1DFunction_HistogramCompactBinEdges() {};

    std::size_t MemorySizeArray() const {return TFCS1DFunction_Array<Tint>::MemorySizeArray();};
    std::size_t MemorySize() const {return sizeof(*this)+MemorySizeArray();};

    ///set number of bins
    void set_nbins(std::size_t nbins) {resize(nbins >= 1 ? nbins-1 : 0);};

    ///return number of bins
    std::size_t get_nbins() const {return size()+1;};
    
    ///set position of lower edge of bins. Requires GetMin() and GetMax() to be set and may not be changed!
    void SetBinLowEdge(std::size_t pos,const T& value) {
      if(pos==0) SetMin(value);
      if(pos>=get_nbins()) SetMax(value);
      operator[](pos-1)=TFCS1DFunction_Numeric<Tint,T>::ExpandToMaxRange((value-GetMin())/Length());
    };

    ///get position of lower edge of bins. Requires GetMin() and GetMax() to be set and may not be changed!
    const T GetBinLowEdge(std::size_t pos) const {
      if(pos==0) return GetMin();
      if(pos>=get_nbins()) return GetMax();
      return GetMin()+Length()*TFCS1DFunction_Numeric<Tint,T>::ToNormalizedRange(operator[](pos-1));
    };
    
    ///set and get minimum
    void SetMin(const T& value) {m_Min=value;};
    const T& GetMin() const {return m_Min;};

    ///set and get maximum
    void SetMax(const T& value) {m_Max=value;};
    const T& GetMax() const {return m_Max;};
    
    ///set minimum and maximum
    void SetMinMax(const T& valuemin,const T& valuemax) {SetMin(valuemin);SetMax(valuemax);};
    
    ///Get length of interval of all bins
    const T Length() const {return GetMax()-GetMin();};
    
    ///return linear interpolated position for bin pos. Interpolation is done with a random value in the range [0,1]
    T position(std::size_t pos,const Trandom& drnd) {
      T pos1=GetBinLowEdge(pos);
      T pos2=GetBinLowEdge(pos+1);
      return (1-drnd)*pos1 + drnd*pos2;
    };
    
    ///return interpolated position for bin pos, such that histograming the position gives a linear slope m,
    ///where m is in units of the bin width for bin pos. Interpolation is done with a random value in the range [0,1]
    T position(std::size_t pos,Trandom m,const Trandom& drnd) {
      if(m>2) m=2;
      if(m<-2) m=-2;
      Trandom x=(0.5*std::sqrt(m*(m+8*drnd-4)+4)-1)/m+0.5;
      T pos1=GetBinLowEdge(pos);
      T pos2=GetBinLowEdge(pos+1);
      return (1-x)*pos1 + x*pos2;
    };
    
  private:
  T m_Min{0};
  T m_Max{0};  

  ClassDef(TFCS1DFunction_HistogramCompactBinEdges,1)  //TFCS1DFunction_HistogramCompactBinEdges
};

class TFCS1DFunction_HistogramInt8BinEdges: public TFCS1DFunction_HistogramCompactBinEdges<float,uint8_t,float>
{
  public:
    TFCS1DFunction_HistogramInt8BinEdges():TFCS1DFunction_HistogramCompactBinEdges<float,uint8_t,float>() {};
    TFCS1DFunction_HistogramInt8BinEdges(std::size_t nbins):TFCS1DFunction_HistogramCompactBinEdges<float,uint8_t,float>(nbins) {};

  ClassDef(TFCS1DFunction_HistogramInt8BinEdges,1)  //TFCS1DFunction_HistogramInt8BinEdges
};

class TFCS1DFunction_HistogramInt16BinEdges: public TFCS1DFunction_HistogramCompactBinEdges<float,uint16_t,float>
{
  public:
    TFCS1DFunction_HistogramInt16BinEdges():TFCS1DFunction_HistogramCompactBinEdges<float,uint16_t,float>() {};
    TFCS1DFunction_HistogramInt16BinEdges(std::size_t nbins):TFCS1DFunction_HistogramCompactBinEdges<float,uint16_t,float>(nbins) {};

  ClassDef(TFCS1DFunction_HistogramInt16BinEdges,1)  //TFCS1DFunction_HistogramInt16BinEdges
};

class TFCS1DFunction_HistogramInt32BinEdges: public TFCS1DFunction_HistogramCompactBinEdges<float,uint32_t,float>
{
  public:
    TFCS1DFunction_HistogramInt32BinEdges():TFCS1DFunction_HistogramCompactBinEdges<float,uint32_t,float>() {};
    TFCS1DFunction_HistogramInt32BinEdges(std::size_t nbins):TFCS1DFunction_HistogramCompactBinEdges<float,uint32_t,float>(nbins) {};

  ClassDef(TFCS1DFunction_HistogramInt32BinEdges,1)  //TFCS1DFunction_HistogramInt32BinEdges
};

template <typename Txvec, typename Ty,typename Trandom=float> class TFCS1DFunctionTemplateHistogram:public TFCS1DFunction
{
  public:
    typedef Trandom random_type;
    typedef Txvec xvec_type;
    typedef Ty y_value_type;

    TFCS1DFunctionTemplateHistogram(TH1* hist=nullptr) {if(hist) Initialize(hist);};
    ~TFCS1DFunctionTemplateHistogram() {};

    std::size_t MemorySizeArray() const {return m_HistoBorders.MemorySizeArray()+m_HistoContents.MemorySizeArray();};
    std::size_t MemorySize() const {return sizeof(*this)+MemorySizeArray();};

    ///set number of bins
    void set_nbins(std::size_t nbins) {m_HistoBorders.set_nbins(nbins);m_HistoContents.set_nbins(nbins);};

    ///return number of bins
    std::size_t get_nbins() const {return m_HistoContents.get_nbins();};
    
    virtual void Initialize(const TH1* hist) {
      Int_t nbins = hist->GetNbinsX();
      
      std::vector<double> temp_HistoContents;
      int first,last;
      double integral=CheckAndIntegrate1DHistogram(hist, temp_HistoContents,first,last);
      if(integral<=0) {
        set_nbins(0);
      } else {
        set_nbins(nbins);

        m_HistoBorders.SetMinMax(hist->GetXaxis()->GetBinLowEdge(first+1),hist->GetXaxis()->GetBinUpEdge(last+1));
        Int_t ihist=0;
        for(Int_t ibin=first;ibin<=last;++ibin) {
          m_HistoBorders.SetBinLowEdge(ihist,hist->GetXaxis()->GetBinLowEdge(ibin+1));
          if(ihist>0) if(m_HistoBorders.GetBinLowEdge(ihist-1)==m_HistoBorders.GetBinLowEdge(ihist)) {
            std::cout<<"Skip bin="<<ibin+1<<" x="<<hist->GetXaxis()->GetBinLowEdge(ibin+1)<<" fx="<<m_HistoBorders.GetBinLowEdge(ihist)<<std::endl;
            --ihist;
            std::cout<<"     bin="<<ibin  <<" x="<<hist->GetXaxis()->GetBinLowEdge(ibin  )<<" fx="<<m_HistoBorders.GetBinLowEdge(ihist)<<std::endl;
          }
          m_HistoContents.set_fraction(ihist,temp_HistoContents[ibin]/integral);
          if(ihist>0) if(m_HistoContents.get_fraction(ihist-1)==m_HistoContents.get_fraction(ihist)) {
            std::cout<<"Skip fbin="<<ihist<<" fx="<<m_HistoBorders.GetBinLowEdge(ihist)<<" frac="<<m_HistoContents.get_fraction(ihist)<<std::endl;
            --ihist;
            std::cout<<"     fbin="<<ihist<<" fx="<<m_HistoBorders.GetBinLowEdge(ihist)<<" frac="<<m_HistoContents.get_fraction(ihist)<<std::endl;
          }

          std::cout<<"bin="<<ibin+1<<" fbin="<<ihist<<" x=["<<hist->GetXaxis()->GetBinLowEdge(ibin+1)<<","<<hist->GetXaxis()->GetBinUpEdge(ibin+1)<<"] fx="<<m_HistoBorders.GetBinLowEdge(ihist)<<" int="<<temp_HistoContents[ibin]/integral<<" frac="<<m_HistoContents.get_fraction(ihist)<<std::endl;
          
          ++ihist;
        }  
        if(ihist!=nbins) {
          set_nbins(ihist);
          m_HistoBorders.SetMinMax(hist->GetXaxis()->GetBinLowEdge(first+1),hist->GetXaxis()->GetBinUpEdge(last+1));
        }
      }  
    }

    using TFCS1DFunction::rnd_to_fct;
    
    ///Function gets random number rnd in the range [0,1) as argument 
    ///and returns function value according to a histogram distribution
    virtual double rnd_to_fct(double rnd) {
      if(m_HistoContents.get_nbins()==0) return 0;
      Trandom residual_rnd;
      std::size_t ibin=m_HistoContents.get_bin(rnd,residual_rnd);
      //std::cout<<"fbin="<<ibin<<" fx="<<m_HistoBorders.GetBinLowEdge(ibin)<<" frac="<<m_HistoContents.get_fraction(ibin)<<" residual_rnd="<<residual_rnd<<std::endl;
      return m_HistoBorders.position(ibin,residual_rnd);
    }

    const Txvec& get_HistoBordersx() const {return m_HistoBorders;};
    Txvec& get_HistoBordersx() {return m_HistoBorders;};
    
    const TFCS1DFunction_HistogramContent<Ty,Trandom>& get_HistoContents() const {return m_HistoContents;};
    TFCS1DFunction_HistogramContent<Ty,Trandom>& get_HistoContents() {return m_HistoContents;};
    
    //static void unit_test(TH1* hist=nullptr);
  protected:
    
    Txvec m_HistoBorders;
    TFCS1DFunction_HistogramContent<Ty,Trandom> m_HistoContents;

  private:

  ClassDef(TFCS1DFunctionTemplateHistogram,1)  //TFCS1DFunctionTemplateHistogram
};

template <typename Txvec, typename Ty,typename Trandom=float> class TFCS1DFunctionTemplateInterpolationHistogram:public TFCS1DFunctionTemplateHistogram<Txvec,Ty,Trandom>
{
  public:
    TFCS1DFunctionTemplateInterpolationHistogram(TH1* hist=nullptr):TFCS1DFunctionTemplateHistogram<Txvec,Ty,Trandom>(hist) {};

    using TFCS1DFunction::rnd_to_fct;
    using TFCS1DFunctionTemplateHistogram<Txvec,Ty,Trandom>::get_nbins;
    using TFCS1DFunctionTemplateHistogram<Txvec,Ty,Trandom>::m_HistoContents;
    using TFCS1DFunctionTemplateHistogram<Txvec,Ty,Trandom>::m_HistoBorders;

    ///Function gets random number rnd in the range [0,1) as argument 
    ///and returns function value according to a histogram distribution.
    ///A linear interpolation is done within each bin
    virtual double rnd_to_fct(double rnd) {
      std::size_t nbins=get_nbins();
      if(nbins==0) return 0;
      Trandom residual_rnd;
      std::size_t ibin=m_HistoContents.get_bin(rnd,residual_rnd);
      
      Trandom frac=m_HistoContents.get_fraction(ibin);
  
      Trandom dfrac;
      Trandom dfracprev;
      if(ibin>0) {
        Trandom fracprev=m_HistoContents.get_fraction(ibin-1);
        dfrac=frac-fracprev;
        if(ibin>1) dfracprev=fracprev-m_HistoContents.get_fraction(ibin-2);
         else dfracprev=fracprev;
      } else {
        dfrac=frac;
        dfracprev=dfrac;
      }
      
      Trandom dfracnext;
      if(ibin<nbins-1) dfracnext=m_HistoContents.get_fraction(ibin+1)-frac;
       else dfracnext=dfrac;
      
      Trandom m=dfrac>0 ? 0.5*(dfracnext - dfracprev) / dfrac : 0;
      
      //std::cout<<"fbin="<<ibin<<" fx="<<m_HistoBorders.GetBinLowEdge(ibin)<<" frac="<<m_HistoContents.get_fraction(ibin)<<" dfracprev="<<dfracprev<<" dfrac="<<dfrac<<" dfracnext="<<dfracnext<<" dfracprev-dfrac="<<dfracprev-dfrac<<" dfracnext-dfrac="<<dfracnext-dfrac<<" m="<<m<<" residual_rnd="<<residual_rnd<<std::endl;
      return m_HistoBorders.position(ibin,m,residual_rnd);
    }

  ClassDef(TFCS1DFunctionTemplateInterpolationHistogram,1)  //TFCS1DFunctionTemplateInterpolationHistogram
};

class TFCS1DFunctionInt8Int8Histogram: public TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint8_t,float>
{
  public:
    TFCS1DFunctionInt8Int8Histogram(TH1* h=nullptr):TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint8_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt8Int8Histogram,1)  //TFCS1DFunctionInt8Int8Histogram
};

class TFCS1DFunctionInt8Int16Histogram: public TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint16_t,float>
{
  public:
    TFCS1DFunctionInt8Int16Histogram(TH1* h=nullptr):TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint16_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt8Int16Histogram,1)  //TFCS1DFunctionInt8Int16Histogram
};

class TFCS1DFunctionInt8Int32Histogram: public TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint32_t,float>
{
  public:
    TFCS1DFunctionInt8Int32Histogram(TH1* h=nullptr):TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint32_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt8Int32Histogram,1)  //TFCS1DFunctionInt8Int32Histogram
};

class TFCS1DFunctionInt16Int16Histogram: public TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint16_t,float>
{
  public:
    TFCS1DFunctionInt16Int16Histogram(TH1* h=nullptr):TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint16_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt16Int16Histogram,1)  //TFCS1DFunctionInt16Int16Histogram
};

class TFCS1DFunctionInt16Int32Histogram: public TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint32_t,float>
{
  public:
    TFCS1DFunctionInt16Int32Histogram(TH1* h=nullptr):TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint32_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt16Int32Histogram,1)  //TFCS1DFunctionInt16Int32Histogram
};

class TFCS1DFunctionInt8Int8InterpolationHistogram: public TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint8_t,float>
{
  public:
    TFCS1DFunctionInt8Int8InterpolationHistogram(TH1* h=nullptr):TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint8_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt8Int8InterpolationHistogram,1)  //TFCS1DFunctionInt8Int8InterpolationHistogram
};

class TFCS1DFunctionInt8Int16InterpolationHistogram: public TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint16_t,float>
{
  public:
    TFCS1DFunctionInt8Int16InterpolationHistogram(TH1* h=nullptr):TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint16_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt8Int16InterpolationHistogram,1)  //TFCS1DFunctionInt8Int16InterpolationHistogram
};

class TFCS1DFunctionInt16Int16InterpolationHistogram: public TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint16_t,float>
{
  public:
    TFCS1DFunctionInt16Int16InterpolationHistogram(TH1* h=nullptr):TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint16_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt16Int16InterpolationHistogram,1)  //TFCS1DFunctionInt16Int16InterpolationHistogram
};

class TFCS1DFunctionInt16Int32InterpolationHistogram: public TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint32_t,float>
{
  public:
    TFCS1DFunctionInt16Int32InterpolationHistogram(TH1* h=nullptr):TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint32_t,float>(h) {};

  ClassDef(TFCS1DFunctionInt16Int32InterpolationHistogram,1)  //TFCS1DFunctionInt16Int32InterpolationHistogram
};




#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCS1DFunction_Numeric<uint8_t,float>+;
#pragma link C++ class TFCS1DFunction_Numeric<uint16_t,float>+;
#pragma link C++ class TFCS1DFunction_Numeric<uint32_t,float>+;
#pragma link C++ class TFCS1DFunction_Numeric<float,float>+;
#pragma link C++ class TFCS1DFunction_Numeric<double,float>+;
#pragma link C++ class TFCS1DFunction_Numeric<double,double>+;

#pragma link C++ class TFCS1DFunction_Array<float>+;
#pragma link C++ class TFCS1DFunction_Array<double>+;
#pragma link C++ class TFCS1DFunction_Array<uint8_t>+;
#pragma link C++ class TFCS1DFunction_Array<uint16_t>+;
#pragma link C++ class TFCS1DFunction_Array<uint32_t>+;

#pragma link C++ class TFCS1DFunction_HistogramContent<float,float>+;
#pragma link C++ class TFCS1DFunction_HistogramContent<double,float>+;
#pragma link C++ class TFCS1DFunction_HistogramContent<double,double>+;
#pragma link C++ class TFCS1DFunction_HistogramContent<uint8_t,float>+;
#pragma link C++ class TFCS1DFunction_HistogramContent<uint16_t,float>+;
#pragma link C++ class TFCS1DFunction_HistogramContent<uint32_t,float>+;

#pragma link C++ class TFCS1DFunction_HistogramBinEdges<float,float>+;
#pragma link C++ class TFCS1DFunction_HistogramBinEdges<double,float>+;
#pragma link C++ class TFCS1DFunction_HistogramBinEdges<double,double>+;

#pragma link C++ class TFCS1DFunction_HistogramCompactBinEdges<float,uint8_t,float>+;
#pragma link C++ class TFCS1DFunction_HistogramCompactBinEdges<float,uint16_t,float>+;
#pragma link C++ class TFCS1DFunction_HistogramCompactBinEdges<float,uint32_t,float>+;

#pragma link C++ class TFCS1DFunction_HistogramInt8BinEdges+;
#pragma link C++ class TFCS1DFunction_HistogramInt16BinEdges+;
#pragma link C++ class TFCS1DFunction_HistogramInt32BinEdges+;
#pragma link C++ class TFCS1DFunction_HistogramFloatBinEdges+;
#pragma link C++ class TFCS1DFunction_HistogramDoubleBinEdges+;

#pragma link C++ class TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint8_t,float>+;
#pragma link C++ class TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint16_t,float>+;
#pragma link C++ class TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint32_t,float>+;
#pragma link C++ class TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint16_t,float>+;
#pragma link C++ class TFCS1DFunctionTemplateHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint32_t,float>+;

#pragma link C++ class TFCS1DFunctionInt8Int8Histogram+;
#pragma link C++ class TFCS1DFunctionInt8Int16Histogram+;
#pragma link C++ class TFCS1DFunctionInt8Int32Histogram+;
#pragma link C++ class TFCS1DFunctionInt16Int16Histogram+;
#pragma link C++ class TFCS1DFunctionInt16Int32Histogram+;

#pragma link C++ class TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint8_t,float>+;
#pragma link C++ class TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt8BinEdges,uint16_t,float>+;
#pragma link C++ class TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint16_t,float>+;
#pragma link C++ class TFCS1DFunctionTemplateInterpolationHistogram<TFCS1DFunction_HistogramInt16BinEdges,uint32_t,float>+;

#pragma link C++ class TFCS1DFunctionInt8Int8InterpolationHistogram+;
#pragma link C++ class TFCS1DFunctionInt8Int16InterpolationHistogram+;
#pragma link C++ class TFCS1DFunctionInt16Int16InterpolationHistogram+;
#pragma link C++ class TFCS1DFunctionInt16Int32InterpolationHistogram+;

#endif

#endif
