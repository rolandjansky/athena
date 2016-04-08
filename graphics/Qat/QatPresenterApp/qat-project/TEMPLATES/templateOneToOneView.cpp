#include "QatDataAnalysis/OptParse.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/PlotHist1DDialog.h"
#include "QatPlotting/PlotStream.h"
#include <QApplication>
#include <QtGui/QMainWindow>
#include <QToolBar>
#include <QtGui/QAction>
#include <cstdlib>
#include <iostream>
#include <string>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + " -i Input -o Output [-v]"; 
  if (argc==1) {
    std::cout << usage << std::endl;
    exit(0);
  }

  HIOOneToOne hio;
  try {
    hio.optParse(argc,argv);
  }
  catch (std::exception & e) {
    std::cerr << "Error " << e.what()  << std::endl;
    std::cerr << usage    << std::endl << std::endl;
    exit(0);
  }
  if (argc!=1) {      
    std::cerr << "Unrecognized option: " << argv[1] <<std::endl;
    exit(0);
  }

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  QAction  *editAction=toolBar->addAction("Edit");
  PlotHist1DDialog *editorDialog=new PlotHist1DDialog(NULL);
  
  nextAction->setShortcut(QKeySequence("n"));
  editAction->setShortcut(QKeySequence("e"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  QObject::connect(editAction, SIGNAL(triggered()), editorDialog, SLOT(show()));
  
  QRectF rect;
  rect.setLeft(0.0);
  rect.setRight(1.0);
  rect.setTop(0.0);
  rect.setBottom(1.0);
  

  PlotView view(&rect);
  window.setCentralWidget(&view);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}

