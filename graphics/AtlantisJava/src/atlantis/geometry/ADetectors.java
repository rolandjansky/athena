package atlantis.geometry;

import atlantis.graphics.*;
import atlantis.parameters.APar;
import atlantis.projection.*;

/**
 * Base class of all collections of detectors
 * All non muon geometry is in AGeometry.xml
 * All muon geometry is in AMuonGeometry.xml
 * ADetectors is the interface to the rest of the program and contains
 * collections of detectors.
 * When a line in our XML file describes several detectors these are stored
 * individually.
 */
public abstract class ADetectors {

  protected ADetector[] det=null;

  protected int[] listdl;
  protected int numDraw;
  protected int numData;
  
  protected static APar parameterStore = APar.instance();

  public ADetectors(ADetector[] det) {
   this.det=det;
   numData=det.length;
   numDraw=0;
   listdl=new int[numData];
 }

 public ADetector[] getDetectors() {
     return det;
 }

 protected void constructDefaultDrawList() {
  numDraw=numData;
  for(int i=0; i<numDraw; ++i)
    listdl[i]=i;
  }

   protected void makeDrawList(String projection){
     constructDefaultDrawList();
   }

   protected ACoord getUser(String projection) {
       makeDrawList(projection);
       int[] numPoly = new int[numDraw];
       int numTotal = 0;
       for (int i=0; i<numDraw; i++) {
           int len = det[listdl[i]].getUser(projection).hv[0].length;
           numPoly[i] = len;
           numTotal += len;
       }
       double[][][] hv = new double[2][numTotal][];
       int[] index = new int[numTotal];
       for (int i=0, k=0; i<numDraw; i++) {
           double temp[][][] = det[listdl[i]].getUser(projection).hv;
           for(int j = 0; j < numPoly[i]; j++, k++) {
               hv[0][k] = temp[0][j];
               hv[1][k] = temp[1][j];
               index[k] = listdl[i];
           }
       }
       return new ACoord(hv, index);
   }

   protected int[] getColor(int[] dl) {
     int[] col=new int[dl.length];

     for(int i=0; i<dl.length; ++i)
       col[i]=det[dl[i]].getFillColor();
     return col;
   }

   protected boolean[] getDraw(int[] dl) {
      boolean[] draw=new boolean[dl.length];

      for(int i=0; i<dl.length; ++i)
        draw[i]=det[dl[i]].getDraw();
      return draw;
    }

   public String getInfo(int index) {
     return det[index].getName();
   }

  protected ADrawParameters getDrawParameters(int layer, int type) {
    if(layer==0)
      return new ADrawParameters(true, 0, 0, 0, 1, 0);
    else
      return new ADrawParameters(true, 0, 0, 1, 0, 0);
  }

  protected ACoord getUserNoTransform(AProjection projection) {
    String name=projection.getName();
    ACoord data=getUser(name);
    if(name.charAt(0)=='F')
      data.includePhiWrapAround(name);
    return data;
  }

  protected ACoord getUser(AProjection2D projection) {
    return projection.nonLinearTransform(getUserNoTransform(projection));
  }

  /*
   protected void draw(AWindow window,AGraphics ag,AProjection2D projection) {
   // in a simple world the implementation would be here
   throw new RuntimeException();
   }
   */
}
