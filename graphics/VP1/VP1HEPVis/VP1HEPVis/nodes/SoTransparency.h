/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SoTransparency                    //
//                                                            //
//  Author: Troels Kofoed Jacobsen                            //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/nodes/SoSubNode.h>

class SoTransparency : public SoNode {

  SO_NODE_HEADER(SoTransparency);

 public:

  // Fields:
  SoSFFloat     transparency; // Amount of transparency (0-1).
  SoSFBool      relative;     // Relative to previous transparency?
  static void   initClass();

  // Constructor
  SoTransparency();

 protected:

  // These implement supported actions. The only actions that
  // deal with materials are the callback and GL render
  // actions. We will inherit all other action methods from
  // SoNode.
  virtual void  GLRender(SoGLRenderAction *action);
  virtual void  callback(SoCallbackAction *action);
  virtual void  doAction(SoAction *action);

 private:

  // Destructor.
  virtual ~SoTransparency();

  // true if initClass() have been callen once.
  static bool s_didInit;

};
