/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* package FTKSteering
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 *
 * Use the class FTKSteering to read a steering file with
 * predefined parameters (undefined parameters give errors)
 *
 * Parameters can be int, double, string or vectors of these types
 *
 * Before the steering is parsed, valid parameters have to be defined.
 * After parsing, the parameters can be accessed
 *
 * For an instance of FTKSteering, one defines all valid parameters.
 * A parameter has a name tag, an (optional) array size, and a default value
 * the array size can be zero, in which case it is determined when parsing.
 *
 * Alternatively, the array size can be given in the code, then the exact 
 * array size has to be matched by the input
 *
 *
 */

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "FTKLogging.h"

class SteeringParameter {
   // base class for steering parameters
 public:
   virtual ~SteeringParameter(void);
   virtual operator int() const;
   virtual operator double() const;
   virtual operator char const *() const;
   virtual bool Read(std::istream &in)=0;
   virtual void Write(std::ostream &out) const =0;
   virtual SteeringParameter *Clone(void) const=0;
};

class SteeringParameterInt : public SteeringParameter {
 public:
   SteeringParameterInt(int init);
   virtual int &operator=(int const &);
   virtual operator int() const;
   virtual operator double() const;
   virtual bool Read(std::istream &in);
   virtual void Write(std::ostream &out) const;
   virtual SteeringParameter *Clone(void) const;
 protected:
   int data;
};

class SteeringParameterDouble : public SteeringParameter {
 public:
   SteeringParameterDouble(double init);
   virtual double &operator=(double const &);
   virtual operator double() const;
   virtual bool Read(std::istream &in);
   virtual void Write(std::ostream &out) const;
   virtual SteeringParameter *Clone(void) const;
 protected:
   double data;
};

class SteeringParameterString : public SteeringParameter {
 public:
   SteeringParameterString(char const *init);
   virtual ~SteeringParameterString(void);
   virtual char const *operator=(char const *);
   virtual operator char const *() const;
   virtual bool Read(std::istream &in);
   virtual void Write(std::ostream &out) const;
   virtual SteeringParameter *Clone(void) const;
 protected:
   std::string data;
};

class SteeringParameterArray {
 public:
   SteeringParameterArray(void);
   ~SteeringParameterArray(void);
   inline unsigned GetFixedSize(void) const { return fixed; }
   inline unsigned GetActualSize(void) const { return data.size(); }
   void Init(SteeringParameter *proto,unsigned size);
   void Clear(void);
   inline operator int () const { return *(data[0]); }
   inline operator double () const { return *(data[0]); }
   inline operator const char * () const { return *(data[0]); }
   inline SteeringParameter const & operator[](int i) const { return *(data[i]); }
   inline SteeringParameter & operator[](int i) { return *(data[i]); }
   SteeringParameter *Expand(void);
   bool Shrink(void);
   SteeringParameterArray &operator=(SteeringParameterArray const &);
 protected:
   unsigned fixed;
   SteeringParameter *prototype;
   std::vector<SteeringParameter *> data;
};

class FTKSteering : public FTKLogging {
 public:
   FTKSteering(const char *name="FTKSteering") : FTKLogging(name) { }
   virtual ~FTKSteering(void);
   virtual int Parse(std::istream &in);
   // add a new parameter
   // returns unique index to access this parameter
   inline SteeringParameterArray &AddIntPar
      (char const *name,unsigned size=1,int init=0) {
      return AddPar(name,new SteeringParameterInt(init),size);
   }
   inline SteeringParameterArray &AddDoublePar
      (char const *name,unsigned size=1,double const &init=0.0) {
      return AddPar(name,new SteeringParameterDouble(init),size);
   }
   inline SteeringParameterArray &AddStringPar
      (char const *name,unsigned size=1,char const *init=0) {
      return AddPar(name,new SteeringParameterString(init),size);
   }
   SteeringParameterArray const *operator[](std::string const &) const;
   void Print(std::ostream &out) const;
 protected:
   SteeringParameterArray &AddPar
      (char const *name,SteeringParameter *proto,unsigned size);
   std::map<std::string,SteeringParameterArray> parameter;
};
