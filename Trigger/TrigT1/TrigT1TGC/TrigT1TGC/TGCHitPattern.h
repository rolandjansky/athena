/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCHitPattern_hh
#define TGCHitPattern_hh
#include <fstream>

namespace LVL1TGCTrigger {

class TGCHitPattern {
 public:
  TGCHitPattern();
  TGCHitPattern(int len);
  ~TGCHitPattern();

  TGCHitPattern(const TGCHitPattern& right);
  TGCHitPattern& operator=(const TGCHitPattern& right);

  void setLength(int length);
  int getLength() const;

  void onChannel(int iChannel);
  void offChannel(int iChannel);
  void setChannel(int iChannel, bool pattern);
  void setChannel(char* pat);
  void clear();
  void print(int unit) const; // print bit pattern separated by underscore.
  void print() const;
  bool getChannel(int iChannel) const;

  // new method for hit visualization (KH 19/01/01) 
  void visual(int Cycle, int Width, int Tag) const;

  // new method to set hit patterns (KH 08/05/01) 
  void setPattern(bool* newpattern);

  void printb() const;
  void printb(std::ofstream* ofs) const;
  void dec2bin(int dec);
  void push_back(TGCHitPattern* hp);

 private:
  bool* m_pattern; // this array is created when the length is specified
  int m_length;
};

inline
int TGCHitPattern::getLength() const
{
  return m_length;
}

inline
void TGCHitPattern::onChannel(int iChannel)
{
  m_pattern[iChannel] = true;
}

inline
void TGCHitPattern::offChannel(int iChannel)
{
  m_pattern[iChannel] = false;
}



} //end of namespace bracket

#endif // TGCHitPattern_hh





