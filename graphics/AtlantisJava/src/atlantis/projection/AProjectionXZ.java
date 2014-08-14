package atlantis.projection;

import atlantis.data.ADHelix;
import atlantis.graphics.ACoord;
import atlantis.parameters.APar;
import atlantis.utils.AMath;
import atlantis.event.*;
import java.awt.*;
import java.awt.geom.*;

public class AProjectionXZ extends AProjection2D {

  public AProjectionXZ() {
    addPopupItem(ASPECT_RATIO_1);
  }

  public String getName() {
    return "XZ";
  }

  public String getXLabel() {
    return "Z";
  }

  public String getYLabel() {
    return "X"+AMath.PRIME;
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
    return parameterStore.get("XZ", "Phi").getD();
  }

  public static double[][] getRotationMatrix() {
    double phi=Math.toRadians(getPhi());
    double cos=Math.cos(phi);
    double sin=Math.sin(phi);
    double[][] r=new double[3][3];

    r[0][0]=cos;
    r[1][1]=cos;
    r[2][2]=1.;
    r[0][1]=sin;
    r[1][0]=-sin;
    return r;
  }

  public static int[] getAxisMapping() {
    return new int[] {2, 0, 1};
  }

  public ACoord getUserPoint(ADHelix dH, double s) {
    return nonLinearTransform(dH.getXZPoint(s));
  }

  public ACoord nonLinearTransform(ACoord user) {
    return nonLinearTransform2D(user, getName());
  }

  public ACoord inverseNonLinearTransform(ACoord user) {
    return inverseNonLinearTransform2D(user, getName());
  }

}
