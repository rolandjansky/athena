# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MatrixToolGetter.py
## @brief:  to instantiate the MatrixTool
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## MatrixToolGetter configures the MatrixTool with the default
#  configuration but does not add to ToolSvc.  User must add as a 
#  private tool or add to ToolSvc.
class MatrixToolGetter ( Configured ) :
  
  _name = 'MatrixToolGetter'

  ## create and configure MatrixTool
  def configure(self):
    mlog = logging.getLogger( self._name+'::configure :' )

    # create tool instance
    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__MatrixTool
    matrixTool = Trk__MatrixTool("MatrixTool",
                                 OutputLevel = align.OutputLevel())
    matrixTool.UseSparse          = align.useSparseMatrix()
    matrixTool.SolveOption        = align.matrixToolSolveOption()
    matrixTool.WriteMat           = align.writeMatrix()
    matrixTool.WriteEigenMat      = align.writeEigenMatrices()
    matrixTool.WriteModuleNames   = align.writeModuleNames()
    matrixTool.ModCut             = align.weakModeToRemove()
    matrixTool.SoftEigenmodeCut   = align.softEigenmodeCut()
    matrixTool.ScaleMatrix        = align.scaleMatrix()
    matrixTool.RunLocalMethod     = align.runLocalMethod()
    matrixTool.Diagonalize        = align.runCLHEPDiagonalize()
    matrixTool.MinNumHitsPerModule = align.minNumHitsPerModule()
    matrixTool.MinNumTrksPerModule = align.minNumTrksPerModule()
    matrixTool.WriteHitmap        = align.writeHitmap()
    matrixTool.WriteHitmapTxt     = align.writeHitmapTxt()
    matrixTool.ReadHitmaps        = align.readHitmaps()
    if align.inputHitmapFiles()!=[]:
      matrixTool.InputHitmapFiles   = align.inputHitmapFiles()
    if align.inputMatrixFiles()!=[]:
      matrixTool.InputMatrixFiles   = align.inputMatrixFiles()
    if align.inputHitmapFiles()!=[]:
      matrixTool.InputVectorFiles   = align.inputVectorFiles()

    self._matrixToolHandle = matrixTool
    
    #mlog.info(" now adding to ToolSvc" )
    #from __main__ import ToolSvc
    #ToolSvc += self.MatrixToolHandle()

    mlog.info(" leaving configure")

    return True

  ## return handle to MatrixTool
  def MatrixToolHandle(self):
      return self._matrixToolHandle
