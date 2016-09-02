/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IIDTrackIsolationDecoratorTool.h
 *
 *  Created on: Sep 29, 2015
 *      Author: goblirsc
 */

#ifndef IIDTRACKISOLATIONDECORATORTOOL_H_
#define IIDTRACKISOLATIONDECORATORTOOL_H_


#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticle.h"


class IIDTrackIsolationDecoratorTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IIDTrackIsolationDecoratorTool)
public:
    virtual StatusCode decorate (const xAOD::IParticle* part) const = 0;
};


#endif /* IIDTRACKISOLATIONDECORATORTOOL_H_ */
