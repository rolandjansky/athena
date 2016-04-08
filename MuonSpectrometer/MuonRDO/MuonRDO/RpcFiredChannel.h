/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcFiredChannel.h

#ifndef RpcFiredChannelH
#define RpcFiredChannelH

// M.Grothe, 28/11/02
// K. Assamagan 06/12/02

#include <vector>
#include <iostream>

typedef unsigned short int ubit16;

class MsgStream; 

class RpcFiredChannel {

 private:
  friend class RpcFiredChannelCnv_p1;


  ubit16 m_bcid;
  ubit16 m_time;
  ubit16 m_ijk;
  ubit16 m_channel;
  ubit16 m_ovl;
  ubit16 m_thr;

 public:

  // Default constructor.
  RpcFiredChannel();

  // Full constructor number 1.
  // to be used for ijk between 0 and 6
  RpcFiredChannel(ubit16 bcid, ubit16 time, ubit16 ijk, 
                  ubit16 channel);

  // Full constructor number 2.
  // to be used for ijk=7
  RpcFiredChannel(ubit16 bcid, ubit16 time, ubit16 ijk, 
                  ubit16 thr, ubit16 ovl);

  // virtual destructor to have VMT for persistency
  virtual ~RpcFiredChannel() {}

  // return bunch crossing id
  ubit16 bcid() const { return m_bcid; }
  // return time
  ubit16 time() const { return m_time; }
  // return ijk
  ubit16 ijk() const { return m_ijk; }
  // return channel
  ubit16 channel() const { return m_channel; }
  // return if channels in pivot that caused trigger were in
  // chamber overlap region
  ubit16 ovl() const {return m_ovl; }
  //return and/or of those hits in the pivot that caused a
  //trigger, i.e. belong to a RPC track that was above the lowest (?) of
  // the three trigger pt tresholds
  ubit16 thr() const {return m_thr; }


 private:

  // Private methods.
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const RpcFiredChannel& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const RpcFiredChannel& coll);


#endif
