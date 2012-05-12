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


#include "QatPlotting/VisFunction.h"
#include <vector>
#include <iostream>
class VisFunction::Clockwork {
public:
  std::string                                 name;
  std::vector<Genfun::Parameter *>            parameter; 
  std::vector<const Genfun::AbsFunction *>    function;
  std::vector<const Cut<double> *>            domainRestriction;
  QRectF                                      rectHint;
};

VisFunction::VisFunction(const std::string & name):
  c(new Clockwork()) 
{
  c->name = name;
}

VisFunction::~VisFunction() 
{
  for (unsigned int i=0;i<c->function.size();i++) {
    delete c->function[i];
    delete c->domainRestriction[i];
  }
  for (unsigned int i=0;i<c->parameter.size();i++) {
    delete c->parameter[i];
  }
  delete c;
}


void VisFunction::addParameter(Genfun::Parameter *p) {
  c->parameter.push_back(p);
}

unsigned int VisFunction::getNumParameters() const {
  return c->parameter.size();
}

Genfun::Parameter *VisFunction::getParameter(unsigned int i) const {
  return c->parameter[i];
}


void VisFunction::addFunction(Genfun::GENFUNCTION f, const Cut<double> * domainRestriction) {
  c->function.push_back(f.clone());
  c->domainRestriction.push_back(domainRestriction ? domainRestriction->clone(): NULL);
}

unsigned int VisFunction::getNumFunctions() const {
  return c->function.size();
}

const Genfun::AbsFunction *VisFunction::getFunction(unsigned int i) const {
    return c->function[i];
}

const Cut<double> *VisFunction::getDomainRestriction(unsigned int i) const {
    return c->domainRestriction[i];
}


const std::string & VisFunction::name() const {
  return c->name;
}


const QRectF & VisFunction::rectHint() const {
  return c->rectHint;
}


QRectF & VisFunction::rectHint() {
  return c->rectHint;
}
