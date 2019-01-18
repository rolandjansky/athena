/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCBoardConnection.hh,v 1.1.1.10 2003-11-03 10:18:17 mishino Exp $
#ifndef TGCBoardConnection_hh
#define TGCBoardConnection_hh

#include <iostream>

namespace LVL1TGCTrigger {

class TGCBoardConnection {
 public:
  inline int getNumberOfType() const { return m_nType; }
  int getNumber(int ntype) const;
  int getId(int type, int board) const;

  void setNumberOfType(int ntype);
  void setNumber(int type, int nBoard);
  void setId(int type, int board, int idIn);

  TGCBoardConnection(const TGCBoardConnection& right);
  TGCBoardConnection& operator=(const TGCBoardConnection& right);

  TGCBoardConnection();
  virtual ~TGCBoardConnection();  

 protected:
  int m_nType;
  int* m_numberOfBoard;
  int** m_id;
};

inline int TGCBoardConnection::getNumber(int type) const
{
  if(m_numberOfBoard==0) {
    std::cerr << "TGCBoardConnection::getNumber : numberOfBoard is zero" << std::endl;  
    return -1;
  }
  return m_numberOfBoard[type];
}

inline
int TGCBoardConnection::getId(int type, int board) const
{
  if(m_id!=0)
    return m_id[type][board];
  else { 
    std::cerr << "TGCBoardConnection::getId: id is zero" << std::endl;  
    return -1; 
  } 
}


} //end of namespace bracket

#endif // TGCBoardConnection_hh
