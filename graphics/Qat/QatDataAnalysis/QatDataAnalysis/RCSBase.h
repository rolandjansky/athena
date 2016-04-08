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


// Base Class for Reference Counted Squeezable objects.  The object
// uncaches all data when the reference count returns to one.  The 
// object is deleted when the reference count goes to zero.  
//
// The idea is that squeezing takes place when no one is looking except
// the manager...



#ifndef RCSBase_h
#define RCSBase_h 1
class RCSBase 
{

  public:

      RCSBase();


      void ref() const;

      void unref() const;

      unsigned int refCount() const;

      virtual void uncache() const;

  protected:

      virtual ~RCSBase();

  private:

      RCSBase(const RCSBase &right);

      const RCSBase & operator=(const RCSBase &right);

      mutable unsigned short int _count;



};






#endif


