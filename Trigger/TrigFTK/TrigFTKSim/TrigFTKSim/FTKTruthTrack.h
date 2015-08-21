/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRUTHTRACK_H
#define FTKTRUTHTRACK_H

#include <TObject.h>
#include <TMath.h>

class FTKTruthTrack : public TObject {

private:
  double m_x;
  double m_y;
  double m_z;
  double m_d0;
  double m_vtx_z;
  int m_q;
  double m_px;
  double m_py;
  double m_pz;
  int m_pdgcode;
  int m_barcode;
  int m_evtindex;

public:
  FTKTruthTrack();

  void setX(double v){m_x = v;}
  void setY(double v){m_y = v;}
  void setZ(double v){m_z = v;}
  void setD0(double v){m_d0 = v;}
  void setVtxZ(double v){m_vtx_z = v;}
  void setPX(double v){m_px = v;}
  void setPY(double v){m_py = v;}
  void setPZ(double v){m_pz = v;}
  void setQ(int v){m_q = v;}
  void setPDGCode(int v){m_pdgcode = v;}
  void setBarcode(int v){m_barcode = v;}
  void setEventIndex(int v) { m_evtindex = v;}

  double getX() const { return m_x;}
  double getY() const { return m_y;}
  double getZ() const { return m_z;}
  double getD0() const { return m_d0;}
  double getVtxZ() const { return m_vtx_z;}
  double getPX() const { return m_px;}
  double getPY() const { return m_py;}
  double getPZ() const { return m_pz;}
  double getPT() const { return TMath::Sqrt(m_px*m_px+m_py*m_py); }
  double getPhi() const { return TMath::ATan2(m_py,m_px); }
  double getEta() const { return TMath::ASinH(m_pz/TMath::Sqrt(m_px*m_px+m_py*m_py)); }
  double getIP() const { return TMath::Sqrt(m_x*m_x+m_y*m_y); }
  int getQ() const { return m_q;}
  int getPDGCode() const { return m_pdgcode;}
  int getBarcode() const { return m_barcode;}
  int getEventIndex() const { return m_evtindex; }
    
  ClassDef(FTKTruthTrack,3)
};

#endif // FTKTRUTHTRACK_H
