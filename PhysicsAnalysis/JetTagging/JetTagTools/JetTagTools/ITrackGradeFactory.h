/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_ITrackGradeFactory_H
#define JETTAGTOOLS_ITrackGradeFactory_H
/**
   @class ITrackGradeFactory
   Package : JetTagTools
   @author  Marc Lehmacher & Giacinto Piacquadio
   Created : 19 May 2008

   Track Grade Factory interface class

*/

#include "GaudiKernel/IAlgTool.h"
#include <string>
#include <vector>
#include "xAODTracking/TrackParticle.h"
#include "xAODJet/Jet.h"

//namespace xAOD { 
//  class TrackParticle; 
//  class JetFourMom_t;
//}


namespace Analysis
{

  

  class TrackGrade;
  class TrackGradesDefinition;

  static const InterfaceID IID_ITrackGradeFactory("Analysis::ITrackGradeFactory", 1, 0);

  /** \class ITrackGradeFactory
    Interface class for the CombinerTool AlgTool, it inherits from IAlgTool
    Detailed information about private members and member functions can be found in the actual implementation class
    CombinerTool.

    @author Andreas.Wildauer@cern.ch
    */

class ITrackGradeFactory : virtual public IAlgTool {

public:
  
  /** Virtual destructor */
  virtual ~ITrackGradeFactory(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITrackGradeFactory; };
  
  virtual TrackGrade* getGrade(const xAOD::TrackParticle &,const xAOD::IParticle::FourMom_t & jetMomentum) const=0;
  

  virtual const TrackGradesDefinition & getTrackGradesDefinition() const=0;

};

}

#endif 

