package atlantis.projection;


import atlantis.graphics.ACoord;
import atlantis.utils.AMath;
import java.awt.*;
import java.awt.geom.*;
import atlantis.data.ADHelix;
import atlantis.parameters.*;


public class AProjectionYZ extends AProjection2D {

  public AProjectionYZ() {
    addPopupItem(ASPECT_RATIO_1);
  }

  public String getName() {
    return "YZ";
  }

  public String getXLabel() {
    return "Z";
  }

  public String getYLabel() {
    return "Y"+AMath.PRIME;
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
    return AProjectionXZ.getPhi();
  }

  public static double[][] getRotationMatrix() {
    return AProjectionXZ.getRotationMatrix();
  }

  public static int[] getAxisMapping() {
    return new int[] {2, 1, 0};
  }

  public ACoord getUserPoint(ADHelix dH, double s) {
    return nonLinearTransform(dH.getYZPoint(s));
  }

  public ACoord nonLinearTransform(ACoord user) {
    return nonLinearTransform2D(user, getName());
  }

  public ACoord inverseNonLinearTransform(ACoord user) {
    return inverseNonLinearTransform2D(user, getName());
  }

}
