/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGEFMISSINGET_PUCLASSIFICATION_H
#define TRIGEFMISSINGET_PUCLASSIFICATION_H

#include <type_traits>

namespace HLT
{
  namespace MET
  {
    namespace PUClassification
    {
      enum PUClassification
      {
        NeutralForward = 1 << 0,
        ChargedPU = 1 << 1,
        ChargedHS = 1 << 2,
      };
      constexpr PUClassification NFcPU = static_cast<PUClassification>(NeutralForward | ChargedPU);
      constexpr PUClassification NFcHS = static_cast<PUClassification>(NeutralForward | ChargedHS);
      constexpr PUClassification Charged = static_cast<PUClassification>(ChargedPU | ChargedHS);
      constexpr PUClassification All = static_cast<PUClassification>(NeutralForward | ChargedPU | ChargedHS);
    } // namespace PUClassification
  }   // namespace MET
} // namespace HLT

#endif //> !TRIGEFMISSINGET_PUCLASSIFICATION_H