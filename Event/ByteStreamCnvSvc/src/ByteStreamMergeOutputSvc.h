/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMMERGEOUTPUTSVC_H
#define BYTESTREAMMERGEOUTPUTSVC_H

/** @file ByteStreamMergeOutputSvc.h
 *  @brief This file contains the class definition for the ByteStreamMergeOutputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMergeOutputSvc.h,v 1.51 2009-03-03 16:03:22 gemmeren Exp $
 **/

#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"
#include "GaudiKernel/ServiceHandle.h"

/** @class ByteStreamOutputSvc
 *  @brief This class provides the services for merging FullEventFragment with existing bytestream input.
 *  Mostly meant for adding new L2+EF results
 **/
class ByteStreamMergeOutputSvc : public ByteStreamOutputSvc {
public:
   /// Constructors:
   ByteStreamMergeOutputSvc(const std::string& name, ISvcLocator* svcloc);

   /// Destructor.
   virtual ~ByteStreamMergeOutputSvc();

   virtual StatusCode initialize();
   /// Implementation of the ByteStreamOutputSvc interface methods.
   virtual bool putEvent(RawEvent* re);

   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

private:
   uint32_t reducedROBid(uint32_t);

   StringProperty m_inSvcName;
   StringProperty m_outSvcName;
   ByteStreamInputSvc*  m_inSvc;
   ByteStreamOutputSvc* m_outSvc;
   StringProperty m_bsOutputStreamName; //!< stream name for multiple output
   BooleanProperty m_overwriteHeader;
};

#endif
