/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

Bool_t AtlasStyle = false;
//
struct xCanvas{
  float xSize;
  float ySize;
};

xCanvas Can22;
xCanvas Can32;

//
void load_file_configuration(int);
void load_CanvasTypes();

void define_file_config0();
void define_file_config1();
void define_file_config2();
void define_file_config3();
void define_file_config4();
void define_file_config5();
void define_file_config6();
void define_file_config7();
void define_file_config8();
void define_file_config9();
void define_file_config10();
void define_file_config11();
void define_file_config12();
void define_file_config13();
void define_file_config14();
void define_file_config15();
void DrawMe(char *htodraw, char *xopt = " ");
void DrawMeProf(char *htodraw, char *xopt = " ");
