/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_TPHELPERS
#define JETTAGINFOTPCNV_TPHELPERS
///
/// Some helpers for the T/P seperation
///

#include <vector>

namespace Analysis {

  ///
  /// Given a vector of poitners to an object, create the persistent
  /// from the transient.
  ///
  template<class CNV, class CNVForObj, class ObjType>
  void persToTransVectorObj (CNV *masterCvt,
			     CNVForObj **cnv,
			     const std::vector<TPObjRef> &persVector,
			     std::vector<ObjType*> &result,
			     MsgStream &msg)
  {
      result.clear();
      result.reserve(persVector.size());
      for (std::vector<TPObjRef>::const_iterator itr = persVector.begin();
	   itr != persVector.end();
	   itr++) {
	result.push_back(masterCvt->createTransFromPStore(cnv, *itr, msg));
      }
  }

  ///
  /// Translate a set of pointers to objects to their persistent
  /// rep, saving them in the tlp container.
  ///
  template<typename CNV, typename CNVForObj, typename ObjType>
  void transToPersVectorObj(CNV *masterCvt,
			    CNVForObj **cnv,
			    const std::vector<ObjType*> &transVector,
			    std::vector<TPObjRef> &result,
			    MsgStream &msg)
    {
      result.clear();
      result.reserve(transVector.size());
      for (unsigned int i = 0; i < transVector.size(); i++) {
	result.push_back(masterCvt->toPersistent(cnv, transVector[i], msg));
      }
    }
}

#endif
