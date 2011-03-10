/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include <string>
#include <algorithm>

class TObject;
class TCanvas;
class TLine;
class TArc;
class TBox;
class TPaveText;


/**
   @class Display
   Display holds 'shapes' depicting driftcircles (from MdtCalibHitBase) and lines (from MuonCalibPattern s, MuonCalibSegment s and MuonCalibTruth s). The projection of these objects on the TCanvas can be set for each shapes using the ShapeCreator.

   @author Niels.Van.Eldik@cern.ch
*/

class Display { 

 public:
  Display(const char* name = "display",
	  double xll=-1500,double yll=500,
	  double xrh=1500,double yrh=3500, int size=600); //!< default constructor setting the size of the canvas
  ~Display();                                             //!< destructor
  
  void addLine(TLine* line) { m_lines.push_back(line); }       //!< add a line-shape to the display
  void addArc(TArc* arc) { m_arcs.push_back(arc); }            //!< add an arc-shape to the display
  void addBox(TBox* box) { m_boxes.push_back(box); }           //!< add a box-shape to the display
  void addGeometry(TObject* ob) { m_geometry.push_back(ob); }  //!< add an object to the display
  void clear();                                                //!< clears the set of shapes 
  void range(double xll,double yll,double xrh,double yrh) { m_xll = xll; m_yll = yll; m_xrh = xrh; m_yrh = yrh; }  //!< sets the size of the canvas
  void reSize(double xll,double yll,double xrh,double yrh) { range(xll,yll,xrh,yrh); draw(); }   //!< sets the size of the canvas and draw
  void draw();                                                 //!< draws canvas with all shapes
  void print(std::string name);                                //!< create a PS file of the display
  void setSize(int size);                                      //!< sets the size
  void setTextBox(std::string text);                           //!< clears textbox and fills text to a textbox
  void update();                                               //!< updates canvas
  TCanvas* canvas();                                           //!< returns canvas
  
   private:
  void calculateCanvasSize(int &width, int& height);           //!< sets the width and height of the canvas, scaled to the largest number
  void calculateRange();                                       //!< calculates size of the canvas and scaling according to the shapes

  double m_xll;                        //!< x-coordinate left-lower corner     
  double m_yll;                        //!< y-coordinate left-lower corner     
  double m_xrh;                        //!< x-coordinate right-upper corner     
  double m_yrh;                        //!< y-coordinate right-upper corner     
  std::vector<TLine*> m_lines;         //!< set of line shapes
  std::vector<TArc*>  m_arcs;          //!< set of arc shapes
  std::vector<TBox*>  m_boxes;         //!< set of box shapes
  std::vector<TObject*> m_geometry;    //!< set of general shapes
  std::string m_name;                  //!< canvas name
  int m_size;                          //!< size of the canvas
  TCanvas* m_canvas;                   //!< canvas
  TPaveText* m_textBox;                //!< optional textbox for comment on the canvas
 };

#endif

#ifndef SHAPECREATOR_H
#define SHAPECREATOR_H

class TLine;
class TArc;
class TBox;
class TVector3;

/**
   @class ShapeCreator
   This class creates shapes to be displayed in the Display. The shapes are created from points/directions/radii etc. 
   
   One can choose a projection as well:
   - 0 = X-Y projection
   - 1 = X-Z projection
   - 2 = Y-Z projection
   - 3 = Z-Y projection
   - 4 = Z-R projection with R = sqrt(X^2 + Y^2) 
   
*/
class ShapeCreator {
 public:
  ShapeCreator() : m_projection(2) {}                                         //!< default constructor setting projection to Y-Z
    void   setProjection(int p);                                              //!< sets projection: 
    
    //!<   - 0 = X-Y projection
    
    //!<   - 1 = X-Z projection
    
    //!<   - 2 = Y-Z projection
    
    //!<   - 3 = Z-Y projection

    //!<   - 4 = Z-R projection with R = sqrt(X^2 + Y^2) 
    TLine* createLine(const TVector3& pos, const TVector3& dir, double length, bool half = false); //!< creates a line with position 'pos', direction 'dir' and length of 'length'
    TArc*  createArc(const TVector3& pos, double r);                            //!< creates an arc with position 'pos' and radius 'r', used as a driftcircle
    TBox*  createBox(const TVector3& upperleft, const TVector3& lowerright);    //!< creates a box defined by its upperleft and lowerright coordinate
    TArc*  createTube(const TVector3& pos) ;                                    //!< creates an arc with position 'pos', radius = 7 mm and a nice turquooise color. Used as a MDT
    
 private:
    void   applyProjection(const TVector3& pos, double& x1, double& x2);        //!< Projects pos into the chosen projection, storing the resulting x1 and x2 coordinates.
    int m_projection;                                                           //!< projection the shape will be displayed in
};




#endif


