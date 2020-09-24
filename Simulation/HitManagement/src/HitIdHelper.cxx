/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HitManagement/HitIdHelper.h"
#include <iostream>
#include <cmath>
#include <assert.h>

void HitIdHelper::InitializeField(const std::string& n, int vmn, int vmx)
{
  int field = vmx -vmn+1;
  int nb=1;
  while (field>pow(2,nb)) nb++;
  IdField id={n,vmn,vmx,nb,m_currentStorage+1};
  m_IDs.push_back(id);
  m_currentStorage+=nb;
  assert (m_currentStorage<33);
}

void HitIdHelper::InitializeField(const std::string& n,int nb)
{
  int vmn=0;
  int vmx=int(pow(2,nb))-1;
  IdField id={n,vmn,vmx,nb,m_currentStorage+1};
  m_IDs.push_back(id);
  m_currentStorage+=nb;
  assert (m_currentStorage<33);
}

void HitIdHelper::SetFieldValue(const std::string& name, int n, HitID& targetID) const
{
  for (unsigned int i=0;i<m_IDs.size();i++)
    {
      if (m_IDs[i].fieldName==name)
        {
          if (n<m_IDs[i].vMin || n>m_IDs[i].vMax)
            std::cout<<"HitIdHelper: field "<<name<<": supplied value "<<n<<
              " is not within the allowed range ["<<m_IDs[i].vMin<<","<<m_IDs[i].vMax
                     <<"] : the result of this action is unpredictable "<<std::endl;
          n-=m_IDs[i].vMin;
          Store(targetID,n,m_IDs[i].firstBit,m_IDs[i].nBits);
          break;
        }
    }
}

int HitIdHelper::GetFieldValue(const std::string& name, HitID targetID) const
{
  for (unsigned int i=0;i<m_IDs.size();i++)
    {
      if (m_IDs[i].fieldName==name)
        {
          int n=Retrieve(targetID,m_IDs[i].firstBit,m_IDs[i].nBits);
          return n+m_IDs[i].vMin;
        }
    }
  std::cout<<"HitIdHelper: field "<<name<<" not found: returning 0"<<
    std::endl;
  return 0;
}

void HitIdHelper::PrintFields()
{
  for (unsigned int i=0;i<m_IDs.size();i++)
    {
      std::cout<<"-- Field "<<m_IDs[i].fieldName<<"\t["<<m_IDs[i].vMin
               <<","<<m_IDs[i].vMax<<"]\tnBits "<<m_IDs[i].nBits
               <<"\tfirst Bit "<<m_IDs[i].firstBit<<std::endl;
    }
}

void HitIdHelper::Print(int id)
{
  for (unsigned int i=0;i<8*sizeof(int);i++)
    {
      if ((id & (1u<<i))) std::cout<<"1";
      else std::cout<<"0";
    }
  std::cout<<std::endl;
}

void HitIdHelper::SetBit(int& i, int bitpos) const
{
  int l=1;
  l<<=(bitpos-1);
  i|=l;
}
void HitIdHelper::UnsetBit(int& i, int bitpos) const
{
  int l=0;
  SetBit(l,bitpos);
  int k=~l;
  i&=k;
}

void HitIdHelper::Store(int& id, int value, int firstbit, int field) const
{
  int l=0;
  for (int i=0;i<field;i++)
    UnsetBit(id,i+firstbit);
  l=value<<(firstbit-1);
  id |= l;
}

int  HitIdHelper::Retrieve(int id, int first, int field) const
{
  int j=0;
  for (int i=0;i<field;i++)
    SetBit(j,(i+first));
  int l=id&j;
  return l>>(first-1);
}
