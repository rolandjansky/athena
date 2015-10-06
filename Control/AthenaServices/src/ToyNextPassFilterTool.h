/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOYNEXTPASSFILTERTOOL_H 
#define TOYNEXTPASSFILTERTOOL_H 1
/** @file ToyNextPassFilterTool.h
 *  @brief a tool providing a toy implementation of INextPassFilter
 *
 *  @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 * $Id: ToyNextPassFilterTool.h,v 1.3 2008-09-23 22:00:47 binet Exp $
 */

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/INextPassFilter.h"


/** @class ToyNextPassFilterTool
 *  @brief a tool providing a toy implementation of INextPassFilter
 */

class ToyNextPassFilterTool : public AthAlgTool,
                              virtual public INextPassFilter {

public:

  /// Standard constructor
  ToyNextPassFilterTool( const std::string& type, 
            const std::string& name,
            const IInterface* parent);

  /// IAlgTool Initialize method
  //INH StatusCode initialize();

  /// IAlgTool Finalize method
  //INH  StatusCode finalize();
  
  /// implementation if INextPassFilter
  virtual bool doNextPass();

  virtual ~ToyNextPassFilterTool( ) {} 

private:
  unsigned int m_passesDone; ///< no of passes already done
  unsigned int m_nPasses; ///< property: no of pass to allow
};
#endif // TOYNEXTPASSFILTERTOOL_H
