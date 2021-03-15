/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_RPCBOOK_H
#define TrigT1RPChardware_RPCBOOK_H

class rpcbookBase {
 public:
 rpcbookBase() {}
 virtual ~rpcbookBase() {};

 virtual void book(const int id,  const char *title, 
                   const int nch, const float xl,   const float xh) = 0;
 virtual void fill(const int id, const float x) = 0;
 virtual void fill(const int id, const float x, const float weight) = 0;
 virtual void write() = 0;
 protected:
 private:
};
//---------------------------------------------------------------------------//
class rpcbook : public rpcbookBase {
 public:
  rpcbook();
  void book(const int id,  const char *title, 
	    const int nch, const float xl, const float xh);
  void fill(const int id, const float x);
  void fill(const int id, const float x, const float w); 
  void write();
  void giveName(const int id, char *str, const int sizeofstr); 
};
//---------------------------------------------------------------------------//


#endif
