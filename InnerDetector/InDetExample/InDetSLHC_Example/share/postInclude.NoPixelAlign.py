# --- switch alignment for IBL geometry off (gives WARNING)
from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
pixelTool           = PixelDetectorTool()
pixelTool.Alignable = False
