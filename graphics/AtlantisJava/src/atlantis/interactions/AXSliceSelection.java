package atlantis.interactions;


import java.awt.*;
import java.awt.geom.*;


/**
 * A panel which selects a slice of the window in the x-direction.
 * That is, it selects a rectangular region with a height equal to the
 * full height of the window and with an adjustable width.
 *
 * @author Charles Loomis
 **/
public class AXSliceSelection extends ASelection {

  public AXSliceSelection() {
    super(6);
  }

  public int init(Point2D.Double p, int key) {
    isValid=false;

    int ymax=window.getSize().height;
    int ymid=ymax/2;

    setCenter(hr[0], p.x, 0);
    setCenter(hr[1], p.x, 0);
    setCenter(hr[2], p.x, ymax);
    setCenter(hr[3], p.x, ymax);
    setCenter(hr[4], p.x, ymid);
    setCenter(hr[5], p.x, ymid);

    region=5;
    return region;
  }

  public void drag(Point2D.Double p, int region, int key) {
    isValid=true;

    switch(region) {
    case 0:
    case 3:
    case 4:
      setCenterX(hr[0], p.x);
      setCenterX(hr[3], p.x);
      setCenterX(hr[4], p.x);
      break;

    case 1:
    case 2:
    case 5:
      setCenterX(hr[1], p.x);
      setCenterX(hr[2], p.x);
      setCenterX(hr[5], p.x);
      break;
    }
  }

  public void paint(Graphics2D g) {
    paintStandard(g);
  }

  public Point2D.Double[] getCorners() {
    // Sort out which are the three control points.
    int first=0;
    int second=1;
    int third=2;

    if(hr[1].getCenterX()<hr[0].getCenterX()) {
      first=1;
      second=0;
      third=3;
    }

    return convert(first, second, third);
  }

}
