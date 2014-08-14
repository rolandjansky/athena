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
public class AYSliceSelection extends ASelection {

  public AYSliceSelection() {
    super(6);
  }

  public int init(Point2D.Double p, int key) {
    isValid=false;

    int xmax=window.getSize().width;
    int xmid=xmax/2;

    setCenter(hr[0], 0, p.y);
    setCenter(hr[1], 0, p.y);
    setCenter(hr[2], xmax, p.y);
    setCenter(hr[3], xmax, p.y);
    setCenter(hr[4], xmid, p.y);
    setCenter(hr[5], xmid, p.y);

    region=5;
    return region;
  }

  public void drag(Point2D.Double p, int region, int key) {
    isValid=true;

    switch(region) {
    case 0:
    case 3:
    case 4:
      setCenterY(hr[0], p.y);
      setCenterY(hr[3], p.y);
      setCenterY(hr[4], p.y);
      break;

    case 1:
    case 2:
    case 5:
      setCenterY(hr[1], p.y);
      setCenterY(hr[2], p.y);
      setCenterY(hr[5], p.y);
      break;
    }
  }

  public void paint(Graphics2D g) {
    paintStandard(g);
  }

  public Point2D.Double[] getCorners() {
    // Sort out which are the three control points.
    int first=1;
    int second=2;
    int third=3;

    if(hr[1].getCenterY()>hr[0].getCenterY()) {
      first=0;
      second=3;
      third=2;
    }

    return convert(first, second, third);
  }

}
