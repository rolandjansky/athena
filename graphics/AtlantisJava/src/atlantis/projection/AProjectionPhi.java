package atlantis.projection;

import atlantis.parameters.AParameter;
import atlantis.parameters.APar;
import atlantis.utils.AMath;
import atlantis.utils.AVector;
import java.awt.geom.*;

/**
 * Base class of all phi projections
 */

public abstract class AProjectionPhi extends AProjection2D {

  public String getScreenName()
  {
    return AMath.PHI+ getXLabel();
  }  
    
  public String getYLabel() {
    return AMath.PHI;
  }

  public String getYUnits() {
    return AMath.DEGREES;
  }

  public double getMinRho() {
    return 2.;
  }

  public Point2D.Double getCenter() {
    int phiWrap=parameterStore.get(getName(), "PhiWrap").getI();

    return new Point2D.Double(0, (360+phiWrap)/2);
  }

  public Point2D.Double[] validateCorners(Point2D.Double[] corners) {
    if(super.validateCorners(corners)==null)
      return null;
    int phiWrap=parameterStore.get(getName(), "PhiWrap").getI();
    AParameter initialFramePar=parameterStore.get(getName(), "InitialFrame");

    if(Math.abs(getMaxPhi(corners)-getMinPhi(corners))>(360+phiWrap))
      return null;
    if(getMaxPhi(corners)>(initialFramePar.getI()*360+2*360-90))
      initialFramePar.setI(initialFramePar.getI()+1);
    if(getMinPhi(corners)<(initialFramePar.getI()*360+90))
      initialFramePar.setI(initialFramePar.getI()-1);
    return corners;
  }

  private static double getMinPhi(Point2D.Double[] corners) {
    double phiMin=corners[0].y;
    
    for(int i=0; i<corners.length; i++)
      if(corners[i].y<phiMin) phiMin=corners[i].y;
    AVector v12=new AVector(corners[1], corners[2]);

    return Math.min(phiMin, corners[0].y+v12.dy);
  }

  private static double getMaxPhi(Point2D.Double[] corners) {
    double phiMax=corners[0].y;

    for(int i=0; i<corners.length; i++)
      if(corners[i].y>phiMax) phiMax=corners[i].y;
    AVector v12=new AVector(corners[1], corners[2]);

    return Math.max(phiMax, corners[0].y+v12.dy);
  }

}
