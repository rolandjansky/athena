#ifndef AFP_LOCRECO_IAFPSIDLOCRECOTOOL_H
#define AFP_LOCRECO_IAFPSIDLOCRECOTOOL_H 1

/// @file   IAFPSiDLocRecoTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Header file for interface IAFPSiDLocRecoTool


// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

static const InterfaceID IID_IAFPSiDLocRecoTool("IAFPSiDLocRecoTool", 1, 0);

/// Interface for AFP tools that reconstruct tracks from silicon detector hits
class IAFPSiDLocRecoTool : virtual public ::IAlgTool
{ 

 public: 
  /// Empty destructor
  virtual ~IAFPSiDLocRecoTool() {}

  static const InterfaceID& interfaceID();

  /// @brief run tracks reconstruction
  /// 
  /// The method that does the actual tracks reconstruction. It reads
  /// silicon detector hits from StoreGate, reconstructs tracks and
  /// saves result to StoreGate.
  virtual StatusCode reconstructTracks() = 0;

}; 

inline const InterfaceID& IAFPSiDLocRecoTool::interfaceID() 
{ 
   return IID_IAFPSiDLocRecoTool; 
}


#endif //> !AFP_LOCRECO_IAFPSiDLOCRECOTOOL_H
