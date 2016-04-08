//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#ifndef  Attribute_h_
#define  Attribute_h_
#include <typeinfo>
#include <string>
// Attribute:  name and type----------------------------------------------//
class Attribute {                                                         //
  //                                                                      //
 public:                                                                  //
                                                                          //
  enum Type {                                                             //
    DOUBLE,                                                               //
    FLOAT,                                                                //
    INT,                                                                  //
    UINT,                                                                 //
    UNKNOWN};                                                             //
  //                                                                      //
  //                                                                      //
  // Constructors:                                                        //
  inline Attribute(const std::string & name, const std::type_info & type);//
  inline Attribute(const std::string & name);                             //
  //                                                                      //
  // Name:                                                                //
  inline std::string & name();                                            //
  inline const std::string & name() const;                                //
  //                                                                      //
  // Type Name                                                            //
  inline std::string typeName() const;                                    //
  //                                                                      //
  // Type                                                                 //
  inline const Type & type() const;                                       //
  //                                                                      //
  // Attribute Id:                                                        //
  inline int & attrId();                                                  //
  inline unsigned int  attrId() const;                                    //
  //                                                                      //
  // Relational operators (lexicographical)                               //
  inline bool operator < (const Attribute & a) const;                     //
  //                                                                      //
  // Equality operator (lexicographical)                                  //
  inline bool operator== (const Attribute & a) const;                     //
  //                                                                      //
  //                                                                      //
  class NameEquals;                                                       //
  //                                                                      //
  //                                                                      //
  inline static Type typeOf(const std::type_info & type);                 //
  inline static std::string typenameOf(const Type type);                  //
  //                                                                      //
 private:                                                                 //
  //                                                                      //
  //                                                                      //
  std::string    _name;                                                   //
  Type           _type;                                                   //
  int            _attrId;                                                 //
};                                                                        //
                                                                          //
//------------------------------------------------------------------------//

#include "QatDataAnalysis/Attribute.icc"


#endif
