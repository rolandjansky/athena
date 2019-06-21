/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRACKOUTPUT_H
#define FTKTRACKOUTPUT_H

#include <TFile.h>
#include <TTree.h>

#include "FTKTrackStream.h"

#include <string>
#include <vector>

class FTKTrackOutput {
public:
  virtual ~FTKTrackOutput() {;}

  virtual void setNBanks(int) = 0;
  virtual int getNBanks() const = 0;

  virtual void setFileName(const char *fname) = 0;
  virtual const char* getFileName() const = 0;
  virtual void setOutDir(const char *dname) = 0;
  virtual const char* getOutDir() const = 0;
  virtual void setMultiOut(bool v) = 0;
  virtual bool getMultiOut() const = 0;
  virtual void init(int bufsize = 0, bool *goodregions=0x0) = 0;
  virtual FTKTrackStream* getStream(int id) = 0;

  virtual void beginFile(const char *fname=0) = 0;
  virtual void eventBegin() = 0;
  virtual void eventEnd() = 0;
  virtual void endFile() = 0;

  virtual unsigned long runNumber(const unsigned int&) const = 0;
  virtual unsigned long eventNumber(const unsigned int&) const = 0;
  virtual void setRunNumber(const unsigned int&,const unsigned long&) = 0;
  virtual void setEventNumber(const unsigned int&,const unsigned long&) = 0;


  virtual void addTrack(int, const FTKTrack&) = 0;
  virtual void addTrack_pre_hw(int, const FTKTrack&) = 0;

  virtual void addNCombs(int ib, int v) = 0;
  virtual void addNFits(int ib, int v) = 0;
  virtual void addNFitsMajority(int ib, int v) = 0;
  virtual void addNFitsMajority_pix(int ib, int v) = 0;
  virtual void addNFitsMajority_SCT(int ib, int v) = 0;
  virtual void addNFitsRecovery(int ib, int v) = 0;
  virtual void addNAddFitsRecovery(int ib, int v) = 0;
  virtual void addNFitsBad(int ib, int v) = 0;
  virtual void addNFitsHWRejected(int ib, int v) = 0;
  virtual void addNFitsBadMajority(int ib, int v) = 0;
  virtual void addNFitsHWRejectedMajority(int ib, int v) = 0;
  virtual void addNConnections(int ib, int v) = 0;
  virtual void addNExtrapolatedTracks(int ib, int v) = 0;

  virtual void addTrackI(int, const FTKTrack&) = 0;
  virtual void addTrackI_pre_hw(int, const FTKTrack&) = 0;
  
  virtual void addNCombsI(int ib, int v) = 0;
  virtual void addNFitsI(int ib, int v) = 0;
  virtual void addNFitsMajorityI(int ib, int v) = 0;
  virtual void addNFitsMajorityI_pix(int ib, int v) = 0;
  virtual void addNFitsMajorityI_SCT(int ib, int v) = 0;
  virtual void addNFitsRecoveryI(int ib, int v) = 0;
  virtual void addNAddFitsRecoveryI(int ib, int v) = 0;
  virtual void addNFitsBadI(int ib, int v) = 0;
  virtual void addNFitsHWRejectedI(int ib, int v) = 0;
  virtual void addNFitsBadMajorityI(int ib, int v) = 0;
  virtual void addNFitsHWRejectedMajorityI(int ib, int v) = 0;

  virtual void addTrack_pattern(int, const FTKTrack&) = 0;
  virtual void addTrack_hits(int, const FTKTrack&) = 0;

};

#endif // FTKTRACKOUTPUT_H
