/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOYNEXTPASSFILTERTOOL_H 
#define TOYNEXTPASSFILTERTOOL_H 1
/** @file ToyNextPassFilterAlg.h
 *  @brief an Algorithm providing a toy implementation of INextPassFilter
 *
 *  @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 * $Id: ToyNextPassFilterAlg.h,v 1.3 2008-09-23 22:00:47 binet Exp $
 */

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaKernel/INextPassFilter.h"

class ISvcLocator;

/** @class ToyNextPassFilterAlg
 *  @brief an Algorithm providing a toy implementation of INextPassFilter
 */

class ToyNextPassFilterAlg : public AthAlgorithm,
	                     virtual public INextPassFilter {

public:

  /// Standard constructor
  ToyNextPassFilterAlg( const std::string& name, ISvcLocator* );

  /// IAlgorithm Initialize method
  //INH StatusCode initialize();

  /// IAlgorithm execute method
  StatusCode execute();

  /// IAlgorithm Finalize method
  //INH  StatusCode finalize();
  
  /// implementation if INextPassFilter
  virtual bool doNextPass();

  virtual ~ToyNextPassFilterAlg( ) {} 

private:
  unsigned int m_passesDone; ///< no of passes already done
  unsigned int m_nPasses; ///< property: no of pass to allow
};
#endif // TOYNEXTPASSFILTERTOOL_H
