
#ifndef TRIGHLTJETHYPO_JETATTRSSTRINGTODOUBLE_H
#define TRIGHLTJETHYPO_JETATTRSSTRINGTODOUBLE_H

class JetAttrsStringToDouble
{
public:
  std::pair<double,double> operator() (const std::string& low, const std::string& high) const {
    double lowLim = std::stod(low);
    double highLim = std::stod(high);

    return std::make_pair(lowLim,highLim);
  }
};

#endif

