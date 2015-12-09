class BaseInfoBase {};

template <class T>
class BaseInfo
{
public:
  static const BaseInfoBase& baseinfo ();

  struct instance_holder
  {
   instance_holder();
    BaseInfoBase* instance;
  };
  static instance_holder s_instance;
};


#include "usingns5_test.h"

const BaseInfoBase* pp [[gnu::thread_safe]] = &BaseInfo<int>::baseinfo();

namespace CLHEP {class RandGauss; }
using CLHEP::RandGauss;

