/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RpcLogicSimTool/TriggerAlgorithm.h"
#include <math.h>
		
TriggerAlgorithm::TriggerAlgorithm(int min_hits_rpc1, int min_hits_rpc2, 
				   int min_hits_rpc3, bool both1, 
				   bool both2, bool both3, int road_width)
{
	
	min_h_rpc1 = min_hits_rpc1;
	min_h_rpc2 = min_hits_rpc2;
	min_h_rpc3 = min_hits_rpc3;
	both_rpc1 = both1;
	both_rpc2 = both2;
	both_rpc3 = both3;
	road_w = road_width;
	stripsPerPort[0]=4;
	stripsPerPort[1]=4;
	stripsPerPort[2]=4;
	num_rpc1=0;
	num_rpc2=0;
	num_rpc3=0;
	
	low_pt_map = initLowPtMapping();
	high_pt_map = initHighPtMapping();
	rpc1_layout = initRPC1Layout();
	rpc2_layout = initRPC2Layout();
	rpc3_layout = initRPC3Layout();
	sector_geom_map = initSectorGeomMapping();
	
}

/*************************************************************
*
* TriggerAlgorithm::trigger(): Takes a vector of hit ids and 
* returns true if there was a combination of hits in any 
* trigger tower that satisfied the predefined trigger logic.
*
*************************************************************/

bool TriggerAlgorithm::trigger(vector<unsigned int> hit_ids)
{
	int size = hit_ids.size();

	initTowerCoords(hit_ids); 
	
	for(int i=0; i<size; i++) {
		vector<unsigned int> same_tower(0);
		unsigned int curr_raw = hit_ids.at(i);
		MuonCalib::MuonFixedId curr_id(curr_raw);
		same_tower.push_back(curr_raw);
//		int currMeasuresPhi = curr_id.rpcMeasuresPhi();
		int currStrip = curr_id.rpcStrip();
		
		for(int j=i+1; j<size; j++)
		{
			unsigned int comp_raw = hit_ids.at(j);
			MuonCalib::MuonFixedId comp_id(comp_raw);
//			int compMeasuresPhi = comp_id.rpcMeasuresPhi();
			int compStrip = comp_id.rpcStrip();
			int dist = fabs(currStrip-compStrip);
			if(sameTower(curr_id, comp_id)&&dist<=road_w)
			{
				same_tower.push_back(comp_raw);
			}
		}
		
		bool trig = triggerTower(same_tower);
		
		
		if(trig)
			return true;
	}	
	
	return false;
}

/*****************************************************************
*
* TriggerAlgorithm::triggerTower(): Returns true if the given hits
* satisfy the defined RPC requirements in a single trigger tower
*
******************************************************************/

bool TriggerAlgorithm::triggerTower(vector <unsigned int> hit_ids)
{

	int num_hits = hit_ids.size();
	
	vector<unsigned int> rpc1hits_phi(0);
	vector<unsigned int> rpc2hits_phi(0);
	vector<unsigned int> rpc3hits_phi(0);

	vector<unsigned int> rpc1hits_eta(0);
	vector<unsigned int> rpc2hits_eta(0);
	vector<unsigned int> rpc3hits_eta(0);
			
	for(int i=0; i<num_hits; i++){
		unsigned int raw_num = hit_ids.at(i);
		MuonCalib::MuonFixedId id(raw_num);
		
		int rpc_num = getRPCNum(id);
		if(rpc_num==1){
			if(id.rpcMeasuresPhi()==1)
				rpc1hits_phi.push_back(raw_num);
			else
				rpc1hits_eta.push_back(raw_num);
		}else if(rpc_num==2){
			if(id.rpcMeasuresPhi()==1)
				rpc2hits_phi.push_back(raw_num);
			else
				rpc2hits_eta.push_back(raw_num);
		}else if(rpc_num==3){
			if(id.rpcMeasuresPhi()==1)
				rpc3hits_phi.push_back(raw_num);
			else
				rpc3hits_eta.push_back(raw_num);
		}
	}	

	
	bool sat1_phi = satisfiesRPC(rpc1hits_phi, 1);
	bool sat2_phi = satisfiesRPC(rpc2hits_phi, 2);
	bool sat3_phi = satisfiesRPC(rpc3hits_phi, 3);

	bool sat1_eta = satisfiesRPC(rpc1hits_eta, 1);
	bool sat2_eta = satisfiesRPC(rpc2hits_eta, 2);
	bool sat3_eta = satisfiesRPC(rpc3hits_eta, 3);

	bool sat1 = (both_rpc1?(sat1_phi&&sat1_eta):(sat1_phi||sat1_eta));
	bool sat2 = (both_rpc2?(sat2_phi&&sat2_eta):(sat2_phi||sat2_eta));
	bool sat3 = (both_rpc3?(sat3_phi&&sat3_eta):(sat3_phi||sat3_eta));

	if(sat1)
		num_rpc1++;
	if(sat2)
		num_rpc2++;
	if(sat3)
		num_rpc3++;

	bool trig = (sat1&&sat2&&sat3);
		
	return trig;
}


/************************************************************
* TriggerAlgorithm::getRPC1(): Helper function that returns
* the number of gas gaps required for RPC1
************************************************************/

int TriggerAlgorithm::getRPC1()
{
	return num_rpc1;

}

/************************************************************
* TriggerAlgorithm::getRPC2(): Helper function that returns
* the number of gas gaps required for RPC2
************************************************************/

int TriggerAlgorithm::getRPC2()
{
	return num_rpc2;
}

/************************************************************
* TriggerAlgorithm::getRPC3(): Helper function that returns
* the number of gas gaps required for RPC3
************************************************************/

int TriggerAlgorithm::getRPC3()
{
	return num_rpc3;
}


/************************************************************
* TriggerAlgorithm::getRPCNum(): Takes a MuonFixedId and 
* returns the number of the RPC that hits was in
************************************************************/

int TriggerAlgorithm::getRPCNum(MuonCalib::MuonFixedId id)
{
	
	int station = id.stationName();
	int rpc_num = 0;
		
	if(station==3||station==4||station==9){
		int eta = fabs(id.eta());
		int phi = id.phi();
		if(station==3&&phi!=7&&eta==7)
			rpc_num = 2;
		else if(station==3&&phi==7&&eta==6)
			rpc_num=2;
		else
			rpc_num = id.rpcDoubletR(); 	
	}else if(station==5||station==6||station==10||station==11){
		rpc_num = 3;
	}

	return rpc_num;
}

/*************************************************************
* TriggerAlgorithm::satisfiesRPC(): This function checks if
* the hits given satisfy the requirements set for the 
* individual RPC specified
**************************************************************/

bool TriggerAlgorithm::satisfiesRPC(vector <unsigned int> hit_ids, int rpc_num)
{
	
	int min_hits = 0;
	bool both = false;
	
	if(rpc_num==1){
		min_hits = min_h_rpc1;
		both = both_rpc1;
	}else if(rpc_num==2){
		min_hits = min_h_rpc2;
		both = both_rpc2;
	}else{
		min_hits = min_h_rpc3;
		both = both_rpc3;
	}
	
	int size = hit_ids.size();

	if(min_hits==0&&size==0)
		return true;


	for(int i=0; i<size; i++){
		MuonCalib::MuonFixedId curr_id(hit_ids.at(i));
		
		int curr_gg = curr_id.rpcGasGap();
		int num_hits = 1;
		
		for(int j=i+1; j<size; j++){
			if(i==j)
				continue;
			
			MuonCalib::MuonFixedId comp_id(hit_ids.at(j));
			int comp_gg = comp_id.rpcGasGap();
									
			if(curr_gg!=comp_gg)
			{
				num_hits++;
			}
			
			if(num_hits >= min_hits)
			{
				return true;
			}
		}
		
		if(num_hits>=min_hits)
		{
			return true;
		}
	}
	
	return false;
		
}


/****************************************************************
* TriggerAlgorithm::initTowerCoords(): this function initializes
* the map that is used for lookup of tower coordinates
****************************************************************/

void TriggerAlgorithm::initTowerCoords(vector<unsigned int> hit_ids)
{
	int size = hit_ids.size();
	
	for(int i=0; i<size; i++)
	{
		unsigned int raw = hit_ids.at(i);
		MuonCalib::MuonFixedId id(raw);
		
		if(id.rpcMeasuresPhi()==1)
			continue;
		
		pair<unsigned int, vector<pair <int, int> > > inspair = make_pair(raw, findTowerCoord(id, getRPCNum(id)));		
		coords.insert(inspair);
	}
}

/*********************************************************************
* TriggerAlgorithm::towerCoord(): Retrieves the tower coordinate of 
* the hit from the map that stores them
*********************************************************************/

vector<pair<int, int> > TriggerAlgorithm::towerCoord(MuonCalib::MuonFixedId id, int /*rpc_num*/)
{
	vector<pair<int, int> > ret(0);
	map<unsigned int, vector<pair<int, int> > >::iterator iter = coords.find(id.getIdInt());	
	ret = ((*iter).second);
		
	return ret;
}

/*******************************************************************
* TriggerAlgorithm::findTowerCoord(): Computes the tower coordinate
* by using the cabling maps defined in the code
********************************************************************/

vector<pair<int, int> > TriggerAlgorithm::findTowerCoord(MuonCalib::MuonFixedId id, int rpc_num)
{
	int phiSector = getSectorType(id);
	int globChannel = getGlobChannel(id, rpc_num);

	int geom = getGeomType(phiSector);
	int towerNum = 0;
	vector<pair<int, int> > ret(0);
	vector<vector<vector<int> > > cabling(0);
	int globPort = 0;
	
	if(globChannel%stripsPerPort[rpc_num-1]==0)
		globPort = (globChannel/stripsPerPort[rpc_num-1]);
	else
		globPort = (globChannel+stripsPerPort[rpc_num-1])/stripsPerPort[rpc_num-1];
	
	if(rpc_num==1||rpc_num==2){
		cabling = getLowPtMapping();
	}else{
		cabling = getHighPtMapping();
	}

		
	vector<vector<int> > eta = cabling.at(geom-1);
	int size = eta.size();
	
	for(int i=0; i<size; i++){
		towerNum = i;
		vector<int> channels = eta.at(i);
		int size1 = channels.size();
//		bool found = false;
		
		
	if((globPort>=channels.at(0)&&globPort<=channels.at(size1-1))||(ret.size()==0&&i==(size-1)))
			ret.push_back(make_pair(phiSector, towerNum));
			ret.push_back(make_pair(phiSector, towerNum-1));
			ret.push_back(make_pair(phiSector, towerNum+1));
	}
	
	return ret;
}



//Gives a vector corresponding to the 23 different types of eta mappings
vector<vector <vector <int> > > TriggerAlgorithm::initLowPtMapping()
{

	vector<vector <vector <int> > > mapping(0); 
	
	//TYPE 1
	vector<vector <int> > eta_map1(0);	
	eta_map1.push_back(createRange(1, 8));
	eta_map1.push_back(createRange(6, 15));
	eta_map1.push_back(createRange(13, 23));
	eta_map1.push_back(createRange(20, 30));
	eta_map1.push_back(createRange(27, 37));
	eta_map1.push_back(createRange(34, 43));
	eta_map1.push_back(createRange(40, 47));
	eta_map1.push_back(createRange(44, 52));
	eta_map1.push_back(createRange(49, 56));
	eta_map1.push_back(createRange(53, 62));
	eta_map1.push_back(createRange(59, 68));	
	eta_map1.push_back(createRange(65, 72));
	mapping.push_back(eta_map1);
	
	//TYPE 2
	vector<vector<int> > eta_map2(0);
	eta_map2.push_back(createRange(1, 10));
	eta_map2.push_back(createRange(6, 16));
	eta_map2.push_back(createRange(14, 22));
	eta_map2.push_back(createRange(20, 30));
	eta_map2.push_back(createRange(28, 35));
	eta_map2.push_back(createRange(32, 41));
	eta_map2.push_back(createRange(38, 47));
	eta_map2.push_back(createRange(43, 56));
	eta_map2.push_back(createRange(52, 64));
	eta_map2.push_back(createRange(60, 69));
	eta_map2.push_back(createRange(66, 78));
	mapping.push_back(eta_map2);

	//TYPE 3
	vector<vector<int> > eta_map3(0);
	eta_map3.push_back(createRange(1, 7));
	eta_map3.push_back(createRange(4, 12));
	eta_map3.push_back(createRange(10, 19));
	eta_map3.push_back(createRange(17, 28));
	eta_map3.push_back(createRange(24, 34));
	eta_map3.push_back(createRange(31, 41));
	eta_map3.push_back(createRange(38, 47));
	eta_map3.push_back(createRange(44, 51));
	eta_map3.push_back(createRange(48, 56));
	eta_map3.push_back(createRange(53, 60));
	eta_map3.push_back(createRange(57, 66));
	eta_map3.push_back(createRange(62, 71));
	eta_map3.push_back(createRange(69, 76));
	mapping.push_back(eta_map3);
	
	//TYPE 4
	vector<vector<int> > eta_map4(0);
	eta_map4.push_back(createRange(1, 7));
	eta_map4.push_back(createRange(4, 12));
	eta_map4.push_back(createRange(10, 19));
	eta_map4.push_back(createRange(17, 28));
	eta_map4.push_back(createRange(24, 34));
	eta_map4.push_back(createRange(31, 41));
	eta_map4.push_back(createRange(38, 47));
	eta_map4.push_back(createRange(44, 51));
	eta_map4.push_back(createRange(48, 56));
	eta_map4.push_back(createRange(53, 60));
	eta_map4.push_back(createRange(57, 66));
	eta_map4.push_back(createRange(62, 71));
	eta_map4.push_back(createRange(69, 76));
	mapping.push_back(eta_map4);
	
	//TYPE 5
	vector<vector<int> > eta_map5(0);
	eta_map5.push_back(createRange(1, 10));
	eta_map5.push_back(createRange(6, 17));
	eta_map5.push_back(createRange(14, 22));
	eta_map5.push_back(createRange(20, 30));
	eta_map5.push_back(createRange(28, 35));
	eta_map5.push_back(createRange(33, 41));
	eta_map5.push_back(createRange(38, 47));
	eta_map5.push_back(createRange(44, 56));
	eta_map5.push_back(createRange(51, 64));
	eta_map5.push_back(createRange(60, 69));
	eta_map5.push_back(createRange(65, 78));
	mapping.push_back(eta_map5);
	
	//TYPE 6
	vector<vector<int> > eta_map6(0);
	eta_map6.push_back(createRange(1, 7));
	eta_map6.push_back(createRange(4, 10));
	eta_map6.push_back(createRange(8, 17));
	eta_map6.push_back(createRange(15, 25));
	eta_map6.push_back(createRange(22, 32));
	eta_map6.push_back(createRange(29, 39));
	eta_map6.push_back(createRange(36, 45));
	eta_map6.push_back(createRange(42, 49));
	eta_map6.push_back(createRange(46, 54));
	eta_map6.push_back(createRange(51, 58));
	eta_map6.push_back(createRange(55, 64));
	eta_map6.push_back(createRange(60, 69));
	eta_map6.push_back(createRange(67, 74));
	mapping.push_back(eta_map6);
	
	
	//TYPE 7
	vector<vector<int> > eta_map7(0);
	eta_map7.push_back(createRange(1, 5));
	eta_map7.push_back(createRange(3, 9));
	eta_map7.push_back(createRange(6, 14));
	eta_map7.push_back(createRange(11, 16));
	eta_map7.push_back(createRange(16, 23));
	eta_map7.push_back(createRange(19, 31));
	eta_map7.push_back(createRange(31, 35));
	eta_map7.push_back(createRange(33, 43));
	mapping.push_back(eta_map7);

	//TYPE 8
	vector<vector<int> > eta_map8(0);
	eta_map8.push_back(createRange(1, 9));
	eta_map8.push_back(createRange(6, 16));
	eta_map8.push_back(createRange(14, 23));
	eta_map8.push_back(createRange(21, 31));
	eta_map8.push_back(createRange(28, 38));
	eta_map8.push_back(createRange(35, 48));
	eta_map8.push_back(createRange(49, 50));
	eta_map8.push_back(createRange(49, 54));
	eta_map8.push_back(createRange(51, 60));
	eta_map8.push_back(createRange(57, 65));
	eta_map8.push_back(createRange(63, 70));
	mapping.push_back(eta_map8);
	
	//TYPE 9
	vector<vector<int> > eta_map9(0);
	eta_map9.push_back(createRange(1, 9));
	eta_map9.push_back(createRange(6, 16));
	eta_map9.push_back(createRange(14, 23));
	eta_map9.push_back(createRange(21, 31));
	eta_map9.push_back(createRange(28, 38));
	eta_map9.push_back(createRange(35, 45));
	eta_map9.push_back(createRange(42, 51));
	eta_map9.push_back(createRange(48, 55));
	eta_map9.push_back(createRange(52, 60));
	eta_map9.push_back(createRange(57, 64));
	eta_map9.push_back(createRange(61, 70));
	eta_map9.push_back(createRange(67, 75));
	eta_map9.push_back(createRange(73, 80));
	mapping.push_back(eta_map9);
	
	//TYPE 10
	vector<vector<int> > eta_map10(0);
	eta_map10.push_back(createRange(1, 7));
	eta_map10.push_back(createRange(5, 12));
	eta_map10.push_back(createRange(10, 20));
	eta_map10.push_back(createRange(17, 27));
	eta_map10.push_back(createRange(24, 34));
	eta_map10.push_back(createRange(31, 41));
	eta_map10.push_back(createRange(38, 47));
	eta_map10.push_back(createRange(44, 51));
	eta_map10.push_back(createRange(48, 56));
	eta_map10.push_back(createRange(54, 60));
	eta_map10.push_back(createRange(57, 66));
	eta_map10.push_back(createRange(63, 72));
	eta_map10.push_back(createRange(69, 76));
	mapping.push_back(eta_map10);
	
	//TYPE 11
	vector<vector<int> > eta_map11(0);
	eta_map11.push_back(createRange(1, 7));
	eta_map11.push_back(createRange(5, 12));
	eta_map11.push_back(createRange(10, 20));
	eta_map11.push_back(createRange(17, 27));
	eta_map11.push_back(createRange(24, 34));
	eta_map11.push_back(createRange(31, 44));
	eta_map11.push_back(createRange(45, 46));
	eta_map11.push_back(createRange(45, 50));
	eta_map11.push_back(createRange(47, 56));
	eta_map11.push_back(createRange(53, 61));
	eta_map11.push_back(createRange(59, 66));
	mapping.push_back(eta_map11);
	
	//TYPE 12
	vector<vector<int> > eta_map12(0);
	eta_map12.push_back(createRange(1, 5));
	eta_map12.push_back(createRange(3, 9));
	eta_map12.push_back(createRange(6, 14));
	eta_map12.push_back(createRange(11, 16));
	eta_map12.push_back(createRange(16, 23));
	eta_map12.push_back(createRange(19, 31));
	eta_map12.push_back(createRange(31, 35));
	eta_map12.push_back(createRange(33, 43));
	mapping.push_back(eta_map12);
	
	//TYPE 13
	vector<vector<int> > eta_map13(0);
	eta_map13.push_back(createRange(1, 10));
	eta_map13.push_back(createRange(6, 16));
	eta_map13.push_back(createRange(14, 22));
	eta_map13.push_back(createRange(20, 30));
	eta_map13.push_back(createRange(28, 35));
	eta_map13.push_back(createRange(32, 41));
	eta_map13.push_back(createRange(38, 47));
	eta_map13.push_back(createRange(43, 56));
	eta_map13.push_back(createRange(52, 64));
	eta_map13.push_back(createRange(60, 69));
	eta_map13.push_back(createRange(66, 78));
	mapping.push_back(eta_map13);
	
	//TYPE 14
	vector<vector<int> > eta_map14(0);
	eta_map14.push_back(createRange(1, 9));		
	eta_map14.push_back(createRange(6, 16));		
	eta_map14.push_back(createRange(14, 23));		
	eta_map14.push_back(createRange(21, 31));		
	eta_map14.push_back(createRange(28, 38));		
	eta_map14.push_back(createRange(35, 48));		
	eta_map14.push_back(createRange(0, 0));
	eta_map14.push_back(createRange(49, 54));		
	eta_map14.push_back(createRange(51, 60));		
	eta_map14.push_back(createRange(57, 65));		
	eta_map14.push_back(createRange(63, 70));		
	mapping.push_back(eta_map14);
	
	//TYPE 15
	vector<vector<int> > eta_map15(0);
	eta_map15.push_back(createRange(1, 7));
	eta_map15.push_back(createRange(5, 12));
	eta_map15.push_back(createRange(10, 20));
	eta_map15.push_back(createRange(17, 27));
	eta_map15.push_back(createRange(24, 34));
	eta_map15.push_back(createRange(31, 44));
	eta_map15.push_back(createRange(0, 0));
	eta_map15.push_back(createRange(45, 50));
	eta_map15.push_back(createRange(47, 56));
	eta_map15.push_back(createRange(53, 61));
	eta_map15.push_back(createRange(59, 66));
	mapping.push_back(eta_map15);
	
	//TYPE 16
	vector<vector<int> > eta_map16(0);
	eta_map16.push_back(createRange(1, 4));
	eta_map16.push_back(createRange(1, 9));
	eta_map16.push_back(createRange(6, 12));
	eta_map16.push_back(createRange(10, 16));
	eta_map16.push_back(createRange(16, 23));
	eta_map16.push_back(createRange(19, 31));
	eta_map16.push_back(createRange(31, 35));
	eta_map16.push_back(createRange(33, 43));
	mapping.push_back(eta_map16);
	
	//TYPE 17
	vector<vector<int> > eta_map17(0);
	eta_map17.push_back(createRange(1, 4));	
	eta_map17.push_back(createRange(1, 9));	
	eta_map17.push_back(createRange(6, 12));	
	eta_map17.push_back(createRange(10, 16));	
	eta_map17.push_back(createRange(16, 23));	
	eta_map17.push_back(createRange(19, 31));	
	eta_map17.push_back(createRange(31, 35));	
	eta_map17.push_back(createRange(33, 43));	
	mapping.push_back(eta_map17);
	
	//TYPE 18
	vector<vector<int> > eta_map18(0);
	eta_map18.push_back(createRange(1, 8));
	eta_map18.push_back(createRange(6, 15));
	eta_map18.push_back(createRange(13, 23));
	eta_map18.push_back(createRange(20, 30));
	eta_map18.push_back(createRange(27, 37));
	eta_map18.push_back(createRange(34, 41));
	eta_map18.push_back(createRange(38, 46));
	eta_map18.push_back(createRange(44, 50));
	eta_map18.push_back(createRange(47, 56));
	eta_map18.push_back(createRange(53, 62));
	eta_map18.push_back(createRange(59, 68));
	eta_map18.push_back(createRange(65, 72));
	mapping.push_back(eta_map18);
	
	//TYPE 19
	vector<vector<int> > eta_map19(0);
	eta_map19.push_back(createRange(1, 7));
	eta_map19.push_back(createRange(4, 12));
	eta_map19.push_back(createRange(10, 19));
	eta_map19.push_back(createRange(17, 28));
	eta_map19.push_back(createRange(24, 34));
	eta_map19.push_back(createRange(31, 41));
	eta_map19.push_back(createRange(38, 45));
	eta_map19.push_back(createRange(42, 51));
	eta_map19.push_back(createRange(48, 54));
	eta_map19.push_back(createRange(52, 60));
	eta_map19.push_back(createRange(57, 66));
	eta_map19.push_back(createRange(62, 71));
	eta_map19.push_back(createRange(69, 76));
	mapping.push_back(eta_map19);
	
	//TYPE 20
	vector<vector<int> > eta_map20(0);
	eta_map20.push_back(createRange(1, 7));
	eta_map20.push_back(createRange(4, 12));
	eta_map20.push_back(createRange(10, 19));
	eta_map20.push_back(createRange(17, 28));
	eta_map20.push_back(createRange(24, 34));
	eta_map20.push_back(createRange(31, 41));
	eta_map20.push_back(createRange(38, 45));
	eta_map20.push_back(createRange(42, 51));
	eta_map20.push_back(createRange(48, 54));
	eta_map20.push_back(createRange(52, 60));
	eta_map20.push_back(createRange(57, 66));
	eta_map20.push_back(createRange(62, 71));
	eta_map20.push_back(createRange(69, 76));
	mapping.push_back(eta_map20);
	
	//TYPE 21
	vector<vector<int> > eta_map21(0);
	eta_map21.push_back(createRange(1, 5));
	eta_map21.push_back(createRange(2, 10));
	eta_map21.push_back(createRange(8, 17));
	eta_map21.push_back(createRange(15, 25));
	eta_map21.push_back(createRange(22, 32));
	eta_map21.push_back(createRange(29, 39));
	eta_map21.push_back(createRange(36, 43));
	eta_map21.push_back(createRange(40, 49));
	eta_map21.push_back(createRange(46, 52));
	eta_map21.push_back(createRange(50, 58));
	eta_map21.push_back(createRange(55, 64));
	eta_map21.push_back(createRange(60, 69));
	eta_map21.push_back(createRange(67, 74));
	mapping.push_back(eta_map21);
	
	//TYPE 22
	vector<vector<int> > eta_map22(0);
	eta_map22.push_back(createRange(1, 9));
	eta_map22.push_back(createRange(6, 16));
	eta_map22.push_back(createRange(14, 23));
	eta_map22.push_back(createRange(21, 31));
	eta_map22.push_back(createRange(28, 38));
	eta_map22.push_back(createRange(35, 45));
	eta_map22.push_back(createRange(42, 50));
	eta_map22.push_back(createRange(46, 55));
	eta_map22.push_back(createRange(52, 59));
	eta_map22.push_back(createRange(56, 64));
	eta_map22.push_back(createRange(61, 70));
	eta_map22.push_back(createRange(67, 75));
	eta_map22.push_back(createRange(73, 80));
	mapping.push_back(eta_map22);
	
	//TYPE 23
	vector<vector<int> > eta_map23(0);
	eta_map23.push_back(createRange(1, 7));
	eta_map23.push_back(createRange(5, 12));
	eta_map23.push_back(createRange(10, 20));
	eta_map23.push_back(createRange(17, 27));
	eta_map23.push_back(createRange(24, 34));
	eta_map23.push_back(createRange(31, 41));
	eta_map23.push_back(createRange(38, 46));
	eta_map23.push_back(createRange(42, 51));
	eta_map23.push_back(createRange(48, 55));
	eta_map23.push_back(createRange(52, 60));
	eta_map23.push_back(createRange(57, 66));
	eta_map23.push_back(createRange(63, 72));
	eta_map23.push_back(createRange(69, 76));
	mapping.push_back(eta_map23);
	
	
	return mapping;
}

vector<vector <vector <int> > > TriggerAlgorithm::initHighPtMapping()
{
	vector<vector <vector <int> > > mapping;
	
	//TYPE 1
	vector<vector<int> > eta_map1;
	eta_map1.push_back(createRange(1, 14));
	eta_map1.push_back(createRange(12, 23));
	eta_map1.push_back(createRange(20, 31));
	eta_map1.push_back(createRange(28, 39));
	eta_map1.push_back(createRange(36, 48));
	eta_map1.push_back(createRange(44, 55));
	eta_map1.push_back(createRange(50, 59));
	eta_map1.push_back(createRange(56, 66));
	eta_map1.push_back(createRange(62, 71));
	eta_map1.push_back(createRange(67, 78));
	eta_map1.push_back(createRange(74, 84));
	eta_map1.push_back(createRange(79, 84));
	mapping.push_back(eta_map1);
	
	//TYPE 2
	vector<vector<int> > eta_map2;
	eta_map2.push_back(createRange(1, 11));
	eta_map2.push_back(createRange(8, 18));
	eta_map2.push_back(createRange(16, 25));
	eta_map2.push_back(createRange(22, 35));
	eta_map2.push_back(createRange(32, 42));
	eta_map2.push_back(createRange(38, 48));
	eta_map2.push_back(createRange(45, 56));
	eta_map2.push_back(createRange(51, 64));
	eta_map2.push_back(createRange(61, 74));
	eta_map2.push_back(createRange(69, 81));
	eta_map2.push_back(createRange(76, 88));
	eta_map2.push_back(createRange(89, 96));
	mapping.push_back(eta_map2);
	
	//TYPE 3
	vector<vector<int> > eta_map3;
	eta_map3.push_back(createRange(1, 7));
	eta_map3.push_back(createRange(4, 14));
	eta_map3.push_back(createRange(12, 23));
	eta_map3.push_back(createRange(20, 31));
	eta_map3.push_back(createRange(28, 39));
	eta_map3.push_back(createRange(36, 48));
	eta_map3.push_back(createRange(44, 55));
	eta_map3.push_back(createRange(51, 59));
	eta_map3.push_back(createRange(56, 66));
	eta_map3.push_back(createRange(62, 71));
	eta_map3.push_back(createRange(67, 78));
	eta_map3.push_back(createRange(74, 84));
	eta_map3.push_back(createRange(79, 84));
	mapping.push_back(eta_map3);
	
	//TYPE 4
	vector<vector<int> > eta_map4;
	eta_map4.push_back(createRange(0, 0));
	eta_map4.push_back(createRange(1, 9));
	eta_map4.push_back(createRange(6, 18));
	eta_map4.push_back(createRange(15, 27));
	eta_map4.push_back(createRange(24, 35));
	eta_map4.push_back(createRange(32, 44));
	eta_map4.push_back(createRange(40, 51));
	eta_map4.push_back(createRange(47, 55));
	eta_map4.push_back(createRange(52, 62));
	eta_map4.push_back(createRange(58, 67));
	eta_map4.push_back(createRange(63, 75));
	eta_map4.push_back(createRange(70, 78));
	eta_map4.push_back(createRange(76, 80));
	mapping.push_back(eta_map4);
	
	//TYPE 5
	vector<vector<int> > eta_map5;
	eta_map5.push_back(createRange(1, 6));
	eta_map5.push_back(createRange(3, 14));
	eta_map5.push_back(createRange(12, 22));
	eta_map5.push_back(createRange(18, 31));
	eta_map5.push_back(createRange(28, 38));
	eta_map5.push_back(createRange(34, 44));
	eta_map5.push_back(createRange(41, 51));
	eta_map5.push_back(createRange(47, 60));
	eta_map5.push_back(createRange(57, 70));
	eta_map5.push_back(createRange(65, 77));
	eta_map5.push_back(createRange(72, 84));
	eta_map5.push_back(createRange(85, 92));
	mapping.push_back(eta_map5);
	
	//TYPE 6
	vector<vector<int> > eta_map6;
	eta_map6.push_back(createRange(1, 10));
	eta_map6.push_back(createRange(7, 14));
	eta_map6.push_back(createRange(12, 23));
	eta_map6.push_back(createRange(20, 31));
	eta_map6.push_back(createRange(28, 39));
	eta_map6.push_back(createRange(36, 48));
	eta_map6.push_back(createRange(44, 55));
	eta_map6.push_back(createRange(51, 59));
	eta_map6.push_back(createRange(56, 66));
	eta_map6.push_back(createRange(62, 71));
	eta_map6.push_back(createRange(67, 78));
	eta_map6.push_back(createRange(74, 84));
	eta_map6.push_back(createRange(79, 84));
	mapping.push_back(eta_map6);
	
	//TYPE 7
	vector<vector<int> > eta_map7;
	eta_map7.push_back(createRange(1, 8));
	eta_map7.push_back(createRange(5, 12));
	eta_map7.push_back(createRange(9, 17));
	eta_map7.push_back(createRange(15, 26));
	eta_map7.push_back(createRange(27, 41));
	eta_map7.push_back(createRange(37, 50));
	eta_map7.push_back(createRange(51, 65));
	eta_map7.push_back(createRange(61, 75));
	eta_map7.push_back(createRange(76, 85));
	mapping.push_back(eta_map7);
	
	//TYPE 8
	vector<vector<int> > eta_map8;
	eta_map8.push_back(createRange(1, 11));
	eta_map8.push_back(createRange(7, 19));
	eta_map8.push_back(createRange(16, 28));
	eta_map8.push_back(createRange(25, 38));
	eta_map8.push_back(createRange(34, 40));
	eta_map8.push_back(createRange(40, 54));
	eta_map8.push_back(createRange(55, 66));
	eta_map8.push_back(createRange(62, 71));
	eta_map8.push_back(createRange(67, 79));
	eta_map8.push_back(createRange(74, 84));
	eta_map8.push_back(createRange(80, 84));
	mapping.push_back(eta_map8);
	
	//TYPE 9
	vector<vector<int> > eta_map9;
	eta_map9.push_back(createRange(1, 11));
	eta_map9.push_back(createRange(8, 19));
	eta_map9.push_back(createRange(16, 27));
	eta_map9.push_back(createRange(24, 35));
	eta_map9.push_back(createRange(33, 44));
	eta_map9.push_back(createRange(40, 52));
	eta_map9.push_back(createRange(48, 59));
	eta_map9.push_back(createRange(55, 63));
	eta_map9.push_back(createRange(60, 71));
	eta_map9.push_back(createRange(66, 76));
	eta_map9.push_back(createRange(72, 83));
	eta_map9.push_back(createRange(78, 86));
	eta_map9.push_back(createRange(84, 88));
	mapping.push_back(eta_map9);
	
	//TYPE 10
	vector<vector<int> > eta_map10;
	eta_map10.push_back(createRange(1, 10));
	eta_map10.push_back(createRange(7, 17));
	eta_map10.push_back(createRange(14, 25));
	eta_map10.push_back(createRange(22, 33));
	eta_map10.push_back(createRange(31, 41));
	eta_map10.push_back(createRange(38, 50));
	eta_map10.push_back(createRange(46, 57));
	eta_map10.push_back(createRange(53, 61));
	eta_map10.push_back(createRange(58, 69));
	eta_map10.push_back(createRange(64, 74));
	eta_map10.push_back(createRange(70, 81));
	eta_map10.push_back(createRange(76, 86));
	eta_map10.push_back(createRange(82, 86));
	mapping.push_back(eta_map10);
	
	//TYPE 11
	vector<vector<int> > eta_map11;
	eta_map11.push_back(createRange(1, 8));
	eta_map11.push_back(createRange(5, 15));
	eta_map11.push_back(createRange(12, 25));
	eta_map11.push_back(createRange(21, 34));
	eta_map11.push_back(createRange(30, 37));
	eta_map11.push_back(createRange(37, 51));
	eta_map11.push_back(createRange(52, 62));
	eta_map11.push_back(createRange(58, 67));
	eta_map11.push_back(createRange(63, 75));
	eta_map11.push_back(createRange(70, 79));
	eta_map11.push_back(createRange(76, 80));
	mapping.push_back(eta_map11);
	
	//TYPE 12
	vector<vector<int> > eta_map12;
	eta_map12.push_back(createRange(1, 8));
	eta_map12.push_back(createRange(5, 12));
	eta_map12.push_back(createRange(9, 17));
	eta_map12.push_back(createRange(15, 26));
	eta_map12.push_back(createRange(27, 41));
	eta_map12.push_back(createRange(37, 50));
	eta_map12.push_back(createRange(51, 65));
	eta_map12.push_back(createRange(61, 75));
	eta_map12.push_back(createRange(76, 85));
	mapping.push_back(eta_map12);
	
	//TYPE 13
	vector<vector<int> > eta_map13;
	eta_map13.push_back(createRange(1, 11));
	eta_map13.push_back(createRange(8, 18));
	eta_map13.push_back(createRange(16, 25));
	eta_map13.push_back(createRange(22, 35));
	eta_map13.push_back(createRange(32, 42));
	eta_map13.push_back(createRange(38, 48));
	eta_map13.push_back(createRange(45, 56));
	eta_map13.push_back(createRange(51, 64));
	eta_map13.push_back(createRange(61, 74));
	eta_map13.push_back(createRange(69, 81));
	eta_map13.push_back(createRange(76, 88));
	eta_map13.push_back(createRange(89, 96));
	mapping.push_back(eta_map13);
	
	//TYPE 14
	vector<vector<int> > eta_map14;
	eta_map14.push_back(createRange(1, 11));
	eta_map14.push_back(createRange(7, 19));
	eta_map14.push_back(createRange(16, 28));
	eta_map14.push_back(createRange(25, 38));
	eta_map14.push_back(createRange(34, 40));
	eta_map14.push_back(createRange(40, 54));
	eta_map14.push_back(createRange(55, 64));
	eta_map14.push_back(createRange(59, 71));
	eta_map14.push_back(createRange(67, 79));
	eta_map14.push_back(createRange(74, 84));
	eta_map14.push_back(createRange(80, 84));
	mapping.push_back(eta_map14);	
	
	//TYPE 15
	vector<vector<int> > eta_map15;
	eta_map15.push_back(createRange(1, 8));
	eta_map15.push_back(createRange(5, 15));
	eta_map15.push_back(createRange(12, 25));
	eta_map15.push_back(createRange(21, 34));
	eta_map15.push_back(createRange(30, 37));
	eta_map15.push_back(createRange(37, 51));
	eta_map15.push_back(createRange(52, 61));
	eta_map15.push_back(createRange(56, 67));
	eta_map15.push_back(createRange(63, 75));
	eta_map15.push_back(createRange(70, 79));
	eta_map15.push_back(createRange(76, 80));
	mapping.push_back(eta_map15);
	
	//TYPE 16
	vector<vector<int> > eta_map16;
	eta_map16.push_back(createRange(1, 6));
	eta_map16.push_back(createRange(3, 12));
	eta_map16.push_back(createRange(9, 15));
	eta_map16.push_back(createRange(13, 26));
	eta_map16.push_back(createRange(27, 41));
	eta_map16.push_back(createRange(37, 50));
	eta_map16.push_back(createRange(51, 65));
	eta_map16.push_back(createRange(61, 75));
	eta_map16.push_back(createRange(76, 85));
	mapping.push_back(eta_map16);
	
	//TYPE 17
	vector<vector<int> > eta_map17;
	eta_map17.push_back(createRange(1, 6));
	eta_map17.push_back(createRange(3, 12));
	eta_map17.push_back(createRange(9, 15));
	eta_map17.push_back(createRange(13, 26));
	eta_map17.push_back(createRange(27, 41));
	eta_map17.push_back(createRange(37, 50));
	eta_map17.push_back(createRange(51, 65));
	eta_map17.push_back(createRange(61, 75));
	eta_map17.push_back(createRange(76, 85));
	mapping.push_back(eta_map17);
	
	//TYPE 18
	vector<vector<int> > eta_map18;
	eta_map18.push_back(createRange(1, 14));
	eta_map18.push_back(createRange(12, 23));
	eta_map18.push_back(createRange(20, 31));
	eta_map18.push_back(createRange(28, 39));
	eta_map18.push_back(createRange(36, 47));
	eta_map18.push_back(createRange(44, 52));
	eta_map18.push_back(createRange(48, 59));
	eta_map18.push_back(createRange(55, 64));
	eta_map18.push_back(createRange(60, 71));
	eta_map18.push_back(createRange(67, 78));
	eta_map18.push_back(createRange(74, 84));
	eta_map18.push_back(createRange(79, 84));
	mapping.push_back(eta_map18);
	
	//TYPE 19
	vector<vector<int> > eta_map19;
	eta_map19.push_back(createRange(1, 7));
	eta_map19.push_back(createRange(4, 14));
	eta_map19.push_back(createRange(12, 23));
	eta_map19.push_back(createRange(20, 31));
	eta_map19.push_back(createRange(28, 39));
	eta_map19.push_back(createRange(36, 47));
	eta_map19.push_back(createRange(44, 52));
	eta_map19.push_back(createRange(48, 59));
	eta_map19.push_back(createRange(56, 64));
	eta_map19.push_back(createRange(60, 71));
	eta_map19.push_back(createRange(67, 78));
	eta_map19.push_back(createRange(74, 84));
	eta_map19.push_back(createRange(79, 84));
	mapping.push_back(eta_map19);
	
	//TYPE 20
	vector<vector<int> > eta_map20;
	eta_map20.push_back(createRange(0, 0));
	eta_map20.push_back(createRange(1, 9));
	eta_map20.push_back(createRange(6, 18));
	eta_map20.push_back(createRange(15, 27));
	eta_map20.push_back(createRange(24, 35));
	eta_map20.push_back(createRange(32, 43));
	eta_map20.push_back(createRange(40, 49));
	eta_map20.push_back(createRange(44, 55));
	eta_map20.push_back(createRange(52, 60));
	eta_map20.push_back(createRange(56, 67));
	eta_map20.push_back(createRange(63, 75));
	eta_map20.push_back(createRange(70, 78));
	eta_map20.push_back(createRange(76, 80));
	mapping.push_back(eta_map20);
	
	//TYPE 21
	vector<vector<int> > eta_map21;
	eta_map21.push_back(createRange(1, 8));
	eta_map21.push_back(createRange(5, 14));
	eta_map21.push_back(createRange(12, 23));
	eta_map21.push_back(createRange(20, 31));
	eta_map21.push_back(createRange(28, 39));
	eta_map21.push_back(createRange(36, 47));
	eta_map21.push_back(createRange(44, 52));
	eta_map21.push_back(createRange(48, 59));
	eta_map21.push_back(createRange(56, 64));
	eta_map21.push_back(createRange(60, 71));
	eta_map21.push_back(createRange(67, 78));
	eta_map21.push_back(createRange(74, 84));
	eta_map21.push_back(createRange(79, 84));
	mapping.push_back(eta_map21);
	
	//TYPE 22
	vector<vector<int> > eta_map22;
	eta_map22.push_back(createRange(1, 11));
	eta_map22.push_back(createRange(8, 19));
	eta_map22.push_back(createRange(16, 27));
	eta_map22.push_back(createRange(24, 35));
	eta_map22.push_back(createRange(33, 44));
	eta_map22.push_back(createRange(40, 52));
	eta_map22.push_back(createRange(48, 57));
	eta_map22.push_back(createRange(53, 63));
	eta_map22.push_back(createRange(60, 69));
	eta_map22.push_back(createRange(64, 76));
	eta_map22.push_back(createRange(71, 83));
	eta_map22.push_back(createRange(78, 86));
	eta_map22.push_back(createRange(84, 88));
	mapping.push_back(eta_map22);	
	
	//TYPE 23
	vector<vector<int> > eta_map23;
	eta_map23.push_back(createRange(1, 10));
	eta_map23.push_back(createRange(7, 17));
	eta_map23.push_back(createRange(14, 25));
	eta_map23.push_back(createRange(22, 33));
	eta_map23.push_back(createRange(31, 41));
	eta_map23.push_back(createRange(38, 50));
	eta_map23.push_back(createRange(46, 55));
	eta_map23.push_back(createRange(51, 61));
	eta_map23.push_back(createRange(58, 67));
	eta_map23.push_back(createRange(62, 74));
	eta_map23.push_back(createRange(70, 81));
	eta_map23.push_back(createRange(76, 86));
	eta_map23.push_back(createRange(82, 86));
	mapping.push_back(eta_map23);
	
	return mapping;	

}

/************************************************************
* TriggerAlgorithm::createRange(): creates a vector with
* elements between low and high, inclusive
************************************************************/

vector<int> TriggerAlgorithm::createRange(int low, int high)
{
	
	vector<int> chambers(0);
	
	for(int i=low; i<=high; i++)
	{
		chambers.push_back(i);
	}
	
	return chambers;

}

/***********************************************************
* TriggerAlgorithm::initRPC1Layout(): creates a vector with
* the number of channels in the various RPC1 chambers
************************************************************/

vector<vector <vector <int> > > TriggerAlgorithm::initRPC1Layout()
{
	vector<vector <vector<int> > > layout(0);	

	//TYPE 1
	vector<vector<int> > eta1(0);
	eta1.push_back(makeZTriplet(32, 0, 0));
	eta1.push_back(makeZTriplet(32, 32, 0));
	eta1.push_back(makeZTriplet(32, 32, 0));
	eta1.push_back(makeZTriplet(40, 0, 0));
	eta1.push_back(makeZTriplet(40, 0, 0));
	eta1.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta1);
	
	//TYPE 2
	vector<vector<int> > eta2(0);
	eta2.push_back(makeZTriplet(32, 32, 0));
	eta2.push_back(makeZTriplet(24, 24, 8));
	eta2.push_back(makeZTriplet(24, 24, 0));
	eta2.push_back(makeZTriplet(40, 24, 0));
	eta2.push_back(makeZTriplet(32, 0, 0));
	eta2.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta2);
	
	//TYPE 3
	vector<vector<int> > eta3(0);
	eta3.push_back(makeZTriplet(24, 24, 0));
	eta3.push_back(makeZTriplet(32, 32, 0));
	eta3.push_back(makeZTriplet(32, 32, 0));
	eta3.push_back(makeZTriplet(40, 0, 0));
	eta3.push_back(makeZTriplet(40, 0, 0));
	eta3.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta3);	
	
	//TYPE 4
	vector<vector<int> > eta4(0);
	eta4.push_back(makeZTriplet(24, 24, 0));
	eta4.push_back(makeZTriplet(32, 32, 0));
	eta4.push_back(makeZTriplet(32, 32, 0));
	eta4.push_back(makeZTriplet(40, 0, 0));
	eta4.push_back(makeZTriplet(40, 0, 0));
	eta4.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta4);
	
	//TYPE 5
	vector<vector<int> > eta5(0);
	eta5.push_back(makeZTriplet(32, 32, 0));
	eta5.push_back(makeZTriplet(24, 24, 8));
	eta5.push_back(makeZTriplet(24, 24, 0));
	eta5.push_back(makeZTriplet(40, 24, 0));
	eta5.push_back(makeZTriplet(32, 0, 0));
	eta5.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta5);
	
	//TYPE 6
	vector<vector<int> > eta6(0);
	eta6.push_back(makeZTriplet(40, 0, 0));
	eta6.push_back(makeZTriplet(32, 32, 0));
	eta6.push_back(makeZTriplet(32, 32, 0));
	eta6.push_back(makeZTriplet(40, 0, 0));
	eta6.push_back(makeZTriplet(40, 0, 0));
	eta6.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta6);
	
	//TYPE 7
	vector<vector<int> > eta7(0);
	eta7.push_back(makeZTriplet(32, 32, 0));
	eta7.push_back(makeZTriplet(32, 32, 0));
	eta7.push_back(makeZTriplet(40, 0, 0));
	layout.push_back(eta7);
	
	//TYPE 8
	vector<vector<int> > eta8(0);
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(40, 0, 0));
	eta8.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta8);
	
	//TYPE 9
	vector<vector<int> > eta9(0);
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(40, 0, 0));
	eta9.push_back(makeZTriplet(40, 0, 0));
	eta9.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta9);
	
	//TYPE 10
	vector<vector<int> > eta10(0);
	eta10.push_back(makeZTriplet(24, 24, 0));
	eta10.push_back(makeZTriplet(32, 32, 0));
	eta10.push_back(makeZTriplet(32, 32, 0));
	eta10.push_back(makeZTriplet(40, 0, 0));
	eta10.push_back(makeZTriplet(40, 0, 0));
	eta10.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta10);
	
	//TYPE 11
	vector<vector<int> > eta11(0);
	eta11.push_back(makeZTriplet(24, 24, 0));
	eta11.push_back(makeZTriplet(32, 32, 0));
	eta11.push_back(makeZTriplet(32, 32, 0));
	eta11.push_back(makeZTriplet(40, 0, 0));
	eta11.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta11);
	
	//TYPE 12
	vector<vector<int> > eta12(0);
	eta12.push_back(makeZTriplet(32, 32, 0));
	eta12.push_back(makeZTriplet(32, 32, 0));
	eta12.push_back(makeZTriplet(40, 0, 0));
	layout.push_back(eta12);	
	
	//TYPE 13
	vector<vector<int> > eta13(0);
	eta13.push_back(makeZTriplet(32, 32, 0));
	eta13.push_back(makeZTriplet(24, 24, 8));
	eta13.push_back(makeZTriplet(24, 24, 0));
	eta13.push_back(makeZTriplet(40, 24, 0));
	eta13.push_back(makeZTriplet(32, 0, 0));
	eta13.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta13);
	
	//TYPE 14
	vector<vector<int> > eta14(0);
	eta14.push_back(makeZTriplet(32, 32, 0));
	eta14.push_back(makeZTriplet(32, 32, 0));
	eta14.push_back(makeZTriplet(32, 32, 0));
	eta14.push_back(makeZTriplet(40, 0, 0));
	eta14.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta14);
	
	//TYPE 15
	vector<vector<int> > eta15(0);
	eta15.push_back(makeZTriplet(24, 24, 0));
	eta15.push_back(makeZTriplet(32, 32, 0));
	eta15.push_back(makeZTriplet(32, 32, 0));
	eta15.push_back(makeZTriplet(40, 0, 0));
	eta15.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta15);
	
	//TYPE 16
	vector<vector<int> > eta16(0);
	eta16.push_back(makeZTriplet(32, 32, 0));
	eta16.push_back(makeZTriplet(32, 32, 0));
	eta16.push_back(makeZTriplet(40, 0, 0));
	layout.push_back(eta16);
	
	//TYPE 17
	vector<vector<int> > eta17(0);
	eta17.push_back(makeZTriplet(32, 32, 0));
	eta17.push_back(makeZTriplet(32, 32, 0));
	eta17.push_back(makeZTriplet(40, 0, 0));
	layout.push_back(eta17);
	
	//TYPE 18
	vector<vector<int> > eta18(0);
	eta18.push_back(makeZTriplet(32, 0, 0));
	eta18.push_back(makeZTriplet(32, 32, 0));
	eta18.push_back(makeZTriplet(32, 32, 0));
	eta18.push_back(makeZTriplet(40, 0, 0));
	eta18.push_back(makeZTriplet(40, 0, 0));
	eta18.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta18);
	
	//TYPE 19
	vector<vector<int> > eta19(0);
	eta19.push_back(makeZTriplet(24, 24, 0));
	eta19.push_back(makeZTriplet(32, 32, 0));
	eta19.push_back(makeZTriplet(32, 32, 0));
	eta19.push_back(makeZTriplet(40, 0, 0));
	eta19.push_back(makeZTriplet(40, 0, 0));
	eta19.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta19);
	
	//TYPE 20
	vector<vector<int> > eta20(0);
	eta20.push_back(makeZTriplet(24, 24, 0));
	eta20.push_back(makeZTriplet(32, 32, 0));
	eta20.push_back(makeZTriplet(32, 32, 0));
	eta20.push_back(makeZTriplet(40, 0, 0));
	eta20.push_back(makeZTriplet(40, 0, 0));
	eta20.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta20);
	
	//TYPE 21
	vector<vector<int> > eta21(0);
	eta21.push_back(makeZTriplet(40, 0, 0));
	eta21.push_back(makeZTriplet(32, 32, 0));
	eta21.push_back(makeZTriplet(32, 32, 0));
	eta21.push_back(makeZTriplet(40, 0, 0));
	eta21.push_back(makeZTriplet(40, 0, 0));
	eta21.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta21);
	
	//TYPE 22
	vector<vector<int> > eta22(0);
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(40, 0, 0));
	eta22.push_back(makeZTriplet(40, 0, 0));
	eta22.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta22);
		
	//TYPE 23
	vector<vector<int> > eta23(0);
	eta23.push_back(makeZTriplet(24, 24, 0));
	eta23.push_back(makeZTriplet(32, 32, 0));
	eta23.push_back(makeZTriplet(32, 32, 0));
	eta23.push_back(makeZTriplet(40, 0, 0));
	eta23.push_back(makeZTriplet(40, 0, 0));
	eta23.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta23);
	
	return layout;

}

/***********************************************************
* TriggerAlgorithm::initRPC2Layout(): creates a vector with
* the number of channels in the various RPC2 chambers
************************************************************/

vector<vector<vector <int> > > TriggerAlgorithm::initRPC2Layout()
{

	vector<vector<vector <int> > > layout(0);
	
	
	//TYPE 1
	vector<vector<int> > eta1(0);
	eta1.push_back(makeZTriplet(32, 0 , 0));
	eta1.push_back(makeZTriplet(32, 32 , 0));
	eta1.push_back(makeZTriplet(32, 32 , 0));
	eta1.push_back(makeZTriplet(40, 0 , 0));
	eta1.push_back(makeZTriplet(40, 0 , 0));
	eta1.push_back(makeZTriplet(24, 24 , 0));
	eta1.push_back(makeZTriplet(16, 0 , 0));
	layout.push_back(eta1);
	
	
	//TYPE 2
	vector<vector<int> > eta2(0);
	eta2.push_back(makeZTriplet(32, 32, 0));
	eta2.push_back(makeZTriplet(24, 24, 8));
	eta2.push_back(makeZTriplet(24, 24, 0));
	eta2.push_back(makeZTriplet(24, 24, 8));
	eta2.push_back(makeZTriplet(32, 0, 0));
	eta2.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta2);
	
	//TYPE 3
	vector<vector<int> > eta3(0);
	eta3.push_back(makeZTriplet(24, 24, 0));
	eta3.push_back(makeZTriplet(32, 32, 0));
	eta3.push_back(makeZTriplet(32, 32, 0));
	eta3.push_back(makeZTriplet(40, 0, 0));
	eta3.push_back(makeZTriplet(40, 0, 0));
	eta3.push_back(makeZTriplet(24, 24, 0));
	eta3.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta3);
	
	//TYPE 4
	vector<vector<int> > eta4(0);
	eta4.push_back(makeZTriplet(24, 24, 0));
	eta4.push_back(makeZTriplet(32, 32, 0));
	eta4.push_back(makeZTriplet(32, 32, 0));
	eta4.push_back(makeZTriplet(40, 0, 0));
	eta4.push_back(makeZTriplet(40, 0, 0));
	eta4.push_back(makeZTriplet(24, 24, 0));
	eta4.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta4);	
		
	//TYPE 5
	vector<vector<int> > eta5(0);
	eta5.push_back(makeZTriplet(32, 32, 0));
	eta5.push_back(makeZTriplet(24, 24, 8));
	eta5.push_back(makeZTriplet(24, 24, 0));
	eta5.push_back(makeZTriplet(24, 24, 8));
	eta5.push_back(makeZTriplet(32, 0, 0));
	eta5.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta5);
	
	//TYPE 6
	vector<vector<int> > eta6(0);
	eta6.push_back(makeZTriplet(40, 0, 0));
	eta6.push_back(makeZTriplet(32, 32, 0));
	eta6.push_back(makeZTriplet(32, 32, 0));
	eta6.push_back(makeZTriplet(40, 0, 0));
	eta6.push_back(makeZTriplet(40, 0, 0));
	eta6.push_back(makeZTriplet(24, 24, 0));
	eta6.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta6);
	
	//TYPE 7
	vector<vector<int> > eta7(0);
	eta7.push_back(makeZTriplet(40, 40, 0));
	eta7.push_back(makeZTriplet(32, 32, 0));
	eta7.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta7);
	
	//TYPE 8
	vector<vector<int> > eta8(0);
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(40, 0, 0));
	eta8.push_back(makeZTriplet(24, 24, 0));
	eta8.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta8);
	
	//TYPE 9
	vector<vector<int> > eta9(0);
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(40, 0, 0));
	eta9.push_back(makeZTriplet(40, 0, 0));
	eta9.push_back(makeZTriplet(24, 24, 0));
	eta9.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta9);
	
	//TYPE 10
	vector<vector<int> > eta10(0);
	eta10.push_back(makeZTriplet(24, 24, 0));
	eta10.push_back(makeZTriplet(32, 32, 0));
	eta10.push_back(makeZTriplet(32, 32, 0));
	eta10.push_back(makeZTriplet(40, 0, 0));
	eta10.push_back(makeZTriplet(40, 0, 0));
	eta10.push_back(makeZTriplet(24, 24, 0));
	eta10.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta10);
	
	//TYPE 11
	vector<vector<int> > eta11(0);
	eta11.push_back(makeZTriplet(24, 24, 0));
	eta11.push_back(makeZTriplet(32, 32, 0));
	eta11.push_back(makeZTriplet(32, 32, 0));
	eta11.push_back(makeZTriplet(40, 0, 0));
	eta11.push_back(makeZTriplet(24, 24, 0));
	eta11.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta11);
	
	//TYPE 12
	vector<vector<int> > eta12(0);
	eta12.push_back(makeZTriplet(40, 40, 0));
	eta12.push_back(makeZTriplet(32, 32, 0));
	eta12.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta12);
	
	//TYPE 13
	vector<vector<int> > eta13(0);
	eta13.push_back(makeZTriplet(32, 32, 0));
	eta13.push_back(makeZTriplet(24, 24, 8));
	eta13.push_back(makeZTriplet(24, 24, 0));
	eta13.push_back(makeZTriplet(24, 24, 8));
	eta13.push_back(makeZTriplet(32, 0, 0));
	eta13.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta13);
	
	//TYPE 14
	vector<vector<int> > eta14(0);
	eta14.push_back(makeZTriplet(32, 32, 0));
	eta14.push_back(makeZTriplet(32, 32, 0));
	eta14.push_back(makeZTriplet(32, 32, 0));
	eta14.push_back(makeZTriplet(40, 0, 0));
	eta14.push_back(makeZTriplet(24, 24, 0));
	eta14.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta14);
	
	//TYPE 15
	vector<vector<int> > eta15(0);
	eta15.push_back(makeZTriplet(24, 24, 0));
	eta15.push_back(makeZTriplet(32, 32, 0));
	eta15.push_back(makeZTriplet(32, 32, 0));
	eta15.push_back(makeZTriplet(40, 0, 0));
	eta15.push_back(makeZTriplet(24, 24, 0));
	eta15.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta15);
	
	//TYPE 16
	vector<vector<int> > eta16(0);
	eta16.push_back(makeZTriplet(40, 40, 0));
	eta16.push_back(makeZTriplet(32, 32, 0));
	eta16.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta16);
	
	//TYPE 17
	vector<vector<int> > eta17(0);
	eta17.push_back(makeZTriplet(40, 40, 0));
	eta17.push_back(makeZTriplet(32, 32, 0));
	eta17.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta17);
	
	//TYPE 18
	vector<vector<int> > eta18(0);
	eta18.push_back(makeZTriplet(32, 0, 0));
	eta18.push_back(makeZTriplet(32, 32, 0));
	eta18.push_back(makeZTriplet(32, 32, 0));
	eta18.push_back(makeZTriplet(40, 0, 0));
	eta18.push_back(makeZTriplet(40, 0, 0));
	eta18.push_back(makeZTriplet(24, 24, 0));
	eta18.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta18);
	
	//TYPE 19
	vector<vector<int> > eta19(0);
	eta19.push_back(makeZTriplet(24, 24, 0));
	eta19.push_back(makeZTriplet(32, 32, 0));
	eta19.push_back(makeZTriplet(32, 32, 0));
	eta19.push_back(makeZTriplet(40, 0, 0));
	eta19.push_back(makeZTriplet(40, 0, 0));
	eta19.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta19);
	
	//TYPE 20
	vector<vector<int> > eta20(0);
	eta20.push_back(makeZTriplet(24, 24, 0));
	eta20.push_back(makeZTriplet(32, 32, 0));
	eta20.push_back(makeZTriplet(32, 32, 0));
	eta20.push_back(makeZTriplet(40, 0, 0));
	eta20.push_back(makeZTriplet(40, 0, 0));
	eta20.push_back(makeZTriplet(24, 24, 0));
	eta20.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta20);
	
	//TYPE 21
	vector<vector<int> > eta21(0);
	eta21.push_back(makeZTriplet(40, 0, 0));
	eta21.push_back(makeZTriplet(32, 32, 0));
	eta21.push_back(makeZTriplet(32, 32, 0));
	eta21.push_back(makeZTriplet(40, 0, 0));
	eta21.push_back(makeZTriplet(40, 0, 0));
	eta21.push_back(makeZTriplet(24, 24, 0));
	eta21.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta21);
	
	//TYPE 22
	vector<vector<int> > eta22(0);
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(40, 0, 0));
	eta22.push_back(makeZTriplet(40, 0, 0));
	eta22.push_back(makeZTriplet(24, 24, 0));
	eta22.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta22);
	
	//TYPE 23
	vector<vector<int> > eta23(0);
	eta23.push_back(makeZTriplet(24, 24, 0));
	eta23.push_back(makeZTriplet(32, 32, 0));
	eta23.push_back(makeZTriplet(32, 32, 0));
	eta23.push_back(makeZTriplet(40, 0, 0));
	eta23.push_back(makeZTriplet(40, 0, 0));
	eta23.push_back(makeZTriplet(24, 24, 0));
	eta23.push_back(makeZTriplet(16, 0, 0));
	layout.push_back(eta23);		
		
	return layout;
	
	

}

/***********************************************************
* TriggerAlgorithm::initRPC3Layout(): creates a vector with
* the number of channels in the various RPC3 chambers
************************************************************/

vector<vector<vector <int> > > TriggerAlgorithm::initRPC3Layout()
{

	vector<vector<vector <int> > > layout(0);
	
	//TYPE 1
	vector<vector<int> > eta1(0);
	eta1.push_back(makeZTriplet(24, 24, 0));	
	eta1.push_back(makeZTriplet(32, 32, 0));	
	eta1.push_back(makeZTriplet(24, 24, 0));	
	eta1.push_back(makeZTriplet(32, 32, 0));	
	eta1.push_back(makeZTriplet(32, 32, 0));	
	eta1.push_back(makeZTriplet(24, 24, 0));	
	layout.push_back(eta1);
	
	//TYPE 2
	vector<vector<int> > eta2(0);
	eta2.push_back(makeZTriplet(32, 32, 0));
	eta2.push_back(makeZTriplet(32, 32, 0));
	eta2.push_back(makeZTriplet(32, 32, 0));
	eta2.push_back(makeZTriplet(32, 32, 0));
	eta2.push_back(makeZTriplet(32, 32, 0));
	eta2.push_back(makeZTriplet(32, 32, 0));
	layout.push_back(eta2);
	
	//TYPE 3
	vector<vector<int> > eta3(0);
	eta3.push_back(makeZTriplet(24, 24, 0));	
	eta3.push_back(makeZTriplet(32, 32, 0));	
	eta3.push_back(makeZTriplet(24, 24, 0));	
	eta3.push_back(makeZTriplet(32, 32, 0));	
	eta3.push_back(makeZTriplet(32, 32, 0));	
	eta3.push_back(makeZTriplet(24, 24, 0));	
	layout.push_back(eta3);
	
	//TYPE 4
	vector<vector<int> > eta4(0);
	eta4.push_back(makeZTriplet(24, 24, 0));
	eta4.push_back(makeZTriplet(24, 24, 0));
	eta4.push_back(makeZTriplet(24, 24, 0));
	eta4.push_back(makeZTriplet(32, 32, 0));
	eta4.push_back(makeZTriplet(32, 32, 0));
	eta4.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta4);
	
	//TYPE 5
	vector<vector<int> > eta5(0);
	eta5.push_back(makeZTriplet(24, 24, 0));
	eta5.push_back(makeZTriplet(32, 32, 0));
	eta5.push_back(makeZTriplet(32, 32, 0));
	eta5.push_back(makeZTriplet(32, 32, 0));
	eta5.push_back(makeZTriplet(32, 32, 0));
	eta5.push_back(makeZTriplet(32, 32, 0));
	layout.push_back(eta5);
	
	//TYPE 6
	vector<vector<int> > eta6(0);
	eta6.push_back(makeZTriplet(24, 24, 0));
	eta6.push_back(makeZTriplet(32, 32, 0));
	eta6.push_back(makeZTriplet(24, 24, 0));
	eta6.push_back(makeZTriplet(32, 32, 0));
	eta6.push_back(makeZTriplet(32, 32, 0));
	eta6.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta6);	
	
	//TYPE 7
	vector<vector<int> > eta7(0);
	eta7.push_back(makeZTriplet(24+32, 32, 24)); //Added BOG chamber for eta=0 and eta = 1
	eta7.push_back(makeZTriplet(32, 32, 24)); // Added BOG chamber for eta=2
	eta7.push_back(makeZTriplet(24, 24, 24)); //Added BOG chamber for eta=3
	eta7.push_back(makeZTriplet(40, 40, 0)); //Added BOG chamber for eta=4
	layout.push_back(eta7);
	
	//TYPE 8
	vector<vector<int> > eta8(0);
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(24, 24, 0));
	eta8.push_back(makeZTriplet(24, 24, 0));
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(32, 32, 0));
	eta8.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta8);
	
	//TYPE 9
	vector<vector<int> > eta9(0);
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(24, 24, 0));
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(32, 32, 0));
	eta9.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta9);
	
	//TYPE 10
	vector<vector<int> > eta10(0);
	eta10.push_back(makeZTriplet(24, 32, 0));
	eta10.push_back(makeZTriplet(32, 32, 0));
	eta10.push_back(makeZTriplet(24, 24, 0));
	eta10.push_back(makeZTriplet(32, 32, 0));
	eta10.push_back(makeZTriplet(32, 32, 0));
	eta10.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta10);
	
	//TYPE 11
	vector<vector<int> > eta11(0);
	eta11.push_back(makeZTriplet(24, 24, 0));
	eta11.push_back(makeZTriplet(24, 24, 0));
	eta11.push_back(makeZTriplet(24, 24, 0));
	eta11.push_back(makeZTriplet(32, 32, 0));
	eta11.push_back(makeZTriplet(32, 32, 0));
	eta11.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta11);
	
	//TYPE 12
	vector<vector<int> > eta12(0);
	eta12.push_back(makeZTriplet(24+32, 32, 24)); //Added BOG for eta=0 and eta=1
	eta12.push_back(makeZTriplet(32, 32, 24)); //Added BOG for eta=2
	eta12.push_back(makeZTriplet(24, 24, 24)); //Added BOG for eta=3
	eta12.push_back(makeZTriplet(40, 40, 0)); //Added BOG for eta=4
	layout.push_back(eta12);
	
	//TYPE 13
	vector<vector<int> > eta13(0);
	eta13.push_back(makeZTriplet(32, 32, 0));
	eta13.push_back(makeZTriplet(32, 32, 0));
	eta13.push_back(makeZTriplet(32, 32, 0));
	eta13.push_back(makeZTriplet(32, 32, 0));
	eta13.push_back(makeZTriplet(32, 32, 0));
	eta13.push_back(makeZTriplet(32, 32, 0));
	layout.push_back(eta13);
	
	//TYPE 14
	vector<vector<int> > eta14(0);
	eta14.push_back(makeZTriplet(32, 32, 0));	
	eta14.push_back(makeZTriplet(24, 24, 0));	
	eta14.push_back(makeZTriplet(24, 24, 0));	
	eta14.push_back(makeZTriplet(32, 32, 0));	
	eta14.push_back(makeZTriplet(32, 32, 0));	
	eta14.push_back(makeZTriplet(24, 24, 0));	
	layout.push_back(eta14);
	
	//TYPE 15
	vector<vector<int> > eta15(0);
	eta15.push_back(makeZTriplet(24, 24, 0));
	eta15.push_back(makeZTriplet(24, 24, 0));
	eta15.push_back(makeZTriplet(24, 24, 0));
	eta15.push_back(makeZTriplet(32, 32, 0));
	eta15.push_back(makeZTriplet(32, 32, 0));
	eta15.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta15);
	
	//TYPE 16
	vector<vector<int> > eta16(0);
	eta16.push_back(makeZTriplet(24+32, 32, 24)); //Added BOG for eta=0 and eta =1
 	eta16.push_back(makeZTriplet(32, 32, 24)); //Added BOG for eta=2
	eta16.push_back(makeZTriplet(24, 24, 24)); //Added BOG for eta=3
	eta16.push_back(makeZTriplet(40, 40, 0)); //Added BOG for eta=4
	layout.push_back(eta16);
	
	//TYPE 17
	vector<vector<int> > eta17(0);
	eta17.push_back(makeZTriplet(24+32, 32, 24)); //Added BOG for eta=0 and eta=1
	eta17.push_back(makeZTriplet(32, 32, 24)); //Added BOG for eta=2
	eta17.push_back(makeZTriplet(24, 24, 24)); //Added BOG for eta=3
	eta17.push_back(makeZTriplet(40, 40, 0)); //Added BOG for eta=4
	layout.push_back(eta17);
	
	//TYPE 18
	vector<vector<int> > eta18(0);
	eta18.push_back(makeZTriplet(24, 24, 0));
	eta18.push_back(makeZTriplet(32, 32, 0));
	eta18.push_back(makeZTriplet(24, 24, 0));
	eta18.push_back(makeZTriplet(32, 32, 0));
	eta18.push_back(makeZTriplet(32, 32, 0));
	eta18.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta18);
	
	//TYPE 19
	vector<vector<int> > eta19(0);
	eta19.push_back(makeZTriplet(24, 24, 0));
	eta19.push_back(makeZTriplet(32, 32, 0));
	eta19.push_back(makeZTriplet(24, 24, 0));
	eta19.push_back(makeZTriplet(32, 32, 0));
	eta19.push_back(makeZTriplet(32, 32, 0));
	eta19.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta19);
	
	//TYPE 20
	vector<vector<int> > eta20(0);
	eta20.push_back(makeZTriplet(24, 24, 0));
	eta20.push_back(makeZTriplet(24, 24, 0));
	eta20.push_back(makeZTriplet(24, 24, 0));
	eta20.push_back(makeZTriplet(32, 32, 0));
	eta20.push_back(makeZTriplet(32, 32, 0));
	eta20.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta20);
	
	//TYPE 21
	vector<vector<int> > eta21(0);
	eta21.push_back(makeZTriplet(24, 24, 0));
	eta21.push_back(makeZTriplet(32, 32, 0));
	eta21.push_back(makeZTriplet(24, 24, 0));
	eta21.push_back(makeZTriplet(32, 32, 0));
	eta21.push_back(makeZTriplet(32, 32, 0));
	eta21.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta21);
	
	//TYPE 22
	vector<vector<int> > eta22(0);
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(24, 24, 0));
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(32, 32, 0));
	eta22.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta22);
	
	//TYPE 23
	vector<vector<int> > eta23(0);
	eta23.push_back(makeZTriplet(24, 32, 0));
	eta23.push_back(makeZTriplet(32, 32, 0));
	eta23.push_back(makeZTriplet(24, 24, 0));
	eta23.push_back(makeZTriplet(32, 32, 0));
	eta23.push_back(makeZTriplet(32, 32, 0));
	eta23.push_back(makeZTriplet(24, 24, 0));
	layout.push_back(eta23);
	
						
	return layout;

}


/******************************************************************
* TriggerAlgorithm::sameTower(): takes two hit identifiers and
* returns true if they occur in the same trigger tower
*******************************************************************/


bool TriggerAlgorithm::sameTower(MuonCalib::MuonFixedId id1, MuonCalib::MuonFixedId id2)
{
	int rpc_num1 = getRPCNum(id1);
	int rpc_num2 = getRPCNum(id2);
	
	
	if(id1.rpcMeasuresPhi()!=id2.rpcMeasuresPhi()){
		bool same_sector = (getSectorType(id1)==getSectorType(id2));
		bool same_eta = (fabs(id1.eta()-id2.eta())<=1);
		bool ret = (same_sector&&same_eta);
		
		return ret;
		
	}
	if(id1.rpcMeasuresPhi()==1&&id2.rpcMeasuresPhi()==1){
		if((getSectorType(id1)==getSectorType(id2))&&(fabs(id1.eta()-id2.eta())<=1)){
			return true;			
		}else{
			return false;
		}
	}
	vector<pair<int, int> > towers1 = towerCoord(id1, rpc_num1);
	vector<pair<int, int> > towers2 = towerCoord(id2, rpc_num2);

	int size1 = towers1.size();
	int size2 = towers2.size();
	
	for(int i=0; i<size1; i++){
		pair<int, int> curr_coord = towers1.at(i);
		int phi1 = curr_coord.first;
		int tower_num1 = curr_coord.second;
		
		for(int j=0; j<size2; j++){
			pair<int, int> comp_coord = towers2.at(j);	
		
			int phi2 = comp_coord.first;
			int tower_num2 = comp_coord.second;
			
			if(phi1==phi2&&tower_num1==tower_num2)
				return true;	
		}
	}
	
	return false;
	
}

/*************************************************************
* TriggerAlgorithm::makeZTriplet(): makes a vector consiting
* of the three values given
*************************************************************/


vector<int> TriggerAlgorithm::makeZTriplet(int one, int two, int three)
{
	vector<int> ret(0);
	ret.push_back(one);
	ret.push_back(two);
	ret.push_back(three);
	
	return ret;
}

/***********************************************************
* TriggerAlgorithm::initSectorGeomMapping(): makes a vector
* that maps sector logic to geometry type
***********************************************************/

vector<int> TriggerAlgorithm::initSectorGeomMapping()
{
	vector<int> map(0);
	map.push_back(18);
	map.push_back(2);
	map.push_back(2);
	map.push_back(3);
	map.push_back(19);
	map.push_back(2);
	map.push_back(2);
	map.push_back(4);
	map.push_back(20);
	map.push_back(2);
	map.push_back(2);
	map.push_back(1);
	map.push_back(18);
	map.push_back(5);
	map.push_back(5);
	map.push_back(6);

	map.push_back(21);
	map.push_back(13);
	map.push_back(13);
	map.push_back(6);
	map.push_back(21);
	map.push_back(7);
	map.push_back(16);
	map.push_back(8);
	map.push_back(14);
	map.push_back(7);
	map.push_back(16);
	map.push_back(6);
	map.push_back(21);
	map.push_back(13);
	map.push_back(13);
	map.push_back(1);

	map.push_back(9);
	map.push_back(2);
	map.push_back(2);
	map.push_back(22);
	map.push_back(9);
	map.push_back(2);
	map.push_back(2);
	map.push_back(23);
	map.push_back(10);
	map.push_back(2);
	map.push_back(2);
	map.push_back(18);
	map.push_back(1);
	map.push_back(5);
	map.push_back(5);
	map.push_back(18);

	map.push_back(1);
	map.push_back(13);
	map.push_back(13);
	map.push_back(21);
	map.push_back(6);
	map.push_back(17);
	map.push_back(12);
	map.push_back(15);
	map.push_back(11);
	map.push_back(17);
	map.push_back(12);
	map.push_back(21);
	map.push_back(6);
	map.push_back(13);
	map.push_back(13);
	map.push_back(22);

	return map;	
}

/********************************************************************
* TriggerAlgorithm::getGlobChannel(): Computes the global channel
* number of a strip in eta to use for tower assignment
********************************************************************/

//Will only be called if the hit is an eta hit
int TriggerAlgorithm::getGlobChannel(MuonCalib::MuonFixedId id, int rpc_num)
{

	vector<vector<vector<int> > > layout(0);
	
	if(rpc_num==1){
		layout = getRPC1Layout();
	}else if(rpc_num==2){
		layout = getRPC2Layout();
	}else if(rpc_num==3){
		layout = getRPC3Layout();
	}
	
	int geom = getGeomType(getSectorType(id));

	
	int strip = id.rpcStrip();
	int eta = fabs(id.eta());
//	int doubletZ = id.rpcDoubletZ();
	int globChan = 0;
	vector<vector<int> > eta_layout = layout.at(geom-1);
	
	for(int i=0; i<eta-1; i++){
	
		//printf("eta: %d, limit: %d, rpc_sector: %d, geom: %d, rpc_num: %d, i: %d, size: %d\n", eta, doubletZ, getSectorType(id), geom, rpc_num, i, eta_layout.size());	

		vector<int> strips = eta_layout.at(i);
		
		int limit = 4;
		
		for(int j=0; j<limit-1; j++){
			globChan+=strips.at(j);
		}			
	}
	
	globChan+=(strip);
	
	return globChan;	
}

/************************************************************
* TriggerAlgorithm::getGeomType(): returns the geometry
* type corresponding to the inputted sector logic
***********************************************************/

int TriggerAlgorithm::getGeomType(int sector_type)
{

	vector<int> map = getSectorGeomMapping();
	
	return map.at(sector_type);
}

/***********************************************************
* TriggerAlgorithm::getSectorType(): returns the sector
* logic corresponding to hte given hit ID
************************************************************/

int TriggerAlgorithm::getSectorType(MuonCalib::MuonFixedId id)
{

	int eta = id.eta();
	int phi = id.phi();
	int station = id.stationName();
	int sector_num = 0;
	int doubletPhi = id.rpcDoubletPhi();
	int offset = -1;
	
	if(station>=3&&station<=6){
		if(station%2!=0){//BML or BOL
			sector_num = 2*phi-1;						
		}else{ //BMS or BOS
			sector_num = 2*phi;
		}
	}else if(station>=9&&station<=11){ //BMF, BOF, and BOG
		sector_num = 2*phi;		
	}
	
	if(eta<0)
		offset = 0;
	else
		offset = 32;
	
	int rpc_sect = 2*sector_num - 2;
	
	if(doubletPhi==1)
		rpc_sect--;
	
	
	if(rpc_sect==-1){
		if(offset==0)
			rpc_sect=31;
		else{
			rpc_sect=63;
			return rpc_sect;
		}
	}
		
	
	rpc_sect+=offset;
	
	return rpc_sect;
}


vector<vector <vector <int> > > TriggerAlgorithm::getLowPtMapping()
{
	return low_pt_map;
}

vector<vector <vector <int > > > TriggerAlgorithm::getHighPtMapping()
{
	return high_pt_map;
}

vector<vector <vector <int> > > TriggerAlgorithm::getRPC1Layout()
{
	return rpc1_layout;
}

vector<vector <vector <int> > > TriggerAlgorithm::getRPC2Layout()
{
	return rpc2_layout;
}

vector<vector <vector <int> > > TriggerAlgorithm::getRPC3Layout()
{
	return rpc3_layout;
}

vector<int> TriggerAlgorithm::getSectorGeomMapping()
{
	return sector_geom_map;
}

                  
      
