package atlantis.geometry;


import java.awt.*;

import atlantis.canvas.*;
import atlantis.graphics.*;
import atlantis.graphics.colormap.AColorMap;
import atlantis.gui.*;
import atlantis.parameters.*;
import atlantis.projection.*;


public class ADetectorSystem 
{

  private ADetectors[] detectors;
  protected static String geometryName = null;
  
  protected static APar parameterStore = APar.instance();

  ADetectorSystem(ADetectors[] detectors) 
  {
    this.detectors=detectors;
  }

  public ADetectors[] getDetectors() 
  {
    return detectors;
  }

  // The whole of atlas is drawn in one loop to avoid problem with frames
  // overlapping


  public void draw(AWindow window, AGraphics ag, AProjection2D projection) 
  {
    if(!parameterStore.get("Det", "Detectors").getStatus()) return;
    boolean drawFrames=parameterStore.get("Det", "HideDetectors").getStatus();
    //also draw frames for Grey/BW color maps
    boolean colorBW=AColorMap.drawFrames();
    Color[] colorMap=AColorMap.getColors();
    ADetectors[] detectors=getDetectors();
    ACoord[] display=new ACoord[detectors.length];
    int[][] index=new int[detectors.length][];
    int[][] detColor=new int[detectors.length][];
    boolean[][] draw=new boolean[detectors.length][];
    int bkgColor = parameterStore.get("Color","BkgFill").getI();

    for(int det=0; det<detectors.length; ++det) {
      display[det]=window.calculateDisplay(detectors[det].getUser(projection));
      index[det]=display[det].index;
      detColor[det]=detectors[det].getColor(index[det]);
      draw[det]=detectors[det].getDraw(index[det]);
    }

    boolean modeSelection = parameterStore.get(projection.getName(), "Mode").getI()==0
        ||(parameterStore.get(projection.getName(), "Mode").getI()>8
            &&(projection instanceof AProjectionYX||projection instanceof AProjectionFR));
    
    for(int layer=0; layer<2; ++layer) 
    {
      for(int det=0; det<detectors.length; ++det) 
      {
        ADrawParameters dp=detectors[det].getDrawParameters(layer, 0);
        ag.updateDrawParameters(dp);
        for(int i=0; i<display[det].hv[0].length; ++i) 
        {
          int numPoints=display[det].hv[0][i].length;
          ag.setColor(colorMap[detColor[det][i]]);
          if(draw[det][i]&& numPoints>0)
          {
            if(modeSelection) 
            {
              if(layer==0 && (drawFrames||colorBW))
              {
                if(colorBW) 
                  ag.setColor(colorMap[AColorMap.BK]);
                ag.drawPolygon(display[det].hv[0][i], display[det].hv[1][i], numPoints);
              } 
              else if(layer==1) 
              {
                if(drawFrames) 
                  ag.setColor(colorMap[bkgColor]);
                ag.fillPolygon(detectors[det], index[det][i], display[det].hv[0][i],
                    display[det].hv[1][i], numPoints);
              }
            } 
            else 
            {
              if(layer==0) 
              {
                if(drawFrames) 
                  ag.setColor(colorMap[bkgColor]);
                ag.fillPolygon(detectors[det], index[det][i], display[det].hv[0][i],
                    display[det].hv[1][i], numPoints);
              } 
              else if(layer==1 && (drawFrames||colorBW))  
              {
                if(colorBW) 
                  ag.setColor(colorMap[AColorMap.BK]);
                ag.drawPolygon(display[det].hv[0][i], display[det].hv[1][i], numPoints);
              }
            }
          }
        }
      }
    }
    ABarrelSiliconDetector.drawAdditionalItems(window, ag, projection);
    AEndcapSiliconDetector.drawAdditionalItems(window, ag, projection);
    ABarrelTRTDetector.drawAdditionalItems(window, ag, projection);
    AEndcapTRTDetector.drawAdditionalItems(window, ag, projection);
    ACalorimeterDetector.drawAdditionalItems(window, ag, projection);
  }

  public static String getGeometryName()
  {
      return geometryName == null ? "n/a" : geometryName;

  } // getName() ------------------------------------------------------------

}
