/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HitIdHelper_H
#define HitIdHelper_H

#include <string>
#include <vector>

typedef int HitID;

struct IdField {
	std::string fieldName;
	int vMin;
	int vMax;
	int nBits;
	int firstBit;
};

typedef std::vector<IdField> IdFields;

class HitIdHelper {
public:
	HitIdHelper():m_id(0), m_currentStorage(0)  {}
	virtual ~HitIdHelper() {}
	void InitializeField(std::string n, int vmn, int vmx);
	void InitializeField(std::string n, int nb);
	void SetFieldValue(std::string name, int n);
	int  GetFieldValue(std::string name);
	void PrintFields();
	void SetID(int i) 	{m_id=i;}
	HitID GetID() 		{return m_id;}
	void ResetID() 		{m_id=0;}
	void Print(int id);
protected:
	HitID m_id;
	IdFields m_IDs;
	int m_currentStorage;
	void SetBit(int& i, int bitpos);
	void UnsetBit(int& i, int bitpos);
	void Store(int& id, int value, int first, int field);
	int  Retrieve(int id, int first, int field);
};

#endif
