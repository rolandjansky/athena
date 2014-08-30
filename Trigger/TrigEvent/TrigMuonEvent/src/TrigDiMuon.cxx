/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

  NAME:     TrigDiMuon.cxx
  PACKAGE:  TrigMuonEvent
  
********************************************************************/

#include "TrigMuonEvent/TrigDiMuon.h"
//#include <math.h>

TrigDiMuon::TrigDiMuon() 
:
m_mdt_coll_size(0),
m_rpc_wcoll_size(0),
m_rpc_scoll_size(0),
m_tgc_wcoll_size(0),
m_tgc_scoll_size(0)
{}



TrigDiMuon::TrigDiMuon	(	TrigInDetTrack* track,
				int mdt_coll_size,
				int rpc_wcoll_size,
				int rpc_scoll_size,
				int tgc_wcoll_size,
				int tgc_scoll_size
			)
:
m_track (track),
m_mdt_coll_size(mdt_coll_size),
m_rpc_wcoll_size(rpc_wcoll_size),
m_rpc_scoll_size(rpc_scoll_size),
m_tgc_wcoll_size(tgc_wcoll_size),
m_tgc_scoll_size(tgc_scoll_size)
{}
