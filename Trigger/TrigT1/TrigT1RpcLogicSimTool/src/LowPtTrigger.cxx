/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RpcLogicSimTool/LowPtTrigger.h"
#include "TrigT1RpcLogicSimTool/TriggerAlgorithm.h"

LowPtTrigger::LowPtTrigger(int road_width)
{

	algo1 = new TriggerAlgorithm(1, 2, 0, false, false, false, road_width);
	algo2 = new TriggerAlgorithm(2, 1, 0, false, false, false, road_width);

}

LowPtTrigger::~LowPtTrigger()
{
  delete algo1;
  delete algo2;
}

bool LowPtTrigger::trigger(vector<unsigned int> hit_ids)
{


	return (algo1->trigger(hit_ids)||algo2->trigger(hit_ids));

}

int LowPtTrigger::getRPC1(int num_algo)
{
	if(num_algo==1)
		return algo1->getRPC1();
	else
		return algo2->getRPC1();
}

int LowPtTrigger::getRPC2(int num_algo)
{
	if(num_algo==1)
		return algo1->getRPC2();
	else
		return algo2->getRPC2();
}

int LowPtTrigger::getRPC3(int num_algo)
{
	if(num_algo==1)
		return algo1->getRPC3();
	else
		return algo2->getRPC3();
}

int LowPtTrigger::getRPCNum(MuonCalib::MuonFixedId id)
{
	return algo1->getRPCNum(id);
}

int LowPtTrigger::getSectorType(MuonCalib::MuonFixedId id)
{
	return algo1->getSectorType(id);
}
