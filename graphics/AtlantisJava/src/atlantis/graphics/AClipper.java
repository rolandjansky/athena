package atlantis.graphics;

import atlantis.utils.AClipPolygon;
import atlantis.utils.APolygon;
import java.awt.Rectangle;


/**
 * Implement uniform treatment of Clipping for screen,
 * postscript and picking.
 */

public class AClipper {
  double[] upper;
  double[] lower;
  double[][] x;
  boolean[][] below;
  boolean[][] within;
  boolean[][] above;
  double hLower;
  double hUpper;
  double vLower;
  double vUpper;

  public static final int NONE_WITHIN=0;
  public static final int MAYBE_WITHIN=1;
  public static final int ALL_WITHIN=2;

  public AClipper(double hLower, double hUpper, double vLower, double vUpper) {

    x=new double[2][2];
    upper=new double[2];
    lower=new double[2];
    below=new boolean[2][2];
    within=new boolean[2][2];
    above=new boolean[2][2];
    upper[0]=hUpper;
    upper[1]=vUpper;
    lower[0]=hLower;
    lower[1]=vLower;
    this.hUpper=hUpper;
    this.hLower=hLower;
    this.vUpper=vUpper;
    this.vLower=vLower;
  }

  public AClipper(Rectangle clipBounds) {
    this(clipBounds.getX(), clipBounds.getX()+clipBounds.getWidth(), clipBounds.getY(),
        clipBounds.getY()+clipBounds.getHeight());
  }

  public boolean isPointWithin(double h0, double v0) {
    return hLower<=h0&&h0<=hUpper&&vLower<=v0&&v0<=vUpper;
  }

  public boolean isLineWithin(double h0, double v0, double h1, double v1) {
    int roughCheck=fastWithin(h0, v0, h1, v1);

    if(roughCheck==0) return false;
    x[0][0]=h0;
    x[1][0]=v0;
    x[0][1]=h1;
    x[1][1]=v1;
    if(roughCheck==2) return true;

    update(0);
    update(1);

    outer:
    while(!lineFullyInside()) {
      if(lineFullyOutside()) return false;

      for(int i=0; i<2; i++)
        for(int point=0; point<2; point++) {
          if(!within[i][point]) {
            if(below[i][point]) {
              x[1-i][point]+=((x[1-i][1-point]-x[1-i][point])/(x[i][1-point]-x[i][point]))
                  *(lower[i]-x[i][point]);
              x[i][point]=lower[i];
            } else if(above[i][point]) {
              x[1-i][point]+=((x[1-i][1-point]-x[1-i][point])/(x[i][1-point]-x[i][point]))
                  *(upper[i]-x[i][point]);
              x[i][point]=upper[i];
            }
            update(point);
            continue outer;
          }
        }
    }
    // don't allow non-zero length line to be given zero length
    // by th clipper
    if((x[0][0]==x[0][1])&&(x[1][0]==x[1][1]))
      return false;
    return true;
  }

  private void update(int p) {
    for(int i=0; i<2; i++) {
      below[i][p]=false;
      within[i][p]=false;
      above[i][p]=false;
      if(x[i][p]<lower[i])
        below[i][p]=true;
      else if(x[i][p]>upper[i])
        above[i][p]=true;
      else
        within[i][p]=true;
    }
  }

  public boolean lineFullyInside() {
    boolean inside=true;

    for(int i=0; i<2; i++)
      for(int j=0; j<2; j++)
        inside=inside&&within[i][j];
    return inside;
  }

  public boolean lineFullyOutside() {
    boolean outside=false;

    for(int i=0; i<2; i++) {
      outside=outside||(below[i][0]&&below[i][1]);
      outside=outside||(above[i][0]&&above[i][1]);
    }
    return outside;
  }

  public double[][] getHV() {
    return x;
  }

  public double getH0() {
    return x[0][0];
  }

  public double getV0() {
    return x[1][0];
  }

  public double getH1() {
    return x[0][1];
  }

  public double getV1() {
    return x[1][1];
  }

  public int fastWithin(double h0, double v0, double h1, double v1) {
    if(h0<hLower&&h1<hLower) return NONE_WITHIN;
    if(h0>hUpper&&h1>hUpper) return NONE_WITHIN;
    if(v0<vLower&&v1<vLower) return NONE_WITHIN;
    if(v0>vUpper&&v1>vUpper) return NONE_WITHIN;
    if(hLower<=h0&&h0<=hUpper&&hLower<=h1&&h1<=hUpper&&vLower<=v0&&v0<=vUpper&&vLower<=v1
        &&v1<=vUpper) return ALL_WITHIN;
    return MAYBE_WITHIN;
  }

  public int fastWithin(double[] h, double[] v, int numPoints) {
    if(numPoints==0) return NONE_WITHIN;
    boolean firstWithin=hLower<=h[0]&&h[0]<=hUpper&&vLower<=v[0]&&v[0]<=vUpper;

    if(firstWithin) {
      for(int i=1; i<numPoints; ++i)
        if(!(hLower<=h[i]&&h[i]<=hUpper&&vLower<=v[i]&&v[i]<=vUpper)) return MAYBE_WITHIN;
      return ALL_WITHIN;
    } else {
      if(h[0]<hLower) {
        int num=1;

        for(int i=1; i<numPoints; ++i)
          if(h[i]<hLower)
            num++;
          else
            break;
        if(num==numPoints) return NONE_WITHIN;
      }
      if(h[0]>hUpper) {
        int num=1;

        for(int i=1; i<numPoints; ++i)
          if(h[i]>hUpper)
            num++;
          else
            break;
        if(num==numPoints) return NONE_WITHIN;
      }
      if(v[0]<vLower) {
        int num=1;

        for(int i=1; i<numPoints; ++i)
          if(v[i]<vLower)
            num++;
          else
            break;
        if(num==numPoints) return NONE_WITHIN;
      }
      if(v[0]>vUpper) {
        int num=1;

        for(int i=1; i<numPoints; ++i)
          if(v[i]>vUpper)
            num++;
          else
            break;
        if(num==numPoints) return NONE_WITHIN;
      }
      return MAYBE_WITHIN;
    }
  }

  public ACoord clipPolygon(double[] h, double[] v, int numPoints) {
    double[] clipH=new double[] { lower[0], upper[0], upper[0], lower[0] };
    double[] clipV=new double[] { lower[1], lower[1], upper[1], upper[1] };
    AClipPolygon clipArea=new AClipPolygon(clipH, clipV);
    APolygon badPolygon=new APolygon(h, v);

    badPolygon.clip(clipArea);
    h=badPolygon.getX();
    v=badPolygon.getY();
    return new ACoord(h, v);
  }

}

