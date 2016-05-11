/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLTPCONVERTER_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLTPCONVERTER_H 1


#define TPAbstractPolyCnvBase   T_AthenaPoolTPAbstractPolyCnvBase
#define TPPolyCnvBase           T_AthenaPoolTPPolyCnvBase
#define TPConverterBase         T_AthenaPoolTPCnvBase

#define TPCnvVector             T_AthenaPoolTPCnvVector
#define TPCnvStdVector          T_AthenaPoolTPCnvStdVector
#define TPPtrVectorCnv          T_AthenaPoolTPPtrVectorCnv
#define TPPolyVectorCnv         T_AthenaPoolTPPolyVectorCnv

#define TPCnvIDCont             T_AthenaPoolTPCnvIDCont
#define TPCnvIDContFromIdentifier       T_AthenaPoolTPCnvIDContFromIdentifier


// include "AthenaPoolTopLevelTPCnvBase.h" to get #defines only, they need to come first
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


#if 0
// does not work because ARA selection files have TP converter names in them

// define old TP classes as typedef to the new classes
#ifndef __GCCXML__

template<class TRANS_BASE, class TRANS, class PERS>
using T_AthenaPoolTPAbstractPolyCnvBase = TPAbstractPolyCnvBase<TRANS_BASE, TRANS, PERS>;

template<class TRANS_BASE, class TRANS, class PERS>
using  T_AthenaPoolTPPolyCnvBase = TPPolyCnvBase<TRANS_BASE, TRANS, PERS>;

template< class TRANS, class PERS >
using T_AthenaPoolTPCnvBase = TPConverterBase<TRANS, PERS>;


template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPPtrVectorCnv = TPPtrVectorCnv<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPPolyVectorCnv = TPPolyVectorCnv<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvVector = TPCnvVector<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvStdVector = TPCnvStdVector<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvIDCont = TPCnvIDCont<TRANS, PERS, CONV>;

template<class TRANS, class PERS, class CONV>
using T_AthenaPoolTPCnvIDContFromIdentifier = TPCnvIDContFromIdentifier<TRANS, PERS, CONV>;

#else   // __GCCXML__

#define T_AthenaPoolTPAbstractPolyCnvBase TPAbstractPolyCnvBase
#define T_AthenaPoolTPPolyCnvBase TPPolyCnvBase
#define T_AthenaPoolTPCnvBase TPConverterBase
#define T_AthenaPoolTPCnvStdVector TPCnvStdVector
#define T_AthenaPoolTPPtrVectorCnv TPPtrVectorCnv

#endif // __GCCXML__

#endif // 0/1

#endif

