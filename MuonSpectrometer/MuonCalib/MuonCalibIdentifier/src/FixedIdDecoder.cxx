/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibIdentifier/FixedIdDecoder.h"
#include "MuonCalibIdentifier/MuonFixedId.h"


namespace MuonCalib {


int FixedIdDecoder :: eta(int id)
	{
	MuonFixedId mid(id);
	return mid.eta();
	}

int FixedIdDecoder :: phi(int id)
	{
	MuonFixedId mid(id);
	return mid.phi();
	}

int FixedIdDecoder :: stationName(int id)
	{
	MuonFixedId mid(id);
	return mid.stationName();
	}

std::string FixedIdDecoder :: stationNameStr(int id)
	{
	MuonFixedId mid(id);
	return mid.stationNameString();
	}


int FixedIdDecoder :: mdtMultilayer(int id)
	{
	MuonFixedId mid(id);
	return mid.mdtMultilayer();
	}

int FixedIdDecoder :: mdtTubeLayer(int id)
	{
	MuonFixedId mid(id);
	return mid.mdtTubeLayer();
	}

int FixedIdDecoder :: mdtTube(int id)
	{
	MuonFixedId mid(id);
	return mid.mdtTube();
	}

int FixedIdDecoder :: mdtMezzanine(int id)
	{
	MuonFixedId mid(id);
	return mid.mdtMezzanine();
	}

int  FixedIdDecoder :: cscChamberLayer(int id)
	{
	MuonFixedId mid(id);
	return mid.cscChamberLayer();
	}
	
int  FixedIdDecoder :: cscWireLayer(int id)
	{
	MuonFixedId mid(id);
	return mid.cscWireLayer();
	}
	
int  FixedIdDecoder :: cscMeasuresPhi(int id)
	{
	MuonFixedId mid(id);
	return mid.cscMeasuresPhi();
	}
	
int  FixedIdDecoder :: cscStrip(int id)
	{
	MuonFixedId mid(id);
	return mid.cscStrip();
	}

int FixedIdDecoder :: rpcDoubletR(int id)
	{
	MuonFixedId mid(id);
	return mid.rpcDoubletR();
	}

int FixedIdDecoder :: rpcDoubletZ(int id)
	{
	MuonFixedId mid(id);
	return mid.rpcDoubletZ();
	}

int FixedIdDecoder :: rpcDoubletPhi(int id)
	{
	MuonFixedId mid(id);
	return mid.rpcDoubletPhi();
	}

int FixedIdDecoder :: rpcGasGap(int id)
	{
	MuonFixedId mid(id);
	return mid.rpcGasGap();
	}

int FixedIdDecoder :: rpcMeasuresPhi(int id)
	{
	MuonFixedId mid(id);
	return mid.rpcMeasuresPhi();
	}

int FixedIdDecoder :: rpcStrip(int id)
	{
	MuonFixedId mid(id);
	return mid.rpcStrip();
	}

int FixedIdDecoder :: tgcGasGap(int id)
	{
	MuonFixedId mid(id);
	return mid.tgcGasGap();
	}

int FixedIdDecoder :: tgcIsStrip(int id)
	{
	MuonFixedId mid(id);
	return mid.tgcIsStrip();
	}

int FixedIdDecoder :: tgcChannel(int id)
	{
	MuonFixedId mid(id);
	return mid.tgcChannel();
	}



}

