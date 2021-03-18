/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_RPCReadOut_H
#define TrigT1RPChardware_RPCReadOut_H

#include<iostream>
#include "TrigT1RPChardware/Lvl1Def.h"

class RPCReadOut {

 public:
 RPCReadOut();
 virtual ~RPCReadOut() = default;
 
 virtual ubit16 getWord(){return m_word;};
 virtual char field()    {return m_field;}; 

 virtual ubit16 makeHeader(ubit16 *inputData)=0;
 virtual ubit16 makeSubHeader()=0;
 virtual ubit16 makeBody(ubit16 *inputData)=0;
 virtual ubit16 makeFooter(ubit16 inputData)=0;
 virtual ubit16 makeFooter(ubit16 *inputData)=0;
 
 virtual bool isHeader()=0;
 virtual bool isBody()=0;
 virtual bool isFooter()=0;

 protected:
 //create a 16 bits word of the CMA frame structure , starting from the fields:
 //ubit16 set16Bits(const ubit16 num, const ubit16 *pos, const ubit16 *val);
 // get the fields of the 16 bits word of the CMA frame structure:
 //ubit16 get16Bits(const ubit16 num, const ubit16  pos, const ubit16  val);

 ubit16 m_word;
 char   m_field;

inline ubit16 set16Bits(const ubit16 n,
                        const ubit16 *pos,const ubit16 *val) {
ubit16 output=0;
ubit16 p=0, v=0;
for(ubit16 i=0;i<n;i++){
 v = *(val+i);
 p = *(pos+i);
 output = output | (v<<p);
}//end-of-for
return output;
}//end-of-MatrixReadOutStructure::set16Bits
//---------------------------------------------------------------------------//
    inline ubit16 get16Bits(const ubit16 input, 
                            const ubit16 position,
                            const ubit16 length) {
ubit16 output=0;
ubit16 CMfield=0;
for(ubit16 i=0; i<length; i++) {CMfield = CMfield | 1<<(position+i);}
output = (input&CMfield)>>position;
return output;
}//end-of-MatrixReadOutStructure::get16Bits



//************************//
 static const ubit16 s_last2bitsON;
 static const ubit16 s_last4bitsON;
 static const ubit16 s_first8bitsON;

};
#endif
