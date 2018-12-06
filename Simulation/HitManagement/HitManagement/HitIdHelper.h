/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HITMANAGEMENT_HitIdHelper_H
#define HITMANAGEMENT_HitIdHelper_H

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
  void SetFieldValue(std::string name, int n, HitID& targetID) const;
  int  GetFieldValue(std::string name);
  int  GetFieldValue(std::string name, HitID targetID) const;
  void PrintFields();
  void SetID(int i)       {m_id=i;}      // FIXME drop this method?
  HitID GetID()           {return m_id;} // FIXME drop this method?
  void ResetID()          {m_id=0;} // FIXME drop this method?
  void Print(int id);
protected:
  HitID m_id; // FIXME drop this member variable
  IdFields m_IDs;
  int m_currentStorage;
  void SetBit(int& i, int bitpos) const;
  void UnsetBit(int& i, int bitpos) const;
  void Store(int& id, int value, int first, int field) const;
  int  Retrieve(int id, int first, int field) const;
};

#endif
