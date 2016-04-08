/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscChamberPlot.h

// Class to draw objects in a CSC chamber.

#ifndef CscChamberPlot_H
#define CscChamberPlot_H

class CscChamberPlot {

public:

  // Constructor.
  // istation = 1 - CSS
  //            2 - CSL
  // eta = -1 or +1
  // phisec = 1, 2, ..., 8
  // If run number is positive, then it appears on the label.
  // If event number is positive, then it appears on the label.
  CscChamberPlot(int istation, bool measphi, int eta=0, int phisec=0,
                 int m_run=-1, int m_evt=-1);

  // Destructor.
  ~CscChamberPlot() {
    delete m_pcan;
  };

  // Return the number of channels in each strip plane.
  int maxchan() const {
    if ( m_measphi ) return 48;
    else return 192;
  }

  // Return the pitch (mm/strip).
  double pitch() const;

  // Convert position to strip coordinate.
  double strip_pos(double pos) const;

  // Return the transverse position of each wire layer.
  double xwpos(int wlay) const;

  // Return the maximum charge on a strip.
  double qmax() const { return 2.6e6; }

  // Distance in the transverse direction that represents the
  // maximum charge on a strip.
  double dx_qmax() const;

  // Title and label.
  string chamber_name() const;
  string title() const;
  string label() const;

  // Number of drawing objects.
  int size() const { return m_objs.GetEntries(); }

  // Add a strip to the plot.
  int add_strip(int wlay, int chan, double charge, int icol =38);

  // Add a truth hit.
  int add_hit(int wlay, double xpos, int sym =24, int icol =30);

  // Draw the plot.
  TCanvas* draw();

private:

  int m_verbose;
  TCanvas* m_pcan;
  int m_station;
  bool m_measphi;
  bool m_eta;       // true for +eta
  int m_phisec;
  int m_sector;
  TObjArray m_objs;  // Objects to be drawn.
  int m_run;   // -1=unknown, -2=multiple
  int m_evt;

};

class CscDetectorPlot {

public:

  // Global detector plot.
  static CscDetectorPlot* global();

  // Define a new global plot. Existing one is deleted.
  static CscDetectorPlot* new_global(int run =-1, int evt =-1);
  
  // Assign a unique index in the range 10, 1,...,64 to the current
  // strip plane.
  static plane_index(int station, bool measphi, int eta, int phisec) const;

public:

  // Constructor.
  // Initializes array with null pointers.
  CscDetectorPlot(int run =-1, int evt =-1);

  // Destructor.
  // Calls clear.
  ~CscDetectorPlot();

  // Return the plot for a given chamber+orientation. 
  CscChamberPlot* get(int istation, bool measphi, int eta, int phisec);

  // Draw all active chambers.
  int draw();

  // Delete all chamber plots.
  int clear();

private:

  int m_run;
  int m_evt;
  CscChamberPlot* m_pplots[2][2][2][8];

};

#endif
