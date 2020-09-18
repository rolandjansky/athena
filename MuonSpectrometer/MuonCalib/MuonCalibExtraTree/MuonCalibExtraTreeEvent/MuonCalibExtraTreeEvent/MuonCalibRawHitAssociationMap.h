/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBRAWHITASSICATIONMAP_H
#define MUONCALIBRAWHITASSICATIONMAP_H

#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "CxxUtils/checker_macros.h"

#include <map>

namespace MuonCalib {
  
  /**
     @class MuonCalibRawHitAssociationMap

     Association map between MuonFixedId and raw hits

     @author niels.van.eldik@cern.ch
  */
  class ATLAS_NOT_THREAD_SAFE MuonCalibRawHitAssociationMap  {
  public:
    /** get raw csc hit for given Identifier, returns 0 if not on track */
    const MuonCalibRawCscHit* getRawCscHit( const MuonFixedId& id ) const;

    /** get raw mdt hit for given Identifier, returns 0 if not on track */
    const MuonCalibRawMdtHit* getRawMdtHit( const MuonFixedId& id ) const;

    /** get raw tgc hit for given Identifier, returns 0 if not on track */
    const MuonCalibRawTgcHit* getRawTgcHit( const MuonFixedId& id ) const;

    /** get raw rpc hit for given Identifier, returns 0 if not on track */
    const MuonCalibRawRpcHit* getRawRpcHit( const MuonFixedId& id ) const;

    /** add associated raw mdt hit */
    void addMdtRawHit( const MuonCalibRawMdtHit* hit ) const { if( !hit ) return; m_associatedRawMdtHits[hit->identify()] = hit; }

    /** add associated raw csc hit */
    void addCscRawHit( const MuonCalibRawCscHit* hit ) const { if( !hit ) return; m_associatedRawCscHits[hit->identify()] = hit; }

    /** add associated raw rpc hit */
    void addRpcRawHit( const MuonCalibRawRpcHit* hit ) const { if( !hit ) return; m_associatedRawRpcHits[hit->identify()] = hit; }

    /** add associated raw tgc hit */
    void addTgcRawHit( const MuonCalibRawTgcHit* hit ) const { if( !hit ) return; m_associatedRawTgcHits[hit->identify()] = hit; }

  private:
    /** maps of raw hits on track */
    mutable std::map<MuonFixedId,const MuonCalibRawCscHit*> m_associatedRawCscHits;
    mutable std::map<MuonFixedId,const MuonCalibRawMdtHit*> m_associatedRawMdtHits;
    mutable std::map<MuonFixedId,const MuonCalibRawRpcHit*> m_associatedRawRpcHits;
    mutable std::map<MuonFixedId,const MuonCalibRawTgcHit*> m_associatedRawTgcHits;

  };

  inline const MuonCalibRawCscHit* MuonCalibRawHitAssociationMap::getRawCscHit( const MuonFixedId& id ) const {
    std::map<MuonFixedId,const MuonCalibRawCscHit*>::const_iterator it = m_associatedRawCscHits.find(id);
    if( it!= m_associatedRawCscHits.end() ) return it->second;
    return 0;
  }

  inline const MuonCalibRawMdtHit* MuonCalibRawHitAssociationMap::getRawMdtHit( const MuonFixedId& id ) const {
    std::map<MuonFixedId,const MuonCalibRawMdtHit*>::const_iterator it = m_associatedRawMdtHits.find(id);
    if( it!= m_associatedRawMdtHits.end() ) return it->second;
    return 0;
  }

  inline const MuonCalibRawRpcHit* MuonCalibRawHitAssociationMap::getRawRpcHit( const MuonFixedId& id ) const {
    std::map<MuonFixedId,const MuonCalibRawRpcHit*>::const_iterator it = m_associatedRawRpcHits.find(id);
    if( it!= m_associatedRawRpcHits.end() ) return it->second;
    return 0;
  }

  inline const MuonCalibRawTgcHit* MuonCalibRawHitAssociationMap::getRawTgcHit( const MuonFixedId& id ) const {
    std::map<MuonFixedId,const MuonCalibRawTgcHit*>::const_iterator it = m_associatedRawTgcHits.find(id);
    if( it!= m_associatedRawTgcHits.end() ) return it->second;
    return 0;
  }

}

#endif
