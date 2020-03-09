// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaServices/src/AthReadAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Generic algorithm wrapping an input conversion.
 */


#ifndef ATHENASERVICES_ATHREADALG_H
#define ATHENASERVICES_ATHREADALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/TypelessWriteHandleKey.h"
#include "AthenaKernel/sgkey_t.h"


/**
 * @brief Generic algorithm wrapping an input conversion.
 *
 * Normally, objects read from the input file are created lazily, when they're
 * first referenced.  There are cases, however, where we'd like the conversion
 * to be schedulable, eg, wrapped by an algorithm.  This class provides
 * a generic algorithm that will wrap reading an object from an input file.
 *
 * The Key property should be set to the object to be read and the algorithm
 * added to topSequence:
 *
 *@code
 *  from AthenaServices.AthenaServicesConf import AthReadAlg
 *  topSequence += AthReadAlg ('fooCnv', Key = 'ObjType/foo')
 @endcode
 *
 * With that declaration, the object ObjType/foo will no longer be lazily
 * readable, but will instead be produced by the algorithm.  If needed,
 * additional algorithm dependencies may be added via the ExtraInputs property.
 */
class AthReadAlg
  : public AthReentrantAlgorithm
{
public:
  /// Use base class ctor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /**
   * @brief Standard initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Standard Algorithm execute method.
   * @param ctx Event context.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  /// Property: Output key.
  SG::TypelessWriteHandleKey m_key { this, "Key", CLID_NULL, "", "" };

  Gaudi::Property<std::vector<std::string> > m_aliases
  { this, "Aliases", {}, "Aliases to create for this object." };

  /// Cache sgkey of proxy corresponding to the original object.
  SG::sgkey_t m_sgkey;
};


#endif // not ATHENASERVICES_ATHREADALG_H
