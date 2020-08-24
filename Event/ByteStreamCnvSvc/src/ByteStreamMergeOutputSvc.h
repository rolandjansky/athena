/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

   virtual StatusCode initialize() override;
   /// Implementation of the ByteStreamOutputSvc interface methods.
   virtual bool putEvent(RawEvent* re) override;
   virtual bool putEvent(RawEvent* re, const EventContext& ctx) override;

   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

private:
   uint32_t reducedROBid(uint32_t);

   Gaudi::Property<std::string> m_inSvcName;
   Gaudi::Property<std::string> m_outSvcName;
   ByteStreamInputSvc*  m_inSvc;
   ByteStreamOutputSvc* m_outSvc;
   Gaudi::Property<std::string> m_bsOutputStreamName; //!< stream name for multiple output
   Gaudi::Property<bool> m_overwriteHeader;
};

#endif
