/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRACKFILEOUTPUT_H
#define FTKTRACKFILEOUTPUT_H

#include <TFile.h>
#include <TTree.h>

#include "TrigFTKSim/FTKTrackOutput.h"
#include "FTKObjectOutput.h"
#include "FTKTrackStream.h"

#include <string>
#include <vector>

class FTKTrackFileOutput : public FTKObjectOutput<FTKTrackStream>, public FTKTrackOutput {
 public:
  FTKTrackFileOutput();
  virtual ~FTKTrackFileOutput();

  virtual void setNBanks(int val) 
  { FTKObjectOutput<FTKTrackStream>::setNBanks(val); }
  virtual int getNBanks() const
  { return FTKObjectOutput<FTKTrackStream>::getNBanks(); }

  virtual void setFileName(const char *fname)
  { FTKObjectOutput<FTKTrackStream>::setFileName(fname);}
  virtual const char* getFileName() const 
  { return FTKObjectOutput<FTKTrackStream>::getFileName(); }
  virtual void setOutDir(const char *dname)
  { FTKObjectOutput<FTKTrackStream>::setOutDir(dname); }
  virtual const char* getOutDir() const
  { return FTKObjectOutput<FTKTrackStream>::getOutDir(); }
  virtual void setMultiOut(bool v)
  { FTKObjectOutput<FTKTrackStream>::setMultiOut(v); }
  virtual bool getMultiOut() const
  { return FTKObjectOutput<FTKTrackStream>::getMultiOut(); }
  virtual void init(int bufsize = 0, bool *goodregions=0x0)
  { FTKObjectOutput<FTKTrackStream>::init(bufsize,goodregions); }
  virtual FTKTrackStream* getStream(int id) 
  { return FTKObjectOutput<FTKTrackStream>::getStream(id); }

  virtual void beginFile(const char *fname=0)
  { FTKObjectOutput<FTKTrackStream>::beginFile(fname); }
  virtual void eventBegin() { FTKObjectOutput<FTKTrackStream>::eventBegin(); }
  virtual void eventEnd() { FTKObjectOutput<FTKTrackStream>::eventEnd(); }
  virtual void endFile() { FTKObjectOutput<FTKTrackStream>::endFile(); }

  virtual unsigned long runNumber(const unsigned int& ival) const
  { return FTKObjectOutput<FTKTrackStream>::runNumber(ival); }
  virtual unsigned long eventNumber(const unsigned int &ival) const
  { return FTKObjectOutput<FTKTrackStream>::eventNumber(ival); }
  virtual void setRunNumber(const unsigned int &v1,const unsigned long &v2)
  { FTKObjectOutput<FTKTrackStream>::setRunNumber(v1,v2); }
  virtual void setEventNumber(const unsigned int &v1,const unsigned long &v2)
  { FTKObjectOutput<FTKTrackStream>::setEventNumber(v1,v2); }

  void addTrack(int, const FTKTrack&);
  void addTrack_pre_hw(int, const FTKTrack&);

  void addNCombs(int ib, int v)
    { m_data[ib]->addNCombs(v); }
  void addNFits(int ib, int v)
    { m_data[ib]->addNFits(v); }
  void addNFitsMajority(int ib, int v)
    { m_data[ib]->addNFitsMajority(v); }
  void addNFitsMajority_pix(int ib, int v)
    { m_data[ib]->addNFitsMajority_pix(v); }
  void addNFitsMajority_SCT(int ib, int v)
    { m_data[ib]->addNFitsMajority_SCT(v); }
  void addNFitsRecovery(int ib, int v)
    { m_data[ib]->addNFitsRecovery(v); }
  void addNAddFitsRecovery(int ib, int v)
    { m_data[ib]->addNAddFitsRecovery(v); }
  void addNFitsBad(int ib, int v)
    { m_data[ib]->addNFitsBad(v); }
  void addNFitsHWRejected(int ib, int v)
    { m_data[ib]->addNFitsHWRejected(v); }
  void addNFitsBadMajority(int ib, int v)
    { m_data[ib]->addNFitsBadMajority(v); }
  void addNFitsHWRejectedMajority(int ib, int v)
    { m_data[ib]->addNFitsHWRejectedMajority(v); }
  void addNConnections(int ib, int v) 
    { m_data[ib]->addNConnections(v); }
  void addNExtrapolatedTracks(int ib, int v)
    { m_data[ib]->addNExtrapolatedTracks(v); }

  void addTrackI(int, const FTKTrack&);
  void addTrackI_pre_hw(int, const FTKTrack&);

  void addNCombsI(int ib, int v)
    { m_data[ib]->addNCombsI(v); }
  void addNFitsI(int ib, int v)
    { m_data[ib]->addNFitsI(v); }
  void addNFitsMajorityI(int ib, int v)
    { m_data[ib]->addNFitsMajorityI(v); }
  void addNFitsMajorityI_SCT(int ib, int v)
    { m_data[ib]->addNFitsMajorityI_SCT(v); }
  void addNFitsMajorityI_pix(int ib, int v)
    { m_data[ib]->addNFitsMajorityI_pix(v); }
  void addNFitsRecoveryI(int ib, int v)
    { m_data[ib]->addNFitsRecoveryI(v); }
  void addNAddFitsRecoveryI(int ib, int v)
    { m_data[ib]->addNAddFitsRecoveryI(v); }
  void addNFitsBadI(int ib, int v)
    { m_data[ib]->addNFitsBadI(v); }
  void addNFitsHWRejectedI(int ib, int v)
    { m_data[ib]->addNFitsHWRejectedI(v); }
  void addNFitsBadMajorityI(int ib, int v)
    { m_data[ib]->addNFitsBadMajorityI(v); }
  void addNFitsHWRejectedMajorityI(int ib, int v)
    { m_data[ib]->addNFitsHWRejectedMajorityI(v); }

  void addTrack_pattern(int, const FTKTrack&);
  void addTrack_hits(int, const FTKTrack&);

};

#endif // FTKTRACKFILEOUTPUT_H
