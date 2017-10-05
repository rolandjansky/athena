#include "AthenaPython/PyAthenaAlg.h"
#include "AthenaPython/PyAthenaSvc.h"
#include "AthenaPython/PyAthenaTool.h"
#include "AthenaPython/PyAthenaAud.h"
#include "../PyComponentMgr.h"
#include "../PyJobOptionsSvc.h"


DECLARE_NAMESPACE_TOOL_FACTORY     ( PyAthena, Tool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PyAthena, Alg )
DECLARE_NAMESPACE_AUDITOR_FACTORY  ( PyAthena, Aud )
DECLARE_NAMESPACE_SERVICE_FACTORY  ( PyAthena, Svc )
DECLARE_NAMESPACE_SERVICE_FACTORY  ( PyAthena, PyComponentMgr )
DECLARE_SERVICE_FACTORY  ( PyJobOptionsSvc )

