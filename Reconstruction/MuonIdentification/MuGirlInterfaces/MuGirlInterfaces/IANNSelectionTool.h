/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IANNSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUGIRLNSANNSELECTIONTOOL_H
#define IMUGIRLNSANNSELECTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
namespace Trk
{
    class MeasuredPerigee;
}

namespace MuGirlNS
{
  class CandidateSummary;
  static const InterfaceID IID_IANNSelectionTool("MuGirlNS::IANNSelectionTool", 1, 0);

  class IANNSelectionTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
      virtual void evaluateANN(CandidateSummary* info, double v[8], double NN[2]) = 0; 
  };

  inline const InterfaceID& MuGirlNS::IANNSelectionTool::interfaceID()
    { 
      return IID_IANNSelectionTool; 
    }

} // end of namespace

#endif 
