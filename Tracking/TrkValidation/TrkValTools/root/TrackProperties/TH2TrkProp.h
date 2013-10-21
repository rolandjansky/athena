// $:$Id: TH2TrkProp.h, v 1.0  04/17/2006 17:07:07 
// Author: Miguel Angel Olivo Gomez   04/17/06

/*************************************************************************
 *                                                                       *
 * Copyright (C) 2006                                    .               *
 * All rights reserved.                                                  *
 *                                                                       *
 *************************************************************************/



class TH2TrkProp {

 public:
  
  TH2TrkProp(const char* format, const char* rootFile, const char* dir, const char* ntuple,
	     const char* xvariable, const char* xvariableName, const char* xunit,
	     const char* yvariable, const char* yvariableName, const char* yunit,
	     const char* cut, const char* cutName, const char* title, 
	     int nbinsx, double xlow, double xup,
	     int nbinsy, double ylow, double yup,
	     bool logx, bool logy
	     );


  ~TH2TrkProp();

  
 private:

  void setStyle();

};
