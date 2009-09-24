/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include "MuonCalibIdentifier/MuonFixedId.h"
#include <vector>
#include <algorithm>

double rpcEfficiencyMap(int station, int eta, 
		     int phi, int doubletR, int doubletPhi, int doubletZ);
bool isNotUsableRPC(unsigned int hitId);
bool isNotUsableRPCWithHardware(unsigned int hitId, double globIneff);
double getEff();
double effFunc(double x);

void passRPC(vector<unsigned int> &raw_hits) {

  raw_hits.erase(std::remove_if(raw_hits.begin(), 
				raw_hits.end(),
				isNotUsableRPC),
		 raw_hits.end());

}


double rpcEfficiencyMap(int station, int eta, 
			int phi, int /*doubletR*/, 
			int doubletPhi, int doubletZ) {

  // unless in list of not working, return one!

  if((station==3||station==5)&&phi==1&&eta==7)
  	return 0.;

  if((station==3||station==5)&&phi==1&&eta==-7)
  	return 0.;
  
  if((station==3||station==5)&&phi==1&&(eta==-4&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==1&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==2&&doubletPhi==1))
  	return 0.;
  	
  if((station==4||station==6)&&phi==1&&(eta==2&&doubletPhi==2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==3&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==4&&doubletPhi==2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==4&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==5&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==6&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==-2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==-4&&doubletPhi==2))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==-4&&doubletPhi==1&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==1&&(eta==-6&&doubletPhi==2))
  	return 0.;
  	
  if((station==3||station==5)&&phi==2&&((eta>0)&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==2&&(eta==-7||eta==7))
  	return 0.;

  if((station==3||station==5)&&phi==2&&(eta==-4&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==2&&((eta>0)&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==2&&(eta==2&&doubletPhi==2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==2&&(eta==5&&doubletPhi==2&&doubletZ==1))
  	return 0.;

  if((station==4||station==6)&&phi==2&&(eta==-2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==2&&(eta==-4&&doubletZ==3))
  	return 0.;

  if((station==3||station==5)&&phi==3&&(eta==1||/*eta==2||eta==-1||eta==-2||*/eta==7||eta==-7))
  	return 0.;

  /*if((station==3||station==5)&&phi==3&&(eta==5&&doubletPhi==2))
  	return 0.;*/

  if((station==3||station==5)&&phi==3&&(eta==6&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==3&&(eta==-6&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==3)
  	return 0.;

  if((station==3||station==5)&&phi==4&&(eta==5&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==4&&(eta==7||eta==-7))
  	return 0.;

  if((station==3||station==5)&&phi==4&&(eta==-3&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==4&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==-1&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==-2&&doubletZ>=2))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==-4&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==-6&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==5&&(eta==-7||eta==7/*||eta==-2*/))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==2&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==4&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==-1&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==-1&&doubletPhi==2&&doubletZ==1))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==-2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==-4&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==-4&&doubletZ==3))
  	return 0.;

  if((station==3||station==5)&&phi==6&&(eta==4&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==6&&(eta==7||eta==-7))
  	return 0.;

  if((station==3||station==5)&&phi==6&&(eta==-1&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==6&&(eta==-5||eta==-6))
  	return 0.;

  if((station==9||station==10||station==11)&&phi==6&&(eta==4&&doubletPhi==1))
  	return 0.;

  /*if((station==9||station==10||station==11)&&phi==6&&(eta==-3))
  	return 0.;*/
  	
  if((station==3||station==5)&&phi==7&&(eta==2&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==7&&(eta==6||eta==-6))
  	return 0.;
  
  if((station==9||station==10||station==11)&&phi==7&&((eta>=1&&eta<=4)&&doubletPhi==1))
  	return 0.;

  if((station==9||station==10||station==11)&&phi==7&&(eta==4&&doubletPhi==2))
  	return 0.;
  	
  if((station==3||station==5)&&phi==8&&(eta>0&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==8&&eta==6)
  	return 0.;
  	
  if((station==3||station==5)&&phi==8&&(eta==7||eta==-7||eta==-2))
  	return 0.;

  if((station==3||station==5)&&phi==8&&((eta==-1||eta==-3)&&doubletPhi==2))
  	return 0.;

  if((station==4||station==6)&&phi==8&&(eta==1||eta==6||eta==-1||eta==-2))
  	return 0.;

  if((station==4||station==6)&&phi==8&&(doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==8&&(eta==2&&doubletZ==3))
  	return 0.;

  if((station==4||station==6)&&phi==8&&(eta==-4&&doubletZ==3))
  	return 0.;
	
  return 1.;

}

bool isNotUsableRPC(unsigned int hitId){

  MuonCalib::MuonFixedId id(hitId);
  

  if (!id.is_rpc())
    return false;

  // Need to return true if random > efficiency! efficiency is 
  // 1 for good hits: function must return 0, in that case 
  // (hit is not NotUsable!)
  return (((double)rand()/((double)(RAND_MAX)+(double)(1)))>
	  rpcEfficiencyMap(id.stationName(),
			id.eta(),
			id.phi(),
			id.rpcDoubletR(),
			id.rpcDoubletPhi(),
			id.rpcDoubletZ()));
  
}

bool isNotUsableRPCWithHardware(unsigned int hitId, double globIneff)
{
	MuonCalib::MuonFixedId id(hitId);
	
	if(!id.is_rpc())
		return false;
		
	bool off = isNotUsableRPC(hitId);
	
	if(off)
		return true;
	
	double eff = getEff();

	double random = ((double)rand()/((double)(RAND_MAX)+(double)(1)));
	
	if(random>eff)
	{
		return true;
	}
	else
	{
		double random2 = ((double)rand()/((double)(RAND_MAX)+(double)(1)));
		
		if(random2<globIneff)
			return true;
		
		return false;
	}
}

double getEff()
{
	double eff = ((double)rand()/((double)(RAND_MAX)+(double)(1)));

	double select = ((double)rand()/((double)(RAND_MAX)+(double)(1)));
	
	double func_val = effFunc(eff);
	
	if(select<func_val)
		return eff;
	else
		return getEff();
}

double effFunc(double x)
{
	if(x>=0&&x<0.5)
		return 0.002;
	else if(x>=0.5&&x<0.6)
		return 0.004;
	else if(x>=0.6&&x<0.66)
		return 0.008;
	else if(x>=0.66&&x<0.72)
		return 0.01;
	else if(x>=0.72&&x<0.8)
		return 0.014;
	else if(x>=0.8&&x<0.84)
		return 0.018;
	else if(x>=0.84&&x<0.86)
		return 0.02;
	else if(x>=0.86&&x<0.88)
		return 0.024;
	else if(x>=0.88&&x<0.90)
		return 0.030;
	else if(x>=0.90&&x<0.91)
		return 0.040;
	else if(x>=0.91&&x<0.92)
		return 0.06;
	else if(x>=0.92&&x<0.93)
		return 0.07;
	else if(x>=0.93&&x<0.94)
		return 0.08;
	else if(x>=0.94&&x<0.96)
		return 0.06;
	else if(x>=0.96&&x<0.98)
		return 0.04;
	else if(x>=0.98&&x<0.99)
		return 0.02;
	else if(x>=0.99&&x<=1.0)
		return 0.002;
	
	return 1;
}
