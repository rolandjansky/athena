/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloBadChannel_H
#define CaloBadChannel_H

class CaloBadChannel {
 public:

  typedef unsigned int      PosType;
  typedef unsigned int      BitWord;

  enum ProblemType {deadBit=0, noisyBit=1, affectedBit=2};
	
  CaloBadChannel( BitWord rawStatus) : m_word(rawStatus) {}
  CaloBadChannel() : m_word(0) {}

  CaloBadChannel(const CaloBadChannel& rBad) = default;
  CaloBadChannel(const CaloBadChannel* pBad) {m_word=pBad->m_word;}
  CaloBadChannel& operator= (const CaloBadChannel& rBad) = default;

  bool statusOK( ProblemType pb) const {
    BitWord mask = 1 << (PosType) pb;
    return ((m_word & mask) == 0); // OK means bit is not set
  }

  bool dead() const {return !statusOK( deadBit);}
  bool noisy() const {return !statusOK( noisyBit);}
  bool affected() const {return !statusOK( affectedBit);}

  BitWord packedData() const {return m_word;}

  /// Sets the bit corresponding to "pb" inside the word passed as second argument to "value".
  /// This static method provides a convenient way of setting the bits of a CaloBadChannel,
  /// without compromizing the const interface of the class. The Bits should be set in a BitWord,
  /// and then the BitWord passed to the constructor of CaloBadChannel.
  static void setBit( ProblemType pb, BitWord& word, bool value=true) {
    BitWord mask = 1 << static_cast<PosType>(pb); // all at 0 axcept pb bit
    if (value) word |= mask;
    else word &= ~mask;
  }

 private:

  BitWord m_word;

};

#endif
