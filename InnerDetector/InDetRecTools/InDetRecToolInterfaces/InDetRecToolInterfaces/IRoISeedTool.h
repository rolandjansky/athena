/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************                           
    begin   : 27-08-2018
    authors : Matthew Gignac (UCSC)
    email   : matthew.gignac@cern.ch
 ***************************************************************************/


#ifndef InDetRecToolInterfaces_IRoISeedTool_H
#define InDetRecToolInterfaces_IRoISeedTool_H

#include "GaudiKernel/AlgTool.h"
#include "EventPrimitives/EventPrimitives.h" 

namespace InDet
{
  static const InterfaceID IID_IRoISeedTool
    ("InDet::IRoISeedTool", 1, 0);
  class IRoISeedTool : virtual public IAlgTool
  {
  public:

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

  public:
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////
    
    static const InterfaceID& interfaceID();
    virtual StatusCode initialize ()=0;
    virtual StatusCode finalize   ()=0;

    /*
      Call to get all measurements

    */
    virtual std::vector<Amg::Vector3D> getRoIs() = 0;

  }; // End of IRoISeedTool class definition 


  inline const InterfaceID& IRoISeedTool::interfaceID()
    {
      return IID_IRoISeedTool;
    }


} // End of namespace InDet
#endif
