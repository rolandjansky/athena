/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_BEAMSPOTSTATUSCODE
#define INDET_BEAMSPOTSTATUSCODE
/** @class BeamSpotStatusCode 
    A simple class/interface to deal with the  status-code bit.
    @author: jwalder     
*/

namespace InDet {
  /** Class to hold the definition and accessor methods for the beamspot status-word.
      As well as position etc., meta-data is also required, i.e. beamspot fit result status, 
      beamspot algorithm type, online/offline mode.
      This class defines the meaning of the word and supplies the necessary methods to access/set
      the information.
   */
class BeamSpotStatusCode { 
 public:
  BeamSpotStatusCode():
    m_word(0)
    {}
  ~BeamSpotStatusCode(){}    
  //Online/offline
  bool isOnline() const {return isOnline(m_word);}
  static bool isOnline(int word){ 
    int i = getItem(word, m_onlineBit, m_onlineLength);
    return ( i > 0 ? true : false );
  } 
  void setOnlineStatus(bool isOnline){setOnlineStatus(isOnline, m_word);}
  static void setOnlineStatus(bool isOnline, int & word){ 
    setWordBit( formBit( m_onlineBit, (isOnline?1:0)), word);}

  //fit status
  int fitStatus() const {return fitStatus(m_word);}
  static int fitStatus(int word) { return getItem(word, m_fitStatusBit, m_fitStatusLength);}
  void setFitStatus(int status){setFitStatus(status, m_word);}
  static void setFitStatus(int status, int & word) {
    setWordBit( formBit( m_fitStatusBit, status), word); } 
   
  //algorthim type
  int algType() const { return algType(m_word);} 
  static int algType(int word) { return getItem(word, m_algorithmTypeBit, m_algorithmTypeLength);}
  void setAlgType(int type){setAlgType(type,m_word);}
  static void setAlgType(int type, int & word) {
    setWordBit( formBit(m_algorithmTypeBit, type), word);}
   
  //width
  bool fitWidth()  const {return fitWidth(m_word);}
  static bool fitWidth(int word){ return getItem(word, m_widthBit, m_widthLength);}
  void setFitWidth(bool fitWidth) {setFitWidth(fitWidth,m_word);}
  static void setFitWidth(bool fitWidth, int & word) {
    setWordBit( formBit( m_widthBit, (fitWidth?1:0)),word); }


  void clearWord() { m_word = 0;}
  void setWord(int word) { m_word = word;}
  int getWord() const {return m_word;} 

private:
  int getItem( int bit, int length) const {return getItem(m_word, bit,length);}
  static int getItem( int word, int bit, int length) {
    int temp = 0;
    for (int i =0; i < length; ++i) {
      temp = (temp << 1) + 1;
    }
    return (word >> bit) & temp;
  }

  static int formBit( int bit, int word) {
    return (word << bit);
  }  
  static void setWordBit(int flag, int & word) {
    word = (word | flag);
  }

  static const int m_onlineBit,m_onlineLength;
  static const int m_fitStatusBit, m_fitStatusLength;
  static const int m_algorithmTypeBit, m_algorithmTypeLength;
  static const int m_widthBit, m_widthLength;

  int m_word;

};

}


#endif 

