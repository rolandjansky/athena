#ifndef __DOUBLETRESLOOKUP_H__
#define __DOUBLETRESLOOKUP_H__

#include <vector>

enum DoubletType{inner, outer};

// A class to store and access fits to resolutions of doublets used in track seed generation
class DoubletResLookup {
  public:
  DoubletResLookup();
  ~DoubletResLookup() {};

  double getRes(unsigned int layer, DoubletType type, double eta);

  private:
  std::vector< std::vector<double> > m_fitPars_inn; // fit parameters for `inner' type doublets
  std::vector< std::vector<double> > m_fitPars_outr; // fit parameters for `outer' type doublets
};

#endif // __DOUBLETRESLOOKUP_H__
