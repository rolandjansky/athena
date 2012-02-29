/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Reconstruction/MuonIdentification/muonEvent/MuonQualityWord.h 
 Author: Ketevi A. Assamagan, July 2009
 Transienti class to hold muon quality bits and the methods to manipulate it
 so as to return the the isLoose, isMedium and isTight to the use.
 This object is not persistified. 
 ---------------------------------------------------------------------------
 ***************************************************************************/

#ifndef MUONEVENT_MUONQUALITYWORD_H
#define MUONEVENT_MUONQUALITYWORD_H

#include <inttypes.h>
#include <iostream>
class MsgStream;

/** @class MuonQualityWord

    @brief A bitted word defining different levels of muon identification
           qualities (in terms of purity and efficiency), based solely
           on type of identification and reconstruction properties.

    The proposed bit encoding is as follows:
     - bits 12-15 encode the ranking of a TIGHT muon
     - bits 8-11 encode the ranking of a MEDIUM muon
     - bits 4-7 encode the ranking of a LOOSE muon
     - bits 0-3 encode the ranking of a UNSPECIFIED muon (i.e. container-specific information)

    As a consequence,
     - isTight() returns the OR of bits 12-15
     - isMedium() returns the OR of bits 8-15
     - isLoose() returns the OR of bits 4-15

    @author Ketevi Assamagan, Rikard Sandstroem
*/

class MuonQualityWord {

public:

    /** full constructor with a quality word */
    MuonQualityWord ( uint16_t qualityWord );

    /** default constructor */
    MuonQualityWord() { m_qualityWord = 0x0; };     

    /** destructor */
    ~MuonQualityWord();

    /** copy constructor */
    MuonQualityWord (const MuonQualityWord&);

    /** assignment operator */
    MuonQualityWord &operator= (const MuonQualityWord&);

    /** set the quality word - needed if default constructor is used */
    void set ( uint16_t qualityWord ) { m_qualityWord = qualityWord; }

    /** Accessor methods */
    bool isLoose()   const;
    bool isMedium () const;
    bool isTight ()  const;
   
    /** container specifi bits - may vary according to muon container */
    uint16_t type () const { return m_qualityWord & 0xF; }

    /** get back the quality word */
    uint16_t qualityWord() const { return m_qualityWord; }

private:

   uint16_t m_qualityWord;
};

/** Overload of << operator for MsgStream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const MuonQualityWord& qword);

/** Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const MuonQualityWord& qword);


inline bool MuonQualityWord::isLoose()  const { return ( m_qualityWord >> 4)  & 0xF; }

inline bool MuonQualityWord::isMedium() const { return ( m_qualityWord >> 8)  & 0xF; }

inline bool MuonQualityWord::isTight()  const { return ( m_qualityWord >> 12) & 0xF; }
 
#endif // MUONEVENT_MUONQUALITYWORD_H


