/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_SIMIDTOOFFLINEID_H
#define MM_SIMIDTOOFFLINEID_H

#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "Identifier/Identifier.h"

struct MM_SimIdToOfflineId {
  
  MM_SimIdToOfflineId( const MmIdHelper* idHelper ) : m_idHelper(idHelper),m_simIdHelper(MicromegasHitIdHelper::GetHelper()) {}
  
  inline Identifier convert( int simId ) const;
  inline int convert (const Identifier & id) const;

  const MmIdHelper* m_idHelper;
  MicromegasHitIdHelper* m_simIdHelper;
};


Identifier MM_SimIdToOfflineId::convert( int simId ) const {
  std::string stationName = m_simIdHelper->GetStationName(simId);
  int stationEta = m_simIdHelper->GetZSector(simId);
  int stationPhi  = m_simIdHelper->GetPhiSector(simId);
  int multilayer = m_simIdHelper->GetMultiLayer(simId);
  int layer = m_simIdHelper->GetLayer(simId);
  int side = m_simIdHelper->GetSide(simId);
  return m_idHelper->channelID( stationName[2] == 'L' ?  "MML" : "MMS",
				side == 1 ? stationEta+1 : -stationEta-1,
				(stationPhi-1)/2+1,multilayer,layer,1 );
}


inline int MM_SimIdToOfflineId::convert (const Identifier & id) const
	{
	int stationEta=m_idHelper->stationEta(id);
	int side(stationEta>0 ? 1 : -1);
	stationEta = abs(stationEta) - 1;
	bool isSmall(m_idHelper->isSmall(id));
	int phi=2*m_idHelper->stationPhi(id) - static_cast<int>(!isSmall);
	int ml=m_idHelper->multilayer(id);
	int ly=m_idHelper->gasGap(id);
	std::ostringstream stationName;
	stationName<<"M"<<stationEta<<(isSmall ? "S" : "L")<<ml;
	return m_simIdHelper->BuildMicromegasHitId(stationName.str(), phi, stationEta, ml, ly, side);
	}


#endif
