//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: QGSP_BERT_Msc92.hh,v 1.1 2006-10-31 11:35:09 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   QGSP_BERT_Msc92
//
// Author:      V.Ivanchenko 01.03.2013 for Geant4 9.4p04
//
// Modified:
//
//----------------------------------------------------------------------------
//
#ifndef TQGSP_BERT_Msc92_h
#define TQGSP_BERT_Msc92_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"
#include "CompileTimeConstraints.hh"

template<class T>
class TQGSP_BERT_Msc92: public T
{
public:
  TQGSP_BERT_Msc92(G4int ver = 1);
  virtual ~TQGSP_BERT_Msc92();
  
public:
  // SetCuts() 
  virtual void SetCuts();

private:
  enum {ok = CompileTimeConstraints::IsA<T, G4VModularPhysicsList>::ok };
};

#include "QGSP_BERT_Msc92.icc"
typedef TQGSP_BERT_Msc92<G4VModularPhysicsList> QGSP_BERT_Msc92;

#endif



