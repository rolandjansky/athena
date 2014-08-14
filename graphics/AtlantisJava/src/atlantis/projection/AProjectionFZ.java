package atlantis.projection;


import atlantis.parameters.AParameter;
import atlantis.parameters.APar;
import java.awt.geom.*;
import java.awt.*;
import atlantis.graphics.ACoord;
import atlantis.data.ADHelix;


public class AProjectionFZ extends AProjectionPhi {

  public String getName() {
    return "FZ";
  }

  public String getXLabel() {
    return "Z";
  }

  public String getXUnits() {
    return "(cm)";
  }

  public Point2D.Double[] calculateNoZoomCorners(Dimension wSize) {
    Point2D.Double[] corners=new Point2D.Double[3];
    int phiWrap=parameterStore.get(getName(), "PhiWrap").getI();

    double length=parameterStore.get("Projection", "Length").getD();

    corners[0]=new Point2D.Double(-length, 360+phiWrap);
    corners[1]=new Point2D.Double(length, 360+phiWrap);
    corners[2]=new Point2D.Double(length, 0);
    return corners;
  }

  public ACoord getUserPoint(ADHelix dH, double s) {
    return nonLinearTransform(dH.getFZPoint(s));
  }

  public ACoord nonLinearTransform(ACoord user) {
    AParameter fishEyePar=parameterStore.get(getName(), "FishEye");

    if(fishEyePar.getStatus()) {
      double[] z;
      double fishEye=0.001*fishEyePar.getD();
      double zTo=parameterStore.get(getName(), "zTo").getD();

      for(int j=0; j<user.hv[0].length; ++j) {
        z=user.hv[0][j];
        for(int i=0; i<z.length; ++i)
          z[i]*=(1+fishEye*zTo)/(1+fishEye*Math.abs(z[i]));
      }
    }
    return user;
  }

  public ACoord inverseNonLinearTransform(ACoord user) {
    AParameter fishEyePar=parameterStore.get(getName(), "FishEye");

    if(fishEyePar.getStatus()) {
      double[] z;
      double fishEye=0.001*fishEyePar.getD();
      double zTo=parameterStore.get(getName(), "zTo").getD();

      for(int j=0; j<user.hv[0].length; ++j) {
        z=user.hv[0][j];
        for(int i=0; i<z.length; ++i)
          z[i]*=1/(1+fishEye*(zTo-Math.abs(z[i])));
      }
    }
    return user;
  }

}
