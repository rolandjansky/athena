/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * L1TopoDataTypes.h
 * author : Ignacio Aracena <ignacio.aracena@cern.ch>
 * Created May 2015
 *
 * @brief Tools for handling fixed point Qm.n numbers
**********************************/
#ifndef L1TopoSimulationUtils_L1TOPODATATYPES_H
#define L1TopoSimulationUtils_L1TOPODATATYPES_H
#include <string>
#include <iostream>

/* Setting maximal number of bits to 32. */
#ifndef MAXBITS 
#define MAXBITS 32
#endif 


namespace {
/// Return a bit mask with the lower @a n bits set.
template <class T>
inline
T ones (unsigned int n)
{
  if (n >= sizeof(T) * 8)
    return ~static_cast<T>(0);
  return (static_cast<T>(1) << n) - 1;
}
}


namespace TSU {

    typedef unsigned int T;
    T convert(const unsigned int& v, const unsigned& in_p, const unsigned int& in_f, 
                                const unsigned int& out_p, const unsigned int& out_f);

/*********************************
 * Template class implements Qm.n number format
 * PREC : Total number of bits
 * F : fractional bits
 * PREC - F - 1 : integer bits (one bit used for sign)
 *
 * Usage TSU::L1TopoDataTypes<10,7> a("0010100011")
 * or 
 * Usage TSU::L1TopoDataTypes<10,7> a(163)
 * with PREC = 10 and F=7 163 (or 0010100011) corresponds to 1.27344
*********************************/

    template <unsigned PREC, unsigned F> class L1TopoDataTypes {
    public:
       L1TopoDataTypes(T v) : m_tvalue(v) { }
       // c'tor using binary pattern as input
       L1TopoDataTypes(std::string b="") : m_tvalue(0) { 
          unsigned int idx = 0;
          for(auto in = b.rbegin(); in!=b.rend(); ++in){
              if(*in=='1') m_tvalue += (1 << idx);
              ++idx;
          }
       }
    
       L1TopoDataTypes(double d) : m_tvalue(d) {
          m_tvalue = d*(1<<F);
       }

       L1TopoDataTypes(int i) : m_tvalue(i) {
          m_tvalue = i*(1<<F);
       }

       // converts number from one set of template parameters to another
       template<unsigned P1, unsigned F1> operator L1TopoDataTypes<P1,F1>(){
          return L1TopoDataTypes<P1,F1>(convert(m_tvalue,PREC,F,P1,F1));
       }

       operator float(){
          return this->to_float();
       }

       operator int(){
           return ones<T>(PREC-F)&((m_tvalue>>F));
       }

       T value() const {
          return m_tvalue;
       }
    
       T abs() const {
          T mask = m_tvalue >> (PREC-1); 
          T res = ((mask ^ m_tvalue) - mask) & ones<T>(PREC);
          return res;
       }

       // returns two's complement of a value (i.e. the negative)
       T complement() const {
          T res=0;
          T v = m_tvalue;
          for(unsigned j=0;j<PREC;++j){
             res += v & (1 << j) ? 0 : (1 << j);
          }
          res += 1;
          return res;
       }
    
       // increments value
       L1TopoDataTypes& operator+=(const T& value){
          m_tvalue += value;
          return *this;
       }

       // flips sign
       L1TopoDataTypes& operator-() {
          m_tvalue = this->complement();
          return *this;
       }

       // scale value with an integer
       L1TopoDataTypes& operator*=(const int& factor){
          short int neg = (m_tvalue >> (PREC-1)) ? 1 : 0;
          if(neg && factor<0){
             neg = 0;
             m_tvalue = this->complement() * ::abs(factor);
          } else if(factor<0){
             m_tvalue = (m_tvalue * ::abs(factor));
             m_tvalue = this->complement();
          } else {
             m_tvalue = m_tvalue * factor;
          }
          return *this;
       }

       // multiplication with an integer
       template<unsigned P,unsigned FF> friend L1TopoDataTypes<P,FF> operator*(const int& factor, L1TopoDataTypes<P,FF> d);
       template<unsigned P,unsigned FF> friend L1TopoDataTypes<P,FF> operator*(L1TopoDataTypes<P,FF> d,const int& factor);

       // add two numbers with different representation
       template<unsigned P1, unsigned F1, unsigned P2, unsigned F2> friend L1TopoDataTypes<((P1-F1) > (P2-F2) ? (P1-F1) : (P2-F2)) + ((F1 > F2) ? F1 : F2), (F1 > F2) ? F1 : F2> operator+(const L1TopoDataTypes<P1,F1>& lhs, const L1TopoDataTypes<P2,F2>& rhs);
    
       // subtract two numbers with different representation
       template<unsigned P1, unsigned F1, unsigned P2, unsigned F2> friend L1TopoDataTypes<((P1-F1) > (P2-F2) ? (P1-F1) : (P2-F2)) + ((F1 > F2) ? F1 : F2), (F1 > F2) ? F1 : F2> operator-(const L1TopoDataTypes<P1,F1> lhs, const L1TopoDataTypes<P2,F2> rhs);
    
       // subtract L1TopoDataTypes from double
       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator-(const double& lhs, const L1TopoDataTypes<P1,F1>& rhs);

       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator-(const L1TopoDataTypes<P1,F1>& lhs, const double& rhs);

       // subtract L1TopoDataTypes from int
       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator-(const int& lhs, const L1TopoDataTypes<P1,F1>& rhs);

       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator-(const L1TopoDataTypes<P1,F1>& lhs, const int& rhs);
       // product of two numbers with different representation
       template<unsigned P1, unsigned F1, unsigned P2, unsigned F2> friend L1TopoDataTypes<(P1+P2-1 > MAXBITS) ? MAXBITS : P1+P2-1, (P1+P2-1 > MAXBITS) ? (F2>F1 ? F2 - ((P1+P2-1) - MAXBITS) : F1 - ((P1+P2-1) - MAXBITS)) : ((F1 > F2) ? F1 : F2)> operator*(const L1TopoDataTypes<P1,F1> lhs, const L1TopoDataTypes<P2,F2> rhs);
    
       // add L1TopoDataTypes to double
       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator+(const double& lhs, const L1TopoDataTypes<P1,F1>& rhs);

       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator+(const L1TopoDataTypes<P1,F1>& lhs, const double& rhs);

       // add L1TopoDataTypes to int
       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator+(const int& lhs, const L1TopoDataTypes<P1,F1>& rhs);

       template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> friend operator+(const L1TopoDataTypes<P1,F1>& lhs, const int& rhs);
       // product of two numbers with different representation
       template<unsigned P1, unsigned F1, unsigned P2, unsigned F2> friend L1TopoDataTypes<(P1+P2-1 > MAXBITS) ? MAXBITS : P1+P2-1, (P1+P2-1 > MAXBITS) ? (F2>F1 ? F2 - ((P1+P2-1) - MAXBITS) : F1 - ((P1+P2-1) - MAXBITS)) : ((F1 > F2) ? F1 : F2)> operator*(const L1TopoDataTypes<P1,F1> lhs, const L1TopoDataTypes<P2,F2> rhs);
    
       // overloaded operator<<
       friend std::ostream& operator<<(std::ostream& os, const L1TopoDataTypes& d){
          std::string out;
          for(int j=PREC-1;j>=0;--j){
             out += ((d.m_tvalue>>j)&1) ? "1" : "0";
          }
          os << "integer value " << d.m_tvalue << " binary " << out << " float " << d.to_float();
          return os;
       }
    
       // return float representation
       float to_float() const {
          // Find sign
          float res = ((m_tvalue>>(PREC-1))&1) ? -(1<<(PREC-F)) : 0.;
          // Get integer part
          res += (m_tvalue>>F)&((1<<(PREC-F))-1) ? float((m_tvalue>>F)&((1<<(PREC-F))-1)) : 0;
          // Do the fractional part
          if (F > 0) {
            unsigned frac = m_tvalue & ( (1<<F)-1 );
            res += static_cast<float>(frac) / (2<<(F-1));
          }
          return res;
       }

       unsigned int prec() const { return PREC; }
       unsigned int frac() const { return F; }
    
    private:
       T m_tvalue;
    };

    // get the 2's complement of bitset with p bits
    T complement(const T& v, const unsigned int& p); 

    // masks from Qm.n to a Qm1.n1, where in_p and in_f = total number of bits and of fraction part of input 
    //T convert(const unsigned int& v, const unsigned& in_p, const unsigned int& in_f, 
    //                            const unsigned int& out_p, const unsigned int& out_f);

    // represent T value with p bits as binary
    std::string to_binary(T value, const unsigned int& p);

    // represent Qm.n number as float with p = total number of bits, f = number of fractional bits
    float to_float(const T& va, const unsigned int& p, const unsigned int& f);

    // operator+ for L1TopoDataTypes
    template<unsigned P1, unsigned F1, unsigned P2, unsigned F2> L1TopoDataTypes<((P1-F1) > (P2-F2) ? (P1-F1) : (P2-F2)) + ((F1 > F2) ? F1 : F2), (F1 > F2) ? F1 : F2> operator+(const L1TopoDataTypes<P1,F1>& lhs, const L1TopoDataTypes<P2,F2>& rhs){
       const unsigned int frac = (F1 > F2) ? F1 : F2;
       const unsigned int digit = ((P1-F1) > (P2-F2)) ? (P1-F1) - 1 : (P2-F2) - 1;
       const unsigned int prec = frac + digit + 1;
       T lhsconvert = convert(lhs.m_tvalue,P1,F1,prec,frac);
       T rhsconvert = convert(rhs.m_tvalue,P2,F2,prec,frac);
       L1TopoDataTypes<prec,frac> res(lhsconvert+rhsconvert);
      
       return res;
    }

    // operator- for L1TopoDataTypes
    template<unsigned P1, unsigned F1, unsigned P2, unsigned F2> L1TopoDataTypes<((P1-F1) > (P2-F2) ? (P1-F1) : (P2-F2)) + ((F1 > F2) ? F1 : F2), (F1 > F2) ? F1 : F2> operator-(const L1TopoDataTypes<P1,F1> lhs, const L1TopoDataTypes<P2,F2> rhs){
       const unsigned int frac = (F1 > F2) ? F1 : F2;
       const unsigned int digit = ((P1-F1) > (P2-F2)) ? (P1-F1) - 1 : (P2-F2) - 1;
       const unsigned int prec = frac + digit + 1;
       T lhsconvert = convert(lhs.m_tvalue,P1,F1,prec,frac);
       T rhsconvert = convert(rhs.m_tvalue,P2,F2,prec,frac);
       L1TopoDataTypes<prec,frac> res(lhsconvert+complement(rhsconvert,prec));
      
       return res;
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator-(const double& lhs, const L1TopoDataTypes<P1,F1>& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs - rhs.to_float());
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator-(const L1TopoDataTypes<P1,F1>& lhs, const double& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs.to_float() - rhs);
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator-(const int& lhs, const L1TopoDataTypes<P1,F1>& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs - rhs.to_float());
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator-(const L1TopoDataTypes<P1,F1>& lhs, const int& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs.to_float() - rhs);
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator+(const double& lhs, const L1TopoDataTypes<P1,F1>& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs + rhs.to_float());
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator+(const L1TopoDataTypes<P1,F1>& lhs, const double& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs.to_float() + rhs);
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator+(const int& lhs, const L1TopoDataTypes<P1,F1>& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs + rhs.to_float());
    }

    template<unsigned P1, unsigned F1> L1TopoDataTypes<P1+1,F1> operator+(const L1TopoDataTypes<P1,F1>& lhs, const int& rhs){
        return L1TopoDataTypes<P1+1,F1>(lhs.to_float() + rhs);
    }


    // operator* for L1TopoDataTypes
    template<unsigned P1, unsigned F1, unsigned P2, unsigned F2> L1TopoDataTypes<(P1+P2-1 > MAXBITS) ? MAXBITS : P1+P2-1, (P1+P2-1 > MAXBITS) ? (F2>F1 ? F2 - ((P1+P2-1) - MAXBITS) : F1 - ((P1+P2-1) - MAXBITS)) : ((F1 > F2) ? F1 : F2)> operator*(const L1TopoDataTypes<P1,F1> lhs, const L1TopoDataTypes<P2,F2> rhs){
       const unsigned int frac = (P1+P2-1 > MAXBITS) ? (F2>F1 ? F2 - ((P1+P2-1) - MAXBITS) : F1 - ((P1+P2-1) - MAXBITS)) : ((F1 > F2) ? F1 : F2);
       const unsigned int digit = ((P1+P2-1 > MAXBITS) ? (MAXBITS - frac) : (P1+P2-1 - frac)) - 1;
       const unsigned int prec = frac + digit + 1;
       T lhsconvert = lhs.m_tvalue;
       T rhsconvert = rhs.m_tvalue;
       // check if either value is negative and work with the absolute value
       if((lhs.m_tvalue >> (P1-1)) & 1){
          lhsconvert = complement(lhsconvert,P1);
       }
       if((rhs.m_tvalue >> (P2-1)) & 1){ 
          rhsconvert = complement(rhsconvert,P2);
       }
       // map numbers into Q1+digit.frac
       T lhsconvint = convert(lhsconvert,P1,F1,P1-F1,0);
       T rhsconvint = convert(rhsconvert,P2,F2,P2-F2,0);
       T lhsconvfrac = convert(lhsconvert,P1,F1,F1+1,F1);
       T rhsconvfrac = convert(rhsconvert,P2,F2,F2+1,F2);
       T prod_int = lhsconvint*rhsconvint; // PREC = P1-F1+P2-F2-1, F = 0
       T prod_frac = lhsconvfrac*rhsconvfrac; // PREC = F1 + F2 + 1, F = F1 + F2
       T prod_mix1 = lhsconvint*rhsconvfrac;  // PREC = P1 - F1 + F2, F = F2
       T prod_mix2 = lhsconvfrac*rhsconvint;  // PREC = P2 - F2 + F1, F = F1
       L1TopoDataTypes<prec,frac> result(prod_int<<frac);
       result += (prod_frac>>(F1+F2-frac));
       result += (prod_mix1>>((F2>frac ? F2 - frac : 0)));
       result += (prod_mix2>>((F1>frac ? F1 - frac : 0)));
       if(!(((lhs.m_tvalue >> (P1-1)) & 1) ^ ((rhs.m_tvalue >> (P2-1)) & 1))){
            return result;
       } else 
            return -result;
    }

    template<unsigned P, unsigned FF> TSU::L1TopoDataTypes<P,FF> operator*(const int& factor, TSU::L1TopoDataTypes<P,FF> d){
       return d*=factor;
     }

    template<unsigned P, unsigned FF> TSU::L1TopoDataTypes<P,FF> operator*(TSU::L1TopoDataTypes<P,FF> d, const int& factor){
       return d*=factor;
     }

} // namespace
#endif
