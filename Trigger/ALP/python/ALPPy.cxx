/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/
// Python bindings for ALP
#include <dlfcn.h>
#include <cstdio>
#include <memory>
#include <Python.h>
#include <sstream>
#include <signal.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "hltinterface/HLTInterface.h"
#include "ALP/ALP_utils.h"

typedef hltinterface::HLTInterface* (*creator)(void);
typedef void (*destroyer)(hltinterface::HLTInterface*);


void sahandler(int sig,siginfo_t * si,void* /*vp*/){
  fprintf(stderr,"ALPPy Got signal\n");
  if(sig==SIGTERM||sig==SIGINT){
    //std::cerr<<"Got signal"<<std::endl;
    std::terminate();
  }
  return;
  std::cerr<<ALPUtils::getTimeTag()<<__PRETTY_FUNCTION__<<"signo="<<si->si_signo
	   <<"  , errno="<<si->si_errno<<std::endl
	   // <<"  , trapno="<<si->si_trapno<<std::endl
    	   <<"  , pid="<<si->si_pid<<std::endl
	   <<"  , uid="<<si->si_uid<<std::endl
	   <<"  , status="<<si->si_status<<std::endl;
  
  std::cerr<<ALPUtils::getTimeTag()<<__PRETTY_FUNCTION__<<" si_code is =";
  switch (si->si_code){
  case CLD_EXITED:
    std::cerr<<"CLD_EXITED"<<std::endl;
    break;
  case CLD_KILLED:
    std::cerr<<"CLD_KILLED"<<std::endl;
    break;
  case CLD_DUMPED:
    std::cerr<<"CLD_DUMPED"<<std::endl;
    break;
  case CLD_TRAPPED:
    std::cerr<<"CLD_TRAPPED"<<std::endl;
    break;
  case CLD_STOPPED:
    std::cerr<<"CLD_STOPPED"<<std::endl;
    break;
  case CLD_CONTINUED:
    std::cerr<<"CLD_CONTINUED"<<std::endl;
    break;
  default:
    std::cerr<<"OTHER CODE = "<<si->si_code<<std::endl;
    break;
  }
}


std::shared_ptr<hltinterface::HLTInterface> s_pu;
static PyObject* ALPPyError;

static PyObject* loadPULibrary(PyObject* self,PyObject* args){
  const char *hltdll=0;
  const char hltdllOrig[]="libHLTMMPU.so";
  if (!PyArg_ParseTuple(args, "s", &hltdll)){
    hltdll=hltdllOrig;
  }
  void * myDllHandle=dlopen(hltdll,RTLD_LAZY|RTLD_GLOBAL);

  if(!myDllHandle){
    char buff[2000];
    const char* errmsg=dlerror();
    if(errmsg){
      snprintf(buff,2000,"Can't open ALP dll '%s' error is %s",hltdll,errmsg);
    }else{
      snprintf(buff,2000,"Can't open ALP dll '%s'",hltdll);
    }
    PyErr_SetString(ALPPyError, buff);
    return NULL;
  }

  typedef creator (*creator_dlsym)(void *, const char*);
  creator c=reinterpret_cast<creator_dlsym>(dlsym)(myDllHandle,"create_interface");
  const char* dlsymError=dlerror();
  if(dlsymError){
    char buff[2000];
    snprintf(buff,2000,"Can't import create_interface function from library '%s' error is %s",hltdll,dlsymError);
    PyErr_SetString(ALPPyError, buff);
    return NULL;
  }

  typedef destroyer (*destroyer_dlsym)(void *, const char*);
  destroyer d=reinterpret_cast<destroyer_dlsym>(dlsym)(myDllHandle,"destroy_interface");
  dlsymError=dlerror();
  if(dlsymError){
    char buff[2000];
    snprintf(buff,2000,"Can't import destroy_interface function from library '%s' error is %s",hltdll,dlsymError);
    PyErr_SetString(ALPPyError, buff);
    return NULL;
  }

  auto hltmppu=c();
  s_pu.reset(hltmppu,std::ptr_fun(d));

  Py_RETURN_TRUE;
}

bool checkInit(){
  if(!s_pu){
    PyErr_SetString(ALPPyError, "Need to call LoadLibrary first!");    
    return false;
  }
  return true;
}

static PyObject* configurePU(PyObject* self,PyObject* args){
  if(!checkInit()){
    return NULL;
  }

  const char * ptreestr;
  if (!PyArg_ParseTuple(args, "s", &ptreestr)){
      char buff[2000];
      snprintf(buff,2000,"Failed to parse the arguments'");
      PyErr_SetString(ALPPyError, buff);
      return NULL;
  }else{
    std::stringstream str(ptreestr);
    boost::property_tree::ptree pt;
    int fl=  boost::property_tree::xml_parser::no_comments|
      boost::property_tree::xml_parser::trim_whitespace;
    try{
      boost::property_tree::xml_parser::read_xml(str,pt,fl);
    }catch(std::exception &ex){
      str.str("Caught exception when parsing ptree. Exception was:");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }
    try{
      if(s_pu->configure(pt)){
	Py_RETURN_TRUE;
      }else{
	Py_RETURN_FALSE;
      }
    }catch(std::exception &ex){
      str.str("Caught exception during configure");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }
  }
  Py_RETURN_NONE;
}

static PyObject* unconfigurePU(PyObject* self,PyObject* args){
  if(!checkInit()){
    return NULL;
  }
  const char * ptreestr;
  if (!PyArg_ParseTuple(args, "s", &ptreestr)){
      char buff[2000];
      snprintf(buff,2000,"Failed to parse the arguments'");
      PyErr_SetString(ALPPyError, buff);
      return NULL;
  }else{
    std::stringstream str(ptreestr);
    boost::property_tree::ptree pt;
    int fl=  boost::property_tree::xml_parser::no_comments|
      boost::property_tree::xml_parser::trim_whitespace;
    try{
      boost::property_tree::xml_parser::read_xml(str,pt,fl);
    }catch(std::exception &ex){
      str.str("Caught exception when parsing ptree. Exception was:");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }
    try{
      if(s_pu->unconfigure(pt)){
	Py_RETURN_TRUE;
      }else{
	Py_RETURN_FALSE;
      }
    }catch(std::exception &ex){
      str.str("Caught exception during unconfigure");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }

  }
  Py_RETURN_NONE;
}

static PyObject* connectPU(PyObject* self,PyObject* args){
  if(!checkInit()){
    return NULL;
  }
  const char * ptreestr;
  if (!PyArg_ParseTuple(args, "s", &ptreestr)){
      char buff[2000];
      snprintf(buff,2000,"Failed to parse the arguments'");
      PyErr_SetString(ALPPyError, buff);
      return NULL;
  }else{
    std::stringstream str(ptreestr);
    boost::property_tree::ptree pt;
    int fl=  boost::property_tree::xml_parser::no_comments|
      boost::property_tree::xml_parser::trim_whitespace;
    try{
      boost::property_tree::xml_parser::read_xml(str,pt,fl);
    }catch(std::exception &ex){
      str.str("Caught exception when parsing ptree. Exception was:");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }
    try{
      if(s_pu->connect(pt)){
	Py_RETURN_TRUE;
      }else{
	Py_RETURN_FALSE;
      }
    }catch(std::exception &ex){
      str.str("Caught exception during connect");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }

  }
  Py_RETURN_NONE;

}

static PyObject* prepareForRunPU(PyObject* self,PyObject* args){
  if(!checkInit()){
    return NULL;
  }
  const char * ptreestr;
  if (!PyArg_ParseTuple(args, "s", &ptreestr)){
      char buff[2000];
      snprintf(buff,2000,"Failed to parse the arguments'");
      PyErr_SetString(ALPPyError, buff);
      return NULL;
  }else{
    std::stringstream str(ptreestr);
    boost::property_tree::ptree pt;
    int fl=  boost::property_tree::xml_parser::no_comments|
      boost::property_tree::xml_parser::trim_whitespace;
    try{
      boost::property_tree::xml_parser::read_xml(str,pt,fl);
    }catch(std::exception &ex){
      str.str("Caught exception when parsing ptree. Exception was:");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }
    try{
      if(s_pu->prepareForRun(pt)){
	Py_RETURN_TRUE;
      }else{
	Py_RETURN_FALSE;
      }
    }catch(std::exception &ex){
      str.str("Caught exception during prepareForRun");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }

  }
  Py_RETURN_NONE;

}

static PyObject* stopRunPU(PyObject* self,PyObject* args){
  if(!checkInit()){
    return NULL;
  }
  const char * ptreestr;
  if (!PyArg_ParseTuple(args, "s", &ptreestr)){
      char buff[2000];
      snprintf(buff,2000,"Failed to parse the arguments'");
      PyErr_SetString(ALPPyError, buff);
      return NULL;
  }else{
    std::stringstream str(ptreestr);
    boost::property_tree::ptree pt;
    int fl=  boost::property_tree::xml_parser::no_comments|
      boost::property_tree::xml_parser::trim_whitespace;
    try{
      boost::property_tree::xml_parser::read_xml(str,pt,fl);
    }catch(std::exception &ex){
      str.str("Caught exception when parsing ptree. Exception was:");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }
    try{
      if(s_pu->stopRun(pt)){
	Py_RETURN_TRUE;
      }else{
	Py_RETURN_FALSE;
      }
    }catch(std::exception &ex){
      str.str("Caught exception during stopRun");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }

  }
  Py_RETURN_NONE;

}

static PyObject* userCommandPU(PyObject* self,PyObject* args){
  if(!checkInit()){
    return NULL;
  }
  const char * ptreestr;
  if (!PyArg_ParseTuple(args, "s", &ptreestr)){
      char buff[2000];
      snprintf(buff,2000,"Failed to parse the arguments'");
      PyErr_SetString(ALPPyError, buff);
      return NULL;
  }else{
    std::stringstream str(ptreestr);
    boost::property_tree::ptree pt;
    int fl=  boost::property_tree::xml_parser::no_comments|
      boost::property_tree::xml_parser::trim_whitespace;
    try{
      boost::property_tree::xml_parser::read_xml(str,pt,fl);
    }catch(std::exception &ex){
      str.str("Caught exception when parsing ptree. Exception was:");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }
    try{
      if(s_pu->hltUserCommand(pt)){
	Py_RETURN_TRUE;
      }else{
	Py_RETURN_FALSE;
      }
    }catch(std::exception &ex){
      str.str("Caught exception during user command");
      str<<std::endl<<ex.what()<<std::endl;
      PyErr_SetString(ALPPyError, str.str().c_str());
      return NULL;      
    }

  }
  Py_RETURN_NONE;
}

static PyObject* setHandler(PyObject* self,PyObject* args){
  static struct sigaction act;
  memset (&act, '\0', sizeof(act));
  act.sa_sigaction=&sahandler;
  act.sa_flags=SA_SIGINFO;
  if(sigaction(SIGTERM,&act,NULL)<0){
    std::cerr<<ALPUtils::getTimeTag()<<"Error setting signal handler for SIGTERM"<<std::endl;
      PyErr_SetString(ALPPyError, "Setting Signal handler failed");
      return NULL;
  }
  if(sigaction(SIGINT,&act,NULL)<0){
    std::cerr<<ALPUtils::getTimeTag()<<"Error setting signal handler for SIGINT"<<std::endl;
      PyErr_SetString(ALPPyError, "Setting Signal handler failed");
      return NULL;
  }
  std::cout<<"Signal handler set"<<std::endl;
  Py_RETURN_TRUE;
}

static PyMethodDef HLTMPPymethods[]= {
  {(char *)"LoadLibrary", (PyCFunction)loadPULibrary, METH_VARARGS, "Method to load HLTMMPU library. Needs to be called first"},
  {(char *)"ConfigurePU", (PyCFunction)configurePU, METH_VARARGS, "configure call for PU"},
  {(char *)"UnconfigurePU", (PyCFunction)unconfigurePU, METH_VARARGS, "unconfigure call for PU"},
  {(char *)"ConnectPU", (PyCFunction)connectPU, METH_VARARGS, "Connect call for PU"},
  {(char *)"PrepareForRunPU", (PyCFunction)prepareForRunPU, METH_VARARGS, "PrepareForRun call for PU"},
  {(char *)"StopRunPU", (PyCFunction)stopRunPU, METH_VARARGS, "StopRun call for PU"},
  {(char *)"UserCommandPU", (PyCFunction)userCommandPU, METH_VARARGS, "Send hltusercommand to Mother process"},
  {(char *)"SetSignalHandler", (PyCFunction)setHandler, METH_VARARGS, "Set Signal handler"},
  { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC init_HLTMPPy(void) { 

  PyObject* m;
  m=Py_InitModule("_HLTMPPy", HLTMPPymethods); 
  ALPPyError=PyErr_NewException("HLTMPPy.error",0,0);
  Py_INCREF(ALPPyError);
  PyModule_AddObject(m,"error",ALPPyError);
}
