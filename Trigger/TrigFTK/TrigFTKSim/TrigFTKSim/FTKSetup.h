/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKSETUP_H
#define FTKSETUP_H

#ifndef FTK_STANDALONE
#include "GaudiKernel/MsgStream.h"
#endif

#include "ftktypes.h"

#include <sys/time.h>

#include <ostream>
#include <cstdarg>
#include <stdexcept>

/** define an exception class for a generic FTK error */
class FTKException : public std::runtime_error {
public:
  FTKException(const std::string &descr) : std::runtime_error(descr) {} 
};
					   
class FTKSetup {
private:
  static FTKSetup m_instance; // singleton instance for the FTK setup

#ifndef FTK_STANDALONE
  static MsgStream *m_log;
#endif


  int m_pid; // current process PID

  struct timeval m_starttime; // time at the start

  bool m_BarrelOnly; // true if the EndCap data are not processed

  bool m_SCTtrkMode;  // if using PIXEL + SCTtrk configuration (Constantinos's 2nd stage)

  int m_IBLMode;  // if using IBL configuration

  bool m_ITkMode; // use ITk geometry

  int m_verbosity; /// verbosity level

  int m_tot_events; // number of event to process

  int m_EnableFTKSim; // !=0 if FTK simulation is enabled

  int m_RawMode; // When 1 (always) the incoming data are raw hits

  int m_SectorsAsPatterns; // the field chages the SS calculation to the module ID, meaning that the sectors are used in place of patterns

  int m_MaxMissingPlanes; // maximum number of missing planes

  unsigned m_MaskLayers; // a binary mask that set which layers are always discarded

  int m_RoadWarrior; // a variable to check if the RW is enabled

  int m_KeepRemoved; // it's true if the RW rejeceted tracks are just falgged

  unsigned int m_HWModeSS; // check the format of the SS-ID: 0 is global, 1 is local (tower based), 2 is local compressed

  int m_event; // current event

  int m_MaxMissingSctPairs; // maximum number of missing SCT pairs
			    // Note that each missing pair will count
			    // as ONE missing plane for the purpose of
			    // m_MaxMissingPlanes

  bool m_restrictSctPairLayer; // Require a missing SCT pair to be in
			       // the outer layers

  bool m_restrictSctPairModule; // Require the outer missing SCT pair
				// to correspond to the barrel-disk
				// transition region. Implies the
				// previous setting is true.

  int m_usage_ncalls; // counter of the statistic calls

  bool m_fixEndcapL0; //fix for endcap L0 in clustering

  FTKSetup(); // private constructor


public:

  ~FTKSetup(); // class destructor

  int getVerbosity() const { return m_verbosity; }
  void setVerbosity(int v) { m_verbosity = v; }

  int getTotEvents() const { return m_tot_events; }
  void setTotEvents(int v) { m_tot_events = v; }

  void usageStat(const char *header=0);

  void setBarrelOnly(bool v) { m_BarrelOnly = v; }
  bool getBarrelOnly() const { return m_BarrelOnly; }

  void setSCTtrkMode(bool v) { m_SCTtrkMode = v; }
  bool getSCTtrkMode() const { return m_SCTtrkMode; }

  void setIBLMode(int v) { m_IBLMode = v; }
  int getIBLMode() const { return m_IBLMode; }

  void setfixEndcapL0(bool v) {m_fixEndcapL0 = v;}
  bool getfixEndcapL0() const { return m_fixEndcapL0;}

  void setITkMode(bool v) { m_ITkMode = v; }
  bool getITkMode() const { return m_ITkMode; }
  
  void setEnableFTKSim(int v) { m_EnableFTKSim = v; }
  int getEnableFTKSim() const { return m_EnableFTKSim; }

  void setRawMode(int v) { m_RawMode = v; }
  int getRawMode() const { return m_RawMode; }

  void setMaxMissingPlanes(int v) { m_MaxMissingPlanes = v; }
  int getMaxMissingPlanes() const { return m_MaxMissingPlanes; }

  void setMaskLayers(unsigned v) { m_MaskLayers = v; }
  unsigned getMaskLayers() const { return m_MaskLayers; }

  void setMaxMissingSctPairs(int v) { m_MaxMissingSctPairs = v; }
  int getMaxMissingSctPairs() const { return m_MaxMissingSctPairs; }

  void setRestrictSctPairLayer(bool v) { m_restrictSctPairLayer = v; }
  bool getRestrictSctPairLayer() const { return m_restrictSctPairLayer; }

  void setRestrictSctPairModule(bool v) { m_restrictSctPairModule = v; }
  bool getRestrictSctPairModule() const { return m_restrictSctPairModule; }

  void setRoadWarrior(int v) { m_RoadWarrior = v; }
  int getRoadWarrior() const { return m_RoadWarrior; }

  void setKeepRemoved(int v) { m_KeepRemoved = v; }
  int getKeepRemoved() const { return m_KeepRemoved; }

  void setSectorsAsPatterns(int v) { m_SectorsAsPatterns = v; }
  int getSectorsAsPatterns() const { return m_SectorsAsPatterns; }

  void setHWModeSS(unsigned int val) { m_HWModeSS = val; }
  const unsigned int &getHWModeSS() { return m_HWModeSS; }
  
  // Message printing operator
  static void  PrintMessage(ftk::message_level_t, const char *);
  static void  PrintMessageFmt(ftk::message_level_t, const char *,...);

  // Print extra debug info (global info for Naoki simulation)
  static int getDBG() {return false; }

  /** return the reference to the unique instance of FTKSetup
      class */
  static FTKSetup& getFTKSetup() { return m_instance; }


#ifndef FTK_STANDALONE
  static MsgStream *getMsgStream() { return m_log; }
  static void setMsgStream(MsgStream *stream) { m_log = stream; }
#endif
};
#endif // FTKSETUP_H
