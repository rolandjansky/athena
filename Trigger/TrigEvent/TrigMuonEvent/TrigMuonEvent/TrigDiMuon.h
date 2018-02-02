/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef TRIGMUON_TRIGDIMUON_H
#define TRIGMUON_TRIGDIMUON_H

/********************************************************************

NAME:     TrigDiMuon.h

PURPOSE:  tracks passed MS identification

********************************************************************/
#include "AthenaKernel/CLASS_DEF.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include <vector>

class TrigDiMuon
{

 public:

  TrigDiMuon();

  TrigDiMuon( TrigInDetTrack* track, 
		int mdt_coll_size,
		int rpc_wcoll_size,
		int rpc_scoll_size,
		int tgc_wcoll_size,
		int tgc_scoll_size);

  int Mdtsize() const { return m_mdt_coll_size; }
  int Rpcwire_size() const { return m_rpc_wcoll_size; }
  int Rpcstrip_size() const { return m_rpc_scoll_size; }
  int Tgcwire_size() const { return m_tgc_wcoll_size; }
  int Tgcstrip_size() const { return m_tgc_scoll_size; }
	
  TrigInDetTrack* get_InDet() const { return m_track; }

  private:
	TrigInDetTrack* m_track;
	int m_mdt_coll_size;
	int m_rpc_wcoll_size;
	int m_rpc_scoll_size;
	int m_tgc_wcoll_size;
	int m_tgc_scoll_size;
};

#endif
