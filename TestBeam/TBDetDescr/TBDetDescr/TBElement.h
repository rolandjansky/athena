/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//==============================================
//  File TBElement.h
//  define the position of one detector 
//=============================================
#ifndef TBElement_H
#define TBElement_H

//#include <CLHEP/Geometry/Vector3D.h>
//#include <CLHEP/Vector/Rotation.h>
#include "GeoPrimitives/GeoPrimitives.h"
#include "TBDetDescr/TBElementID.h"
#include <string>


class TBElement 
{ 
  friend class TBDetDescrManager;

public: 

  TBElement();
  TBElement ( TBElementID::TBElementID id, std::string name,
              Amg::Vector3D position, Amg::RotationMatrix3D rotation );

  virtual ~TBElement() {}

  // accessor  
  TBElementID::TBElementID  id()        const  { return m_id; }
  std::string               name()      const  { return m_name; }
  Amg::Vector3D             position()  const  { return m_position; }
  Amg::RotationMatrix3D     rotation()  const  { return m_rotation; }
  
  // Convertion operator to a std::string
  // Can be used in a cast operation : (std::string) TBElement
  virtual operator std::string() const;

private:

  TBElementID::TBElementID m_id;
  std::string m_name;
  Amg::Vector3D m_position;
  Amg::RotationMatrix3D m_rotation;

}; 
 
#endif
