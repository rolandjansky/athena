/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COOLCONVUTILS_POOLMAPELEMENT_H
#define COOLCONVUTILS_POOLMAPELEMENT_H
// PoolMapElement.h - simple class to hold information about a POOL file usage
// and the conditions data folders that reference it 
// Used by AtlCoolCopy.cxx

#include<vector>
#include<string>

class PoolMapElement {
 public:
  PoolMapElement();
  PoolMapElement(const int usage,const std::string foldertag);
  // increment usage count
  void inccount();
  void setErrorBit(const int bit);
  void setlfn(const std::string& lfn);
  void setpfn(const std::string& pfn);
  // access methods
  int count() const;
  int errcode() const;
  const std::string& lfn() const;
  const std::string& pfn() const;
  std::vector<std::string>& foldertag();
  const std::vector<std::string>& foldertag() const;

  void addfoldertag(const std::string& foldertag);

 private:
  int m_count;
  int m_errcode;
  std::string m_lfn;
  std::string m_pfn;
  std::vector<std::string> m_folders;
};

inline PoolMapElement::PoolMapElement(const int usage, 
				      const std::string foldertag) :
  m_count(usage),m_errcode(0),m_lfn(""),m_pfn("") {
  m_folders.push_back(foldertag);
}

inline PoolMapElement::PoolMapElement(): m_count(0),m_errcode(0),m_lfn(),m_pfn(),m_folders() {}

inline void PoolMapElement::inccount() { ++m_count;}
inline void PoolMapElement::setErrorBit(const int bit) 
   { m_errcode=m_errcode | (1 << bit);}
inline void PoolMapElement::setlfn(const std::string& lfn) { m_lfn=lfn;}
inline void PoolMapElement::setpfn(const std::string& pfn) { m_pfn=pfn;} 

inline int PoolMapElement::count() const { return m_count; }
inline int PoolMapElement::errcode() const {return m_errcode; }
inline const std::string& PoolMapElement::lfn() const {return m_lfn; }
inline const std::string& PoolMapElement::pfn() const {return m_pfn; }

inline std::vector<std::string>& PoolMapElement::foldertag() { return m_folders; }
inline const std::vector<std::string>& PoolMapElement::foldertag() const {return m_folders; }
inline void PoolMapElement::addfoldertag(const std::string& foldertag) {
  if (find(m_folders.begin(),m_folders.end(),foldertag)==m_folders.end())
    m_folders.push_back(foldertag);
}

#endif // COOLCONVUTILS_POOLMAPELEMENT_H
