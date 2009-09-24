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
double getEff(int area);
double effFuncTop(double x);
double effFuncBot(double x);
int getSector(int stat, int phi);

// MuonFixedId conventions
enum{BIL=1, BIS, BML, BMS,
     BOL, BOS, BEE, BIR,
     BMF, BOF, BOG, BOH,
     BIM, EIC, EIL, EEL,
     EES, EMC, EML, EMS,
     EOC, EOL, EOS, EIS,
     T1F, T1E, T2F, T2E,
     T3F, T3E, T4F, T4E,
     CSS, CSL, XXX};
                                        

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

  if((station==3||station==5)&&phi==1)
  	return 0.;
  
  if((station==4||station==6)&&phi==1)
  	return 0.;
  	
  if((station==3||station==5)&&phi==2&&(eta==-1&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==2&&(eta==-7&&doubletPhi==2))
  	return 0.;
  
  if((station==4||station==6)&&phi==2&&(eta==4&&doubletPhi==2))
  	return 0.;

  if((station==4||station==6)&&phi==2&&(eta==-4&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==3&&(eta==5&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==3&&(eta==-4&&doubletPhi==2))
  	return 0.;
 
  if((station==3||station==5)&&phi==3&&(eta==-5&&doubletPhi==1))
  	return 0.;
  	
  if((station==3||station==5)&&phi==3&&(eta==-6&&doubletPhi==2))
  	return 0.;

  if((station==4||station==6)&&phi==3&&(eta==4))
  	return 0.;

  if((station==4||station==6)&&phi==3&&(eta==5&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==4&&(eta==-4&&doubletPhi==2))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==1&&doubletPhi==1))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==-2&&doubletZ==2))
  	return 0.;

  if((station==4||station==6)&&phi==4&&(eta==-4&&doubletZ>=2&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==5&&(eta==3&&doubletZ==2))
  	return 0.;

  if((station==3||station==5)&&phi==5&&(eta==4&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==5&&(eta==5&&doubletPhi==1))
  	return 0.;

  if((station==3||station==5)&&phi==5&&(eta==-5))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==2&&doubletZ>=2))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==6&&doubletPhi==2))
  	return 0.;

  if((station==4||station==6)&&phi==5&&(eta==-3&&doubletPhi==1))
  	return 0.;
  	
  if((station==4||station==6)&&phi==5&&(eta==-4&&doubletPhi==1&&doubletZ==3))
  	return 0.;

  if((station==3||station==5)&&phi==6&&(eta==4))
  	return 0.;

  if((station==3||station==5)&&phi==6&&(eta==-6&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==7&&(eta==2&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==7&&(eta==-1))
  	return 0.;

  if((station==9||station==10||station==11)&&phi==7&&(eta==3&&doubletPhi==1&&doubletZ==1))
  	return 0.;

  if((station==9||station==10||station==11)&&phi==7&&(eta==4&&doubletPhi==2))
  	return 0.;

  if((station==3||station==5)&&phi==8&&(eta==6&&doubletPhi==1&&doubletZ==1))
  	return 0.;

  if((station==4||station==6)&&phi==8&&(eta==-2&&doubletPhi==2&&doubletZ>=2))
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
	
	int sector = getSector(id.stationName(), id.phi());
	double eff = 0.;
	
	if(sector>=1&&sector<=8)
		eff = getEff(1);
	else if(sector>=9&&sector<=16)
		eff = getEff(2);

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

double getEff(int area)
{
	double eff = ((double)rand()/((double)(RAND_MAX)+(double)(1)));

	double select = ((double)rand()/((double)(RAND_MAX)+(double)(1)));
	
	double func_val = 0.;
	
	if(area==1)
		func_val = effFuncTop(eff);
	else if(area==2)
		func_val = effFuncBot(eff);
	
	func_val*=5;
	
	if(select<func_val)
		return eff;
	else
		return getEff(area);
}

double effFuncTop(double x)
{
	if(x>=0.0&&x<0.7)
		return 0.0056;
	else if(x>=0.7&&x<0.8)
		return 0.011;
	else if(x>=0.8&&x<0.825)
		return 0.017;
	else if(x>=0.825&&x<0.85)
		return 0.028;
	else if(x>=0.85&&x<0.875)
		return 0.034;
	else if(x>=0.875&&x<0.9)
		return 0.062;
	else if(x>=0.90&&x<0.925)
		return 0.107;
	else if(x>=0.925&&x<0.95)
		return 0.230;
	else if(x>=0.95&&x<0.96)
		return 0.198;
	else if(x>=0.96&&x<0.97)
		return 0.164;
	else if(x>=0.97&&x<0.98)
		return 0.085;
	else if(x>=0.98&&x<0.99)
		return 0.034;
	else if(x>=0.99&&x<=1.0)
		return 0.023;
	
	return 1;
}

double effFuncBot(double x)
{
	if(x>=0.0&&x<0.4)
		return 0.006;
	else if(x>=0.4&&x<0.6)
		return 0.018;
	else if(x>=0.6&&x<0.7)
		return 0.030;
	else if(x>=0.70&&x<0.75)
		return 0.036;
	else if(x>=0.75&&x<0.8)
		return 0.048;
	else if(x>=0.8&&x<0.825)
		return 0.060;
	else if(x>=0.825&&x<0.85)
		return 0.077;
	else if(x>=0.85&&x<0.875)
		return 0.107;
	else if(x>=0.875&&x<0.9)
		return 0.131;
	else if(x>=0.9&&x<0.925)
		return 0.137;
	else if(x>=0.925&&x<0.95)
		return 0.119;
	else if(x>=0.95&&x<0.96)
		return 0.113;
	else if(x>=0.96&&x<0.97)
		return 0.071;
	else if(x>=0.97&&x<0.98)
		return 0.030;
	else if(x>=0.98&&x<0.99)
		return 0.012;
	else if(x>=0.99&&x<=1.0)
		return 0.006;
		
	return 1;
}

int getSector(int hitstatname, int hitstatphi)
{
  int statSector = -9;
  if (hitstatname==BIL || 
      hitstatname==BML || 
      hitstatname==BOL) 
    statSector = 2*hitstatphi-1;
  else if (hitstatname==BIS || 
	   hitstatname==BMS || 
	   hitstatname==BOS || 
	   hitstatname==BOF || 
	   hitstatname == BOG || 
	   hitstatname == BMF) 
    statSector = 2*hitstatphi;
  return statSector;
}
           
