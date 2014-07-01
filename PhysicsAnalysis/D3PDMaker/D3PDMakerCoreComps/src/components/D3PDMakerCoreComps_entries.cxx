// $Id$
/**
 * @file D3PDMakerCoreComps/src/components/D3PDMakerCoreComps_entries.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief List Gaudi components.
 */


#include "../MakerAlg.h"
#include "../DummyInitAlg.h"
#include "../DummyVarFillerTool.h"
#include "../CollectionGetterRegistryTool.h"
#include "../ObjFillerTool.h"
#include "../VoidObjFillerTool.h"
#include "../VectorFillerTool.h"
#include "../ContainedAssociationFillerTool.h"
#include "../IndexAssociationFillerTool.h"
#include "../IndexFillerTool.h"
#include "../ContainedMultiAssociationFillerTool.h"
#include "../ContainedVectorMultiAssociationFillerTool.h"
#include "../IndexMultiAssociationFillerTool.h"
#include "../SGObjGetterTool.h"
#include "../SGDataVectorGetterTool.h"
#include "../UserDataFillerTool.h"
#include "../AuxDataFillerTool.h"
#include "../SelectedParticlesFilterTool.h"
#include "../ContainerFlagFillerTool.h"
#include "../IdentityAssociationTool.h"
#include "../FirstAssociationTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, MakerAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, DummyInitAlg)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, DummyVarFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CollectionGetterRegistryTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ObjFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VoidObjFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, VectorFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ContainedAssociationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, IndexAssociationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, IndexFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ContainedMultiAssociationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ContainedVectorMultiAssociationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, IndexMultiAssociationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SGObjGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SGDataVectorGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, UserDataFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, AuxDataFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SelectedParticlesFilterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ContainerFlagFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, IdentityAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, FirstAssociationTool)


DECLARE_FACTORY_ENTRIES(D3PDMakerCoreComps) {
  DECLARE_NAMESPACE_ALGORITHM (D3PD, MakerAlg)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, DummyInitAlg)
  DECLARE_NAMESPACE_TOOL   (D3PD, DummyVarFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, CollectionGetterRegistryTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, ObjFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, VoidObjFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, VectorFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, ContainedAssociationFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, IndexAssociationFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, IndexFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, ContainedMultiAssociationFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, ContainedVectorMultiAssociationFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, IndexMultiAssociationFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, SGObjGetterTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, SGDataVectorGetterTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, UserDataFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, AuxDataFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, SelectedParticlesFilterTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, ContainerFlagFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, IdentityAssociationTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, FirstAssociationTool)
}
