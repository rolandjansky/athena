#include "./svec2dvec.h"
#include "./ArgStrToDouble.h"
#include <algorithm>

std::vector<double>  svec2dvec(const std::vector<std::string>& svec){
  
  std::vector<double> result;
  result.reserve(svec.size());
  
  std::transform(svec.cbegin(), svec.cend(), result.begin(), ArgStrToDouble());
  return result;
}
