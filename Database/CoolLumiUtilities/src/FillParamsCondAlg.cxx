/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/FillParamsCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>, from existing FillParamsTool.
 * @date May, 2019
 * @brief Conditions algorithm to unpack fill parameters from COOL.
 */


#include "FillParamsCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoralBase/Blob.h"
#include "CoolKernel/IObject.h"
#include <stdint.h>

#ifdef __linux__
#include <endian.h>
static_assert (__BYTE_ORDER == __LITTLE_ENDIAN,
               "FillParamsCondAlg assumes little-endian byte ordering.");
#endif


/**
 * @brief Gaudi initialize method.
 */
StatusCode
FillParamsCondAlg::initialize()
{
  ATH_CHECK( m_fillParamsFolderInputKey.initialize() );
  ATH_CHECK( m_fillParamsOutputKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 * @param ctx Event Context.
 */
StatusCode
FillParamsCondAlg::execute (const EventContext& ctx) const
{
  SG::ReadCondHandle<AthenaAttributeList> fillParamsFolder
    (m_fillParamsFolderInputKey, ctx);
  EventIDRange range;
  ATH_CHECK( fillParamsFolder.range (range) );

  if ((**fillParamsFolder)["BCIDmasks"].isNull()) {
    ATH_MSG_ERROR( "BunchCode is NULL in " << m_fillParamsFolderInputKey.key() << "!" );
    return StatusCode::FAILURE;
  }

  // Do everything here for now, but should copy this to a vanilla object based on attrList
  cool::UInt32 nb1 = (**fillParamsFolder)["Beam1Bunches"].data<cool::UInt32>();
  cool::UInt32 nb2 = (**fillParamsFolder)["Beam2Bunches"].data<cool::UInt32>();
  cool::UInt32 ncol = (**fillParamsFolder)["LuminousBunches"].data<cool::UInt32>();

  ATH_MSG_DEBUG( "Beam1Bunches: " << nb1 );
  ATH_MSG_DEBUG( "Beam2Bunches: " << nb2 );
  ATH_MSG_DEBUG( "LuminousBunches: " << ncol );

  const coral::Blob& blob = (**fillParamsFolder)["BCIDmasks"].data<coral::Blob>();

  // Verify length
  if ( static_cast<cool::UInt32>( blob.size() ) != 2 * (nb1 + nb2 + ncol)) {
    ATH_MSG_WARNING( "BCIDmasks length " << blob.size() << " != 2 * " << (nb1+nb2+ncol) );
    return StatusCode::SUCCESS;
  }

  const uint16_t* p=static_cast<const uint16_t*>(blob.startingAddress());

  auto fp = std::make_unique<FillParamsCondData>();

  // Decode beam1 list
  fp->setBeam1Bunches (p, p+nb1);
  p += nb1;

  // Decode beam2 list
  fp->setBeam2Bunches (p, p+nb2);
  p += nb2;

  // Decode luminous list
  fp->setLuminousBunches (p, p+ncol);
  p += ncol;

  SG::WriteCondHandle<FillParamsCondData> fillParamsData
    (m_fillParamsOutputKey, ctx);
  ATH_CHECK( fillParamsData.record (range, std::move (fp)) );
  return StatusCode::SUCCESS;
}
