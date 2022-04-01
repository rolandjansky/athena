/*
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTCONFTOOLS_IHTTEVENTSELECTIONSVC_H
#define HTTCONFTOOLS_IHTTEVENTSELECTIONSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IService.h"

class HTTHit;
class HTTTrack;
class HTTOfflineTrack;
class HTTTruthTrack;
class HTTTrackPars;
class HTTRegionSlices;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;
enum class SampleType;

class IHTTEventSelectionSvc : public virtual IService
{
 public:

  static const InterfaceID& interfaceID();

  virtual HTTTrackPars getMin() const = 0;
  virtual HTTTrackPars getMax() const = 0;
  virtual unsigned getRegionID() const = 0;
  virtual SampleType getSampleType() const = 0;
  virtual bool checkPU() const = 0;
  virtual int getLRTpdgID() const = 0;
  virtual const HTTRegionSlices* getRegions() = 0;

  virtual bool passCuts(const HTTHit&) const = 0;
  virtual bool passCuts(const HTTTrack&) const = 0;
  virtual bool passCuts(const HTTOfflineTrack&) const = 0;
  virtual bool passCuts(const HTTTruthTrack&) const = 0;

  virtual bool passMatching(HTTTrack const &) const = 0;
  virtual bool passMatching(HTTTruthTrack const &) const = 0;

  virtual bool selectEvent(HTTEventInputHeader*) const = 0;
  virtual bool selectEvent(HTTLogicalEventInputHeader*) const = 0;

  virtual bool passQOverPt(const HTTTrack&) const = 0;
  virtual bool passEta(const HTTTrack&) const = 0;
  virtual bool passPhi(const HTTTrack&) const = 0;
  virtual bool passD0(const HTTTrack&) const = 0;
  virtual bool passZ0(const HTTTrack&) const = 0;

  virtual bool passQOverPt(const HTTOfflineTrack&) const = 0;
  virtual bool passEta(const HTTOfflineTrack&) const = 0;
  virtual bool passPhi(const HTTOfflineTrack&) const = 0;
  virtual bool passD0(const HTTOfflineTrack&) const = 0;
  virtual bool passZ0(const HTTOfflineTrack&) const = 0;
};

inline const InterfaceID& IHTTEventSelectionSvc::interfaceID()
{
  static const InterfaceID IID("IHTTEventSelectionSvc", 1, 0);
  return IID;
}

#endif  //HTTCONFTOOLS_IHTTEVENTSELECTIONSVC_H
