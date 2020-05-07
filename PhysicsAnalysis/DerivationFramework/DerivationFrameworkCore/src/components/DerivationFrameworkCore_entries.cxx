#include "DerivationFrameworkCore/DerivationKernel.h"
#include "DerivationFrameworkCore/CommonAugmentation.h"
#include "DerivationFrameworkCore/TriggerMatchingAugmentation.h"

DECLARE_COMPONENT( DerivationFramework::DerivationKernel )
DECLARE_COMPONENT( DerivationFramework::CommonAugmentation )

#include "../GoodRunsListFilterAlgorithm.h"
DECLARE_COMPONENT( DerivationFramework::GoodRunsListFilterAlgorithm )
DECLARE_COMPONENT( DerivationFramework::TriggerMatchingAugmentation  )
