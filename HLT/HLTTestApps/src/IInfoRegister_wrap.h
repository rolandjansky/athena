/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file src/IInfoRegister_wrap.h
 * @author Ricardo Abreu
 *
 * @brief Python bindings for hltinterface::IInfoRegister
 */

#ifndef IINFOREGISTER_WRAP_H_
#define IINFOREGISTER_WRAP_H_

namespace HLTTestApps
{
  /**
   *  Wrap the IInfoRegister type. When this is called, a wrapped IInfoRegister
   *  is declared for Python usage, with the following public members:
   *    - a ctor, which receives a string, specifying the name of the
   *      library that contains a particular IInfoRegister implementation
   *    - configure, which receives a ptree, and returns a boolean value
   *      indicating success or failure
   *    - prepareForRun, which receives a ptree, and returns a boolean value
   *      indicating success or failure
   *    - prepareWorker, which receives a ptree, and returns a boolean value
   *      indicating success or failure
   *    - finalizeWorker, which receives a ptree, and returns a boolean value
   *      indicating success or failure
   *    - finalize, which receives a ptree, and returns a boolean value
   *      indicating success or failure
   *
   *  For more information, refer to the IInfoRegister directly.
   *
   */
  void wrap_IInfoRegister();
}

#endif /* IINFOREGISTER_WRAP_H_ */
