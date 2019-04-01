/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_STRAWSTATUSSUMMARYTOOL_H
#define ITRT_STRAWSTATUSSUMMARYTOOL_H
/** @file ITRT_StrawStatusSummaryTool.h
 * @brief abstract interface to TRT straw status constants
 * @author Peter Hansen <phansen@nbi.dk>
 */

#include <string>
#include "GaudiKernel/IAlgTool.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/TRTStrawStatusData.h"
#include "TRT_ConditionsData/StrawStatusContainer.h"

class Identifier;
namespace TRTCOND {
  class ExpandedIdentifier;
}

/** @class ITRT_StrawStatusSummaryTool
 abstract interface to TRT straw status constants
*/
class ITRT_StrawStatusSummaryTool: virtual public IAlgTool
{
 public:

  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer ;

  DeclareInterfaceID(ITRT_StrawStatusSummaryTool, 1, 0);

  virtual int getStatus(Identifier ) const =0;
  virtual int getStatusPermanent(Identifier) const =0;
  virtual int getStatusHT(Identifier) const =0;
  virtual bool get_status(Identifier) const =0;
  virtual bool get_statusHT(Identifier) const =0;

  virtual const StrawStatusContainer* getStrawStatusContainer() const =0;
  virtual const StrawStatusContainer* getStrawStatusPermanentContainer() const =0;
  virtual const StrawStatusContainer* getStrawStatusHTContainer() const =0;

};

#endif //  ITRT_STRAWSTATUSSUMMARYTOOL_H
