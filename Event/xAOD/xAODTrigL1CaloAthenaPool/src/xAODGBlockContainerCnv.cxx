/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODGBlockContainerCnv.h"

namespace
{
  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink(SG::AuxVectorBase *cont, const std::string &key)
  {

    // The link to set up:
    DataLink<SG::IConstAuxStore> link(key + "Aux.");

    // Give it to the container:
    cont->setStore(link);

    return;
  }
} // namespace

xAODGBlockContainerCnv::xAODGBlockContainerCnv(ISvcLocator *svcLoc)
    : xAODGBlockContainerCnvBase(svcLoc)
{
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODGBlockContainerCnv::createObj(IOpaqueAddress *pAddr, DataObject *&pObj)
{

  // Get the key of the container that we'll be creating:
  m_key = *(pAddr->par() + 1);

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj(pAddr, pObj);
}

xAOD::GBlockContainer *
xAODGBlockContainerCnv::createPersistent(xAOD::GBlockContainer *trans)
{

  // Create a view copy of the container:
  xAOD::GBlockContainer *result =
      new xAOD::GBlockContainer(trans->begin(), trans->end(),
                                SG::VIEW_ELEMENTS);

  // Prepare the objects to be written out:
  xAOD::GBlockContainer::iterator itr = result->begin();
  xAOD::GBlockContainer::iterator end = result->end();
  for (; itr != end; ++itr)
  {
    toPersistent(*itr);
  }

  // Return the new container:
  return result;
}

xAOD::GBlockContainer *xAODGBlockContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid guid("C199624A-2DFE-4FD5-A3C7-26C381BE811B");

  // Check if we're reading the most up to date type:
  if (compareClassGuid(guid))
  {
    xAOD::GBlockContainer *c = poolReadObject<xAOD::GBlockContainer>();
    setStoreLink(c, m_key);
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error("Unsupported version of "
                           "xAOD::GBlockContainer found");
  return 0;
}

void xAODGBlockContainerCnv::
    toPersistent(xAOD::GBlock * /*cluster*/) const
{

  return;
}
