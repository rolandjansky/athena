#include "RIO_OnTrackErrorScalingKit.h"

// need toinstantiate static instance pointer
template <>
std::unique_ptr< KitManager<RIO_OnTrackErrorScalingKit> >
KitManager<RIO_OnTrackErrorScalingKit>::s_instance {};

size_t RIO_OnTrackErrorScalingKit::getParamIndex(const std::string &name) const {
  const char **param_names = paramNames();
  for(size_t idx=0; idx<nParametres(); ++idx) {
    if (strcmp(param_names[idx],name.c_str())==0) return idx;
  }
  std::stringstream message;
  message << "RIO_OnTrackErrorScaling parameter " << name << " not found.";
  throw std::runtime_error(message.str());
  return nParametres();
}

