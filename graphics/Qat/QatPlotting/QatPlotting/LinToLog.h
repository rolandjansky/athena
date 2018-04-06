#ifndef Lin2Log
#define Lin2Log
#include <cmath>
#include <stdexcept>
class LinToLog {

 public:

  // Constructor:
  LinToLog(double minVal, double maxVal):m_a(minVal),m_b(maxVal) 
    {
      if (m_a>m_b)  throw std::runtime_error ("Error in logarithmic mapper a>b ");
      if (m_a<=0) throw std::runtime_error ("Error in logarithmic mapper a<=0 ");
      m_c = (m_a-m_b)/log(m_a/m_b);
    }
    
    double operator () (double x) const { 
      return m_a + m_c*log(x/m_a);
    }

 private:

    double m_a;
    double m_b;
    double m_c;
};
#endif

