// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CaloIdentifier/CaloHelpersTest.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2018
 * @brief Helper to initialize ID helpers for unit tests.
 */


#ifndef CALOIDENTIFIER_CALOHELPERSTEST_H
#define CALOIDENTIFIER_CALOHELPERSTEST_H


#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include <memory>


class IdDictParser;


/**
 * @brief Helper to initialize ID helpers for unit tests.
 *
 * An object of this class can be created in a unit test in order to provide
 * initialized instances of the ID helpers.
 */
class CaloHelpersTest
{
public:
  CaloHelpersTest();
  ~CaloHelpersTest();

  const LArEM_ID&        emID() const;
  const LArHEC_ID&       hecID() const;
  const LArFCAL_ID&      fcalID() const;
  const LArMiniFCAL_ID&  minifcalID() const;
  const TileID&          tileID() const;
  const CaloCell_ID&     caloID() const;

private:
  std::unique_ptr<IdDictParser> m_parser;
  LArEM_ID                      m_em_idHelper;
  LArHEC_ID                     m_hec_idHelper;
  LArFCAL_ID                    m_fcal_idHelper;
  LArMiniFCAL_ID                m_minifcal_idHelper;
  TileID                        m_tile_idHelper;
  std::unique_ptr<CaloCell_ID>  m_calo_idHelper;
};


#endif // not CALOIDENTIFIER_CALOHELPERSTEST_H
