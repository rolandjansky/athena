/*
 * IIDTrackCaloDepositsDecoratorTool.h
 *
 *  Created on: Oct 29, 2015
 *      Author: mbellomo
 */

#ifndef IIDTRACKCALODEPOSITSDECORATORTOOL_H_
#define IIDTRACKCALODEPOSITSDECORATORTOOL_H_


#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticle.h"


class IIDTrackCaloDepositsDecoratorTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IIDTrackCaloDepositsDecoratorTool)
public:
    virtual StatusCode decorate (const xAOD::IParticle* part) const = 0;
};


#endif /* IIDTRACKISOLATIONDECORATORTOOL_H_ */
