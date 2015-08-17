/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************************
// $Id:$
//
// InDetGlobalBeamSpotMonTool is a module to monitor primary vertices and the beam spot in
// the context of package InnerDetector/InDetMonitoring/InDetGlobalMonitoring. A
// scaled-down version doing only primary vertex monitoring is available as module
// InDetGlobalPrimaryVertexMonTool (the reason for having two tools is that in
// InDetGlobalBeamSpotMonTool monitoring is done wrt beam spot, while InDetGlobalPrimaryVertexMonTool
// does not use the beam spot). Originally, this module was developed in package
// InDetAlignmentMonitoring under the name InDetAlignMonBeamSpot.
//
// Written in March 2008 by Juerg Beringer (LBNL)
//
// ********************************************************************************

#ifndef InDetGlobalBeamSpotMonTool_H
#define InDetGlobalBeamSpotMonTool_H

//#include <vector>
#include <string>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetGlobalMotherMonTool.h"

class TH1F_LW;
class TH2F_LW;

class IBeamCondSvc;

class InDetGlobalBeamSpotMonTool : public InDetGlobalMotherMonTool {

public:
  InDetGlobalBeamSpotMonTool( const std::string & type, const std::string & name, const IInterface* parent ); 

  virtual ~InDetGlobalBeamSpotMonTool();

  virtual StatusCode initialize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

protected:
  ServiceHandle<IBeamCondSvc> m_beamCondSvc;
  bool m_hasBeamCondSvc;

  TH1F_LW* m_hTrNPt;
  TH1F_LW* m_hTrPt;
  TH2F_LW* m_hTrDPhi;
  TH2F_LW* m_hTrDPhiCorr;

  TH1F_LW* m_hBsX;
  TH1F_LW* m_hBsY;
  TH1F_LW* m_hBsZ;
  TH1F_LW* m_hBsTiltX;
  TH1F_LW* m_hBsTiltY;

  TH1F_LW* m_hPvN;
  TH1F_LW* m_hPvNPriVtx;
  TH1F_LW* m_hPvNPileupVtx;

  TH1F_LW* m_hPvX;
  TH1F_LW* m_hPvY;
  TH1F_LW* m_hPvZ;
  TH1F_LW* m_hPvErrX;
  TH1F_LW* m_hPvErrY;
  TH1F_LW* m_hPvErrZ;
  TH1F_LW* m_hPvChiSqDoF;

  TH2F_LW* m_hPvXZ;
  TH2F_LW* m_hPvYZ;
  TH2F_LW* m_hPvYX;

  TH1F_LW* m_hPvNTracksAll;
  TH1F_LW* m_hPvNTracks;
  TH1F_LW* m_hPvTrackPt;
  TH1F_LW* m_hPvTrackEta;

private:
  TH1F_LW* makeAndRegisterTH1F(MonGroup& mon, const char* name, std::string title, int nBins, float minX, float maxX);
  TH2F_LW* makeAndRegisterTH2F(MonGroup& mon, const char* hName, std::string hTitle,
  int nBinsX, float minX, float maxX,
  int nBinsY, float minY, float maxY);

  std::string m_stream;
  bool m_useBeamspot;
  std::string m_vxContainerName;
  bool m_vxContainerWithBeamConstraint;
  std::string m_trackContainerName;
  std::string m_histFolder;
  std::string m_triggerChainName;
  unsigned int m_minTracksPerVtx;
  float m_minTrackPt;
};

#endif
