// $Id$
/**
 * @file D3PDMakerTest/src/components/D3PDMakerTest_entries.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief List Gaudi components.
 */


#include "../FillerAlg.h"
#include "../HitsFillerAlg.h"
#include "../Obj1FillerTool.h"
#include "../Obj2FillerTool.h"
#include "../Obj12FillerTool.h"
#include "../Obj4FillerTool.h"
#include "../Obj4GetterTool.h"
#include "../Obj5FillerTool.h"
#include "../DefaultFillerTool.h"
#include "../DefaultFillerTool2.h"
#include "../Obj1Obj2AssociationTool.h"
#include "../Obj1Obj2MultiAssociationTool.h"
#include "../ToObj2AssociationTool.h"
#include "../ToObj2MultiAssociationTool.h"
#include "../IN4MHLVAssociationTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PDTest, FillerAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PDTest, HitsFillerAlg)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj1FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj2FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj12FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj4FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj4GetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj5FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, DefaultFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, DefaultFillerTool2)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj1Obj2AssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, Obj1Obj2MultiAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, ToObj2AssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, ToObj2MultiAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PDTest, IN4MHLVAssociationTool)


DECLARE_FACTORY_ENTRIES(D3PDMakerTest) {
  DECLARE_NAMESPACE_ALGORITHM (D3PDTest, FillerAlg)
  DECLARE_NAMESPACE_ALGORITHM (D3PDTest, HitsFillerAlg)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj1FillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj2FillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj12FillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj4FillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj4GetterTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj5FillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, DefaultFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, DefaultFillerTool2)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj1Obj2AssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, Obj1Obj2MultiAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, ToObj2AssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, ToObj2MultiAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PDTest, IN4MHLVAssociationTool)
}
