/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MuRoads.h"
#include "TrigmuFast/RoadsDefADM.h"
#include "TrigmuFast/Datatypes.h"
#include "MDTcabling/MDTGeometry.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

using namespace std;

const string RoadsDefADM::s_name = "Roads definitions from RPC track";

RoadsDefADM::RoadsDefADM(MuMessageInterface& msg,
                         const TriggerData& trigger_data, 
			 const TrackFit& track,
		         RoadRegions& roads,
			 const MDTGeometry* mdtgeo, 
			 const MuRoads* muRoads)
    : RoadsDefStd(msg,trigger_data, track, roads, mdtgeo, muRoads)
{
}

RoadsDefADM::~RoadsDefADM(void)
{
}

bool
RoadsDefADM::run(MuonFeatureDetails* det)
{   
    int trigger_sector = m_trig_data.MDT_Sector_for_trigger;
    int overlap_sector = m_trig_data.MDT_Sector_for_overlap;
    int physics_sector = m_trig_data.PhysicsSector;
    int chamber_type   = m_trig_data.LargeSmall;
    
    if (trigger_sector==99)
    {
        trigger_sector = (physics_sector - 1)*2 + chamber_type;
    }

    bool result = true;
    
    result |= processInner(physics_sector,trigger_sector,chamber_type,0);
    result |= processMiddle(physics_sector,trigger_sector,chamber_type,0);
    result |= processOuter(physics_sector,trigger_sector,chamber_type,0);
    result |= processExtended(physics_sector,trigger_sector,chamber_type,0);
    if(det!=0) {
        det->setPos(0,0,m_roads.pos1[0]);
        det->setPos(0,1,m_roads.pos2[0]);
        det->setPos(0,2,m_roads.pos3[0]);
        det->setAw(0, 0, m_roads.aw1[0]);
        det->setBw(0, 0, m_roads.bw1[0]);
        det->setAw(0, 1, m_roads.aw2[0]);
        det->setBw(0, 1, m_roads.bw2[0]);
        det->setAw(0, 2, m_roads.aw3[0]);
        det->setBw(0, 2, m_roads.bw3[0]);
    
        for (int st=0; st<3; ++st) {
            det->setZetaMin(0, st, m_roads.zetaMin[0][st]);
            det->setZetaMax(0, st, m_roads.zetaMax[0][st]);
            det->setRadMin (0, st, m_roads.radMin [0][st]);
            det->setRadMax (0, st, m_roads.radMax [0][st]);
            det->setEtaMin (0, st, m_roads.etaMin[0][st]);
            det->setEtaMax (0, st, m_roads.etaMax[0][st]);
            det->setSt_roads(0, st, m_roads.roads[0][st]);
            det->setSt_phi(0, st, m_roads.phi[0][st]);
            det->setChamber_type_1(0,st,m_roads.ChamberTypes[0][st][0]);
            det->setChamber_type_2(0,st,m_roads.ChamberTypes[0][st][1]);
        }
    }
    
    if(overlap_sector!=99)
    {
        physics_sector = overlap_sector/2 + 1;
        chamber_type   = overlap_sector%2;
        result |= processInner(physics_sector,overlap_sector,chamber_type,1);
        result |= processMiddle(physics_sector,overlap_sector,chamber_type,1);
        result |= processOuter(physics_sector,overlap_sector,chamber_type,1);
        result |= processExtended(physics_sector,overlap_sector,chamber_type,1);
    
        if(det!=0) {
            det->setPos(1,0,m_roads.pos1[1]);
            det->setPos(1,1,m_roads.pos2[1]);
            det->setPos(1,2,m_roads.pos3[1]);
            det->setAw(1, 0, m_roads.aw1[1]);
            det->setBw(1, 0, m_roads.bw1[1]);
            det->setAw(1, 1, m_roads.aw2[1]);
            det->setBw(1, 1, m_roads.bw2[1]);
            det->setAw(1, 2, m_roads.aw3[1]);
            det->setBw(1, 2, m_roads.bw3[1]);
    
            for (int st=0; st<3; ++st) {
                det->setZetaMin(1, st, m_roads.zetaMin[1][st]);
                det->setZetaMax(1, st, m_roads.zetaMax[1][st]);
                det->setRadMin (1, st, m_roads.radMin [1][st]);
                det->setRadMax (1, st, m_roads.radMax [1][st]);
                det->setEtaMin (1, st, m_roads.etaMin[1][st]);
                det->setEtaMax (1, st, m_roads.etaMax[1][st]);
                det->setSt_roads(1, st, m_roads.roads[1][st]);
                det->setSt_phi(1, st, m_roads.phi[1][st]);
                det->setChamber_type_1(1,st,m_roads.ChamberTypes[1][st][0]);
                det->setChamber_type_2(1,st,m_roads.ChamberTypes[1][st][1]);
            }
        }
    }
    
    m_roads.Type = m_track.Type;
    
    return result;
}

bool
RoadsDefADM::processInner(int PhysicsSector,int sector,int type,int tr)
{
    int station = 0;                    // 0 = Inner , 1 = Middle , 2 = Outer
    int special = 0;                    // 0 = Standard , 1 = Special
    
    float& Zmin1 = m_roads.zetaMin[tr][0];
    float& Zmax1 = m_roads.zetaMax[tr][0];
    float& Rmin1 = m_roads.radMin [tr][0];
    float& Rmax1 = m_roads.radMax [tr][0];
    
    int& ty1 = m_roads.ChamberTypes[tr][0][0];
    int& ty2 = m_roads.ChamberTypes[tr][0][1];

    
    if (m_trig_data.isEndcap)
    {
        get_address('E', 'I', PhysicsSector, station, special, type);
	m_mdtgeo->getEstatZ(sector, station, m_trig_data.side, Zmin1, Zmax1);
	m_mdtgeo->getEstatT(sector, station, m_trig_data.side, ty1, ty2);
	Zmin1 *= 10;
	Zmax1 *= 10;
	float aw = m_roads.aw1[tr];
        float bw = m_roads.bw1[tr];
	m_roads.pos1[tr] = station + 3*special + 6*type;
	bool st = false;
	st=find_endcap_road_dim(100.,aw,bw,Zmin1,Zmax1,&Rmin1,&Rmax1);
	if(!st) return false;
	m_roads.phi[tr][0] = compute_endcap_phi(0,tr);
	m_roads.roads[tr][0] = (m_trig_data.tgcInn[2]!=0.)? 100. : 200.;
    }
    else
    {
        // Define Muon Roads dimension for Inner
        get_address('B', 'I', PhysicsSector, station, special, type);
        m_mdtgeo->getBstatR(sector, station, Rmin1, Rmax1);
	m_mdtgeo->getBstatT(sector, station, ty1, ty2);
        road_parameters(Rmin1,Rmax1,'I',tr);
	float aw = m_roads.aw1[tr];
        float bw = m_roads.bw1[tr];
        m_roads.pos1[tr] = station + 3*special + 6*type;
	bool st = false;
        st=find_barrel_road_dim(m_roads.pos1[tr],m_track.Type,aw,bw,Rmin1,Rmax1,
	                 &Zmin1,&Zmax1);
        if(!st) return false;
	m_roads.phi[tr][0] = compute_barrel_phi(0,tr);
	m_roads.roads[tr][0] = 
	             (m_muRoads->max_roads())[m_track.Type][m_roads.pos1[tr]];
    }
    
    find_eta_min_max(Zmin1,Rmin1,Zmax1,Rmax1,
                     m_roads.etaMin[tr][0],m_roads.etaMax[tr][0]);
    
    return true;
}

bool
RoadsDefADM::processMiddle(int PhysicsSector,int sector,int type,int tr)
{
    int station = 0;                    // 0 = Inner , 1 = Middle , 2 = Outer
    int special = 0;                    // 0 = Standard , 1 = Special
    
    float& Zmin2 = m_roads.zetaMin[tr][1];
    float& Zmax2 = m_roads.zetaMax[tr][1];
    float& Rmin2 = m_roads.radMin [tr][1];
    float& Rmax2 = m_roads.radMax [tr][1];
    
    int& ty1 = m_roads.ChamberTypes[tr][1][0];
    int& ty2 = m_roads.ChamberTypes[tr][1][1];

    float aw = m_roads.aw2[tr];
    float bw = m_roads.bw2[tr];
    
    if (m_trig_data.isEndcap)
    {        
        get_address('E', 'M', PhysicsSector, station, special, type);
	//m_mdtgeo->getEstatZ(sector,station,m_trig_data.side,Zmin2,Zmax2);
	//Zmin2 *= 10;
	//Zmax2 *= 10;	
	m_mdtgeo->getEstatT(sector, station, m_trig_data.side, ty1, ty2);
	if(m_trig_data.tgcMid1[3] < m_trig_data.tgcMid2[3])
	{
	    Zmin2 = m_trig_data.tgcMid1[3];
	    Zmax2 = m_trig_data.tgcMid2[3];
	}
	else
	{
	    Zmin2 = m_trig_data.tgcMid2[3];
	    Zmax2 = m_trig_data.tgcMid1[3];
	}
	
	m_roads.pos2[tr] = station + 3*special + 6*type;
	bool st = false;
	st=find_endcap_road_dim(100.,aw,bw,Zmin2,Zmax2,&Rmin2,&Rmax2);
	if(!st) return false;
        m_roads.phi[tr][1] = compute_endcap_phi(1,tr);
	m_roads.roads[tr][1] = 100.;
    }
    else
    {    
        // Define Muon Roads dimension for Middle
        get_address('B', 'M', PhysicsSector, station, special, type);
        m_mdtgeo->getBstatR(sector, station, Rmin2, Rmax2);
	m_mdtgeo->getBstatT(sector, station, ty1, ty2);
        m_roads.pos2[tr] = station + 3*special + 6*type;
        bool st = false;
        st=find_barrel_road_dim(m_roads.pos2[tr],m_track.Type,aw,bw,Rmin2,Rmax2,
	                 &Zmin2,&Zmax2);
        if(!st) return false;
	m_roads.phi[tr][1] = compute_barrel_phi(1,tr);
	m_roads.roads[tr][1] = 
	             (m_muRoads->max_roads())[m_track.Type][m_roads.pos2[tr]];
    }
    
    find_eta_min_max(Zmin2,Rmin2,Zmax2,Rmax2,
                     m_roads.etaMin[tr][1],m_roads.etaMax[tr][1]);
        
    return true;
}

bool
RoadsDefADM::processOuter(int PhysicsSector,int sector,int type,int tr)
{
    int station = 0;                    // 0 = Inner , 1 = Middle , 2 = Outer
    int special = 0;                    // 0 = Standard , 1 = Special
    
    float& Zmin3 = m_roads.zetaMin[tr][2];
    float& Zmax3 = m_roads.zetaMax[tr][2];
    float& Rmin3 = m_roads.radMin [tr][2];
    float& Rmax3 = m_roads.radMax [tr][2];
    
    int& ty1 = m_roads.ChamberTypes[tr][2][0];
    int& ty2 = m_roads.ChamberTypes[tr][2][1];
    
    if (m_trig_data.isEndcap)
    {
        get_address('E','O',PhysicsSector,station,special,type);
	m_mdtgeo->getEstatZ(sector, station, m_trig_data.side, Zmin3, Zmax3);
	m_mdtgeo->getEstatT(sector, station, m_trig_data.side, ty1, ty2);
	Zmin3 *= 10;
	Zmax3 *= 10;
	float aw = m_roads.aw3[tr];
        float bw = m_roads.bw3[tr];
	m_roads.pos3[tr] = station + 3*special + 6*type;
	bool st = false;
	st=find_endcap_road_dim(100.,aw,bw,Zmin3,Zmax3,&Rmin3,&Rmax3);
	if(!st) return false;
	m_roads.phi[tr][2] = compute_endcap_phi(2,tr);
	m_roads.roads[tr][2] = 100.;
    }
    else
    {    
        // Define Muon Roads dimension for Outer
        get_address('B','O',PhysicsSector,station,special,type);
        m_mdtgeo->getBstatR(sector, station, Rmin3, Rmax3);
	m_mdtgeo->getBstatT(sector, station, ty1, ty2);
        road_parameters(Rmin3,Rmax3,'O',tr);
	float aw = m_roads.aw3[tr];
        float bw = m_roads.bw3[tr];
        m_roads.pos3[tr] = station + 3*special + 6*type;
        bool st = false;
        st=find_barrel_road_dim(m_roads.pos3[tr],m_track.Type,aw,bw,Rmin3,Rmax3,
	                 &Zmin3,&Zmax3);
        if(!st) return false;
	m_roads.phi[tr][2] = compute_barrel_phi(2,tr);
	m_roads.roads[tr][2] = 
	             (m_muRoads->max_roads())[m_track.Type][m_roads.pos3[tr]];
    }
    
    find_eta_min_max(Zmin3,Rmin3,Zmax3,Rmax3,
                     m_roads.etaMin[tr][2],m_roads.etaMax[tr][2]);

    return true;
}

bool
RoadsDefADM::processExtended(int /* PhysicsSector */,int /* sector */,int /* type */,int tr)
{
    //int station = 0;                    // 0 = Inner , 1 = Middle , 2 = Outer
    //int special = 0;                    // 0 = Standard , 1 = Special
    
    
    //dummy use to avoid warnings
    // CID 22992,993,994: PARAM_SET_BUT_NOT_USED 
    // PhysicsSector = 0; 
    // sector        = 0;
    // type          = 0;
    
    float& Zmin4 = m_roads.zetaMin[tr][3];
    float& Zmax4 = m_roads.zetaMax[tr][3];
    float& Rmin4 = m_roads.radMin [tr][3];
    float& Rmax4 = m_roads.radMax [tr][3];
    
    if (m_trig_data.isEndcap)
    {
        //temporary
        return true;
    }
    else
    {    
        // no extended station for the barrel
        return true;
    }
    
    find_eta_min_max(Zmin4,Rmin4,Zmax4,Rmax4,
                     m_roads.etaMin[tr][3],m_roads.etaMax[tr][3]);

    return true;
}

int
RoadsDefADM::road_parameters(float Rmin, float Rmax, char s, int tr)
{    
    float e0,dd,del,xx1,xx2,dy,dx;
    
    dy = Rmax - Rmin;
    e0 = Rmin-m_track.y1;
    dd = m_track.aq - 4*(e0*e0+m_track.b*e0);
    if(dd>=0.) {
        del = sqrt(dd);
        xx1 = -m_track.a2 + m_track.is*del/2.;
        e0 = Rmax - m_track.y1;
        dd = m_track.aq - 4*(e0*e0+m_track.b*e0);
        if(dd>=0.) {
            del = sqrt(dd);
            xx2 = -m_track.a2 + m_track.is*del/2.;
            dx = xx2-xx1;
            if(s=='I') {
                if(dx) {
                    m_roads.aw1[tr] = dy/dx;
                    m_roads.bw1[tr] = Rmin - m_roads.aw1[tr]*(xx1+m_track.x1);
                } else {
                    m_roads.aw1[tr] = 0.;
                    m_roads.bw1[tr] = xx1+m_track.x1;
                }
                return 0;
            } else if(s=='O') {
                if(dx) {
                    m_roads.aw3[tr] = dy/dx;
                    m_roads.bw3[tr] = Rmin - m_roads.aw3[tr]*(xx1+m_track.x1);
                } else {
                    m_roads.aw3[tr] = 0.;
                    m_roads.bw3[tr] = xx1+m_track.x1;
                }
                return 0;
            }
        } return 1;
    } else return 1;
}
