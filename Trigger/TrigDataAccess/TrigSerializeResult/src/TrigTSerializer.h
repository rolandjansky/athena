// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @class  TrigTSerializer
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 */


#ifndef TRIGTSERIALIZER_H
#define TRIGTSERIALIZER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "TrigSerializeResult/ITrigSerializerToolBase.h"
#include "RVersion.h"
#include "Rtypes.h"
#include "TError.h"
#include <string>
#include <map>

//forward decl
class TBuffer;
class MsgStream;

#include "DataModelRoot/RootType.h"
typedef RootType        Type;
typedef RootDataMember  Member;


class ATLAS_NOT_THREAD_SAFE TrigTSerializer : virtual public ITrigSerializerToolBase, public AthAlgTool {
public:
  TrigTSerializer(const std::string& name, const std::string& type,
		  const IInterface* parent);
  virtual ~TrigTSerializer();

  StatusCode initialize();
  StatusCode finalize();

  /*
  StatusCode serialize(uint32_t *serialized, void *instance,
		       const std::string nameOfClass, int &veclen, const int maxsize);
  */
  std::vector<uint32_t> serialize(const std::string &nameOfClass, void* instance);

  void serialize(const std::string &nameOfClass, void* instance, std::vector<uint32_t> &v);

  void* deserialize(const std::string &nameOfClass, const std::vector<uint32_t>& v);
  /*
    StatusCode deserialize(const uint32_t *serialized,  const std::string nameOfClass);
  */

  StatusCode initClass(const std::string &nameOfClass) const;

  //reset Serializer internal data between running serialize methods with unrelated data
  void reset();

  StatusCode peekCLID(const std::vector<uint32_t>& v, uint32_t *guid) const;
  
  void setCLID(const uint32_t *guid);

private:
  void do_persistify(const std::string nameOfClass, void* instance);
  void do_persistify_obj(const std::string nameOfClass, void* instance);
  void do_follow_ptr(const std::string nameOfClass, void* instance);
  void add_previous_streamerinfos();
  
  static void streamerErrorHandler(Int_t level, Bool_t abort_bool,
				   const char* location, const char *msg);
  void prepareForTBuffer(const std::string &nameOfClass);
  void restoreAfterTBuffer(const std::string &nameOfClass);

  //  StatusCode persistifyEL(const TClass *cl, void* instance);
  bool      m_onlineMode;
  uint32_t  m_guid[4];  

  // for error handling
  ErrorHandlerFunc_t  m_defaultHandler;
  static   bool       s_decodingError;
  //static   bool       m_reportError;
  uint32_t  m_IgnoreErrLvl;
  std::map<std::string, uint32_t>   m_errCount;
};

#undef REFLEX_NS


#endif
