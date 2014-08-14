package atlantis.interactions;


import java.awt.geom.*;


/**
 * This listener extends AMouseDragListener in order to make it possible to
 * get drag events even without any hot region.
 */
public interface ASleepMouseDragListener extends AMouseDragListener {
  int init(Point2D.Double p, int key);
}
