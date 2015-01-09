/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileTBOldNtupleRead_H
#define TileTBOldNtupleRead_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// STL includes
#include <string>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TTree.h"

#include "TileConditions/TileCablingService.h"

class StoreGateSvc;
class TileHWID;
class TileBeamInfoProvider;
class TileTBOldNtupleStruct;

class TileTBOldNtupleRead : public AthAlgorithm {
public:
  TileTBOldNtupleRead(std::string name, ISvcLocator* pSvcLocator);
  virtual ~TileTBOldNtupleRead();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  int RunNumber()   {return m_runNumber;}
  int EventNumber() {return m_eventNumber;}
  int EventShift(int dr)  {return m_eventShift[dr];}
  int TrigType()    {return m_trigType;}

  int NSamp()   {return m_nSamp;}
  bool BiGain() {return m_bigain;}

  std::string& DigitsContainer() { return m_digitsContainer; }
  std::string& BeamInfoContainer() { return m_beamContainer; }

  TTree* getNtuple() {return m_ntuple;}
  
  static TileTBOldNtupleRead * getInstance();

private:

  std::string m_ntupleID;
  std::string m_ntupleDir;
  std::string m_digitsContainer;
  std::string m_beamContainer;
  
  bool m_bigain;
  bool m_pmtOrder;
  bool m_barrelOnly;
  bool m_adderFit;
  std::vector<int> m_eventShift;

  const TileHWID * m_tileHWID;
  const TileCablingService * m_cabling;
  ToolHandle<TileBeamInfoProvider> m_beamInfo;

  int m_runNumber;
  int m_eventNumber;
  int m_maxEventNumber;
  int m_trigType;
  int m_nSamp;

  TFile * m_file;
  TTree * m_ntuple;
  TileTBOldNtupleStruct * m_ntupleStruct;

  static TileTBOldNtupleRead * s_instance ;

  inline int digiChannel2PMT(int ros, int chan) {
    return (abs(m_cabling->channel2hole(ros,chan))-1); 
  }

  inline int drawer2ros(int ros, int drawer) {
    if (m_barrelOnly) return ros;
    else return (drawer<2) ? ros : ros+2;
  }
      
};

#endif
