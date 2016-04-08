#include "QatPresenter/PresenterApplication.h"
#include <QtGui/QApplication>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
int main(int argc, char ** argv) {

  char * _cmtpath=getenv("CMTPATH");
  char * _cmtconfig=getenv("CMTCONFIG");

  std::string cmtpath   = _cmtpath   ? _cmtpath:  "";
  std::string cmtconfig = _cmtconfig ? _cmtconfig:"";
  

  QApplication app(argc,argv);

  if (_cmtpath && _cmtconfig) {
    int begin=0;
    while (1) {
      int end=cmtpath.find(":",begin);
      std::string tok = cmtpath.substr(begin,end-begin);
      tok += "/InstallArea/";
      tok += cmtconfig;
      tok += "/lib/qatplugins";
      app.addLibraryPath(tok.c_str());
      begin=end+1;
      if (end==(int) cmtpath.npos) break;
    }
  }
  else {
    app.addLibraryPath("/usr/local/share/qatplugins");
  }

  PresenterApplication presenter;

  // Get Vis Dirs:
  char *visDir=getenv("PRESENTER_VIS_DIR");
  if (visDir) {
    char *thisDir=strtok(visDir,(char *)":");
    while (thisDir) {
      presenter.addVisFunctionDirectory(thisDir);
      thisDir=strtok(NULL, (char *) ":");
    }
  }
  else {
    presenter.addVisFunctionDirectory("/usr/local/share/");
  }

  presenter.readVisFunctions();

  if (argc==1) {
  }
  else if ((argc==2) && std::string(argv[1]).find(".pres")!=std::string::npos) {
    presenter.load(argv[1]);
  }
  else {
    for (int c=1;c<argc;c++) {
      presenter.open(argv[c]);
    }
  }
  presenter.show();
  return app.exec();
}
