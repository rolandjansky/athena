package atlantis.interactions;


import java.awt.*;
import java.awt.geom.*;

import atlantis.graphics.*;
import atlantis.parameters.*;
import atlantis.projection.*;
import atlantis.utils.*;


public class AClockInteraction extends AInteraction
  implements ASleepMouseDragListener {

  private ALineSegment line;
  private double phi;

  public AClockInteraction() {
    super(1, WINDOW_TOTAL_REPAINT, WORLD_COORDINATES, false);
    // double phi = APar.get("YX","Phi").getD()*AMath.DEGREES_TO_RADIANS+Math.PI;
    double phi=Math.toRadians(parameterStore.get("YX", "Phi").getD());

    line=new ALineSegment(0, 0, 1500*Math.cos(phi), 1500*Math.sin(phi), 10);
    hr[0]=line;
  }

  public RectangularShape getScreenHotRegion(int region) {
    if(region==0)
      return new ALineSegment(window.calculateDisplay(line.p1), window.calculateDisplay(line.p2), 10);
    else
      return null;
  }

  public int init(Point2D.Double p, int key) {
    return 1;
  }

  public int getButton() {
    return AInteractionsManager.LEFT;
  }

  public void start(Point2D.Double p, int region, int key) {
    // if(parameterStore.get("YX","Clock").getStatus())
    if(region==0)
      parameterStore.get("YX", "Clock").setD(0.0000001);
    else if(region==1) {
      AProjection2D projection=(AProjection2D)window.getProjection();
      Point2D.Double p0=projection.inverseNonLinearTransform(p);

      this.phi=Math.atan2(p0.y, p0.x);
    }
  }

  public void drag(Point2D.Double p, int region, int key) {
    if(region==0) {
      double phi=Math.atan2(p.y, p.x);

      // phi+=Math.PI;
      if(phi<0) phi+=2*Math.PI;
      parameterStore.get("YX", "Phi").setD(Math.toDegrees(phi));
    } else if(region==1&&parameterStore.get("YX", "Clock").getStatus()) {
      double phiPrime=Math.atan2(p.y, p.x);
      double phi0=Math.toRadians(parameterStore.get("YX", "Phi").getD());
      double deltaPhiPrime=phiPrime-phi0;

      if(deltaPhiPrime<-Math.PI) deltaPhiPrime+=2*Math.PI;
      if(deltaPhiPrime>Math.PI) deltaPhiPrime-=2*Math.PI;
      double deltaPhi=this.phi-phi0;

      if(deltaPhi<-Math.PI) deltaPhi+=2*Math.PI;
      if(deltaPhi>Math.PI) deltaPhi-=2*Math.PI;

      if((deltaPhi>0.&&deltaPhiPrime-deltaPhi>0.)||(deltaPhi<0.&&deltaPhiPrime-deltaPhi<0.)) {
        double a=(deltaPhi*Math.PI-deltaPhiPrime*Math.abs(deltaPhi))/(deltaPhiPrime-deltaPhi);

        parameterStore.get("YX", "Clock").setD(Math.min(Math.max(Math.PI/a, 0.000001), 7.));
      } else {
        parameterStore.get("YX", "Clock").setD(0.000001);
      }
    }
  }

  public void stop() {}

  public void cancel() {}

  public void paint(Graphics2D g) {
    // double phi = parameterStore.get("YX","Phi").getD()*AMath.DEGREES_TO_RADIANS+Math.PI;
    double phi=Math.toRadians(parameterStore.get("YX", "Phi").getD());

    line.p2.setLocation(1500*Math.cos(phi), 1500*Math.sin(phi));
    ALineSegment screenLine=(ALineSegment)getScreenHotRegion(0);

    AGraphics ag=AGraphics.makeAGraphics(g);

    ag.updateDrawParameters(frameDrawParameters);
    ag.drawLine(screenLine.p1.x, screenLine.p1.y, screenLine.p2.x, screenLine.p2.y);
    ag.updateDrawParameters(drawParameters);
    ag.drawLine(screenLine.p1.x, screenLine.p1.y, screenLine.p2.x, screenLine.p2.y);
  }

}
