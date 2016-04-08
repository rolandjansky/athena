#ifndef  _CHISQ_H_
#define  _CHISQ_H_
#include "QatDataModeling/ObjectiveFunction.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/AbsFunctional.hh"
#include <vector>
class ChiSq:public Genfun::AbsFunctional {
public:

  // Constructor
  ChiSq(){}

  // Destructor:
  ~ChiSq(){}
  
  // Function Call Operator
  virtual double operator [] (const Genfun::AbsFunction & f) const {
    double c2=0.0;
    for (unsigned int i=0;i<_points.size();i++) {
      point p = _points[i];
      double y = f(p.x);
      c2 += (p.y-y)*(p.y-y)/p.dy/p.dy;
    }
    return c2;
  }

  // Add a data point:
  void addPoint (double x, double y, double dy) {
    point p;
    p.x=x;
    p.y=y;
    p.dy=dy;
    _points.push_back(p);
  }
  
private:
  struct point {
    double x;
    double y;
    double dy;
  };
  
  std::vector<point> _points;

};
#endif
