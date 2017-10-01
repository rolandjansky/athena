/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPVis_SoGL2PSAction_h
#define HEPVis_SoGL2PSAction_h 

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/actions/SoGLRenderAction.h>

/**
 *  SoGL2PSAction inherits Inventor/SoGLRenderAction.
 * It uses the gl2ps software to produce vector PostScript of a scene graph.
 *  See applications/DetectorTreeKit, Polyhedron for examples.
 */

class SoGL2PSAction : public SoGLRenderAction {
  SO_ACTION_HEADER(SoGL2PSAction);
public:
  SoGL2PSAction(const SbViewportRegion&);
  // gl2ps file formats (mapping of GL2PS_PS, etc...)
/* in gl2ps.h 1.3.2 :
#define GL2PS_PS  0
#define GL2PS_EPS 1
#define GL2PS_TEX 2
#define GL2PS_PDF 3
#define GL2PS_SVG 4
#define GL2PS_PGF 5
*/
  enum Format {
    PS = 0,
    EPS = 1,
    TEX = 2,
    PDF = 3,
    SVG = 4,
    PGF = 5
  };
  void setFileFormat(Format);
  void setFileName(const char*);
  SbBool setPageOptions(const SbString& options);
  void enableFileWriting();
  void disableFileWriting();
  SbBool fileWritingEnabled() const;
  SbBool addBitmap(int,int,float=0,float=0,float=0,float=0);
  void beginViewport(SbBool draw_back = TRUE);
  void endViewport();
public: /*SoINTERNAL*/
  static void initClass();
protected:
  virtual void beginTraversal(SoNode*);
private:
  void gl2psBegin();
  static void separatorAction(SoAction*,SoNode*);
  static void drawStyleAction(SoAction*,SoNode*);
private:
  Format m_fileFormat;
  SbString m_fileName;
  FILE* m_file;
  int m_pageOptions;
  static bool s_didInit;
};

#endif

