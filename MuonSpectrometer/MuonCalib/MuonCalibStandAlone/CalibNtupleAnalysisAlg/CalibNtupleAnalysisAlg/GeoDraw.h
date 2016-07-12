/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib__GeoDraw_h
#define MuonCalib__GeoDraw_h

#include "map"

namespace MuonCalib {

class GeoDraw_Data;

class GeoDraw {
 public:
  static void Load(const char *filename);
	
  static double PosX(unsigned int tube_id);
  static double PosY(unsigned int tube_id);
  static double PosZ(unsigned int tube_id);
	
  static double GPosX(unsigned int tube_id);
  static double GPosY(unsigned int tube_id);
  static double GPosZ(unsigned int tube_id);
	
  static double LocalToGlobalX(unsigned int id, double x, double y, double z);
  static double LocalToGlobalY(unsigned int id, double x, double y, double z);
  static double LocalToGlobalZ(unsigned int id, double x, double y, double z);
	
  static double GlobalToLocalX(unsigned int id, double x, double y, double z);
  static double GlobalToLocalY(unsigned int id, double x, double y, double z);
  static double GlobalToLocalZ(unsigned int id, double x, double y, double z);
	
 private:	
  static GeoDraw_Data *s_data;
};

}  //namespace MuonCalib
#endif
