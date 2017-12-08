/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ConstantFieldHandler_H
#define ConstantFieldHandler_H
#include <string>

#include "FadsField/FieldManager.h"
#include "G4Field/ConstantField.h"

class ConstantFieldHandler{
public:
    ConstantFieldHandler();

    static void setFieldValues(std::string fieldName, float Bx, float By, float Bz); 
    static void setXField(std::string fieldName, double xmin, double xmax);
    static void setYField(std::string fieldName, double ymin, double ymax);
    static void setZField(std::string fieldName, double zmin, double zmax);
};
#endif
