/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MYNEWPACKAGE_MYALGTOOL_H
#define MYNEWPACKAGE_MYALGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DevTutorialWriteAlgAndTool/IMyAlgTool.h"

class MyAlgTool : public AthAlgTool, virtual public IMyAlgTool {
public:
      MyAlgTool(const std::string&,const std::string&,const IInterface*);
      MyAlgTool& operator= (const MyAlgTool& ) = delete;
        
      bool selectTrackParticle( const xAOD::TrackParticle& particle) const override;
};

#endif
