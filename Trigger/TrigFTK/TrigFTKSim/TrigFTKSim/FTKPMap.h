/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKPMAP_H
#define FTKPMAP_H

#include <vector>
#include <string>

#include "ftkdefs.h"
#include <TDirectory.h>

/** This class describes a plane section, used within the plane map
    class */
class FTKPlaneSection {
private:
  int m_plane; // plane ID
  int m_section; // section ID

public:
  FTKPlaneSection();
  FTKPlaneSection(int, int);
  virtual ~FTKPlaneSection() {;}

  int getPlane()  const { return m_plane; }
  int getSection() const { return m_section; }

  void setPlane(int v)  { m_plane = v; }
  void setSection(int v)  { m_section = v; }

};


/** \brief Layer module informations.

    Max index of modules along phi and eta is specified.
    Attention: on SCT layers the modules with eta index 0 is missing so
    the actual number of modules is neta-1.
*/
class FTKModuleInfo {
private:
  int m_isEndcap;	/* 0 = barrel, 1 = endcap */
  int m_layer;		/**< physical layer number */
  int m_pdisk;		/**< actual physical disk; signed, 1-indexed disk number, barrel is zero */
  int m_nphi; 		/**< max phi index */
  int m_neta;		/**< max eta index */
  int m_nfi;		/**< max par index */ // cy addition - phi for SCTtrk
  int m_ncott;		/**< max par index */ // cy addition
  int m_ncurv;		/**< max par index */ // cy addition
  int m_nste;
  int m_ndimension;


public:
  FTKModuleInfo();
  FTKModuleInfo(int isEndcap,int layer,int pdisk,
		int nphi,int neta,int nfi,int ncott,int ncurv,int nste,int ndimension);
  virtual ~FTKModuleInfo() {;}

  int getIsEndcap() const { return m_isEndcap; }
  int getLayer() const { return m_layer; }
  int getPDisk() const { return m_pdisk; }
  int getNumPhi() const { return m_nphi; }
  int getNumEta() const { return m_neta; }
  int getNumFi() const { return m_nfi; }
  int getNumCott() const { return m_ncott; }
  int getNumCurv() const { return m_ncurv; }
  int getNSte() const { return m_nste; }
  int getNDimension() const { return m_ndimension; }

  void setIsEndcap(int v) { m_isEndcap = v; }
  void setLayer(int v) { m_layer = v; }
  void setPDisk(int v) { m_pdisk = v; }
  void setNumPhi(int v) { m_nphi = v; }
  void setNumEta(int v) { m_neta = v; }
  void setNumFi(int v) { m_nfi = v; }
  void setNumCott(int v) { m_ncott = v; }
  void setNumCurv(int v) { m_ncurv = v; }
  void setNSte(int v) { m_nste = v; }
  void setNDimension(int v) { m_ndimension = v; }
};


/** This class describes the configuration of the detectors plane
    used by FTK */
class FTKPlaneMap {
private:
  std::string m_path; // coniguration file path
  int m_nplanes; // number of used detector planes
   //int m_nlayers; // number of detector layers
  int m_rlayers; // 
  int m_totaldim; // dimension of the coordinates
  int *m_planeType; // SCT / PIXEL / SCTtrk
  int *m_isZ; // legacy (unused)
  int *m_planePair;
  bool m_enablePlanePair; // if true the SCT pairing is activated, effect on getPlanePair(int) method
  int *m_idim[2];
  int *m_nsections; // number of sections in the used planes
  /* First index: PIXEL/SCT/SCTtrk
   * Second index: BARREL/ENDCAP
   * returns a pair: (plane,section)
   */
  FTKPlaneSection *m_map[ftk::NHITTYPES][2];
  FTKModuleInfo **m_planes; // used modules information
  bool m_isok; // false if there were error during configuration

protected:
   static const TString s_dirname;
   static int GetConst(TDirectory* file, TString ConstName);


public:
  FTKPlaneMap(const char *);
   FTKPlaneMap(TDirectory* file); //!< used for instantiation from root file
  ~FTKPlaneMap();

   // interface to rootfile
   void WriteMapToRootFile(TDirectory* dir);
   static FTKPlaneMap* GetPMapFromRootFile(TDirectory* file);
   static int GetNPlanes(TDirectory* file);
   static int GetChecksum(TDirectory* file);
   static int GetTotalDim(TDirectory* file);

   int CalcChecksum() const;
   void DumpPMap() const;
   
  const char *getPath() const { return m_path.c_str(); }

  int getNPlanes() const { return m_nplanes; }
  void setNPlanes(int v) { m_nplanes = v; }

  int getNSections(int pos) const { return m_nsections[pos]; }

  int getRLayers() const { return m_rlayers; }
  void setRLayers(int v) { m_rlayers = v; }

  int isSCT(int pl) const { return m_planeType[pl]==ftk::SCT; }
  int isPixel(int pl) const { return m_planeType[pl]==ftk::PIXEL; }
  int isSCTtrk(int pl) const { return m_planeType[pl]==ftk::SCTtrk; }
  int getSCTtrkPlane() const;

  int getDim(int pl, int id) const { return m_idim[id][pl]; }
  int getTotalDim() const { return m_totaldim; }
  int getPlanePair(int pl) const { return m_enablePlanePair ? m_planePair[pl] : pl; }

  bool getEnablePlanePair() const { return m_enablePlanePair; }
  void setEnablePlanePair(const bool state) { m_enablePlanePair = state; }

  FTKModuleInfo &getPlane(int plane, int section) const 
    { return m_planes[plane][section]; }

  FTKPlaneSection& getMap(int type, int section, int layer) const
    { return m_map[type][section][layer]; }

  bool getIsOK() const { return m_isok;}
  bool operator!()
  {return !m_isok; }
};
#endif // FTKPMAP_H
