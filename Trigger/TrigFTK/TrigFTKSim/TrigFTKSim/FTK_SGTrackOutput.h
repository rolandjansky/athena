/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SGTrackOutput_H
#define FTK_SGTrackOutput_H

#include "TrigFTKSim/FTKTrackOutput.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "TrigFTKSim/FTK_SGTrackOutputI.h"

#include "TrigFTKPool/FTKAthTrackContainer.h"
#include "TrigFTKPool/FTKTrackFitterStats.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

class FTK_SGTrackOutput : virtual public FTK_SGTrackOutputI, public AthAlgTool,
			 public FTKTrackOutput {
private:
  int m_nbanks;

  FTKAthTrackContainer **m_tracks; // arrays of tracks
  FTKAthTrackContainer **m_tracks_pre_hw; // arrays of tracks
  FTKTrackFitterStats **m_TFstats; // TF informations

  bool m_storeIncomplete;

  FTKAthTrackContainer **m_tracksI; // arrays of tracks
  FTKAthTrackContainer **m_tracksI_pre_hw; // arrays of tracks
  FTKTrackFitterStats **m_TFstatsI; // TF informations

  FTKAthTrackContainer **m_tracks_pattern; // arrays of tracks
  FTKAthTrackContainer **m_tracks_hits; // arrays of tracks

  StoreGateSvc*  m_storeGate;

  mutable MsgStream m_log;

  void notValid(const char *);

public:
  FTK_SGTrackOutput(const std::string&, const std::string&, const IInterface*);
  virtual ~FTK_SGTrackOutput();

  StatusCode initialize();
  StatusCode finalize();

  virtual FTKTrackOutput* reference() { return dynamic_cast<FTKTrackOutput*>(this);}


  virtual void setNBanks(int nbanks) { m_nbanks = nbanks; }
  virtual int getNBanks() const { return m_nbanks; }

  virtual void setFileName(const char *) { notValid("SetFileName"); }
  virtual const char* getFileName() const { return "StoreGate"; }
  virtual void setOutDir(const char *) { notValid("SetOutDir"); }
  virtual const char* getOutDir() const { return "StoreGate"; }
  virtual void setMultiOut(bool) { notValid("setMultiOut"); }
  virtual bool getMultiOut() const { return false; }
  virtual void init(int bufsize = 0, bool *goodregions=0x0);
  virtual FTKTrackStream* getStream(int) { return 0x0; }

  virtual void beginFile(const char *fname=0);
  virtual void eventBegin();
  virtual void eventEnd();
  virtual void endFile();

  virtual unsigned long runNumber(const unsigned int&) const;
  virtual unsigned long eventNumber(const unsigned int&) const;
  virtual void setRunNumber(const unsigned int&,const unsigned long&);
  virtual void setEventNumber(const unsigned int&,const unsigned long&);


  virtual void addTrack(int, const FTKTrack&);
  virtual void addTrack_pre_hw(int, const FTKTrack&);
  virtual void addNCombs(int ib, int v);
  virtual void addNFits(int ib, int v);
  virtual void addNFitsMajority(int ib, int v);
  virtual void addNFitsMajority_pix(int ib, int v);
  virtual void addNFitsMajority_SCT(int ib, int v);
  virtual void addNFitsRecovery(int ib, int v);
  virtual void addNAddFitsRecovery(int ib, int v);
  virtual void addNFitsBad(int ib, int v);
  virtual void addNFitsHWRejected(int ib, int v);
  virtual void addNFitsBadMajority(int ib, int v);
  virtual void addNFitsHWRejectedMajority(int ib, int v);
  virtual void addNConnections(int ib, int v);
  virtual void addNExtrapolatedTracks(int ib, int v);

  virtual void addTrackI(int, const FTKTrack&);
  virtual void addTrackI_pre_hw(int, const FTKTrack&);
  virtual void addNCombsI(int ib, int v);
  virtual void addNFitsI(int ib, int v);
  virtual void addNFitsMajorityI(int ib, int v);
  virtual void addNFitsMajorityI_pix(int ib, int v);
  virtual void addNFitsMajorityI_SCT(int ib, int v);
  virtual void addNFitsRecoveryI(int ib, int v);
  virtual void addNAddFitsRecoveryI(int ib, int v);
  virtual void addNFitsBadI(int ib, int v);
  virtual void addNFitsHWRejectedI(int ib, int v);
  virtual void addNFitsBadMajorityI(int ib, int v);
  virtual void addNFitsHWRejectedMajorityI(int ib, int v);

  virtual void addTrack_pattern(int, const FTKTrack&);
  virtual void addTrack_hits(int, const FTKTrack&);


};
#endif // FTK_SGRoadOutput_H
