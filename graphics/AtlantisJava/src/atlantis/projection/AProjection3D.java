package atlantis.projection;

import atlantis.data.ADHelix;
import atlantis.graphics.*;
import atlantis.parameters.APar;
import atlantis.utils.AMath;
import atlantis.event.*;
import java.awt.*;
import java.awt.geom.*;
import javax.swing.JMenuItem;

public class AProjection3D extends AProjection2D {

  // axis about which the rotations take place
  static private double phi;
  static private double sinPhi, cosPhi;
  // center of rotation in xyz frame
  static private double rx, ry, rz;
  // three axis abc in the xyz frame
  static private double ax, ay, az;
  static private double bx, by, bz;
  static private double cx, cy, cz;

  static private double[][] abc=new double[3][3];

  public AProjection3D() {
    JMenuItem asp = new JMenuItem(ASPECT_RATIO_1);
    asp.addActionListener(this);
    popupItems.add(asp);
  }

  public String getName() {
    return "3D";
  }

  public String getXLabel() {
    return "V";
  }

  public String getYLabel() {
    return "H";
  }

  public String getXUnits() {
    return "(cm)";
  }

  public String getYUnits() {
    return "(cm)";
  }

  public Point2D.Double getCenter() {
    return new Point2D.Double(0, 0);
  }

  public Point2D.Double[] calculateNoZoomCorners(Dimension wSize) {
    double radius=parameterStore.get("Projection", "TrackingRadius").getD();
    double length=parameterStore.get("Projection", "TrackingLength").getD();
    return aspectRatioLayout(length, radius, wSize);
  }

  public static double getPhi() {
    return parameterStore.get("3D", "Phi").getD();
  }

  public static void updateParameters() {
    phi=Math.toRadians(getPhi());
    sinPhi=Math.sin(phi);
    cosPhi=Math.cos(phi);
    ax=parameterStore.get("3D", "xAxis").getD();
    ay=parameterStore.get("3D", "yAxis").getD();
    az=parameterStore.get("3D", "zAxis").getD();
    rx=parameterStore.get("Event", "XVtx").getD();
    ry=parameterStore.get("Event", "YVtx").getD();
    rz=parameterStore.get("Event", "ZVtx").getD();
    double dx;
    double dy;
    double dz;

    if(ax==0.&&ay==0) {
      // d is the y-axis
      dx=0.;
      dy=-1.;
      dz=0.;
    } else {
      // d is the z-axis
      dx=0.;
      dy=0.;
      dz=1.;
    }

    bx=+ay*dz-az*dy;
    by=-ax*dz+az*dx;
    bz=+ax*dy-ay*dx;

    cx=+ay*bz-az*by;
    cy=-ax*bz+az*bx;
    cz=+ax*by-ay*bx;
  }

  public static double[][] getRotationMatrix() {
    updateParameters();
    abc[0][0]=ax;
    abc[0][1]=ay;
    abc[0][2]=az;
    abc[1][0]=bx;
    abc[1][1]=by;
    abc[1][2]=bz;
    abc[2][0]=cx;
    abc[2][1]=cy;
    abc[2][2]=cz;
    return abc;
  }

  public static double[] getRotated(double[] p) {
    updateParameters();
    double x=p[0]-rx;
    double y=p[1]-ry;
    double z=p[2]-rz;

    double u=x*ax+y*ay+z*az;
    double v=x*bx+y*by+z*bz;
    double w=x*cx+y*cy+z*cz;

    p[0]=u;
    p[1]=v*cosPhi-w*sinPhi;
    p[2]=v*sinPhi+w*cosPhi;
    return p;
  }

  public static double[][] getRotated(int numDraw, int[] listdl, float[] x, float[] y, float[] z) {
    updateParameters();

    double[][] uvw=new double[3][numDraw];

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      double xx=x[list]-rx;
      double yy=y[list]-ry;
      double zz=z[list]-rz;
      double u=xx*ax+yy*ay+zz*az;
      double v=xx*bx+yy*by+zz*bz;
      double w=xx*cx+yy*cy+zz*cz;
      uvw[0][i]=u;
      uvw[1][i]=v*cosPhi-w*sinPhi;
      uvw[2][i]=v*sinPhi+w*cosPhi;
    }
    return uvw;
  }

  public static int[] getAxisMapping() {
    return new int[] {0, 1, 2};
  }

  public ACoord getUserPoint(ADHelix dH, double s) {
    return nonLinearTransform(dH.get3DPoint(s));
  }

  public ACoord nonLinearTransform(ACoord user) {
    return nonLinearTransform2D(user, getName());
  }

  public ACoord inverseNonLinearTransform(ACoord user) {
    return inverseNonLinearTransform2D(user, getName());
  }

}
