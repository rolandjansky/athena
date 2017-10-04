/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// this :
#include <VP1HEPVis/actions/SoGL2PSAction.h>

// Inventor :
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/errors/SoDebugError.h>

//LD HEPVis specific #include <HEPVis/SbStyles.h>
#include "SbStringHelpers.h"

#include "VP1HEPVis/gl2ps.h"

#include <stdio.h>

SO_ACTION_SOURCE(SoGL2PSAction)
//////////////////////////////////////////////////////////////////////////////
bool SoGL2PSAction::s_didInit = false;
void SoGL2PSAction::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if ( !s_didInit ) {
    s_didInit = true;
    SO_ACTION_INIT_CLASS(SoGL2PSAction,SoGLRenderAction);
    SO_ACTION_ADD_METHOD(SoSeparator,separatorAction);
    SO_ACTION_ADD_METHOD(SoDrawStyle,drawStyleAction);
  }
}
//////////////////////////////////////////////////////////////////////////////
SoGL2PSAction::SoGL2PSAction(
 const SbViewportRegion& aViewPortRegion
)
:SoGLRenderAction(aViewPortRegion)
,m_fileFormat(EPS)
,m_fileName("out.ps")
,m_file(0)
,m_pageOptions(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ACTION_CONSTRUCTOR(SoGL2PSAction);

  m_pageOptions = GL2PS_OCCLUSION_CULL |
                 GL2PS_BEST_ROOT |
                 GL2PS_SILENT |
                 GL2PS_DRAW_BACKGROUND;
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::setFileFormat(
 Format aFileFormat
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  m_fileFormat = aFileFormat;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoGL2PSAction::setPageOptions(
 const SbString& aOptions
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbPList words = SbStringGetWords(aOptions,";");
  int wordn = words.getLength();
  for(int wordi=0;wordi<wordn;wordi++) {
    SbString& word = *((SbString*)words[wordi]);
    if(!word.getLength()) continue;

    SbString option("");
    SbBool value = FALSE;
   {SbPList o_v = SbStringGetWords(word,"=");
    if(o_v.getLength()==2) {
      option = *((SbString*)o_v[0]);
      SbString svalue = *((SbString*)o_v[1]);
      if(SbStringConvertToBool(svalue,value)==FALSE)  {
        SoDebugError::postInfo("SoGL2PSAction::setPageOptions",
          "value \"%s\" not a bool.\n",svalue.getString());
      }
    } else {
      option = word;
      value = TRUE;
    }
    SbStringDelete(o_v);}

#define SET_OPT(a_opt) \
      if(value==TRUE) {\
        m_pageOptions |= a_opt;\
      } else {\
        m_pageOptions &= ~a_opt;\
      }

    if(option=="DEFAULTS") {
      m_pageOptions = GL2PS_OCCLUSION_CULL |
                     GL2PS_BEST_ROOT |
                     GL2PS_SILENT |
                     GL2PS_DRAW_BACKGROUND;
    } else if(option=="NONE") {
      m_pageOptions = 0;
    } else if(option=="DRAW_BACKGROUND") {
      SET_OPT(GL2PS_DRAW_BACKGROUND)
    } else if(option=="SIMPLE_LINE_OFFSET") {
      SET_OPT(GL2PS_SIMPLE_LINE_OFFSET)
    } else if(option=="SILENT") {
      SET_OPT(GL2PS_SILENT)
    } else if(option=="BEST_ROOT") {
      SET_OPT(GL2PS_BEST_ROOT)
    } else if(option=="OCCLUSION_CULL") {
      SET_OPT(GL2PS_OCCLUSION_CULL)
    } else if(option=="NO_TEXT") {
      SET_OPT(GL2PS_NO_TEXT)
    } else if(option=="LANDSCAPE") {
      SET_OPT(GL2PS_LANDSCAPE)
    } else if(option=="NO_PS3_SHADING") {
      SET_OPT(GL2PS_NO_PS3_SHADING)
    } else if(option=="NO_PIXMAP") {
      SET_OPT(GL2PS_NO_PIXMAP)
    } else if(option=="USE_CURRENT_VIEWPORT") {
      SET_OPT(GL2PS_USE_CURRENT_VIEWPORT)
    } else if(option=="COMPRESS") {
      SET_OPT(GL2PS_COMPRESS)
    } else if(option=="NO_BLENDING") {
      SET_OPT(GL2PS_NO_BLENDING)
    } else if(option=="TIGHT_BOUNDING_BOX") {
      SET_OPT(GL2PS_TIGHT_BOUNDING_BOX)
    } else {
      SoDebugError::postInfo("SoGL2PSAction::setPageOptions",
        "bad option \"%s\".\n",option.getString());
    }
  }
  SbStringDelete(words);

  //::printf("debug : SoGL2PSAction::setPageOptions : %d\n",m_pageOptions);

  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::setFileName(
 const char* aFileName
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  m_fileName = aFileName;
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::enableFileWriting(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(m_fileFormat==GL2PS_PDF) {
    m_file = ::fopen(m_fileName.getString(),"wb");
  } else {
    m_file = ::fopen(m_fileName.getString(),"w");
  }
  if(!m_file) {
    SoDebugError::post("SoGL2PSAction::enableFileWriting",
                       "Cannot open file %s",m_fileName.getString());
    return;
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::disableFileWriting(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ::fclose(m_file);
  m_file = 0;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoGL2PSAction::fileWritingEnabled(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (m_file?TRUE:FALSE);
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoGL2PSAction::addBitmap(
 int aWidth
,int aHeight
,float aXorig
,float aYorig
,float aXmove
,float aYmove
)
/////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!m_file) return FALSE;
  GLboolean valid;
  glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID,&valid);
  if(valid==GL_FALSE) return FALSE;
  float pos[4];
  glGetFloatv(GL_CURRENT_RASTER_POSITION,pos);
  int xoff = -(int)(aXmove + aXorig);
  int yoff = -(int)(aYmove + aYorig);
  int x = (int)(pos[0] + xoff);
  int y = (int)(pos[1] + yoff);
  // Should clip against viewport area :
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT,vp);
  GLsizei w = aWidth;
  GLsizei h = aHeight;
  if(x+w>(vp[0]+vp[2])) w = vp[0]+vp[2]-x;
  if(y+h>(vp[1]+vp[3])) h = vp[1]+vp[3]-y;
  int s = 3 * w * h;
  if(s<=0) return FALSE;
  float* image = new float[s];
  glReadPixels(x,y,w,h,GL_RGB,GL_FLOAT,image);
  GLint status = gl2psDrawPixels(w,h,xoff,yoff,GL_RGB,GL_FLOAT,image);
  delete [] image;
  return (status!=GL2PS_SUCCESS ? FALSE : TRUE);
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::beginViewport(
 SbBool aDrawBack
)
/////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!m_file) return;
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT,vp);
  if(aDrawBack) {
    gl2psBeginViewport(vp);
  } else {
    GLint opts = 0;
    gl2psGetOptions(&opts);
    gl2psSetOptions(0);
    gl2psBeginViewport(vp);
    gl2psSetOptions(opts);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::endViewport(
)
/////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!m_file) return;
  gl2psEndViewport();
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::beginTraversal(
 SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{

  if(m_file) {
    const SbViewportRegion& vpr = getViewportRegion();
    SoViewportRegionElement::set(getState(),vpr);

    gl2psBegin();
    traverse(aNode);
    gl2psEndViewport();
    gl2psEndPage();
  } else {
    SoGLRenderAction::beginTraversal(aNode);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::gl2psBegin(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!m_file) return;

  int sort = GL2PS_BSP_SORT;
  //int sort = GL2PS_SIMPLE_SORT;

  const SbViewportRegion& vpr = getViewportRegion();
  SoViewportRegionElement::set(getState(),vpr);

  const SbVec2s& win = vpr.getWindowSize();
  GLint vp[4];
  vp[0] = 0;
  vp[1] = 0;
  vp[2] = win[0];
  vp[3] = win[1];

  int bufsize = 0;
  gl2psBeginPage("title","HEPVis::SoGL2PSAction",
                 vp,
                 m_fileFormat,
                 sort,
                 m_pageOptions,
                 GL_RGBA,0, NULL,0,0,0,
                 bufsize,
                 m_file,m_fileName.getString());

  gl2psBeginViewport(vp);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::separatorAction(
 SoAction* aThis
,SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoGL2PSAction* This = (SoGL2PSAction*)aThis;
  if(This->m_file) {
    //SoSeparator may use render caching.
    aNode->doAction(aThis);
  } else {
    SoNode::GLRenderS(aThis,aNode);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoGL2PSAction::drawStyleAction(
 SoAction* aThis
,SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNode::GLRenderS(aThis,aNode);
  SoGL2PSAction* This = (SoGL2PSAction*)aThis;
  if(This->m_file) {
    SoDrawStyle* soDrawStyle = (SoDrawStyle*)aNode;
    if(soDrawStyle->style.getValue()==SoDrawStyle::LINES) {
      float w = soDrawStyle->lineWidth.getValue();
      gl2psLineWidth(w);
      gl2psDisable(GL2PS_LINE_STIPPLE);

      /* LD HEPVis specific
      unsigned short pattern = soDrawStyle->linePattern.getValue();
      if(pattern==SbLinePattern_solid)
        gl2psDisable(GL2PS_LINE_STIPPLE);
      else
        gl2psEnable(GL2PS_LINE_STIPPLE);
      */
    }
  }
}
