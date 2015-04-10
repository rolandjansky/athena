/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MeanAndRMS_h
#define MeanAndRMS_h

#include <math.h>

class MeanAndRMS {
public :
  MeanAndRMS():w(0),wx(0),wx2(0) {};
  virtual ~MeanAndRMS() {};
   
  MeanAndRMS& add(double xadd,double weight=1) {wx+=weight*xadd;wx2+=weight*xadd*xadd;w+=weight;return *this;};
  MeanAndRMS& operator+=(double xadd) {return add(xadd);};
  MeanAndRMS& operator-=(double xadd) {return add(-xadd);};
  
  double sum_weight() const {return w;};
  double mean()       const {if(w!=0) return wx/w; else return 0;};
  double rms2()       const {double x=mean();return wx2/w - x*x;};
  double rms()        const {double r2=rms2();if(r2>=0) return sqrt(r2); else return 0;};
  double mean_error() const {return rms()/sqrt(w);};
  double rms_error()  const {return rms()/sqrt(2*w);};

  operator double() const { return mean(); }
protected:
  double w;
  double wx,wx2;
};

#endif

