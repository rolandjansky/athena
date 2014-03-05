// Check.h

#ifndef asg_Check
#define asg_Check

namespace asg {

class Check {
public:
  static void passed();
  static void failed();
  static void setshowpass(bool val);
  static bool showpass();
};

}  // end asg namespace

#define ASG_CHECK(EXP) do {                     \
 StatusCode sc__ = (EXP);                       \
 if ( !sc__.isSuccess() ) asg::Check::failed(); \
 else                     asg::Check::passed(); \
} while (0)

#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_STANDALONE
#define ATH_CHECK(EXP) ASG_CHECK(EXP)
#endif

#endif
