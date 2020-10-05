/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>

#include "egammaConditions/EMDatabaseID.h"

EMDatabaseID::EMDatabaseID()
{
  clear();
}
	
EMDatabaseID::EMDatabaseID(const EMDatabaseID& ob)
{
  //  clear();
  set(ob);
}
	
EMDatabaseID::EMDatabaseID(const std::string& s)
{
  //  clear();
  setUniqueID(s);
}

/** Constructor via unique id-string*/
EMDatabaseID::EMDatabaseID(EMDatabaseIDDescriptor &id)
{
  if (id.SimSWV.empty())	set(id.Object, id.Container, id.Type, id.Channel, id.Author, id.RecoSWV, id.Tag, id.runStart, id.runEnd);
  else			set(id.Object, id.Container, id.Type, id.Channel, id.Author, id.RecoSWV, id.Tag, id.SimSWV);
}

EMDatabaseID::EMDatabaseID(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, long start, long end)
{
  //  clear(); // not needed; set overwrites everything
  set(Object, Container, Type, Channel, Author, RecoSWV, Tag, start, end);
}
	
EMDatabaseID::EMDatabaseID(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, const std::string& SimSWV)
{
  //  clear();
  set(Object, Container, Type, Channel, Author, RecoSWV, Tag, SimSWV);
}

EMDatabaseID::EMDatabaseID(const std::string& Object, const std::string& Type, const std::string& Tag)
{
  //  clear();
  set(Object, "", Type, "", "", "", Tag, "");
}
	
EMDatabaseID::~EMDatabaseID()
{
  clear();
}

EMDatabaseID& EMDatabaseID::operator = (const EMDatabaseID& ob)
{
  set(ob);
  return *this;
}

bool EMDatabaseID::operator == (const EMDatabaseID& ob) const
{
  if (m_idDes.Object	!= ob.m_idDes.Object)		return false;
  if (m_idDes.Container	!= ob.m_idDes.Container)	return false;
  if (m_idDes.Type	!= ob.m_idDes.Type)		return false;
  if (m_idDes.Channel	!= ob.m_idDes.Channel)		return false;
  if (m_idDes.Author	!= ob.m_idDes.Author)		return false;
  if (m_idDes.RecoSWV	!= ob.m_idDes.RecoSWV)		return false;
  if (m_idDes.Tag		!= ob.m_idDes.Tag)		return false;
  if (m_idDes.runStart	!= ob.m_idDes.runStart)		return false;
  if (m_idDes.runEnd	!= ob.m_idDes.runEnd)		return false;
  if (m_idDes.SimSWV	!= ob.m_idDes.SimSWV)		return false;
  return true;
}

bool EMDatabaseID::isInSubgroup(const EMDatabaseID& ob) const
{
  if ((m_idDes.Object	!= ob.m_idDes.Object)		&& (!ob.m_idDes.Object.empty()) && (!m_idDes.Object.empty())) 	return false;
  if ((m_idDes.Container!= ob.m_idDes.Container)		&& (!ob.m_idDes.Container.empty()) && (!m_idDes.Container.empty())) 	return false;
  if ((m_idDes.Type	!= ob.m_idDes.Type)		&& (!ob.m_idDes.Type.empty()) && (!m_idDes.Type.empty())) 		return false;
  if ((m_idDes.Channel	!= ob.m_idDes.Channel)		&& (!ob.m_idDes.Channel.empty()) && (!m_idDes.Channel.empty()))	return false;
  if ((m_idDes.Author	!= ob.m_idDes.Author)		&& (!ob.m_idDes.Author.empty()) && (!m_idDes.Author.empty())) 	return false;
  if ((m_idDes.RecoSWV	!= ob.m_idDes.RecoSWV)		&& (!ob.m_idDes.RecoSWV.empty()) && (!m_idDes.RecoSWV.empty()))	return false;
  if ((m_idDes.Tag	!= ob.m_idDes.Tag)		&& (!ob.m_idDes.Tag.empty()) && (!m_idDes.Tag.empty()))	return false;
  return true;
}

void EMDatabaseID::set(const EMDatabaseID& ob)
{
  m_idDes	= ob.m_idDes;
}

void EMDatabaseID::set(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, long start, long end)
{
  m_idDes.Object	= beautify(Object);
  m_idDes.Container	= beautify(Container);
  m_idDes.Type		= beautify(Type);
  m_idDes.Channel	= beautify(Channel);
  m_idDes.Author	= beautify(Author);
  m_idDes.RecoSWV	= beautify(RecoSWV);
  m_idDes.Tag	= beautify(Tag);
  m_idDes.runStart	= start;
  m_idDes.runEnd	= end;
  m_idDes.SimSWV	= "";
}

void EMDatabaseID::set(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, const std::string& SimSWV)
{
  m_idDes.Object	= beautify(Object);
  m_idDes.Container	= beautify(Container);
  m_idDes.Type		= beautify(Type);
  m_idDes.Channel	= beautify(Channel);
  m_idDes.Author	= beautify(Author);
  m_idDes.RecoSWV	= beautify(RecoSWV);
  m_idDes.Tag	= beautify(Tag);
  m_idDes.runStart	= 0;
  m_idDes.runEnd	= 0;
  m_idDes.SimSWV	= beautify(SimSWV);
}

// for object retrieval
void EMDatabaseID::set(const std::string& Object, const std::string& Type, const std::string& Tag)
{
  set(Object, "", Type, "", "", "", Tag, "");
}

void EMDatabaseID::clear()
{
  m_idDes.Object	= "";
  m_idDes.Container	= "";
  m_idDes.Type		= "";
  m_idDes.Channel	= "";
  m_idDes.Author	= "";
  m_idDes.RecoSWV	= "";
  m_idDes.Tag	= "";
  m_idDes.runStart	= 0;
  m_idDes.runEnd	= 0;
  m_idDes.SimSWV	= "";			
}

bool EMDatabaseID::isMCData() const	
{
  return !m_idDes.SimSWV.empty();
}

bool EMDatabaseID::isComplete() const
{	
  if (m_idDes.Object.empty()) 	return false;
  if (m_idDes.Container.empty())	return false;
  if (m_idDes.Type.empty()) 		return false;
  if (m_idDes.Channel.empty())	return false;
  if (m_idDes.Author.empty())	 	return false;
  if (m_idDes.RecoSWV.empty())	return false;
  if (m_idDes.Tag.empty())		return false;
  if ((m_idDes.runStart==0) && (m_idDes.runEnd==0) && (m_idDes.SimSWV.empty()))	return false;
  return true;
}

std::string EMDatabaseID::getUniqueID() const
{
  if (!isComplete())
    {
      std::cout<<"EMDatabaseID::Error: Request UniqueID for non complete object."<<std::endl;
      return "";
    }
  std::string s;
  s = object() + "_" + container() + "_" + type() + "_" + channel() + "_" + author() + "_" + recoSWV() + "_" + tag();
  if (isMCData()) s+="_MC"+m_idDes.SimSWV;
  else s=s+"_Run-"+intToString(runStart())+"-"+intToString(runEnd());
  return s;
}

bool EMDatabaseID::setUniqueID(const std::string& s)
{
  std::string n;
  int step=0;
  for (char i : s)
    {
      if (i=='_')
	{
	  step++;
	  if (step==1) object(n);	
	  if (step==2) container(n);
	  if (step==3) type(n);
	  if (step==4) channel(n);
	  if (step==5) author(n);
	  if (step==6) recoSWV(n);
	  if (step==7) tag(n);
	  n="";
	} else
	{
	  n+=i;	
	}
    }
  step++;
  if (step==8)
    {
      if (setRunOrMCSWV(n)!=0) return true;
    }
  std::cout<<"EMDatabaseID::Warning: Set UniqueID-String is non complete."<<std::endl;
  //clear();
  return false;
}

int EMDatabaseID::setRunOrMCSWV(std::string n)
{
  if (n.substr(0, 2)=="MC")
    {
      n=n.erase(0, 2);
      simSWV(n);
      return 1;
    }
  if (n.substr(0, 3)=="Run")
    {
      n=n.erase(0, 3);
      int tag=0;
      for (unsigned int j=0; j<n.size(); j++)	if (n[j]=='-') tag=j;
      run(atoi((n.substr(1,tag-1)).c_str()),atoi((n.substr(tag+1,n.size())).c_str()));
      return 2;
    }
  return 0;
}

std::string EMDatabaseID::beautify(const std::string& s) const
{
  std::string n;
  for (char i : s)	if ((i!=' ') && (i!='_') && (i!='/') && (i!='\\')) n+=i;
  return n;
}

std::string EMDatabaseID::intToString(long l) const
{
  //  std::string s;
  std::stringstream out;
  out << l;
  return out.str();
}

std::string EMDatabaseID::getCoolChannel() const
{
  return (object() + "_" + type());
}

