/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __FTK_DATAPROVIDERTOOL_H__
#define __FTK_DATAPROVIDERTOOL_H__

#include <string>
#include <vector>

//#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetToolInterfaces/IFTK_DataProviderTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/ServiceHandle.h"

#include "TrkTrack/TrackCollection.h"
//#include "TrigInDetPattRecoEvent/TrigFTK_TrackSeed.h"

class IRoiDescriptor;
class TrigFTK_TrackSeed;


class FTK_DataProviderTool: public AthAlgTool, virtual public IFTK_DataProviderTool { 
  
public: 
  
  FTK_DataProviderTool(const std::string&, const std::string&, const IInterface*);
  virtual ~FTK_DataProviderTool();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  virtual StatusCode retrieveFTK_Data(TrackCollection&);
  virtual StatusCode retrieveFTK_Data(const IRoiDescriptor&, TrackCollection&);
  virtual StatusCode retrieveFTK_Data(const IRoiDescriptor&, std::vector<const Trk::Track*>&);
  virtual StatusCode retrieveFTK_Data(std::vector<const Trk::Track*>&);
  virtual StatusCode retrieveFTK_Data(const IRoiDescriptor&, std::vector<TrigFTK_TrackSeed>&);
  virtual StatusCode retrieveFTK_Data(std::vector<TrigFTK_TrackSeed>&);
  
private:
  
  std::string m_ftkTracksName;
  const TrackCollection* m_ftkTrackContainer;  
  
  StatusCode getTrackCollection();
  Trk::Track* createOutputTrack(const Trk::Track*); 
  
};


#endif
