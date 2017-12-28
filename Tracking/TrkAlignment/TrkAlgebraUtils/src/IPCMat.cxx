/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <GaudiKernel/StatusCode.h>
#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include "TrkAlgebraUtils/IPCMat.h"
#include <cstring>

namespace Trk {

pid_t ipcmat_pid;

/*
IPCMat::IPCMat()
  : m_msgSvc(0),
    m_ncalls(0),
    m_msgid(0),
    m_name("GlobAlign::IPCMat")
{}
*/

IPCMat::IPCMat(IMessageSvc *msgSvc)
  : m_msgSvc(msgSvc),
    m_ncalls(0),
    m_msgid(0),
    m_name("GlobAlign::IPCMat"),
    m_log(new MsgStream(msgSvc,name()))
{ 
}


const std::string& IPCMat::name() const {
  return m_name;
}

//=====================================================================
//  IncMat
//=====================================================================
StatusCode IPCMat::incMat(uint32_t a, uint32_t b, double c){
  //MsgStream log(m_msgSvc,name());

  m_ipcmat_msgp.mtype  = M_INCR_M;
  m_ipcmat_msgp.data.matrix.i = a;
  m_ipcmat_msgp.data.matrix.j = b;
  m_ipcmat_msgp.data.matrix.v = c;

  if (msgsnd(m_msgid,&m_ipcmat_msgp, sizeof(long)+2*sizeof(uint32_t)+sizeof(double),0)<0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__  << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  incVec
//=====================================================================
StatusCode IPCMat::incVec(uint32_t a, double c){
  //MsgStream log(m_msgSvc,name());

  m_ipcmat_msgp.mtype  = M_INCR_V;
  m_ipcmat_msgp.data.matrix.i = a;
  m_ipcmat_msgp.data.matrix.j = 0;
  m_ipcmat_msgp.data.matrix.v = c;

  if (m_log->level()>=MSG::DEBUG)
    *m_log << MSG::DEBUG << "call number " << std::setw(8) << m_ncalls
	   << endmsg;

  if (msgsnd(m_msgid,&m_ipcmat_msgp, sizeof(long)+2*sizeof(uint32_t)+sizeof(double),0)<0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  m_ncalls++;
  return StatusCode::SUCCESS;
}

//=====================================================================
//  scaleMat
//=====================================================================
StatusCode IPCMat::scaleMat(int scale){
  //MsgStream log(m_msgSvc,name());
  if (m_log->level()>=MSG::INFO)
    *m_log << MSG::INFO << "in ipcmat_scaleMat with scale=" << scale << endmsg;

  m_ipcmat_msgp.mtype = M_SCALE_M;
  m_ipcmat_msgp.data.scale = scale;

  if (msgsnd(m_msgid,&m_ipcmat_msgp, sizeof(long)+sizeof(double),0)<0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  scaleVec
//=====================================================================
StatusCode IPCMat::scaleVec(int scale){
  //MsgStream log(m_msgSvc,name());
  if (m_log->level()>=MSG::INFO)
    *m_log << MSG::INFO << "in ipcmat_scaleVec with scale=" << scale  << endmsg;

  m_ipcmat_msgp.mtype      = M_SCALE_V;
  m_ipcmat_msgp.data.scale = scale;

  if (msgsnd(m_msgid,&m_ipcmat_msgp, sizeof(long)+sizeof(double),0)<0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__  << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  reSize
//=====================================================================
StatusCode IPCMat::reSize(int newsize){
  //MsgStream log(m_msgSvc,name());
  if (m_log->level()>=MSG::INFO)
    *m_log << MSG::INFO << "in ipcmat_resize" << endmsg;

  m_ipcmat_msgp.mtype  = M_RESIZE;
  m_ipcmat_msgp.data.n = newsize;

  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(long)+sizeof(uint32_t), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__  << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  removeModule
//=====================================================================
StatusCode IPCMat::removeModule(int mod){
  //MsgStream log(m_msgSvc,name());

  m_ipcmat_msgp.mtype  = M_REMOVE_MOD;
  m_ipcmat_msgp.data.n = mod;

  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(long)+sizeof(uint32_t), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  removeAlignPar
//=====================================================================
StatusCode IPCMat::removeAlignPar(int alpar){
  //MsgStream log(m_msgSvc,name());

  m_ipcmat_msgp.mtype  = M_REMOVE_PAR;
  m_ipcmat_msgp.data.n = alpar;

  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(long)+sizeof(uint32_t), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  init
//=====================================================================
StatusCode IPCMat::init(void){
  //MsgStream log(m_msgSvc,name());
  if (m_log->level()>=MSG::INFO) {
    *m_log << MSG::INFO; m_log->setColor(MSG::BLUE);
    *m_log << "in IPCMat::ipcmat_init" << endmsg;
  }

  key_t key;

  if ((key = ftok("/dev/null", 69)) == -1) {
    *m_log << MSG::ERROR << "Unable to get key" << endmsg;
    return StatusCode::FAILURE;
  }
  else{
    if (m_log->level()>=MSG::INFO)
      *m_log << MSG::INFO << "Got key " << key << endmsg;
  }

  // destroy message queue if existed
  msgctl(m_msgid, IPC_RMID, NULL);

  m_msgid = msgget(key, IPC_CREAT | 0666);
  if(m_msgid < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__  << " key: " << key
          << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }else{
    if (m_log->level()>=MSG::INFO)
      *m_log << MSG::INFO << "m_msgid = " << m_msgid << endmsg;
  }

  // fork receiver
  if ((ipcmat_pid = fork()) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__  << " key: " << key
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_log->level()>=MSG::INFO)
    *m_log << MSG::INFO << "ipcmat_pid = " << ipcmat_pid << endmsg;

  if (ipcmat_pid == 0)
    {
      // ipcmatr compiled in 64 bits using same struct for message queue
      // as declared in IPCMat.h. Must be in PATH
      if (execlp("ipcmatr", "", NULL) < 0)
        {
          *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__  << " key: " << key
                << " Error number is " << errno << endmsg;
          return StatusCode::FAILURE;
        }
    }

  // let the children start
  sleep(1);

  return StatusCode::SUCCESS;
}

//=====================================================================
//  allocate
//=====================================================================
StatusCode IPCMat::allocate(int size){
  //MsgStream log(m_msgSvc,name());
  if (m_log->level()>=MSG::INFO) {
    *m_log << MSG::INFO; m_log->setColor(MSG::BLUE);
    *m_log << "in IPCMat::ipcmat_allocate" << endmsg;
  }

  m_ipcmat_msgp.mtype  = M_ALLOCATE;
  m_ipcmat_msgp.data.n = size;

  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(long)+sizeof(uint32_t), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  setScale
//=====================================================================
StatusCode IPCMat::setScale(int scale){
  //MsgStream log(m_msgSvc,name());
  if (m_log->level()>=MSG::INFO)
    *m_log << MSG::INFO << "setting m_scale to : " << scale << endmsg;

  m_ipcmat_msgp.mtype      = M_SETSCALE;
  m_ipcmat_msgp.data.scale = scale;

  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(long)+sizeof(uint32_t), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  setVersion
//=====================================================================
StatusCode IPCMat::setVersion(float version, bool isMatrix){
  //MsgStream log(m_msgSvc,name());

  m_ipcmat_msgp.mtype        = M_SETVERSION;
  m_ipcmat_msgp.data.version = version;

  std::string mType = isMatrix ? "MatVersion" : "VecVersion";
  if (m_log->level()>=MSG::INFO)
    *m_log << MSG::INFO << "setting " << mType << " to = " << version << endmsg;


  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(long)+sizeof(uint32_t), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
//  write
//=====================================================================
StatusCode IPCMat::write(const std::string ipcname, bool isMatrix){
  //MsgStream log(m_msgSvc,name());
  if (m_log->level()>=MSG::INFO)
    *m_log << MSG::INFO << "Writting ipcmat. Selected name is : " <<  ipcname
	   << endmsg;

  m_ipcmat_msgp.mtype = isMatrix ? M_WRITE_M : M_WRITE_V;
  strncpy(m_ipcmat_msgp.data.fname, ipcname.c_str(), sizeof(m_ipcmat_msgp.data.fname)-1);
  m_ipcmat_msgp.data.fname[sizeof(m_ipcmat_msgp.data.fname) - 1] = '\0';

  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(m_ipcmat_msgp)-sizeof(long), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=====================================================================
//  end
//=====================================================================
StatusCode IPCMat::end(){
  //MsgStream log(m_msgSvc,name());

  m_ipcmat_msgp.mtype  = M_END;
  if (msgsnd(m_msgid, &m_ipcmat_msgp, sizeof(long)+sizeof(uint32_t), IPC_NOWAIT) < 0) {
    *m_log << MSG::ERROR << "ipcmats: line: " <<  __LINE__
	   << " Error number is " << errno << endmsg;
    return StatusCode::FAILURE;
  }

  // let chid finish its jobs
  wait4(ipcmat_pid, NULL, 0, NULL);
  return StatusCode::SUCCESS;
}

//=====================================================================
//  summary
//=====================================================================
void IPCMat::summary(){
  //MsgStream log(m_msgSvc,name());

  if (m_log->level()>=MSG::INFO) {
    *m_log << MSG::INFO << "++++++++++++++++++++++++++++++++++++++++" << endmsg;
    *m_log << MSG::INFO << "   Total calls = " << m_ncalls            << endmsg;
    *m_log << MSG::INFO << "++++++++++++++++++++++++++++++++++++++++" << endmsg;
  }
}

} // end namespace Trk
