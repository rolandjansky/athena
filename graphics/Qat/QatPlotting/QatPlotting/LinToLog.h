#ifndef Lin2Log
#define Lin2Log
#include <cmath>
#include <stdexcept>
class LinToLog {

 public:

  // Constructor:
  LinToLog(double minVal, double maxVal):a(minVal),b(maxVal) 
    {
      if (a>b)  throw std::runtime_error ("Error in logarithmic mapper a>b ");
      if (a<=0) throw std::runtime_error ("Error in logarithmic mapper a<=0 ");
      c = (a-b)/log(a/b);
    }
    
    double operator () (double x) const { 
      return a + c*log(x/a);
    }

 private:

    double a;
    double b;
    double c;
};
#endif

