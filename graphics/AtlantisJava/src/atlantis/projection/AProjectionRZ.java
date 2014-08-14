package atlantis.projection;

import atlantis.data.ADHelix;
import atlantis.graphics.ACoord;
import atlantis.utils.AMath;
import java.awt.*;
import java.awt.geom.*;
import atlantis.event.*;
import atlantis.parameters.*;

public class AProjectionRZ extends AProjection2D {

  public AProjectionRZ() {
    addPopupItem(ASPECT_RATIO_1);
  }

  public String getName() {
    return "RZ";
  }

  public String getScreenName()
  {
      return AMath.RHO + "Z";
  }
  
  public String getXLabel() {
    return "Z";
  }

  public String getYLabel() {
    return AMath.RHO;
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
    double radius=parameterStore.get("Projection", "Radius").getD();
    double length=parameterStore.get("Projection", "Length").getD();
    return aspectRatioLayout(length, radius, wSize);
  }

  public ACoord getUserPoint(ADHelix dH, double s) {
    return nonLinearTransform(dH.getRZPoint(s));
  }

  public ACoord nonLinearTransform(ACoord user) {
    return nonLinearTransform2D(user, getName());
  }

  public ACoord inverseNonLinearTransform(ACoord user) {
    return inverseNonLinearTransform2D(user, getName());
  }

  public static int[] getAxisMapping() {
    return new int[] {2, 1, 0};
  }

}
