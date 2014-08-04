/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMSINGLEEVENTINPUTSVC_H
#define BYTESTREAMSINGLEEVENTINPUTSVC_H

/** @file ByteStreamSingleEventInputSvc.h
 *  @brief This file contains the class definition for the ByteStreamSingleEventInputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "EventStorage/DataReader.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  // no forward decl: typedef
#include "AthenaBaseComps/AthService.h"

#include "ByteStreamCnvSvc/IByteStreamSingleEventInputSvc.h"

#include "ByteStreamData/RawEvent.h"

class StoreGateSvc;
class IIncidentSvc;

/** @class ByteStreamSingleEventInputSvc
 *  @brief This class implements the interface to the ByteStreamInputSvc for reading a single event
 *  from ByteStream file given a file name and event position in that file.
 **/
class ByteStreamSingleEventInputSvc : public ::AthService, virtual public IByteStreamSingleEventInputSvc {
public:
   /// Standard Service Constructor
   ByteStreamSingleEventInputSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~ByteStreamSingleEventInputSvc();

   /// Required of all Gaudi Services
   StatusCode initialize();
   StatusCode stop();
   StatusCode finalize();
   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// get event for a given filename and event position
   const RawEvent* getEvent(const std::string& filename, const long long pos);

private: // data
   RawEvent*          m_re;            //!< current event
   DataReader*        m_reader;        //!< DataReader from EventStorage
   ServiceHandle<StoreGateSvc> m_mdSvc; //!< StoreGateSvc
   ServiceHandle<IIncidentSvc> m_incidentSvc; //!< IncidentSvc
   std::string        m_currentFilename; //!< name of the currently opened file

private: // properties
   BooleanProperty    m_dump;	      //!< Dump fragments
};

#endif
