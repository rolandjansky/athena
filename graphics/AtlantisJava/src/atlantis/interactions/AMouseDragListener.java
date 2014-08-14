package atlantis.interactions;


import java.awt.geom.*;


/**
 * Implemented by the interactions to get drag events
 * from the InteractionsManager.
 */
public interface AMouseDragListener {

  // Should return one of the LEFT, MIDDLE or RIGHT constants from AWindow
  int getButton();

  void start(Point2D.Double p, int region, int key);
  void drag(Point2D.Double p, int region, int key);
  void stop();
  void cancel();
}
