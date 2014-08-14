package atlantis.interactions;


import java.awt.*;
import java.awt.geom.*;


public class ARotatedRectangleSelection extends ASelection {

  // The initial starting width of the first and last sides.
  private final static int STARTING_WIDTH=25;

  public ARotatedRectangleSelection() {
    super(6);
  }

  // Initialize the control points given the starting point (x,y).
  public int init(Point2D.Double p, int key) {
    isValid=false;

    setCenter(hr[0], p.x-STARTING_WIDTH, p.y);
    setCenter(hr[1], p.x-STARTING_WIDTH, p.y);
    setCenter(hr[2], p.x+STARTING_WIDTH, p.y);
    setCenter(hr[3], p.x+STARTING_WIDTH, p.y);
    setCenter(hr[4], p.x, p.y);
    setCenter(hr[5], p.x, p.y);

    region=5;
    return region;
  }

  public void drag(Point2D.Double p, int region, int key) {
    isValid=true;
    double dx, dy, deltax, deltay, angle, radius;

    switch(region) {
    case 0:
      radius=getRadius(p.x, p.y, 4);
      angle=getAngle();

      deltax=p.x-hr[4].getCenterX();
      deltay=p.y-hr[4].getCenterY();

      dx=-Math.round(radius*Math.sin(angle));
      dy=Math.round(radius*Math.cos(angle));

      if(deltax*dx+deltay*dy<0) {
        dx=-dx;
        dy=-dy;
      }

      setCenter(hr[0], hr[4].getCenterX()+dx, hr[4].getCenterY()+dy);
      setCenter(hr[3], hr[4].getCenterX()-dx, hr[4].getCenterY()-dy);
      setCenter(hr[1], hr[5].getCenterX()+dx, hr[5].getCenterY()+dy);
      setCenter(hr[2], hr[5].getCenterX()-dx, hr[5].getCenterY()-dy);
      break;

    case 1:
      radius=getRadius(p.x, p.y, 5);
      angle=getAngle();

      deltax=p.x-hr[5].getCenterX();
      deltay=p.y-hr[5].getCenterY();

      dx=-Math.round(radius*Math.sin(angle));
      dy=Math.round(radius*Math.cos(angle));

      if(deltax*dx+deltay*dy<0) {
        dx=-dx;
        dy=-dy;
      }

      setCenter(hr[0], hr[4].getCenterX()+dx, hr[4].getCenterY()+dy);
      setCenter(hr[3], hr[4].getCenterX()-dx, hr[4].getCenterY()-dy);
      setCenter(hr[1], hr[5].getCenterX()+dx, hr[5].getCenterY()+dy);
      setCenter(hr[2], hr[5].getCenterX()-dx, hr[5].getCenterY()-dy);
      break;

    case 2:
      radius=getRadius(p.x, p.y, 5);
      angle=getAngle();

      deltax=p.x-hr[5].getCenterX();
      deltay=p.y-hr[5].getCenterX();

      dx=-Math.round(radius*Math.sin(angle));
      dy=Math.round(radius*Math.cos(angle));

      if(deltax*dx+deltay*dy<0) {
        dx=-dx;
        dy=-dy;
      }

      setCenter(hr[0], hr[4].getCenterX()-dx, hr[4].getCenterY()-dy);
      setCenter(hr[3], hr[4].getCenterX()+dx, hr[4].getCenterY()+dy);
      setCenter(hr[1], hr[5].getCenterX()-dx, hr[5].getCenterY()-dy);
      setCenter(hr[2], hr[5].getCenterX()+dx, hr[5].getCenterY()+dy);
      break;

    case 3:
      radius=getRadius(p.x, p.y, 4);
      angle=getAngle();

      deltax=p.x-hr[4].getCenterX();
      deltay=p.y-hr[4].getCenterY();

      dx=-Math.round(radius*Math.sin(angle));
      dy=Math.round(radius*Math.cos(angle));

      if(deltax*dx+deltay*dy<0) {
        dx=-dx;
        dy=-dy;
      }

      setCenter(hr[0], hr[4].getCenterX()-dx, hr[4].getCenterY()-dy);
      setCenter(hr[3], hr[4].getCenterX()+dx, hr[4].getCenterY()+dy);
      setCenter(hr[1], hr[5].getCenterX()-dx, hr[5].getCenterY()-dy);
      setCenter(hr[2], hr[5].getCenterX()+dx, hr[5].getCenterY()+dy);
      break;

    case 4:
    case 5:
      radius=getRadius(hr[0].getCenterX(), hr[0].getCenterY(), 4);

      // Update the active control point.
      setCenter(hr[region], p.x, p.y);
      angle=getAngle();

      dx=-Math.round(radius*Math.sin(angle));
      dy=Math.round(radius*Math.cos(angle));

      setCenter(hr[0], hr[4].getCenterX()+dx, hr[4].getCenterY()+dy);
      setCenter(hr[3], hr[4].getCenterX()-dx, hr[4].getCenterY()-dy);
      setCenter(hr[1], hr[5].getCenterX()+dx, hr[5].getCenterY()+dy);
      setCenter(hr[2], hr[5].getCenterX()-dx, hr[5].getCenterY()-dy);
      break;
    }
  }

  private double getAngle() {
    double deltax=hr[5].getCenterX()-hr[4].getCenterX();
    double deltay=hr[5].getCenterY()-hr[4].getCenterY();

    if(deltax!=0||deltay!=0)
      return Math.atan2(deltay, deltax);
    else
      return 0.;
  }

  public void paint(Graphics2D g) {
    paintStandard(g);
  }

  /**
   * Make the affine transform which corresponds to this rectangular selection.
   */
  public Point2D.Double[] getCorners() {
    int first=getUpperLeftRegion();

    // Calculate the index of the opposite corner.
    int third=(first+2)%4;

    // Now use the cross-product to determine which of the
    // remaining points is the one which keep the path going
    // clockwise.
    int second=(first+1)%4;
    double dx0=hr[third].getCenterX()-hr[first].getCenterX();
    double dy0=hr[third].getCenterY()-hr[first].getCenterY();
    double dx1=hr[second].getCenterX()-hr[first].getCenterX();
    double dy1=hr[second].getCenterY()-hr[first].getCenterY();

    if(dx0*dy1-dy0*dx1>0)
      second=(first+3)%4;
    // Get the appropriate radius.
    int centerIndex=(first==0||first==3)?4:5;
    double radius=getRadius(hr[first].getCenterX(), hr[first].getCenterY(), centerIndex);
    double angle=getAngle();
    // Calculate the delta-x and delta-y for the points.
    double dx=Math.abs(radius*Math.sin(angle));
    double dy=Math.abs(radius*Math.cos(angle));
    // Get the sign of the offsets from the control points.
    double sdx;
    double sdy;

    // The point closest to the origin.
    centerIndex=(first==0||first==3)?4:5;
    sdx=((hr[first].getCenterX()-hr[centerIndex].getCenterX())>0)?1.:-1.;
    sdy=((hr[first].getCenterY()-hr[centerIndex].getCenterY())>0)?1.:-1.;
    double x0=hr[centerIndex].getCenterX()+sdx*dx;
    double y0=hr[centerIndex].getCenterY()+sdy*dy;

    // The next point clockwise.
    centerIndex=(second==0||second==3)?4:5;
    sdx=((hr[second].getCenterX()-hr[centerIndex].getCenterX())>0)?1.:-1.;
    sdy=((hr[second].getCenterY()-hr[centerIndex].getCenterY())>0)?1.:-1.;
    double x1=hr[centerIndex].getCenterX()+sdx*dx;
    double y1=hr[centerIndex].getCenterY()+sdy*dy;

    // The next point clockwise.
    centerIndex=(third==0||third==3)?4:5;
    sdx=((hr[third].getCenterX()-hr[centerIndex].getCenterX())>0)?1.:-1.;
    sdy=((hr[third].getCenterY()-hr[centerIndex].getCenterY())>0)?1.:-1.;
    double x2=hr[centerIndex].getCenterX()+sdx*dx;
    double y2=hr[centerIndex].getCenterY()+sdy*dy;

    // The control points are in the correct order, so we can just
    // call the utility function of the parent.
    return convert(x0, y0, x1, y1, x2, y2);
  }

}
