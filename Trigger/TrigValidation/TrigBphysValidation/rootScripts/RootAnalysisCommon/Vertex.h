/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************
// Vertex.h
// Simple representation of a vertex
// Can be created from the n-tuple tree
// made by GetVertex in BPhysAnalysisTools
//*****************************************

class Vertex {
  
private:
  int m_event;
  int m_lumiBlock;
  int m_run;
  int m_vtxType;
  double m_bsX;
  double m_bsY;
  double m_bsZ;
  double m_bsSigX;
  double m_bsSigY;
  double m_bsSigZ;
  double m_vtxCovXX;
  double m_vtxCovXY;
  double m_vtxCovXZ;
  double m_vtxCovYZ;
  double m_vtxCovYY;
  double m_vtxCovZZ;
  double m_vtxX;
  double m_vtxY;
  double m_vtxZ;
  double m_vtxChi2;
  int m_vtxNDF;
  int m_index;

public:
  // Constructor
  Vertex(void) {
    m_event = 0;
    m_lumiBlock = 0;
    m_run = 0;
    m_vtxType = 0;
    m_bsX = 0.0;
    m_bsY = 0.0;
    m_bsZ = 0.0;
    m_bsSigX = 0.0;
    m_bsSigY = 0.0;
    m_bsSigZ = 0.0;
    m_vtxCovXX = 0.0;
    m_vtxCovXY = 0.0;
    m_vtxCovXZ = 0.0;
    m_vtxCovYZ = 0.0;
    m_vtxCovYY = 0.0;
    m_vtxCovZZ = 0.0;
    m_vtxX = 0.0;
    m_vtxY = 0.0;
    m_vtxZ = 0.0;
    m_vtxChi2 = 0.0;
    m_vtxNDF = 0;
    m_index = 0;
  }
  // Set methods
  void setEvent(int input) {m_event = input;}
  void setLumiBlock(int input) {m_lumiBlock = input;}
  void setRun(int input) {m_run = input;}
  void setType(int input) {m_vtxType = input;}
  void setBSX(double input) {m_bsX = input;} 
  void setBSY(double input) {m_bsY = input;} 
  void setBSZ(double input) {m_bsZ = input;} 
  void setBSSigX(double input) {m_bsSigX = input;} 
  void setBSSigY(double input) {m_bsSigY = input;} 
  void setBSSigZ(double input) {m_bsSigZ = input;} 
  void setCovXX(double input) {m_vtxCovXX = input;}
  void setCovXY(double input) {m_vtxCovXY = input;}
  void setCovXZ(double input) {m_vtxCovXZ = input;}
  void setCovYZ(double input) {m_vtxCovYZ = input;}
  void setCovYY(double input) {m_vtxCovYY = input;}
  void setCovZZ(double input) {m_vtxCovZZ = input;}
  void setX(double input) {m_vtxX = input;}
  void setY(double input) {m_vtxY = input;}
  void setZ(double input) {m_vtxZ = input;}
  void setChi2(double input) {m_vtxChi2 = input;}
  void setNDF(int input) {m_vtxNDF = input;}
  void setIndex(int input) {m_index = input;}


  // Get methods
  int event(void) {return m_event;}
  int lumiBlock(void) {return m_lumiBlock;}
  int run(void) {return m_run;}
  int vtxType(void) {return m_vtxType;}
  double BSX(void) {return m_bsX;} 
  double BSY(void) {return m_bsY;} 
  double BSZ(void) {return m_bsZ;} 
  double BSSigX(void) {return m_bsSigX;} 
  double BSSigY(void) {return m_bsSigY;} 
  double BSSigZ(void) {return m_bsSigZ;} 
  double covXX(void) {return m_vtxCovXX;}
  double covXY(void) {return m_vtxCovXY;}
  double covXZ(void) {return m_vtxCovXZ;}
  double covYZ(void) {return m_vtxCovYZ;}
  double covYY(void) {return m_vtxCovYY;}
  double covZZ(void) {return m_vtxCovZZ;}
  double x(void) {return m_vtxX;}
  double y(void) {return m_vtxY;}
  double z(void) {return m_vtxZ;}
  double chi2(void) {return m_vtxChi2;}
  int ndf(void) {return m_vtxNDF;}
  int index(void) {return m_index;}

}; // End of class definition
