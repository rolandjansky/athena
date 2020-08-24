/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcSIMIDTOOFFLINEID_H
#define sTgcSIMIDTOOFFLINEID_H

#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "Identifier/Identifier.h"

struct sTgcSimIdToOfflineId {
  
  sTgcSimIdToOfflineId( const sTgcIdHelper* idHelper ) : m_idHelper(idHelper),m_simIdHelper(sTgcHitIdHelper::GetHelper()) {}
  
  inline Identifier convert( int simId ) const;
  inline int convert (const Identifier & id) const;

  const sTgcIdHelper* m_idHelper;
  sTgcHitIdHelper* m_simIdHelper;
};


Identifier sTgcSimIdToOfflineId::convert( int simId ) const {
  std::string stationName = m_simIdHelper->GetStationName(simId);
  int stationEta = m_simIdHelper->GetZSector(simId);
  int stationPhi  = m_simIdHelper->GetPhiSector(simId);
  int multilayer = m_simIdHelper->GetMultiLayer(simId);
  int layer = m_simIdHelper->GetLayer(simId);
  int side = m_simIdHelper->GetSide(simId);
  return m_idHelper->channelID( stationName[1] == 'L' ?  "STL" : "STS",
				side == 1 ? stationEta+1 : -stationEta-1,
				(stationPhi-1)/2+1,multilayer,layer,1,1 );
}

inline int sTgcSimIdToOfflineId::convert (const Identifier & id) const
	{
	int stationEta=m_idHelper->stationEta(id);
	int side(stationEta>0 ? 1 : -1);
	stationEta = abs(stationEta) - 1;
	bool isSmall(m_idHelper->isSmall(id));
	int phi=2*m_idHelper->stationPhi(id) - static_cast<int>(!isSmall);
	int ml=m_idHelper->multilayer(id);
	int ly=m_idHelper->gasGap(id);
	std::ostringstream stationName;
	stationName<<"T"<<stationEta<<(isSmall ? "S" : "L")<<ml;
	return m_simIdHelper->BuildsTgcHitId(stationName.str(), phi, stationEta, ml, ly, side);
	}

#endif
