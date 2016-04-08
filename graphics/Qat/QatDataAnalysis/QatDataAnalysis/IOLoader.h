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


#ifndef _IOLoader_h_
#define _IOLoader_h_

#include <string>
class  IODriver;

class IOLoader {

 public:

  //  Create:
  IOLoader();

  // Destroy:
  ~IOLoader();

  // Get a pointer to the IO driver:
  const IODriver *ioDriver(const std::string & name) const;

 private:

  // Illegal operations:
  IOLoader(const IOLoader & );
  IOLoader & operator = (const IOLoader &);

  // Private member data:
  class Clockwork;
  Clockwork *c;

};

#endif
