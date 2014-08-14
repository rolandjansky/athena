/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_MBTSTimeFilterTool_H
#define PrimaryDPDMaker_MBTSTimeFilterTool_H

/** @file MBTSTimeFilterTool.h
 *  @brief This file contains the class definition for the MBTSTimeFilterTool class.
 *  @author Max Baak <mbaak@cern.ch>
 **/

#include "AthenaBaseComps/AthAlgTool.h"

/** @class MBTSTimeFilterTool
 *  @brief This file contains the class definition for the MBTSTimeFilterTool class.
 **/

static const InterfaceID IID_MBTSTimeFilterTool("MBTSTimeFilterTool", 1, 0);

class TileTBID;

class MBTSTimeFilterTool : public AthAlgTool
{
 public:    
  MBTSTimeFilterTool( const std::string&, const std::string&, const IInterface* );
  virtual ~MBTSTimeFilterTool();

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MBTSTimeFilterTool; };

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode getTimeDifference(bool& passCut, double& timeDiff, double& timeA, double&timeC, int& countA, int& countC);

 protected:

  bool m_warningPrinted;

  /** For access to the tile test beam identifiers */
  const TileTBID* m_tileTBID;

  std::string m_mbtsContainerName;
  double m_chargethreshold;
  int m_minhitsperside;
  double m_maxtimediff;
};

#endif

