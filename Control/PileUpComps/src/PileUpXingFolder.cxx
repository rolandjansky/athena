/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IProperty.h"
#include <exception>
#include "PileUpXingFolder.h"
PileUpXingFolder::PileUpXingFolder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent) :
  base_class(type, name, parent),
  m_folder( "SG::Folder/SGF", this ),
  m_firstXing(-999),
  m_lastXing(999),
  m_itemList(),
  m_cacheRefreshFrequency(0.0)
{
  declareProperty("FirstXing", m_firstXing);
  declareProperty("LastXing", m_lastXing);
  declareProperty("ItemList", m_itemList);
  m_cacheRefreshFrequency.verifier().setBounds(0.0, 1.0);
  declareProperty("CacheRefreshFrequency", m_cacheRefreshFrequency,
                  "frequency with which data objs in this folder should be cleared from bkg caches at end of event. Default 0 (never clear), range [0.0, 1.0]");
}

StatusCode PileUpXingFolder::initialize() {
  if (!(m_folder.retrieve()).isSuccess()) return StatusCode::FAILURE;
  return (dynamic_cast<IProperty&>(*m_folder)).setProperty(m_itemList);
}
