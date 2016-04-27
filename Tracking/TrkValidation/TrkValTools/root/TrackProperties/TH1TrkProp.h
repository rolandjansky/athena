// $:$Id: TH1TrkProp.h, v 1.0  04/17/2006 12:07:07 
// Author: Miguel Angel Olivo Gomez   04/17/06

/*************************************************************************
 *                                                                       *
 * Copyright (C) 2006                                    .               *
 * All rights reserved.                                                  *
 *                                                                       *
 *************************************************************************/



class TH1TrkProp {

 public:
  
  TH1TrkProp(const char* format, const char* rootFile, const char* dir, const char* ntuple,
	     const char* xvariable, const char* xvariableName, 
	     const char* xunit, const char* cut, const char* cutName, 
	     const char* title, int nbinsx, double xlow, double xup,
	     bool logy, bool fitGaus
	     );

  ~TH1TrkProp();

  
 private:

  void setStyle(bool fitGauss);

};
