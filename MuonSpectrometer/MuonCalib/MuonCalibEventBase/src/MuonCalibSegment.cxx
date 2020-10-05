/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibStl/DeleteObject.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <algorithm>
#include <iostream>

namespace MuonCalib {

  MuonCalibSegment::MuonCalibSegment(double chi2, const Amg::Vector3D &pos, const Amg::Vector3D &dir,
				     const Amg::Transform3D &locToGlo, unsigned int qualityFlag ) 
    :  m_chi2(chi2), m_dy0(0), m_dtheta(0), m_localPosition(pos),m_localDirection(dir),
       m_localToGlobal(locToGlo), m_fittedT0(-99999.), m_qualityFlag(qualityFlag), m_author(-1)
  {}

  MuonCalibSegment::~MuonCalibSegment() {
    // MuonCalibSegments owns MdtCalibHitBases
    std::for_each( mdtHOTBegin(), mdtHOTEnd(), DeleteObject() );
    std::for_each( mdtCloseHitsBegin(), mdtCloseHitsEnd(), DeleteObject() );

    // MuonCalibSegments owns CscCalibHitBases
    std::for_each( cscHOTBegin(), cscHOTEnd(), DeleteObject() );
    std::for_each( cscCloseHitsBegin(), cscCloseHitsEnd(), DeleteObject() ); 

    // MuonCalibSegments owns RpcCalibHitBases
    std::for_each( rpcHOTBegin(), rpcHOTEnd(), DeleteObject() );
    std::for_each( rpcCloseHitsBegin(), rpcCloseHitsEnd(), DeleteObject() );

    // MuonCalibSegments owns TgcCalibHitBases
    std::for_each( tgcHOTBegin(), tgcHOTEnd(), DeleteObject() );
    std::for_each( tgcCloseHitsBegin(), tgcCloseHitsEnd(), DeleteObject() );
  }

  MuonCalibSegment::MuonCalibSegment(const MuonCalibSegment &seg) {
    m_chi2           = seg.chi2();
    m_dy0            = seg.error_dy0();
    m_dtheta         = seg.error_dtheta();
    m_localPosition  = seg.position();
    m_localDirection = seg.direction();
    m_localToGlobal  = seg.localToGlobal();
    m_qualityFlag    = seg.qualityFlag();
    m_author         = seg.author();
    m_fittedT0       = seg.m_fittedT0; // don't use function as it would return 0 if m_fittedT0 == -99999.

    // Add MdtCalibHits
    MdtHitCit mdt_it     = seg.mdtHOTBegin();
    MdtHitCit mdt_it_end = seg.mdtHOTEnd();
    for( ; mdt_it!=mdt_it_end;++mdt_it){
      addHitOnTrack( new MdtCalibHitBase(**mdt_it));
    }

    mdt_it     = seg.mdtCloseHitsBegin();
    mdt_it_end = seg.mdtCloseHitsEnd();
    for( ; mdt_it!=mdt_it_end;++mdt_it){
      addCloseHit( new MdtCalibHitBase(**mdt_it));
    }

    // Add CscCalibHits
    CscHitCit csc_it     = seg.cscHOTBegin();
    CscHitCit csc_it_end = seg.cscHOTEnd();
    for( ; csc_it!=csc_it_end;++csc_it){
      addHitOnTrack( new CscCalibHitBase(**csc_it));
    }

    csc_it     = seg.cscCloseHitsBegin();
    csc_it_end = seg.cscCloseHitsEnd();
    for( ; csc_it!=csc_it_end;++csc_it){
      addCloseHit( new CscCalibHitBase(**csc_it));
    }
    
    // Add RpcCalibHits
    RpcHitCit rpc_it     = seg.rpcHOTBegin();
    RpcHitCit rpc_it_end = seg.rpcHOTEnd();
    for( ; rpc_it!=rpc_it_end;++rpc_it){
      addHitOnTrack( new RpcCalibHitBase(**rpc_it));
    }

    rpc_it     = seg.rpcCloseHitsBegin();
    rpc_it_end = seg.rpcCloseHitsEnd();
    for( ; rpc_it!=rpc_it_end;++rpc_it){
      addCloseHit( new RpcCalibHitBase(**rpc_it));
    }
    
    // Add TgcCalibHit
    TgcHitCit tgc_it     = seg.tgcHOTBegin();
    TgcHitCit tgc_it_end = seg.tgcHOTEnd();
    for( ; tgc_it!=tgc_it_end;++tgc_it){
      addHitOnTrack( new TgcCalibHitBase(**tgc_it));
    }

    tgc_it     = seg.tgcCloseHitsBegin();
    tgc_it_end = seg.tgcCloseHitsEnd();
    for( ; tgc_it!=tgc_it_end;++tgc_it){
      addCloseHit( new TgcCalibHitBase(**tgc_it));
    }
  }  //end MuonCalibSegment::MuonCalibSegment

  MuonCalibSegment& MuonCalibSegment::operator=( const MuonCalibSegment &seg) {
    if (this!=&seg) {
      m_chi2           = seg.chi2();
      m_dy0            = seg.error_dy0();
      m_dtheta         = seg.error_dtheta();
      m_localPosition  = seg.position();
      m_localDirection = seg.direction();
      m_localToGlobal  = seg.localToGlobal();
      m_qualityFlag    = seg.qualityFlag();
      m_author         = seg.author();
      m_fittedT0       = seg.m_fittedT0; // don't use function as it would return 0 if m_fittedT0 == -99999.

      std::for_each( mdtHOTBegin(), mdtHOTEnd(), DeleteObject() );
      m_mdtHitsOnTrack.clear();
      MdtHitCit mdt_it     = seg.mdtHOTBegin();
      MdtHitCit mdt_it_end = seg.mdtHOTEnd();
      for( ; mdt_it!=mdt_it_end;++mdt_it){
        addHitOnTrack( new MdtCalibHitBase(**mdt_it));
      }

      std::for_each( mdtCloseHitsBegin(), mdtCloseHitsEnd(), DeleteObject() );
      m_mdtCloseHits.clear();
      mdt_it     = seg.mdtCloseHitsBegin();
      mdt_it_end = seg.mdtCloseHitsEnd();
      for( ; mdt_it!=mdt_it_end;++mdt_it){
        addCloseHit( new MdtCalibHitBase(**mdt_it));
      }

      std::for_each( cscHOTBegin(), cscHOTEnd(), DeleteObject() );
      m_cscHitsOnTrack.clear();
      CscHitCit csc_it     = seg.cscHOTBegin();
      CscHitCit csc_it_end = seg.cscHOTEnd();
      for( ; csc_it!=csc_it_end;++csc_it){
        addHitOnTrack( new CscCalibHitBase(**csc_it));
      }

      std::for_each( cscCloseHitsBegin(), cscCloseHitsEnd(), DeleteObject() ); 
      m_cscCloseHits.clear();
      csc_it     = seg.cscCloseHitsBegin();
      csc_it_end = seg.cscCloseHitsEnd();
      for( ; csc_it!=csc_it_end;++csc_it){
        addCloseHit( new CscCalibHitBase(**csc_it));
      }

      std::for_each( rpcHOTBegin(), rpcHOTEnd(), DeleteObject() );
      m_rpcHitsOnTrack.clear();
      RpcHitCit rpc_it     = seg.rpcHOTBegin();
      RpcHitCit rpc_it_end = seg.rpcHOTEnd();
      for( ; rpc_it!=rpc_it_end;++rpc_it){
        addHitOnTrack( new RpcCalibHitBase(**rpc_it));
      }

      std::for_each( rpcCloseHitsBegin(), rpcCloseHitsEnd(), DeleteObject() );
      m_rpcCloseHits.clear();
      rpc_it     = seg.rpcCloseHitsBegin();
      rpc_it_end = seg.rpcCloseHitsEnd();
      for( ; rpc_it!=rpc_it_end;++rpc_it){
        addCloseHit( new RpcCalibHitBase(**rpc_it));
      }

      std::for_each( tgcHOTBegin(), tgcHOTEnd(), DeleteObject() );
      m_tgcHitsOnTrack.clear();
      TgcHitCit tgc_it     = seg.tgcHOTBegin();
      TgcHitCit tgc_it_end = seg.tgcHOTEnd();
      for( ; tgc_it!=tgc_it_end;++tgc_it){
        addHitOnTrack( new TgcCalibHitBase(**tgc_it));
      }

      std::for_each( tgcCloseHitsBegin(), tgcCloseHitsEnd(), DeleteObject() );
      m_tgcCloseHits.clear();
      tgc_it     = seg.tgcCloseHitsBegin();
      tgc_it_end = seg.tgcCloseHitsEnd();
      for( ; tgc_it!=tgc_it_end;++tgc_it){
        addCloseHit( new TgcCalibHitBase(**tgc_it));
      }
    }
    return *this;
  }  //end MuonCalibSegment::operator=
  
  unsigned int MuonCalibSegment::hitsPerML(int ML)const {
    int counter = 0;
    std::vector<MdtCalibHitBase*>::const_iterator hit_it = mdtHOTBegin();
    for (; hit_it !=mdtHOTEnd(); ++hit_it) {
      if(((*hit_it)->identify()).mdtMultilayer() == ML ) ++counter;
    }
    return counter;
  }

  std::ostream& MuonCalibSegment::dump( std::ostream& stream) const {
    stream << "MuonCalibSegment with chi2 " << chi2() << std::endl;
    stream << " -- local position " << position() << " direction " << direction() << std::endl;
    stream << " -- HitsOnTrack " << hitsOnTrack() << std::endl;
  
    // Dump MdtCalibHit
    MdtHitCit mdt_it = mdtHOTBegin();
    for( ; mdt_it!=mdtHOTEnd(); ++mdt_it){
      (*mdt_it)->dump(stream);
    }
    stream << " -- CloseHits " << mdtCloseHits() << std::endl;
    mdt_it = mdtCloseHitsBegin();
    for( ; mdt_it!=mdtCloseHitsEnd(); ++mdt_it){
      (*mdt_it)->dump(stream);
    }

    // Dump CscCalibHit
    CscHitCit csc_it = cscHOTBegin();
    for( ; csc_it!=cscHOTEnd(); ++csc_it){
      (*csc_it)->dump(stream);
    }
    stream << " -- CloseHits " << cscCloseHits() << std::endl;
    csc_it = cscCloseHitsBegin();
    for( ; csc_it!=cscCloseHitsEnd(); ++csc_it){
      (*csc_it)->dump(stream);
    }

    // Dump RpcCalibHit
    RpcHitCit rpc_it = rpcHOTBegin();
    for( ; rpc_it!=rpcHOTEnd(); ++rpc_it){
      (*rpc_it)->dump(stream);
    }
    stream << " -- CloseHits " << rpcCloseHits() << std::endl;
    rpc_it = rpcCloseHitsBegin();
    for( ; rpc_it!=rpcCloseHitsEnd(); ++rpc_it){
      (*rpc_it)->dump(stream);
    }

    // Dump TgcCalibHit
    TgcHitCit tgc_it = tgcHOTBegin();
    for( ; tgc_it!=tgcHOTEnd(); ++tgc_it){
      (*tgc_it)->dump(stream);
    }
    stream << " -- CloseHits " << tgcCloseHits() << std::endl;
    tgc_it = tgcCloseHitsBegin();
    for( ; tgc_it!=tgcCloseHitsEnd(); ++tgc_it){
      (*tgc_it)->dump(stream);
    }

    return stream;
  }  //end MuonCalibSegment::dump

  void MuonCalibSegment::refineMdtSelection(const std::vector<unsigned int> &new_selection) {
    if(new_selection.size() != m_mdtHitsOnTrack.size()) {
      MsgStream log(Athena::getMessageSvc(),"MuonCalibSegment");
      log<<MSG::WARNING<<"MuonCalibSegment::refineMdtSelection: Wrong size of vector!"<<endmsg;
      return;
    }
//copy old hit vector
    MdtHitVec old_hit_vec(m_mdtHitsOnTrack);
    m_mdtHitsOnTrack.clear();
    for(unsigned int i=0; i<old_hit_vec.size(); i++) {
      if(!new_selection[i]) {
	m_mdtHitsOnTrack.push_back(old_hit_vec[i]);
      } else {
	m_mdtCloseHits.push_back(old_hit_vec[i]);
      }
    }
  }  //end MuonCalibSegment::refineMdtSelection

}  //namespace MuonCalib

std::ostream& operator << (std::ostream &stream, const MuonCalib::MuonCalibSegment &seg) {
  return seg.dump(stream);
}
