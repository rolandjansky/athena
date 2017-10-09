/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1UtilitySystems/VP1PartSpectSystem.h"
#include "VP1UtilitySystems/VP1PartSpectController.h"

#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Serialise.h"

/* --- FIXME: Qat has still to be ported to Qt5 I think...
// Plotting
//#include "QatPlotting/PlotStream.h"
//#include "QatPlotWidgets/PlotView.h"
//#include "QatDataAnalysis/Hist1D.h"
//#include "QatPlotting/PlotHist1D.h"
 */

#include <QMainWindow>
#include <QToolBar>
#include <QAction>

#include "TFile.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TKey.h"
#include <sstream>


class VP1PartSpectSystem::Imp {
public:
  Imp();
  ~Imp();

  VP1PartSpectController* controller;
  QMainWindow* PlotMainWindow;
  TFile* stream;

  QString histogramPrefix(VP1PartSpect::ParticleType);
};

VP1PartSpectSystem::Imp::Imp()
  :controller(0)
  ,PlotMainWindow(0)
  ,stream(0)
{
}

VP1PartSpectSystem::Imp::~Imp()
{
  delete PlotMainWindow;
  delete stream;
}

QString VP1PartSpectSystem::Imp::histogramPrefix(VP1PartSpect::ParticleType parttype)
{
  switch(parttype){
  case VP1PartSpect::Baryon :
    return QString("baryon_");
  case VP1PartSpect::E :
    return QString("e_");
  case VP1PartSpect::Gamma :
    return QString("gamma_");
  case VP1PartSpect::Lepton :
    return QString("lepton_");
  case VP1PartSpect::Meson :
    return QString("meson_");
  case VP1PartSpect::Neutron :
    return QString("neutron_");
  case VP1PartSpect::Nucleus :
    return QString("nucleus_");
  case VP1PartSpect::Pi :
    return QString("pi_");
  case VP1PartSpect::Proton :
    return QString("proton_");
  default:
    return QString("");
  }
}

VP1PartSpectSystem::VP1PartSpectSystem()
  : IVP13DSystemSimple("PartSpectrum"
		       ,"System for displaying particle spectrum histograms"
		       ,"Vakho Tsulaia <tsulaia@mail.cern.ch>")
  , d(new Imp())
{
}

VP1PartSpectSystem::~VP1PartSpectSystem()
{
  delete d;
}

QWidget* VP1PartSpectSystem::buildController()
{
  // Standard stuff
  d->controller = new VP1PartSpectController(this);

  // Connect controller signals to various slots
  connect(d->controller,SIGNAL(fileUpdated(QString)),this,SLOT( fileUpdated(QString)));

  connect(d->controller,SIGNAL(particleTypeChanged(VP1PartSpect::ParticleType)),this,SLOT(particleType()));

  return d->controller;
}

void VP1PartSpectSystem::buildPermanentSceneGraph(StoreGateSvc*, SoSeparator*)
{
}
 
void VP1PartSpectSystem::buildEventSceneGraph(StoreGateSvc*, SoSeparator*)
{
}

QByteArray VP1PartSpectSystem::saveState()
{
  VP1Serialise serialise(0/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  ensureBuildController();

  serialise.save(d->controller->saveSettings());

  serialise.disableUnsavedChecks();
  return serialise.result();
}

void VP1PartSpectSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);

  if (state.version()!=0) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }

  ensureBuildController();

  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  d->controller->restoreSettings(state.restoreByteArray());

  state.disableUnrestoredChecks();
}

void VP1PartSpectSystem::plotSpectrum(QStack<QString>& path, int copyNumber)
{
  // close plot main window if already open
  delete  d->PlotMainWindow;
  d->PlotMainWindow = 0;

  // Do something only if the system is active
  if(activeState()==IVP1System::OFF) return;

  if(!d->stream) {
    message("No input file");
    return;
  }

  d->stream->cd();
  std::ostringstream copyNumberStream;
  copyNumberStream << copyNumber;

  while(path.size()>1){
    QString dirBase= path.pop().replace(QString("::"),QString("_"));
    QString directory=dirBase;
    if(!gDirectory->cd(directory.toLatin1())) {
      directory = (dirBase.toStdString()+"_"+copyNumberStream.str()).c_str();
      if (!gDirectory->cd(directory.toLatin1())) {
	directory = (dirBase.toStdString()+"_0").c_str();
	if (!gDirectory->cd(directory.toLatin1())) {
	  message(QString("Unable to cd to ") + directory);
	  return;
	} else {
	  messageDebug(QString("Entered ") + directory);
	}
      }
    }
  }
  
  QString histogramName = d->histogramPrefix(d->controller->getParticleType());
  QString entredHistoName = histogramName + "entred_";
  QString entredHistoName_n = histogramName + "entred_";
  QString entredHistoName_0 = histogramName + "entred_";
  QString madeinHistoName = histogramName + "madein_";

  QString histoNameBody   = path.pop().replace(QString("::"),QString("_"))+QString("_hist");
  QString histoNameBody_n = histoNameBody+QString("_") + QString(copyNumberStream.str().c_str())+QString("_hist");

  QString histoNameBody_0 = histoNameBody+QString("_0_hist");
  
  histogramName   += histoNameBody;
  

  entredHistoName   += histoNameBody;
  entredHistoName_n += histoNameBody_n;
  entredHistoName_0 += histoNameBody_0;
  
  TObject* entredObject = gROOT->FindObject(entredHistoName.toStdString().c_str());
  if (!entredObject) {
    entredObject = gROOT->FindObject(entredHistoName_n.toStdString().c_str());
    if (!entredObject) {
      entredObject = gROOT->FindObject(entredHistoName_0.toStdString().c_str());
      if (!entredObject) {
	message(QString("Unable to find histogram ") + entredHistoName);
      }
      else {
	madeinHistoName += histoNameBody_0;
      }
    }
    else {
      madeinHistoName += histoNameBody_n;
    }
  }
  else {
    madeinHistoName += histoNameBody;
  }
  
  
  
  TH1F* tHist1F1 = 0;
  TH1F* tHist1F2 = 0;

  
  if(entredObject) {
    tHist1F1 = dynamic_cast<TH1F*> (entredObject);
  }
  
  
  TObject* madeinObject = gROOT->FindObject(madeinHistoName.toStdString().c_str());
  if(!madeinObject) {
    message(QString("Unable to find histogram ") + madeinHistoName);
  } else {
    if(tHist1F1){
      tHist1F2 = dynamic_cast<TH1F*> (madeinObject);
    } else {
      tHist1F1 = dynamic_cast<TH1F*> (madeinObject);
    }
  }
  
  if((!tHist1F1) 
     && (!tHist1F2)) {
    message("Unable to find either entred or madein histograms");
    return;
  }

  /* FIXME: Qat has to be ported to Qt5...
  // ___________ Plotting _______________

  // Create new window for the plot
  d->PlotMainWindow = new QMainWindow();
  QToolBar *toolBar=d->PlotMainWindow->addToolBar("Tools");
  QAction  *dismissAction=toolBar->addAction("Dismiss");
  dismissAction->setShortcut(QKeySequence("d"));
  connect(dismissAction,SIGNAL(activated()), d->PlotMainWindow, SLOT(hide()));

  // Create histogram
  Hist1D* hist1D = new Hist1D(histogramName.toStdString()
			      ,tHist1F1->GetXaxis()->GetNbins()
			      ,tHist1F1->GetXaxis()->GetXmin()
			      ,tHist1F1->GetXaxis()->GetXmax());
  for(int ii=0; ii<tHist1F1->GetXaxis()->GetNbins(); ++ii)
    hist1D->accumulate(tHist1F1->GetBinCenter(ii),tHist1F2!=0?tHist1F1->GetBinContent(ii)+tHist1F2->GetBinContent(ii):tHist1F1->GetBinContent(ii));

  PlotHist1D* pHist = new PlotHist1D(*hist1D);
  PlotHist1D::Properties* prop = new PlotHist1D::Properties();
  prop->pen.setColor(QColor("darkRed"));
  prop->pen.setWidth(3);
  pHist->setProperties(*prop); //const reference, but does not take possession of pointer
    
  // Make a plot
  QRectF rect = pHist->rectHint();
  rect.setTop(1.0);
  rect.setLeft(1.0);
  PlotView* view = new PlotView(&rect);
  view->setLogX(true);
  view->setLogY(true);

  d->PlotMainWindow->setCentralWidget(view);
  view->setBox(false);
  view->add(pHist);

  // Construct three label streams
  PlotStream titleStream(view->titleTextEdit()), xLabelStream(view->xLabelTextEdit()), yLabelStream(view->yLabelTextEdit());
      
  // Label the title
  titleStream
    << PlotStream::Clear() 
    << PlotStream::Center() 
    << PlotStream::Family("Sans Serif")  
    << PlotStream::Size(12)
    << PlotStream::Color(QColor("red")) 
    << histogramName.toStdString().c_str()
    << PlotStream::EndP();
  
  // Label the x-axis
  xLabelStream 
    << PlotStream::Clear() 
    << PlotStream::Center() 
    << PlotStream::Family("Sans Serif") 
    << PlotStream::Size(12) 
    << "energy [MeV/c" 
    << PlotStream::Super()  
    << "2"
    << PlotStream::Normal()
    << "]"
    << PlotStream::EndP();
  
  // Label the y-axis
  yLabelStream 
    << PlotStream::Clear() 
    << PlotStream::Center() 
    << PlotStream::Family("Sans Serif")  
    << PlotStream::Size(12)
    << (d->controller->getParticleType()==VP1PartSpect::Neutron ? "neutrons / bin" : "electrons / bin")
    << PlotStream::EndP();

  // Show this view

  d->PlotMainWindow->show();
  delete prop;
  prop=nullptr;
  // ___________ Plotting _______________
  */




}

void VP1PartSpectSystem::fileUpdated(const QString& fileName)
{
  // Close previously open file
  delete d->stream;
  d->stream=0;

  // Open the new file
  d->stream = new TFile(fileName.toStdString().c_str(),"READ");
  messageVerbose(fileName + " opened");
}

void VP1PartSpectSystem::particleType()
{
  // To do
}
