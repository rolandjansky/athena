/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLSERVICES_ATHENAROOTSTREAMER_H
#define ATHENAPOOLSERVICES_ATHENAROOTSTREAMER_H

/** @file AthenaRootStreamer.h
 *  @brief this file contains the class definition for the AthenaRootStreamer class.
 *  @author Marcin Nowak
 *  $Id: 
 **/

#include "GaudiKernel/StatusCode.h"

#include "TClassStreamer.h"

#include <string>
#include <map>
#include <set>
#include <memory>

class TFile;
class Service;
class AthenaRootConverterHandle;

/** @class AthenaRootStreamer
 *  @brief This class enhances ROOT custom streamer base TClassStreamer
 *  and can choose a converter function based on the streamerinfo checksum
 **/

class AthenaRootStreamer : public TClassStreamer
{
public:
  /// Constructor
  /// @param class_name [IN] name of the class that is stored in the file
  /// (in most cases of schema evolution this is the same name as in the
  /// dictionary, but it may be different)
  /// @param service [IN] pointer to parent service, used to write out messages to log
  AthenaRootStreamer(const std::string& class_name, ::Service* service=0);

  /// Destructor
  virtual ~AthenaRootStreamer();

  /// Add a converter to this streamer
  /// @param converter [IN] - converter handle holding user defined converter object
  StatusCode AddConverter(std::unique_ptr<AthenaRootConverterHandle> converter);

   /// Adopt (enable) this ROOT custom streamer
   StatusCode Adopt();
  
  /// this operator is colled by ROOT to ready and write objects 
  virtual void operator()(TBuffer& b, void* obj);

protected:
   static Version_t R__FindStreamerInfoVersion(const TClass* cl, UInt_t checksum);
   void FindVersion(TBuffer* buf, UInt_t* startpos, UInt_t* bcnt);

private:
  std::string m_className;
  TClass*     m_class;
  UInt_t      m_streamerChecksum;	// retrieved by FindVersion from the file
  Version_t   m_streamerVersion;	// retrieved by FindVersion from the file
  TFile*      m_lastFile;

  typedef std::map<UInt_t, std::unique_ptr<AthenaRootConverterHandle> > ConverterMap;
  ConverterMap	m_converterMap;		// set of converters for this class

  typedef std::set<UInt_t>	ChecksumSet;
  ChecksumSet	m_seenChecksums;	// unknown streamer checksums seen in files

  Service*	m_service;       	//AthenaRootStreamerMgrSvc
};

#endif







