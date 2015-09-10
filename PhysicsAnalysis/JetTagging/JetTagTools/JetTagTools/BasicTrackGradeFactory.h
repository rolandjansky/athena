/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_BasicTrackGradeFactory_H
#define JETTAGTOOLS_BasicTrackGradeFactory_H
/**
   @class BasicTrackGradeFactory
   Package : JetTagTools
   @author  Marc Lehmacher & Giacinto Piacquadio
   Created : 19 May 2008

   Basic Track Grade Factory (provides "Good" and "Shared" classes

*/

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetTagTools/ITrackGradeFactory.h"
#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "JetTagInfo/TrackGradesDefinition.h"
#include "xAODTracking/TrackParticle.h"


//namespace xAOD { class TrackParticle; }
  

namespace Analysis
{

  class TrackGrade;

class BasicTrackGradeFactory : public AthAlgTool, virtual public ITrackGradeFactory
{
 public:

  BasicTrackGradeFactory(const std::string&,const std::string&,const IInterface*);
  virtual ~BasicTrackGradeFactory();

  /** AlgTool initailize method */
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  TrackGrade* getGrade(const xAOD::TrackParticle & track, const xAOD::IParticle::FourMom_t &) const;

  
  virtual const TrackGradesDefinition & getTrackGradesDefinition() const;

private:

  TrackGradesDefinition m_trackGradesDefinition;

  bool m_useSharedHitInfo; /// if false the following cuts are ignored
  int m_nSharedBLayer;/// max. number of shared hits in B layer
  int m_nSharedPix;   /// max. number of shared hits in pixels
  int m_nSharedSct;   /// max. number of shared hits in SCT
  int m_nSharedSi;    /// max. number of shared hits in pixels+SCT

};

}
#endif /// JETTAGTOOLS_BasicTrackGradeFactory_H

