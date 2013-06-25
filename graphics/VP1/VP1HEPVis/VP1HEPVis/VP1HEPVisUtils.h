/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1HEPVisUtils                      //
//                                                            //
//  Description: Utilities connected to the VP1HEPVis         //
//               classes.                                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1HEPVISUTILS_H
#define VP1HEPVISUTILS_H

class SoGroup;
class SoNode;

class VP1HEPVisUtils {
public:

  static void initAllCustomClasses();//calls ::initClass() on all custom classes.

  static SoGroup* convertToStandardScene(SoGroup*);
  static bool isCustomNode(SoNode*);
  static SoNode * convertCustomNodeToAlternateRep(SoNode*);//Returns empty group node if not custom or without alternate rep.

  static void updateAllAlternativeReps(SoGroup*);
  static void clearAllAlternativeReps(SoGroup*);

private:

  VP1HEPVisUtils();
  ~VP1HEPVisUtils();

  class Imp;
};

#endif
