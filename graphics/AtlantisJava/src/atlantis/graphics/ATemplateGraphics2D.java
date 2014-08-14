package atlantis.graphics;

import java.awt.*;

/**
 * A very limited template graphics which implements the default behaviour
 * of the clip region.
 */

public abstract class ATemplateGraphics2D extends ADummyGraphics2D {

  private Rectangle currentClip;
  private Rectangle imageBounds;

  public ATemplateGraphics2D(Rectangle bounds) {
    currentClip=new Rectangle(bounds);
    imageBounds=new Rectangle(bounds);
  }

  protected ATemplateGraphics2D(ATemplateGraphics2D parent) {
    currentClip=new Rectangle(parent.currentClip);
    imageBounds=new Rectangle(parent.imageBounds);
  }

  public abstract Graphics create();

  public Rectangle getClipBounds() {
    return new Rectangle(currentClip);
  }

  public void setClip(int x, int y, int width, int height) {
    currentClip=new Rectangle(x, y, width, height);
  }

  public void clipRect(int x, int y, int width, int height) {
    currentClip=currentClip.intersection(new Rectangle(x, y, width, height));
  }

  public void translate(int x, int y) {
    translate((double)x, (double)y);
  }

  public void translate(double x, double y) {
    currentClip.setLocation((int)Math.round(currentClip.x-x), (int)Math.round(currentClip.y-y));
  }

}
