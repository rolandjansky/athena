/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @file TauDetailsContainerCnv_p1.cxx
///
/// Implementation of transient<->persistent converter
/// for TauDetailsContainer classes
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by on 2007-07-06.
///
/// $Id: TauDetailsContainerCnv_p1.cxx,v 1.4 2009-05-08 14:32:44 tburgess Exp $

#include "tauEventTPCnv/TauDetailsContainerCnv_p1.h"
#include "tauEventTPCnv/TauShotCnv_p1.h"
#include "tauEvent/TauRecDetails.h"
#include "tauEvent/TauRecExtraDetails.h"
#include "tauEvent/Tau1P3PDetails.h"
#include "tauEvent/Tau1P3PExtraDetails.h"

TauDetailsContainerCnv_p1::TauDetailsContainerCnv_p1() :
    T_AthenaPoolTPCnvBase<
    Analysis::TauDetailsContainer,
    TauDetailsContainer_p1>()
{
}

TauDetailsContainerCnv_p1::~TauDetailsContainerCnv_p1()
{
}

void TauDetailsContainerCnv_p1::transToPers(
    const Analysis::TauDetailsContainer *transVect,
    TauDetailsContainer_p1 *persVect,
    MsgStream &msg )
{
    ITPConverterFor<Analysis::TauDetails> *cnv = 0;
    persVect->clear();
    persVect->reserve( transVect->size() );
    for( Analysis::TauDetailsContainer::const_iterator it = transVect->begin();
	 it != transVect->end();  ++it ) {
	if (dynamic_cast<const Analysis::TauRecDetails *>(*it)!=0) continue;
	if (dynamic_cast<const Analysis::TauRecExtraDetails*>(*it)!=0) continue;
	if (dynamic_cast<const Analysis::Tau1P3PDetails*>(*it)!=0) continue;
	if (dynamic_cast<const Analysis::Tau1P3PExtraDetails*>(*it)!=0) continue;
	persVect->push_back(
	    toPersistent(
		&cnv, *it, msg ) );
    }
}

void TauDetailsContainerCnv_p1::persToTrans(
    const TauDetailsContainer_p1 *persVect,
    Analysis::TauDetailsContainer *transVect,
    MsgStream &msg )
{
    ITPConverterFor<Analysis::TauDetails> *cnv = 0;
    transVect->clear();
    transVect->reserve( persVect->size() );
    for( TauDetailsContainer_p1 :: const_iterator it  = persVect->begin();
	 it != persVect->end();  ++it )
    {
      // Bleh --- the list of converters was reordered at one point,
      // a Thing One Should Never Do.
      // Try to hack around this.
      TPObjRef ref = *it;
      ITPConverter* cnvtest = nullptr;
      cnvtest = converterForRef (cnvtest, ref, msg);
      if (dynamic_cast<TauShotCnv_p1*>(cnvtest)) {
        unsigned short tlid = ref.topLevelCnvID();
        unsigned short cnvid = (ref.typeID() & 0xffff);
        ref = TPObjRef (TPObjRef::typeID_t(tlid, cnvid+1), ref.index());
      }

      transVect->push_back(
	    createTransFromPStore(
		&cnv, ref, msg ) );
    }
}
