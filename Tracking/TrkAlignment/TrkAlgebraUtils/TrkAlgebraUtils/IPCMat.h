/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_IPCMAT_H
#define TRKALGS_IPCMAT_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string>

#define M_ALLOCATE         1
#define M_INCR_M           2
#define M_INCR_V           3
#define M_SCALE_M          4
#define M_SCALE_V          5
#define M_WRITE_M          6
#define M_WRITE_V          7
#define M_RESIZE           8
#define M_REMOVE_MOD       9
#define M_REMOVE_PAR      10
#define M_SETSCALE        12
#define M_SETVERSION      13
#define M_END            255

class StatusCode;
class IMessageSvc;

namespace Trk {

////////////////////////
class IPCMat {
////////////////////////

 public:
  IPCMat(IMessageSvc *msgSvc);
  ~IPCMat(){}

  StatusCode init(void);
  StatusCode allocate(int);
  StatusCode write(const std::string,bool);
  StatusCode end();
  StatusCode incMat(uint32_t,uint32_t,double);
  StatusCode incVec(uint32_t,double);
  StatusCode scaleMat(int);
  StatusCode scaleVec(int);
  StatusCode setScale(int);
  StatusCode setVersion(float,bool);
  StatusCode reSize(int);
  StatusCode removeModule(int);
  StatusCode removeAlignPar(int);

  void summary();

 private:
  IPCMat(); // don't allow constructor without MessageSvc

  const std::string &name() const;

 private:
  IMessageSvc *m_msgSvc;
  int m_ncalls;
  int m_msgid;
  std::string  m_name;
  MsgStream* m_log;

  struct MsgBuf {
    long mtype;
  MsgBuf(): mtype(255){}
    union
    {
      char fname[100];
      uint32_t n;
      double scale;
      float version;
      struct
      {
        uint32_t i;
        uint32_t j;
        double   v;
      } matrix;
    } data;
 } ipcmat_msgp;


};

} // end namespace Trk

#endif // TRKALGS_IPCMAT_H
