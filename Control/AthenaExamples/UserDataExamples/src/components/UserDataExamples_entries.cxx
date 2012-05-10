//$Id: UserDataExamples_entries.cxx,v 1.6 2008-07-07 05:35:52 yyao Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../CreateDataWithUserData.h"
#include "../ReadDataWithUserData.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY( UserDataExamples, CreateDataWithUserData )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( UserDataExamples, ReadDataWithUserData )

DECLARE_FACTORY_ENTRIES(UserDataExamples) {
  DECLARE_NAMESPACE_ALGORITHM( UserDataExamples, CreateDataWithUserData )
  DECLARE_NAMESPACE_ALGORITHM( UserDataExamples, ReadDataWithUserData )
}
