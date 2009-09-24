/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_ALGORITHM_H
#define TRIGGER_ALGORITHM_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include <vector>
#include <map>

using std::vector;
using std::map;
using std::pair;
using std::make_pair;

class TriggerAlgorithm
{

	public:
		TriggerAlgorithm(int min_hits_rpc1, int min_hits_rpc2, int min_hits_rpc3, bool both1, bool both2, bool both3, int road_width);
		bool trigger(vector <unsigned int> hit_ids);
		int getRPC1();
		int getRPC2();
		int getRPC3();
		int getRPCNum(MuonCalib::MuonFixedId id);
		int getSectorType(MuonCalib::MuonFixedId id);		
	private:
		int min_h_rpc1;
		int min_h_rpc2;
		int min_h_rpc3;
		int stripsPerPort[3];
		bool both_rpc1;
		bool both_rpc2;
		bool both_rpc3;
		int road_w;
		int num_rpc1;
		int num_rpc2;
		int num_rpc3;
		map<unsigned int, vector<pair<int, int> > > coords;
		vector<vector<vector<int> > > low_pt_map;
		vector<vector<vector<int> > > high_pt_map;
		vector<vector<vector<int> > > rpc1_layout;
		vector<vector<vector<int> > > rpc2_layout;
		vector<vector<vector<int> > > rpc3_layout;
		vector<int> sector_geom_map;
		vector<vector<int> > stat_eta_map;

		void initSats();
		
		void initTowerCoords(vector<unsigned int> hit_ids);
		vector<pair<int, int> > findTowerCoord(MuonCalib::MuonFixedId id, int rpc_num);

		
		bool triggerTower(vector<unsigned int> hit_ids);	
		bool satisfiesRPC(vector <unsigned int> hit_ids, int rpc_num);
		
		vector<pair<int, int> > towerCoord(MuonCalib::MuonFixedId id, int rpc_num);
		
		vector<vector <vector <int> > > getLowPtMapping();
		vector<vector <vector <int > > > getHighPtMapping();
		vector<vector<vector<int> > >  initLowPtMapping();
		vector<vector<vector<int> > > initHighPtMapping();
		
		vector<int> createRange(int low, int high);
		vector<vector <vector <int> > > getRPC1Layout();
		vector<vector<vector <int> > > getRPC2Layout();
		vector<vector<vector <int> > > getRPC3Layout();
		vector<vector<vector<int> > > initRPC1Layout();
		vector<vector<vector<int> > > initRPC2Layout();
		vector<vector<vector<int> > > initRPC3Layout();
		
		bool sameTower(MuonCalib::MuonFixedId id1, MuonCalib::MuonFixedId id2);
		int getGlobChannel(MuonCalib::MuonFixedId id, int rpc_num);
		vector<int> makeZTriplet(int one, int two, int three);
		
		vector<int> getSectorGeomMapping();
		vector<vector<int> > getStationEtaChannelMapping();
		vector<int> initSectorGeomMapping();
		vector<vector<int> > initStationEtaChannelMapping();
		
		int getGeomType(int sector_type);
		
};
#endif // TRIGGER_ALGORITHM_H
