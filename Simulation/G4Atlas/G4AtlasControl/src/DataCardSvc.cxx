/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/DataCardSvc.h"

#include <iostream>

DataCardSvc* DataCardSvc::thePointer=0;

DataCardSvc::DataCardSvc()
{
	std::cout<<"creating the datacard service"<<std::endl;
}

DataCardSvc* DataCardSvc::GetDataCardSvc()
{
	if (!thePointer) thePointer=new DataCardSvc();
	return thePointer;
}

void DataCardSvc::DefineCard(const std::string name, const std::string type, int defValue)
{
	if (type=="i") 
		XInfo[name]=type;
	else
	{
		std::cout<<" wrong type of card: expect 'i' "<<std::endl;
		return;
	}
	theInts[name]=defValue;
}
void DataCardSvc::DefineCard(const std::string name, const std::string type, double defValue)
{
	if (type=="d") 
		XInfo[name]=type;
	else
	{
		std::cout<<" wrong type of card: expect 'd' "<<std::endl;
		return;
	}
	theDoubles[name]=defValue;
}
void DataCardSvc::DefineCard(const std::string name, const std::string type, const std::string defValue)
{
	if (type=="s") 
		XInfo[name]=type;
	else
	{
		std::cout<<" wrong type of card: expect 's' "<<std::endl;
		return;
	}
	theStrings[name]=defValue;
}	
void DataCardSvc::SetCard(const std::string name, int Value)
{
	if (XInfo.find(name)!=XInfo.end())
		if (XInfo[name]=="i") 
			theInts[name]=Value;
		else
			std::cout<<" card "<<name<<" not of type 'i'"<<std::endl;
	else
	{
		XInfo[name]="i";
		theInts[name]=Value;
	}
}
void DataCardSvc::SetCard(const std::string name, double Value)
{
	if (XInfo.find(name)!=XInfo.end())
		if (XInfo[name]=="d") 
			theDoubles[name]=Value;
		else
			std::cout<<" card "<<name<<" not of type 'd'"<<std::endl;
	else
	{
		XInfo[name]="d";
		theDoubles[name]=Value;
	}
}
void DataCardSvc::SetCard(const std::string name, const std::string Value)
{
	if (XInfo.find(name)!=XInfo.end())
		if (XInfo[name]=="s") 
			theStrings[name]=Value;
		else
			std::cout<<" card "<<name<<" not of type 's'"<<std::endl;
	else
	{
		XInfo[name]="s";
		theStrings[name]=Value;
	}
}	
int DataCardSvc::GetCardAsInt(const std::string name)
{
	if (theInts.find(name)!=theInts.end())
		return theInts[name];
	else
	{
//		std::cout<<" integer card "<<name<<" not found: returning 0"<<std::endl;
		return 0;
	}
}
double DataCardSvc::GetCardAsDouble(const std::string name)
{
	if (theDoubles.find(name)!=theDoubles.end())
		return theDoubles[name];
	else
	{
//		std::cout<<" double card "<<name<<" not found: returning 0"<<std::endl;
		return 0.;
	}
}
std::string DataCardSvc::GetCardAsString(const std::string name)
{
	static std::string temp="";
	if (theStrings.find(name)!=theStrings.end())
		return theStrings[name];
	else
	{
//		std::cout<<" string card "<<name<<" not found: returning empty string"<<std::endl;
		return temp;
	}
}
