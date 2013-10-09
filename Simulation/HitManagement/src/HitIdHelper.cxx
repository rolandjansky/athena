/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HitManagement/HitIdHelper.h"
#include <iostream>
#include <cmath>
#include <assert.h>

void HitIdHelper::InitializeField(std::string n, int vmn, int vmx)
{
	int field = vmx -vmn+1;
	int nb=1;
	while (field>pow(2,nb)) nb++;
	IdField id={n,vmn,vmx,nb,currentStorage+1};
	IDs.push_back(id);
	currentStorage+=nb;
	assert (currentStorage<33);
}

void HitIdHelper::InitializeField(std::string n,int nb)
{
	int vmn=0;
	int vmx=int(pow(2,nb))-1;
	IdField id={n,vmn,vmx,nb,currentStorage+1};
	IDs.push_back(id);
	currentStorage+=nb;
	assert (currentStorage<33);
}
void HitIdHelper::SetFieldValue(std::string name, int n)
{
	for (unsigned int i=0;i<IDs.size();i++)
	{
		if (IDs[i].fieldName==name) 
		{
			if (n<IDs[i].vMin || n>IDs[i].vMax)
				std::cout<<"HitIdHelper: field "<<name<<": supplied value "<<n<<
				" is not within the allowed range ["<<IDs[i].vMin<<","<<IDs[i].vMax
				<<"] : the result of this action is unpredictable "<<std::endl;
			n-=IDs[i].vMin;
			Store(_id,n,IDs[i].firstBit,IDs[i].nBits);
			break;
		}
	}
}

int  HitIdHelper::GetFieldValue(std::string name)
{
	for (unsigned int i=0;i<IDs.size();i++)
	{
		if (IDs[i].fieldName==name) 
		{
			int n=Retrieve(_id,IDs[i].firstBit,IDs[i].nBits);
			return n+IDs[i].vMin;
		}
	}
	std::cout<<"HitIdHelper: field "<<name<<" not found: returning 0"<<
	std::endl;
	return 0;
}

void HitIdHelper::PrintFields()
{
	for (unsigned int i=0;i<IDs.size();i++)
	{
		std::cout<<"-- Field "<<IDs[i].fieldName<<"\t["<<IDs[i].vMin
		         <<","<<IDs[i].vMax<<"]\tnBits "<<IDs[i].nBits
			 <<"\tfirst Bit "<<IDs[i].firstBit<<std::endl;
	}
}

void HitIdHelper::Print(int id)
{
	int l=1;
	for (unsigned int i=0;i<8*sizeof(int);i++)
	{
		if ((id & (l<<i))) std::cout<<"1";
		else std::cout<<"0";
	}
	std::cout<<std::endl;
}

void HitIdHelper::SetBit(int& i, int bitpos)
{
	int l=1;
	l<<=(bitpos-1);
	i|=l;
}
void HitIdHelper::UnsetBit(int& i, int bitpos)
{
	int l=0;
	SetBit(l,bitpos);
	int k=~l;
	i&=k;
}

void HitIdHelper::Store(int& id, int value, int firstbit, int field)
{
	int l=0;
	for (int i=0;i<field;i++)
		UnsetBit(id,i+firstbit);
	l=value<<(firstbit-1);
	id |= l;
}

int  HitIdHelper::Retrieve(int id, int first, int field)
{
	int j=0;
	for (int i=0;i<field;i++)
		SetBit(j,(i+first));
	int l=id&j;
	return l>>(first-1);
}

