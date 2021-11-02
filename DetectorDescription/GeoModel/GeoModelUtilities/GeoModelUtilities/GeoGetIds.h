// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeoModelUtilities/GeoGetIds.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Visitor to collect all IDs under a GeoModel node.
 */


#ifndef GEOMODELUTILITIES_GEOGETIDS_H
#define GEOMODELUTILITIES_GEOGETIDS_H

/// Ensure that the extensions for the Vector3D are properly loaded
#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoNodeAction.h"


/**
 * @brief Define a simple callback for each ID found.
 *        It will probably be more convenient to use the
 *        templated versions below.
 */
class IGeoGetIdsAction
{
public:
  virtual ~IGeoGetIdsAction() = default;
  virtual void operator() (int id) = 0;
};


/**
 * @brief Visitor to traverse a GeoModel graph and make a callback
 *        for each identifier found.
 */
class GeoGetIds
  : public GeoNodeAction
{
public:
  GeoGetIds (IGeoGetIdsAction& action, int depthLimit = 1);
  virtual void handleSerialTransformer (const GeoSerialTransformer  *sT) override;
  virtual void handleIdentifierTag (const GeoIdentifierTag *idTag) override;
  virtual void handleSerialIdentifier(const GeoSerialIdentifier *sI) override;


private:
  IGeoGetIdsAction& m_action;
  const GeoSerialIdentifier *m_serialIdentifier = nullptr;
};


/**
 * @brief Callback object calling a templated functional.
 */
template <class FUNCTION>
class GeoGetIdsAction
  : public IGeoGetIdsAction
{
public:
  GeoGetIdsAction (FUNCTION f) : m_f (f) {}
  virtual void operator() (int id) override
  {
    m_f (id);
  }
  FUNCTION m_f;
};



/**
 * @brief Template helper for running the visitor.  Example:
 *@code
 *  GeoFullPhysVol* pv = ...;
 *  std::vector<int> ids;
 *  geoGetIds ([&](int id) { ids.push_back (id); }, pv);
 @endcode
 */
template <class FUNCTION>
void geoGetIds (FUNCTION f, const GeoGraphNode* node, int depthLimit = 1)
{
  GeoGetIdsAction<FUNCTION> act (f);
  GeoGetIds visitor (act, depthLimit);
  node->exec (&visitor);
}


#endif // not GEOMODELUTILITIES_GEOGETIDS_H
