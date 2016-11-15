/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
MsgStream* msglog;

#define _LINE_ __LINE__ 
#define REPORT_AND_RETURN(__message, __ret) { *msglog << MSG::ERROR << "LINE:" <<_LINE_ << " Test failed: " <<  __message << endmsg; return __ret; }
#define REPORT_AND_STOP(__message) REPORT_AND_RETURN(__message, StatusCode::FAILURE) 
#define ABORT(__message) REPORT_AND_RETURN(__message, -1)


#define REPORT_AND_CONTINUE(__message) {*msglog << MSG::INFO << "LINE:" <<_LINE_ << " Test progress fine:  " <<  __message << endmsg;}
#define REPORT_REGTEST(__message) {*msglog << MSG::INFO << "REGTEST " <<  __message << endmsg;}



#define BEGIN_TEST(__name) const std::string TEST_NAME_=__name;		\
  *msglog << MSG::INFO << "LINE:" <<_LINE_ << " Start of the test: " <<  TEST_NAME_ << endmsg 
#define END_TEST   *msglog << MSG::INFO << "LINE:" <<_LINE_ << " End of the test: " <<  TEST_NAME_ << endmsg; return StatusCode::SUCCESS
