/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexSelectionTool.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef TRK_IVertexSelectionTool_H
#define TRK_IVertexSelectionTool_H

#include "GaudiKernel/IAlgTool.h"

class VxContainer;

namespace Trk
{

  static const InterfaceID IID_IVertexSelectionTool("IVertexSelectionTool", 1, 0);

  /**
   @class IVertexSelectionTool
   @brief Interface class for vertex Container Sorting.

   For more detailed information, take a look at the header file of the actual
   implementation files.

  */

  class IVertexSelectionTool : virtual public IAlgTool {

     public:
     
       /** Virtual destructor */
       virtual ~IVertexSelectionTool(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IVertexSelectionTool; };


       /** Selection method returning the position of selected the PV in  the container */
       virtual unsigned int findVertexInContainer(const VxContainer * ) const = 0;


  };//end of class definitions
}//end of namespace definitions

#endif //TRK_IVertexSelectionTool_H
