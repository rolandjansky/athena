/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ICalibMuonsSelectorTool_h__
#define __ICalibMuonsSelectorTool_h__

//::: ROOT includes
#include "TH1F.h"

//::: Framework includes
#include "GaudiKernel/IAlgTool.h"

//::: EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"


static const InterfaceID IID_ICalibMuonsSelectorTool( "ICalibMuonsSelectorTool", 1, 0 );

typedef std::vector< std::pair< const xAOD::Muon*, const xAOD::Muon* > > PairsVector;

class ICalibMuonsSelectorTool : virtual public ::IAlgTool { 

public: 

  virtual ~ICalibMuonsSelectorTool() {};

  static const InterfaceID& interfaceID();

  virtual PairsVector GetMuons( const xAOD::MuonContainer* the_muons, TH1F* cut_flow ) = 0;

protected: 

}; 

inline const InterfaceID& ICalibMuonsSelectorTool::interfaceID() { 

   return IID_ICalibMuonsSelectorTool; 

}


#endif 
