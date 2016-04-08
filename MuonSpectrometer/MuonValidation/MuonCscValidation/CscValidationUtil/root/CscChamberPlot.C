/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscChamberPlot.C

#include "CscChamberPlot.h"

namespace {

string station_name(int istation) {
  if ( istation == 1 ) return "CSS";
  else if ( istation == 2 ) return "CSL";
  return "UNKNOWN_STATION";
}

string measphi_name(bool measphi) {
  if ( measphi ) return "phi";
  else return "eta";
}

}  // end unnamed namespace
 
//**********************************************************************

CscChamberPlot::
CscChamberPlot(int istation, bool measphi, int eta, int phisec, int run, int evt)
: m_verbose(5), m_pcan(0), m_station(istation), m_measphi(measphi),
  m_eta(eta>=0), m_phisec(phisec), m_run(run), m_evt(evt), m_sector(eta*(2*phisec-istation+1)) { }

CscChamberPlot::~CscChamberPlot() {
  if ( m_verbose > 3 ) {
    cout << "Deleting canvas ";
    if ( m_pcan ) cout << m_pcan->GetName() << " ";
    cout << "at " << m_pcan << "..." << endl;
  }
  delete m_pcan;
  if ( m_verbose > 3 ) cout << "Canvas deleted." << m_pcan << endl;
}

double CscChamberPlot::pitch() const {
  if ( m_station == 1 ) {
    if ( m_measphi ) return 12.92;
    return 5.60;
  } else if ( m_station == 2 ) {
    if ( m_measphi ) return 21.0;
    else return 5.31;
  }
  return 0;
}

double CscChamberPlot::strip_pos(double pos) const {
  double spos = pos/pitch() + 0.5*maxchan() + 1.0; 
  if ( m_eta > 0 && m_measphi ) {
    spos = -pos/pitch() + 0.5*maxchan() + 1.0;
  }
  return spos;
}

double CscChamberPlot::xwpos(int wlay) const {
  if ( wlay == 1 ) return -36.1;
  else if ( wlay == 2 ) return -12.1;
  else if ( wlay == 3 ) return 12.1;
  else if ( wlay == 4 ) return 36.1;
  return 0.0;
}

double CscChamberPlot::dx_qmax() const {
  double x14 = xwpos(4) - xwpos(1);
  double qxrat = 0.20;
  return qxrat*x14;
}

string CscChamberPlot::chamber_name() const {
  ostringstream sstitle;
  sstitle << station_name(m_station);
  if ( m_eta ) sstitle << "+";
  else sstitle << "-";
  sstitle << m_phisec;
  sstitle << measphi_name(m_measphi);
  sstitle << " sector:" << m_sector;
  return sstitle.str();
}

string CscChamberPlot::title() const {
  ostringstream sstitle;
  if ( m_run > 0 ) sstitle << "Run " << m_run << " ";
  if ( m_evt > 0 ) sstitle << "Event " << m_evt << " ";
  sstitle << "Chamber " + chamber_name();
  return sstitle.str().c_str();
}

string CscChamberPlot::label() const {
  ostringstream sstitle;
  if ( m_run > 0 ) sstitle << "run" << m_run << "_";
  if ( m_evt > 0 ) sstitle << "evt" << m_evt << "_";
  sstitle << "chamber_" + chamber_name();
  return sstitle.str().c_str();
}

int CscChamberPlot::
add_strip(int wlay, int chan, double charge, int icol) {
  if ( wlay < 0 || wlay > 4 ) return 1;
  double y1 = xwpos(wlay);
  // Scale up strip charge in draw_clusters
  double y2 = y1 + 1.0*dx_qmax()*charge/qmax();
  double x1 = chan;
  double x2 = x1 + 1.0;
  TBox* pobj = new TBox(x1, y1, x2, y2);
  pobj->SetFillColor(icol);
  m_objs.Add(pobj);
  return 0;
}

int CscChamberPlot::
add_hit(int wlay, double xpos, int isym, int icol) {
  if ( wlay < 0 || wlay > 4 ) return 1;
  double y1 = xwpos(wlay);
  double x1 = strip_pos(xpos);
  TMarker* pobj = new TMarker(x1, y1, isym);
  pobj->SetMarkerColor(icol);
  pobj->SetMarkerStyle(isym);
  pobj->SetMarkerSize(1);
  m_objs.Add(pobj);
  return 0;
}

// Draw plots.
// X (transverse to planes is the vertical coordinate.
// strip meaurement is in horizontal.
TCanvas* CscChamberPlot::draw() {
  if ( m_verbose > 3 ) cout << "Deleting canvas at " << m_pcan << endl;
  delete m_pcan;
  string name = "can_" + label();
  m_pcan = new TCanvas(name.c_str(), title().c_str(), 800, 500);
  if ( m_verbose > 3 ) cout << "Created canvas at " << m_pcan << endl;
  double x14 = xwpos(4) - xwpos(1);
  double dxlab = 0.06*x14;
  double dtick1 = 0.03*x14;
  double dtick2 = 0.5*dtick1;
  double xmin = xwpos(1);
  double xmax = xwpos(4) + 0.3333*x14;
  double xcanborder = 0.03*x14;
  double ymin = 1.0;
  double ymax = maxchan() + 1.0;
  double ycanborder = 0.03*(ymax-ymin);
  if ( m_verbose > 2 ) cout << ymin << " " << xmin << " " << ymax << " " << xmax << endl;
  m_pcan->Range(ymin-ycanborder, xmin-dtick1-xcanborder,
                ymax+ycanborder, xmax+dxlab+xcanborder);
  for ( int wlay=1; wlay<5; ++wlay ) {
    double x0 = xwpos(wlay);
    TLine* pline = new TLine(ymin, xwpos(wlay), ymax, xwpos(wlay));
    pline->Draw();
    for ( double chan=0.0; chan<ymax; chan+=10.0 ) {
      if ( chan >= ymin && chan <= ymax ) {
        TLine* ptick = new TLine(chan, x0, chan, x0-dtick1 );
        ptick->Draw();
      }
      double chan2 = chan + 5;
      if ( chan2 >= ymin && chan2 <= ymax ) {
        TLine* ptick2 = new TLine(chan2, x0, chan2, x0-dtick2 );
        ptick2->Draw();
      }
    }
    //char* opt = "-U";
    //if ( wlay == 1 ) opt = "-U";
    //int tick = m_measphi ? 205 : 219;
    //TGaxis* pax =
    //  new TGaxis(ymin, xwpos(wlay), ymax, xwpos(wlay), 0, ymax, tick, opt);
    //pax->Draw();
  }
  if ( m_verbose > 2 ) cout << "Drawing " << m_objs.GetEntries() << " objects" << endl;
  for ( int iobj=0; iobj<m_objs.GetEntries(); ++iobj ) {
    TObject* pobj = m_objs[iobj];
    //cout << "Drawing " << pobj->Print() << endl;
    pobj->Draw("same");
  }
  TPaveLabel* plab = new TPaveLabel(ymin, xmax, 0.4*ymax, xmax+dxlab, title().c_str());
  plab->Draw();
  m_pcan->Update();
  return m_pcan;
}

//**********************************************************************

namespace {
  CscDetectorPlot* pcscdetplot = 0;
}

CscDetectorPlot* CscDetectorPlot::global() {
  if ( pcscdetplot == 0 ) {
    pcscdetplot = new CscDetectorPlot;
  }
  return pcscdetplot;
}

CscDetectorPlot* CscDetectorPlot::new_global(int run, int evt) {
  delete pcscdetplot;
  pcscdetplot = new CscDetectorPlot(run, evt);
  return pcscdetplot;
}

int CscDetectorPlot::plane_index() const {
  int idx = 0;
  if ( m_station == 2 ) idx += 32;
  if ( m_measphi ) idx += 16;
  if ( m_eta > 0 ) idx += 8;
  idx += m_phisec;
  if ( idx < 0 || idx > 63 ) {
    cerr << "Invalid plane index: " << idx << endl;
    idx = 0;
  }
  return idx;
}

CscDetectorPlot::CscDetectorPlot(int run, int evt)
: m_run(run), m_evt(evt) {
  for ( int is=0; is<2; ++is ) {
    for ( int im=0; im<2; ++im ) {
      for ( int ie=0; ie<2; ++ie ) {
        for ( int ip=0; ip<8; ++ip ) {
          m_pplots[is][im][ie][ip] = 0;
        }
      }
    }
  }
}

CscDetectorPlot::~CscDetectorPlot() {
  clear();
}

CscChamberPlot* CscDetectorPlot::
get(int istation, bool measphi, int eta, int phisec) {
  int is = istation -1;
  if ( is < 0 || is > 1 ) return 0;
  int im = measphi;
  int ie = 1;
  if ( eta < 0 ) ie = 0;
  int ip = phisec - 1;
  if ( ip < 0 || ip > 7 ) return 0;
  if ( m_pplots[is][im][ie][ip] == 0 ) {
    m_pplots[is][im][ie][ip] =
      new CscChamberPlot(istation, measphi, eta, phisec, m_run, m_evt);
  }
  CscChamberPlot* ppcha = m_pplots[is][im][ie][ip];
  //cout << "Returning  " << ppcha->title() << " with "
  //     << ppcha->size() << " entries" << endl;
  //cout << ppcha << endl;
  return ppcha;
}

int CscDetectorPlot::draw() {
  int ndraw = 0;
  for ( int is=0; is<2; ++is ) {
    for ( int im=1; im>=0; --im ) {
      for ( int ie=0; ie<2; ++ie ) {
        for ( int ip=0; ip<8; ++ip ) {
          CscChamberPlot* ppcha = m_pplots[is][im][ie][ip];
          if ( ppcha != 0 ) {
            ++ndraw;
            ppcha->draw();
          }
        }
      }
    }
  }
  return ndraw;
}

int CscDetectorPlot::clear() {
  int ndraw = 0;
  for ( int is=0; is<2; ++is ) {
    for ( int im=0; im<2; ++im ) {
      for ( int ie=0; ie<2; ++ie ) {
        for ( int ip=0; ip<8; ++ip ) {
          CscChamberPlot* ppcha = m_pplots[is][im][ie][ip];
          if ( ppcha != 0 ) {
            delete ppcha;
            m_pplots[is][im][ie][ip] = 0;
          }
        }
      }
    }
  }
  return ndraw;
}

  
//**********************************************************************
