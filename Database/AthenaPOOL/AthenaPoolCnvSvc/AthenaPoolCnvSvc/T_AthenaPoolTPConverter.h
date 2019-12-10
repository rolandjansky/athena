/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLTPCONVERTER_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLTPCONVERTER_H 1

// provide AthenaPoolTopLevelTPCnvBase definition to T/P converters
#include "AthenaPoolTopLevelTPCnvBase.h"

#include "TPTools/TPConverter.h"


template<class TRANS, class PERS>
class T_TPCnv
  : public ITPCnvBase
{
//   // FIXME: we should find a way to properly catch all of those...
//   // T_TPCnv<> should actually be completely retired, i think.
// public:
//   T_TPCnv() : ITPCnvBase()
//   { std::cerr << "::sb:: ctor - [" 
//               << System::typeinfoName(typeid(this))
//               << "]\n";
//   }
};


// define old TP classes as typedef to the new classes

template<class TRANS_BASE, class TRANS, class PERS>
using T_AthenaPoolTPAbstractPolyCnvBase = TPAbstractPolyCnvBase<TRANS_BASE, TRANS, PERS>;

template<class TRANS_BASE, class TRANS, class PERS>
using T_AthenaPoolTPAbstractPolyCnvConstBase = TPAbstractPolyCnvConstBase<TRANS_BASE, TRANS, PERS>;

template<class TRANS_BASE, class TRANS, class PERS>
using  T_AthenaPoolTPPolyCnvBase = TPPolyCnvBase<TRANS_BASE, TRANS, PERS>;

template< class TRANS, class PERS >
using T_AthenaPoolTPCnvBase = TPConverterBase<TRANS, PERS>;

template< class TRANS, class PERS >
using T_AthenaPoolTPCnvConstBase = TPConverterConstBase<TRANS, PERS>;

template< class TRANS, class PERS >
using T_AthenaPoolTPCnvWithKeyBase = TPConverterWithKeyBase<TRANS, PERS>;


template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPPtrVectorCnv = TPPtrVectorCnv<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPPtrVectorCnvConst = TPPtrVectorCnvConst<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPThinnedPtrVectorCnv = TPThinnedPtrVectorCnv<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPPolyVectorCnv = TPPolyVectorCnv<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPPolyVectorCnvConst = TPPolyVectorCnvConst<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvVector = TPCnvVector<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvVectorConst = TPCnvVectorConst<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvStdVector = TPCnvStdVector<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvStdVectorConst = TPCnvStdVectorConst<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvIDCont = TPCnvIDCont<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvIDContFromIdentifier = TPCnvIDContFromIdentifier<TRANS, PERS, CONV>;

#endif
