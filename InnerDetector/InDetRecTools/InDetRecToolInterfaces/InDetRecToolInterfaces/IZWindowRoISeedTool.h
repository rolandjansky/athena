/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for Z-window(s) RoI tool
/////////////////////////////////////////////////////////////////////////////////

#ifndef InDetRecToolInterfaces_IZWindowRoISeedTool_H
#define InDetRecToolInterfaces_IZWindowRoISeedTool_H

#include "GaudiKernel/AlgTool.h"
#include "EventPrimitives/EventPrimitives.h" 

namespace InDet
{
  static const InterfaceID IID_IZWindowRoISeedTool
    ("InDet::IZWindowRoISeedTool", 1, 0);
  class IZWindowRoISeedTool : virtual public IAlgTool
  {
  public:

    ///////////////////////////////////////////////////////////////////
    // Public return type
    ///////////////////////////////////////////////////////////////////
    class ZWindow {
    public:
      //* Lower and Upper z bound of the window */
      float z_window[2];
      //* Reference z-position (if any) */
      float z_reference;
      //* The perigee z-positions of the tracks (if any) */
      float z_perigee_pos[2];

      //* Constructor setting default values */
      ZWindow() { 
	z_reference = -999.;
	z_window[0]=z_window[1]=-999.; 
	z_perigee_pos[0]=z_perigee_pos[1]=-999.; 
      }

      ~ZWindow() {};
    };

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    /*
      Compute RoI z-window(s)
    */
    virtual std::vector<ZWindow> getRoIs() = 0;

  public:
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////
    
    static const InterfaceID& interfaceID();
    virtual StatusCode initialize ()=0;
    virtual StatusCode finalize   ()=0;

  }; // End of IZWindowRoISeedTool class definition 


  inline const InterfaceID& IZWindowRoISeedTool::interfaceID()
    {
      return IID_IZWindowRoISeedTool;
    }


} // End of namespace InDet
#endif
