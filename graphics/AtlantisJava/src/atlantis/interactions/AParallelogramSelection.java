package atlantis.interactions;


import java.awt.*;
import java.awt.geom.*;


public class AParallelogramSelection extends ASelection {
  final private static int STARTING_WIDTH=25;

  public AParallelogramSelection() {
    super(6);
  }

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

    double dx, dy;

    switch(region) {
    case 4:
      dx=hr[0].getCenterX()-hr[4].getCenterX();
      dy=hr[0].getCenterY()-hr[4].getCenterY();

      setCenter(hr[region], p.x, p.y);
      setCenter(hr[0], p.x+dx, p.y+dy);
      setCenter(hr[3], p.x-dx, p.y-dy);
      break;

    case 5:
      dx=hr[1].getCenterX()-hr[5].getCenterX();
      dy=hr[1].getCenterY()-hr[5].getCenterY();

      setCenter(hr[region], p.x, p.y);
      setCenter(hr[1], p.x+dx, p.y+dy);
      setCenter(hr[2], p.x-dx, p.y-dy);
      break;

    case 0:
    case 3:
      setCenter(hr[region], p.x, p.y);

      dx=hr[region].getCenterX()-hr[4].getCenterX();
      dy=hr[region].getCenterY()-hr[4].getCenterY();

      if(region==3) {
        dx=-dx;
        dy=-dy;
      }

      setCenter(hr[1], hr[5].getCenterX()+dx, hr[5].getCenterY()+dy);
      setCenter(hr[2], hr[5].getCenterX()-dx, hr[5].getCenterY()-dy);
      setCenter(hr[0], hr[4].getCenterX()+dx, hr[4].getCenterY()+dy);
      setCenter(hr[3], hr[4].getCenterX()-dx, hr[4].getCenterY()-dy);
      break;

    case 1:
    case 2:
      setCenter(hr[region], p.x, p.y);

      dx=hr[region].getCenterX()-hr[5].getCenterX();
      dy=hr[region].getCenterY()-hr[5].getCenterY();

      if(region==2) {
        dx=-dx;
        dy=-dy;
      }

      setCenter(hr[1], hr[5].getCenterX()+dx, hr[5].getCenterY()+dy);
      setCenter(hr[2], hr[5].getCenterX()-dx, hr[5].getCenterY()-dy);
      setCenter(hr[0], hr[4].getCenterX()+dx, hr[4].getCenterY()+dy);
      setCenter(hr[3], hr[4].getCenterX()-dx, hr[4].getCenterY()-dy);
      break;
    }
  }

  public void paint(Graphics2D g) {
    paintStandard(g);
  }

  /**
   * Make the affine transform which corresponds to this
   * paralleogram-shaped selection.
   */
  public Point2D.Double[] getCorners() {
    int first=getUpperLeftRegion();

    // Calculate the opposite index.
    int third=(first+2)%4;

    // Now use the cross-product to determine which of the
    // remaining points is the one which keep the path going
    // clockwise.
    int second=(first+1)%4;
    double dx0=hr[third].getCenterX()-hr[first].getCenterX();
    double dy0=hr[third].getCenterY()-hr[first].getCenterY();
    double dx1=hr[second].getCenterX()-hr[first].getCenterX();
    double dy1=hr[second].getCenterY()-hr[first].getCenterY();

    if(dx0*dy1-dy0*dx1>0) second=(first+3)%4;

    return convert(first, second, third);
  }

}
