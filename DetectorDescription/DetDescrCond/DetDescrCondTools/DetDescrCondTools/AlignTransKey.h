/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTOOLS_ALIGNTRANSKEYL_H
#define DETDESCRCONDTOOLS_ALIGNTRANSKEYL_H
// AlignTransKey.h
// a helper class for the AlignTransTool AlgTool to store details of
// known keys/directories, and their sort state
#endif // DETDESCRCONDTOOLS_ALIGNTRANSKEYL_H

#include <string>

class AlignTransKey {
 public:
  AlignTransKey(const std::string& key);

  const std::string key() const;
  bool sorted() const;

  void set_sorted(bool status);

 private:
  std::string m_keyname;
  bool m_sorted;
};

inline AlignTransKey::AlignTransKey(const std::string& key) : 
  m_keyname(key), m_sorted(false) {}

inline const std::string AlignTransKey::key() const { return m_keyname; }
inline bool AlignTransKey::sorted() const { return m_sorted; }
inline void AlignTransKey::set_sorted(bool status) { m_sorted=status; }

// comparison pperator for find - look only at name field 
inline bool operator== (const AlignTransKey& lhs, const AlignTransKey& rhs)
{ return (lhs.key()==rhs.key()); }
