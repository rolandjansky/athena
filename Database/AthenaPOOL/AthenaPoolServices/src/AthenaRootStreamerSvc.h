/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLSERVICES_ATHENAROOTSTREAMERSVC_H
#define ATHENAPOOLSERVICES_ATHENAROOTSTREAMERSVC_H

/** @file AthenaRootStreamerSvc.h
 *  @brief Athena ROOT Streaming Service class 
 *  @author Marcin Nowak
 *  $Id: 
 **/

#include "AthenaPoolServices/IAthenaRootStreamerSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "DataModelRoot/RootType.h"

#include <map>


/** @class AthenaRootStreamerSvc
 *  @brief This class provides the manager for ROOT custom streamers.
 *  @author Marcin Nowak
 **/

class AthenaRootStreamer;
namespace pool {
  class IOODatabase;
}

class AthenaRootStreamerSvc :  public virtual IAthenaRootStreamerSvc,
                               public AthService
{
public: 
   /// Standard Service Constructor
   AthenaRootStreamerSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AthenaRootStreamerSvc();

public:
   /// Required of all Gaudi services:
   virtual StatusCode initialize();
   /// Required of all Gaudi services:
   virtual StatusCode finalize();
   /// Required of all Gaudi services:  see Gaudi documentation for details
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  /// Service type.
   virtual const InterfaceID& type() const;

  // IAthenaRootStreamerSvc  interface implemenation
  virtual StatusCode AddStreamer(const std::string& converter_classname, bool adopt=true);  

  virtual StatusCode AddStreamer(T_AthenaRootConverterBase *converter, bool adopt=true);
  
  virtual StatusCode AdoptStreamerForClass(const std::string& class_name);

  virtual StatusCode AdoptAllStreamers();

  
private:

  StringArrayProperty		m_streamerClassNames;

  /// map of streamers for given class names
  typedef std::map<std::string, AthenaRootStreamer*> StreamerMap;
  StreamerMap 			m_streamerMap;

  /// vector of converters created by the service,  (as Reflex Objects)
  /// kept so they can be deleted at the end
  typedef std::pair<RootType, void*> ConverterPair_t;
  typedef std::vector< ConverterPair_t > ConverterVector_t;
  ConverterVector_t		m_createdConverters;
};
#endif
