/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCHitPattern.hh,v 1.1.1.10 2003-11-03 10:18:17 mishino Exp $
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
  bool isEmpty() const;
  bool getChannel(int iChannel) const;

  // new method for hit visualization (KH 19/01/01) 
  void visual(int Cycle, int Width, int Tag) const;

  // new method to set hit patterns (KH 08/05/01) 
  void setPattern(bool* newpattern);

  void printb() const;
  void printb(std::ofstream* ofs) const;
  void printb(std::ofstream* ofs, int lengthIn) const;
  void printb3(std::ofstream* ofs) const;
  void printb012(std::ofstream* ofs) const;
  void printb123(std::ofstream* ofs) const;
  void insert(int pos, bool v);
  void dec2bin(int dec);
  void dec2binInv(int dec);
  void resize(int size);
  void push_back(TGCHitPattern* hp);
  void replace(int pos, TGCHitPattern* hp);
  void del(int pos);
  void write(char* buf) const;
  bool* getPatad(void) const;
  void reverse(int pos);
  const char* bool2char(void);

private:
  bool* m_pattern; // this array is created when the length is specified
  int m_length;

  char* m_cPattern;
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
  if(m_cPattern){delete [] m_cPattern;}
  m_cPattern = 0;
}

inline
void TGCHitPattern::offChannel(int iChannel)
{
  m_pattern[iChannel] = false;
  if(m_cPattern){delete [] m_cPattern;}
  m_cPattern = 0;
}



} //end of namespace bracket

#endif // TGCHitPattern_hh





