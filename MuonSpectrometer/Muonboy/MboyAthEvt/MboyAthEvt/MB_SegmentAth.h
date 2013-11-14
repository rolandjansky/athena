/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_SegmentAth_H
#define MB_SegmentAth_H

namespace Trk {
  class  PrepRawData;
  class  Surface;
}

namespace Muon {
  class  CscPrepData;
  class  MdtPrepData;
  class  RpcPrepData;
  class  TgcPrepData;
  class  CscStripPrepData;
}


/////////////////////////////////////////////////////////////////////////////
#include "MboyAthEvt/MboyAthEvtMisc.h"
   
#include "MboyCoreEvt/MB_Segment.h"

class MuonRecDigiStoreAth;

  /**
   @class MB_SegmentAth

   This class is managing the segment data. It derives from MB_Segment
   to which are added connections to PrepData.
   
   A MB_SegmentAth is built from a preexisting MB_Segment and 
   a MuonRecDigiStoreAth. Indexes to associated MuonRecDigit digit
   permit to retrieve from MuonRecDigiStoreAth and to store in MB_SegmentAth
   the pointers to PrepData 

  @author samusog@cern.ch
  
  */

class MB_SegmentAth: public MB_Segment{
public:
  MB_SegmentAth();
  MB_SegmentAth(const MB_Segment & oMB_Segment,
                const MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                );
  virtual ~MB_SegmentAth();

public:
  ///////////////////////////////////

  /** Get  Surface */
  const Trk::Surface* LiIdGetSurface        (int LinearNber) const;

  /** Get  PrepData */
  const Trk::PrepRawData*       LiIdGetPrepRawData            (int LinearNber) const;

  /** Get  MdtPrepData */
  const Muon::MdtPrepData*      LiIdGetMdtPrepData             (int LinearNber) const;

  /** Get  RpcPrepData */
  const Muon::RpcPrepData*      LiIdGetRpcPrepData             (int LinearNber) const;

  /** Get  TgcPrepData */
  const Muon::TgcPrepData*      LiIdGetTgcPrepData             (int LinearNber) const;

  /** Get  CscStripPrepData */
  const Muon::CscStripPrepData* LiIdGetCscStripPrepData        (int LinearNber) const;

  /** Get  CscPrepData */
  const Muon::CscPrepData*      LiIdGetCscClusterPrepData      (int LinearNber) const;

private:
///////////////////////////////////
//Data

  std::vector<const Muon::MdtPrepData*>       MdtPrepDataVector         ; //!<  Storage of associated PrepData
  std::vector<const Muon::RpcPrepData*>       RpcPrepDataVector         ; //!<  Storage of associated PrepData
  std::vector<const Muon::TgcPrepData*>       TgcPrepDataVector         ; //!<  Storage of associated PrepData
  std::vector<const Muon::CscStripPrepData*>  CscStripPrepDataVector    ; //!<  Storage of associated PrepData
  std::vector<const Muon::CscPrepData*>       CscCluPrepDataVector      ; //!<  Storage of associated PrepData

};
#endif
