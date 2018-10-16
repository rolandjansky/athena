/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SoTransparency                      //
//                                                            //
//  Description: Custom node for VP1 which sets the absolute  //
//               or relative transparency of the current      //
//               state.                                       //
//                                                            //
//  Author: Troels Kofoed Jacobsen                            //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1HEPVis/nodes/SoTransparency.h"
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/elements/SoTransparencyElement.h>
#include <algorithm>//for min/max

bool SoTransparency::s_didInit = false;

SO_NODE_SOURCE(SoTransparency)
// Initializes the SoTransparency class.
void
SoTransparency::initClass()
{
  if ( !s_didInit )
  {
    SO_NODE_INIT_CLASS(SoTransparency, SoNode, "Node");
    s_didInit = true;
  }
}

// Constructor
SoTransparency::SoTransparency()
{
  SO_NODE_CONSTRUCTOR(SoTransparency);

  SO_NODE_ADD_FIELD(transparency, (0.5));
  SO_NODE_ADD_FIELD(relative, (true));
  setNodeType(EXTENSION);

}

// Destructor
SoTransparency::~SoTransparency()
{
}

// Implements GL render action.
void
SoTransparency::GLRender(SoGLRenderAction *action)
{
  SoTransparency::doAction(action);
  SoMaterialBundle  mb(action);
  mb.forceSend(0);
}

// Implements callback action.
void
SoTransparency::callback(SoCallbackAction *action)
{
  SoTransparency::doAction(action);
}

void
SoTransparency::doAction(SoAction *action)
{
  // Make sure the "transparency" field is not ignored. If it is,
  // then we don't need to change anything in the state.
  if (transparency.isIgnored())
    return;

  const float isRelative = relative.getValue();
  const float transpVal = transparency.getValue();
  if ( isRelative && transpVal<=0.0f )
    return;//No effect

  const SoTransparencyElement * STE = SoTransparencyElement::getInstance( action->getState() );
  const int n = STE->getNum();
  if (n<=0)
    return;//Nothing to modify

  float * finalTransparency = new float[n];//NB: Who takes care of deleting finalTransparency??
  for (int i = 0; i < n; i++)
    finalTransparency[i] = std::min<float>( 1.0f, std::max<float>( 0.0f, ( isRelative ? ( 1.0 - (1.0 - transpVal) * (1.0 - STE->get(0)) ) : transpVal ) ) );

  SoTransparencyElement::set(action->getState(), this, n, finalTransparency);

}
