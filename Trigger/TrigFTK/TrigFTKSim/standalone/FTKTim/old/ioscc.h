/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

namespace ioscc
{
  enum TColorCode {red=1, blue, green, reset};
  const char *_sesc    = "\033[";
  const char *_sreset  = "0m";
  const char *_sred    = "31;1m";
  const char *_sblue   = "34;1m";
  const char *_sgreen  = "32;1m";
};

struct _colored_stream
{
  std::ostream &os;
};

inline _colored_stream operator<< (std::ostream &os, ioscc::TColorCode ccode)
{
  switch (ccode)
  {
    case ioscc::red    : os<<ioscc::_sesc<<ioscc::_sred;   break;
    case ioscc::blue   : os<<ioscc::_sesc<<ioscc::_sblue;  break;
    case ioscc::green  : os<<ioscc::_sesc<<ioscc::_sgreen; break;
    case ioscc::reset  : os<<ioscc::_sesc<<ioscc::_sreset; break;
  }
  _colored_stream colos={os};
  return colos;
}

template <typename T>
_colored_stream operator<< (_colored_stream os, const T     &rhs)
{
  os.os << rhs;
  return os;
};
template <>
_colored_stream operator<< (_colored_stream os, const ioscc::TColorCode &ccode)
{
  os.os << ioscc::_sesc<<ioscc::_sreset;
  os.os << ccode;
  return os;
};
_colored_stream operator<< (_colored_stream os, std::ostream& (*__pf)(std::ostream&))
{
  os.os << ioscc::_sesc<<ioscc::_sreset;
  __pf(os.os);
  return os;
};
